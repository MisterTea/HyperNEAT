#include "HCUBE_Defines.h"

#include "Experiments/HCUBE_OthelloExperiment.h"

#define OTHELLO_EXPERIMENT_DEBUG (0)

#define OTHELLO_EXPERIMENT_PRINT_GAMES (0)

#define DEBUG_BOARD_CACHE (0)

#define DEBUG_CHECK_HAND_CODED_HEURISTIC (0)

#define DEBUG_USE_BOARD_EVALUATION_CACHE (1)

#define DEBUG_DUMP_BOARD_LEAF_EVALUATIONS (0)

#define DEBUG_DIRECT_LINKS (0)

#define MAX_GAME_COUNT (90)

#define OTHELLO_EXPERIMENT_DUMP_PLAYER (0)

#define OTHELLO_EXPERIMENT_RECOVER_PLAYER (0)

#define OTHELLO_EXPERIMENT_PRINT_BOARD_RATINGS (0)

#define OTHELLO_EXPERIMENT_USE_TEMPO (0)

#define DEBUG_USE_ABSOLUTE_COORDS (1)

#define DEBUG_USE_DELTAS (0)

namespace HCUBE
{
	extern ofstream *outfile;
	using namespace NEAT;

	OthelloExperiment::OthelloExperiment(string _experimentName,int _threadID)
		:
	Experiment(_experimentName,_threadID),
		currentSubstrateIndex(0),
		DEBUG_USE_HANDCODED_EVALUATION(0),
		DEBUG_USE_HYPERNEAT_EVALUATION(0)
	{
		numNodesX[0] = numNodesY[0] = 8;
		numNodesX[1] = numNodesY[1] = 8;
		numNodesX[2] = numNodesY[2] = 1;

		generateSubstrate();

		resetBoard(userEvaluationBoard);
		userEvaluationRound = (0);

		//boardStateCache.reserve(10000);
	}

	GeneticPopulation* OthelloExperiment::createInitialPopulation(int populationSize)
	{
		GeneticPopulation *population = new GeneticPopulation();
		vector<GeneticNodeGene> genes;

		genes.push_back(GeneticNodeGene("Bias","NetworkSensor",0,false));
		genes.push_back(GeneticNodeGene("X1","NetworkSensor",0,false));
		genes.push_back(GeneticNodeGene("Y1","NetworkSensor",0,false));
#if DEBUG_USE_ABSOLUTE_COORDS
		genes.push_back(GeneticNodeGene("X2","NetworkSensor",0,false));
		genes.push_back(GeneticNodeGene("Y2","NetworkSensor",0,false));
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

		cout << "Finished creating population\n";
		return population;
	}

