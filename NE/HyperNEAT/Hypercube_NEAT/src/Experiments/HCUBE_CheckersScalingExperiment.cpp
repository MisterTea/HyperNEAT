#include "HCUBE_Defines.h"

#include "Experiments/HCUBE_CheckersScalingExperiment.h"

#include "Experiments/HCUBE_CheckersExperiment.h"
#include "Experiments/HCUBE_CheckersExperimentNoGeom.h"

#define CHECKERS_EXPERIMENT_DEBUG (0)

#define DEBUG_BOARD_CACHE (0)

#define DEBUG_CHECK_HAND_CODED_HEURISTIC (0)

#define DEBUG_USE_BOARD_EVALUATION_CACHE (1)

#define DEBUG_DUMP_BOARD_LEAF_EVALUATIONS (0)

#define DEBUG_DIRECT_LINKS (0)

#define DEBUG_USE_ABSOLUTE (1)

#define DEBUG_USE_DELTAS (1)

extern int SMART_ATTACKER;
extern int SMART_DEFENDER;

namespace HCUBE
{
    extern ofstream *outfile;

    using namespace NEAT;

    CheckersScalingExperiment::CheckersScalingExperiment(string _experimentName,int _threadID)
            :
            CheckersExperiment(_experimentName,_threadID),
            currentSubstrateIndex(0),
            substrateBaseX(0),
            substrateBaseY(0)
    {
        //boardEvaluationCaches[0].reserve(10000);
        //boardEvaluationCaches[1].reserve(10000);

        mininumNodesX[0] = mininumNodesY[0] = 3;
        mininumNodesX[1] = mininumNodesY[1] = 1;

        generateMiniSubstrate();

        //resetBoard(userEvaluationBoard);
        //userEvaluationRound = (0);

        //boardStateCache.reserve(10000);
    }

