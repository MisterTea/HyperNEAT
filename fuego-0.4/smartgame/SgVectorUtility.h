//----------------------------------------------------------------------------
/** @file SgVectorUtility.h
    %SgVector utility classes.
    
    SgVector operations that are useful but not basic enough to be included 
    in SgVector itself.
*/
//----------------------------------------------------------------------------

#ifndef SG_VECTORUTILITY_H
#define SG_VECTORUTILITY_H

#include "SgVector.h"

//----------------------------------------------------------------------------

namespace SgVectorUtility
{
    /** Exclude all elements of vector2 from vector */
    void Difference(SgVector<int>* vector, const SgVector<int>& vector2);

    /** Keep only elements in vector that also occur in vector2 */
    void Intersection(SgVector<int>* vector, const SgVector<int>& vector2);

#if UNUSED
    /** Reverse order of elements in vector */
    void Reverse(SgVector<int>* vector);
#endif
}

//----------------------------------------------------------------------------

#endif // SG_VECTORUTILITY_H
