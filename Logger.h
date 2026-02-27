#pragma once

#include <fstream>
#include <strstream>
#include <string>
#include <locale>
#include <codecvt>
#include <ctime>

class Logger
{
public:
	bool FileState = false;
	enum Level { LOG_INFO, LOG_DEBUG, LOG_WARN, LOG_ERROR };
	Logger(std::wstring File = L"");
	~Logger();
	void SetFile(std::wstring File);
	std::wstring GetFile();
	std::wstring GetTime();
	void Info(std::wstring Message);
	void Debug(std::wstring Message);
	void Warn(std::wstring Message);
	void Error(std::wstring Message);
private:
	std::wfstream fileOpera;
	std::wstring m_File;
	void LogOut(Level level, std::wstring Message);
	std::wstring string_to_wstring(const std::string& str);
	std::string wstring_to_string(const std::wstring& wstr);
};

