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

/// @brief Removes common French diacritics from a UTF-8 string.
/// @details Maps accented characters to their ASCII equivalents:
///   é è ê ë → e, à â ä → a, î ï → i, ô ö → o, ù û ü → u, ç → c, ÿ → y, œ → oe, æ → ae
/// @param input UTF-8 encoded string.
/// @return ASCII string with diacritics stripped.
[[nodiscard]] inline std::string remove_diacritics(const std::string& input) {
    std::string result;
    result.reserve(input.size());

    for (size_t i = 0; i < input.size(); ++i) {
        auto byte = static_cast<unsigned char>(input[i]);

        // Two-byte UTF-8 sequences (0xC0-0xDF prefix)
        if (byte >= 0xC0 && byte <= 0xDF && i + 1 < input.size()) {
            auto next = static_cast<unsigned char>(input[i + 1]);
            // Decode the code point
            uint32_t cp = ((byte & 0x1F) << 6) | (next & 0x3F);
            ++i; // consume second byte

            switch (cp) {
                // Latin lowercase
                case 0xE0: case 0xE1: case 0xE2: case 0xE3: case 0xE4: // à á â ã ä
                    result += 'a'; break;
                case 0xE7: // ç
                    result += 'c'; break;
                case 0xE8: case 0xE9: case 0xEA: case 0xEB: // è é ê ë
                    result += 'e'; break;
                case 0xEC: case 0xED: case 0xEE: case 0xEF: // ì í î ï
                    result += 'i'; break;
                case 0xF0: case 0xF1: case 0xF2: case 0xF3: case 0xF4: // ð ñ ò ó ô
                    if (cp == 0xF1) result += 'n';
                    else result += 'o';
                    break;
                case 0xF9: case 0xFA: case 0xFB: case 0xFC: // ù ú û ü
                    result += 'u'; break;
                case 0xFF: // ÿ
                    result += 'y'; break;
                // Latin uppercase
                case 0xC0: case 0xC1: case 0xC2: case 0xC3: case 0xC4: // À Á Â Ã Ä
                    result += 'A'; break;
                case 0xC7: // Ç
                    result += 'C'; break;
                case 0xC8: case 0xC9: case 0xCA: case 0xCB: // È É Ê Ë
                    result += 'E'; break;
                case 0xCC: case 0xCD: case 0xCE: case 0xCF: // Ì Í Î Ï
                    result += 'I'; break;
                case 0xD2: case 0xD3: case 0xD4: // Ò Ó Ô
                    result += 'O'; break;
                case 0xD9: case 0xDA: case 0xDB: case 0xDC: // Ù Ú Û Ü
                    result += 'U'; break;
                // Ligatures (Latin Extended-A)
                case 0x152: // Œ
                    result += "OE"; break;
                case 0x153: // œ
                    result += "oe"; break;
                default:
                    // Pass through other 2-byte chars as-is
                    result += static_cast<char>(byte);
                    result += static_cast<char>(next);
                    break;
            }
        }
        // Three-byte UTF-8 sequences (0xE0-0xEF prefix) — pass through
        else if (byte >= 0xE0 && byte <= 0xEF && i + 2 < input.size()) {
            result += static_cast<char>(byte);
            result += static_cast<char>(input[++i]);
            result += static_cast<char>(input[++i]);
        }
        // Four-byte UTF-8 sequences (0xF0-0xF7 prefix) — pass through
        else if (byte >= 0xF0 && byte <= 0xF7 && i + 3 < input.size()) {
            result += static_cast<char>(byte);
            result += static_cast<char>(input[++i]);
            result += static_cast<char>(input[++i]);
            result += static_cast<char>(input[++i]);
        }
        // ASCII — pass through
        else {
            result += static_cast<char>(byte);
        }
    }

    return result;
}

/// @brief Normalizes a string for search: removes diacritics and lowercases.
[[nodiscard]] inline std::string normalize_for_search(const std::string& input) {
    return to_lower(remove_diacritics(input));
}

}  // namespace city_mapper

