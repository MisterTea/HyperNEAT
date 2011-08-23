#include "HCUBE_Defines.h"

#include "Experiments/HCUBE_CoCheckersExperiment.h"

#ifdef EPLEX_INTERNAL

#define DEBUG_GAME_ANNOUNCER (0)

#define COCHECKERS_EXPERIMENT_DEBUG (0)

using namespace NEAT;

namespace HCUBE
{
    CoCheckersExperiment::CoCheckersExperiment(string _experimentName,int _threadID)
            :
            CheckersExperiment(_experimentName,_threadID)
    {
        DEBUG_USE_HYPERNEAT_EVALUATION=1;
    }

    NEAT::GeneticPopulation* CoCheckersExperiment::createInitialPopulation(int populationSize)
    {
        GeneticPopulation *population = new GeneticPopulation(
            shared_ptr<CoCheckersExperiment>((CoCheckersExperiment*)this->clone())
        );
        vector<GeneticNodeGene> genes;

        genes.push_back(GeneticNodeGene("Bias","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("X1","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("Y1","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("X2","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("Y2","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("DeltaX","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("DeltaY","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("Output_ab","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
        genes.push_back(GeneticNodeGene("Output_bc","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
        genes.push_back(GeneticNodeGene("Output_ac","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
#if CHECKERS_EXPERIMENT_ENABLE_BIASES
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

    /*
    CheckersNEATDatatype CoCheckersExperiment::evaluateLeafWhite(uchar b[8][8])
    {
        throw CREATE_LOCATEDEXCEPTION_INFO("YOU SHUOLD NEVER HAVE WHITE AS A LEAF NODE!");

        return 0.0f;
    }
    */

    pair<double,double> CoCheckersExperiment::playGame(
        shared_ptr<NEAT::GeneticIndividual> ind1,
        shared_ptr<NEAT::GeneticIndividual> ind2
    )
    {
        //You get 1 point just for entering the game, wahooo!
        pair<double,double> rewards(1.0,1.0);

#if DEBUG_GAME_ANNOUNCER
        cout << "Playing game\n";
#endif

        populateSubstrate(ind1,0);
        populateSubstrate(ind2,1);

        uchar b[8][8];

        //cout << "Playing games with HyperNEAT as black\n";
        //for (handCodedType=0;handCodedType<5;handCodedType++)

        for (testCases=0;testCases<2;testCases++)
        {
            if (testCases==0)
            {
                individualBlack = ind1;
                individualWhite = ind2;
            }
            else //testCases==1
            {
                individualBlack = ind2;
                individualWhite = ind1;
            }

            resetBoard(b);

            int retval=-1;
            int rounds=0;

            for (rounds=0;rounds<CHECKERS_MAX_ROUNDS&&retval==-1;rounds++)
            {
                //cout << "Round: " << rounds << endl;
                moveToMake = CheckersMove();

                if (testCases==0)
                {
                    currentSubstrateIndex=0;
                }
                else //testCases==1
                {
                    currentSubstrateIndex=1;
                }

                //cout << "Black is thinking...\n";
                evaluatemax(b,CheckersNEATDatatype(INT_MAX/2),0,2);

#if CHECKERS_EXPERIMENT_DEBUG
                cout << "BLACK MAKING MOVE\n";

                printBoard(b);
#endif

                if (moveToMake.from.x==255)
                {
                    //black loses
                    cout << "BLACK LOSES!\n";
                    retval = WHITE;
                }
                else
                {
                    makeMove(moveToMake,b);
                    retval = getWinner(b,WHITE);
                }

#if CHECKERS_EXPERIMENT_LOG_EVALUATIONS
                memcpy(gameLog[rounds*2],b,sizeof(uchar)*8*8);
#endif

#if COCHECKERS_EXPERIMENT_DEBUG
                printBoard(b);
                CREATE_PAUSE("");
#endif

                if (retval==-1)
                {
                    //printBoard(b);

                    moveToMake = CheckersMove();
                    {
                        //progress_timer t;
                        if (testCases==0)
                        {
                            currentSubstrateIndex=1;
                        }
                        else //testCases==1
                        {
                            currentSubstrateIndex=0;
                        }

                        //cout << "White is thinking...\n";
                        evaluatemin(b,CheckersNEATDatatype(INT_MAX/2),0,3);
                        //cout << "SimpleCheckers time: ";
                    }

#if COCHECKERS_EXPERIMENT_DEBUG
                    cout << "WHITE MAKING MOVE\n";

                    printBoard(b);
#endif

                    if (moveToMake.from.x==255)
                    {
                        //white loses
                        cout << "WHITE LOSES BECAUSE THERE'S NO MOVES LEFT!\n";
                        retval = BLACK;
#if COCHECKERS_EXPERIMENT_DEBUG
                        printBoard(b);
                        CREATE_PAUSE("");
#endif
                    }
                    else
                    {
                        makeMove(moveToMake,b);
                        retval = getWinner(b,BLACK);
                    }

#if COCHECKERS_EXPERIMENT_DEBUG
                    printBoard(b);
                    CREATE_PAUSE("");
#endif
                }

#if CHECKERS_EXPERIMENT_LOG_EVALUATIONS
                memcpy(gameLog[rounds*2+1],b,sizeof(uchar)*8*8);
#endif
            }

            if (retval==BLACK)
            {
#if DEBUG_GAME_ANNOUNCER
                cout << "BLACK WON!\n";
#endif
                if (ind1==individualBlack)
                {
                    rewards.first += 800;
                    rewards.first += (CHECKERS_MAX_ROUNDS-rounds);
                }
                else
                {
                    rewards.second += 800;
                    rewards.second += (CHECKERS_MAX_ROUNDS-rounds);
                }

            }
            else if (retval==-1) //draw
            {
#if DEBUG_GAME_ANNOUNCER
                cout << "WE TIED!\n";
#endif
                //rewards.first += 200;
                //rewards.second += 200;
            }
            else //White wins
            {
#if DEBUG_GAME_ANNOUNCER
                cout << "WHITE WON\n";
#endif
                if (ind1==individualWhite)
                {
                    rewards.first += 800;
                    rewards.first += (CHECKERS_MAX_ROUNDS-rounds);
                }
                else
                {
                    rewards.second += 800;
                    rewards.second += (CHECKERS_MAX_ROUNDS-rounds);
                }
            }

            int whiteMen,blackMen,whiteKings,blackKings;

            //countPieces(gi.board,whiteMen,blackMen,whiteKings,blackKings);
            countPieces(b,whiteMen,blackMen,whiteKings,blackKings);

            if (ind1==individualWhite)
            {
                rewards.first += (2 * (whiteMen) );
                rewards.first += (3 * (whiteKings) );

                rewards.second += (2 * (blackMen) );
                rewards.second += (3 * (blackKings) );
            }
            else
            {
                rewards.first += (2 * (blackMen) );
                rewards.first += (3 * (blackKings) );

                rewards.second += (2 * (whiteMen) );
                rewards.second += (3 * (whiteKings) );
            }
        }

#if DEBUG_GAME_ANNOUNCER
        cout << "Fitness earned: " << rewards.first << " & " << rewards.second << endl;
        CREATE_PAUSE("");
#endif

        return rewards;
    }

    void CoCheckersExperiment::processGroup(shared_ptr<NEAT::GeneticGeneration> generation)
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

    void CoCheckersExperiment::processIndividualPostHoc(shared_ptr<NEAT::GeneticIndividual> individual)
    {
        clearGroup();
        addIndividualToGroup(individual);
        shared_ptr<GeneticGeneration> dummy;
        CheckersExperiment::processGroup(dummy);

        individual->setUserData(toString(individual->getFitness()));
        cout << "INDIVIDUAL FITNESS: " << individual->getFitness() << endl;
    }

    Experiment* CoCheckersExperiment::clone()
    {
        CoCheckersExperiment* experiment = new CoCheckersExperiment(*this);

        return experiment;
    }

    void CoCheckersExperiment::addGenerationData(
        shared_ptr<NEAT::GeneticGeneration> generation,
        shared_ptr<NEAT::GeneticIndividual> individual
    )
    {
    }
}

#endif
