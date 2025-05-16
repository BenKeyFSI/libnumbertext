/* Soros interpreter (see numbertext.org)
 * 2018 (c) László Németh
 * License: LGPL/BSD dual license */

#ifndef NUMBERTEXT_HXX
#define NUMBERTEXT_HXX

#include "NumberTextAPI.hxx"
#include "Soros.hxx"
#include <unordered_map>

class NUMBER_TEXT_API Numbertext
{
public:
    Numbertext();
    void set_prefix(const std::string& st)
    {
        prefix = st;
    }
    void set_prefix(const std::wstring& st)
    {
        prefix = wstring2string(st);
    }
    bool load(std::string lang, std::string filename = "");
    bool load(std::wstring lang, std::wstring filename = L"");
    bool good() const;
    bool numbertext(std::wstring& number, const std::string& lang);
    bool numbertext(std::wstring& number, const std::wstring& lang);
    // UTF-8 encoded input
    bool numbertext(std::string& number, const std::string& lang);
    std::string numbertext(int number, const std::string& lang);
    std::wstring numbertext(int number, const std::wstring& lang);
    static std::wstring string2wstring(const std::string& s);
    static std::string wstring2string(const std::wstring& s);

private:
    std::string prefix;
    std::unordered_map<std::string, Soros> modules;
};

#endif
