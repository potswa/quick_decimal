// to_decimal.h - No-frills number serialization.
// 2017-12-24 David Krauss (Potatoswatter). This is released under the MIT license.

// So far, this library supports serializing 32-bit unsigned integers on 64-bit hardware.
// (At least, hardware with fast 64-bit addition. It may yet work well on some 32-bit CPUs.)

#ifndef S6_QUICK_DECIMAL
#define S6_QUICK_DECIMAL

#if __cplusplus
#   include <cstdint>
#   if __cplusplus >= 201103
#       define CONSTDECL constexpr
#   else
#       define CONSTDECL const
#   endif

namespace s6_quick_decimal {
using std::uint64_t;
using std::uint32_t;

#else
#   include <stdint.h>
#   define CONSTDECL const
#endif

inline char * to_decimal_u32( uint32_t n, char * out ) {
    static CONSTDECL uint64_t unit = ((uint64_t)1) << 60;
    static CONSTDECL uint64_t prescale_base = unit / ((uint64_t)1000000000);

    static CONSTDECL struct initial {
        int digits;
        uint64_t prescale;
    } start[33] = { // Predict the number of decimal digits given (number of bits / 2).
        { 10, prescale_base }, // Exactly 2^32-1
        { 10, prescale_base }, // Up to 4B
        { 10, prescale_base }, // Up to 2B
        { 10, prescale_base }, // Up to 1B
        { 9, prescale_base * 10 }, // Up to 512M
        { 9, prescale_base * 10 }, // Up to 256M
        { 9, prescale_base * 10 }, // Up to 128M
        { 8, prescale_base * 100 }, // Up to 64M
        { 8, prescale_base * 100 }, // Up to 32M
        { 8, prescale_base * 100 }, // Up to 16M
        { 7, prescale_base * 1000 }, // Up to 8M
        { 7, prescale_base * 1000 }, // Up to 4M
        { 7, prescale_base * 1000 }, // Up to 2M
        { 7, prescale_base * 1000 }, // Up to 1M
        { 6, prescale_base * 10000 }, // Up to 256k
        { 6, prescale_base * 10000 }, // Up to 256k
        { 6, prescale_base * 10000 }, // Up to 128k
        { 5, prescale_base * 100000 }, // Up to 64k
        { 5, prescale_base * 100000 }, // Up to 32k
        { 5, prescale_base * 100000 }, // Up to 16k
        { 4, prescale_base * 1000000 }, // Up to 8k
        { 4, prescale_base * 1000000 }, // Up to 4k
        { 4, prescale_base * 1000000 }, // Up to 2k
        { 4, prescale_base * 1000000 }, // Up to 1k
        { 3, prescale_base * 10000000 }, // Up to 510
        { 3, prescale_base * 10000000 }, // Up to 254
        { 3, prescale_base * 10000000 }, // Up to 126
        { 2, prescale_base * 100000000 }, // Up to 62
        { 2, prescale_base * 100000000 }, // Up to 30
        { 2, prescale_base * 100000000 }, // Up to 14
        { 1, prescale_base * 1000000000 }, // Up to 6
        { 1, prescale_base * 1000000000 }, // Up to 2
        { 2, 0 } // Treat zero as a two-digit number. Only the first zero is dropped.
    };
    
    uint64_t frac = n;
    ++ frac;
    int order = __builtin_clzll( frac ) - 31;
    struct initial start_state = start[ order ];
    frac *= start_state.prescale;
    
    if ( frac >= unit ) {
        /*  For the particular case of n > 10^9, n * prescale may be off by more than 1.
            Include one more bit of precision here to fix the last digit.
            This only affects large numbers, but testing "if (order > 2)" would only add more complexity than would save.
        */
        frac += n / 2;
        * out ++ = '0' + frac / unit;
        frac %= unit;
    }
    int i = start_state.digits;
    while ( -- i ) {
        frac *= 10;
        * out ++ = '0' + frac / unit;
        frac %= unit;
    }
    return out;
}

#if __cplusplus

// Overloading for C++ convenience and future extensibility.
inline char * to_decimal( uint32_t n, char * out ) {
    return to_decimal_u32( n, out );
}

} // end namespace
#endif

#endif
