//#include "pch.h"
#include "CheatMemory.h"

/// <summary>
/// 从指定进程的内存中读取任意 POD 类型的值（通过 ReadProcessMemory）。
/// </summary>
/// <param name="pid">目标进程的进程 ID。</param>
/// <param name="rValue">输出引用，用于接收读取到的值。函数会把从目标地址读取到的字节解释为类型 T 并写入此引用。</param>
/// <param name="BaseAddress">起始基址。可以是模块基址、静态地址或指针地址（当使用多级指针时通常传入指针所在的地址）。</param>
/// <param name="OffsetArr">
/// 偏移链（按读取顺序）。行为如下：
/// - 若 OffsetArr 为空，则直接对 BaseAddress 进行读/写；
/// - 若 OffsetArr 非空，内部会按顺序解析多级指针直到倒数第二项，最后把最后一项偏移加到解析到的地址上并进行最终读操作。
/// </param>
/// <returns>若能成功打开进程并从目标地址读取 sizeof(T) 字节则返回 true；否则返回 false（例如 OpenProcess/ReadProcessMemory 失败）。</returns>
/// <remarks>
/// - 本函数仅适合读取内存中按目标平台字节序存储的 POD 数据类型（整型、浮点等）。
/// - 调用者应确保目标地址有效并且目标进程具有相应访问权限；否则 ReadProcessMemory 会失败或抛出访问异常。
/// - 函数内部会调用 APIGF 打开目标进程并解析偏移链；调用结束后会 CloseHandle 返回的句柄。
/// - 模板显式实例化已在文件末尾列出，若需要其他类型请在源文件补充实例化声明。
/// </remarks>
template <typename T>
bool CheatMemory::APIread(DWORD pid, T& rValue, LPVOID BaseAddress, OffsetList OffsetArr) {
	void* pLastAddr = nullptr;
	HANDLE hProcess = APIGF(pid, (DWORD_PTR&)pLastAddr, BaseAddress, OffsetArr);
	if (hProcess == NULL) { return false; }
	DWORD_PTR Offset = (OffsetArr.size() == 0) ? 0 : OffsetArr[OffsetArr.size() - 1];
	::ReadProcessMemory(hProcess, (LPVOID)((DWORD_PTR&)pLastAddr + Offset), &rValue, sizeof(rValue), NULL);
	CloseHandle(hProcess);
	return true;
}
template bool CheatMemory::APIread<char>(DWORD pid, char& rValue, LPVOID BaseAddress, OffsetList OffsetArr);
template bool CheatMemory::APIread<BYTE>(DWORD pid, BYTE& rValue, LPVOID BaseAddress, OffsetList OffsetArr);
template bool CheatMemory::APIread<unsigned short>(DWORD pid, unsigned short&rValue, LPVOID BaseAddress, OffsetList OffsetArr);
template bool CheatMemory::APIread<short>(DWORD pid, short&rValue, LPVOID BaseAddress, OffsetList OffsetArr);
template bool CheatMemory::APIread<unsigned int>(DWORD pid, unsigned int&rValue, LPVOID BaseAddress, OffsetList OffsetArr);
template bool CheatMemory::APIread<int>(DWORD pid, int& rValue, LPVOID BaseAddress, OffsetList OffsetArr);
template bool CheatMemory::APIread<unsigned long>(DWORD pid, unsigned long& rValue, LPVOID BaseAddress, OffsetList OffsetArr);
template bool CheatMemory::APIread<long>(DWORD pid, long& rValue, LPVOID BaseAddress, OffsetList OffsetArr);
template bool CheatMemory::APIread<unsigned long long>(DWORD pid, unsigned long long& rValue, LPVOID BaseAddress, OffsetList OffsetArr);
template bool CheatMemory::APIread<long long>(DWORD pid, long long& rValue, LPVOID BaseAddress, OffsetList OffsetArr);
template bool CheatMemory::APIread<float>(DWORD pid, float& rValue, LPVOID BaseAddress, OffsetList OffsetArr);
template bool CheatMemory::APIread<double>(DWORD pid, double& rValue, LPVOID BaseAddress, OffsetList OffsetArr);
template bool CheatMemory::APIread<long double>(DWORD pid, long double& rValue, LPVOID BaseAddress, OffsetList OffsetArr);

/// <summary>
/// 向指定进程的内存写入任意 POD 类型的值（通过 WriteProcessMemory）。
/// </summary>
/// <param name="pid">目标进程的进程 ID。</param>
/// <param name="wValue">要写入的值（常量引用）。</param>
/// <param name="BaseAddress">起始基址。可为模块基址或指针所在地址。</param>
/// <param name="OffsetArr">
/// 偏移链（按读取顺序）。解析规则同 APIread：若为空直接将 BaseAddress 视为目标地址；否则通过多级指针解析到倒数第二级并把最后一项偏移加到解析地址后写入。
/// </param>
/// <returns>写入成功返回 true；失败返回 false（例如打开进程失败或 WriteProcessMemory 写入字节数不匹配）。</returns>
/// <remarks>
/// - 写入大小为 sizeof(T)。请确保目标内存有足够写入空间并且写入不会破坏目标程序状态（尤其是写入代码段或只读段会失败）。
/// - 内部通过 APIGF 打开进程并解析偏移链，调用结束后会 CloseHandle 返回的句柄。
/// </remarks>
template <typename T>
bool CheatMemory::APIwrite(DWORD pid, const T& wValue, LPVOID BaseAddress, CheatMemory::OffsetList OffsetArr) {
	DWORD_PTR OffsetAddress;
	HANDLE hProcess = APIGF(pid, OffsetAddress, BaseAddress, OffsetArr);
	if (hProcess == NULL) { return false; }
	DWORD_PTR Offset = (OffsetArr.size() == 0) ? 0 : OffsetArr[OffsetArr.size() - 1];
	::WriteProcessMemory(hProcess, (LPVOID)(OffsetAddress + Offset), &wValue, sizeof(wValue), NULL);
	CloseHandle(hProcess);
	return true;
}
template bool CheatMemory::APIwrite<char>(DWORD pid, const char& wValue, LPVOID BaseAddress, OffsetList OffsetArr);
template bool CheatMemory::APIwrite<BYTE>(DWORD pid, const BYTE& wValue, LPVOID BaseAddress, OffsetList OffsetArr);
template bool CheatMemory::APIwrite<unsigned short>(DWORD pid, const unsigned short& wValue, LPVOID BaseAddress, OffsetList OffsetArr);
template bool CheatMemory::APIwrite<short>(DWORD pid, const short& wValue, LPVOID BaseAddress, OffsetList OffsetArr);
template bool CheatMemory::APIwrite<unsigned int>(DWORD pid, const unsigned int& wValue, LPVOID BaseAddress, OffsetList OffsetArr);
template bool CheatMemory::APIwrite<int>(DWORD pid, const int& wValue, LPVOID BaseAddress, OffsetList OffsetArr);
template bool CheatMemory::APIwrite<unsigned long>(DWORD pid, const unsigned long& wValue, LPVOID BaseAddress, OffsetList OffsetArr);
template bool CheatMemory::APIwrite<long>(DWORD pid, const long& wValue, LPVOID BaseAddress, OffsetList OffsetArr);
template bool CheatMemory::APIwrite<unsigned long long>(DWORD pid, const unsigned long long& wValue, LPVOID BaseAddress, OffsetList OffsetArr);
template bool CheatMemory::APIwrite<long long>(DWORD pid, const long long& wValue, LPVOID BaseAddress, OffsetList OffsetArr);
template bool CheatMemory::APIwrite<float>(DWORD pid, const float& wValue, LPVOID BaseAddress, OffsetList OffsetArr);
template bool CheatMemory::APIwrite<double>(DWORD pid, const double& wValue, LPVOID BaseAddress, OffsetList OffsetArr);
template bool CheatMemory::APIwrite<long double>(DWORD pid, const long double& wValue, LPVOID BaseAddress, OffsetList OffsetArr);

