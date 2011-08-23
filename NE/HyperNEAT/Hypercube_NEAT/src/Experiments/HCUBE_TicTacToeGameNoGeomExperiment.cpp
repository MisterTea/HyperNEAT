#include "HCUBE_Defines.h"

#include "Experiments/HCUBE_TicTacToeGameNoGeomExperiment.h"

#include "Experiments/HCUBE_TicTacToeShared.h"

#define TIC_TAC_TOE_GAME_NO_GEOM_EXPERIMENT_DEBUG (0)

extern int SMART_ATTACKER;
extern int SMART_DEFENDER;

namespace HCUBE
{
    using namespace NEAT;

    TicTacToeGameNoGeomExperiment::TicTacToeGameNoGeomExperiment(string _experimentName,int _threadID)
            :
            Experiment(_experimentName,_threadID),
            numNodesX(3),
            numNodesY(3)
    {
        //Populate the nameLookup table
        for (int y1=0;y1<numNodesY;y1++)
        {
            for (int x1=0;x1<numNodesX;x1++)
            {
                for (int z=0;z<3;z++)
                {
                    if (z==1)
                    {
                        //Let NEAT make its own hidden nodes.
                        continue;
                    }

                    int xmod = x1-numNodesX/2;
                    int ymod = y1-numNodesY/2;
                    Node node(xmod,ymod,z);
                    //cout << (y1-numNodesY/2) << '/' << (x1-numNodesX/2) << endl;
                    nameLookup[node] = (toString(xmod)+string("/")+toString(ymod) + string("/") + toString(z));

                }
            }
        }
    }

    GeneticPopulation* TicTacToeGameNoGeomExperiment::createInitialPopulation(int populationSize)
    {
        GeneticPopulation *population = new GeneticPopulation();
        vector<GeneticNodeGene> genes;

        cout << "Generating substrate...";

        int nodeCounter=0;

        for (int y1=0;y1<numNodesY;y1++)
        {
            for (int x1=0;x1<numNodesX;x1++)
            {
                for (int z=0;z<3;z++)
                {
                    if (z==1)
                    {
                        //Let NEAT make its own hidden nodes.
                        continue;
                    }

                    int xmod = x1-numNodesX/2;
                    int ymod = y1-numNodesY/2;
                    Node node(xmod,ymod,z);
                    //cout << (y1-numNodesY/2) << '/' << (x1-numNodesX/2) << endl;
                    //nameLookup[node] = (toString(xmod)+string("/")+toString(ymod) + string("/") + toString(z));

                    if (z==0)
                    {
                        genes.push_back(
                            GeneticNodeGene(
                                nameLookup[node],
                                "NetworkSensor",
                                0,
                                false
                            )
                        );
                    }
                    else //z==2
                    {
                        genes.push_back(
                            GeneticNodeGene(
                                nameLookup[node],
                                "NetworkOutputNode",
                                1,
                                false,
                                ACTIVATION_FUNCTION_SIGMOID
                            )
                        );
                    }

                    //nodes[nodeCounter] = NetworkNode(nameLookup[node]);
                    //nodeBiases[nodeCounter]=0.0;
                    //nodeCounter++;

                    /*
                    if (
                    y1==(numNodesY/2)
                    )
                    {
                    (*(nodes.end()-1))->setUpdate(false);
                    }
                    else
                    {
                    (*(nodes.end()-1))->setUpdate(true);
                    }
                    */
                }
            }
        }

        genes.push_back(
            GeneticNodeGene(
                "Bias",
                "NetworkSensor",
                0,
                false
            )
        );

        for (int a=0;a<populationSize;a++)
        {
            shared_ptr<GeneticIndividual> individual(new GeneticIndividual(genes,true,1.0));

            for (int b=0;b<0;b++)
            {
                individual->testMutate();
            }

            population->addIndividual(individual);
        }

        cout << "Finished creating population\n";
        return population;
    }

