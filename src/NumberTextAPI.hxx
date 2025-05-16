#pragma once

#if defined(_NUMBER_TEXT_EXPORTS)
#  define NUMBER_TEXT_API __declspec(dllexport)
#elif (defined(_WIN32) && defined(_NUMBER_TEXT_IMPORTS))
#  define NUMBER_TEXT_API __declspec(dllimport)
#else
#  define NUMBER_TEXT_API
#endif
