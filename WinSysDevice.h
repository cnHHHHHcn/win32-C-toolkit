#pragma once

#include <windows.h>
#include <imm.h>
#include <vector>
#include <map>
#include <string>
#include <fstream>

#pragma comment(lib, "imm32.lib")

#define DESKTOP
#define MOUSE
#define KEYBOARD
#define CLIPBOARD
#define CAPTURE

class WinSysDevice{
public:
	//WinSysDevice();
	//~WinSysDevice();
	struct StringInfo {
		void* pData;
		USHORT Size;
	};
#if defined(DESKTOP)
public:
	struct WinSize {
		int Width;
		int Height;
	};

	WinSize GetDesktopSize();   // 获取桌面大小尺寸
#endif  // DESKTOP

#if defined(MOUSE)
public:
	enum MouseKey {
		MouseNULL = 0x0,
		LeftClick = 0x2 + 0x4,  // 左键点击
		LeftDown = 0x2,  // 左键按下
		LeftUp = 0x4,   // 左键弹起
		RightClick = 0x8 + 0x10, // 右键点击
		RightDown = 0x8,   // 右键按下
		RightUp = 0x10,   // 右键弹起
		MiddleClick = 0x20 + 0x40,   // 滚轮键点击
		MiddleDown = 0x20,   // 滚轮键按下
		MiddleUp = 0x40   // 滚轮键弹起
	};

	struct MouseInfo {
		WinSysDevice::MouseKey Key = MouseNULL;
		POINT AbsolutePos = { 0, 0 };
		POINT ScreenPos = { 0, 0 };
		int WheelMove = 0;
	};

	void MouseSend(MouseInfo Data);   // 鼠标输出
	MouseKey ExchangeMouseMSG(UINT MouseMSG);
	POINT GetAbsolutePos();   // 获取鼠标相对位置
	POINT GetScreenPos();   // 获取鼠标绝对位置
	bool DisplayMouseInfo(MouseInfo Data, POINT PreviousPoint = {0, 0});
private:
	POINT LastPos;
#endif  // MOUSE

#if defined(KEYBOARD)
public:
	enum KeyboardKey {
		/*
		IME_ON = VK_IME_ON,    // IME On
		IME_OFF = VK_IME_OFF,    // IME OFF
		IME_KANJI = VK_KANJI,    // IME 汉字
		IME_Convert = VK_CONVERT,       //IME 转换
		*/
		KeyboardNULL = 0x0,
		Esc = VK_ESCAPE,    // Esc 键
		Space = VK_SPACE,     // 空格键
		BackSpace = VK_BACK,      // Backspace 键
		Tab = VK_TAB,	      // Tab 键
		Clear = VK_CLEAR,     // 清除键
		Enter = VK_RETURN,   // 回车键

		LWin = VK_LWIN,      // 左 Windows 徽标键
		RWin = VK_RWIN,      // 右 Windows 徽标键

		Shift = VK_SHIFT,     // 换档键
		LShift = VK_LSHIFT,   // 左移键
		RShift = VK_RSHIFT,   // 右移键

		Crtl = VK_CONTROL,   // Ctrl 键
		LCrtl = VK_LCONTROL, // 左 Ctrl 键
		RCrtl = VK_RCONTROL, // 右 Ctrl 键

		Alt = VK_MENU,       // Alt 键
		LAlt = VK_LMENU,     // 左 Alt 键
		RAlt = VK_RMENU,     // 右 Alt 键

		Pause = VK_PAUSE,     // 暂停键
		CapsLock = VK_CAPITAL,  // Caps lock 键
		NumLock = VK_NUMLOCK,   // Num lock 键
		ScrollLock = VK_SCROLL,    // 滚动锁键

		Insert = VK_INSERT,    // 插入键
		Delete = VK_DELETE,    // 删除密钥
		End = VK_END,       // 结束键
		Home = VK_HOME,      // 主键
		PgUp = VK_PRIOR,     // Page up 键
		PgDw = VK_NEXT,      // Page down 键

		Left = VK_LEFT,      // 向左键
		Up = VK_UP,        // 向上键
		Right = VK_RIGHT,     // 向右键
		Down = VK_DOWN,      // 向下键

		OEM_Comma = VK_OEM_COMMA,     // , < 符号
		OEM_Period = VK_OEM_PERIOD,     // . > 符号
		OEM_1 = VK_OEM_1,    // ; : 符号
		OEM_2 = VK_OEM_2,    // / ? 符号
		OEM_3 = VK_OEM_3,    // ` ~ 符号
		OEM_4 = VK_OEM_4,    // [ { 符号
		OEM_5 = VK_OEM_5,    // \ | 符号
		OEM_6 = VK_OEM_6,    // ] } 符号
		OEM_7 = VK_OEM_7,    // ' " 符号

