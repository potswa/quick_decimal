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
    #define unit (((uint64_t)1) << 60)
    #define EXP10(N) ((uint64_t)1.e ## N)
    #define SCALE(N) { N, unit / EXP10(N) - unit % (EXP10(9)/EXP10(N)) }

    static CONSTDECL struct initial {
        int digits;
        uint64_t prescale;
    } start[33] = { // Predict the number of decimal digits given the number of bits.
        SCALE(9), // Exactly 2^32-1
        SCALE(9), // Up to 4B
        SCALE(9), // Up to 2B
        SCALE(9), // Up to 1B
        SCALE(8), // Up to 512M
        SCALE(8), // Up to 256M
        SCALE(8), // Up to 128M
        SCALE(7), // Up to 64M
        SCALE(7), // Up to 32M
        SCALE(7), // Up to 16M
        SCALE(6), // Up to 8M
        SCALE(6), // Up to 4M
        SCALE(6), // Up to 2M
        SCALE(6), // Up to 1M
        SCALE(5), // Up to 256k
        SCALE(5), // Up to 256k
        SCALE(5), // Up to 128k
        SCALE(4), // Up to 64k
        SCALE(4), // Up to 32k
        SCALE(4), // Up to 16k
        SCALE(3), // Up to 8k
        SCALE(3), // Up to 4k
        SCALE(3), // Up to 2k
        SCALE(3), // Up to 1k
        SCALE(2), // Up to 510
        SCALE(2), // Up to 254
        SCALE(2), // Up to 126
        SCALE(1), // Up to 62
        SCALE(1), // Up to 30
        SCALE(1), // Up to 14
        SCALE(0), // Up to 6
        SCALE(0), // Up to 2
        { 1, 0 } // Print zero as one digit.
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
    char * ret = out + i;
    while ( i -- ) {
        frac *= 10;
        * out ++ = '0' + frac / unit;
        frac %= unit;
    }
    return ret;
    
    #undef unit
    #undef EXP10
    #undef SCALE
}

#if __cplusplus

// Overloading for C++ convenience and future extensibility.
inline char * to_decimal( uint32_t n, char * out ) {
    return to_decimal_u32( n, out );
}

} // end namespace
#endif

#endif
