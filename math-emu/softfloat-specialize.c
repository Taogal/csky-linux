
/*
===============================================================================

This C source fragment is part of the SoftFloat IEC/IEEE Floating-point
Arithmetic Package, Release 2.

Written by John R. Hauser.  This work was made possible in part by the
International Computer Science Institute, located at Suite 600, 1947 Center
Street, Berkeley, California 94704.  Funding was partially provided by the
National Science Foundation under grant MIP-9311980.  The original version
of this code was written as part of a project to build a fixed-point vector
processor in collaboration with the University of California at Berkeley,
overseen by Profs. Nelson Morgan and John Wawrzynek.  More information
is available through the Web page
http://www.jhauser.us/arithmetic/SoftFloat-2b/SoftFloat-source.txt

THIS SOFTWARE IS DISTRIBUTED AS IS, FOR FREE.  Although reasonable effort
has been made to avoid it, THIS SOFTWARE MAY CONTAIN FAULTS THAT WILL AT
TIMES RESULT IN INCORRECT BEHAVIOR.  USE OF THIS SOFTWARE IS RESTRICTED TO
PERSONS AND ORGANIZATIONS WHO CAN AND WILL TAKE FULL RESPONSIBILITY FOR ANY
AND ALL LOSSES, COSTS, OR OTHER PROBLEMS ARISING FROM ITS USE.

Derivative works are acceptable, even for commercial purposes, so long as
(1) they include prominent notice that the work is derivative, and (2) they
include prominent notice akin to these three paragraphs for those parts of
this code that are retained.

===============================================================================
*/

/*
-------------------------------------------------------------------------------
Underflow tininess-detection mode, statically initialized to default value.
(The declaration in `softfloat.h' must match the `int8' type here.)
-------------------------------------------------------------------------------
*/
int8 float_detect_tininess = float_tininess_after_rounding;

/*
-------------------------------------------------------------------------------
Raises the exceptions specified by `flags'.  Floating-point traps can be
defined here if desired.  It is currently not possible for such a trap to
substitute a result value.  If traps are not implemented, this routine
should be simply `float_exception_flags |= flags;'.

ScottB:  November 4, 1998
Moved this function out of softfloat-specialize into fpmodule.c.
This effectively isolates all the changes required for integrating with the
Linux kernel into fpmodule.c.  Porting to NetBSD should only require modifying
fpmodule.c to integrate with the NetBSD kernel (I hope!).
-------------------------------------------------------------------------------
void raise_float_exception( int8 flags )
{
    float_exception_flags |= flags;
}
*/

/*
-------------------------------------------------------------------------------
Internal canonical NaN format.
-------------------------------------------------------------------------------
*/
typedef struct {
    flag sign;
    bits64 high, low;
} commonNaNT;

/*
-------------------------------------------------------------------------------
The pattern for a default generated single-precision NaN.
-------------------------------------------------------------------------------
*/
#define float32_default_nan 0xFFFFFFFF

/*
-------------------------------------------------------------------------------
Returns 1 if the single-precision floating-point value `a' is a NaN;
otherwise returns 0.
-------------------------------------------------------------------------------
*/
flag float32_is_nan( float32 a )
{

    return ( 0xFF000000 < (bits32) ( a<<1 ) );

}

/*
-------------------------------------------------------------------------------
Returns 1 if the single-precision floating-point value `a' is a signaling
NaN; otherwise returns 0.
-------------------------------------------------------------------------------
*/
flag float32_is_signaling_nan( float32 a )
{

    return ( ( ( a>>22 ) & 0x1FF ) == 0x1FE ) && ( a & 0x003FFFFF );

}

/*
-------------------------------------------------------------------------------
Returns the result of converting the single-precision floating-point NaN
`a' to the canonical NaN format.  If `a' is a signaling NaN, the invalid
exception is raised.
-------------------------------------------------------------------------------
*/
static commonNaNT float32ToCommonNaN( float32 a )
{
    commonNaNT z;

    if ( float32_is_signaling_nan( a ) ) raise_float_exception( float_flag_invalid );
    z.sign = a>>31;
    z.low = 0;
    z.high = ( (bits64) a )<<41;
    return z;

}

