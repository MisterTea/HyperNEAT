#include "HCUBE_Defines.h"

#include "Experiments/HCUBE_OthelloCoExperiment.h"

#ifdef EPLEX_INTERNAL

/*
#define OTHELLO_EXPERIMENT_DEBUG (0)

#define OTHELLO_EXPERIMENT_PRINT_GAMES (0)

#define DEBUG_BOARD_CACHE (0)

#define DEBUG_CHECK_HAND_CODED_HEURISTIC (0)

#define DEBUG_USE_BOARD_EVALUATION_CACHE (1)

#define DEBUG_DUMP_BOARD_LEAF_EVALUATIONS (0)

#define DEBUG_DIRECT_LINKS (0)

#define OTHELLO_EXPERIMENT_DUMP_PLAYER (1)

#define OTHELLO_EXPERIMENT_RECOVER_PLAYER (0)

#define OTHELLO_EXPERIMENT_PRINT_BOARD_RATINGS (0)
*/

#define DEBUG_USE_ABSOLUTE_COORDS (0)

namespace HCUBE
{
    using namespace NEAT;

    OthelloCoExperiment::OthelloCoExperiment(string _experimentName,int _threadID)
            :
            OthelloExperiment(_experimentName,_threadID)
    {
        generateSubstrate(1);
    }

