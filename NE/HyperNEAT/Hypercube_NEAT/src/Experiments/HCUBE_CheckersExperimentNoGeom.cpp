#include "HCUBE_Defines.h"

#include "Experiments/HCUBE_CheckersExperimentNoGeom.h"

#define CHECKERS_EXPERIMENT_DEBUG (0)

namespace HCUBE
{
    using namespace NEAT;

    CheckersExperimentNoGeom::CheckersExperimentNoGeom(string _experimentName,int _threadID)
            :
            CheckersExperiment(_experimentName,_threadID)
    {
        generateSubstrate();
    }

    GeneticPopulation* CheckersExperimentNoGeom::createInitialPopulation(int populationSize)
    {
        GeneticPopulation *population = new GeneticPopulation();
        vector<GeneticNodeGene> genes;

        genes.push_back(GeneticNodeGene("Bias","NetworkSensor",0,false));

        for (int x=0;x<8;x++)
        {
            for (int y=0;y<8;y++)
            {
                if ((x+y)%2==0)
                {
                    Node node(x,y,0);
                    //cout << (y1-numNodesY/2) << '/' << (x1-numNodesX/2) << endl;
                    string name = (toString(x)+string("/")+toString(y) + string("/") + toString(0));
                    nameLookup[node] = name;
                    genes.push_back(GeneticNodeGene(name,"NetworkSensor",0,false));
                }
            }
        }

        Node node(0,0,2);
        //cout << (y1-numNodesY/2) << '/' << (x1-numNodesX/2) << endl;
        string name = (toString(0)+string("/")+toString(0) + string("/") + toString(2));
        nameLookup[node] = name;
        genes.push_back(GeneticNodeGene(name,"NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));

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

    pair<CheckersNEATDatatype,int> CheckersExperimentNoGeom::evaluateLeafHyperNEAT(
        uchar b[8][8]
    )
    {
#if CHECKERS_EXPERIMENT_ENABLE_BIASES
        NEAT::FastBiasNetwork<CheckersNEATDatatype>* network;

        throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: Can't run NoGeom version of CheckersExperiment with biases turned on!");
#else
        NEAT::FastNetwork<CheckersNEATDatatype>* network;
#endif

        network = &networks[currentSubstrateIndex];

        if (DEBUG_USE_HANDCODED_EVALUATION)
        {
            throw CREATE_LOCATEDEXCEPTION_INFO("WRONG TIME FOR HANDCODED EVALUATION");
        }
        CheckersNEATDatatype output;

#if DEBUG_USE_BOARD_EVALUATION_CACHE
        tmpboard.loadBoard(b);
        ushort hashVal = tmpboard.getShortHash();

        BoardCacheList::iterator bIterator =
            boardEvaluationCaches[currentSubstrateIndex][hashVal].begin();
        BoardCacheList::iterator bEnd =
            boardEvaluationCaches[currentSubstrateIndex][hashVal].end();

        for (;bIterator != bEnd;bIterator++)
        {
            if (bIterator->first == tmpboard)
            {
                //We have a match!
                break;
            }
        }

        if (bIterator != bEnd)
        {
            output = bIterator->second;
        }
        else
#endif
        {

            network->reinitialize();
            network->dummyActivation();

            if (network->hasNode("Bias"))
            {
                network->setValue("Bias",(CheckersNEATDatatype)0.3);
            }

            for (int y=0;y<numNodesY[0];y++)
            {
                for (int x=0;x<numNodesX[0];x++)
                {
                    if ( (x+y)%2==1 ) //ignore empty squares.
                        continue;

                    int boardx = x;
                    int boardy = y;

                    //cout << boardx << ',' << boardy << ": " << b[boardx][boardy] << endl;

                    if ( (b[boardx][boardy]&WHITE) )
                    {
                        //cout << "FOUND WHITE\n";
                        if ( (b[boardx][boardy]&KING) )
                        {
                            network->setValue( getNameFromNode(Node(x,y,0)) , -0.75 );
                        }
                        else if ( (b[boardx][boardy]&MAN) )
                        {
                            network->setValue( getNameFromNode(Node(x,y,0)) , -0.5 );
                        }
                        else
                        {
                            throw CREATE_LOCATEDEXCEPTION_INFO("Could not determine piece type (man/king)!");
                        }
                    }
                    else if ( (b[boardx][boardy]&BLACK) )
                    {
                        //cout << "FOUND BLACK\n";
                        if ( (b[boardx][boardy]&KING) )
                        {
							network->setValue( getNameFromNode(Node(x,y,0)) , 0.75 );
                        }
                        else if ( (b[boardx][boardy]&MAN) )
                        {
                            network->setValue( getNameFromNode(Node(x,y,0)) , 0.5 );
                        }
                        else
                        {
                            throw CREATE_LOCATEDEXCEPTION_INFO("Could not determine piece type (man/king)!");
                        }
                    }
                    else
                    {
                        //cout << "FOUND NOTHING\n";
                        network->setValue( getNameFromNode(Node(x,y,0)) , 0.0 );
                    }

                }
            }

            network->updateFixedIterations(2);
            output = network->getValue(getNameFromNode(Node(0,0,2)));

#if CHECKERS_EXPERIMENT_DEBUG
            static CheckersNEATDatatype prevOutput;

            if (fabs(output-prevOutput)>1e-3)
            {
                prevOutput = output;
                cout << "BOARD RATING:" << output << endl;
                CREATE_PAUSE("");
            }
#endif

#if DEBUG_USE_BOARD_EVALUATION_CACHE
            if (boardEvaluationCaches[currentSubstrateIndex][hashVal].size()<10000)
            {
                boardEvaluationCaches[currentSubstrateIndex][hashVal].push_back(
                    BoardCachePair(tmpboard,output)
                    );
            }
#endif
        }

#if DEBUG_DUMP_BOARD_LEAF_EVALUATIONS
        cout << "Printing board leaf evaluations\n";

        int whiteMen,blackMen,whiteKings,blackKings;
        countPieces(b,whiteMen,blackMen,whiteKings,blackKings);

        *outfile << "Info for board below: WM:" << whiteMen << " WK:" << whiteKings
            << " BM:" << blackMen << " BK:" << blackKings << endl;
        for (int y=0;y<8;y++)
        {
            for (int x=7;x>=0;x--)
            {
                if ((x+y)%2==1)
                {
                    *outfile << " ";
                }
                else if (b[x][y]&FREE)
                {
                    *outfile << " ";
                }
                else if (b[x][y]&BLACK)
                {
                    if (b[x][y]&MAN)
                    {
                        *outfile << "b";
                    }
                    else
                    {
                        *outfile << "B";
                    }
                }
                else if (b[x][y]&WHITE)
                {
                    if (b[x][y]&MAN)
                    {
                        *outfile << "w";
                    }
                    else
                    {
                        *outfile << "W";
                    }
                }
                else
                {
                    *outfile << "_";
                }
            }
            *outfile << endl;
        }
        *outfile << "BOARD RATING: " << output << endl;

        *outfile << "Hidden layer values:\n";
        for (int y=0;y<numNodesY[1];y++)
        {
            for (int x=0;x<numNodesX[1];x++)
            {
                if (x)
                {
                    *outfile << ", ";
                }

                *outfile << setprecision(3) << substrate->getValue( getNameFromNode(Node(x,y,1)) );
            }
            *outfile << endl;
        }

        *outfile << endl;
#endif

        //ROUNDING: TRY TO ROUND AND SEE IF IT WORKS
        output = floor(output*1000.0+0.50001)/1000.0;

		numHyperNEATEvaluations++;

#if DEBUG_SHOW_HYPERNEAT_ALTERNATIVES
		boardEvaluationList.push_back(BoardEvaluation(b,output));
#endif

        return pair<CheckersNEATDatatype,int>(output,numHyperNEATEvaluations-1);
    }

    void CheckersExperimentNoGeom::generateSubstrate(int substrateNum)
    {}

    void CheckersExperimentNoGeom::populateSubstrate(
        shared_ptr<NEAT::GeneticIndividual> individual,
        int substrateNum
    )
    {
        if (substrateIndividuals[substrateNum]==individual)
        {
            //Don't bother remaking the same substrate
            return;
        }

        substrateIndividuals[substrateNum]=individual;

        //Clear the evaluation cache
        for (int a=0;a<65536;a++)
        {
            boardEvaluationCaches[substrateNum][a].clear();
        }

        //Clear the state list caches
        for (int a=0;a<65536;a++)
        {
            boardStateLists[substrateNum][BLACK][a].clear();
        }
        for (int a=0;a<65536;a++)
        {
            boardStateLists[substrateNum][WHITE][a].clear();
        }

        networks[substrateNum] = individual->spawnFastPhenotypeStack<CheckersNEATDatatype>();
    }

    Experiment* CheckersExperimentNoGeom::clone()
    {
        CheckersExperimentNoGeom* experiment = new CheckersExperimentNoGeom(*this);

        return experiment;
    }
}