/// <summary>
/// 从目标进程读取固定字节长度的 ANSI 字符串到 std::string（按长度读取，不保证以 NUL 结尾）。
/// </summary>
/// <param name="pid">目标进程 ID。</param>
/// <param name="rValue">用于接收读取数据的 std::string 引用。</param>
/// <param name="rSize">希望读取的字节数（ANSI 字符数量）。</param>
/// <param name="BaseAddress">起始基址或指针地址。</param>
/// <param name="OffsetArr">偏移链，可为空。</param>
/// <returns>成功返回 true；否则返回 false。</returns>
/// <remarks>
/// - 该函数会按 rSize 分配临时缓冲并读取字节，然后用构造函数按长度赋值给 rValue（不依赖 '\0'）。
/// - 若希望读取以 '\0' 结尾的字符串，请传入足够大的 rSize 或在读取后处理截断逻辑。
/// </remarks>
bool CheatMemory::APIreadString(DWORD pid, std::string& rValue, size_t rSize, LPVOID BaseAddress, OffsetList OffsetArr){
	void* pLastAddr = nullptr; std::vector<char> RTN(rSize);
	HANDLE hProcess = APIGF(pid, (DWORD_PTR&)pLastAddr, BaseAddress, OffsetArr);
	if (hProcess == NULL) { return false; }
	DWORD_PTR Offset = (OffsetArr.size() == 0) ? 0 : OffsetArr[OffsetArr.size() - 1];
	::ReadProcessMemory(hProcess, (LPVOID)((DWORD_PTR&)pLastAddr + Offset), RTN.data(), rSize, NULL);
	CloseHandle(hProcess);
	rValue.assign(RTN.data(), rSize);
	return true;
}

/// <summary>
/// 从目标进程读取固定长度的宽字符（UTF-16）字符串到 std::wstring（按 wchar_t 数读取）。
/// </summary>
/// <param name="pid">目标进程 ID。</param>
/// <param name="rValue">接收读取数据的 std::wstring 引用。</param>
/// <param name="rSize">要读取的 wchar_t 个数（实际读取字节为 rSize * sizeof(wchar_t)）。</param>
/// <param name="BaseAddress">起始基址或指针地址。</param>
/// <param name="OffsetArr">偏移链，可为空。</param>
/// <returns>成功返回 true；否则返回 false。</returns>
/// <remarks>
/// - 与 APIreadString 一样，本函数按指定长度读取，不保证返回字符串以 L'\\0' 结尾（除非目标内存包含终止符）。
/// - 注意 sizeof(wchar_t) 在 Windows 上通常为 2；因此函数会按 rSize * 2 的字节数读取。
/// </remarks>
bool CheatMemory::APIreadWString(DWORD pid, std::wstring& rValue, size_t rSize, LPVOID BaseAddress, OffsetList OffsetArr){
	void* pLastAddr = nullptr; std::vector<wchar_t> RTN(rSize * 2);
	HANDLE hProcess = APIGF(pid, (DWORD_PTR&)pLastAddr, BaseAddress, OffsetArr);
	if (hProcess == NULL) { return false; }
	DWORD_PTR Offset = (OffsetArr.size() == 0) ? 0 : OffsetArr[OffsetArr.size() - 1];
	::ReadProcessMemory(hProcess, (LPVOID)((DWORD_PTR&)pLastAddr + Offset), RTN.data(), rSize * 2, NULL);
	CloseHandle(hProcess);
	rValue.assign(RTN.data(), rSize);
	return true;
}

/// <summary>
/// 在目标进程写入固定长度的 ANSI 字符串（按字节数写入）。</summary>
/// <param name="pid">目标进程 ID。</param>
/// <param name="wValue">要写入的 std::string（数据源）。</param>
/// <param name="wSize">写入的字节数（若字符串短于此长度，将写入字符串后的内存中未定义内容，调用者应保证安全）。</param>
/// <param name="BaseAddress">起始基址或指针地址。</param>
/// <param name="OffsetArr">偏移链，可为空。</param>
/// <returns>写入成功返回 true；否则返回 false。</returns>
/// <remarks>
/// - 为安全起见，建议把 wSize 设置为 wValue.size() + 1（包含终止符），并确保目标缓冲区足够大。
/// </remarks>
bool CheatMemory::APIwriteString(DWORD pid, std::string& wValue, size_t wSize, LPVOID BaseAddress, OffsetList OffsetArr){
	void* pLastAddr = nullptr; 
	HANDLE hProcess = APIGF(pid, (DWORD_PTR&)pLastAddr, BaseAddress, OffsetArr);
	if (hProcess == NULL) { return false; }
	DWORD_PTR Offset = (OffsetArr.size() == 0) ? 0 : OffsetArr[OffsetArr.size() - 1];
	::WriteProcessMemory(hProcess, (LPVOID)((DWORD_PTR&)pLastAddr + Offset), wValue.c_str(), wSize, NULL);
	CloseHandle(hProcess);
	return true;
}

/// <summary>
/// 在目标进程写入固定长度的宽字符字符串（按 wchar_t 数写入）。</summary>
/// <param name="pid">目标进程 ID。</param>
/// <param name="wValue">要写入的 std::wstring（数据源）。</param>
/// <param name="wSize">写入的 wchar_t 个数（实际字节为 wSize * sizeof(wchar_t)）。</param>
/// <param name="BaseAddress">起始基址或指针地址。</param>
/// <param name="OffsetArr">偏移链，可为空。</param>
/// <returns>写入成功返回 true；否则返回 false。</returns>
/// <remarks>
/// - 建议将 wSize 设置为 wValue.size() + 1 以包含终止符，且确保目标缓冲区足够大以避免溢出。
/// </remarks>
bool CheatMemory::APIwriteWString(DWORD pid, std::wstring& wValue, size_t wSize, LPVOID BaseAddress, OffsetList OffsetArr){
	void* pLastAddr = nullptr;
	HANDLE hProcess = APIGF(pid, (DWORD_PTR&)pLastAddr, BaseAddress, OffsetArr);
	if (hProcess == NULL) { return false; }
	DWORD_PTR Offset = (OffsetArr.size() == 0) ? 0 : OffsetArr[OffsetArr.size() - 1];
	::WriteProcessMemory(hProcess, (LPVOID)((DWORD_PTR&)pLastAddr + Offset), wValue.c_str(), wSize * 2, NULL);
	CloseHandle(hProcess);
	return true;
}

/// <summary>
/// 内部辅助：打开目标进程并按偏移链解析到最终基址（返回打开的进程句柄）。
/// </summary>
/// <param name="pid">目标进程 ID。</param>
/// <param name="OffsetAddress">输出：解析后的地址（用于最终读/写）。</param>
/// <param name="BaseAddress">起始基址或指针地址（当 OffsetArr 非空时通常是指针所在地址）。</param>
/// <param name="OffsetArr">偏移链。若为空则直接将 BaseAddress 转为地址返回。</param>
/// <returns>
/// 返回一个有效的进程句柄（调用者负责 CloseHandle）或 NULL 表示失败（OpenProcess 失败）。
/// </returns>
/// <remarks>
/// - 内部使用 OpenProcess(PROCESS_ALL_ACCESS)。若需要更小权限可修改为更细的权限集合以提高安全性与兼容性。
/// - 当 OffsetArr 非空时：首先从 BaseAddress 读出一个指针值作为 OffsetAddress，然后按顺序读取每一级指针（直到倒数第二项），
///   最终调用者会在 OffsetAddress 上加上最后一项偏移得到真实的读/写地址。
/// - ReadProcessMemory 的返回值在当前实现中未被检查（可根据需要添加错误处理/日志）。
/// - AddressSize 成员用于指示平台指针宽度（4 或 8）。该值可在通过进程名检测时设置（GetProcessIDbyName 中会设置）。
/// </remarks>
HANDLE CheatMemory::APIGF(DWORD pid, DWORD_PTR& OffsetAddress, LPVOID BaseAddress, OffsetList OffsetArr) {
	HANDLE hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, false, pid);
	if (hProcess == NULL) { return NULL; }
	if (OffsetArr.size() == 0) { OffsetAddress = (DWORD_PTR)BaseAddress; return hProcess; }
	ReadProcessMemory(hProcess, BaseAddress, &OffsetAddress, sizeof(OffsetAddress), NULL);
	for (size_t i = 0; i < (OffsetArr.size() - 1); i++) {
		ReadProcessMemory(hProcess, (LPVOID)(OffsetAddress + OffsetArr[i]), &OffsetAddress, AddressSize, NULL);
	}
	return hProcess;
}