    extern int checkWin(int xBoardState,int oBoardState);
    /*{
        int tmpBoardState = xBoardState;

        if (
            (HASBIT(tmpBoardState,0)&&HASBIT(tmpBoardState,1)&&HASBIT(tmpBoardState,2)) ||
            (HASBIT(tmpBoardState,3)&&HASBIT(tmpBoardState,4)&&HASBIT(tmpBoardState,5)) ||
            (HASBIT(tmpBoardState,6)&&HASBIT(tmpBoardState,7)&&HASBIT(tmpBoardState,8)) ||

            (HASBIT(tmpBoardState,0)&&HASBIT(tmpBoardState,3)&&HASBIT(tmpBoardState,6)) ||
            (HASBIT(tmpBoardState,1)&&HASBIT(tmpBoardState,4)&&HASBIT(tmpBoardState,7)) ||
            (HASBIT(tmpBoardState,2)&&HASBIT(tmpBoardState,5)&&HASBIT(tmpBoardState,8)) ||

            (HASBIT(tmpBoardState,0)&&HASBIT(tmpBoardState,4)&&HASBIT(tmpBoardState,8)) ||
            (HASBIT(tmpBoardState,6)&&HASBIT(tmpBoardState,4)&&HASBIT(tmpBoardState,2))
        )
        {
            return 1;
        }

        tmpBoardState = oBoardState;

        if (
            (HASBIT(tmpBoardState,0)&&HASBIT(tmpBoardState,1)&&HASBIT(tmpBoardState,2)) ||
            (HASBIT(tmpBoardState,3)&&HASBIT(tmpBoardState,4)&&HASBIT(tmpBoardState,5)) ||
            (HASBIT(tmpBoardState,6)&&HASBIT(tmpBoardState,7)&&HASBIT(tmpBoardState,8)) ||

            (HASBIT(tmpBoardState,0)&&HASBIT(tmpBoardState,3)&&HASBIT(tmpBoardState,6)) ||
            (HASBIT(tmpBoardState,1)&&HASBIT(tmpBoardState,4)&&HASBIT(tmpBoardState,7)) ||
            (HASBIT(tmpBoardState,2)&&HASBIT(tmpBoardState,5)&&HASBIT(tmpBoardState,8)) ||

            (HASBIT(tmpBoardState,0)&&HASBIT(tmpBoardState,4)&&HASBIT(tmpBoardState,8)) ||
            (HASBIT(tmpBoardState,6)&&HASBIT(tmpBoardState,4)&&HASBIT(tmpBoardState,2))
        )
        {
            return -1;
        }

        return 0;
    }*/

