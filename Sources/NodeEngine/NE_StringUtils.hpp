#ifndef NE_STRINGUTILS_HPP
#define NE_STRINGUTILS_HPP

#include <string>

namespace NE
{

std::wstring	StringToWString (const std::string& str);
std::string		WStringToString (const std::wstring& str);

std::wstring	ReplaceAll (const std::wstring& string, const std::wstring& from, const std::wstring& to);
std::wstring	ReplaceAll (const std::wstring& string, const std::wstring& from, const std::initializer_list<std::wstring>& to);

}

#endif
