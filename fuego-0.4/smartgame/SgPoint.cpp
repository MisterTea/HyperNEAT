//----------------------------------------------------------------------------
/** @file SgPoint.cpp
    See SgPoint.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgPoint.h"

#include <iostream>
#include <sstream>
#include "SgUtil.h"

using namespace std;

//----------------------------------------------------------------------------

std::string SgPointUtil::PointToString(SgPoint p)
{
    ostringstream buffer;
    if (p == SG_NULLMOVE)
        buffer << "NULL";
    else if (p == SG_PASS)
        buffer << "PASS";
    else if (p == SG_COUPONMOVE)
        buffer << "COUPON";
    else if (p == SG_COUPONMOVE_VIRTUAL)
        buffer << "COUPON_VIRTUAL";
    else if (p == SG_RESIGN)
        buffer << "RESIGN";
    else
        buffer << Letter(Col(p)) << Row(p);
    return buffer.str();
}

SgPoint SgPointUtil::Rotate(int rotation, SgPoint p, int size)
{
    SG_ASSERT(rotation < 8);
    if (p == SG_PASS)
        return SG_PASS;
    int col = Col(p);
    int row = Row(p);
    switch (rotation)
    {
    case 0:
        return Pt(col, row);
    case 1:
        return Pt(size - col + 1, row);
    case 2:
        return Pt(col, size - row + 1);
    case 3:
        return Pt(row, col);
    case 4:
        return Pt(size - row + 1, col);
    case 5:
        return Pt(row, size - col + 1);
    case 6:
        return Pt(size - col + 1, size - row + 1);
    case 7:
        return Pt(size - row + 1, size - col + 1);
    default:
        SG_ASSERT(false);
        return p;
    }
}

int SgPointUtil::InvRotation(int rotation)
{
    switch (rotation)
    {
    case 0:
        return 0;
    case 1:
        return 1;
    case 2:
        return 2;
    case 3:
        return 3;
    case 4:
        return 5;
    case 5:
        return 4;
    case 6:
        return 6;
    case 7:
        return 7;
    default:
        SG_ASSERT(false);
        return 0;
    }
}

//----------------------------------------------------------------------------

void SgReadPoint::Read(std::istream& in) const
{
    string s;
    in >> s;
    if (! in)
        return;
    SG_ASSERT(s.length() > 0);
    if (s == "PASS" || s == "pass")
    {
        m_point = SG_PASS;
        return;
    }
    if (s == "COUPON" || s == "coupon")
    {
        m_point = SG_COUPONMOVE;
        return;
    }
    if (s == "COUPON_VIRTUAL" || s == "coupon_virtual")
    {
        m_point = SG_COUPONMOVE_VIRTUAL;
        return;
    }
    if (s == "RESIGN" || s == "resign")
    {
        m_point = SG_RESIGN;
        return;
    }
    char c = s[0];
    if (c >= 'A' && c <= 'Z')
        c = c - 'A' + 'a';
    else if (c < 'a' || c > 'z')
    {
        in.setstate(ios::failbit);
        return;
    }
    int col = c - 'a' + 1;
    if (c >= 'j')
        --col;
    istringstream sin(s.substr(1));
    int row;
    sin >> row;
    if (! sin || ! SgUtil::InRange(col, 1, SG_MAX_SIZE)
        || ! SgUtil::InRange(row, 1, SG_MAX_SIZE))
    {
        in.setstate(ios::failbit);
        return;
    }
    m_point = SgPointUtil::Pt(col, row);
}

//----------------------------------------------------------------------------

ostream& operator<<(ostream& out, const SgWritePoint& writePoint)
{
    out << SgPointUtil::PointToString(writePoint.m_p);
    return out;
}

//----------------------------------------------------------------------------

