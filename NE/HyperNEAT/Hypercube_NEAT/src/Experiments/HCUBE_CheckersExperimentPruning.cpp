#include "HCUBE_Defines.h"

#include "Experiments/HCUBE_CheckersExperimentPruning.h"

#define CHECKERS_EXPERIMENT_DEBUG (0)

#define DEBUG_CHECK_HAND_CODED_HEURISTIC (0)

#define DEBUG_USE_BOARD_EVALUATION_CACHE (1)

#define DEBUG_DUMP_BOARD_LEAF_EVALUATIONS (0)

#define DEBUG_DIRECT_LINKS (0)

#define DEBUG_USE_ABSOLUTE (1)

#define DEBUG_USE_DELTAS (1)

#define DEBUG_DO_ITERATIVE_DEEPENING (0)

#define BASE_EVOLUTION_SEARCH_DEPTH (4)

#define NEAT_SEARCH_HANDICAP (0)

int HANDCODED_PLAYER_TESTS = 100;

#define PLAY_BOTH_SIDES (0)

#define CHECKERS_PRINT_ALTERNATE_MOVES (0)

#define CHECKERS_EXPERIMENT_INTERACTIVE_PLAY (0)

#define CHECKERS_EXPERIMENT_LOG_EVALUATIONS (0)

#define CHECKERS_EXPERIMENT_PRINT_EVALUATIONS_ON_LOSS (0)

#define DO_REGULAR_RUN_FOR_POSTHOC (1)

#define DEBUG_NO_LONG_RANGE_LINKS (0)

#define DEBUG_USE_DELTAS_ON_LONG_RANGE (1)

#define DEBUG_OVERRIDE_SECOND_LAYER_TO_ONE (0)

#define DEBUG_MAX_DELTA_RANGE (2)

#define DEBUG_PLAY_CAKE_INSTEAD_OF_CLICHE (1)

extern ofstream handCodedEvalStream;
extern ofstream handCodedTreeStream;
extern ofstream hyperNEATEvalStream;
extern ofstream hyperNEATTreeStream;

#define DEBUG_SHOW_HYPERNEAT_ALTERNATIVES (0)

extern CheckersAdvisor *checkersAdvisor;

namespace HCUBE
{
    class BoardEvaluation
    {
    public:
        uchar board[8][8];
        float value;

        BoardEvaluation(uchar _board[8][8],float _value)
            :
        value(_value)
        {
            memcpy(board,_board,sizeof(uchar)*8*8);
        }

        BoardEvaluation(const BoardEvaluation &other)
        {
            value = other.value;
            memcpy(board,other.board,sizeof(uchar)*8*8);
        }

        const BoardEvaluation &operator=(const BoardEvaluation &other)
        {
            if(this!=&other)
            {
                value = other.value;
                memcpy(board,other.board,sizeof(uchar)*8*8);
            }
            return *this;
        }
    };

#if DEBUG_SHOW_HYPERNEAT_ALTERNATIVES
    vector<BoardEvaluation> boardEvaluationList;

    ofstream *outfile = NULL;
#endif

    using namespace NEAT;

    extern mutex cakeMutex;

    CheckersExperimentPruning::CheckersExperimentPruning(string _experimentName,int _threadID)
        :
    Experiment(_experimentName,_threadID),
        from(255,255),
        DEBUG_USE_HANDCODED_EVALUATION(0),
        DEBUG_USE_HYPERNEAT_EVALUATION(0),
        chanceToMakeSecondBestMove(0.0),
        dumpEvaluationImages(false),
        cakeRandomSeed(1000),
		maxCakeNodes(10000)
    {
        searchInfo.repcheck = (REPETITION*)malloc((MAXDEPTH+HISTORYOFFSET)*sizeof(REPETITION));
        //boardEvaluationCaches[0].resize(10000);
        //boardEvaluationCaches[1].resize(10000);

        resetBoard(userEvaluationBoard);
        userEvaluationRound = (0);

        //boardStateCache.reserve(10000);

        layerInfo.layerSizes.push_back(Vector2<int>(8,8));
        layerInfo.layerIsInput.push_back(true);
        layerInfo.layerLocations.push_back(Vector3<float>(0,0,0));
        layerInfo.layerNames.push_back("Input");

        layerInfo.layerSizes.push_back(Vector2<int>(8,8));
        layerInfo.layerIsInput.push_back(false);
        layerInfo.layerLocations.push_back(Vector3<float>(0,4,0));
        layerInfo.layerNames.push_back("Processing");

        layerInfo.layerSizes.push_back(Vector2<int>(8,8));
        layerInfo.layerIsInput.push_back(false);
        layerInfo.layerLocations.push_back(Vector3<float>(0,8,0));
        layerInfo.layerNames.push_back("Output");

        layerInfo.layerAdjacencyList.push_back(std::pair<string,string>("Input","Processing"));
        layerInfo.layerAdjacencyList.push_back(std::pair<string,string>("Processing","Output"));

        layerInfo.normalize = true;
        layerInfo.useOldOutputNames = false;
        layerInfo.layerValidSizes = layerInfo.layerSizes;

        substrate.setLayerInfo(layerInfo);
    }

