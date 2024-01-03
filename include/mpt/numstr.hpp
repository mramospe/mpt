#pragma once
#include "mpt/types.hpp"
#include <cstdint>
#include <cmath>
#include <iostream>
#include <variant>
#include <stdexcept>
#include <string>
#include <string_view>

namespace mpt {

    using arithmetic_types = mpt::types<bool, int, unsigned int, long int, unsigned long int, long long int, unsigned long long int, float, double, long double>;

    using arithmetic_var = mpt::specialize_template_from_types_t<std::variant, arithmetic_types>;

    namespace {

        using io_bit_state = long unsigned int;

        /** @brief Identifier for a state that has just been built
         * 
         * The lowest part corresponds to the data type, next bit refers to
         * whether the value is signed or not (only valid for integral types),
         * the third refers to whether the value should be considered positive
         * or negative, the fourth refers to how many times the precision
         * is doubled. By default only the bit corresponding to the sign is set,
         * specifying that the value is positive.
        */
        static constexpr io_bit_state unspecified_value_state = 0000u;

        template<class EnumType>
        struct bit_helper_t;

        enum data_type_enum : io_bit_state { integral=0x0u, floating_point=0x1u };

        template<>
        struct bit_helper_t<data_type_enum> {
            static constexpr io_bit_state mask = 0xfu;
            static constexpr unsigned char offset = 0u;
        };

        enum precision_enum : io_bit_state { normal_value=0x0u, long_value=0x1u, long_long_value=0x2u, single_precision_floating_point=0x3u };

        template<>
        struct bit_helper_t<precision_enum> {
            static constexpr io_bit_state mask = 0xf0u;
            static constexpr unsigned char offset = 4u;
        };

        enum integral_sign_enum : io_bit_state { signed_integral=0x0u, unsigned_integral=0x1u };

        template<>
        struct bit_helper_t<integral_sign_enum> {
            static constexpr io_bit_state mask = 0xf00u;
            static constexpr unsigned char offset = 8u;
        };

        enum value_sign_enum : io_bit_state { positive=0x0u, negative=0x1u };

        template<>
        struct bit_helper_t<value_sign_enum> {
            static constexpr io_bit_state mask = 0xf000u;
            static constexpr unsigned char offset = 12u;
        };

        enum exponent_sign_enum : io_bit_state { positive_exponent=0x0u, negative_exponent=0x1u };

        template<>
        struct bit_helper_t<exponent_sign_enum> {
            static constexpr io_bit_state mask = 0xf0000u;
            static constexpr unsigned char offset = 16u;
        };

        struct exponent_value {
            using value_type = int;
        };

        template<>
        struct bit_helper_t<exponent_value> {
            static constexpr io_bit_state omit = 0xfffffu; // maximum value for "long double" is 4932
            static constexpr unsigned char offset = 20u;
        };

        template<class T>
        io_bit_state set_bit(io_bit_state state, io_bit_state value) {
            return (value << bit_helper_t<T>::offset) | (state & ~bit_helper_t<T>::mask);
        }

        template<class T>
        io_bit_state get_bit(io_bit_state state) {
            return (state & bit_helper_t<T>::mask) >> bit_helper_t<T>::offset;
        }

        template<class T>
        io_bit_state is_bit(io_bit_state state, io_bit_state value) {
            return get_bit<T>(state) == value;
        }

        io_bit_state set_integral(io_bit_state state) {
            return set_bit<data_type_enum>(state, data_type_enum::integral);
        }

        bool is_integral(io_bit_state state) {
            return is_bit<data_type_enum>(state, data_type_enum::integral);
        }

        io_bit_state set_floating_point(io_bit_state state) {
            return set_bit<data_type_enum>(state, data_type_enum::floating_point);
        }

        bool is_floating_point(io_bit_state state) {
            return is_bit<data_type_enum>(state, data_type_enum::floating_point);
        }

        io_bit_state set_unsigned(io_bit_state state) {
            return set_bit<integral_sign_enum>(state, integral_sign_enum::unsigned_integral);
        }

        bool is_unsigned(io_bit_state state) {
            return is_bit<integral_sign_enum>(state, integral_sign_enum::unsigned_integral);
        }

        io_bit_state set_positive(io_bit_state state) {
            return set_bit<value_sign_enum>(state, value_sign_enum::positive);
        }

        io_bit_state set_negative(io_bit_state state) {
            return set_bit<value_sign_enum>(state, value_sign_enum::negative);
        }