    GeneticPopulation* CheckersScalingExperiment::createInitialPopulation(int populationSize)
    {
        GeneticPopulation *population = new GeneticPopulation();
        vector<GeneticNodeGene> genes;

        genes.push_back(GeneticNodeGene("Bias","NetworkSensor",0,false));

        genes.push_back(GeneticNodeGene("X1","NetworkSensor",0,true));
        genes.back().setEnabled(false);
        genes.back().setTopologyFrozen(true);

        genes.push_back(GeneticNodeGene("Y1","NetworkSensor",0,true));
        genes.back().setEnabled(false);
        genes.back().setTopologyFrozen(true);

#if DEBUG_USE_ABSOLUTE
        genes.push_back(GeneticNodeGene("X2","NetworkSensor",0,true));
        genes.back().setEnabled(false);
        genes.back().setTopologyFrozen(true);

        genes.push_back(GeneticNodeGene("Y2","NetworkSensor",0,true));
        genes.back().setEnabled(false);
        genes.back().setTopologyFrozen(true);
#endif
#if DEBUG_USE_DELTAS
        genes.push_back(GeneticNodeGene("DeltaX","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("DeltaY","NetworkSensor",0,false));
#endif
        genes.push_back(GeneticNodeGene("Output_ab","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
        genes.push_back(GeneticNodeGene("Output_bc","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
#if DEBUG_DIRECT_LINKS
        genes.push_back(GeneticNodeGene("Output_ac","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
#endif
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

        cout << "Finished creating population\n";
        return population;
    }

    void CheckersScalingExperiment::generateMiniSubstrate(int substrateNum)
    {
#if CHECKERS_EXPERIMENT_ENABLE_BIASES
        NEAT::FastBiasNetwork<CheckersNEATDatatype>* substrate;
#else
        NEAT::FastNetwork<CheckersNEATDatatype>* substrate;
#endif

        substrate = &minisubstrates[substrateNum];

        map<Node,NEAT::NetworkNode*> nodeLookup;

        cout << "Generating substrate...";
        boost::object_pool<NEAT::NetworkNode> networkNodePool;
        boost::object_pool<NEAT::NetworkLink> networkLinkPool;

        NEAT::NetworkNode *nodes = NULL;
        NEAT::NetworkLink *links = NULL;
#if CHECKERS_EXPERIMENT_ENABLE_BIASES
        CheckersNEATDatatype *nodeBiases = NULL;
#endif

        int nodeCount =
            (mininumNodesX[0]*mininumNodesY[0]) +
            (mininumNodesX[1]*mininumNodesY[1]);

        //In the future the first part can be divided by 2 because half are invalid (from free squares) but it's really hard to pull off
        int linkCount =
            (mininumNodesX[0]*mininumNodesY[0]*mininumNodesX[1]*mininumNodesY[1]);

        try
        {
            nodes = (NEAT::NetworkNode*)malloc(sizeof(NEAT::NetworkNode)*nodeCount);
            links = (NEAT::NetworkLink*)malloc(sizeof(NEAT::NetworkLink)*linkCount);
#if CHECKERS_EXPERIMENT_ENABLE_BIASES
            nodeBiases = new CheckersNEATDatatype[nodeCount];
#endif
        }
        catch(const std::exception &e)
        {
            cout << e.what() << endl;
            CREATE_PAUSE(string("Error!: ")+toString(__LINE__));
        }

        mininameLookup.clear();

        cout << "Creating nodes...\n";

        int nodeCounter=0;

        for (int z=0;z<2;z++)
        {
            for (int y1=0;y1<mininumNodesY[z];y1++)
            {
                for (int x1=0;x1<mininumNodesX[z];x1++)
                {
                    if (x1==7 && y1==7 && z==1)
                    {
                        int breakme=0;
                    }

                    int xmod = x1;
                    int ymod = y1;
                    Node node(xmod,ymod,z);
                    //cout << (y1-mininumNodesY/2) << '/' << (x1-mininumNodesX/2) << endl;
                    string name = (toString(xmod)+string("/")+toString(ymod) + string("/") + toString(z));
                    mininameLookup[node] = name;
                    bool update;
                    if (z==0)
                    {
                        update=false; //base level nodes are constant
                    }
                    else
                    {
                        update=true;
                    }

                    if (z==2)
                    {
                        new (&nodes[nodeCounter]) NetworkNode(name,update);
                    }
                    else
                    {
                        new (&nodes[nodeCounter]) NetworkNode(name,update);
                    }

                    nodeLookup[node] = &nodes[nodeCounter];
#if CHECKERS_EXPERIMENT_ENABLE_BIASES
                    nodeBiases[nodeCounter]=0.0;
#endif
                    if (nodeCounter>=nodeCount)
                    {
                        throw CREATE_LOCATEDEXCEPTION_INFO("Too many nodes for what was allocated!");
                    }
                    nodeCounter++;
                }
            }
        }

        cout << "Creating links...\n";

        int linkCounter=0;

        for (int z1=0;z1<2;z1++)
        {
            for (int z2=z1+1;z2<2;z2++)
            {
                for (int y1=0;y1<mininumNodesY[z1];y1++)
                {
                    for (int x1=0;x1<mininumNodesX[z1];x1++)
                    {
                        for (int y2=0;y2<mininumNodesY[z2];y2++)
                        {
                            for (int x2=0;x2<mininumNodesX[z2];x2++)
                            {
#if DEBUG_DIRECT_LINKS
#else
                                if (z1==0 && z2==2)
                                {
                                    continue;
                                }
#endif
                                if (
                                    nodeLookup.end()==nodeLookup.find(Node(x1,y1,z1)) ||
                                    nodeLookup.end()==nodeLookup.find(Node(x2,y2,z2))
                                )
                                {
                                    throw CREATE_LOCATEDEXCEPTION_INFO("COULD NOT FIND NODE FROM LOCATION!");
                                }

                                if (linkCounter>=linkCount)
                                {
                                    throw CREATE_LOCATEDEXCEPTION_INFO("Too many links for what was allocated!");
                                }

                                NetworkNode *n1 = nodeLookup[Node(x1,y1,z1)];

                                int n1pos = int(n1-&nodes[0]);

                                if (n1pos>=nodeCounter)
                                {
                                    throw CREATE_LOCATEDEXCEPTION_INFO("INVALID NODE 1");
                                }

                                NetworkNode *n2 = nodeLookup[Node(x2,y2,z2)];

                                int n2pos = int(n2-&nodes[0]);

                                if (n2pos>=nodeCounter)
                                {
                                    throw CREATE_LOCATEDEXCEPTION_INFO("INVALID NODE 2");
                                }

                                new (&links[linkCounter]) NetworkLink(
                                    nodeLookup[Node(x1,y1,z1)],
                                    nodeLookup[Node(x2,y2,z2)],
                                    0.0
                                );

                                linkCounter++;
                            }
                        }
                    }
                }
            }
        }

#if CHECKERS_EXPERIMENT_ENABLE_BIASES
        cout << "Creating FastBiasNetwork\n";

        (*substrate) = NEAT::FastBiasNetwork<CheckersNEATDatatype>(
                           nodes,
                           nodeCounter,
                           links,
                           linkCounter,
                           nodeBiases
                       );
#else
        cout << "Creating FastNetwork\n";
        (*substrate) = NEAT::FastNetwork<CheckersNEATDatatype>(
                           nodes,
                           nodeCounter,
                           links,
                           linkCounter
                       );
#endif

        for (int a=0;a<nodeCounter;a++)
        {
            nodes[a].~NetworkNode();
        }

        for (int a=0;a<linkCounter;a++)
        {
            links[a].~NetworkLink();
        }

        free(nodes);
        free(links);
#if CHECKERS_EXPERIMENT_ENABLE_BIASES
        delete[] nodeBiases;
#endif
    }

    void CheckersScalingExperiment::populateMiniSubstrate(
        shared_ptr<const NEAT::GeneticIndividual> individual,
        int substrateNum
    )
    {
#if CHECKERS_EXPERIMENT_ENABLE_BIASES
        NEAT::FastBiasNetwork<CheckersNEATDatatype>* minisubstrate;
#else
        NEAT::FastNetwork<CheckersNEATDatatype>* minisubstrate;
#endif
        if (minisubstrateIndividuals[substrateNum]==individual)
        {
            //Don't bother remaking the same substrate
            return;
        }

        minisubstrateIndividuals[substrateNum]=individual;

        minisubstrate = &minisubstrates[substrateNum];

        //Clear the evaluation cache
        //boardEvaluationCaches[substrateNum].clear();


        NEAT::FastNetwork<CheckersNEATDatatype> network = individual->spawnFastPhenotypeStack<CheckersNEATDatatype>();

        int linkCounter=0;

#if CHECKERS_EXPERIMENT_DEBUG
        double linkChecksum=0.0;
#endif

        for (int z1=0;z1<2;z1++)
        {
            for (int z2=z1+1;z2<2;z2++)
            {
                for (int y1=0;y1<mininumNodesY[z1];y1++)
                {
                    for (int x1=0;x1<mininumNodesX[z1];x1++)
                    {
                        for (int y2=0;y2<mininumNodesY[z2];y2++)
                        {
                            for (int x2=0;x2<mininumNodesX[z2];x2++)
                            {
                                /*Remap the nodes to the [-1,1] domain*/
                                CheckersNEATDatatype x1normal,y1normal,x2normal,y2normal;

                                /*
                                if (mininumNodesX[z1]>1)
                                {
                                 x1normal = -1.0 + (CheckersNEATDatatype(x1)/(mininumNodesX[z1]-1))*2.0;
                                }
                                else
                                {
                                 x1normal = 0.0;
                                }

                                if (mininumNodesY[z1]>1)
                                {
                                 y1normal = -1.0 + (CheckersNEATDatatype(y1)/(mininumNodesY[z1]-1))*2.0;
                                }
                                else
                                {
                                 y1normal = 0.0;
                                }

                                if (mininumNodesX[z2]>1)
                                {
                                 x2normal = -1.0 + (CheckersNEATDatatype(x2)/(mininumNodesX[z2]-1))*2.0;
                                }
                                else
                                {
                                 x2normal = 0.0;
                                }

                                if (mininumNodesY[z2]>1)
                                {
                                 y2normal = -1.0 + (CheckersNEATDatatype(y2)/(mininumNodesY[z2]-1))*2.0;
                                }
                                else
                                {
                                 y2normal = 0.0;
                                }
                                */

                                if(mininumNodesX[z1]==1)
                                {
                                    x1normal = 0;
                                    y1normal = 0;
                                }
                                else
                                {
                                    x1normal = -1.0 + (CheckersNEATDatatype(x1)/(mininumNodesX[z1]-1))*2.0;
                                    y1normal = -1.0 + (CheckersNEATDatatype(y1)/(mininumNodesY[z1]-1))*2.0;
                                }

                                if(mininumNodesX[z2]==1)
                                {
                                    x2normal = 0;
                                    y2normal = 0;
                                }
                                else
                                {
                                    x2normal = -1.0 + (CheckersNEATDatatype(x2)/(mininumNodesX[z2]-1))*2.0;
                                    y2normal = -1.0 + (CheckersNEATDatatype(y2)/(mininumNodesY[z2]-1))*2.0;
                                }


                                //Coordinates are still based on full checkerboard space

                                if (z1==0 || z1==2)
                                {
                                    x1normal *= (CheckersNEATDatatype)(2.0/(8.0-1.0));
                                    y1normal *= (CheckersNEATDatatype)(2.0/(8.0-1.0));
                                }
                                else if (z1==1)
                                {
                                    x1normal *= (CheckersNEATDatatype)(2.0/(8.0-1.0));
                                    y1normal *= (CheckersNEATDatatype)(2.0/(8.0-1.0));
                                }

                                if (z2==0 || z2==2)
                                {
                                    x2normal *= (CheckersNEATDatatype)(2.0/(8.0-1.0));
                                    y2normal *= (CheckersNEATDatatype)(2.0/(8.0-1.0));
                                }
                                else if (z2==1)
                                {
                                    x2normal *= (CheckersNEATDatatype)(2.0/(8.0-1.0));
                                    y2normal *= (CheckersNEATDatatype)(2.0/(8.0-1.0));
                                }

                                //Translate to fit a specific substrate location
                                x1normal += substrateBaseX;
                                y1normal += substrateBaseY;

                                x2normal += substrateBaseX;
                                y2normal += substrateBaseY;

                                network.reinitialize();

                                if (network.hasNode("X1"))
                                {
                                    network.setValue("X1",x1normal);
                                    network.setValue("Y1",y1normal);
                                }
                                if (network.hasNode("X2"))
                                {
                                    network.setValue("X2",x2normal);
                                    network.setValue("Y2",y2normal);
                                }

                                //In the small board, a delta of '1' corresponds
                                //to a delta of '2/7' on the big board because
                                //the big board is 7/2 times bigger

                                if (network.hasNode("DeltaX"))
                                {
                                    //cout << x1 << ',' << x2 << ',' << y1 << ',' << y2 << endl;
                                    //cout << x1normal << ',' << x2normal << ',' << y1normal << ',' << y2normal << endl;
                                    //cout << "DeltaX:" << (x2normal-x1normal)
                                        //<< ", DeltaY: " << (y2normal-y1normal)
                                        //<< endl;
                                    network.setValue("DeltaX",(x2normal-x1normal));
                                    network.setValue("DeltaY",(y2normal-y1normal));
                                }
                                network.setValue("Bias",(CheckersNEATDatatype)0.3);
                                network.update();

                                CheckersNEATDatatype output;

                                if (z1==0 && z2==1)
                                {
                                    output = network.getValue("Output_ab");
                                }
                                else if (z1==1 && z2==2)
                                {
                                    output = network.getValue("Output_bc");
                                }
                                else if (z1==0 && z2==2)
                                {
#if DEBUG_DIRECT_LINKS
                                    output = network.getValue("Output_ac");
#else
                                    output = 0;
#endif
                                }
                                else
                                {
                                    throw CREATE_LOCATEDEXCEPTION_INFO("wtf");
                                }

                                output = convertOutputToWeight(output);

                                /*
                                if (z1)
                                {
                                cout << x2normal << ',' << y2normal << endl;
                                cout << "Setting weight for "
                                << mininameLookup[Node(x1-mininumNodesX[z1]/2,y1-mininumNodesY[z1]/2,z1)] << " -> "
                                << mininameLookup[Node(x2-mininumNodesX[z2]/2,y2-mininumNodesY[z2]/2,z2)]
                                << endl;
                                cout << "Weight: " << output << endl;
                                cout << "Outputs: " << network.getValue("Output_ab") << ','
                                << network.getValue("Output_bc") << endl;
                                CREATE_PAUSE("");
                                }
                                */

                                //if (z1)
                                {
                                    //To account for the fact that so many nodes are merging into 1 node.
                                    //TODO: Try to find a more intuitive way to do this

                                    //output /= mininumNodesX[z1];
                                    //output /= mininumNodesY[z1];


                                    //output *= mininumNodesX[z2];
                                    //output *= mininumNodesY[z2];
                                }

                                if (
                                    (mininameLookup.find(Node(x1,y1,z1))==mininameLookup.end()) ||
                                    (mininameLookup.find(Node(x2,y2,z2))==mininameLookup.end())
                                )
                                {
                                    throw CREATE_LOCATEDEXCEPTION_INFO("COULD NOT FIND A NODE'S NAME!");
                                }

#if DEBUG_DIRECT_LINKS
#else
                                if (! (z1==0 && z2==2) )
                                {
#endif
                                minisubstrate->getLink(
                                    mininameLookup[Node(x1,y1,z1)],
                                    mininameLookup[Node(x2,y2,z2)]
                                )->weight = output;
                                //cout << "Set link weight to: " << output << endl;
#if CHECKERS_EXPERIMENT_DEBUG
                                linkChecksum += output;
#endif

#if DEBUG_DIRECT_LINKS
#else
                            }
#endif

                                linkCounter++;

#if CHECKERS_EXPERIMENT_ENABLE_BIASES
                                if (x1==0&&y1==0&&z1==0)
                                {
                                    CheckersNEATDatatype nodeBias;

                                    if (z2==1)
                                    {
                                        nodeBias = network.getValue("Bias_b");

                                        nodeBias = convertOutputToWeight(nodeBias);

                                        /*{
                                        cout << "Setting bias for "
                                        << mininameLookup[Node(x2-mininumNodesX[z2]/2,y2-mininumNodesY[z2]/2,1)]
                                        << endl;
                                        cout << "Bias: " << nodeBias << endl;
                                        CREATE_PAUSE("");
                                        }*/

                                        minisubstrate->setBias(
                                            *mininameLookup.getData(Node(x2,y2,z2)),
                                            nodeBias
                                        );
                                    }
                                    else if (z2==2)
                                    {
                                        nodeBias = network.getValue("Bias_c");

                                        nodeBias = convertOutputToWeight(nodeBias);

                                        /*{
                                        cout << "Setting bias for "
                                        << mininameLookup[Node(x2-mininumNodesX[z2]/2,y2-mininumNodesY[z2]/2,2)]
                                        << endl;
                                        cout << "Bias: " << nodeBias << endl;
                                        CREATE_PAUSE("");
                                        }*/

                                        minisubstrate->setBias(
                                            *mininameLookup.getData(Node(x2,y2,z2)),
                                            nodeBias
                                        );
                                    }
                                    else
                                    {
                                        throw CREATE_LOCATEDEXCEPTION_INFO("wtf");
                                    }
                                }
#endif
                            }
                        }
                    }
                }
            }
        }
    }

#define SCALE_UP_GENERATION (100)

    void CheckersScalingExperiment::processGroup(shared_ptr<NEAT::GeneticGeneration> generation)
    {
        shared_ptr<NEAT::GeneticIndividual> individual = group.front();

        if(generation->getGenerationNumber()==SCALE_UP_GENERATION)
        {
            NEAT::GeneticNodeGene *x1 = individual->getNode("X1");
            if(x1)
            {
                individual->getNode("X1")->setTopologyFrozen(false);
                individual->getNode("X1")->setEnabled(true);
                individual->addLink(GeneticLinkGene(x1->getID(),individual->getNode("Output_ab")->getID()));
                individual->addLink(GeneticLinkGene(x1->getID(),individual->getNode("Output_bc")->getID()));
            }
            NEAT::GeneticNodeGene *x2 = individual->getNode("X2");
            if(individual->getNode("X2"))
            {
                individual->getNode("X2")->setTopologyFrozen(false);
                individual->getNode("X2")->setEnabled(true);
                individual->addLink(GeneticLinkGene(x2->getID(),individual->getNode("Output_ab")->getID()));
                individual->addLink(GeneticLinkGene(x2->getID(),individual->getNode("Output_bc")->getID()));
            }
            NEAT::GeneticNodeGene *y1 = individual->getNode("Y1");
            if(individual->getNode("Y1"))
            {
                individual->getNode("Y1")->setTopologyFrozen(false);
                individual->getNode("Y1")->setEnabled(true);
                individual->addLink(GeneticLinkGene(y1->getID(),individual->getNode("Output_ab")->getID()));
                individual->addLink(GeneticLinkGene(y1->getID(),individual->getNode("Output_bc")->getID()));
            }
            NEAT::GeneticNodeGene *y2 = individual->getNode("Y2");
            if(individual->getNode("Y2"))
            {
                individual->getNode("Y2")->setTopologyFrozen(false);
                individual->getNode("Y2")->setEnabled(true);
                individual->addLink(GeneticLinkGene(y2->getID(),individual->getNode("Output_ab")->getID()));
                individual->addLink(GeneticLinkGene(y2->getID(),individual->getNode("Output_bc")->getID()));
            }

        }

        if(generation->getGenerationNumber()>=SCALE_UP_GENERATION)
        {
            CheckersExperiment::processGroup(generation);
            return;
        }

        //You get 10 points just for entering the game, wahooo!
        individual->setFitness(10);

        populateMiniSubstrate(individual);

        /*
        static const int NUM_TESTS = 3;

        static const double testCases[NUM_TESTS][3][3] =
        {
         {
          { 0.0,  0.0,  0.0},
          { 0.0,  0.5,  0.0},
          { 0.0,  0.0,  0.0}
         },
         {
          { 0.0,  0.0,  0.0},
          { 0.0, -0.5,  0.0},
          { 0.0,  0.0,  0.0}
         },
         {
          { 0.0,  0.0,  0.0},
          { 0.0, 0.0,  0.0},
          { 0.0,  0.0,  0.0}
         }
        };

        static const double testScores[NUM_TESTS] =
        {
         0.5,
          -0.5,
          0.0
        };
        */

        static const int NUM_TESTS = 8;

        static const double testCases[NUM_TESTS][3][3] =
        {
            {
                { 0.0,  0.0,  0.0},
                { 0.0,  0.5,  0.0},
                { 0.0,  0.0,  0.0}
            },
            {
                { 0.0,  0.0,  0.0},
                { 0.0,  -0.5,  0.0},
                { 0.0,  0.0,  0.0}
            },
            {
                { 0.0,  0.0,  0.0},
                { 0.0,  0.0,  0.0},
                { 0.0,  0.0,  0.0}
            },
            {
                { 0.5,  0.0,  0.0},
                { 0.0,  -0.5,  0.0},
                { 0.0,  0.0,  0.0}
            },
            {
                { 0.5,  0.0,  0.5},
                { 0.0, -0.5,  0.0},
                { 0.0,  0.0,  0.0}
            },
            {
                { 0.0,  0.0,  0.5},
                { 0.0, -0.5,  0.0},
                { -0.5,  0.0,  0.0}
            },
            {
                { 0.5,  0.0,  0.0},
                { 0.0, -0.5,  0.0},
                { 0.0,  0.0,  -0.5}
            },
            {
                { 0.5,  0.0,  0.5},
                { 0.0, -0.5,  0.0},
                { -0.5,  0.0,  -0.5}
            }
        };

        static const double testScores[NUM_TESTS] =
        {
            0.5,
            -0.5,
            0.0,
            0.7,
            0.7,
            0.0,
            0.0,
            0.0
        };

#if CHECKERS_EXPERIMENT_ENABLE_BIASES
        NEAT::FastBiasNetwork<CheckersNEATDatatype>* substrate;
#else
        NEAT::FastNetwork<CheckersNEATDatatype>* substrate;
#endif

        substrate = &minisubstrates[currentSubstrateIndex];

        CheckersNEATDatatype output;

        bool failedTest = false;

        for (int a=0;a<NUM_TESTS;a++)
        {
            substrate->reinitialize();
            substrate->dummyActivation();

            for (int y=0;y<3;y++)
            {
                for (int x=0;x<3;x++)
                {
                    //if ( (x+y)%2==1 ) //ignore empty squares.
                        //continue;

                    int boardx = x;
                    int boardy = y;

                    substrate->setValue( getMiniNameFromNode(Node(x,y,0)) , testCases[a][y][x] );

                }
            }

            //cout << "***UPDATING SUBSTRATE***\n";
            substrate->updateFixedIterations(1);

            output = substrate->getValue(getMiniNameFromNode(Node(0,0,1)));

            CheckersNEATDatatype outputDifference = fabs(output-testScores[a]);

            if (outputDifference<0.05)
            {
                //Keep from overspecializing
                outputDifference=0.0;
            }

            if (outputDifference>(CheckersNEATDatatype)0.2)
            {
                failedTest=true;
            }

            CheckersNEATDatatype rewardValue = 500*(2-outputDifference);
            individual->reward(rewardValue);
        }

        if (!failedTest)
        {
            cout << "INDIVIDUAL PASSED ALL TESTS!!!!!!!\n";
        }

        //cout << "Trial over! score: " << individual->getFitness() << "\n";
    }

    void CheckersScalingExperiment::processIndividualPostHoc(shared_ptr<NEAT::GeneticIndividual> individual)
    {
        cout << "Performing Posthoc process\n";
        //cout << "INDIVIDUAL FITNESS BEFORE: " << individual->getFitness() << endl;
        CheckersExperiment* tmpExperiment = new CheckersExperiment(experimentName,threadID);
        tmpExperiment->clearGroup();
        tmpExperiment->addIndividualToGroup(individual);
        individual->setUserData(CheckersStats().toString());

        for (int a=0;a<100;a++)
        {
            shared_ptr<GeneticGeneration> dummy;
            tmpExperiment->setChanceToMakeSecondBestMove(0.10);
            //Force CheckersExperiment processGroup
            tmpExperiment->processGroup(dummy);
            tmpExperiment->setChanceToMakeSecondBestMove(0.0);
            //cout << "INDIVIDUAL FITNESS: " << individual->getFitness() << endl;
        }
    }

    Experiment* CheckersScalingExperiment::clone()
    {
        CheckersScalingExperiment* experiment = new CheckersScalingExperiment(*this);

        return experiment;
    }
}