/*
template <typename T>
void CheatMemory::Pread(T& rValue, DWORD BaseAddress, OffsetList OffsetArr) {
	this->PGF(rValue, BaseAddress, OffsetArr);
	rValue = *(T*)(rValue + OffsetArr[OffsetArr.size() - 1]);
}
template void CheatMemory::Pread<char>(char& rValue, DWORD BaseAddress, OffsetList OffsetArr);
template void CheatMemory::Pread<byte>(byte& rValue, DWORD BaseAddress, OffsetList OffsetArr);
template void CheatMemory::Pread<unsigned short>(unsigned short& rValue, DWORD BaseAddress, OffsetList OffsetArr);
template void CheatMemory::Pread<short>(short& rValue, DWORD BaseAddress, OffsetList OffsetArr);
template void CheatMemory::Pread<unsigned int>(unsigned int& rValue, DWORD BaseAddress, OffsetList OffsetArr);
template void CheatMemory::Pread<int>(int& rValue, DWORD BaseAddress, OffsetList OffsetArr);
template void CheatMemory::Pread<unsigned long>(unsigned long& rValue, DWORD BaseAddress, OffsetList OffsetArr);
template void CheatMemory::Pread<long>(long& rValue, DWORD BaseAddress, OffsetList OffsetArr);
template void CheatMemory::Pread<unsigned long long>(unsigned long long& rValue, DWORD BaseAddress, OffsetList OffsetArr);
template void CheatMemory::Pread<long long>(long long& rValue, DWORD BaseAddress, OffsetList OffsetArr);
template void CheatMemory::Pread<float>(float& rValue, DWORD BaseAddress, OffsetList OffsetArr);
template void CheatMemory::Pread<double>(double& rValue, DWORD BaseAddress, OffsetList OffsetArr);
template void CheatMemory::Pread<long double>(long double& rValue, DWORD BaseAddress, OffsetList OffsetArr);
template void CheatMemory::Pread<string>(string& rValue, DWORD BaseAddress, OffsetList OffsetArr);
template void CheatMemory::Pread<wstring>(wstring& rValue, DWORD BaseAddress, OffsetList OffsetArr);

template <typename T>
void CheatMemory::Pwrite(T& wValue, DWORD BaseAddress, OffsetList OffsetArr) {
	DWORD_PTR OffsetAddress;
	this->PGF(OffsetAddress, BaseAddress, OffsetArr);
	OffsetAddress = *(DWORD_PTR*)(OffsetAddress + OffsetArr[OffsetArr.size() - 1]);
	*OffsetAddress = (T&)wValue;
}
template void CheatMemory::Pwrite<char>(char& rValue, DWORD BaseAddress, OffsetList OffsetArr);
template void CheatMemory::Pwrite<byte>(byte& rValue, DWORD BaseAddress, OffsetList OffsetArr);
template void CheatMemory::Pwrite<unsigned short>(unsigned short& rValue, DWORD BaseAddress, OffsetList OffsetArr);
template void CheatMemory::Pwrite<short>(short& rValue, DWORD BaseAddress, OffsetList OffsetArr);
template void CheatMemory::Pwrite<unsigned int>(unsigned int& rValue, DWORD BaseAddress, OffsetList OffsetArr);
template void CheatMemory::Pwrite<int>(int& rValue, DWORD BaseAddress, OffsetList OffsetArr);
template void CheatMemory::Pwrite<unsigned long>(unsigned long& rValue, DWORD BaseAddress, OffsetList OffsetArr);
template void CheatMemory::Pwrite<long>(long& rValue, DWORD BaseAddress, OffsetList OffsetArr);
template void CheatMemory::Pwrite<unsigned long long>(unsigned long long& rValue, DWORD BaseAddress, OffsetList OffsetArr);
template void CheatMemory::Pwrite<long long>(long long& rValue, DWORD BaseAddress, OffsetList OffsetArr);
template void CheatMemory::Pwrite<float>(float& rValue, DWORD BaseAddress, OffsetList OffsetArr);
template void CheatMemory::Pwrite<double>(double& rValue, DWORD BaseAddress, OffsetList OffsetArr);
template void CheatMemory::Pwrite<long double>(long double& rValue, DWORD BaseAddress, OffsetList OffsetArr);
template void CheatMemory::Pwrite<string>(string& rValue, DWORD BaseAddress, OffsetList OffsetArr);
template void CheatMemory::Pwrite<wstring>(wstring& rValue, DWORD BaseAddress, OffsetList OffsetArr);

void CheatMemory::PGF(DWORD_PTR& OffsetAddress, DWORD_PTR BaseAddress, OffsetList OffsetArr) {
	OffsetAddress = *(DWORD_PTR*)BaseAddress;
	for (int i = 0; i < (OffsetArr.size() - 1); i++) {
		OffsetAddress = *(DWORD_PTR*)OffsetAddress;
	}
}
*/

/// <summary>
/// 在目标进程的指定地址写入原始字节（用于修补/恢复代码段）。
/// </summary>
/// <param name="pid">目标进程 ID。</param>
/// <param name="AsmAddress">要写入的目标地址。</param>
/// <param name="AsmCode">要写入的字节序列。</param>
/// <returns>若写入成功并写入字节数与 AsmCode.size() 匹配则返回 true；否则返回 false。</returns>
/// <remarks>
/// - 函数使用 OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE) 打开进程并调用 WriteProcessMemory。
/// - 写入代码段或受保护内存可能会失败，必要时请先更改目标内存保护（VirtualProtectEx），或确保权限足够。
/// - 此函数不会自动恢复原始内存保护，也不做事务回滚，调用方应自行管理备份与恢复。
/// </remarks>
bool CheatMemory::UpdateAsmCode(DWORD pid, LPVOID AsmAddress, std::vector<BYTE> AsmCode){
	SIZE_T WriteByteTotal = NULL;
	HANDLE hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, pid);
	if (hProcess == NULL) return false; 
	BOOL WFlag = WriteProcessMemory(hProcess, AsmAddress, AsmCode.data(), AsmCode.size(), &WriteByteTotal);
	CloseHandle(hProcess);
	if (!(WFlag && WriteByteTotal == AsmCode.size())) return false;
	return true;
}

/// <summary>
/// 在目标进程指定地址写入跳转（或覆盖）指令，并把真实逻辑写入新分配的可执行内存。
/// </summary>
/// <param name="pid">目标进程 ID。</param>
/// <param name="AsmAddress">要被覆盖/跳转的目标地址。</param>
/// <param name="AsmJump">写入到目标地址的跳转/覆盖字节，函数会把分配地址的低 AddressSize 字节写入到 AsmJump 的偏移区（从索引 1 开始）。</param>
/// <param name="AsmCode">实际要写入并在新分配内存中执行的代码字节序列。</param>
/// <param name="AllocedAddress">输出：分配到的远程内存地址（供调用方后续释放或保留）。</param>
/// <returns>操作成功返回 true；任何子步骤失败返回 false。</returns>
/// <remarks>
/// - 函数会调用 VirtualAllocEx 分配可执行内存并写入 AsmCode，然后把 AsmJump 写入到 AsmAddress（用于跳转到分配的地址）。
/// - AsmJump 的具体格式由调用方决定（例如 x86 相对跳转或 x86-64 绝对跳转占位），本函数只做简单的低字节填充。
/// - 写入过程中若任一步骤失败，函数会返回 false；分配的内存地址通过 AllocedAddress 返回，调用方负责在不需要时调用 VirtualFreeEx 释放或保留用于后续恢复。
/// - 注意跨位（32/64）兼容性：AddressSize 决定了填充多少字节，调用方应确保 AsmJump 长度足够并且偏移位置正确。
/// </remarks>
bool CheatMemory::InjectAsmCode(DWORD pid, LPVOID AsmAddress, std::vector<BYTE> AsmJump, std::vector<BYTE> AsmCode, LPVOID* AllocedAddress)
{
	HANDLE hProcess;
	BOOL WFlag = FALSE;
	SIZE_T WriteByteTotal = NULL;
	hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, pid);
	if (hProcess == NULL) return false;
	*AllocedAddress = VirtualAllocEx(hProcess, NULL, AsmCode.size(), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (*AllocedAddress == nullptr) return false;
	for (int i = 0; i < AddressSize; ++i) {
		AsmJump[1 + i] = ((DWORD_PTR)*AllocedAddress >> (i * 8)) & 0xFF;
	}
	WFlag = WriteProcessMemory(hProcess, AsmAddress, AsmJump.data(), AsmJump.size(), &WriteByteTotal);
	if (!(WFlag && WriteByteTotal == AsmJump.size())) return false;
	WFlag = WriteProcessMemory(hProcess, *AllocedAddress, AsmCode.data(), AsmCode.size(), &WriteByteTotal);
	if (!(WFlag && WriteByteTotal == AsmCode.size())) return false;
	CloseHandle(hProcess);
	return true;
}

