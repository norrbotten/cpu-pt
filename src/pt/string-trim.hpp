#pragma once

#include <algorithm>
#include <cctype>
#include <locale>

// https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring

namespace PT::Util {

/**
 * In-place trims a string from the left
 */
static inline void ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) { return !std::isspace(ch); }));
}

/**
 * In-place trims a string from the right
 */
static inline void rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) { return !std::isspace(ch); }).base(),
            s.end());
}

/**
 * In-place trims a string
 */
static inline void trim(std::string& s) {
    ltrim(s);
    rtrim(s);
}

} // namespace PT::Util