    CheckersExperimentPruning::~CheckersExperimentPruning()
    {
        if(searchInfo.repcheck)
            free(searchInfo.repcheck);
    }

    GeneticPopulation* CheckersExperimentPruning::createInitialPopulation(int populationSize)
    {
        GeneticPopulation *population = new GeneticPopulation();
        vector<GeneticNodeGene> genes;

        genes.push_back(GeneticNodeGene("Bias","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("X1","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("Y1","NetworkSensor",0,false));
#if DEBUG_USE_ABSOLUTE
        genes.push_back(GeneticNodeGene("X2","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("Y2","NetworkSensor",0,false));
#endif
#if DEBUG_USE_DELTAS
        genes.push_back(GeneticNodeGene("DeltaX","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("DeltaY","NetworkSensor",0,false));
#endif
        genes.push_back(GeneticNodeGene("Output_Input_Processing","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
        genes.push_back(GeneticNodeGene("Output_Processing_Output","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
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

    void CheckersExperimentPruning::populateSubstrate(
        shared_ptr<NEAT::GeneticIndividual> individual
        )
    {
        if (substrateIndividual==individual)
        {
            //Don't bother remaking the same substrate
            return;
        }

        substrateIndividual=individual;

        substrate.populateSubstrate(individual);
    }

	void CheckersExperimentPruning::setBoardPosition(unsigned char b[8][8])
	{
		substrate.getNetwork()->reinitialize();
		substrate.getNetwork()->dummyActivation();

		for (int y=0;y<8;y++)
		{
			for (int x=0;x<8;x++)
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
						substrate.setValue( (Node(x,y,0)) , -0.75 );
					}
					else if ( (b[boardx][boardy]&MAN) )
					{
						substrate.setValue( (Node(x,y,0)) , -0.5 );
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
						substrate.setValue( (Node(x,y,0)) , 0.75 );
					}
					else if ( (b[boardx][boardy]&MAN) )
					{
						substrate.setValue( (Node(x,y,0)) , 0.5 );
					}
					else
					{
						throw CREATE_LOCATEDEXCEPTION_INFO("Could not determine piece type (man/king)!");
					}
				}
				else
				{
					//cout << "FOUND NOTHING\n";
					substrate.setValue( (Node(x,y,0)) , 0.0 );
				}
			}
		}

		substrate.getNetwork()->update();
	}

	float CheckersExperimentPruning::getBoardValue(int xpos,int ypos)
	{
		float output = substrate.getValue((Node(xpos,ypos,2)));
		return output;
	}

    CheckersNEATDatatype CheckersExperimentPruning::processEvaluation(
        wxDC *drawContext
        )
    {
        return 5.0;
    }

    void CheckersExperimentPruning::makeMoveCake(uchar b[8][8],int colorToMove,int* retval,bool useAdvisor,bool firstMove)
    {
        int otherColor=BLACK;
        if(colorToMove==BLACK)
            otherColor=WHITE;

        boost::mutex::scoped_lock lock(cakeMutex);
        srand(cakeRandomSeed);

		if(useAdvisor)
		{
			checkersAdvisor = this;
		}
		else
		{
			checkersAdvisor = NULL;
		}

        //cout << "CAKE BEFORE\n";
        //printBoard(b);

        //Play with cake
        POSITION position;
        HCUBE::ucharboardtobitboard(b,&position);
        position.color = colorToMove;
        char output[255];
        memset(output,0,sizeof(char)*255);
        int playnow=0;
        int cakeReturn = cake_getmove(&searchInfo,&position,NODE_BASED,10000000.0,1000000,maxCakeNodes,output,&playnow,0,int(firstMove));

        //cout << "Cake Value: " << cakeReturn << " Output: " << output << endl;
        //cout << "Move: " << moveNotation << endl;
        HCUBE::ucharbitboardtoboard(position,b);
        //printBoard(b);
        //CREATE_PAUSE("");
        //cout << "CAKE AFTER\n";
        //printBoard(b);

        //firstevaluatemin(b,BASE_EVOLUTION_SEARCH_DEPTH);
        //cout << "SimpleCheckers time: ";

		checkersAdvisor = NULL;

		if(cakeReturn==WIN)
		{
			//The cake is a lie!
			*retval = colorToMove;
			return;
		}
		else if(cakeReturn==LOSS)
		{
			//The cake is a lie!
			*retval = otherColor;
			return;
		}
		else if(cakeReturn==DRAW)
		{
			*retval = -2;
			return;
		}

        *retval = getWinner(b,otherColor);
    }

int GIVE_OPPONENT_HANDICAP=false;

    void CheckersExperimentPruning::processGroup(shared_ptr<NEAT::GeneticGeneration> generation)
    {
        //cout << "Processing evaluation...\n";
        shared_ptr<NEAT::GeneticIndividual> individual = group.front();
        //You get 10 points just for entering the game, wahooo!
        individual->setFitness(10);
        numHandCodedStreams=0;
        numHyperNEATStreams = 0;

        populateSubstrate(individual);

        uchar b[8][8];

		CheckersStats emptyStats;
        individual->setUserData(emptyStats.toString());

        //cout << "Playing games with HyperNEAT as black\n";
        for (handCodedType=0;handCodedType<HANDCODED_PLAYER_TESTS;handCodedType++)
        {
            resetBoard(b);

            int retval=-1;
            moves=0;

            double inGameFitness=0.0;
            bool repfound=false;

            for (currentRound=0;currentRound<CHECKERS_MAX_ROUNDS&&retval==-1;currentRound++)
            {
				cakeRandomSeed = (10000 + handCodedType);
				//maxCakeNodes = 808;
				maxCakeNodes = 600;
				//printf("CAKE WITH HYPERNEAT:\n");
                makeMoveCake(b,BLACK,&retval,true,currentRound==0);

		for(int a=0;a<currentRound*2;a++)
		{
			if(memcmp(boardHistory[a],b,sizeof(uchar)*8*8)==0)
			{
				//repetition found
				repfound=true;
				break;
			}
		}
		if(repfound) break;
		memcpy(boardHistory[currentRound*2],b,sizeof(uchar)*8*8);

                if(retval!=-1)
                {
                    break;
                }

				cakeRandomSeed = (10000 + handCodedType)*2;
				//maxCakeNodes = 808;
		if(GIVE_OPPONENT_HANDICAP)
			maxCakeNodes = maxCakeNodes*3/2;
				//printf("CAKE WITHOUT HYPERNEAT:\n");
                makeMoveCake(b,WHITE,&retval,false,false);
		for(int a=0;a<currentRound*2+1;a++)
		{
			if(memcmp(boardHistory[a],b,sizeof(uchar)*8*8)==0)
			{
				//repetition found
				repfound=true;
				break;
			}
		}
		if(repfound) break;
		memcpy(boardHistory[currentRound*2+1],b,sizeof(uchar)*8*8);

                if(retval!=-1)
                {
                    break;
                }

                int whiteMen,blackMen,whiteKings,blackKings;

                //countPieces(gi.board,whiteMen,blackMen,whiteKings,blackKings);
                countPieces(b,whiteMen,blackMen,whiteKings,blackKings);

                //Reward for # of pieces at every turn.
                individual->reward(2 * (12-whiteMen) );
                individual->reward(2 * (blackMen) );

                individual->reward(3 * (12-whiteKings) );
                individual->reward(3 * (blackKings) );

            }


            //cout << "RETVAL: " << (retval==1?"WHITE":(retval==2?"BLACK":"")) << endl;

            if (retval==BLACK)
            {
                //#if CHECKERS_EXPERIMENT_DEBUG
                //cout << "WE WON!\n";
#if CHECKERS_EXPERIMENT_LOG_EVALUATIONS
                for (int a=0;a<moves;a++)
                {
                    printBoard(gameLog[a]);
                }
                cout << gameLogToPDN() << endl;
                CREATE_PAUSE("");
#endif
                //CREATE_PAUSE("");
                //#endif

                if (individual->getUserData().length())
                {
                    CheckersStats stats(individual->getUserData());
                    stats.wins++;
                    individual->setUserData(stats.toString());
                }

                individual->reward(40000);

                //You get bonus fitness for every round you don't have to play
                //if you win
                int roundsLeftOver = CHECKERS_MAX_ROUNDS-currentRound;
                individual->reward(roundsLeftOver*72.0);

            }
            /*NOTE: Tying counts as a loss
            else if (retval==-1) //draw
            {
            cout << "WE TIED!\n";
            individual->reward(200);
            }
            */
            else //loss
            {
#if CHECKERS_EXPERIMENT_DEBUG
                //Final board:
                printBoard(b);
                //CREATE_PAUSE("LOSS!");
#endif
                //if(gi.nummoves<90)
                //{
                //individual->reward(rounds/(((CheckersNEATDatatype)CHECKERS_MAX_ROUNDS)/10));
                //}


#if CHECKERS_EXPERIMENT_PRINT_EVALUATIONS_ON_LOSS
                for (int a=0;a<moves;a++)
                {
                    printBoard(gameLog[a]);
                }
                cout << "PDN: " << gameLogToPDN() << endl;
                CREATE_PAUSE("");
#endif

                if (retval==-1 || retval==-2)
                {
                    //tie
                    if (individual->getUserData().length())
                    {
                        CheckersStats stats(individual->getUserData());
                        stats.ties++;
                        individual->setUserData(stats.toString());
                    }
                }
                else
                {
                    //loss
                    if (individual->getUserData().length())
                    {
                        CheckersStats stats(individual->getUserData());
                        stats.losses++;
                        individual->setUserData(stats.toString());
                    }
                }

                individual->reward(10000);
            }
        }

        CheckersStats stats(individual->getUserData());
		cout << "WINS: " << stats.wins << endl;
    }

    void CheckersExperimentPruning::processIndividualPostHoc(shared_ptr<NEAT::GeneticIndividual> individual)
    {
	//GIVE_OPPONENT_HANDICAP=false;
#if DO_REGULAR_RUN_FOR_POSTHOC
	int prevHandCodedPlayerTests = HANDCODED_PLAYER_TESTS;
	HANDCODED_PLAYER_TESTS = 1000;

        //cout << "INDIVIDUAL FITNESS BEFORE: " << individual->getFitness() << endl;
        clearGroup();
        addIndividualToGroup(individual);
        individual->setUserData(CheckersStats().toString());

        chanceToMakeSecondBestMove=0.0;
        //dumpEvaluationImages = true;
        shared_ptr<GeneticGeneration> dummy;
        processGroup(dummy);
        //dumpEvaluationImages = false;

	HANDCODED_PLAYER_TESTS = prevHandCodedPlayerTests;
#else
        //cout << "INDIVIDUAL FITNESS BEFORE: " << individual->getFitness() << endl;
        clearGroup();
        addIndividualToGroup(individual);
        individual->setUserData(shared_ptr<CheckersStats>(new CheckersStats()));

        for (int a=0;a<100;a++)
        {
            shared_ptr<GeneticGeneration> dummy;
            chanceToMakeSecondBestMove=0.10;
            processGroup(dummy);
            chanceToMakeSecondBestMove=0.0;
            //cout << "INDIVIDUAL FITNESS: " << individual->getFitness() << endl;
        }
#endif
	//GIVE_OPPONENT_HANDICAP=true;
    }

#ifndef HCUBE_NOGUI
    void CheckersExperimentPruning::createIndividualImage(wxDC &drawContext,shared_ptr<NEAT::GeneticIndividual> individual)
    {
    }

    bool CheckersExperimentPruning::handleMousePress(wxMouseEvent& event,wxSize &bitmapSize)
    {
		return true;
    }
#endif

    Experiment* CheckersExperimentPruning::clone()
    {
        CheckersExperimentPruning* experiment = new CheckersExperimentPruning(*this);
        experiment->searchInfo.repcheck = (REPETITION*)malloc((MAXDEPTH+HISTORYOFFSET)*sizeof(REPETITION));

        return experiment;
    }

    void CheckersExperimentPruning::resetGenerationData(shared_ptr<NEAT::GeneticGeneration> generation)
    {
        //generation->setUserData(new TicTacToeStats());
    }

    void CheckersExperimentPruning::addGenerationData(shared_ptr<NEAT::GeneticGeneration> generation,shared_ptr<NEAT::GeneticIndividual> individual)
    {
        //TicTacToeStats* ticTacToeStats = (TicTacToeStats*)generation->getUserData();

        //if (individual->getUserData())
        //(*ticTacToeStats) += *((TicTacToeStats*)individual->getUserData());
    }
}
