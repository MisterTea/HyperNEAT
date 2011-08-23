//----------------------------------------------------------------------------
/** @file SgMath.h
    Math utility functions.
*/
//----------------------------------------------------------------------------

#ifndef SG_MATH_H
#define SG_MATH_H

#include <cmath>

//----------------------------------------------------------------------------

/** Square-root of 2.
    Can be used instead of M_SQRT2 to support systems that don't define
    BSD math constants (Mac OS X Tiger?)
*/
#define SG_SQRT2 1.41421356237309504880

//----------------------------------------------------------------------------

/** Math utility functions. */
namespace SgMath
{
    /** Round to the closest integer value. */
    int RoundToInt(double v);
}

inline int SgMath::RoundToInt(double v)
{
    return static_cast<int>(std::floor(v + 0.5));
}
//----------------------------------------------------------------------------

#endif // SG_MATH_H
