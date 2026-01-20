#include "Configuration.h"


Configuration::Configuration(std::wstring File)
{
	m_File = File;
	if (File != L"") LoadSuccess = LoadFile(File); 
}

void Configuration::SetFile(std::wstring File)
{
	if (File == L"") return;
	m_File = File;
	m_SectionList.clear();
	LoadFile(File);
}

void Configuration::ModifySection(std::wstring Section_Old_Name, std::wstring Section_New_Name)
{
	for(int i = 0; i < m_SectionList.size(); i++){
		if(m_SectionList[i].SectionName == Section_Old_Name){
			m_SectionList[i].SectionName = Section_New_Name;
		}
	}
	stru_Section tmp = { Section_New_Name, {} };
	m_SectionList.push_back(tmp);
}

std::vector<std::wstring> Configuration::GetSectionName()
{
	std::vector<std::wstring> RTN;
	for(int i = 0; i < m_SectionList.size(); i++){
		RTN.push_back(m_SectionList[i].SectionName); 
	}
	return RTN;
}

Configuration::stru_Section Configuration::GetSectionData(std::wstring Section)
{
    for(int i = 0; i < m_SectionList.size(); i++){
    	if(m_SectionList[i].SectionName == Section) return m_SectionList[i]; 
	}
}


void Configuration::RemoveSection(std::wstring Section) {
	if (Section.empty()) return;
	for (auto it = m_SectionList.begin(); it != m_SectionList.end(); ) {
		if (it->SectionName == Section) {
			it = m_SectionList.erase(it);
		}
		else {
			++it;
		}
	}
}

void Configuration::ModifyMultipleKeyValue(std::wstring Section, KeyValueList MultipleList)
{
	for(int i = 0; i < m_SectionList.size(); i++){
		if(m_SectionList[i].SectionName == Section){
			for(KeyValueList::iterator it = MultipleList.begin(); it != MultipleList.end(); ++it){
				m_SectionList[i].DataList[it->first] = it->second; 
			}
		}
	} 
}

std::vector<std::wstring> Configuration::GetKeyName(std::wstring Section)
{
	std::vector<std::wstring> RTN; 
	for(int i = 0; i < m_SectionList.size(); i++){
		if (m_SectionList[i].SectionName == Section) {
			for (KeyValueList::iterator it = m_SectionList[i].DataList.begin(); it != m_SectionList[i].DataList.end(); ++it) {
				RTN.push_back(it->first);
			}
		}
	}
	return RTN;
}



void Configuration::RemoveKey(std::wstring Section, std::wstring Key)
{
	for(int i = 0; i < m_SectionList.size(); i++){
		if(m_SectionList[i].SectionName == Section){
			m_SectionList[i].DataList.erase(Key); break;
		}
	}
}

bool Configuration::Reload()
{
	m_SectionList.clear();
	LoadSuccess = LoadFile(m_File);
	return LoadSuccess;
}

void Configuration::Save()
{
	std::wofstream OutFile(m_File, std::ios::out | std::ios::trunc); 
	for(int i = 0; i < m_SectionList.size(); i++){
		OutFile << "[" << m_SectionList[i].SectionName << "]" << std::endl;
		for(KeyValueList::iterator it = m_SectionList[i].DataList.begin(); it != m_SectionList[i].DataList.end(); ++it){
			OutFile << it->first << "=" << it->second << std::endl; 
		}
		OutFile << std::endl; 
	}
}

void Configuration::SaveAs(std::wstring NewFile)
{
	std::wofstream OutFile(NewFile, std::ios::out); 
	for(int i = 0; i < m_SectionList.size(); i++){
		OutFile << "[" << m_SectionList[i].SectionName << "]" << std::endl;
		for(KeyValueList::iterator it = m_SectionList[i].DataList.begin(); it != m_SectionList[i].DataList.end(); ++it){
			OutFile << it->first << "=" << it->second << std::endl; 
		}
		OutFile << std::endl; 
	}
}

bool Configuration::LoadFile(std::wstring File)
{	
	std::wstring tmp_Data, tmp_Key, tmp_Value;
	stru_Section tmp_Section;
	std::wifstream InFile(File, std::ios::in);
	InFile.imbue(std::locale(InFile.getloc(), new std::codecvt_utf8_utf16<wchar_t>));
	if (InFile.is_open()) {
		while (!InFile.eof()) {
			std::getline(InFile, tmp_Data);
			if (tmp_Data.find(L"[") != -1 && tmp_Data.find(L"]") != -1) {
				if (tmp_Section.SectionName != L"") {
					m_SectionList.push_back(tmp_Section);
					tmp_Section.DataList.clear();
				}
				tmp_Section.SectionName = tmp_Data.substr(1, tmp_Data.find(L"]") - 1);
			}
			if (tmp_Data.find(L"=") != -1) {
				tmp_Key = tmp_Data.substr(0, tmp_Data.find(L"="));
				tmp_Value = tmp_Data.substr(tmp_Data.find(L"=") + 1, tmp_Data.length() - (tmp_Data.find(L"=") + 1));
				tmp_Section.DataList.insert({ tmp_Key, tmp_Value });
			}
		}
		m_SectionList.push_back(tmp_Section);
    }else{
    	InFile.close();
    	return false;
	}
    InFile.close();
    return true;
}

std::vector<std::wstring> Configuration::Split(std::wstring str_Data, const wchar_t *delimter)
{
    std::vector<std::wstring> list;
    while (str_Data.find(delimter) != -1) {
        list.push_back(str_Data.substr(0, str_Data.find(delimter)));
        str_Data = str_Data.substr(str_Data.find(delimter) + 1);
    }
    return list;
}


