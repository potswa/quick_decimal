// to_decimal.h - No-frills number serialization.
// 2017-12-24 David Krauss (Potatoswatter). This is released under the MIT license.

// So far, this library supports serializing 32-bit unsigned integers on 64-bit hardware.
// (At least, hardware with fast 64-bit addition. It may yet work well on some 32-bit CPUs.)

#ifndef S6_QUICK_DECIMAL
#define S6_QUICK_DECIMAL

#if __cplusplus
#   include <cstdint>
#   include <cstring>
#   if __cplusplus >= 201103
#       define CONSTDECL constexpr
#   else
#       define CONSTDECL const
#   endif

namespace s6_quick_decimal {
using std::uint64_t;
using std::uint32_t;
using std::memcpy;

#else
#   include <stdint.h>
#   include <string.h>
#   define CONSTDECL const
#endif

inline char * to_decimal_u32( uint32_t n, char * out ) {
    static CONSTDECL uint64_t unit = ((uint64_t)1) << 57;
    static CONSTDECL uint64_t tens = unit * 10;
    static CONSTDECL uint64_t prescale_base = unit / ((uint64_t)100000000);

    static CONSTDECL struct initial {
        int digits;
        uint64_t prescale;
    } start[33] = { // Predict the number of decimal digits given the number of bits.
        { 10-2, prescale_base }, // Exactly 2^32-2
        { 10-2, prescale_base }, // Up to 4B
        { 10-2, prescale_base }, // Up to 2B
        { 10-2, prescale_base }, // Up to 1B
        { 9-2, prescale_base * 10 }, // Up to 512M
        { 9-2, prescale_base * 10 }, // Up to 256M
        { 9-2, prescale_base * 10 }, // Up to 128M
        { 8-2, prescale_base * 100 }, // Up to 64M
        { 8-2, prescale_base * 100 }, // Up to 32M
        { 8-2, prescale_base * 100 }, // Up to 16M
        { 7-2, prescale_base * 1000 }, // Up to 8M
        { 7-2, prescale_base * 1000 }, // Up to 4M
        { 7-2, prescale_base * 1000 }, // Up to 2M
        { 7-2, prescale_base * 1000 }, // Up to 1M
        { 6-2, prescale_base * 10000 }, // Up to 256k
        { 6-2, prescale_base * 10000 }, // Up to 256k
        { 6-2, prescale_base * 10000 }, // Up to 128k
        { 5-2, prescale_base * 100000 }, // Up to 64k
        { 5-2, prescale_base * 100000 }, // Up to 32k
        { 5-2, prescale_base * 100000 }, // Up to 16k
        { 4-2, prescale_base * 1000000 }, // Up to 8k
        { 4-2, prescale_base * 1000000 }, // Up to 4k
        { 4-2, prescale_base * 1000000 }, // Up to 2k
        { 4-2, prescale_base * 1000000 }, // Up to 1k
        { 3-2, prescale_base * 10000000 }, // Up to 510
        { 3-2, prescale_base * 10000000 }, // Up to 254
        { 3-2, prescale_base * 10000000 }, // Up to 126
        { 2-2, prescale_base * 100000000 }, // Up to 62
        { 2-2, prescale_base * 100000000 }, // Up to 30
        { 2-2, prescale_base * 100000000 }, // Up to 14
        { 2-2, prescale_base * 100000000 }, // Up to 6
        { 2-2, prescale_base * 100000000 }, // Up to 2
        { 2-2, 0 } // Zero
    };
    
    typedef char const str2[2];
    static CONSTDECL str2 digitpair[100] = {
        '0','0', '0','1', '0','2', '0','3', '0','4', '0','5', '0','6', '0','7', '0','8', '0','9',
        '1','0', '1','1', '1','2', '1','3', '1','4', '1','5', '1','6', '1','7', '1','8', '1','9',
        '2','0', '2','1', '2','2', '2','3', '2','4', '2','5', '2','6', '2','7', '2','8', '2','9',
        '3','0', '3','1', '3','2', '3','3', '3','4', '3','5', '3','6', '3','7', '3','8', '3','9',
        '4','0', '4','1', '4','2', '4','3', '4','4', '4','5', '4','6', '4','7', '4','8', '4','9',
        '5','0', '5','1', '5','2', '5','3', '5','4', '5','5', '5','6', '5','7', '5','8', '5','9',
        '6','0', '6','1', '6','2', '6','3', '6','4', '6','5', '6','6', '6','7', '6','8', '6','9',
        '7','0', '7','1', '7','2', '7','3', '7','4', '7','5', '7','6', '7','7', '7','8', '7','9',
        '8','0', '8','1', '8','2', '8','3', '8','4', '8','5', '8','6', '8','7', '8','8', '8','9',
        '9','0', '9','1', '9','2', '9','3', '9','4', '9','5', '9','6', '9','7', '9','8', '9','9'
    };
    
    uint64_t frac = n;
    ++ frac;
    int order = __builtin_clzll( frac ) - 31;
    struct initial start_state = start[ order ];
    frac *= start_state.prescale;
    
    if ( frac < tens ) {
        * out ++ = '0' + frac / unit;
    } else {
        frac += n / 2;
        str2 * outpen = digitpair + frac / unit;
        memcpy( out, *outpen, 2 );
        out += 2;
    }
    
    int i = start_state.digits;
    char * ret = out + i;
    while ( i > 1 ) {
        frac %= unit;
        frac *= 100;
        str2 * outpen = digitpair + frac / unit;
        memcpy( out, *outpen, 2 );
        out += 2;
        i -= 2;
    }
    if ( i ) {
        frac %= unit;
        frac *= 10;
        * out ++ = '0' + frac / unit;
    }
    return ret;
}

#if __cplusplus

// Overloading for C++ convenience and future extensibility.
inline char * to_decimal( uint32_t n, char * out ) {
    return to_decimal_u32( n, out );
}

} // end namespace
#endif

#endif