    GeneticPopulation* OthelloCoExperiment::createInitialPopulation(int populationSize)
    {
        GeneticPopulation *population = new GeneticPopulation(
            shared_ptr<OthelloCoExperiment>((OthelloCoExperiment*)this->clone())
        );

        vector<GeneticNodeGene> genes;

        genes.push_back(GeneticNodeGene("Bias","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("X1","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("Y1","NetworkSensor",0,false));
#if DEBUG_USE_ABSOLUTE_COORDS
        genes.push_back(GeneticNodeGene("X2","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("Y2","NetworkSensor",0,false));
#endif
        genes.push_back(GeneticNodeGene("DeltaX","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("DeltaY","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("Output_ab","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
        genes.push_back(GeneticNodeGene("Output_bc","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
#if DEBUG_DIRECT_LINKS
        genes.push_back(GeneticNodeGene("Output_ac","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
#endif
#if OTHELLO_EXPERIMENT_ENABLE_BIASES
        genes.push_back(GeneticNodeGene("Bias_b","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
        genes.push_back(GeneticNodeGene("Bias_c","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
#endif

        for (int a=0;a<populationSize;a++)
        {
            shared_ptr<GeneticIndividual> individual(new GeneticIndividual(genes,true,1.0));

            for (int b=0;b<0;b++)
            {
                individual->testMutate();
            }

            population->addIndividual(individual);
        }

        shared_ptr<NEAT::CoEvoGeneticGeneration> coEvoGeneration =
            static_pointer_cast<NEAT::CoEvoGeneticGeneration>(population->getGeneration());

        //Create the tests
        for (int a=0;a<(population->getIndividualCount()/10);a++)
        {
            shared_ptr<NEAT::GeneticIndividual> individual(new GeneticIndividual(genes,true,1.0));

            //This function clones the individual to make a test
            coEvoGeneration->addTest(individual);
        }

        coEvoGeneration->bootstrap();


        cout << "Finished creating population\n";
        return population;
    }

    pair<double,double> OthelloCoExperiment::playGame(
        shared_ptr<GeneticIndividual> ind1,
        shared_ptr<GeneticIndividual> ind2
    )
    {
        pair<double,double> rewards(10.0,10.0);

        populateSubstrate(ind1,0);
        populateSubstrate(ind2,1);

        /*
                 gameinfo gi;

                 // init gameinfo
                 memset (&gi, 0, sizeof (gi));

                 gi.experiment = this;
                 gi.result = UNKNOWN;

                 gi.player = "HyperNEAT";

                 // check opponent
                 gi.opponent = "simplech";
        */

        ushort b[8][8];
        DEBUG_USE_HANDCODED_EVALUATION = 0;
        DEBUG_USE_HYPERNEAT_EVALUATION = 1;

        double *blackReward,*whiteReward;

        for (int curGame=0;curGame<2;curGame++)
        {
            if (!curGame)
            {
                blackReward = &(rewards.first);
                whiteReward = &(rewards.second);
            }
            else
            {
                blackReward = &(rewards.second);
                whiteReward = &(rewards.first);
            }

            resetBoard(b);

            int retval=OTHELLO_END_UNKNOWN;
            int rounds=0;

            for (
                rounds=0;
                (retval==OTHELLO_END_UNKNOWN);
                rounds++
            )
            {
                //cout << "Round: " << rounds << endl;
                moveToMake = OthelloMove();

                //in the first game, the first individual is black.
                currentSubstrateIndex = curGame;

                evaluatemax(b,OthelloNEATDatatype(INT_MAX/2),0,1);

#if OTHELLO_EXPERIMENT_PRINT_GAMES
                cout << "Playing games with HyperNEAT as black\n";
                cout << "BLACK MAKING MOVE\n";

                printBoard(b);
#endif

                if (moveToMake.position.x==255)
                {
                    //black passes
                }
                else
                {
                    makeMove(moveToMake,b);
                    retval = getWinner(b);
                }

#if OTHELLO_EXPERIMENT_LOG_EVALUATIONS
                memcpy(gameLog[rounds*2],b,sizeof(ushort)*8*8);
#endif

#if OTHELLO_EXPERIMENT_PRINT_GAMES
                printBoard(b);
                CREATE_PAUSE("");
#endif

                if (retval==OTHELLO_END_UNKNOWN)
                {
                    //printBoard(b);

                    moveToMake = OthelloMove();
                    {
                        //progress_timer t;

                        //in the first game, the second invidiaul is white.
                        currentSubstrateIndex = 1-curGame;

                        evaluatemin(b,OthelloNEATDatatype(INT_MAX/2),0,1);
                        //cout << "SimpleOthello time: ";
                    }

                    /*
                    cout << "JGAI SAYS TO MAKE MOVE FROM ("
                    << moveToMake.from.x << ','
                    << moveToMake.from.y << ") -> (" << moveToMake.to.x
                    << ',' << moveToMake.to.y << ")\n";

                    char str[255];
                    int playnow=0;
                    int fromx,fromy,tox,toy;
                    ushort tmpBoard[8][8];
                    memcpy(tmpBoard,b,sizeof(ushort)*8*8);
                    getmove(tmpBoard,WHITE,1000.0,str,&playnow,0,0,fromx,fromy,tox,toy);

                    cout << "AI SAYS TO MAKE MOVE FROM (" << fromx << ','
                    << fromy << ") -> (" << tox << ',' << toy << ")\n";

                    printBoard(b);
                    CREATE_PAUSE("");
                    */

#if OTHELLO_EXPERIMENT_PRINT_GAMES
                    cout << "Playing games with HyperNEAT as black\n";
                    cout << "WHITE MAKING MOVE\n";

                    printBoard(b);
#endif

                    if (moveToMake.position.x==255)
                    {
                        //white passes
                    }
                    else
                    {
                        makeMove(moveToMake,b);
                        retval = getWinner(b);
                    }

#if OTHELLO_EXPERIMENT_PRINT_GAMES
                    printBoard(b);
                    CREATE_PAUSE("");
#endif
                }

#if OTHELLO_EXPERIMENT_LOG_EVALUATIONS
                memcpy(gameLog[rounds*2+1],b,sizeof(ushort)*8*8);
#endif
            }

#if OTHELLO_EXPERIMENT_LOG_EVALUATIONS
            for (int a=0;a<rounds;a++)
            {
                printBoard(gameLog[a*2]);

                //if (a+1<=rounds)
                {
                    //White didn't make the final move because black won
                    printBoard(gameLog[a*2+1]);
                }
            }
            cout << "PlayerType: " << handCodedType << " PlayerDepth: " << handCodedDepth << endl;
#endif

            if (retval==OTHELLO_BLACK)
            {
#if OTHELLO_EXPERIMENT_LOG_EVALUATIONS
                cout << "WE WIN!\n";
#endif
                (*blackReward) += 1200;
            }
            else if (retval==OTHELLO_END_TIE) //draw
            {
#if OTHELLO_EXPERIMENT_LOG_EVALUATIONS
                cout << "WE TIED!\n";
#endif
                (*whiteReward) += 600;
            }
            else //loss
            {
#if OTHELLO_EXPERIMENT_LOG_EVALUATIONS
                cout << "WE LOST!\n";
#endif
            }

            (*blackReward) += (5 * (OTHELLO_GET_NUM_BLACK_PIECES(b)) );
            (*whiteReward) += (5 * (OTHELLO_GET_NUM_WHITE_PIECES(b)) );

#if OTHELLO_EXPERIMENT_LOG_EVALUATIONS
            CREATE_PAUSE("");
#endif
        }

        return rewards;
    }

    void OthelloCoExperiment::processGroup(shared_ptr<NEAT::GeneticGeneration> generation)
    {
        //cout << "Processing group\n";

        shared_ptr<NEAT::CoEvoGeneticGeneration> coEvoGeneration =
            static_pointer_cast<NEAT::CoEvoGeneticGeneration>(generation);

        //Play all the tests, but do not change test fitness
        for (int a=0;a<coEvoGeneration->getTestCount();a++)
        {
            pair<double,double> rewards =
                playGame(
                    group[0],
                    coEvoGeneration->getTest(a)
                );

            group[0]->reward(rewards.first);
        }
    }

    Experiment* OthelloCoExperiment::clone()
    {
        OthelloCoExperiment* experiment = new OthelloCoExperiment(*this);

        return experiment;
    }
}

#endif
