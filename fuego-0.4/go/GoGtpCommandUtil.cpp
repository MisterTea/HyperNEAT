//----------------------------------------------------------------------------
/** @file GoGtpCommandUtil.cpp
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoGtpCommandUtil.h"

#include <limits>
#include "GoBoard.h"
#include "GtpEngine.h"
#include "SgDebug.h"
#include "SgPointArray.h"

using namespace std;
using SgPointUtil::Pt;

//----------------------------------------------------------------------------

namespace {

/** Comparison of gogui-analyze_commands lines for
    SortResponseAnalyzeCommands
*/
bool LessAnalyzeLabel(const string& line1, const string& line2)
{
    // Analyze label is the second entry in the line, separator is '/'
    size_t pos1 = line1.find("/");
    size_t pos2 = line2.find("/");
    if (pos1 == string::npos || pos2 == string::npos)
    {
        // Shouldn't happen in well-formed lines
        SG_ASSERT(false);
        return true;
    }
    return (line1.substr(pos1) < line2.substr(pos2));
}

} // namespace

//----------------------------------------------------------------------------

SgEmptyBlackWhite GoGtpCommandUtil::EmptyBlackWhiteArg(const GtpCommand& cmd,
                                                       std::size_t number)
{
    string value = cmd.ArgToLower(number);
    if (value == "e" || value == "empty")
        return SG_EMPTY;
    if (value == "b" || value == "black")
        return SG_BLACK;
    if (value == "w" || value == "white")
        return SG_WHITE;
    throw GtpFailure() << "argument " << (number + 1)
                       << " must be black, white or empty";
}

SgBlackWhite GoGtpCommandUtil::BlackWhiteArg(const GtpCommand& cmd,
                                             std::size_t number)
{
    string value = cmd.ArgToLower(number);
    if (value == "b" || value == "black")
        return SG_BLACK;
    if (value == "w" || value == "white")
        return SG_WHITE;
    throw GtpFailure() << "argument " << (number + 1)
                       << " must be black or white";
}

SgPoint GoGtpCommandUtil::EmptyPointArg(const GtpCommand& cmd,
                                        std::size_t number,
                                        const GoBoard& board)
{
    SgPoint point = PointArg(cmd, number, board);
    if (board.GetColor(point) != SG_EMPTY)
        throw GtpFailure() << "point " << SgWritePoint(point)
                           << " must be empty";
    return point;
}

SgVector<SgPoint> GoGtpCommandUtil::GetHandicapStones(int size, int n)
{
    SgVector<SgPoint> stones;
    if (n == 0)
        return stones;
    // GTP locations are defined up to size 25, but SG_MAX_SIZE could be
    // smaller
    if (size > SG_MAX_SIZE || size > 25)
        throw GtpFailure("no standard handicap locations defined");
    int line1 = -1;
    int line2 = -1;
    int line3 = -1;
    if (size >= 13)
    {
        line1 = 4;
        line3 = size - 3;
    }
    else if (size >= 7)
    {
        line1 = 3;
        line3 = size - 2;
    }
    if (size >= 9 && size % 2 != 0)
        line2 = size / 2 + 1;
    if (line1 < 0 || n == 1 || n > 9 || (n > 4 && line2 < 0))
        throw GtpFailure("no standard handicap locations defined");
    if (n >= 1)
        stones.PushBack(Pt(line1, line1));
    if (n >= 2)
        stones.PushBack(Pt(line3, line3));
    if (n >= 3)
        stones.PushBack(Pt(line1, line3));
    if (n >= 4)
        stones.PushBack(Pt(line3, line1));
    if (n >= 5 && n % 2 != 0)
    {
        stones.PushBack(Pt(line2, line2));
        --n;
    }
    if (n >= 5)
        stones.PushBack(Pt(line1, line2));
    if (n >= 6)
        stones.PushBack(Pt(line3, line2));
    if (n >= 7)
        stones.PushBack(Pt(line2, line1));
    if (n >= 8)
        stones.PushBack(Pt(line2, line3));
    return stones;
}