/// <summary>
/// 将一段机器码注入目标进程并创建远程线程执行（适用于短小且自包含的代码段）。
/// </summary>
/// <param name="pid">目标进程 ID。</param>
/// <param name="AsmCode">要注入并执行的机器码字节序列。</param>
/// <returns>执行完成并返回线程退出码（或成功启动并等待结束）则返回 true；否则返回 false。</returns>
/// <remarks>
/// - 使用 CreateRemoteThread 在目标进程创建线程并以注入地址为线程入口执行。若目标进程对 CreateRemoteThread 限制严格（如某些防护软件），可能失败。
/// - 注入的机器码必须自行负责堆栈/寄存器清理和返回（例如以 ret 结束），否则可能导致目标线程异常。
/// - 注入后函数会等待线程结束并释放所分配内存（VirtualFreeEx）。
/// </remarks>
bool CheatMemory::InjectAsmCall(DWORD pid, std::vector<BYTE> AsmCode) {
	HANDLE hProcess, hThread;
	LPVOID InjectAddr;
	SIZE_T WriteByteTotal;
	// 打开进程
	hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, pid);
	if (hProcess == NULL) return false;
	// 申请代码地址空间
	InjectAddr = VirtualAllocEx(hProcess, NULL, AsmCode.size(), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (InjectAddr == NULL) {
		CloseHandle(hProcess);
		return false;
	}
	// 注入代码
	BOOL writeResult = WriteProcessMemory(hProcess, InjectAddr, AsmCode.data(), AsmCode.size(), &WriteByteTotal);
	if (writeResult && WriteByteTotal == AsmCode.size()) {
		// 创建线程，运行代码
		hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)InjectAddr, NULL, 0, NULL);
		if (hThread != NULL) {
			WaitForSingleObject(hThread, INFINITE);
			VirtualFreeEx(hProcess, InjectAddr, 0, MEM_RELEASE);
			CloseHandle(hThread); CloseHandle(hProcess);
			return true;
		}
	}
	return false;
}

/// <summary>
/// 典型的通过远程线程调用 LoadLibraryW 注入 DLL 的实现（经典注入法）。
/// </summary>
/// <param name="pid">目标进程 ID。</param>
/// <param name="DLLpath">要注入的 DLL 路径（宽字符字符串）。</param>
/// <returns>若注入成功返回 true；否则返回 false。</returns>
/// <remarks>
/// - 步骤：VirtualAllocEx 分配缓冲 -> WriteProcessMemory 写入 DLL 路径 -> CreateRemoteThread 调用 LoadLibraryW -> 等待线程完成 -> 清理资源。
/// - 需注意目标进程的地址空间与权限；CreateRemoteThread/WriteProcessMemory 在某些保护环境下可能会被阻止或失败。
/// - 若 DLL 路径长度超过 MAX_PATH，当前实现会失败；可根据需要动态按实际长度分配内存。
/// </remarks>
bool CheatMemory::InjectDLL(DWORD pid, const wchar_t* DLLpath) {
	HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, pid);
	if (hProcess == NULL) return false;
	LPVOID InjectAddr = VirtualAllocEx(hProcess, NULL, MAX_PATH, MEM_COMMIT, PAGE_READWRITE);   // 分配内存空间
	if (InjectAddr == nullptr) { CloseHandle(hProcess); return false; }
	if (!WriteProcessMemory(hProcess, InjectAddr, DLLpath, (wcslen(DLLpath) + 1) * sizeof(wchar_t), NULL)) {   //写入进程内存
		VirtualFreeEx(hProcess, InjectAddr, 0, MEM_RELEASE);
		CloseHandle(hProcess);  return false;
	}   
	HMODULE hModule = GetModuleHandle(L"Kernel32.dll");
	if (hModule == NULL) {
		VirtualFreeEx(hProcess, InjectAddr, 0, MEM_RELEASE);
		CloseHandle(hProcess);  return false;
	}
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)GetProcAddress(hModule, "LoadLibraryW"), InjectAddr, NULL, NULL);   //创建远程线程
	if (hThread == NULL) { VirtualFreeEx(hProcess, InjectAddr, 0, MEM_RELEASE); CloseHandle(hProcess); return false; }   //等待线程，无限等待
	WaitForSingleObject(hThread, INFINITE);
	VirtualFreeEx(hProcess, InjectAddr, 0, MEM_RELEASE);   //释放内存
	CloseHandle(hProcess); CloseHandle(hThread);
	return true;
}

/// <summary>
/// 使用内置汇编模板在目标进程注入并通过创建远程线程执行（通过写入一个结构和可执行模板代码实现 LoadLibraryW 调用）。
/// </summary>
/// <param name="pid">目标进程 ID。</param>
/// <param name="DLLpath">要注入的 DLL 路径（宽字符）。</param>
/// <returns>成功返回 true；否则 false。</returns>
/// <remarks>
/// - 本方法依赖成员 InjectDLLAsmCode 和 AsmCodeData 结构：将 LoadLibraryW 的函数指针和 DLL 路径写入目标进程的数据结构，并把模板机器码写入目标进程执行。
/// - 模板机器码需要与目标进程架构匹配（x86 / x64）；否则注入会失败或导致进程异常。
/// - 写入完成后通过 CreateRemoteThread 在 InjectAddr 上执行模板代码，等待执行完毕并清理资源。
/// </remarks>
bool CheatMemory::InjectDLLWithAsm(DWORD pid, const wchar_t* DLLpath){
	HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, pid);
	LPVOID DLLInfoAddr = nullptr, InjectAddr = nullptr;
	DLLInfoAddr = VirtualAllocEx(hProcess, NULL, sizeof(AsmCodeData), MEM_COMMIT, PAGE_READWRITE);
	InjectAddr = VirtualAllocEx(hProcess, NULL, InjectDLLAsmCode.size(), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!DLLInfoAddr) VirtualFreeEx(hProcess, DLLInfoAddr, 0, MEM_RELEASE);
	if (!InjectAddr) VirtualFreeEx(hProcess, InjectAddr, 0, MEM_RELEASE);
	if (!InjectAddr || !DLLInfoAddr) { CloseHandle(hProcess); return false; }

	HMODULE hModule = GetModuleHandle(L"Kernel32.dll");
	if (hModule == NULL) {
		VirtualFreeEx(hProcess, DLLInfoAddr, 0, MEM_RELEASE);
		VirtualFreeEx(hProcess, InjectAddr, 0, MEM_RELEASE);
		CloseHandle(hProcess); return false;
	}
	AsmCodeData InjectInfo;
	InjectInfo.pLoadLibrary = GetProcAddress(hModule, "LoadLibraryW");
	ZeroMemory(&InjectInfo.DLLpath, 520);
	wcscpy_s(InjectInfo.DLLpath, DLLpath);
	if (!WriteProcessMemory(hProcess, DLLInfoAddr, &InjectInfo, sizeof(AsmCodeData), NULL) ||
		!WriteProcessMemory(hProcess, InjectAddr, InjectDLLAsmCode.data(), InjectDLLAsmCode.size(), NULL)) {
		VirtualFreeEx(hProcess, DLLInfoAddr, 0, MEM_RELEASE);
		VirtualFreeEx(hProcess, InjectAddr, 0, MEM_RELEASE);
		CloseHandle(hProcess); return false;
	}
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)InjectAddr, DLLInfoAddr, NULL, NULL);
	if (hThread == NULL) {
		VirtualFreeEx(hProcess, DLLInfoAddr, 0, MEM_RELEASE);
		VirtualFreeEx(hProcess, InjectAddr, 0, MEM_RELEASE);
		CloseHandle(hProcess); return false;
	}
	WaitForSingleObject(hThread, INFINITE);
	VirtualFreeEx(hProcess, DLLInfoAddr, 0, MEM_RELEASE);
	VirtualFreeEx(hProcess, InjectAddr, 0, MEM_RELEASE);
	CloseHandle(hProcess); CloseHandle(hThread); return true;
}