    double TicTacToeGameNoGeomExperiment::processEvaluation(
        wxDC *drawContext,
        int_gen_type &move_gen
    )
    {
        /*if (drawContext)drawContext->Clear();

        double fitness = 0;

        int xBoardState=0;
        int oBoardState=0;

        int currentMove=0;

        for (int turns=0;turns<5;turns++)
        {
            int winner = checkWin(xBoardState,oBoardState);

            if (winner)
            {
                return 10.0*((winner+1)/2);
            }

        #if TIC_TAC_TOE_GAME_NO_GEOM_EXPERIMENT_DEBUG
            {
                cout << xBoardState << ' ' << oBoardState << endl;
            }
        #endif

            network.reinitialize();
            network.dummyActivation();

            //network.getNode("Bias")->setValue(0.3);

            for (int y=0;y<3;y++)
            {
                for (int x=0;x<3;x++)
                {
        #if TIC_TAC_TOE_GAME_NO_GEOM_EXPERIMENT_DEBUG
                    //cout << (toString(y)+string("/")+toString(x)) << endl;
        #endif
                    int position = (y)*3+(x);
                    int value = HASBIT(xBoardState,position)-HASBIT(oBoardState,position);

                    double doubleValue = value?value:0.1;

                    network.setValue( string("Input")+toString(y-1)+string("/")+toString(x-1) , doubleValue );

                    //if (drawContext)drawContext->DrawText(toString(value).c_str(),(x+2)*20,(y+2)*20);

                }
        #if TIC_TAC_TOE_GAME_NO_GEOM_EXPERIMENT_DEBUG
                {
                    cout << endl;
                }
        #endif
            }
        #if TIC_TAC_TOE_GAME_NO_GEOM_EXPERIMENT_DEBUG
            {
                cout << endl;
            }
        #endif

            network.update();

        #if TIC_TAC_TOE_GAME_NO_GEOM_EXPERIMENT_DEBUG
            {
                cout << "Set initial values\n";
            }
        #endif

            double biggestAnswer=-INT_MAX;
            int biggestX,biggestY;

            for (int y=0;y<3;y++)
            {
                for (int x=0;x<3;x++)
                {
                    int position = (y)*3+(x);
                    double answer = network.getValue( string("Output")+toString(y-1)+string("/")+toString(x-1) );

                    if (
                        !HASBIT(xBoardState|oBoardState,position) &&
                        answer>biggestAnswer
                    )
                    {
                        biggestAnswer=answer;
                        biggestX = x;
                        biggestY = y;
                    }
                }

            }

            if (biggestAnswer==-INT_MAX)
            {
                throw CREATE_LOCATEDEXCEPTION_INFO("Can't move! WTF?");
            }

            SETBIT(xBoardState,(biggestY*3+biggestX));

            if (drawContext)
            {
                drawContext->DrawText('X',(biggestX+1)*20,(biggestY+1)*20);
                drawContext->DrawText(toString(++currentMove).c_str(),(biggestX+1)*20+10,(biggestY+1)*20+10);
            }

            winner = checkWin(xBoardState,oBoardState);

            if (winner)
            {
                return 10.0*((winner+1)/2);
            }

            if (turns<4)
            {
                bool done=false;
                while (!done)
                {
                    int move = move_gen();
                    int y = move/3;
                    int x = move%3;

                    if (
                        !HASBIT(xBoardState|oBoardState,(y)*3+(x))
                    )
                    {
                        SETBIT(oBoardState,(y)*3+(x));

                        if (drawContext)
                        {
                            drawContext->DrawText('O',(x+1)*20,(y+1)*20);
                            drawContext->DrawText(toString(++currentMove).c_str(),(x+1)*20+10,(y+1)*20+10);
                        }

                        done=true;
                    }
                }

            }

        #if TIC_TAC_TOE_GAME_NO_GEOM_EXPERIMENT_DEBUG
            {
                cout << "Done checking board\n";
            }
        #endif
        }

        #if TIC_TAC_TOE_GAME_NO_GEOM_EXPERIMENT_DEBUG
        cout << "Tie game!\n";
        #endif
        return 5.0;*/

        if (drawContext)drawContext->Clear();

        double fitness = 0;

        int xBoardState=0;
        int oBoardState=0;

        int currentMove=0;

        for (int turns=0;turns<5;turns++)
        {
            int winner = checkWin(xBoardState,oBoardState);

            if (winner)
            {
                return 10.0*((winner+1)/2);
            }

#if TIC_TAC_TOE_GAME_EXPERIMENT_DEBUG
            {
                cout << xBoardState << ' ' << oBoardState << endl;
            }
#endif

            substrate.reinitialize();
            substrate.dummyActivation();

            for (int y=(-numNodesY/2);y<=numNodesY/2;y++)
            {
                for (int x=(-numNodesX/2);x<=numNodesX/2;x++)
                {
                    int boardx = (x+(numNodesX/2))/(numNodesX/3);
                    int boardy = (y+(numNodesY/2))/(numNodesY/3);
#if TIC_TAC_TOE_EXPERIMENT_DEBUG
                    //cout << (toString(y)+string("/")+toString(x)) << endl;
#endif
                    if ( HASBIT(xBoardState,(boardy)*3+(boardx)) )
                    {
                        substrate.setValue( nameLookup[Node(x,y,0)] , 1.0 );
                    }
                    else if ( HASBIT(oBoardState,(boardy)*3+(boardx)) )
                    {
                        substrate.setValue( nameLookup[Node(x,y,0)] , -1.0 );
                    }

                    bool isAnX = HASBIT(xBoardState,(boardy)*3+(boardx));

                    bool isAnO = HASBIT(oBoardState,(boardy)*3+(boardx));

#if TIC_TAC_TOE_EXPERIMENT_DEBUG
                    {
                        if (HASBIT(boardState,(boardy)*3+(boardx)))
                        {
                            cout << 'X';
                        }
                        else
                        {
                            cout << 'O';
                        }
                    }
#endif
                }
#if TIC_TAC_TOE_EXPERIMENT_DEBUG
                {
                    cout << endl;
                }
#endif
            }
#if TIC_TAC_TOE_EXPERIMENT_DEBUG
            {
                cout << endl;
            }
#endif

            for (int a=0;a<2;a++)
            {
                substrate.update();
            }

#if TIC_TAC_TOE_GAME_EXPERIMENT_DEBUG
            {
                cout << "Set initial values\n";
            }
#endif

            double biggestAnswer=-INT_MAX;
            int biggestX,biggestY;

            for (int y=(-numNodesY/2);y<=numNodesY/2;y++)
            {
                for (int x=(-numNodesX/2);x<=numNodesX/2;x++)
                {
                    int boardx = (x+(numNodesX/2))/(numNodesX/3);
                    int boardy = (y+(numNodesY/2))/(numNodesY/3);

                    double value = substrate.getValue( nameLookup[Node(x,y,2)] );

                    if (
                        !HASBIT(xBoardState,(boardy)*3+(boardx)) &&
                        !HASBIT(oBoardState,(boardy)*3+(boardx))
                    )
                    {
                        if (value>biggestAnswer)
                        {
                            biggestAnswer = value;
                            biggestX = boardx;
                            biggestY = boardy;
                        }
                    }
                }
            }

            //CREATE_PAUSE(string("Error!: ")+toString(__LINE__));

            if (biggestAnswer==-INT_MAX)
            {
                throw CREATE_LOCATEDEXCEPTION_INFO("Can't move! WTF?");
            }

            SETBIT(xBoardState,(biggestY*3+biggestX));

            if (drawContext)
            {
                drawContext->DrawText(_T("X"),(biggestX+2)*20,(biggestY+2)*20);
                drawContext->DrawText(STRING_TO_WXSTRING(toString(++currentMove)),(biggestX+2)*20+10,(biggestY+2)*20+10);
            }

            winner = checkWin(xBoardState,oBoardState);

            if (winner)
            {
                return 10.0*((winner+1)/2);
            }

            if (turns<4)
            {
                bool done=false;

                if (SMART_ATTACKER)
                {
                    for (int a=0;!done&&a<9;a++)
                    {
                        int y = a/3;
                        int x = a%3;

                        if (HASBIT(xBoardState,a)||HASBIT(oBoardState,a))
                        {
                            //Do nothing, we can't move here.
                        }
                        else
                        {
                            int newOBoardState = oBoardState|(1<<a);

                            int winner = checkWin(xBoardState,newOBoardState);

                            if (winner==-1)
                            {
                                if (drawContext)
                                {
                                    drawContext->DrawText(_T("O"),(x+2)*20,(y+2)*20);
                                    drawContext->DrawText(STRING_TO_WXSTRING(toString(++currentMove)),(x+2)*20+10,(y+2)*20+10);
                                }
                                oBoardState = newOBoardState;
                                done=true;
                            }
                        }
                    }
                }

                if (SMART_DEFENDER)
                {
                    for (int a=0;!done&&a<9;a++)
                    {
                        int y = a/3;
                        int x = a%3;

                        if (HASBIT(xBoardState,a)||HASBIT(oBoardState,a))
                        {
                            //Do nothing, we can't move here.
                        }
                        else
                        {
                            int futureXBoardState = xBoardState|(1<<a);

                            int winner = checkWin(futureXBoardState,oBoardState);

                            if (winner==1)
                            {
                                //X would win if he went here, we must go
                                //here to defend.
                                oBoardState = oBoardState|(1<<a);
                                if (drawContext)
                                {
                                    drawContext->DrawText(_T("O"),(x+2)*20,(y+2)*20);
                                    drawContext->DrawText(STRING_TO_WXSTRING(toString(++currentMove)),(x+2)*20+10,(y+2)*20+10);
                                }
                                done=true;

                            }
                        }
                    }
                }

                while (!done)
                {
                    int move = move_gen();
                    int y = move/3;
                    int x = move%3;

                    if (
                        !HASBIT(xBoardState|oBoardState,(y)*3+(x))
                    )
                    {
                        SETBIT(oBoardState,(y)*3+(x));

                        if (drawContext)
                        {
                            drawContext->DrawText(_T("O"),(x+2)*20,(y+2)*20);
                            drawContext->DrawText(STRING_TO_WXSTRING(toString(++currentMove)),(x+2)*20+10,(y+2)*20+10);
                        }

                        done=true;
                    }
                }

            }

#if TIC_TAC_TOE_GAME_EXPERIMENT_DEBUG
            {
                cout << "Done checking board\n";
            }
#endif
        }

#if TIC_TAC_TOE_GAME_EXPERIMENT_DEBUG
        cout << "Tie game!\n";
#endif
        return 5.0;
    }