		Sleep_Key = VK_SLEEP,     // 计算机睡眠键
		Num0 = VK_NUMPAD0,   // 数字键盘 0 键
		Num1 = VK_NUMPAD1,   // 数字键盘 1 键
		Num2 = VK_NUMPAD2,   // 数字键盘 2 键
		Num3 = VK_NUMPAD3,   // 数字键盘 3 键
		Num4 = VK_NUMPAD4,   // 数字键盘 4 键
		Num5 = VK_NUMPAD5,   // 数字键盘 5 键
		Num6 = VK_NUMPAD6,   // 数字键盘 6 键
		Num7 = VK_NUMPAD7,   // 数字键盘 7 键
		Num8 = VK_NUMPAD8,   // 数字键盘 8 键
		Num9 = VK_NUMPAD9,   // 数字键盘 9 键
		Multiply = VK_MULTIPLY,  // 相乘键
		Add = VK_ADD,       // 添加密钥
		Separator = VK_SEPARATOR, // 分隔符键
		Sub = VK_SUBTRACT,  // 减去键
		Decimal = VK_DECIMAL,   // 十进制键
		Divide = VK_DIVIDE,    // 除键
		F1 = VK_F1,        // F1 键
		F2 = VK_F2,        // F2 键
		F3 = VK_F3,        // F3 键
		F4 = VK_F4,        // F4 键
		F5 = VK_F5,        // F5 键
		F6 = VK_F6,        // F6 键
		F7 = VK_F7,        // F7 键
		F8 = VK_F8,        // F8 键
		F9 = VK_F9,        // F9 键
		F10 = VK_F10,       // F10 键
		F11 = VK_F11,       // F11 键
		F12 = VK_F12,       // F12 键
	};
	struct InputInfo {
		std::wstring LayoutName;
		std::wstring LanguageName;
		HKL LayoutHandle;
	};

	enum LayoutOpera {
		List_Current = 0,      // 显示当前
		List_All = 1,          // 显示全部
		Set_Next = 2,          // 设置下一个
		Set_Appoint = 3,       // 指定设置
	};
	enum ControlHotKey : char {
		Cut = 88,      // 剪切
		Copy = 67,     // 复制
		Paste = 86,    // 黏贴
		New = 78,      // 新建
		Open = 79,     // 打开
		All = 65,      // 全选
		Save = 83,     // 保存
		Repeat = 68,   // 重复
		Recall = 90,   // 撤回 
		Restore = 89,  // 恢复
	};
	struct KeyboardInfo {
		StringInfo WordData;
		wchar_t LanguageName[8];
		bool WithShift;
		ControlHotKey CtrlHotKey;
	};
	typedef std::map<int, InputInfo> LayoutMap;
	void KeyboardSend(std::wstring WordData, bool WithShift = false);   // 键盘输出(除去汉字)
	void CtrlHotKey(ControlHotKey Flag);   // 发送Ctrl快捷键
	LayoutMap InputMethod(LayoutOpera Flag, HKL HandleKeyboardLayout = 0);   // 输入法获取与切换
private:
	HKL GetCurrentKeyboardLayout();   // 获取当前键盘布局句柄
	std::map<int, HKL> GetAvailableKeyboardLayouts();   // 获取系统所有可用的键盘布局
	std::wstring GetKeyboardLayoutNames(HKL layout);   // 获取键盘布局的名称
	std::wstring GetKeyboardLayoutLanguageName(HKL layout);   // 获取键盘布局的语言名称

	bool SwitchToNextKeyboardLayout();   // 切换键盘布局
	bool SetKeyboardLayout(HKL HandleKeyboardLayout);   // 设置特定键盘布局
	void DisplayCurrentLayoutInfo(InputInfo& LayoutInfo);   // 显示当前键盘布局信息
	void DisplayAvailableLayouts(std::map<int, InputInfo>& LayoutInfo);   // 显示所有可用的键盘布局
#endif  // KEYBOARD
	
#if defined(CLIPBOARD)
public:
	enum CharVersion : char {
		ANSI = 'A',   // ANSI版本
		Unicode = 'W',   // UNICODE版本
	};
	struct ClipboardInfo {
		StringInfo ClipData = { 0 };			// 剪贴板数据内容，根据操作类型的不同，可能是要设置的数据或者获取到的数据
		UCHAR Version = Unicode;		// 版本，A 代表 ANSI版本，W 代表 UNICODE版本，默认使用 UNICODE版本
		UCHAR Operation = 'C';			// 操作类型，S 表示设置剪贴板数据，G 表示获取剪贴板数据，C 表示清除剪贴板数据
	};
	// A 代表 ANSI版本， W 代表 UNICODE版本，默认使用 UNICODE版本
	bool ClipboardSetTextA(std::string ClipData);   // 剪贴板设置数据
	bool ClipboardSetTextW(std::wstring ClipData);   
	std::string ClipboardGetTextA();  // 剪贴板获取数据 
	std::wstring ClipboardGetTextW();   
#if defined(KEYBOARD)
	void ClipboardSendTextA(std::string ClipData, int Delay = 5000);   // 剪贴板输出
	void ClipboardSendTextW(std::wstring ClipData, int Delay = 5000);   
#endif  // KEYBOARD
	bool ClipboardClear();   // 清除剪贴板内容
#endif  // CLIPBOARD
	
#if defined(CAPTURE)
public:
	struct CaptureInfo{
		USHORT Size = 0;					  // 文件路径长度
		wchar_t FilePath[MAX_PATH] = { 0 };   // 图片保存路径
	};
#if defined(DESKTOP)
	void CaptureDesktop(const wchar_t* FileName);   // 电脑桌面截图
#endif  // DESKTOP
	void CaptureWindow(HWND Handle, const wchar_t* FileName);   // 指定窗口截图
	void CaptureScreenArea(int x, int y, int Width, int Height, const wchar_t* FileName);   // 屏幕区域截图 
private:
	bool SaveBitmapToFile(HBITMAP hBitmap, const wchar_t* FileName);    // 保存图片后缀名为.bmp
#endif  // CAPTURE
};
