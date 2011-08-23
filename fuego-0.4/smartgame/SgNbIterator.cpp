//----------------------------------------------------------------------------
/** @file SgNbIterator.cpp
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgNbIterator.h"

using namespace std;

//----------------------------------------------------------------------------

SgNb4Iterator::Precomp::Precomp()
{
    for (SgGrid row = 1; row <= SG_MAX_SIZE; ++row)
        for (SgGrid col = 1; col <= SG_MAX_SIZE; ++col)
        {
            SgPoint p = SgPointUtil::Pt(col, row);
            m_nb[p][0] = p - SG_NS;
            m_nb[p][1] = p - SG_WE;
            m_nb[p][2] = p + SG_WE;
            m_nb[p][3] = p + SG_NS;
        }
}

const SgNb4Iterator::Precomp SgNb4Iterator::s_precomp;

//----------------------------------------------------------------------------

const int SgNb4DiagIterator::s_diag[4] = {
    -SG_NS - SG_WE,
    -SG_NS + SG_WE,
    +SG_NS - SG_WE,
    +SG_NS + SG_WE
};

//----------------------------------------------------------------------------

const int SgNb8Iterator::s_nb8[8] = {
    -SG_NS - SG_WE,
    -SG_NS,
    -SG_NS + SG_WE,
    -SG_WE,
    +SG_WE,
    +SG_NS - SG_WE,
    +SG_NS,
    +SG_NS + SG_WE
};

//----------------------------------------------------------------------------

