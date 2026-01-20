#include <windows.h>
#include <winternl.h>
#include <intrin.h>
#include <cstdarg>
#include <tlhelp32.h>
#include <psapi.h>
#include <vector>
#include <map>
#include <string>
#include <cwchar>

#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "psapi.lib")

#pragma	once

class CheatMemory {
public:
#if defined(_M_IX86) || defined( __i386) || defined(_M_ARM) || defined(__arm__)              // x86 平台
	#define AddressSize  4                   // 指针大小（4字节）	
	#define dllPathOffset 8                 // DLL路径 地址 偏移
	#define LoadLibraryOffset 13             // LoadLibraryW 地址 偏移
	#define originalUpOffset 27			     // 原始 指令地址 偏移
	typedef DWORD32 DWORD_PTR;               // 32位指针类型
#elif defined(_M_X64) || defined(__x86_64__) || defined(_M_ARM64) || defined(__aarch64__)    // x64 平台   如下不在赘述
	#define AddressSize 8
	#define dllPathOffset 16
	#define LoadLibraryOffset 26
	#define originalUpOffset 52
	typedef DWORD64 DWORD_PTR;
#endif
	// 定义 地址偏移列表 类型 和 线程ID列表 类型
	typedef std::map<int, DWORD_PTR> OffsetList;
	typedef std::vector<DWORD> ThreadIDList;
private:
	BOOL IsWow64 = FALSE; // 目标进程是否为 32 位（在 WOW64 下运行）
	// DLL 注入 相关汇编代码 和 数据结构
	struct AsmCodeData {
		LPVOID pLoadLibrary;
		wchar_t DLLpath[MAX_PATH] = {0};
	};
	// 通用的 DLL 注入调用代码（x86/x64 通用模板）
	std::vector<BYTE> InjectDLLAsmCode = {
	#if defined(_M_IX86) || defined(__i386) || defined(_M_ARM) || defined(__arm__)               // x86 平台
		0x8B, 0x03,               // mov eax,[ebx]
		0x8D, 0x5B, 0x04,         // lea ebx,[ebx+0x4]
		0x83, 0xEC, 0x14,         // sub esp,0x14
		0x53,                     // push ebx
		0xFF, 0xD0,               // call eax
		0x83, 0xC4, 0x14,         // add esp,0x14
	#elif defined(_M_X64) || defined(__x86_64__) || defined(_M_ARM64) || defined(__aarch64__)    // x64 平台
		0x48, 0x8B, 0x01,         // mov rax,[rcx]
		0x48, 0x8D, 0x49, 0x08,   // lea rcx,[rcx+0x8]
		0x48, 0x83, 0xEC, 0x28,   // sub rsp,0x28
		0xFF, 0xD0,               // call rax
		0x48, 0x83, 0xC4, 0x28,   // add rsp,0x28
	#endif
		0xC3                      // ret
	};
	// 通用的 DLL 注入劫持代码（x86/x64 通用模板）
	std::vector<BYTE> HijackAsmCode = {
	#if   defined(_M_IX86) || defined( __i386) || defined(_M_ARM) || defined(__arm__)           // x86 平台
		0x83, 0xEC, 0x14,												 // sub esp, 0x14							
		0x89, 0x44, 0x24, 0x0C,											 // mov [esp+0xC], eax
		0x68, 0x00, 0x00, 0x00, 0x00,									 // push [dllPath] (占位)
		0xB8, 0x00, 0x00, 0x00, 0x00,                                    // mov eax, [LoadLiraryW] (占位)
		0xFF, 0xD0,														 // call eax
		0x8B, 0x44, 0x24, 0x0C,											 // mov eax, [esp+0xC]
		0x83, 0xC4, 0x14,												 // add esp, 0x14
		0xBB, 0x00, 0x00, 0x00, 0x00,									 // mov ebx, [originalUp] (占位)
		0xFF, 0xE3														 // jmp ebx
	#elif defined(_M_X64) || defined(__x86_64__) || defined(_M_ARM64) || defined(__aarch64__)  // x64 平台                  
		0x48, 0x83, 0xEC, 0x28,                                           // sub rsp, 0x28
		0x48, 0x89, 0x4C, 0x24, 0x10,                                     // mov [rsp+0x10], rcx
		0x48, 0x89, 0x44, 0x24, 0x18,									  // mov [rsp+0x18], rax
		0x48, 0xB9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,       // mov rcx, [dllPath] (占位)
		0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,       // mov rax, [LoadLiraryW] (占位)
		0xFF, 0xD0,                                                       // call rax
		0x48, 0x8B, 0x4C, 0x24, 0x10,                                     // mov rcx, [rsp+0x10]
		0x48, 0x8B, 0x44, 0x24, 0x18,                                     // mov rax, [rsp+0x18]
		0x48, 0x83, 0xC4, 0x28,                                           // add rsp, 0x28
		0x49, 0xBB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,       // mov r11, [originalUp] (占位)
		0x41, 0xFF, 0xE3												  // jmp r11
	#endif
	};
	// Windows 内部 PEB_LDR_DATA_TABLE_ENTRY 结构体（简化版）
	typedef struct _PEB_LDR_DATA_TABLE_ENTRY {
		LIST_ENTRY InLoadOrderLinks;				    // 0x00 加载顺序模块列表
		LIST_ENTRY InMemoryOrderLinks;				    // 0x10 内存顺序模块列表
		LIST_ENTRY InInitializationOrderLinks;			// 0x20 初始化顺序模块列表
		PVOID DLLBaseAddress;                           // 0x30 DLL 基址
		PVOID EntryPoint;                               // 0x38 DLL 入口点
		ULONG SizeOfImage;                              // 0x40 DLL 映像大小
		UNICODE_STRING FullDllName;                     // 0x48 DLL 完整路径
		UNICODE_STRING BaseDllName;                     // 0x58 DLL 基础名称
	} PEB_LDR_DATA_TABLE_ENTRY, * PPEB_LDR_DATA_TABLE_ENTRY;
	// Windows 内部 PEB_LDR_DATA 结构体（简化版）
	typedef struct _PEB_LDR_DATA {
		ULONG Length;                                  // 0x00
		UCHAR Initialized;                             // 0x04
		PVOID SsHandle;                                // 0x08
		LIST_ENTRY InLoadOrderModuleList;              // 0x10 加载顺序模块列表头
		LIST_ENTRY InMemoryOrderModuleList;            // 0x20 内存顺序模块列表头
		LIST_ENTRY InInitializationOrderModuleList;    // 0x30 初始化顺序模块列表头
	} PEB_LDR_DATA, * PPEB_LDR_DATA;
	// Windows 内部 PEB 结构体（简化版）
	typedef struct _PEB {
		UCHAR InheritedAddressSpace;                   // 0x00
		UCHAR ReadImageFileExecOptions;                // 0x01
		UCHAR BeingDebugged;                           // 0x02
		UCHAR BitField;                                // 0x03
		UCHAR Padding0[4];                             // 0x04
		PVOID Mutant;                                  // 0x08
		PVOID ImageBaseAddress;                        // 0x10
		PPEB_LDR_DATA Ldr;                             // 0x18 指向 PEB_LDR_DATA 结构体
	} PEB, * PPEB;
	// Windows 内部 PROCESS_BASIC_INFORMATION 结构体定义
	typedef struct _PROCESS_BASIC_INFORMATION {
		NTSTATUS ExitStatus;
		PVOID PebBaseAddress;
		ULONG_PTR AffinityMask;
		DWORD BasePriority;
		HANDLE UniqueProcessId;
		HANDLE InheritedFromUniqueProcessId;
	} PROCESS_BASIC_INFORMATION;
	// Windows 内部 NtQueryInformationProcess 函数类型定义
	typedef NTSTATUS(NTAPI *NtQueryInformationProcess)(
		HANDLE  ProcessHandle,
		DWORD   ProcessInformationClass,
		PVOID   ProcessInformation,
		ULONG   ProcessInformationLength,
		PULONG  ReturnLength
	);
	/*
	// Windows 内部 NtQueryInformationThread 函数类型定义
	typedef NTSTATUS (NTAPI *NtQueryInformationThread)(
		HANDLE          ThreadHandle,
		THREADINFOCLASS ThreadInformationClass,
		PVOID           ThreadInformation,
		ULONG           ThreadInformationLength,
		PULONG          ReturnLength
	);
	*/
	HANDLE APIGF(DWORD pid, DWORD_PTR& OffsetAddress, LPVOID BaseAddress, OffsetList OffsetArr);
	//void PGF(DWORD_PTR& OffsetAddress, DWORD_PTR BaseAddress, OffsetList OffsetArr);
	void IsProcess32BitOnWow64(DWORD pid);  // 判断目标进程是否为32位（在64位系统上运行的WOW64进程）
public:
	PPEB GetProcessPEB(DWORD pid, HANDLE &out_hProcess);
	LPVOID ReadPEFile(const wchar_t* DLLpath, DWORD* pFileSize);
	LPVOID BulidMemoryImage(LPVOID pPEFileBaseAddr, DWORD* pImageSize);
	// API读写  成功返回 true , 否则返回 false	使用API读写 内存地址 
	template <typename T>
	bool APIread(DWORD pid, T& rValue, LPVOID BaseAddress, OffsetList OffsetArr);
	template <typename T>
	bool APIwrite(DWORD pid, const T& wValue, LPVOID BaseAddress, OffsetList OffsetArr);
	bool APIreadString(DWORD pid, std::string& rValue, size_t rSize, LPVOID BaseAddress, OffsetList OffsetArr);
	bool APIreadWString(DWORD pid, std::wstring& rValue, size_t rSize, LPVOID BaseAddress, OffsetList OffsetArr);
	bool APIwriteString(DWORD pid, std::string& wValue, size_t wSize, LPVOID BaseAddress, OffsetList OffsetArr);
	bool APIwriteWString(DWORD pid, std::wstring& wValue, size_t wSize, LPVOID BaseAddress, OffsetList OffsetArr);
	/*
	template <typename T>
	void Pread(T& rValue, DWORD BaseAddress, OffsetList OffsetArr);
	template <typename T>
	void Pwrite(T& wValue, DWORD BaseAddress, OffsetList OffsetArr);
	*/

