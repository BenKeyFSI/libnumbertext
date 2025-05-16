/* Soros interpreter (see numbertext.org)
 * 2018 (c) László Németh
 * License: LGPL/BSD dual license */

#ifndef SOROS_HXX_
#define SOROS_HXX_

#include <iostream>
#include <iterator>
#include <string>

#include "NumberTextAPI.hxx"
#include "numbertext-version.h"

#ifdef NUMBERTEXT_BOOST
  #include <boost/regex.hpp>
#else
  #include <regex>
#endif

class Soros {

public:
    NUMBER_TEXT_API Soros(std::wstring program, std::wstring filtered_lang);
    NUMBER_TEXT_API int run(std::wstring& input);
    NUMBER_TEXT_API static std::wstring translate(std::wstring s,
                std::wstring chars, const std::wstring& chars2, const std::wstring& delim);
private:
    void run(std::wstring& input, int& level, bool begin = true, bool end = true);
    static void replace(std::wstring& s, const std::wstring& search,
                const std::wstring& replace);

#ifdef NUMBERTEXT_BOOST
    std::vector<boost::wregex> patterns;
    static const boost::wregex func;
#else
    std::vector<std::wregex> patterns;
    static const std::wregex func;
#endif
    std::vector<std::wstring> values;
    std::vector<bool> begins;
    std::vector<bool> ends;

    static const std::wstring m;
    static const std::wstring m2;
    static const std::wstring c;
    static const std::wstring c2;
    static const std::wstring slash;
    static const std::wstring pipe;
};

#endif
