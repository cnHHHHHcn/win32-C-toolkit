#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <locale>
#include <codecvt>

#pragma once

class Configuration
{
public:	
	Configuration(std::wstring File = L"");

	typedef std::map<std::wstring /*Key*/, std::wstring /*Value*/> KeyValueList;

	struct stru_Section {
		std::wstring SectionName = L"";
		KeyValueList DataList;
	};

	typedef std::vector<stru_Section> SectionList;
	
	bool LoadSuccess = false; 
	void SetFile(std::wstring File);
	void ModifySection(std::wstring Section_Old_Name, std::wstring Section_New_Name);
	std::vector<std::wstring> GetSectionName();
	stru_Section GetSectionData(std::wstring Section);
	void RemoveSection(std::wstring Section);
	
	template<typename T>
	void ModifyKeyValue(std::wstring Section, std::wstring Key, T Value);
	void ModifyMultipleKeyValue(std::wstring Section, KeyValueList MultipleList);
	std::vector <std::wstring> GetKeyName(std::wstring Section);
	template<typename T>
	std::wstring GetKeyValue(std::wstring Section, std::wstring Key);
	void RemoveKey(std::wstring Section, std::wstring Key);
	
	bool Reload();
	void Save();
	void SaveAs(std::wstring NewFile);

private:
	std::wstring m_File = L"";
	SectionList m_SectionList;
	// 文件操作函数
	bool LoadFile(std::wstring File); 
	// 字符调整函数 
	std::vector<std::wstring> Split(std::wstring str_Data, const wchar_t* delimter);
	// 类型转换
	template<typename T>
	T ConvertWString(const std::wstring& str);
	template<typename T>
	std::wstring ConvertToWString(const T& value);
};

template<typename T>
void Configuration::ModifyKeyValue(std::wstring Section, std::wstring Key, T Value)
{
	for (int i = 0; i < m_SectionList.size(); i++) {
		if (m_SectionList[i].SectionName == Section) {
			m_SectionList[i].DataList[Key] = this->ConvertToWString(Value);
			break;
		}
	}
}

template<typename T>
std::wstring Configuration::GetKeyValue(std::wstring Section, std::wstring Key)
{
	for (const auto& sec : m_SectionList) {
		if (sec.SectionName == Section) {
			auto it = sec.DataList.find(Key);
			if (it != sec.DataList.end()) {
				return this->ConvertWString<T>(it->second);
			}
		}
	}
}

template<typename T>
std::wstring Configuration::ConvertToWString(const T& value) {
	std::wstringstream wss;
	wss << value;
	return wss.str();
}

template<typename T>
T Configuration::ConvertWString(const std::wstring& str) {
	T value{};
	std::wstringstream wss(str);
	wss >> value;
	return value;
}