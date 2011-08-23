#include "HCUBE_Defines.h"

#include "Experiments/HCUBE_TicTacToeGameTreeSearch.h"

#include "Experiments/HCUBE_CheckersExperiment.h"

#define TIC_TAC_TOE_GAME_TREE_SEARCH_DEBUG (0)

int SMART_ATTACKER = 0;
int SMART_DEFENDER = 0;

namespace HCUBE
{
    extern int checkWin(int xBoardState,int oBoardState);

    int factorial(int a)
    {
        if (a<=-1)
            return -factorial(-a);
        else if (a<=1)
            return 1;
        else
            return a * (a-1);
    }

    int getOpenSpotsCount(int xBoardState,int oBoardState)
    {
        int count=0;

        for (int a=0;a<9;a++)
        {
            if (! (HASBIT(xBoardState,a)||HASBIT(oBoardState,a)) )
            {
                count++;
            }
        }

        return count;
    }

    bool checkFullBoard(int xBoardState,int oBoardState)
    {
        for (int a=0;a<9;a++)
        {
            if (! (HASBIT(xBoardState,a)||HASBIT(oBoardState,a)) )
            {
                return false;
            }
        }

        return true;
    }

    TicTacToeGameTreeSearch::TicTacToeGameTreeSearch()
    {
        resultsData = (TicTacToeStats*)malloc(496*496*sizeof(TicTacToeStats));

        results = (TicTacToeStats**)malloc(496*sizeof(TicTacToeStats*));

        for (int a=0;a<496;a++)
        {
            results[a] = &resultsData[a*496];
        }
    }

    TicTacToeGameTreeSearch::TicTacToeGameTreeSearch(const TicTacToeGameTreeSearch &other)
    {
        resultsData = (TicTacToeStats*)malloc(496*496*sizeof(TicTacToeStats));
        memcpy(resultsData,other.resultsData,496*496*sizeof(TicTacToeStats));

        results = (TicTacToeStats**)malloc(496*sizeof(TicTacToeStats*));

        for (int a=0;a<496;a++)
        {
            results[a] = &resultsData[a*496];
        }
    }

    TicTacToeGameTreeSearch::~TicTacToeGameTreeSearch()
    {
        free(results);
        free(resultsData);
    }

    void TicTacToeGameTreeSearch::getFullResults(
        TicTacToeStats *stats,
        NEAT::FastNetwork<double> &substrate,
        int numNodesX,
        int numNodesY,
        map<Node,string> &nameLookup
    )
    {
        //cout << "Getting full results...\n";
        memset(resultsData,0,496*496*sizeof(TicTacToeStats));

        getResult(
            0,
            0,
            substrate,
            numNodesX,
            numNodesY,
            nameLookup
        );

        *stats = results[0][0];
    }

