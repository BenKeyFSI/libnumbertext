#include "Numbertext.hxx"
#include "numbertext-version.h"
#include <cstring>

#if (defined(_MSC_VER) && defined(_MSC_FULL_VER) && defined(_WIN32))

#  pragma push_macro("DATADIR")
#  undef DATADIR
#  if defined(PSAPI_VERSION)
#    undef PSAPI_VERSION
#  endif
#  define PSAPI_VERSION 2
#  include <windows.h>
#  include <array>
#  include <filesystem>
#  include <string>

std::wstring GetLibNumberTextDataDirectory()
{
    constexpr DWORD PathLen{512};
    std::array<wchar_t, PathLen> moduleFileName{};
    const auto result{::GetModuleFileNameW(nullptr, moduleFileName.data(), PathLen)};
    if (result == 0)
    {
        return {};
    }
    auto moduleFileNamePath{std::filesystem::path{moduleFileName.data()}};
    auto moduleDirectory{moduleFileNamePath.parent_path()};
    std::array<std::wstring, 3> candidates{L"data", L"../data", L"../../data"};
    for (const auto& candidate : candidates)
    {
        auto candidatePath{moduleDirectory / candidate};
        if (std::filesystem::exists(candidatePath))
        {
            const auto canonicalPath{std::filesystem::canonical(candidatePath)};
            const auto canonicalPathString{canonicalPath.wstring()};
            return canonicalPathString + std::filesystem::path::preferred_separator;
        }
    }
    return {};
}

#  pragma pop_macro("DATADIR")

#endif

#ifdef HAVE_BOOST_REGEX
  using namespace boost;
#else
  using namespace std;
#endif

#define LANG "LANG"
#define PATH "NUMBERTEXTPATH"
#define DEFPATH DATADIR "/libnumbertext/"
#define DEFPATH2 "data/"

enum State { base, loaded, flag_lang, flag_prefix};

void error()
{
    std::cerr << "spellout: missing language module" << std::endl;
    std::exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
    if (argc == 1) {
        std::cout << "spellout " NUMBERTEXT_VERSION ": convert numbers to number names and money amounts" << std::endl;
        std::cout << "Usage: spellout [-l lang] [-p prefix] par1 [par2...]" << std::endl;
        std::cout << "Parameter: n: number; n-m: range; n-m~s: range with step" << std::endl;
        std::cout << "Examples: spellout 1-10 500 1000-10000~1000" << std::endl;
        std::cout << "          spellout -l en-GB -p ordinal 1-100" << std::endl;
        std::cout << "          spellout -l en -p ordinal-number 1-100" << std::endl;
        std::cout << "          spellout -l en -p USD 100.45" << std::endl;
        std::cout << "          spellout -l en -p \"money USD\" 100.45" << std::endl;
        std::cout << "Help of language module: spellout -l es help" << std::endl;
        std::cout << "License: GNU LGPL/BSD dual-license\n";
        return 0;
    }
    std::vector <std::string> paths;
    paths.emplace_back("");
#if (defined(_MSC_VER) && defined(_MSC_FULL_VER) && defined(_WIN32))
    auto libNumberTextDataDirectory{GetLibNumberTextDataDirectory()};
    if (!libNumberTextDataDirectory.empty())
    {
        paths.push_back(Numbertext::wstring2string(libNumberTextDataDirectory));
    }
#endif
    paths.emplace_back(DEFPATH);
    paths.emplace_back(DEFPATH2);

    if (getenv(PATH))
        paths.insert(paths.begin() + 1, std::string(getenv(PATH)) + "/");
    std::string lang;

    Numbertext nt;
    State state = State::base;
    std::string prefix;
    for (int i = 1; i < argc; i++)
    {
        if (state == State::flag_lang || state == State::flag_prefix)
        {
            if (state == State::flag_lang)
            {
                lang = argv[i];
            }
            else
            {
                prefix = argv[i];
                prefix += " ";
            }
            state = State::base;
            continue;
        }
        if (strcmp(argv[i], "-l") == 0)
        {
            state = State::flag_lang;
        }
        else if (strcmp(argv[i], "-p") == 0)
        {
            state = State::flag_prefix;
        }
        else
        {
            if (lang.empty()) {
                if (getenv(LANG)) {
                    lang = std::string(getenv(LANG));
                    lang = lang.substr(0, lang.find("."));
                }
                if (lang.empty())
                    lang = "en";
            }

            if (state != State::loaded) {
                for(auto const& path: paths) {
                    nt.set_prefix(path);
                    if (nt.load(lang))
                        break;
                }
                state = State::loaded;
            }

            std::string arg = argv[i];
            smatch n;
            if (regex_match(arg, n, regex("([0-9]+)-([0-9]+)~?([0-9]+)?")))
            {
                long long b = std::stoll(n[1].str());
                long long end = std::stoll(n[2].str());
                long long step = (n[3].length() == 0) ? 1 : std::stoll(n[3].str());
                for (long long j = b; j <= end; j = j + step) {
                    std::string result = prefix + std::to_string(j);
                    if (!nt.numbertext(result, lang))
                        error();
                    std::cout << result << std::endl;
                }
            }
            else
            {
                std::string result = prefix + arg;
                if (!nt.numbertext(result, lang))
                    error();
                std::cout << result << std::endl;
            }
        }
    }
}
