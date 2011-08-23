#ifndef HCUBE_TICTACTOEGAMETREESEARCH_H_INCLUDED
#define HCUBE_TICTACTOEGAMETREESEARCH_H_INCLUDED

#ifndef DOXYGEN_PROCESSOR

#include "HCUBE_Defines.h"
#include "Experiments/HCUBE_TicTacToeShared.h"

namespace HCUBE
{
    class TicTacToeGameTreeSearch
    {
    public:
        TicTacToeStats* *results;
        TicTacToeStats* resultsData;
    protected:

    public:
        TicTacToeGameTreeSearch();

        virtual ~TicTacToeGameTreeSearch();

        void getFullResults(
            TicTacToeStats *stats,
            NEAT::FastNetwork<double> &substrate,
            int numNodesX,
            int numNodesY,
            map<Node,string> &nameLookup
        );

        void getResult(
            int xBoardState,
            int oBoardState,
            NEAT::FastNetwork<double> &substrate,
            int numNodesX,
            int numNodesY,
            map<Node,string> &nameLookup
        );

        TicTacToeGameTreeSearch(const TicTacToeGameTreeSearch &other);

    protected:
        void operator=(const TicTacToeGameTreeSearch &other)
        {}
    };
}

#endif

#endif // HCUBE_TICTACTOEGAMETREESEARCH_H_INCLUDED
