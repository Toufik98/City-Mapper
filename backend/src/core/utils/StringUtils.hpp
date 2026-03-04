#pragma once

/// @file StringUtils.hpp
/// @brief String utility functions for case-insensitive comparison and trimming.

#include <algorithm>
#include <cctype>
#include <string>
#include <string_view>

namespace city_mapper {

/// @brief Case-insensitive string comparison.
/// @param a First string.
/// @param b Second string.
/// @return true if strings are equal ignoring case.
inline bool iequals(std::string_view a, std::string_view b) noexcept {
    if (a.size() != b.size()) {
        return false;
    }
    return std::equal(a.begin(), a.end(), b.begin(),
                      [](unsigned char ca, unsigned char cb) {
                          return std::tolower(ca) == std::tolower(cb);
                      });
}

/// @brief Case-insensitive check if haystack contains needle.
/// @return true if needle is found within haystack (case-insensitive).
inline bool icontains(std::string_view haystack, std::string_view needle) noexcept {
    if (needle.empty()) return true;
    if (haystack.size() < needle.size()) return false;

    auto it = std::search(
        haystack.begin(), haystack.end(),
        needle.begin(), needle.end(),
        [](unsigned char ch1, unsigned char ch2) {
            return std::tolower(ch1) == std::tolower(ch2);
        });

    return it != haystack.end();
}

/// @brief Trims leading and trailing whitespace from a string.
/// @param s The string to trim (modified in place).
/// @return Reference to the trimmed string.
inline std::string& trim(std::string& s) {
    // Trim leading whitespace
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    // Trim trailing whitespace
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
    return s;
}

/// @brief Trims a string and returns a copy.
[[nodiscard]] inline std::string trimmed(std::string s) {
    trim(s);
    return s;
}

/// @brief Converts a string to lowercase.
[[nodiscard]] inline std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

}  // namespace city_mapper