/*
-------------------------------------------------------------------------------
Returns the result of converting the canonical NaN `a' to the single-
precision floating-point format.
-------------------------------------------------------------------------------
*/
static float32 commonNaNToFloat32( commonNaNT a )
{

    return ( ( (bits32) a.sign )<<31 ) | 0x7FC00000 | ( a.high>>41 );

}

/*
-------------------------------------------------------------------------------
Takes two single-precision floating-point values `a' and `b', one of which
is a NaN, and returns the appropriate NaN result.  If either `a' or `b' is a
signaling NaN, the invalid exception is raised.
-------------------------------------------------------------------------------
*/
static float32 propagateFloat32NaN( float32 a, float32 b )
{
    flag aIsNaN, aIsSignalingNaN, bIsNaN, bIsSignalingNaN;

    aIsNaN = float32_is_nan( a );
    aIsSignalingNaN = float32_is_signaling_nan( a );
    bIsNaN = float32_is_nan( b );
    bIsSignalingNaN = float32_is_signaling_nan( b );
    a |= 0x00400000;
    b |= 0x00400000;
    if ( aIsSignalingNaN | bIsSignalingNaN ) raise_float_exception( float_flag_invalid );
    if ( aIsNaN ) {
        return ( aIsSignalingNaN & bIsNaN ) ? b : a;
    }
    else {
        return b;
    }

}

/*
-------------------------------------------------------------------------------
The pattern for a default generated double-precision NaN.
-------------------------------------------------------------------------------
*/
#define float64_default_nan LIT64( 0xFFFFFFFFFFFFFFFF )

/*
-------------------------------------------------------------------------------
Returns 1 if the double-precision floating-point value `a' is a NaN;
otherwise returns 0.
-------------------------------------------------------------------------------
*/
flag float64_is_nan( float64 a )
{

    return ( LIT64( 0xFFE0000000000000 ) < (bits64) ( a<<1 ) );

}

/*
-------------------------------------------------------------------------------
Returns 1 if the double-precision floating-point value `a' is a signaling
NaN; otherwise returns 0.
-------------------------------------------------------------------------------
*/
flag float64_is_signaling_nan( float64 a )
{

    return
           ( ( ( a>>51 ) & 0xFFF ) == 0xFFE )
        && ( a & LIT64( 0x0007FFFFFFFFFFFF ) );

}

/*
-------------------------------------------------------------------------------
Returns the result of converting the double-precision floating-point NaN
`a' to the canonical NaN format.  If `a' is a signaling NaN, the invalid
exception is raised.
-------------------------------------------------------------------------------
*/
static commonNaNT float64ToCommonNaN( float64 a )
{
    commonNaNT z;

    if ( float64_is_signaling_nan( a ) ) raise_float_exception( float_flag_invalid );
    z.sign = a>>63;
    z.low = 0;
    z.high = a<<12;
    return z;

}

/*
-------------------------------------------------------------------------------
Returns the result of converting the canonical NaN `a' to the double-
precision floating-point format.
-------------------------------------------------------------------------------
*/
static float64 commonNaNToFloat64( commonNaNT a )
{

    return
          ( ( (bits64) a.sign )<<63 )
        | LIT64( 0x7FF8000000000000 )
        | ( a.high>>12 );

}

/*
-------------------------------------------------------------------------------
Takes two double-precision floating-point values `a' and `b', one of which
is a NaN, and returns the appropriate NaN result.  If either `a' or `b' is a
signaling NaN, the invalid exception is raised.
-------------------------------------------------------------------------------
*/
static float64 propagateFloat64NaN( float64 a, float64 b )
{
    flag aIsNaN, aIsSignalingNaN, bIsNaN, bIsSignalingNaN;

    aIsNaN = float64_is_nan( a );
    aIsSignalingNaN = float64_is_signaling_nan( a );
    bIsNaN = float64_is_nan( b );
    bIsSignalingNaN = float64_is_signaling_nan( b );
    a |= LIT64( 0x0008000000000000 );
    b |= LIT64( 0x0008000000000000 );
    if ( aIsSignalingNaN | bIsSignalingNaN ) raise_float_exception( float_flag_invalid );
    if ( aIsNaN ) {
        return ( aIsSignalingNaN & bIsNaN ) ? b : a;
    }
    else {
        return b;
    }

}

