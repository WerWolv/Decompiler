#pragma once

#include <dc.hpp>
#include <helpers/static_string.hpp>
#include <helpers/utils.hpp>

#include <numeric>

namespace dc::hlp {

    template<StaticString Pattern>
    class BitPattern {
    public:

        [[maybe_unused]]
        consteval static size_t getPlaceholderCount() {
            std::array<bool, 256> placeholders = { };

            for (char c : Pattern) {
                if (isLower(c) || isUpper(c))
                    placeholders[c] = true;
            }

            return std::accumulate(placeholders.begin(), placeholders.end(), 0);
        }

        consteval static size_t getBitCount() {
            return std::count_if(Pattern.begin(), Pattern.end(), shouldConsiderCharacter);
        }

        consteval static size_t getByteCount() { return getBitCount() / 8; }

        [[nodiscard]]
        [[maybe_unused]]
        constexpr static bool matches(const auto &container) {
            constexpr size_t Size = getByteCount();
            constexpr auto BitMask = getBitMask();
            constexpr auto CompareValues = getBitCompareValues();

            if (container.size() < Size) return false;
            if (sizeof(container[0]) != sizeof(uint8_t)) return false;

            for (uint32_t i = 0; i < Size; i++) {
                if ((container[i] & BitMask[i]) != CompareValues[i])
                    return false;
            }

            return true;
        }

        template<char Placeholder>
        [[nodiscard]] [[maybe_unused]] constexpr static auto getPlaceholderValue(const auto &container) {
            static_assert(isLower(Placeholder) || isUpper(Placeholder), "Invalid placeholder");

            uint64_t result = 0x00;

            uint32_t pos = 0;
            for (char c : Pattern) {
                if (c == Placeholder) {
                    result <<= 1;
                    result |= (container[pos / 8] & (0x80 >> (pos % 8))) != 0;
                }

                if (shouldConsiderCharacter(c))
                    pos++;
            }

            return result;
        }

    private:
        consteval static bool placeholdersValid() {
            bool hasLowerCasePlaceholders = std::any_of(Pattern.begin(), Pattern.end(), isLower);
            bool hasUpperCasePlaceholders = std::any_of(Pattern.begin(), Pattern.end(), isUpper);

            return !(hasLowerCasePlaceholders && hasUpperCasePlaceholders);
        }

        consteval static bool patternValid() {
            return std::all_of(Pattern.begin(), Pattern.end(), [](char c) {
                return
                        c == '0'   || c == '1' ||
                        c == '\''  || c == ' ' ||
                        isLower(c) || isUpper(c) ||
                        c == 0x00;
            });
        }

        constexpr static bool shouldConsiderCharacter(char c) {
            return c == '0' || c == '1' || isLower(c) || isUpper(c);
        }

        constexpr static auto getBitMask() {
            std::array<uint8_t, getByteCount()> result = { };

            uint32_t pos = 0;
            for (char c : Pattern) {
                if (c == '0' || c == '1') {
                    result[pos / 8] <<= 1;
                    result[pos / 8] |= 1;
                }

                if (shouldConsiderCharacter(c))
                    pos++;
            }

            return result;
        }

        constexpr static auto getBitCompareValues() {
            std::array<uint8_t, getByteCount()> result = { };

            uint32_t pos = 0;
            for (char c : Pattern) {
                if (c == '0' || c == '1')
                    result[pos / 8] <<= 1;
                if (c == '1')
                    result[pos / 8] |= 1;

                if (shouldConsiderCharacter(c))
                    pos++;
            }

            return result;
        }

        static_assert(placeholdersValid(), "Can't have both upper and lower case placeholder characters!");
        static_assert(patternValid(), "Invalid characters in pattern! Allowed are 0, 1, ', <space>, a-z and A-Z.");
        static_assert((getBitCount() % 8) == 0 && getBitCount() != 0, "Invalid pattern size. Pattern needs to consist of a multiple of 8 bits.");
    };

}