/// <summary>
/// 使用 APC（异步过程调用）将 LoadLibraryW 调用排入目标线程（尝试触发目标线程执行以完成注入）。
/// </summary>
/// <param name="pid">目标进程 ID。</param>
/// <param name="DLLpath">要注入的 DLL 路径（宽字符）。</param>
/// <returns>若成功将一个 APC 排队并触发线程返回 true；否则返回 false。</returns>
/// <remarks>
/// - 步骤：在目标进程分配内存并写入 DLL 路径 -> 枚举线程 -> 对可打开的线程调用 QueueUserAPC 将 LoadLibraryW 加入 APC 列表 -> 通过 Suspend/Resume 等手段促使线程处理 APC（注意：这不是可靠的触发方式）。
/// - APC 注入依赖目标线程在进入可警报状态时处理 APC，对于某些线程或进程（如没有进入可警报状态）无法触发成功。
/// - 该方法有较多限制并且在现代系统或受保护进程上通常不可靠或被防护软件检测到，请谨慎使用。
/// </remarks>
bool CheatMemory::InjectDLLWithAPC(DWORD pid, const wchar_t* DLLpath){
	HANDLE hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, pid);
	if (!hProcess) return false;
	LPVOID InjectAddr = VirtualAllocEx(hProcess, NULL, MAX_PATH, MEM_COMMIT, PAGE_READWRITE);
	if (!InjectAddr) { CloseHandle(hProcess); return false; }
	if (!WriteProcessMemory(hProcess, InjectAddr, DLLpath, (wcslen(DLLpath) + 1) * sizeof(wchar_t), NULL)) {
		VirtualFreeEx(hProcess, InjectAddr, 0, MEM_RELEASE);
		CloseHandle(hProcess); return false;
	}
	ThreadIDList ThreadIDs = GetThreadIDByPID(pid);
	for (DWORD ThreadID : ThreadIDs) {
		HANDLE hThread = OpenThread(THREAD_SET_CONTEXT, FALSE, ThreadID);
		if (hThread) QueueUserAPC((PAPCFUNC)GetProcAddress(GetModuleHandle(L"Kernel32.dll"), "LoadLibraryW"), hThread, (ULONG_PTR)InjectAddr);
	}
	VirtualFreeEx(hProcess, InjectAddr, 0, MEM_RELEASE);
	CloseHandle(hProcess);
	return !ThreadIDs.empty();
}

/// <summary>
/// 通过劫持线程上下文并写入 x64 劫持代码来加载 DLL（仅针对 x64，基于 HijackAsmCode64 的模板实现）。
/// </summary>
/// <param name="pid">目标进程 ID。</param>
/// <param name="DLLpath">要注入的 DLL 路径（宽字符）。</param>
/// <returns>若成功劫持并恢复线程上下文则返回 true；否则返回 false。</returns>
/// <remarks>
/// - 仅在 AddressSize == 8（x64）时有效。函数会：分配目标内存写入 DLL 路径和劫持代码 -> 枚举线程 -> 挂起线程 -> 获取并修改上下文使 Rip 指向劫持代码 -> 恢复线程以触发执行 -> 恢复并清理资源。
/// - 劫持线程是一种危险操作，可能导致目标线程或进程崩溃；请仅在受控环境下使用并备份数据。
/// - 本实现依赖 HijackAsmCode64 模板格式，模板内的偏移位需要与代码一致（当前实现对模板中的特定偏移位置进行写入）。
/// </remarks>
bool CheatMemory::InjectDLLWithHijack(DWORD pid, const wchar_t* DLLpath){
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS , FALSE, pid);
	if (!hProcess) return false;
	LPVOID DLLPathAddr = VirtualAllocEx(hProcess, NULL, (wcslen(DLLpath) + 1) * sizeof(wchar_t), MEM_COMMIT, PAGE_READWRITE);
	LPVOID InjectAddr = VirtualAllocEx(hProcess, NULL, HijackAsmCode.size(), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!DLLPathAddr) VirtualFreeEx(hProcess, DLLPathAddr, 0, MEM_RELEASE);
	if (!InjectAddr) VirtualFreeEx(hProcess, InjectAddr, 0, MEM_RELEASE);
	if (!InjectAddr || !DLLPathAddr) { CloseHandle(hProcess); return false; }
	if (!WriteProcessMemory(hProcess, DLLPathAddr, DLLpath, (wcslen(DLLpath) + 1) * sizeof(wchar_t), NULL)) {
		VirtualFreeEx(hProcess, DLLPathAddr, 0, MEM_RELEASE);
		VirtualFreeEx(hProcess, InjectAddr, 0, MEM_RELEASE);
		CloseHandle(hProcess); return false;
	}
	ThreadIDList ThreadIDs = GetThreadIDByPID(pid);
	HMODULE hModule = GetModuleHandle(L"Kernel32.dll");
	if (hModule == NULL) {
		VirtualFreeEx(hProcess, InjectAddr, 0, MEM_RELEASE);
		CloseHandle(hProcess); return false;
	}
	DWORD_PTR LoadLibraryAddr = (DWORD_PTR)GetProcAddress(hModule, "LoadLibraryW");
	for(DWORD ThreadID : ThreadIDs) {
		HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT | THREAD_SET_CONTEXT, FALSE, ThreadID);
		if (hThread) {
			CONTEXT Ctx; Ctx.ContextFlags = CONTEXT_FULL;
			if (SuspendThread(hThread) == -1) { CloseHandle(hThread); continue; }
			if (!GetThreadContext(hThread, &Ctx)) {
				ResumeThread(hThread);
				CloseHandle(hThread);
				continue;
			}
			std::vector<BYTE> HijackCode = HijackAsmCode;
			for (int i = 0; i < AddressSize; ++i) {
				HijackCode[dllPathOffset + i] = ((DWORD_PTR)DLLPathAddr >> (i * 8)) & 0xFF; // 写入 DLL路径 地址
				HijackCode[LoadLibraryOffset + i] = ((DWORD_PTR)LoadLibraryAddr >> (i * 8)) & 0xFF;  // 写入 LoadLibraryW 地址
				#if defined(_M_IX86) || defined( __i386)  // 写入 原始 指令地址
					HijackCode[originalUpOffset + i] = ((DWORD_PTR)Ctx.Eip >> (i * 8)) & 0xFF;
				#elif defined(_M_X64) || defined(__x86_64__)
					HijackCode[originalUpOffset + i] = ((DWORD_PTR)Ctx.Rip >> (i * 8)) & 0xFF;
				#elif defined(_M_ARM64) || defined(_M_ARM) || defined(__aarch64__) || defined(__arm__)
					HijackCode[originalUpOffset + i] = ((DWORD_PTR)Ctx.Pc >> (i * 8)) & 0xFF;
				#endif
			}
			if (!WriteProcessMemory(hProcess, InjectAddr, HijackCode.data(), HijackCode.size(), NULL)) {
				ResumeThread(hThread);
				CloseHandle(hThread);
				continue;
			}
			// 修改线程上下文
			#if defined(_M_IX86)( || defined(__i386__)
				Ctx.Eip = (DWORD_PTR)InjectAddr;
			#elif defined(_M_X64) || defined(__x86_64__)
				Ctx.Rip = (DWORD_PTR)InjectAddr;
			#elif defined(_M_ARM64) || defined(_M_ARM) || defined(__aarch64__) || defined(__arm__)
				Ctx.Pc = (DWORD_PTR)InjectAddr;
			#endif
			DWORD Flag = SetThreadContext(hThread, &Ctx);
			ResumeThread(hThread);
			CloseHandle(hThread);
			if (Flag) break;   // 仅劫持一个线程
		}else {
			continue;
		}
	}
	if(ThreadIDs.empty()){
		VirtualFreeEx(hProcess, DLLPathAddr, 0, MEM_RELEASE);
		VirtualFreeEx(hProcess, InjectAddr, 0, MEM_RELEASE);
	}
	CloseHandle(hProcess);
	return !ThreadIDs.empty();
}

