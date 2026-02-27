#include "pch.h"
#include "Logger.h"


Logger::Logger(std::wstring File)
{
	if (File.empty()) return;
	m_File = File; std::locale::global(std::locale(""));
	fileOpera.open(wstring_to_string(File), std::ios::in | std::ios::out | std::ios::app);
	FileState = fileOpera.is_open(); fileOpera.imbue(std::locale(fileOpera.getloc(), new std::codecvt_utf8<wchar_t>));
}

Logger::~Logger()
{
	if(fileOpera.is_open()) fileOpera.close();
}

void Logger::SetFile(std::wstring File)
{
	if (!m_File.empty()) fileOpera.close(); 
	std::locale::global(std::locale(""));
	fileOpera.open(wstring_to_string(File), std::ios::in | std::ios::out | std::ios::app);
	FileState = fileOpera.is_open(); fileOpera.imbue(std::locale(fileOpera.getloc(), new std::codecvt_utf8<wchar_t>));
}


std::wstring Logger::GetFile(){	return m_File; }

std::wstring Logger::GetTime()
{
	time_t Currenttime = time(0); tm timeInfo = { 0 }; localtime_s(&timeInfo, &Currenttime);
	std::wstring strCurTime;
	strCurTime.append(
		std::to_wstring(timeInfo.tm_year + 1900) + L"/" + std::to_wstring(timeInfo.tm_mon + 1) + L"/" + std::to_wstring(timeInfo.tm_mday) + L" " + 
		std::to_wstring(timeInfo.tm_hour) + L":" + std::to_wstring(timeInfo.tm_min) + L":" + std::to_wstring(timeInfo.tm_sec));
	return strCurTime;
}

void Logger::Info(std::wstring Message){ LogOut(LOG_INFO, Message); }

void Logger::Debug(std::wstring Message){ LogOut(LOG_DEBUG, Message); }

void Logger::Warn(std::wstring Message) { LogOut(LOG_WARN, Message); }

void Logger::Error(std::wstring Message){ LogOut(LOG_ERROR, Message); }

void Logger::LogOut(Level level, std::wstring Message)
{
	switch (level) {
	case LOG_INFO:  fileOpera << L"[INFO,";  break;
	case LOG_DEBUG: fileOpera << L"[DEBUG,"; break;
	case LOG_WARN:  fileOpera << L"[WARN,";  break;
	case LOG_ERROR: fileOpera << L"[ERROR,";
	}
	fileOpera << GetTime() << L"]" << Message;

}

std::wstring Logger::string_to_wstring(const std::string& str) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.from_bytes(str);
}
std::string Logger::wstring_to_string(const std::wstring& wstr) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.to_bytes(wstr);
}