#if 0

/*
-------------------------------------------------------------------------------
The pattern for a default generated extended double-precision NaN.  The
`high' and `low' values hold the most- and least-significant bits,
respectively.
-------------------------------------------------------------------------------
*/
#define floatx80_default_nan_high 0xFFFF
#define floatx80_default_nan_low  LIT64( 0xFFFFFFFFFFFFFFFF )

/*
-------------------------------------------------------------------------------
Returns 1 if the extended double-precision floating-point value `a' is a
NaN; otherwise returns 0.
-------------------------------------------------------------------------------
*/
flag floatx80_is_nan( floatx80 a )
{

    return ( ( a.high & 0x7FFF ) == 0x7FFF ) && (bits64) ( a.low<<1 );

}

/*
-------------------------------------------------------------------------------
Returns 1 if the extended double-precision floating-point value `a' is a
signaling NaN; otherwise returns 0.
-------------------------------------------------------------------------------
*/
flag floatx80_is_signaling_nan( floatx80 a )
{
    //register int lr;
    bits64 aLow;

    //__asm__("mov %0, lr" : : "g" (lr));
    //fp_printk("floatx80_is_signalling_nan() called from 0x%08x\n",lr);
    aLow = a.low & ~ LIT64( 0x4000000000000000 );
    return
           ( ( a.high & 0x7FFF ) == 0x7FFF )
        && (bits64) ( aLow<<1 )
        && ( a.low == aLow );

}

/*
-------------------------------------------------------------------------------
Returns the result of converting the extended double-precision floating-
point NaN `a' to the canonical NaN format.  If `a' is a signaling NaN, the
invalid exception is raised.
-------------------------------------------------------------------------------
*/
static commonNaNT floatx80ToCommonNaN( floatx80 a )
{
    commonNaNT z;

    if ( floatx80_is_signaling_nan( a ) ) raise_float_exception( float_flag_invalid );
    z.sign = a.high>>15;
    z.low = 0;
    z.high = a.low<<1;
    return z;

}

/*
-------------------------------------------------------------------------------
Returns the result of converting the canonical NaN `a' to the extended
double-precision floating-point format.
-------------------------------------------------------------------------------
*/
static floatx80 commonNaNToFloatx80( commonNaNT a )
{
    floatx80 z;

    z.low = LIT64( 0xC000000000000000 ) | ( a.high>>1 );
    z.high = ( ( (bits16) a.sign )<<15 ) | 0x7FFF;
    z.__padding = 0;
    return z;

}

/*
-------------------------------------------------------------------------------
Takes two extended double-precision floating-point values `a' and `b', one
of which is a NaN, and returns the appropriate NaN result.  If either `a' or
`b' is a signaling NaN, the invalid exception is raised.
-------------------------------------------------------------------------------
*/
static floatx80 propagateFloatx80NaN( floatx80 a, floatx80 b )
{
    flag aIsNaN, aIsSignalingNaN, bIsNaN, bIsSignalingNaN;

    aIsNaN = floatx80_is_nan( a );
    aIsSignalingNaN = floatx80_is_signaling_nan( a );
    bIsNaN = floatx80_is_nan( b );
    bIsSignalingNaN = floatx80_is_signaling_nan( b );
    a.low |= LIT64( 0xC000000000000000 );
    b.low |= LIT64( 0xC000000000000000 );
    if ( aIsSignalingNaN | bIsSignalingNaN ) raise_float_exception( float_flag_invalid );
    if ( aIsNaN ) {
        return ( aIsSignalingNaN & bIsNaN ) ? b : a;
    }
    else {
        return b;
    }

}

#endif