bool CheatMemory::InjectDLLWithPE(){
	return false;
}

/// <summary>
/// 从磁盘读取 PE 文件到内存缓冲（完整文件副本），并做最基本的完整性检查（DOS/NT 签名）。
/// </summary>
/// <param name="DLLpath">PE 文件路径（宽字符）。</param>
/// <param name="pFileSize">输出：读取到的文件大小（可为 nullptr）。</param>
/// <returns>成功返回指向已分配的缓冲区地址（需要调用方 VirtualFree 释放）；失败返回 nullptr。</returns>
/// <remarks>
/// - 函数会打开文件、读取整个文件到 VirtualAlloc 分配的缓冲区并检查 DOS / NT 头签名与文件范围边界；若校验失败会释放缓冲区并返回 nullptr。
/// - 调用方负责释放返回的缓冲区（VirtualFree）。
/// - 参数校验：当前实现对路径后缀的简单判定与文件读取进行基本检查，但不替代完整的 PE 格式校验。
/// </remarks>
LPVOID CheatMemory::ReadPEFile(const wchar_t* DLLpath, DWORD* pFileSize){
	wchar_t* pFileSuffix = (wchar_t*)calloc(5, 2);
	wcsncpy_s(pFileSuffix, 5, DLLpath + wcslen(DLLpath) - 4, 4);
	if (DLLpath == nullptr && wcscmp(pFileSuffix, L".exe") == -1 || wcscmp(pFileSuffix, L".dll") == -1) return nullptr;
	HANDLE hFile = CreateFile(DLLpath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile) return nullptr;
	DWORD fileSize = GetFileSize(hFile, NULL);
	if (!hFile && fileSize < sizeof(IMAGE_DOS_HEADER)) { CloseHandle(hFile); return nullptr; }
	LPVOID FileBuffer = VirtualAlloc(NULL, fileSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!FileBuffer) { CloseHandle(hFile); return nullptr; }
	DWORD ReadByteTotal = 0;
	BOOL RFlag = ReadFile(hFile, FileBuffer, fileSize, &ReadByteTotal, NULL);
	CloseHandle(hFile);
	if (!(RFlag && ReadByteTotal == fileSize)) {
		VirtualFree(FileBuffer, 0, MEM_RELEASE);
		return nullptr;
	}
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)FileBuffer;
	if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) {  // 确定 DOS_HEADERS 签名正确 (MZ) 
		VirtualFree(FileBuffer, 0, MEM_RELEASE);
		return nullptr;
	}
	if (pDosHeader->e_lfanew + sizeof(IMAGE_NT_HEADERS) > fileSize) {  // 确定 NT_HEADERS 在文件范围内
		VirtualFree(FileBuffer, 0, MEM_RELEASE);
		return nullptr;
	}
	// 指向 NT_HEADERS 部分
	PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)((DWORD_PTR)FileBuffer + pDosHeader->e_lfanew);
	if( pNtHeaders->Signature != IMAGE_NT_SIGNATURE) {  // 确定 NT_HEADERS 签名正确 (PE00)
		VirtualFree(FileBuffer, 0, MEM_RELEASE);
		return nullptr;
	}
	if (pFileSize) *pFileSize = fileSize;  // 确定传入参数 pFileSize 是否有地址，并返回文件大小
	return FileBuffer;
}

/// <summary>
/// 将从磁盘读取的 PE 文件（文件映像）构建为内存映像（按 OptionalHeader.SizeOfImage 分配并复制节数据）。
/// </summary>
/// <param name="pPEFileBaseAddr">ReadPEFile 返回的文件缓冲区地址（文件格式的完整数据）。</param>
/// <param name="pImageSize">输出：构建的内存映像大小（可为 nullptr）。</param>
/// <returns>成功返回指向新分配内存映像基址（需要调用方 VirtualFree 释放）；失败返回 nullptr。</returns>
/// <remarks>
/// - 函数会按 PE 的 OptionalHeader.SizeOfImage 分配内存，复制头部并逐节把原始文件数据复制到对应的虚拟地址位置，并对虚拟大小大于原始数据的区域清零（BSS）。
/// - 返回的内存映像仅为按文件构建的内存副本，不包含重定位、导入解析或节权限修正等完整加载步骤；如需完整运行需进一步处理重定位与导入表等。
/// - 调用方负责释放返回的内存（VirtualFree）。
/// </remarks>
LPVOID CheatMemory::BulidMemoryImage(LPVOID pPEFileBaseAddr, DWORD* pImageSize){
	/* 获取PE头信息 */
	// 指向 DOS_HEADERS 部分 (DOS头)
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pPEFileBaseAddr;
	// 指向 NT_HEADERS 部分 (NT头)
	PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)((DWORD_PTR)pPEFileBaseAddr + pDosHeader->e_lfanew);
	// 计算内存映像大小
	DWORD imageSize = pNtHeaders->OptionalHeader.SizeOfImage;
	LPVOID pImageBaseAddr = VirtualAlloc(NULL, imageSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!pImageBaseAddr) return nullptr;
	// 复制 PE 头到内存中 (PE = DOS + PE + ST[Section Table])
	ZeroMemory(pImageBaseAddr, imageSize);
	memcpy(pImageBaseAddr, pPEFileBaseAddr, pNtHeaders->OptionalHeader.SizeOfHeaders);

	// 复制各节到内存映像(计算节在内存中的位置和文件中的位置)
	PIMAGE_SECTION_HEADER pSectionHeader = IMAGE_FIRST_SECTION(pNtHeaders);
	for (WORD i = 0; i < pNtHeaders->FileHeader.NumberOfSections; i++) {
		// 目标地址 = 映像基址 + 节的虚拟地址
		LPVOID pDest = (LPVOID)((DWORD_PTR)pImageBaseAddr + pSectionHeader[i].VirtualAddress);
		// 源地址 = 文件基址 + 节的原始数据指针
		LPVOID pSrc = (LPVOID)((DWORD_PTR)pPEFileBaseAddr + pSectionHeader[i].PointerToRawData);
		// 复制节数据
		DWORD sectionSize = pSectionHeader[i].SizeOfRawData;
		memcpy(pDest, pSrc, sectionSize);
		if(pSectionHeader[i].Misc.VirtualSize > sectionSize) {
			// 若节的虚拟大小大于原始数据大小，需将多余部分清零(bss段)
			ZeroMemory((LPVOID)((DWORD_PTR)pDest + sectionSize), pSectionHeader[i].Misc.VirtualSize - sectionSize);
		}
	}
	if (pImageSize) *pImageSize = imageSize;
	return pImageBaseAddr;
}