SgMove GoGtpCommandUtil::MoveArg(const GtpCommand& cmd, std::size_t number,
                                 const GoBoard& board)
{
    if (cmd.ArgToLower(number) == "pass")
        return SG_PASS;
    return PointArg(cmd, number, board);
}

void GoGtpCommandUtil::ParseMultiStoneArgument(GtpCommand& cmd,
                                               const GoBoard& board,
                                               SgBlackWhite& toPlay,
                                               SgBlackWhite& defender,
                                               SgVector<SgPoint>& crucial)
{
    toPlay = GoGtpCommandUtil::BlackWhiteArg(cmd, 0);
    SgDebug() << "Set " << SgBW(toPlay) << " to play\n";
    SgPoint point = GoGtpCommandUtil::StoneArg(cmd, 1, board);
    defender = board.GetColor(point);
    SG_ASSERT(defender == SG_BLACK || defender == SG_WHITE);
    crucial.PushBack(point);
    for (size_t i = 2; i < cmd.NuArg(); ++i)
    {
        SgPoint p = GoGtpCommandUtil::StoneArg(cmd, i, board);
        if (board.GetColor(p) != defender)
            throw GtpFailure("Crucial stones must be same color");
        else
            crucial.PushBack(p);
    }
}

SgPoint GoGtpCommandUtil::PointArg(const GtpCommand& cmd,
                                   const GoBoard& board)
{
    cmd.CheckNuArg(1);
    return PointArg(cmd, 0, board);
}

SgPoint GoGtpCommandUtil::PointArg(const GtpCommand& cmd, std::size_t number,
                                   const GoBoard& board)
{
    string arg = cmd.Arg(number);
    istringstream in(arg);
    SgPoint p;
    in >> SgReadPoint(p);
    if (! in)
        throw GtpFailure() << "invalid point " << arg;
    if (p == SG_PASS)
        throw GtpFailure("expected point, not pass");
    if (SgMoveUtil::IsCouponMove(p))
        throw GtpFailure("expected point, not coupon move");
    if (! board.IsValidPoint(p))
        throw GtpFailure() << "point outside board " << arg;
    return p;
}

SgVector<SgPoint> GoGtpCommandUtil::PointListArg(const GtpCommand& cmd,
                                               std::size_t number,
                                               const GoBoard& board)
{
    SgVector<SgPoint> result;
    for (size_t i = number; i < cmd.NuArg(); ++i)
        result.PushBack(PointArg(cmd, i, board));
    return result;
}

void GoGtpCommandUtil::RespondNumberArray(GtpCommand& cmd,
                                          const SgPointArray<int>& array,
                                          int scale, const GoBoard& board)
{
    SgPointArray<string> result("\"\"");
    for (GoBoard::Iterator it(board); it; ++it)
    {
        SgPoint p(*it);
        if (array[p] != numeric_limits<int>::min())
        {
            ostringstream out;
            out << (array[p] / scale);
            result[p] = out.str();
        }
    }
    cmd << '\n' << SgWritePointArray<string>(result, board.Size());
}

string GoGtpCommandUtil::SortResponseAnalyzeCommands(const string& response)
{
    vector<string> allLines;
    istringstream in(response);
    string line;
    while (getline(in, line))
        allLines.push_back(line);        
    sort(allLines.begin(), allLines.end(), LessAnalyzeLabel);
    ostringstream sortedResponse;
    for (vector<string>::const_iterator it = allLines.begin();
         it != allLines.end(); ++it)
        sortedResponse << *it << '\n';
    return sortedResponse.str();
}

SgPoint GoGtpCommandUtil::StoneArg(const GtpCommand& cmd, std::size_t number,
                                   const GoBoard& board)
{
    SgPoint point = PointArg(cmd, number, board);
    if (board.GetColor(point) == SG_EMPTY)
        throw GtpFailure() << "point " << SgWritePoint(point)
                           << " must be occupied";
    return point;
}

//----------------------------------------------------------------------------

