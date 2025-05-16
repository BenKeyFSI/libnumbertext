/* Soros interpreter (see numbertext.org)
 * 2018 (c) László Németh
 * License: LGPL/BSD dual license */

#ifndef NUMBERTEXT_HXX
#define NUMBERTEXT_HXX

#include "NumberTextAPI.hxx"
#include "Soros.hxx"
#include <unordered_map>

class Numbertext
{
public:
    NUMBER_TEXT_API Numbertext();
    NUMBER_TEXT_API void set_prefix(const std::string& st)
    {
        prefix = st;
    }
    NUMBER_TEXT_API void set_prefix(const std::wstring& st)
    {
        prefix = wstring2string(st);
    }
    NUMBER_TEXT_API bool load(std::string lang, std::string filename = "");
    NUMBER_TEXT_API bool load(std::wstring lang, std::wstring filename = L"");
    NUMBER_TEXT_API bool good() const;
    NUMBER_TEXT_API bool numbertext(std::wstring& number, const std::string& lang);
    NUMBER_TEXT_API bool numbertext(std::wstring& number, const std::wstring& lang);
    // UTF-8 encoded input
    NUMBER_TEXT_API bool numbertext(std::string& number, const std::string& lang);
    NUMBER_TEXT_API std::string numbertext(int number, const std::string& lang);
    NUMBER_TEXT_API std::wstring numbertext(int number, const std::wstring& lang);
    NUMBER_TEXT_API static std::wstring string2wstring(const std::string& s);
    NUMBER_TEXT_API static std::string wstring2string(const std::wstring& s);

private:
    std::string prefix;
    std::unordered_map<std::string, Soros> modules;
};

#endif