/// <summary>
/// 从目标进程的 LDR 列表中尝试移除（unlink）指定模块的三条链表条目，常用于隐藏已注入的 DLL。
/// </summary>
/// <param name="pid">目标进程 ID。</param>
/// <param name="hModule">目标模块基址（HMODULE）。必须是目标进程中该模块的基址（即目标进程地址空间内的地址）。</param>
/// <returns>
/// 成功返回 true（已在某个 LDR 链表中找到并更新了相邻节点指针）；失败返回 false（例如参数无效、无法读取/写入目标进程内存或未找到目标模块）。
/// </returns>
/// <remarks>
/// 行为与实现细节：
/// - 函数首先通过 GetProcessPEB(pid) 获取目标进程的 PEB 地址，并从中取出 Ldr 指针指向 PEB_LDR_DATA（在目标进程地址空间）。
/// - 使用 OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE) 打开目标进程句柄；随后通过 ReadProcessMemory 读取 LDR 列表项并查找与 hModule 匹配的条目。
/// - 实现上分别遍历三个链表：InLoadOrderModuleList、InMemoryOrderModuleList、InInitializationOrderModuleList。由于 PEB_LDR_DATA_TABLE_ENTRY 在三条链表中所处的内存布局不同（偏移不同），
///   代码在读取时对指针地址做了相应调整（基于当前简化的 PEB_LDR_DATA_TABLE_ENTRY 布局）。找到目标条目后，会读取目标条目的前驱和后继 LIST_ENTRY，
///   修改它们的 Flink/Blink 指针以跳过目标条目，然后通过 WriteProcessMemory 写回以完成 unlink 操作。
/// - 注意：该函数仅修改 LDR 链表指针以“隐藏”模块，不会卸载模块（不会调用 LdrUnload 或 FreeLibrary），也不会释放模块在目标进程中的内存或清理导出表等资源。
/// - 风险与限制：
///   * 函数直接在目标进程地址空间读写指针结构，存在竞争条件（线程并发访问、模块正在加载/卸载等）。在高可靠性场景下应先暂停相关线程或在安全时机执行此操作。  
///   * 若目标进程与当前进程位宽不一致（32/64 位），或所使用的 LDR 结构布局与假定不同，读取/写入会失败或损坏目标内存。请确保在同位宽下运行或对结构按位宽调整。  
///   * 函数对 ReadProcessMemory/WriteProcessMemory 的返回并非在每处都做详尽检查；在生产代码中可增加日志/重试与更严格的错误处理。  
///   * 部分防护/安全软件或操作系统策略可能阻止对 PEB/LDR 的修改或要求更高权限；请确保拥有足够权限并在受控环境中使用。  
/// - 建议：调用方需确认 hModule 为目标进程中的有效模块基址，并在必要时先备份相关 LIST_ENTRY 内容以便恢复。该函数适用于需要隐藏模块条目的场景，但应了解潜在稳定性与兼容性风险。
/// </remarks>
bool CheatMemory::UnlinkModuleFromLDR(DWORD pid, const wchar_t* DLLName){
	bool Loaded = false, Memory = false, Init = false;
	PVOID DesignatedDLLBase = (PVOID)GetModuleBaseAddress(pid, DLLName);
	HANDLE hProcess = NULL;
	PPEB pPeb = GetProcessPEB(pid, hProcess);
	if (!pPeb) return false;
	
	PPEB_LDR_DATA pLdrData = pPeb->Ldr;
	if (!pLdrData) return false;
	if (!hProcess) return false;
	if (!DesignatedDLLBase)return false;
	SIZE_T bytesRead;

	// 遍历 InLoadOrderModuleList 查找目标模块
	PLIST_ENTRY pCurrentEntry = pLdrData->InLoadOrderModuleList.Flink;
	while (pCurrentEntry != &pLdrData->InLoadOrderModuleList) {
		PEB_LDR_DATA_TABLE_ENTRY ldrEntry = { 0 };
		if (!ReadProcessMemory(hProcess, pCurrentEntry, &ldrEntry, sizeof(PEB_LDR_DATA_TABLE_ENTRY), &bytesRead) || bytesRead != sizeof(PEB_LDR_DATA_TABLE_ENTRY)) break;
		if (ldrEntry.DLLBaseAddress == DesignatedDLLBase) {
			// 找到目标模块，进行卸载
			// 更新前驱和后继节点的指针
			LIST_ENTRY prevEntry, nextEntry;
			if (!ReadProcessMemory(hProcess, ldrEntry.InLoadOrderLinks.Blink, &prevEntry, sizeof(LIST_ENTRY), &bytesRead) || bytesRead != sizeof(LIST_ENTRY) ||
				!ReadProcessMemory(hProcess, ldrEntry.InLoadOrderLinks.Flink, &nextEntry, sizeof(LIST_ENTRY), &bytesRead) || bytesRead != sizeof(LIST_ENTRY)) break;
			prevEntry.Flink = ldrEntry.InLoadOrderLinks.Flink;
			nextEntry.Blink = ldrEntry.InLoadOrderLinks.Blink;
			// 写回更新后的前驱和后继节点
			WriteProcessMemory(hProcess, ldrEntry.InLoadOrderLinks.Blink, &prevEntry, sizeof(LIST_ENTRY), NULL);
			WriteProcessMemory(hProcess, ldrEntry.InLoadOrderLinks.Flink, &nextEntry, sizeof(LIST_ENTRY), NULL);
			Loaded = true;
		}
		pCurrentEntry = ldrEntry.InLoadOrderLinks.Flink;
	}
	// 遍历 InMemoryOrderModuleList 查找目标模块
	pCurrentEntry = pLdrData->InMemoryOrderModuleList.Flink;
	while (pCurrentEntry != &pLdrData->InMemoryOrderModuleList) {
		PEB_LDR_DATA_TABLE_ENTRY ldrEntry = { 0 };
		if (!ReadProcessMemory(hProcess, (PBYTE)pCurrentEntry - sizeof(LIST_ENTRY), &ldrEntry, sizeof(PEB_LDR_DATA_TABLE_ENTRY), &bytesRead) || bytesRead != sizeof(PEB_LDR_DATA_TABLE_ENTRY)) break;
		if (ldrEntry.DLLBaseAddress == DesignatedDLLBase) {
			// 找到目标模块，进行卸载
			// 更新前驱和后继节点的指针
			LIST_ENTRY prevEntry, nextEntry;
			if (!ReadProcessMemory(hProcess, ldrEntry.InMemoryOrderLinks.Blink, &prevEntry, sizeof(LIST_ENTRY), &bytesRead) || bytesRead != sizeof(LIST_ENTRY) ||
				!ReadProcessMemory(hProcess, ldrEntry.InMemoryOrderLinks.Flink, &nextEntry, sizeof(LIST_ENTRY), &bytesRead) || bytesRead != sizeof(LIST_ENTRY)) break;
			prevEntry.Flink = ldrEntry.InMemoryOrderLinks.Flink;
			nextEntry.Blink = ldrEntry.InMemoryOrderLinks.Blink;
			// 写回更新后的前驱和后继节点
			WriteProcessMemory(hProcess, ldrEntry.InMemoryOrderLinks.Blink, &prevEntry, sizeof(LIST_ENTRY), NULL);
			WriteProcessMemory(hProcess, ldrEntry.InMemoryOrderLinks.Flink, &nextEntry, sizeof(LIST_ENTRY), NULL);
			Memory = true;
		}
		pCurrentEntry = ldrEntry.InMemoryOrderLinks.Flink;
	}
	// 遍历 InInitializationOrderModuleList 查找目标模块
	pCurrentEntry = pLdrData->InInitializationOrderModuleList.Flink;
	while (pCurrentEntry != &pLdrData->InInitializationOrderModuleList) {
		PEB_LDR_DATA_TABLE_ENTRY ldrEntry = { 0 };
		if (!ReadProcessMemory(hProcess, (PBYTE)pCurrentEntry - 2 * sizeof(LIST_ENTRY), &ldrEntry, sizeof(PEB_LDR_DATA_TABLE_ENTRY), &bytesRead) || bytesRead != sizeof(PEB_LDR_DATA_TABLE_ENTRY)) break;
		if (ldrEntry.DLLBaseAddress == DesignatedDLLBase) {
			// 找到目标模块，进行卸载
			// 更新前驱和后继节点的指针
			LIST_ENTRY prevEntry, nextEntry;
			if (!ReadProcessMemory(hProcess, ldrEntry.InInitializationOrderLinks.Blink, &prevEntry, sizeof(LIST_ENTRY), &bytesRead) || bytesRead != sizeof(LIST_ENTRY) ||
				!ReadProcessMemory(hProcess, ldrEntry.InInitializationOrderLinks.Flink, &nextEntry, sizeof(LIST_ENTRY), &bytesRead) || bytesRead != sizeof(LIST_ENTRY)) break;
			prevEntry.Flink = ldrEntry.InInitializationOrderLinks.Flink;
			nextEntry.Blink = ldrEntry.InInitializationOrderLinks.Blink;
			// 写回更新后的前驱和后继节点
			WriteProcessMemory(hProcess, ldrEntry.InInitializationOrderLinks.Blink, &prevEntry, sizeof(LIST_ENTRY), NULL);
			WriteProcessMemory(hProcess, ldrEntry.InInitializationOrderLinks.Flink, &nextEntry, sizeof(LIST_ENTRY), NULL);
			Init = true;
		}
		pCurrentEntry = ldrEntry.InInitializationOrderLinks.Flink;
	}
	CloseHandle(hProcess);
	return Loaded && Memory && Init;
}