	// 更新汇编代码  成功返回 true , 否则返回 false	使用API写入 汇编操作码 到 汇编操作地址
	bool UpdateAsmCode(DWORD pid, LPVOID AsmAddress, std::vector<BYTE> AsmCode);
	bool InjectAsmCode(DWORD pid, LPVOID AsmAddress, std::vector<BYTE> AsmJump, std::vector<BYTE> AsmCode, LPVOID* AllocedAddress);
	bool InjectAsmCall(DWORD pid, std::vector<BYTE> AsmCode);
	// DLL 注入  成功返回 true , 否则返回 false
	bool InjectDLL(DWORD pid, const wchar_t* DLLpath);
	bool InjectDLLWithAsm(DWORD pid, const wchar_t* DLLpath);
	bool InjectDLLWithAPC(DWORD pid, const wchar_t* DLLpath);
	bool InjectDLLWithHijack(DWORD pid, const wchar_t* DLLpath);
	bool InjectDLLWithPE();

	bool UnlinkModuleFromLDR(DWORD pid, const wchar_t* DLLpath);  // 隐藏指定DLL模块 

	// 获取进程ID 
	DWORD HWND_to_PID(HWND Handle);
	DWORD GetProcessIDbyTitle(LPCWSTR WindowTitle);
	DWORD GetProcessIDbyName(const wchar_t* ProcessName);

	uintptr_t GetModuleBaseAddress(DWORD pid, const wchar_t* moduleName);  // 获取进程模块基址
	ThreadIDList GetThreadIDByPID(DWORD pid);   // 获取指定进程的所有线程ID
	void AntiScreenCapture(HWND Handle, bool _switch);  // 防止屏幕捕获  开启或关闭

	

};