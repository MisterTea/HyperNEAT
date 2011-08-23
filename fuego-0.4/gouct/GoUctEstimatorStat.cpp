//----------------------------------------------------------------------------
/** @file GoUctEstimatorStat.cpp
    See GoUctEstimatorStat.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctEstimatorStat.h"

#include <boost/format.hpp>
#include "GoModBoard.h"
#include "GoUctSearch.h"
#include "SgDebug.h"
#include "SgUctTreeUtil.h"

using namespace std;
using boost::format;

//----------------------------------------------------------------------------

void GoUctEstimatorStat::Compute(GoUctSearch& search,
                                 std::size_t trueValueMaxGames,
                                 std::size_t maxGames,
                                 std::size_t stepSize,
                                 const std::string& fileName)
{
    double maxTime = numeric_limits<double>::max();
    vector<SgMoveInfo> moves;
    search.GenerateAllMoves(moves);
    SgArray<float,SG_PASS + 1> trueValues;
    for (size_t i = 0; i < moves.size(); ++i)
    {
        SgPoint p = moves[i].m_move;
        GoModBoard modBoard(search.Board());
        modBoard.Board().Play(p);
        vector<SgMove> sequence;
        float value = search.Search(trueValueMaxGames, maxTime, sequence);
        trueValues[p] = SgUctSearch::InverseEval(value);
        modBoard.Board().Undo();
    }
    search.StartSearch();
    if (search.MpiSynchronizer()->IsRootProcess())
    {
    ofstream out(fileName.c_str(), ios::app);
    for (size_t n = 0; n < maxGames; n += stepSize)
    {
        search.PlayGame();
        for (size_t i = 0; i < moves.size(); ++i)
        {
        SgPoint p = moves[i].m_move;
        const SgUctTree& tree = search.Tree();
        const SgUctNode* child =
            SgUctTreeUtil::FindChildWithMove(tree, tree.Root(), p);
        if (child == 0)
            continue; // Root may not have been expanded yet
        out << (format("%1$d\t"
                   "%2$.2f\t"
                   "%3$d\t"
                   "%4$.2f\t"
                   "%5$d\t"
                   "%6$.2f\n"
                   )
            % n // 1
            % trueValues[p] // 2
            % child->MoveCount() // 3
            % (child->HasMean() ?
               SgUctSearch::InverseEval(child->Mean()) : 0) // 4
            % child->RaveCount() // 5
            % (child->HasRaveValue() ? child->RaveValue() : 0) // 6
            );
        }
    }
    }
    search.EndSearch();
}

//----------------------------------------------------------------------------