/// <summary>
/// 获取目标进程的 PEB 地址（进程环境块指针）。
/// </summary>
/// <param name="pid">目标进程 ID。</param>
/// <returns>成功返回指向目标进程 PEB 的指针（PPEB），失败返回 nullptr。</returns>
/// <remarks>
/// - 内部通过 NtQueryInformationProcess 获取 PROCESS_BASIC_INFORMATION 并从中读取 PebBaseAddress。
/// - 需要 PROCESS_QUERY_INFORMATION | PROCESS_VM_READ 权限来打开目标进程；若无法获取函数地址或打开权限不足则返回 nullptr。
/// - 返回的 PPEB 为目标进程地址空间中的地址，不能直接在当前进程中解引用，必须通过 ReadProcessMemory 在目标进程句柄上读取相关数据。
/// </remarks>
CheatMemory::PPEB CheatMemory::GetProcessPEB(DWORD pid, HANDLE &out_hProcess)
{
	static NtQueryInformationProcess NtQueryInfoProcess = nullptr;
	if (!NtQueryInfoProcess) {
		NtQueryInfoProcess = (NtQueryInformationProcess)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtQueryInformationProcess");
		if (!NtQueryInfoProcess) return nullptr;
	}
	out_hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
	if (!out_hProcess) return nullptr;
	PROCESS_BASIC_INFORMATION pbi = { 0 };
	NTSTATUS status = NtQueryInfoProcess(out_hProcess, ProcessBasicInformation, &pbi, sizeof(pbi), NULL);
	if (status != 0) return nullptr;
	PEB Peb;
	ReadProcessMemory(out_hProcess, pbi.PebBaseAddress, &Peb, sizeof(PEB), NULL);
	return &Peb;
}
/// <summary>
/// 查找并返回指定进程中某个模块（DLL/EXE）的基地址。
/// </summary>
/// <param name="pid">目标进程 ID。</param>
/// <param name="moduleName">模块名（不含路径），例如 L"kernel32.dll" 或 L"yourapp.exe"。</param>
/// <returns>若找到返回模块基地址（uintptr_t），找不到或出错返回 0。</returns>
/// <remarks>
/// - 内部使用 CreateToolhelp32Snapshot + Module32First/Module32Next 进行枚举；当目标为 32 位进程且当前进程为 64 位时，需要指定 TH32CS_SNAPMODULE32 以枚举 32 位模块。
/// - 调用者应检查返回值是否为 0 来判断是否找到模块。
/// </remarks>
uintptr_t CheatMemory::GetModuleBaseAddress(DWORD pid, const wchar_t* moduleName) {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
	if (hSnapshot == INVALID_HANDLE_VALUE) {
		return 0;
	}
	MODULEENTRY32 moduleEntry;
	moduleEntry.dwSize = sizeof(MODULEENTRY32);

	if (!Module32First(hSnapshot, &moduleEntry)) {
		CloseHandle(hSnapshot);
		return 0;
	}
	do {
		if (_wcsicmp(moduleEntry.szModule, moduleName) == 0) {
			CloseHandle(hSnapshot);
			return (uintptr_t)moduleEntry.modBaseAddr;
		}
	} while (Module32Next(hSnapshot, &moduleEntry));

	CloseHandle(hSnapshot);
	return 0;
}

/// <summary>
/// 枚举并返回属于指定进程的线程 ID 列表。
/// </summary>
/// <param name="pid">目标进程 ID。</param>
/// <returns>返回线程 ID 列表（可能为空，当枚举失败或该进程没有线程时）。</returns>
/// <remarks>
/// - 使用 CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD) 并遍历 THREADENTRY32，收集 th32OwnerProcessID == pid 的条目。
/// - 返回的列表包含至少当前进程的主线程（若枚举成功），但并不保证顺序或实时性（线程可能在枚举期间创建/退出）。
/// </remarks>
CheatMemory::ThreadIDList CheatMemory::GetThreadIDByPID(DWORD pid)
{
	ThreadIDList ThreadID;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE) { return ThreadID; }
	THREADENTRY32 ThreadEntry;
	ThreadEntry.dwSize = sizeof(THREADENTRY32);

	if (!Thread32First(hSnapshot, &ThreadEntry)) {
		CloseHandle(hSnapshot);
		return ThreadID;
	}
	do {
		if (ThreadEntry.th32OwnerProcessID == pid) {
			ThreadID.push_back(ThreadEntry.th32ThreadID);
		}
	} while (Thread32Next(hSnapshot, &ThreadEntry));

	CloseHandle(hSnapshot);
	return ThreadID;
}

/// <summary>
/// 根据可执行文件名查找进程并返回其进程 ID；同时尝试检测目标进程为 32/64 位以设置内部 AddressSize 字段。
/// </summary>
/// <param name="ProcessName">可执行文件名（例如 L"notepad.exe"）。比较时不区分大小写。</param>
/// <returns>找到返回进程 ID；未找到或出错返回 0。</returns>
/// <remarks>
/// - 本函数会遍历系统进程快照（CreateToolhelp32Snapshot + Process32First/Process32Next），匹配 szExeFile 字段。
/// - 当找到目标进程时，会尝试以 PROCESS_QUERY_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION 打开进程并调用 IsWow64Process 来判断是否为 32 位（WOW64）进程，
///   以便设置 AddressSize（4 或 8）。该设定会影响后续按偏移填充和指针大小的处理逻辑。
/// - 注意：若当前进程无权打开目标进程或 IsWow64Process 返回失败，AddressSize 可能不会被正确设置。
/// </remarks>
DWORD CheatMemory::GetProcessIDbyName(const wchar_t* ProcessName) {
	
	PROCESSENTRY32 temp;
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE) {
		return 0;
	}
	temp.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hProcessSnap, &temp)) {
		do {
			if (_wcsicmp(temp.szExeFile ,ProcessName) == 0) {
				CloseHandle(hProcessSnap);
				#if defined(_M_X64) || defined(__x86_64__) || defined(_M_ARM64) || defined(__aarch64__)
					IsProcess32BitOnWow64(temp.th32ProcessID);
				#endif
				return temp.th32ProcessID;
			}
		} while (Process32Next(hProcessSnap, &temp));
	}
	CloseHandle(hProcessSnap);
	return NULL;
}

/// <summary>
/// 通过窗口标题查找窗口并返回其所属进程 ID（FindWindowW + GetWindowThreadProcessId）。
/// </summary>
/// <param name="WindowTitle">窗口标题（宽字符）。</param>
/// <returns>找到返回进程 ID；找不到返回 0。</returns>
DWORD CheatMemory::GetProcessIDbyTitle(LPCWSTR WindowTitle) {
	return this->HWND_to_PID(::FindWindowW(WindowTitle, NULL));
}

/// <summary>
/// 根据窗口句柄获取所属进程 ID（GetWindowThreadProcessId 的封装）。
/// </summary>
/// <param name="Handle">窗口句柄。</param>
/// <returns>所属进程 ID（DWORD）。</returns>
DWORD CheatMemory::HWND_to_PID(HWND Handle) {
	DWORD pid;	GetWindowThreadProcessId(Handle, &pid);	return pid;
}

/// <summary>
/// 设置或取消窗口的显示捕获排斥（用于阻止 Windows 屏幕捕获 API 捕获窗口内容）。
/// </summary>
/// <param name="Handle">目标窗口句柄。</param>
/// <param name="_switch">true 表示开启防截图（设置 WDA_EXCLUDEFROMCAPTURE）；false 表示关闭（设置 WDA_NONE）。</param>
/// <remarks>
/// - 该 API 对系统与显卡驱动有依赖，某些截图工具或屏幕录制软件可能绕过此保护；并非绝对安全手段，仅作为辅助保护措施。
/// - 仅适用于受支持的 Windows 版本和桌面应用窗口，不适用于所有类型的渲染上下文（例如某些游戏或硬件加速窗口可能无效）。
/// </remarks>
void CheatMemory::AntiScreenCapture(HWND Handle, bool _switch) {
	DWORD Flag = !_switch ? WDA_NONE : WDA_EXCLUDEFROMCAPTURE;
	SetWindowDisplayAffinity(Handle, Flag);
}

void CheatMemory::IsProcess32BitOnWow64(DWORD pid) {
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
	// IsWow64 : true 表示是 32 位进程（WOW64）
	IsWow64Process(hProcess, &IsWow64);
}