	void OthelloExperiment::generateSubstrate(int substrateNum)
	{
#if OTHELLO_EXPERIMENT_ENABLE_BIASES
		NEAT::FastBiasNetwork<OthelloNEATDatatype>* substrate;
#else
		NEAT::FastNetwork<OthelloNEATDatatype>* substrate;
#endif

		substrate = &substrates[substrateNum];

		cout << "Generating substrate...";
		boost::object_pool<NEAT::NetworkNode> networkNodePool;
		boost::object_pool<NEAT::NetworkLink> networkLinkPool;

		NEAT::NetworkNode *nodes = NULL;
		NEAT::NetworkLink *links = NULL;
#if OTHELLO_EXPERIMENT_ENABLE_BIASES
		OthelloNEATDatatype *nodeBiases = NULL;
#endif

		int nodeCount =
			(numNodesX[0]*numNodesY[0]) +
			(numNodesX[1]*numNodesY[1]) +
			(numNodesX[2]*numNodesY[2]);

		//In the future the first part can be divided by 2 because half are invalid (from free squares) but it's really hard to pull off
		int linkCount =
			(numNodesX[0]*numNodesY[0]*numNodesX[1]*numNodesY[1]) +
			(numNodesX[1]*numNodesY[1]*numNodesX[2]*numNodesY[2]) +
			(numNodesX[0]*numNodesY[0]*numNodesX[2]*numNodesY[2]);

		try
		{
			nodes = (NEAT::NetworkNode*)malloc(sizeof(NEAT::NetworkNode)*nodeCount);
			links = (NEAT::NetworkLink*)malloc(sizeof(NEAT::NetworkLink)*linkCount);
#if OTHELLO_EXPERIMENT_ENABLE_BIASES
			nodeBiases = new OthelloNEATDatatype[nodeCount];
#endif
		}
		catch(const std::exception &e)
		{
			cout << e.what() << endl;
			CREATE_PAUSE(string("Error!: ")+toString(__LINE__));
		}

		nameLookup.clear();

		cout << "Creating nodes...\n";

		int nodeCounter=0;

		for (int z=0;z<3;z++)
		{
			for (int y1=0;y1<numNodesY[z];y1++)
			{
				for (int x1=0;x1<numNodesX[z];x1++)
				{
					int xmod = x1;
					int ymod = y1;
					Node node(xmod,ymod,z);
					//cout << (y1-numNodesY/2) << '/' << (x1-numNodesX/2) << endl;
					string name = (toString(xmod)+string("/")+toString(ymod) + string("/") + toString(z));
					nameLookup[node] = name;
					new (&nodes[nodeCounter]) NetworkNode(name);
					nodeLookup[node] = &nodes[nodeCounter];
#if OTHELLO_EXPERIMENT_ENABLE_BIASES
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

		for (int z1=0;z1<3;z1++)
		{
			for (int z2=z1+1;z2<3;z2++)
			{
				for (int y1=0;y1<numNodesY[z1];y1++)
				{
					for (int x1=0;x1<numNodesX[z1];x1++)
					{
						for (int y2=0;y2<numNodesY[z2];y2++)
						{
							for (int x2=0;x2<numNodesX[z2];x2++)
							{
#if DEBUG_DIRECT_LINKS
#else
								if (z1==0 && z2==2)
								{
									continue;
								}
#endif
								new (&links[linkCounter]) NetworkLink(
									nodeLookup[Node(x1,y1,z1)],
									nodeLookup[Node(x2,y2,z2)],
									0.0
									);

								if (linkCounter>=linkCount)
								{
									throw CREATE_LOCATEDEXCEPTION_INFO("Too many links for what was allocated!");
								}
								linkCounter++;
							}
						}
					}
				}
			}
		}

#if OTHELLO_EXPERIMENT_ENABLE_BIASES
		cout << "Creating FastBiasNetwork\n";

		(*substrate) = NEAT::FastBiasNetwork<OthelloNEATDatatype>(
			nodes,
			nodeCounter,
			links,
			linkCounter,
			nodeBiases
			);
#else
		cout << "Creating FastNetwork\n";
		(*substrate) = NEAT::FastNetwork<OthelloNEATDatatype>(
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
#if OTHELLO_EXPERIMENT_ENABLE_BIASES
		delete[] nodeBiases;
#endif
	}

	inline OthelloNEATDatatype convertOutputToWeight(OthelloNEATDatatype output)
	{
		if (fabs(output)>0.2)
		{
			if (output>0.0)
				output = (OthelloNEATDatatype)( ((output-0.2)/0.8)*3.0 );
			else
				output = (OthelloNEATDatatype)( ((output+0.2)/0.8)*3.0 );
		}
		else
		{
			output = (0.0);
		}
		return output;
	}

	void OthelloExperiment::populateSubstrate(
		shared_ptr<const NEAT::GeneticIndividual> individual,
		int substrateNum
		)
	{
#if OTHELLO_EXPERIMENT_ENABLE_BIASES
		NEAT::FastBiasNetwork<OthelloNEATDatatype>* substrate;
#else
		NEAT::FastNetwork<OthelloNEATDatatype>* substrate;
#endif
		if (substrateIndividuals[substrateNum]==individual)
		{
			//Don't bother remaking the same substrate
			return;
		}

		substrateIndividuals[substrateNum]=individual;

		substrate = &substrates[substrateNum];

		//Clear the evaluation cache
		boardEvaluationCache.clear();


		NEAT::FastNetwork<OthelloNEATDatatype> network = individual->spawnFastPhenotypeStack<OthelloNEATDatatype>();

		int linkCounter=0;

#if OTHELLO_EXPERIMENT_DEBUG
		double linkChecksum=0.0;
#endif

		for (int z1=0;z1<3;z1++)
		{
			for (int z2=z1+1;z2<3;z2++)
			{
				for (int y1=0;y1<numNodesY[z1];y1++)
				{
					for (int x1=0;x1<numNodesX[z1];x1++)
					{
						for (int y2=0;y2<numNodesY[z2];y2++)
						{
							for (int x2=0;x2<numNodesX[z2];x2++)
							{
								/*Remap the nodes to the [-1,1] domain*/
								OthelloNEATDatatype x1normal,y1normal,x2normal,y2normal;

								if (numNodesX[z1]>1)
								{
									x1normal = (OthelloNEATDatatype)(-1.0 + (OthelloNEATDatatype(x1)/(numNodesX[z1]-1))*2.0);
								}
								else
								{
									x1normal = 0.0;
								}

								if (numNodesY[z1]>1)
								{
									y1normal = (OthelloNEATDatatype)(-1.0 + (OthelloNEATDatatype(y1)/(numNodesY[z1]-1))*2.0);
								}
								else
								{
									y1normal = 0.0;
								}

								if (numNodesX[z2]>1)
								{
									x2normal = (OthelloNEATDatatype)(-1.0 + (OthelloNEATDatatype(x2)/(numNodesX[z2]-1))*2.0);
								}
								else
								{
									x2normal = 0.0;
								}

								if (numNodesY[z2]>1)
								{
									y2normal = (OthelloNEATDatatype)(-1.0 + (OthelloNEATDatatype(y2)/(numNodesY[z2]-1))*2.0);
								}
								else
								{
									y2normal = 0.0;
								}

								network.reinitialize();
								network.setValue("X1",x1normal);
								network.setValue("Y1",y1normal);
								if (network.hasNode("X2"))
								{
									network.setValue("X2",x2normal);
									network.setValue("Y2",y2normal);
								}
								if (network.hasNode("DeltaX"))
								{
									network.setValue("DeltaX",x2normal-x1normal);
									network.setValue("DeltaY",y2normal-y1normal);
								}
								else
								{
									throw CREATE_LOCATEDEXCEPTION_INFO("THIS NETWORK HAS NO DELTAS BY ACCIDENT!\n");
								}
								network.setValue("Bias",(OthelloNEATDatatype)0.3);
								network.update();

								OthelloNEATDatatype output;

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
								<< nameLookup[Node(x1-numNodesX[z1]/2,y1-numNodesY[z1]/2,z1)] << " -> "
								<< nameLookup[Node(x2-numNodesX[z2]/2,y2-numNodesY[z2]/2,z2)]
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

									output /= numNodesX[z1];
									//output /= numNodesY[z1];


									//output *= numNodesX[z2];
									//output *= numNodesY[z2];
								}

								if (
									(nameLookup.find(Node(x1,y1,z1))==nameLookup.end()) ||
									(nameLookup.find(Node(x2,y2,z2))==nameLookup.end())
									)
								{
									throw CREATE_LOCATEDEXCEPTION_INFO("COULD NOT FIND A NODE'S NAME!");
								}

#if DEBUG_DIRECT_LINKS
#else
								if (! (z1==0 && z2==2) )
								{
#endif
									substrate->getLink(
										nameLookup[Node(x1,y1,z1)],
										nameLookup[Node(x2,y2,z2)]
										)->weight = output;
#if OTHELLO_EXPERIMENT_DEBUG
									linkChecksum += output;
#endif

#if DEBUG_DIRECT_LINKS
#else
								}
#endif

								linkCounter++;

#if OTHELLO_EXPERIMENT_ENABLE_BIASES
								if (x1==0&&y1==0&&z1==0)
								{
									OthelloNEATDatatype nodeBias;

									if (z2==1)
									{
										nodeBias = network.getValue("Bias_b");

										nodeBias = convertOutputToWeight(nodeBias);

										/*{
										cout << "Setting bias for "
										<< nameLookup[Node(x2-numNodesX[z2]/2,y2-numNodesY[z2]/2,1)]
										<< endl;
										cout << "Bias: " << nodeBias << endl;
										CREATE_PAUSE("");
										}*/

										substrate->setBias(
											nameLookup[Node(x2,y2,z2)],
											nodeBias
											);
									}
									else if (z2==2)
									{
										nodeBias = network.getValue("Bias_c");

										nodeBias = convertOutputToWeight(nodeBias);

										/*{
										cout << "Setting bias for "
										<< nameLookup[Node(x2-numNodesX[z2]/2,y2-numNodesY[z2]/2,2)]
										<< endl;
										cout << "Bias: " << nodeBias << endl;
										CREATE_PAUSE("");
										}*/

										substrate->setBias(
											nameLookup[Node(x2,y2,z2)],
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

#if OTHELLO_EXPERIMENT_DUMP_PLAYER
		{
			//Dump the player and then bail
			ofstream outfile("PLAYERDUMP.dat");

			outfile << 64 << ' ' << 64 << ' ' << 1 << endl;

			for (int z1=0;z1<2;z1++)
			{
				int z2 = z1+1;

				for (int y2=0;y2<numNodesY[z2];y2++)
				{
					for (int x2=0;x2<numNodesX[z2];x2++)
					{
						//First, dump the bias
						outfile << substrates[0].getBias(*nameLookup.getData(Node(x2,y2,z2))) << ' ';

						for (int y1=0;y1<numNodesY[z1];y1++)
						{
							for (int x1=0;x1<numNodesX[z1];x1++)
							{
								OthelloNEATDatatype weight =
									substrates[0].getLink(
									*nameLookup.getData(Node(x1,y1,z1)),
									*nameLookup.getData(Node(x2,y2,z2))
									)->weight;

								outfile << weight << ' ';
							}
						}

						outfile << endl;
					}
				}
			}

		}

		exit(0);
#endif

#if OTHELLO_EXPERIMENT_RECOVER_PLAYER
		{
			//Dump the player and then bail
			ifstream infile("PLAYERDUMP.dat");

			int z,zz,zzz;
			infile >> z >> zz >> zzz;

			for (int z1=0;z1<2;z1++)
			{
				int z2 = z1+1;

				for (int y2=0;y2<numNodesY[z2];y2++)
				{
					for (int x2=0;x2<numNodesX[z2];x2++)
					{
						//First, dump the bias
						OthelloNEATDatatype bias;
						infile >> bias;
						substrates[0].setBias(*nameLookup.getData(Node(x2,y2,z2)),bias);

						for (int y1=0;y1<numNodesY[z1];y1++)
						{
							for (int x1=0;x1<numNodesX[z1];x1++)
							{
								OthelloNEATDatatype weight;
								infile >> weight;
								substrates[0].getLink(
									*nameLookup.getData(Node(x1,y1,z1)),
									*nameLookup.getData(Node(x2,y2,z2))
									)->weight = weight;
							}
						}
					}
				}
			}
		}
#endif
	}

	void OthelloExperiment::resetBoard(ushort b[8][8])
	{
		memset(b,0,sizeof(ushort)*8*8);

		b[3][3] = b[4][4] = OTHELLO_WHITE;
		b[4][3] = b[3][4] = OTHELLO_BLACK;

		OTHELLO_SET_NUM_BLACK_PIECES(b,2);
		OTHELLO_SET_NUM_WHITE_PIECES(b,2);
	}

	const double weights[OTHELLO_OPPONENT_TYPES][8][8] =
	{
		{
			{   1.0,    -0.25,  0.1,    0.05,   0.05,   0.1,    -0.25,  1.0     },
			{   -0.25,  -0.25,  0.01,   0.01,   0.01,   0.01,   -0.25,  -0.25   },
			{   0.1,    0.01,   0.05,   0.02,   0.02,   0.05,   0.01,   0.1     },
			{   0.05,   0.01,   0.02,   0.01,   0.01,   0.02,   0.01,   0.05    },
			{   0.05,   0.01,   0.02,   0.01,   0.01,   0.02,   0.01,   0.05    },
			{   0.1,    0.01,   0.05,   0.02,   0.02,   0.05,   0.01,   0.1     },
			{   -0.25,  -0.25,  0.01,   0.01,   0.01,   0.01,   -0.25,  -0.25   },
			{   1.0,    -0.25,  0.1,    0.05,   0.05,   0.1,    -0.25,  1.0     }
		}/*,
		{
			{   4.622507, -1.477853, 1.409644, -0.066975, -0.305214, 1.633019, -1.050899, 4.365550      },
			{   -1.329145, -2.245663, -1.060633, -0.541089, -0.332716, -0.475830, -2.274535, -0.032595  },
			{   2.681550, -0.906628, 0.229372, 0.059260, -0.150415, 0.321982, -1.145060, 2.986767       },
			{   -0.746066, -0.317389, 0.140040, -0.045266, 0.236595, 0.158543, -0.720833, -0.131124     },
			{   -0.305566, -0.328398, 0.073872, -0.131472, -0.172101, 0.016603, -0.511448, -0.264125    },
			{   2.777411, -0.769551, 0.676483, 0.282190, 0.007184, 0.269876, -1.408169, 2.396238        },
			{   -1.566175, -3.049899, -0.637408, -0.077690, -0.648382, -0.911066, -3.329772, -0.870962  },
			{   5.046583, -1.468806, 1.545046, -0.031175, 0.263998, 2.063148, -0.148002, 5.781035       }
		},
		{
			{   5.046583, -1.468806, 1.545046, -0.031175, 0.263998, 2.063148, -0.148002, 5.781035       },
			{   -1.566175, -3.049899, -0.637408, -0.077690, -0.648382, -0.911066, -3.329772, -0.870962  },
			{   2.777411, -0.769551, 0.676483, 0.282190, 0.007184, 0.269876, -1.408169, 2.396238        },
			{   -0.305566, -0.328398, 0.073872, -0.131472, -0.172101, 0.016603, -0.511448, -0.264125    },
			{   -0.746066, -0.317389, 0.140040, -0.045266, 0.236595, 0.158543, -0.720833, -0.131124     },
			{   2.681550, -0.906628, 0.229372, 0.059260, -0.150415, 0.321982, -1.145060, 2.986767       },
			{   -1.329145, -2.245663, -1.060633, -0.541089, -0.332716, -0.475830, -2.274535, -0.032595  },
			{   4.622507, -1.477853, 1.409644, -0.066975, -0.305214, 1.633019, -1.050899, 4.365550      }
		},
		{
			{ 4.36555, -1.0509, 1.63302, -0.305214, -0.066975, 1.40964, -1.47785, 4.62251    },
			{ -0.032595, -2.27454, -0.47583, -0.332716, -0.541089, -1.06063, -2.24566, -1.32915  },
			{ 2.98677, -1.14506, 0.321982, -0.150415, 0.05926, 0.229372, -0.906628, 2.68155   },
			{ -0.131124, -0.720833, 0.158543, 0.236595, -0.045266, 0.14004, -0.317389, -0.746066  },
			{ -0.264125, -0.511448, 0.016603, -0.172101, -0.131472, 0.073872, -0.328398, -0.305566 },
			{ 2.39624, -1.40817, 0.269876, 0.007184, 0.28219, 0.676483, -0.769551, 2.77741   },
			{ -0.870962, -3.32977, -0.911066, -0.648382, -0.07769, -0.637408, -3.0499, -1.56618  },
			{ 5.78104, -0.148002, 2.06315, 0.263998, -0.031175, 1.54505, -1.46881, 5.04658   }
		},
		{
			{ 5.78104, -0.148002, 2.06315, 0.263998, -0.031175, 1.54505, -1.46881, 5.04658   },
			{ -0.870962, -3.32977, -0.911066, -0.648382, -0.07769, -0.637408, -3.0499, -1.56618  },
			{ 2.39624, -1.40817, 0.269876, 0.007184, 0.28219, 0.676483, -0.769551, 2.77741   },
			{ -0.264125, -0.511448, 0.016603, -0.172101, -0.131472, 0.073872, -0.328398, -0.305566 },
			{ -0.131124, -0.720833, 0.158543, 0.236595, -0.045266, 0.14004, -0.317389, -0.746066  },
			{ 2.98677, -1.14506, 0.321982, -0.150415, 0.05926, 0.229372, -0.906628, 2.68155   },
			{ -0.032595, -2.27454, -0.47583, -0.332716, -0.541089, -1.06063, -2.24566, -1.32915  },
			{ 4.36555, -1.0509, 1.63302, -0.305214, -0.066975, 1.40964, -1.47785, 4.62251    }
		}*/
	};

	//For evaluation.  Positive means winning for black, and negative means
	//winning for white.
	OthelloNEATDatatype OthelloExperiment::evaluateLeafHandCoded(ushort b[8][8])
	{
		if (DEBUG_USE_HYPERNEAT_EVALUATION)
		{
			throw CREATE_LOCATEDEXCEPTION_INFO("WRONG TIME FOR HAND CODED EVALUATION");
		}

		OthelloNEATDatatype retval=0;

		//ushort posval;
		for (int x=0;x<8;x++)
		{
			for (int y=0;y<8;y++)
			{
				//Multiply the weight times the board position
				//posval = (*( ((ushort*)b) + a));
				int pieceType = OTHELLO_GET_PIECE( b[x][y] );
				int typeToVal;
				switch (pieceType)
				{
				case OTHELLO_BLACK:
					{
						typeToVal = 1;
					}
					break;
				case OTHELLO_WHITE:
					{
						typeToVal = -1;
					}
					break;
				case OTHELLO_EMPTY:
					{
						typeToVal = 0;
					}
					break;
				default:
					{
						throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: BOARD CORRUPTION!");
					}
					break;
				}
				retval += (OthelloNEATDatatype)(weights[handCodedType][y][x] * typeToVal);
			}
		}

		return retval;
	}

	OthelloNEATDatatype OthelloExperiment::evaluateLeafHyperNEAT(
		ushort b[8][8]
	)
	{
#if OTHELLO_EXPERIMENT_ENABLE_BIASES
		NEAT::FastBiasNetwork<OthelloNEATDatatype>* substrate;
#else
		NEAT::FastNetwork<OthelloNEATDatatype>* substrate;
#endif

		substrate = &substrates[currentSubstrateIndex];

		if (DEBUG_USE_HANDCODED_EVALUATION)
		{
			throw CREATE_LOCATEDEXCEPTION_INFO("WRONG TIME FOR HYPERNEAT EVALUATION");
		}

		OthelloNEATDatatype output;

#if DEBUG_USE_BOARD_EVALUATION_CACHE
		tmpboard.loadBoard(b);
		OthelloBoardCacheMap::iterator bIterator = boardEvaluationCache.find(tmpboard);

		if (bIterator != boardEvaluationCache.end())
		{
			output = bIterator->second;
		}
		else
#endif
		{

			substrate->reinitialize();
			substrate->dummyActivation();

			for (int y=0;y<numNodesY[0];y++)
			{
				for (int x=0;x<numNodesX[0];x++)
				{
					int boardx = x;
					int boardy = y;

					//cout << boardx << ',' << boardy << ": " << b[boardx][boardy] << endl;

					if ( OTHELLO_GET_PIECE(b[boardx][boardy]) == OTHELLO_WHITE )
					{
						substrate->setValue( getNameFromNode(Node(x,y,0)) , -1.0 );
					}
					else if ( OTHELLO_GET_PIECE(b[boardx][boardy]) == OTHELLO_BLACK )
					{
						substrate->setValue( getNameFromNode(Node(x,y,0)) , 1.0 );
					}
					else
					{
						//Not necessary, reinitialize will set all to 0 in the beginning.
						//substrate->setValue( getNameFromNode(Node(x,y,0)) , 0.0 );
					}

				}
			}

			substrate->update();
			substrate->update();
			output = substrate->getValue(getNameFromNode(Node(0,0,2)));

#if OTHELLO_EXPERIMENT_PRINT_BOARD_RATINGS
			printBoard(b);
			cout << "BOARD RATING:" << output << endl;
			CREATE_PAUSE("");
#endif

#if OTHELLO_EXPERIMENT_DEBUG
			static OthelloNEATDatatype prevOutput;

			if (fabs(output-prevOutput)>1e-3)
			{
				prevOutput = output;
				cout << "BOARD RATING:" << output << endl;
				CREATE_PAUSE("");
			}
#endif

#if DEBUG_USE_BOARD_EVALUATION_CACHE
			if (boardEvaluationCache.size()<10000)
			{
				boardEvaluationCache[tmpboard] = output;
			}
#endif
		}

#if 0 //#if DEBUG_DUMP_BOARD_LEAF_EVALUATIONS
		//cout << "Printing board leaf evaluations\n";
		for (int y=0;y<8;y++)
		{
			for (int x=7;x>=0;x--)
			{
				if (b[x][y]&FREE)
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
		*outfile << endl;
#endif
		return output;
	}

	OthelloNEATDatatype OthelloExperiment::evaluateLeafWhite(ushort b[8][8])
	{
		if (DEBUG_USE_HANDCODED_EVALUATION)
		{
			return evaluateLeafHandCoded(b);
		}
		else //DEBUG_USE_HYPERNEAT_EVALUATION
		{
			//You have to flip the board to keep the initiative consisent
#if OTHELLO_EXPERIMENT_USE_TEMPO
			for(int x=0;x<8;x++)
			{
				for(int y=0;y<8;y++)
				{
					if(OTHELLO_GET_PIECE(b[x][y]) != OTHELLO_EMPTY)
					{
						OTHELLO_REVERSE_PIECE(b[x][y]);
					}
				}
			}
#endif

			OthelloNEATDatatype retval = evaluateLeafHyperNEAT(b);

#if OTHELLO_EXPERIMENT_USE_TEMPO
			//Now, flip the evaluation and then unflip the pieces
			retval = -retval;
			for(int x=0;x<8;x++)
			{
				for(int y=0;y<8;y++)
				{
					if(OTHELLO_GET_PIECE(b[x][y]) != OTHELLO_EMPTY)
					{
						OTHELLO_REVERSE_PIECE(b[x][y]);
					}
				}
			}
#endif

			return retval;
		}
	}

	OthelloNEATDatatype OthelloExperiment::evaluateLeafBlack(ushort b[8][8])
	{
		if (DEBUG_USE_HANDCODED_EVALUATION)
		{
			return evaluateLeafHandCoded(b);
		}
		else //DEBUG_USE_HYPERNEAT_EVALUATION
		{
			return evaluateLeafHyperNEAT(b);
		}
	}

	class OthelloBoardMoveState
	{
	public:
		OthelloNEATDatatype parentBeta;
		int depth,maxDepth;
		OthelloBoardMoveState()
		{}
		OthelloBoardMoveState(
			OthelloNEATDatatype _parentBeta,
			int _depth,
			int _maxDepth
			)
			:
		parentBeta(_parentBeta),
			depth(_depth),
			maxDepth(_maxDepth)
		{}
	};

	OthelloNEATDatatype OthelloExperiment::evaluatemax(ushort b[8][8],  OthelloNEATDatatype parentBeta, int depth,int maxDepth)
	{
		if (depth==0)
		{
			totalNumMoves=0;
#if DEBUG_DUMP_BOARD_LEAF_EVALUATIONS
			cout << "Creating new outfile\n";
			if (outfile) delete outfile;
			outfile = new ofstream("BoardEvaluations.txt");
#endif
		}
#if DEBUG_BOARD_CACHE
		OthelloBoardState boardState(b,OTHELLO_BLACK,depth);
		if (depth==0)
		{
			boardStateCache.clear();
		}
		else
		{
			OthelloBoardStateMap::iterator bsIterator = boardStateCache.find(boardState);

			if (bsIterator!=boardStateCache.end())
			{
				//cout << "CACHE HIT, BAILING\n";
				//This board has already been explored before, treat this as a loss
				if (bsIterator->second>parentBeta)
				{
					//This will be worse than the last time this state was seen.  Don't bother.
					return INT_MIN/2;
				}
			}

			if (boardStateCache.size()<10000)
			{
				boardStateCache.insert(boardState,parentBeta);
			}
		}
#endif

		int newMoveStart = totalNumMoves;

		OthelloMove *moveList = &totalMoveList[newMoveStart];
		int moveListCount;

		OthelloNEATDatatype alpha=OthelloNEATDatatype(INT_MIN);

		int bestMoveSoFarIndex=-1;

		moveListCount = generateMoveList(b,moveList,OTHELLO_BLACK);

		if (!moveListCount)
		{
			//No possible moves, this is a loss!
			return INT_MIN/2;
		}

		if (depth==0 && moveListCount==1)
		{
			//Forced move, don't bother doing any evaluations
			moveToMake = moveList[0];
			return 0;
		}

		if (depth==0 && randomMoveChance)
		{
			int makeCrazyMove = NEAT::Globals::getSingleton()->getRandom().getRandomWithinRange(1,20);

			if (makeCrazyMove<=randomMoveChance)
			{
				//random chance of making a crazy move
				int randomMove =
					NEAT::Globals::getSingleton()->getRandom().getRandomWithinRange(0,moveListCount-1);

				moveToMake = moveList[randomMove];
				return 0;
			}
		}

#if OTHELLO_EXPERIMENT_DEBUG
		printBoard(b);
		cout << "Moves for black: " << endl;
		for (int a=0;a<moveListCount;a++)
		{
			cout << "MOVE: (" << (int)moveList[a].position.x << ',' << (int)moveList[a].position.y << ")" << endl;
		}
		CREATE_PAUSE("Done listing moves");
#endif

		if (depth==maxDepth)
		{
			//This is a leaf node, return the neural network's evaluation
			return evaluateLeafBlack(b);
		}

		OthelloNEATDatatype childBeta;

		totalNumMoves+=moveListCount;

		if (totalNumMoves >= MAX_TOTAL_MOVES)
		{
			CREATE_PAUSE("Shit! I blew my move stack!");
		}

		for (int a=0;a<moveListCount;a++)
		{
			makeMove(moveList[a],b);

			int winner = getWinner(b);

			if (winner==OTHELLO_BLACK)
			{
				//CREATE_PAUSE("FOUND WIN FOR BLACK!");
				reverseMove(moveList[a],b);

				if (depth==0)
					moveToMake = moveList[a];

				return OthelloNEATDatatype(INT_MAX/2);
			}

			childBeta = evaluatemin(b,alpha,depth+1,maxDepth);
			reverseMove(moveList[a],b);

#if OTHELLO_EXPERIMENT_DEBUG
			for (int dd=0;dd<depth;dd++)
			{
				cout << "*";
			}
			cout << childBeta << endl;
#endif

			if (childBeta > alpha)
			{
#if OTHELLO_EXPERIMENT_DEBUG
				cout << "Found new alpha\n";
#endif
				alpha = childBeta;
				bestMoveSoFarIndex = a;
				if (depth==0)
				{
					//This means that this is the root max, so store the best move.
#if OTHELLO_EXPERIMENT_DEBUG
					cout << "BLACK: MOVE_TO_MAKE: (" << moveList[a].position.x << ',' << moveList[a].position.y << ")" << endl;
					CREATE_PAUSE("SETTING MOVE_TO_MAKE");
#endif

					moveToMake = moveList[a];
				}
				else
				{
					if (parentBeta <= childBeta)
					{
#if OTHELLO_EXPERIMENT_DEBUG
						cout << "Parent will never choose this path\n";
						CREATE_PAUSE("");
#endif
						//parent will never choose this alpha
						totalNumMoves-=moveListCount;

#if DEBUG_DUMP_BOARD_LEAF_EVALUATIONS
						//cout << "Printing board leaf evaluations\n";
						if (depth<=2)
						{
							*outfile << "Depth: " << depth << endl;
							for (int y=0;y<8;y++)
							{
								for (int x=7;x>=0;x--)
								{
									if (b[x][y]&FREE)
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
							*outfile << "BEST MOVE FOR BLACK: " << (int)moveList[a].from.x << ',' << (int)moveList[a].from.y << " to "
								<< (int)moveList[a].getFinalDestination().x << ',' << (int)moveList[a].getFinalDestination().y << endl;
							*outfile << endl;
						}
#endif
#if DEBUG_DUMP_BOARD_LEAF_EVALUATIONS
						if (depth==0)
						{
							cout << __LINE__ << " Closing outfile handle\n";
							if (outfile)
							{
								outfile->close();
								delete outfile;
							}
							outfile = NULL;
						}
#endif
						return alpha;
					}
				}
			}
		}

		totalNumMoves-=moveListCount;
#if DEBUG_DUMP_BOARD_LEAF_EVALUATIONS
		//cout << "Printing board leaf evaluations\n";
		if (depth<=2)
		{
			*outfile << "Depth: " << depth << endl;
			for (int y=0;y<8;y++)
			{
				for (int x=7;x>=0;x--)
				{
					if (b[x][y]&FREE)
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
			if (bestMoveSoFarIndex!=-1)
			{
				*outfile << "BEST MOVE FOR BLACK: " << (int)moveList[bestMoveSoFarIndex].from.x << ',' << (int)moveList[bestMoveSoFarIndex].from.y << " to "
					<< (int)moveList[bestMoveSoFarIndex].getFinalDestination().x << ',' << (int)moveList[bestMoveSoFarIndex].getFinalDestination().y << endl;
				*outfile << endl;
			}
			else
			{
				*outfile << "NO MOVES FOUND (OR WILL NEVER GET TO THIS PART OF THE GAME TREE)\n";
			}
		}
#endif
#if DEBUG_DUMP_BOARD_LEAF_EVALUATIONS
		if (depth==0)
		{
			cout << __LINE__ << " Closing outfile handle\n";
			if (outfile)
			{
				outfile->close();
				delete outfile;
			}
			outfile = NULL;
		}
#endif
		return alpha;
	}

	OthelloNEATDatatype OthelloExperiment::evaluatemin(ushort b[8][8],  OthelloNEATDatatype parentAlpha, int depth,int maxDepth)
	{
		if (depth==0)
		{
			totalNumMoves=0;
		}
#if DEBUG_BOARD_CACHE
		OthelloBoardState boardState(b,OTHELLO_WHITE,depth);
		if (depth==0)
		{
			boardStateCache.clear();
		}
		else
		{
			OthelloBoardStateMap::iterator bsIterator = boardStateCache.find(boardState);

			if (bsIterator!=boardStateCache.end())
			{
				//cout << "CACHE HIT, BAILING\n";
				//This board has already been explored before, treat this as a loss
				if (bsIterator->second<parentAlpha)
				{
					//This will be a subset of the moves seen previously.  Ignore
					return (OthelloNEATDatatype)INT_MAX/2;
				}
			}

			boardStateCache.insert(boardState,parentAlpha);
		}
#endif


		int newMoveStart = totalNumMoves;

		OthelloMove *moveList = &totalMoveList[newMoveStart];
		int moveListCount;

		OthelloNEATDatatype beta=OthelloNEATDatatype(INT_MAX);

		int bestMoveSoFarIndex=-1;

		moveListCount = generateMoveList(b,moveList,OTHELLO_WHITE);

		if (!moveListCount)
		{
			/*NOTE:
			It is ok to put this before the getWinner(...)
			because if you have taken all the opponent's pieces,
			you can always move somewhere
			*/

			//No possible moves, this is a win (for black)!
			return (OthelloNEATDatatype)INT_MAX/2;
		}

		if (depth==0 && moveListCount==1)
		{
			//Forced move, don't bother doing any evaluations
			moveToMake = moveList[0];
			return 0;
		}

		if (depth==0 && randomMoveChance)
		{
			int makeCrazyMove = NEAT::Globals::getSingleton()->getRandom().getRandomWithinRange(1,20);

			if (makeCrazyMove<=randomMoveChance)
			{
				//random chance of making a crazy move
				int randomMove =
					NEAT::Globals::getSingleton()->getRandom().getRandomWithinRange(0,moveListCount-1);

				moveToMake = moveList[randomMove];
				return 0;
			}
		}

#if OTHELLO_EXPERIMENT_DEBUG
		printBoard(b);
		cout << "Moves for white: " << endl;
		for (int a=0;a<moveListCount;a++)
		{
			cout << "MOVE: (" << (int)moveList[a].position.x << ',' << (int)moveList[a].position.y << ")\n";
		}
		CREATE_PAUSE("Done listing moves");
#endif

		if (depth==maxDepth)
		{
			//This is a leaf node, return the hand coded evaluation
			return evaluateLeafWhite(b);
		}

		OthelloNEATDatatype childAlpha;

		totalNumMoves+=moveListCount;

		if (totalNumMoves >= MAX_TOTAL_MOVES)
		{
			CREATE_PAUSE("Shit! I blew my move stack!");
		}

		for (int a=0;a<moveListCount;a++)
		{
			makeMove(moveList[a],b);

			int winner = getWinner(b);

			if (winner==OTHELLO_WHITE)
			{
				//CREATE_PAUSE("FOUND WIN FOR WHITE!");
				reverseMove(moveList[a],b);

				if (depth==0)
					moveToMake = moveList[a];

				return (OthelloNEATDatatype)INT_MIN/2;
			}

			childAlpha = evaluatemax(b,beta,depth+1,maxDepth);
			reverseMove(moveList[a],b);

			if (childAlpha < beta)
			{
#if OTHELLO_EXPERIMENT_DEBUG
				cout << "Found new beta\n";
#endif
				beta = childAlpha;
				bestMoveSoFarIndex = a;

				if (depth==0)
				{
					//This means that this is the root max, so store the best move.
#if OTHELLO_EXPERIMENT_DEBUG
					cout << "WHITE: MOVE_TO_MAKE: (" << moveList[a].position.x << ',' << moveList[a].position.y << ")\n";
					CREATE_PAUSE("SETTING MOVE_TO_MAKE");
#endif

					moveToMake = moveList[a];
				}
				else
				{
					if (parentAlpha >= beta)
					{
						//parent will never choose this beta
						totalNumMoves-=moveListCount;
#if DEBUG_DUMP_BOARD_LEAF_EVALUATIONS
						if (depth<=2)
						{
							*outfile << "Depth: " << depth << endl;
							//cout << "Printing board leaf evaluations\n";
							for (int y=0;y<8;y++)
							{
								for (int x=7;x>=0;x--)
								{
									if (b[x][y]&FREE)
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
							if (bestMoveSoFarIndex!=-1)
							{
								*outfile << "BEST MOVE FOR WHITE: " << (int)moveList[bestMoveSoFarIndex].from.x << ',' << (int)moveList[bestMoveSoFarIndex].from.y << " to "
									<< (int)moveList[bestMoveSoFarIndex].getFinalDestination().x << ',' << (int)moveList[bestMoveSoFarIndex].getFinalDestination().y << endl;
								*outfile << endl;
							}
							else
							{
								*outfile << "NO MOVES FOUND (OR WILL NEVER GET TO THIS PART OF THE GAME TREE)\n";
							}
						}
#endif
						return beta;
					}
				}
			}
		}

		if (depth==0 && moveListCount==0)
		{
			cout << "NO MOVES TO MAKE!!!\n";
		}

		totalNumMoves-=moveListCount;
#if DEBUG_DUMP_BOARD_LEAF_EVALUATIONS
		//cout << "Printing board leaf evaluations\n";
		if (depth<=2)
		{
			*outfile << "Depth: " << depth << endl;
			for (int y=0;y<8;y++)
			{
				for (int x=7;x>=0;x--)
				{
					if (b[x][y]&FREE)
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
			if (bestMoveSoFarIndex!=-1)
			{
				*outfile << "BEST MOVE FOR WHITE: " << (int)moveList[bestMoveSoFarIndex].from.x << ',' << (int)moveList[bestMoveSoFarIndex].from.y << " to "
					<< (int)moveList[bestMoveSoFarIndex].getFinalDestination().x << ',' << (int)moveList[bestMoveSoFarIndex].getFinalDestination().y << endl;
				*outfile << endl;
			}
			else
			{
				*outfile << "NO MOVES FOUND (OR WILL NEVER GET TO THIS PART OF THE GAME TREE)\n";
			}
		}
#endif
		return beta;
	}

	void OthelloExperiment::processGroup(shared_ptr<NEAT::GeneticGeneration> generation)
	{
		//cout << "Processing group\n";
		shared_ptr<NEAT::GeneticIndividual> individual = group.front();
		//You get 10 points just for entering the game, wahooo!
		individual->setFitness(10);

		populateSubstrate(individual);

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

		for (int gameCount=0;gameCount<MAX_GAME_COUNT;gameCount++)
		{
			if (gameCount > (MAX_GAME_COUNT*2/3))
			{
				randomMoveChance=2;
			}
			else if (gameCount > (MAX_GAME_COUNT/3))
			{
				randomMoveChance=1;
			}
			else
			{
				randomMoveChance=0;
			}

			for (handCodedType=0;handCodedType<OTHELLO_OPPONENT_TYPES;handCodedType++)
			{
				for (handCodedDepth=0;handCodedDepth<OTHELLO_DEPTH_ITERATIONS;handCodedDepth++)
				{

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
						DEBUG_USE_HANDCODED_EVALUATION = 0;
						DEBUG_USE_HYPERNEAT_EVALUATION = 1;
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
								DEBUG_USE_HANDCODED_EVALUATION = 1;
								DEBUG_USE_HYPERNEAT_EVALUATION = 0;
								evaluatemin(b,OthelloNEATDatatype(INT_MAX/2),0,1+(handCodedDepth));
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

						individual->reward(1200);
					}
					else if (retval==OTHELLO_END_TIE) //draw
					{
#if OTHELLO_EXPERIMENT_LOG_EVALUATIONS
						cout << "WE TIED!\n";
#endif
						individual->reward(600);
					}
					else //loss
					{
#if OTHELLO_EXPERIMENT_LOG_EVALUATIONS
						cout << "WE LOST!\n";
#endif
					}

					individual->reward( (OTHELLO_GET_NUM_BLACK_PIECES(b)) );

		{
			//Dump the player and then bail
			ofstream outfile("PLAYERDUMP.dat");

			outfile << 64 << ' ' << 64 << ' ' << 1 << endl;

			for (int z1=0;z1<2;z1++)
			{
				int z2 = z1+1;

				for (int y2=0;y2<numNodesY[z2];y2++)
				{
					for (int x2=0;x2<numNodesX[z2];x2++)
					{
						//First, dump the bias
						outfile << substrates[0].getBias(nameLookup[Node(x2,y2,z2)]) << ' ';

						for (int y1=0;y1<numNodesY[z1];y1++)
						{
							for (int x1=0;x1<numNodesX[z1];x1++)
							{
								OthelloNEATDatatype weight =
									substrates[0].getLink(
									nameLookup[Node(x1,y1,z1)],
									nameLookup[Node(x2,y2,z2)]
									)->weight;

								outfile << weight << ' ';
							}
						}

						outfile << endl;
					}
				}
			}

		}

		exit(0);


#if OTHELLO_EXPERIMENT_LOG_EVALUATIONS
					CREATE_PAUSE("");
#endif
				}
			}
		}

		//Now, let's do some trials where HyperNEAT evaluates for white
#if OTHELLO_EXPERIMENT_PRINT_GAMES
		cout << "Playing games with HyperNEAT as white\n";
#endif
		for (int gameCount=0;gameCount<MAX_GAME_COUNT;gameCount++)
		{
			if (gameCount > (MAX_GAME_COUNT*2/3))
			{
				randomMoveChance=2;
			}
			else if (gameCount > (MAX_GAME_COUNT/3))
			{
				randomMoveChance=1;
			}
			else
			{
				randomMoveChance=0;
			}

			for (handCodedType=0;handCodedType<OTHELLO_OPPONENT_TYPES;handCodedType++)
			{
				for (handCodedDepth=0;handCodedDepth<OTHELLO_DEPTH_ITERATIONS;handCodedDepth++)
				{

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
						DEBUG_USE_HANDCODED_EVALUATION = 1;
						DEBUG_USE_HYPERNEAT_EVALUATION = 0;
						evaluatemax(b,OthelloNEATDatatype(INT_MAX/2),0,1+(handCodedDepth));

#if OTHELLO_EXPERIMENT_PRINT_GAMES
						cout << "Playing games with HyperNEAT as white\n";
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
								DEBUG_USE_HANDCODED_EVALUATION = 0;
								DEBUG_USE_HYPERNEAT_EVALUATION = 1;
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
							cout << "Playing games with HyperNEAT as white\n";
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

					if (retval==OTHELLO_WHITE)
					{
#if OTHELLO_EXPERIMENT_LOG_EVALUATIONS
						cout << "WE WON!\n";
#endif

						individual->reward(1200);
					}
					else if (retval==OTHELLO_END_TIE) //draw
					{
#if OTHELLO_EXPERIMENT_LOG_EVALUATIONS
						cout << "WE TIED!\n";
#endif

						individual->reward(600);
					}
					else //loss
					{
#if OTHELLO_EXPERIMENT_LOG_EVALUATIONS
						cout << "WE LOST!\n";
#endif
					}

					individual->reward((OTHELLO_GET_NUM_WHITE_PIECES(b)) );

#if OTHELLO_EXPERIMENT_LOG_EVALUATIONS
					CREATE_PAUSE("");
#endif
				}
			}
		}

		//cout << "Trial over!\n";
	}

	void OthelloExperiment::processIndividualPostHoc(shared_ptr<NEAT::GeneticIndividual> individual)
	{
		cout << "INDIVIDUAL FITNESS BEFORE: " << individual->getFitness() << endl;
		clearGroup();
		addIndividualToGroup(individual);
		shared_ptr<GeneticGeneration> dummy;

		//force this version of processGroup so that I can test co-evolved
		//individuals against the weighted piece counter.
		OthelloExperiment::processGroup(dummy);

		cout << "INDIVIDUAL FITNESS: " << individual->getFitness() << endl;
	}

#ifndef HCUBE_NOGUI
	void OthelloExperiment::createIndividualImage(wxDC &drawContext,shared_ptr<NEAT::GeneticIndividual> individual)
	{
		if (lastIndividualSeen!=individual)
		{
			//cout << "Repopulating substrate\n";
			populateSubstrate(individual);
			lastIndividualSeen = individual;
		}

		drawContext.Clear();

		for (int x=0;x<8;x++)
		{
			for (int y=0;y<8;y++)
			{
				//array goes in opposite direction of drawing
				int drawX = x;
				int drawY = 8-y;

				//if (userEvaluationRound%2==1)
				{
					//Flip the board for white
					drawX = 8-x;
					drawY = y;
				}

				int piece = OTHELLO_GET_PIECE(userEvaluationBoard[x][y]);

				if (piece==OTHELLO_BLACK)
				{
					drawContext.DrawText(_T("B"),20+drawX*20,20+drawY*20);
				}
				else if (piece==OTHELLO_WHITE)
				{
					drawContext.DrawText(_T("W"),20+drawX*20,20+drawY*20);
				}
				else
				{
					drawContext.DrawText(_T("_"),20+drawX*20,20+drawY*20);
				}
			}
		}

		int playerToMove;
		if (userEvaluationRound%2==0)
		{
			playerToMove = OTHELLO_BLACK;
		}
		else
		{
			playerToMove = OTHELLO_WHITE;
		}

		int winner = getWinner(userEvaluationBoard);

		if (winner==OTHELLO_WHITE)
		{
			drawContext.DrawText(_T("WHITE WINS"),30,300);
		}
		else if (winner==OTHELLO_BLACK)
		{
			drawContext.DrawText(_T("BLACK WINS"),30,300);
		}
		else
		{
			if (userEvaluationRound%2==0)
			{
				drawContext.DrawText(_T("Black's turn.  Click to see black's move"),30,300);
			}
			else
			{
				drawContext.DrawText(_T("Move to?"),30,300);
			}
		}
	}

	bool OthelloExperiment::handleMousePress(wxMouseEvent& event,wxSize &bitmapSize)
	{
		int playerToMove;
		if (userEvaluationRound%2==0)
		{
			playerToMove = OTHELLO_BLACK;
		}
		else
		{
			playerToMove = OTHELLO_WHITE;
		}

		int winner = getWinner(userEvaluationBoard);

		if (winner==OTHELLO_WHITE||winner==OTHELLO_BLACK)
		{
			resetBoard(userEvaluationBoard);
			return true;
		}

		if (userEvaluationRound%2==0)
		{
			//Computer makes his move
			timer t;
#if DEBUG_DUMP_BOARD_LEAF_EVALUATIONS
			for (int depth=1;depth==1;depth+=2) //Do not go deeper if logging.
#else
			for (int depth=1;depth<=1;depth+=2)
#endif
			{
				moveToMake = OthelloMove();
				cout << __LINE__ << " CALLING EVALUATEMAX\n";
				DEBUG_USE_HANDCODED_EVALUATION = 0;
				DEBUG_USE_HYPERNEAT_EVALUATION = 1;
				evaluatemax(userEvaluationBoard,OthelloNEATDatatype(INT_MAX/2),0,depth);
				if (t.elapsed()>0.5) //When you increase depth by 2, it's about 20 times as long. (maybe 100 times)
					break;
			}

			if (moveToMake.position.x==255)
			{
				//black passes
				cout << "BLACK PASSES!\n";
			}
			else
			{
				makeMove(moveToMake,userEvaluationBoard);
			}
			userEvaluationRound++;

		}
		else
		{
			Vector2<uchar> boardPosition(
				(event.GetX()-20)/20,
				(event.GetY()-20)/20
				);

			//if (userEvaluationRound%2==1)
			{
				//Flip the board for white
				boardPosition.x = 8-boardPosition.x;
			}
			//else
			{
				//boardPosition.y = 8-boardPosition.y;
			}

			int numMoves = generateMoveList(
				userEvaluationBoard,
				totalMoveList,
				OTHELLO_WHITE
				);

			if (numMoves==0)
			{
				userEvaluationRound++;
			}
			else
			{

				for (int a=0;a<numMoves;a++)
				{
					if (
						totalMoveList[a].position==boardPosition
						)
					{
						//Move was a valid move, make it happen
						makeMove(totalMoveList[a],userEvaluationBoard);
						userEvaluationRound++;
					}
				}
			}
		}

		return true;
	}
#endif

	Experiment* OthelloExperiment::clone()
	{
		OthelloExperiment* experiment = new OthelloExperiment(*this);

		return experiment;
	}

	void OthelloExperiment::resetGenerationData(shared_ptr<NEAT::GeneticGeneration> generation)
	{
		//generation->setUserData(new TicTacToeStats());
	}

	void OthelloExperiment::addGenerationData(shared_ptr<NEAT::GeneticGeneration> generation,shared_ptr<NEAT::GeneticIndividual> individual)
	{
		//TicTacToeStats* ticTacToeStats = (TicTacToeStats*)generation->getUserData();

		//if (individual->getUserData())
		//(*ticTacToeStats) += *((TicTacToeStats*)individual->getUserData());
	}
}
