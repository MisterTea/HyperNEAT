//----------------------------------------------------------------------------
/** @file GoBoardCheckPerformance.cpp
    See GoBoardCheckPerformance.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoBoardCheckPerformance.h"

#include <fstream>
#include "GoBoard.h"
#include "GoBoardUtil.h"
#include "SgTime.h"

using namespace std;

//----------------------------------------------------------------------------

void GoBoardCheckPerformance::CheckPerformance(const GoBoard& board,
                                               ostream& out)
{
    const int NUM_REPETITIONS = 10000;
    int i;

    double startTime = SgTime::Get();
    int sum1 = 0;
    for (i = 0; i < NUM_REPETITIONS; ++i)
    {
        for (SgPoint p = 0; p < SG_MAXPOINT; ++p)
        {   if (board.IsEmpty(p))
                sum1 += p;
        }
    }
    double endTime1 = SgTime::Get();

    int sum2 = 0;
    for (i = 0; i < NUM_REPETITIONS; ++i)
    {
        for (SgPoint p = board.FirstBoardPoint(); p <= board.LastBoardPoint();
             ++p)
        {   if (board.IsEmpty(p))
                sum2 += p;
        }
    }
    double endTime2 = SgTime::Get();

    int sum3 = 0;
    for (i = 0; i < NUM_REPETITIONS; ++i)
        for (GoBoard::Iterator it(board); it; ++it)
        {
            if (board.IsEmpty(*it))
                sum3 += *it;
        }
    double endTime3 = SgTime::Get();

    int sum4 = 0;
    for (i = 0; i < NUM_REPETITIONS; ++i)
    {
        for (SgPoint p = 0; p < SG_MAXPOINT; ++p)
        {
            if (board.IsEmpty(p))
                sum4 += p;
        }
    }
    double endTime4 = SgTime::Get();

    int sum5 = 0;
    for (i = 0; i < NUM_REPETITIONS; ++i)
    {
        for (SgPoint p = board.FirstBoardPoint(); p <= board.LastBoardPoint();
             ++p)
        {
            if (board.IsEmpty(p))
                sum5 += p;
        }
    }
    double endTime5 = SgTime::Get();

    SG_ASSERT(sum1 == sum2);
    SG_ASSERT(sum2 == sum3);
    SG_ASSERT(sum3 == sum4);
    SG_ASSERT(sum4 == sum5);

    double time1 = endTime1 - startTime;
    double time2 = endTime2 - endTime1;
    double time3 = endTime3 - endTime2;
    double time4 = endTime4 - endTime3;
    double time5 = endTime5 - endTime4;

    out << "Time1: " << time1 << " For 0..SG_MAXPOINT\n"
        << "Time2: " << time2 << " First/LastBoardPoint\n"
        << "Time3: " << time3 << " GoBoard::Iterator\n"
        << "Time4: " << time4 << " For 0..SG_MAXPOINT, no dependency\n"
        << "Time5: " << time5 << " First/LastBoardPoint, no dependency\n";
}