    void TicTacToeGameNoGeomExperiment::processGroup(shared_ptr<NEAT::GeneticGeneration> generation)
    {
        shared_ptr<NEAT::GeneticIndividual> individual = group.front();
        //You get 10 points just for entering the game, wahooo!
        individual->setFitness(10);

        substrate = individual->spawnFastPhenotypeStack<double>();

        shared_ptr<TicTacToeStats> individualStats(new TicTacToeStats());

        searchTree.getFullResults(individualStats.get(),substrate,numNodesX,numNodesY,nameLookup);

        double totalFitness;
        /*totalFitness = (1000.0*individualStats->wins+500.0*individualStats->ties - 500*individualStats->losses) / (individualStats->wins+individualStats->ties+individualStats->losses);*/

        //totalFitness = 1000 + individualStats->wins - individualStats->losses + (individualStats->ties/2);

        totalFitness = individualStats->wins + individualStats->ties;

        individual->reward( max(0.0,totalFitness) );
        //cout << "Finished with individual! Stats: " << individualStats->toString() << " Fitness_Minus_10: " << totalFitness << endl;

        individual->setUserData(individualStats->toString());

        /*
        for (int a=0;a<numGames;a++)
        {
        double rewardValue = processEvaluation(move_gen);

        if (fabs(rewardValue)<1e-6)
        {
        (individualStats->losses)++;
        }
        else if (fabs(rewardValue-5.0)<1e-6)
        {
        (individualStats->ties)++;
        }
        else if (fabs(rewardValue-10.0)<1e-6)
        {
        (individualStats->wins)++;
        }
        else
        {
        cout << (string("ERROR: Unexpected fitness result!") + toString(rewardValue)) << endl;
        throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: Unexpected fitness result!");
        }

        individual->reward(rewardValue);
        }
        */

    }

#ifndef HCUBE_NOGUI
    void TicTacToeGameNoGeomExperiment::createIndividualImage(wxDC &drawContext,shared_ptr<NEAT::GeneticIndividual> individual)
    {
        base_generator_type generator = base_generator_type(static_cast<unsigned int>(std::clock()%1000000000));

        int_gen_type move_gen = int_gen_type(generator, int_distribution_type(0, 8));

        if (lastIndividualSeen!=individual)
        {
            cout << "Repopulating substrate\n";
            substrate = individual->spawnFastPhenotypeStack<double>();
            lastIndividualSeen = individual;
        }

        processEvaluation(&drawContext,move_gen);
    }

    bool TicTacToeGameNoGeomExperiment::handleMousePress(wxMouseEvent& event,wxSize &bitmapSize)
    {
        wxPoint clickPoint = event.GetPosition();
        int x = clickPoint.x/20-1;
        int y = clickPoint.y/20-1;
        return true;
    }
#endif

    Experiment* TicTacToeGameNoGeomExperiment::clone()
    {
        TicTacToeGameNoGeomExperiment* experiment = new TicTacToeGameNoGeomExperiment(*this);

        return experiment;
    }

    void TicTacToeGameNoGeomExperiment::resetGenerationData(shared_ptr<NEAT::GeneticGeneration> generation)
    {
		TicTacToeStats stats;
        generation->setUserData(stats.toString());
    }

    void TicTacToeGameNoGeomExperiment::addGenerationData(shared_ptr<NEAT::GeneticGeneration> generation,shared_ptr<NEAT::GeneticIndividual> individual)
    {
        TicTacToeStats ticTacToeStats = TicTacToeStats(generation->getUserData());

        if (individual->getUserData().length())
            ticTacToeStats += TicTacToeStats(individual->getUserData());

		generation->setUserData(ticTacToeStats.toString());
    }
}