    void TicTacToeGameTreeSearch::getResult(
        int xBoardState,
        int oBoardState,
        NEAT::FastNetwork<double> &substrate,
        int numNodesX,
        int numNodesY,
        map<Node,string> &nameLookup
    )
    {
        //cout << xBoardState << ' ' << oBoardState << endl;
        TicTacToeStats *tmpStats = &results[xBoardState][oBoardState];

        if (tmpStats->wins||tmpStats->losses||tmpStats->ties)
        {
            //This function has been run before, don't run again;
            return;
        }

        //This variable is to make sure we get a good average
        int numRecurse=0;

        int winner = checkWin(xBoardState,oBoardState);

        int openSpots = getOpenSpotsCount(xBoardState,oBoardState);

        if (winner==1)
        {
            //You won, get your points
            tmpStats->wins=10000;//factorial(openSpots);
            tmpStats->losses=0;
            tmpStats->ties=0;
            return;
        }
        else if (winner==-1)
        {
            //You lose, get your losses
            tmpStats->wins=0;
            tmpStats->losses=10000;//factorial(openSpots);
            tmpStats->ties=0;
            return;
        }
        else if (openSpots==0)
        {
            //board was full, you get a tie
            tmpStats->ties=10000;//1;
            tmpStats->wins=0;
            tmpStats->losses=0;
            return;
        }
        else
        {
            //Make X move
            substrate.reinitialize();
            substrate.dummyActivation();

            for (int y=(-numNodesY/2);y<=numNodesY/2;y++)
            {
                for (int x=(-numNodesX/2);x<=numNodesX/2;x++)
                {
                    int boardx = (x+(numNodesX/2))/(numNodesX/3);
                    int boardy = (y+(numNodesY/2))/(numNodesY/3);
#if TIC_TAC_TOE_GAME_TREE_SEARCH_DEBUG
                    cout << "Setting input values...\n";
                    cout << (toString(y)+string("/")+toString(x)) << endl;
                    cout << nameLookup[Node(x,y,0)] << endl;
                    string line;
                    getline(cin,line);
#endif
                    if ( HASBIT(xBoardState,(boardy)*3+(boardx)) )
                    {
                        substrate.setValue( nameLookup[Node(x,y,0)] , 1.0 );
                    }
                    else if ( HASBIT(oBoardState,(boardy)*3+(boardx)) )
                    {
                        substrate.setValue( nameLookup[Node(x,y,0)] , -1.0 );
                    }

                }
            }

            if (substrate.hasNode("Bias"))
            {
                substrate.setValue("Bias",0.3);
            }

#if TIC_TAC_TOE_GAME_TREE_SEARCH_DEBUG
            cout << "Updating network\n";
#endif

            for (int a=0;a<2;a++)
            {
                substrate.update();
            }

            double biggestAnswer=-INT_MAX;
            int biggestX,biggestY;

            for (int y=(-numNodesY/2);y<=numNodesY/2;y++)
            {
                for (int x=(-numNodesX/2);x<=numNodesX/2;x++)
                {
                    int boardx = (x+(numNodesX/2))/(numNodesX/3);
                    int boardy = (y+(numNodesY/2))/(numNodesY/3);

                    double value = substrate.getValue( nameLookup[Node(x,y,2)] );

                    //if (
                    //!HASBIT(xBoardState,(boardy)*3+(boardx)) &&
                    //!HASBIT(oBoardState,(boardy)*3+(boardx))
                    //)
                    {
                        if (value>biggestAnswer)
                        {
                            biggestAnswer = value;
                            biggestX = boardx;
                            biggestY = boardy;
                        }
                    }
                    /*     else
                         {
                    #if TIC_TAC_TOE_GAME_TREE_SEARCH_DEBUG
                             cout << "Board spot is taken: " << xBoardState << " " << oBoardState << endl;
                             cout << HASBIT(xBoardState,(boardy)*3+(boardx)) << "/"
                             << HASBIT(oBoardState,(boardy)*3+(boardx)) << endl
                             << ":" << (boardy)*3+(boardx) << endl
                             << "#" << numNodesX << ' ' << numNodesY << endl;
                    #endif
                         }
                    */
#if TIC_TAC_TOE_GAME_TREE_SEARCH_DEBUG
                    cout << "output...\n";
                    cout << (toString(y)+string("/")+toString(x)) << endl;
                    cout << nameLookup[Node(x,y,2)] << endl;
                    string line;
                    cout << value << endl;
                    getline(cin,line);
#endif
                }
            }

            //CREATE_PAUSE(string("Error!: ")+toString(__LINE__));

            if (biggestAnswer==-INT_MAX)
            {
                throw CREATE_LOCATEDEXCEPTION_INFO("Can't move! WTF?");
            }

            int newXBoardState = xBoardState;

            if (
                !HASBIT(xBoardState,(biggestY)*3+(biggestX)) &&
                !HASBIT(oBoardState,(biggestY)*3+(biggestX))
            )
            {
                SETBIT(newXBoardState,(biggestY*3+biggestX));

                //X moved, drop the number of open spots
                openSpots--;
            }
            else
            {
                //X forfeits his move if he doesn't play in a square that makes sense
            }

#if TIC_TAC_TOE_GAME_TREE_SEARCH_DEBUG
            cout << "X played at " << (biggestY*3+biggestX) << endl;
#endif


            if (checkWin(newXBoardState,oBoardState)==1)
            {
                //You win! You get wins
                tmpStats->wins=10000;//factorial(openSpots);
                tmpStats->losses=0;
                tmpStats->ties=0;
                return;
            }
            else if (openSpots==0)
            {
                //Tie game!
                tmpStats->wins=0;
                tmpStats->losses=0;
                tmpStats->ties=10000;//1;
                return;
            }

            //O is going to move in this loop, drop openSpots
            openSpots--;

            if (SMART_ATTACKER)
            {
                for (int a=0;a<9;a++)
                {
                    if (HASBIT(newXBoardState,a)||HASBIT(oBoardState,a))
                    {
                        //Do nothing, we can't move here.
                    }
                    else
                    {
                        int newOBoardState = oBoardState|(1<<a);

                        int winner = checkWin(newXBoardState,newOBoardState);

                        if (winner==-1)
                        {
                            //You lost, get some losses
                            tmpStats->wins=0;
                            tmpStats->ties=0;
                            tmpStats->losses=10000;//factorial(openSpots);
                            return;
                        }
                    }
                }
            }

            if (SMART_DEFENDER)
            {
                for (int a=0;a<9;a++)
                {
                    if (HASBIT(newXBoardState,a)||HASBIT(oBoardState,a))
                    {
                        //Do nothing, we can't move here.
                    }
                    else
                    {
                        int futureXBoardState = newXBoardState|(1<<a);

                        int winner = checkWin(futureXBoardState,oBoardState);

                        if (winner==1)
                        {
                            //X would win if he went here, we must go
                            //here to defend.
                            int newOBoardState = oBoardState|(1<<a);

                            getResult(
                                newXBoardState,
                                newOBoardState,
                                substrate,
                                numNodesX,
                                numNodesY,
                                nameLookup
                            );

                            numRecurse++;

                            //Return the results
                            TicTacToeStats *tmpStats2 = &results[newXBoardState][newOBoardState];
                            tmpStats->wins = tmpStats2->wins;
                            tmpStats->losses = tmpStats2->losses;
                            tmpStats->ties = tmpStats2->ties;
                            return;
                        }
                    }
                }
            }

            for (int a=0;a<9;a++)
            {
                if (HASBIT(newXBoardState,a)||HASBIT(oBoardState,a))
                {
                    //Do nothing, we can't move here.
                }
                else
                {
                    int newOBoardState = oBoardState|(1<<a);

                    getResult(
                        newXBoardState,
                        newOBoardState,
                        substrate,
                        numNodesX,
                        numNodesY,
                        nameLookup
                    );

                    numRecurse++;

                    //Sum the results
                    TicTacToeStats *tmpStats2 = &results[newXBoardState][newOBoardState];

                    {
                        tmpStats->wins += tmpStats2->wins;
                        tmpStats->losses += tmpStats2->losses;
                        tmpStats->ties += tmpStats2->ties;
                    }
                }
            }

            if (!numRecurse)
            {
                //Shouldn't happen
                throw CREATE_LOCATEDEXCEPTION_INFO("Num Recurse was 0!");
            }

            //Divide by the count to get the average.
            int sum = tmpStats->wins+tmpStats->losses+tmpStats->ties;

            double sumOverTenThousand = sum/10000.0;

            tmpStats->wins = (int)floor((tmpStats->wins / sumOverTenThousand)+0.5);

            tmpStats->losses = (int)floor((tmpStats->losses / sumOverTenThousand)+0.5);

            tmpStats->ties = (int)floor((tmpStats->ties / sumOverTenThousand)+0.5);
        }
    }
}
