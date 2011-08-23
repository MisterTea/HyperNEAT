//----------------------------------------------------------------------------
/** @file GoSetupUtil.cpp
    See GoSetupUtil.h
*/
//----------------------------------------------------------------------------

#include <cstdio>
#include "SgSystem.h"
#include "GoBoard.h"
#include "GoSetup.h"
#include "GoSetupUtil.h"

using SgPointUtil::Pt;

//----------------------------------------------------------------------------
namespace {

bool IsBlackChar(char c)
{
    return c == 'x' || c == 'X' || c == '@';
}

bool IsWhiteChar(char c)
{
    return c == '0' || c == 'o' || c == 'O';
}

bool IsEmptyChar(char c)
{
    return c == '.' || c == '+';
}

bool IsIgnoreChar(char c)
{
    return c == ' ' || c == '\t';
}

bool ReadLine(std::streambuf& in, GoSetup& setup, int row, int& currLength)
{
    int col = 0;
    for (char c = in.sbumpc(); c != EOF && c != '\n'; c = in.sbumpc())
    {
        if (IsBlackChar(c) || IsWhiteChar(c) || IsEmptyChar(c))
        {
            ++col;
            if (col > SG_MAX_SIZE)
                return false; // bad data?
        }
        else if (! IsIgnoreChar(c))
            throw SgException("bad input data for ReadLine");

        SgPoint p = Pt(col, row);
        if (IsBlackChar(c))
        {
            setup.m_stones[SG_BLACK].Include(p);
        }
        else if (IsWhiteChar(c))
        {
            setup.m_stones[SG_WHITE].Include(p);
        }
    }
    currLength = col;
    return currLength > 0;
}

} // namespace

//----------------------------------------------------------------------------

GoSetup GoSetupUtil::CreateSetupFromStream(std::streambuf& in, int& boardSize)
{
    GoSetup setup;
    boardSize = 0;
    
    int currLength = 0;
    for (int row = 1; ReadLine(in, setup, row, currLength); ++row)
    {
        if (currLength != boardSize)
        {
            if (boardSize == 0)
                boardSize = currLength; // length established
            else
                throw SgException("bad input data for CreateSetupFromStream");
        }
        if (row > boardSize)
            break;
    }

    // The standard display format has rows in decreasing order, so flip rows
    // from top to bottom.
    SgPointSetUtil::Rotate(2, setup.m_stones[SG_BLACK], boardSize);
    SgPointSetUtil::Rotate(2, setup.m_stones[SG_WHITE], boardSize);
    return setup;
}

GoSetup GoSetupUtil::CreateSetupFromString(std::string& in,
                                           int& boardSize)
{
    std::stringbuf inBuf(in);
    return GoSetupUtil::CreateSetupFromStream(inBuf, boardSize);
}

GoSetup GoSetupUtil::CurrentPosSetup(const GoBoard& bd)
{
    GoSetup setup;
    setup.m_player = bd.ToPlay();
    for (GoBoard::Iterator it2(bd); it2; ++it2)
    {
        SgPoint p = *it2;
        if (bd.Occupied(p))
            setup.m_stones[bd.GetColor(p)].Include(p);
    }
    return setup;
}

//----------------------------------------------------------------------------
