//----------------------------------------------------------------------------
/** @file SgVectorUtility.cpp
    See SgVectorUtility.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgVectorUtility.h"

#include <algorithm>

//----------------------------------------------------------------------------

void SgVectorUtility::Difference(SgVector<int>* vector,
                                 const SgVector<int>& vector2)
{
    // @todo speed up by using tags, if used in time-critical code.
    for (SgVectorIterator<int> it(vector2); it; ++it)
        vector->Exclude(*it);
}

void SgVectorUtility::Intersection(SgVector<int>* vector,
                                   const SgVector<int>& vector2)
{
     SgVector<int> newVector;
     for (SgVectorIterator<int> it(*vector); it; ++it)
     {
         // @todo speed up by hash tags, if used in time-critical code
         if (vector2.Contains(*it))
             newVector.PushBack(*it);
     }
     newVector.SwapWith(vector);
}