        bool is_negative(io_bit_state state) {
            return is_bit<value_sign_enum>(state, value_sign_enum::negative);
        }

        io_bit_state set_long(io_bit_state state) {
            return set_bit<precision_enum>(state, precision_enum::long_value);
        }

        io_bit_state set_long_long(io_bit_state state) {
            return set_bit<precision_enum>(state, precision_enum::long_long_value);
        }

        bool is_precision_set(io_bit_state state) {
            return !is_bit<precision_enum>(state, precision_enum::normal_value);
        }

        io_bit_state set_single_precision_floating_point(io_bit_state state) {
            return set_bit<precision_enum>(set_floating_point(state), precision_enum::single_precision_floating_point);
        }

        io_bit_state set_exponent_positive(io_bit_state state) {
            return set_bit<exponent_sign_enum>(state, exponent_sign_enum::positive_exponent);
        }

        io_bit_state set_exponent_negative(io_bit_state state) {
            return set_bit<exponent_sign_enum>(state, exponent_sign_enum::negative_exponent);
        }

        io_bit_state set_exponent_value(io_bit_state state, io_bit_state value) {
            return (state & bit_helper_t<exponent_value>::omit) | (value << bit_helper_t<exponent_value>::offset);
        }

        auto get_exponent(io_bit_state state) {
            return (is_bit<exponent_sign_enum>(state, exponent_sign_enum::positive_exponent) ? +1 : -1) * exponent_value::value_type((state ^ bit_helper_t<exponent_value>::omit) >> bit_helper_t<exponent_value>::offset );
        }

        template<class IntegralType, class Iterator>
        IntegralType parse_raw_sanitized_integral(Iterator begin, Iterator it) {
            using value_type = IntegralType;
            static constexpr auto base = value_type('0'); // assume sorted values [0, 9]
            value_type value = 0;
            do {
                --it;
                value = 10 * value + (value_type(*it) - base);
            } while ( it != begin );
            return value;
        }

        template<class Iterator>
        arithmetic_var integral_value_impl(io_bit_state state, Iterator begin, Iterator end) {
            if ( is_bit<integral_sign_enum>(state, integral_sign_enum::signed_integral) ) {
                if ( is_bit<precision_enum>(state, precision_enum::long_long_value) )
                    return parse_raw_sanitized_integral<long long int>(begin, end);
                else if ( is_bit<precision_enum>(state, precision_enum::long_value) )
                    return parse_raw_sanitized_integral<long int>(begin, end);
                else
                    return parse_raw_sanitized_integral<int>(begin, end);
            }
            else { // unsigned
                if ( is_bit<precision_enum>(state, precision_enum::long_long_value) )
                    return parse_raw_sanitized_integral<unsigned long long int>(begin, end);
                else if ( is_bit<precision_enum>(state, precision_enum::long_value) )
                    return parse_raw_sanitized_integral<unsigned long int>(begin, end);
                else
                    return parse_raw_sanitized_integral<unsigned int>(begin, end);
            }
        }

        template<class FloatType, class Iterator>
        arithmetic_var parse_floating_point_value(io_bit_state state, Iterator begin, Iterator end) {

            using value_type = FloatType;

            static constexpr value_type base = int('0'); // assume sorted values [0, 9]

            value_type value{0};

            int exponent = get_exponent(state);

            if ( exponent > std::numeric_limits<FloatType>::max_exponent10 )
                throw std::runtime_error("Exponent is larger than the maximum allowed for the given precision");

            bool decimal = false;
            for ( auto it = begin; it != end; ++it ) {

                if ( std::isdigit(*it) ) {
                    value = 10 * value + value_type(int(*it) - base);
                    if ( decimal )
                        --exponent;
                }
                else // is a colon
                    decimal = true;
            }

            if ( is_negative(state) )
                value *= value_type{-1};

            return exponent != 0 ? value : value * std::pow(value_type{10}, value_type(exponent));
        }

        template<class Iterator>
        arithmetic_var floating_point_value_impl(io_bit_state state, Iterator begin, Iterator end) {
            if ( is_bit<precision_enum>(state, precision_enum::single_precision_floating_point) )
                return parse_floating_point_value<float>(state, begin, end);
            else if ( is_bit<precision_enum>(state, precision_enum::long_value) )
                return parse_floating_point_value<double>(state, begin, end);
            else // long double
                return parse_floating_point_value<long double>(state, begin, end);
        }
    }

    auto arithmetic_value_from_str(std::string_view str) {

        auto it = std::cbegin(str);
        auto end = std::cend(str);

        // determine the sign of the value
        auto state = unspecified_value_state;
        while ( it != end ) {
            if ( std::isspace(*it) )
                ++it;
            else if ( *it == '-' ) {
                state = is_negative(state) ? set_positive(state) : set_negative(state);
                ++it;
            }
            else if ( *it == '+' ) {
                ++it;
            }
            else
                break;
        }
        
        if ( it == end )
            throw std::runtime_error("Missing number");

        // the actual start of the number
        auto start = it;

        // determine the point at which the number without suffixes ends
        auto raw_number_end = [&]() {
            while ( it != end ) {
                if ( std::isdigit(*it) )
                    ++it;
                else if ( *it == '.' ) {
                    if ( is_integral(state) )
                        state = set_floating_point(state);
                    else
                        throw std::runtime_error("Repeated colon in floating-point value");

                    ++it;
                }
                else
                    return it;
            }
            return it;
        }();

        // parse the suffix
        if ( *it == 'e' || *it == 'E' ) {
            // checking whether it is a floating-point value and setting/omitting
            // it is as computationally expensive as setting it straight away
            state = set_floating_point(state);
            ++it;
            if ( it == end )
                throw std::runtime_error("Exponent has no digits");
            else {
                // parse the exponent sign and value
                state = [&]() {
                    if ( *it == '-' ) {
                        ++it;
                        if ( !std::isdigit(*it) )
                            throw std::runtime_error("Exponent has no digits");
                        else
                            return set_exponent_negative(state);
                    }
                    else if ( *it == '+' ){
                        ++it;
                        if ( !std::isdigit(*it) )
                            throw std::runtime_error("Exponent has no digits");
                        else
                            return set_exponent_positive(state);
                    }
                    else if ( std::isdigit(*it) )
                        return set_exponent_positive(state);
                    else
                        throw std::runtime_error("Unrecognized exponent");
                }();

                auto exponent_begin_it = it;
                do { ++it; } while ( it != end && std::isdigit(*it) );
                state = set_exponent_value(state, parse_raw_sanitized_integral<io_bit_state>(exponent_begin_it, it));
            }
        }

        while ( it != end ) {
            if ( *it == 'e' || *it == 'E' ) {
                if ( is_integral(state) )
                    throw std::runtime_error("Invalid use of scientific notation in integral value type");
                else
                    throw std::runtime_error("Repeated scientific floating-point specifier");
            }
            else if ( *it == '.' ) {
                if ( is_integral(state) )
                    throw std::runtime_error("Invalid use of colon in integral value type");
                else
                    throw std::runtime_error("Invalid use of colon in the suffix of a floating-point type");
            }
            else if ( *it == 'u' ) {

                if ( is_floating_point(state) )
                    throw std::runtime_error("Invalid use of the \"u\" suffix in floating-point type");
                else
                    state = set_integral(state);

                if ( is_unsigned(*it) )
                    throw std::runtime_error("Repeated \"u\" specifier in suffix");
                else
                    state = set_unsigned(state);

                ++it;
            }
            else if ( *it == 'l' ) {
                if ( is_precision_set(state) )
                    throw std::runtime_error("Additional use of the \"l\" specifier");
                else if ( is_floating_point(state) ) {
                    state = set_long_long(state); // by default values are already of double-precision
                    ++it;
                }
                else {
                    state = set_long(state);
                    ++it;
                    if ( it != end && *it == 'l' ) {
                        state = set_long_long(state);
                        ++it;
                    }
                }
            }
            else if ( *it == 'f' ) {
                if ( is_precision_set(state) )
                    throw std::runtime_error("Invalid use of the single-precision floating-point specifier");
                else
                    state = set_single_precision_floating_point(state);

                ++it;
            }
            else
                break;
        }

        // If the precision has not been set and is a floating-point
        // number then we must set it as "double" (the default)
        if ( !is_precision_set(state) && is_floating_point(state) )
            state = set_long(state);

        if ( is_integral(state) )
            return integral_value_impl(state, start, raw_number_end);
        else
            return floating_point_value_impl(state, start, raw_number_end);
    }

    auto arithmetic_value_from_str(std::string const& str) {
        return arithmetic_value_from_str(std::string_view{str});
    }

    auto arithmetic_value_from_str(const char* ch) {
        return arithmetic_value_from_str(std::string_view{ch});
    }
}