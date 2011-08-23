#include "HCUBE_Defines.h"

#include "Experiments/HCUBE_GoExperiment.h"

#include "GoBoardUtil.h"
#include "SgTimeRecord.h"
#include "SgPoint.h"
#include "SgEvaluatedMoves.h"

#include "SpAveragePlayer.h"
#include "SpCapturePlayer.h"
#include "SpDumbTacticalPlayer.h"
#include "SpGreedyPlayer.h"
#include "SpInfluencePlayer.h"
#include "SpLadderPlayer.h"
#include "SpLibertyPlayer.h"
#include "SpMaxEyePlayer.h"
#include "SpMinLibPlayer.h"
#include "SpRandomPlayer.h"
#include "SpSafePlayer.h"

#include "GoUctPlayer.h"

#include "SgSystem.h"

#define GO_EXPERIMENT_DEBUG (0)

#define DEBUG_CHECK_HAND_CODED_HEURISTIC (0)

#define DEBUG_USE_BOARD_EVALUATION_CACHE (1)

#define DEBUG_DUMP_BOARD_LEAF_EVALUATIONS (0)

#define DEBUG_DIRECT_LINKS (0)

#define DEBUG_USE_SOURCE_ABSOLUTE (1)

#define DEBUG_USE_ABSOLUTE (1)

#define DEBUG_USE_DELTAS (1)

#define DEBUG_DO_ITERATIVE_DEEPENING (0)

#define NEAT_SEARCH_HANDICAP (0)

#define HANDCODED_PLAYER_TESTS (1)

#define PLAY_BOTH_SIDES (0)

#define GO_PRINT_ALTERNATE_MOVES (0)

#define GO_EXPERIMENT_INTERACTIVE_PLAY (0)

#define GO_EXPERIMENT_LOG_EVALUATIONS (0)

#define GO_EXPERIMENT_PRINT_EVALUATIONS_ON_LOSS (0)

#define DEBUG_NO_LONG_RANGE_LINKS (0)

#define DEBUG_USE_DELTAS_ON_LONG_RANGE (1)

#define DEBUG_OVERRIDE_SECOND_LAYER_TO_ONE (0)

#define DEBUG_MAX_DELTA_RANGE (2)

#define DEBUG_SHOW_HYPERNEAT_ALTERNATIVES (0)

#ifdef _DEBUG
#define DEBUG_PRINT_GAME_IN_PROGRESS (1)
#else
#define DEBUG_PRINT_GAME_IN_PROGRESS (0)
#endif

#define USE_SINGLE_LAYER_FOR_PIECES (1)

#ifdef _DEBUG
#define UCT_MAX_GAMES (100)
#else
#define UCT_MAX_GAMES (500)
#endif

namespace HCUBE
{
    using namespace NEAT;

    boost::mutex fuegoMutex;

    GoExperiment* currentExperiment = NULL;

    int HyperNEATMoveGenerator::Score(SgPoint p)
    {
        SG_UNUSED(p);
        // HyperNEATMoveGenerator uses whole-board move generation, 
        // it does not work by scoring individual moves.
        SG_ASSERT(false);
        return INT_MIN; 
    }

    void HyperNEATMoveGenerator::GenerateMoves(
        SgEvaluatedMoves& eval,
        SgBlackWhite toPlay
        )
    {
        GoRestoreToPlay restoreToPlay(m_board);
        m_board.SetToPlay(toPlay);
        // Don't permit player to kill its own groups.
        GoRestoreSuicide restoreSuicide(m_board, false);

        //Passing is an option
        eval.AddMove(SG_PASS,0);

        experiment->generateMoves(eval);

        // Otherwise make a random legal move that doesn't fill own eye
        // This will be done automatically by the simple player if no moves
        // have been generated.
        //TODO: SINCE WE ADD PASS MOVE, THIS SHOULD NEVER HAPPEN
    }

    GoExperiment::GoExperiment(string _experimentName, int _threadID)
        :
    Experiment(_experimentName,_threadID),
        currentSubstrateIndex(0),
        goBoardSize(0),
        uctEnabled(false),
        board(NULL),
        self(NULL),
        numGamesPerOpponent(20)
    {
        boost::mutex::scoped_lock lock(fuegoMutex);
        goBook.Read("FuegoBook/book.dat");
        setGoBoardSize(9,true);
    }

    GoExperiment::~GoExperiment()
    {
        delete self;
        for(int a=0;a<(int)opponents.size();a++)
        {
            delete opponents[a];
        }
    }

    GeneticPopulation* GoExperiment::createInitialPopulation(int populationSize)
    {
        GeneticPopulation *population = new GeneticPopulation();
        vector<GeneticNodeGene> genes;

        genes.push_back(GeneticNodeGene("Bias","NetworkSensor",0,false));
		int scalingType = int(NEAT::Globals::getSingleton()->getParameterValue("GoScalingType")+0.1);
		if(
			scalingType == GO_SCALING_NONE ||
			scalingType == GO_SCALING_BASIC ||
			scalingType == GO_SCALING_NO_RANGE
			)
		{
#if DEBUG_USE_SOURCE_ABSOLUTE
        genes.push_back(GeneticNodeGene("X1","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("Y1","NetworkSensor",0,false));
#endif
#if DEBUG_USE_ABSOLUTE
        genes.push_back(GeneticNodeGene("X2","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("Y2","NetworkSensor",0,false));
#endif
		}

#if DEBUG_USE_DELTAS
        genes.push_back(GeneticNodeGene("DeltaX","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("DeltaY","NetworkSensor",0,false));
#endif
        genes.push_back(GeneticNodeGene("Output_WhitePieces_Processing1","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
        genes.push_back(GeneticNodeGene("Output_BlackPieces_Processing1","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
        genes.push_back(GeneticNodeGene("Output_OutOfBounds_Processing1","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
        genes.push_back(GeneticNodeGene("Output_Processing1_Output","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
#if DEBUG_DIRECT_LINKS
        genes.push_back(GeneticNodeGene("Output_0_4","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
        genes.push_back(GeneticNodeGene("Output_1_4","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
        genes.push_back(GeneticNodeGene("Output_2_4","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
#endif
#if GO_EXPERIMENT_ENABLE_BIASES
        throw CREATE_LOCATEDEXCEPTION_INFO("NOT IMPLEMENTED");
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

    void GoExperiment::populateSubstrate(
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

        substrates[substrateNum].populateSubstrate(individual);
    }

    void GoExperiment::printBoard(const GoBoard *boardToPrint)
    {
        for(int r=-1;r<goBoardSize+1;r++)
        {
            for(int c=-1;c<goBoardSize+1;c++)
            {
                if(r==-1||c==-1||r==goBoardSize||c==goBoardSize)
                {
                    cout << "#";
                    continue;
                }

                SgBoardColor boardColor = boardToPrint->GetColor(SgPointUtil::Pt(c+1,r+1));
                if(boardColor==SG_BLACK)
                {
                    cout << "-";
                }
                else if(boardColor==SG_WHITE)
                {
                    cout << "*";
                }
                else
                {
                    cout << " ";
                }
            }
            cout << endl;
        }
    }

    float globalGetMoveHeuristic(const GoBoard *board, SgPoint p)
    {
        if(currentExperiment==NULL)
        {
            cout << "OOPS! Wrong time to use HyperNEAT!!!\n";
        }
        return currentExperiment->getMoveHeuristic(board,p);
    }

    float BEST_WIN_PERCENT = 0.0f;

	void GoExperiment::preprocessIndividual(
		shared_ptr<NEAT::GeneticGeneration> generation,
		shared_ptr<NEAT::GeneticIndividual> individual
		)
	{
		int scalingType = int(NEAT::Globals::getSingleton()->getParameterValue("GoScalingType")+0.1);

		if(generation->getGenerationNumber()==499)
		{
			//Reset BEST_WIN_PERCENT
			BEST_WIN_PERCENT=0.0f;


			if(
				scalingType==GO_SCALING_DELAYED_ABSOLUTE ||
				scalingType==GO_SCALING_DELAYED_ABSOLUTE_NO_RANGE
				)
			{
				if(individual->getNode("X1")==NULL)
				{
		#if DEBUG_USE_SOURCE_ABSOLUTE
					individual->addNode(GeneticNodeGene("X1","NetworkSensor",0,false));
					individual->addNode(GeneticNodeGene("Y1","NetworkSensor",0,false));
		#endif
		#if DEBUG_USE_ABSOLUTE
					individual->addNode(GeneticNodeGene("X2","NetworkSensor",0,false));
					individual->addNode(GeneticNodeGene("Y2","NetworkSensor",0,false));
		#endif
				}
			}
		}
	}

    void GoExperiment::processGroup(shared_ptr<NEAT::GeneticGeneration> generation)
    {
      //boost::progress_timer t;
        shared_ptr<NEAT::GeneticIndividual> individual = group.front();
		int scalingType = int(NEAT::Globals::getSingleton()->getParameterValue("GoScalingType")+0.1);

        boost::mutex::scoped_lock lock(fuegoMutex);
		if( (generation && generation->getGenerationNumber()>=499) || scalingType==GO_SCALING_NONE)
		{
			setGoBoardSize(9,true);
		}
		else
		{
			setGoBoardSize(9,true);
		}

		//cout << "Processing individual...";
#ifndef _DEBUG
		SgDebugToNull();
#endif

        //cout << "Processing evaluation...\n";
        individual->setFitness(10);
		GoStats stats;

        populateSubstrate(individual);

        //Initialize the out of bounds area in the substrate
        int boundsLayer = substrates[0].getLayerIndex("OutOfBounds");
        if(boundsLayer>=0)
        {
            for(int r=0;r<goBoardSize+4;r++)
            {
                for(int c=0;c<goBoardSize+4;c++)
                {
                    if(r<2||c<2||r>goBoardSize+1||c>goBoardSize+1)
                    {
                        substrates[0].setValue( Node(r,c,boundsLayer) , -1 );
                    }
                    else
                    {
                        substrates[0].setValue( Node(r,c,boundsLayer) , 1 );
                    }
                }
            }
        }

        vector<int> wins(opponents.size(),0);

        //Iterate over all opponents
        for(int opponentIndex=0;opponentIndex<int(opponents.size());opponentIndex++)
        {
#if DEBUG_PRINT_GAME_IN_PROGRESS
            cout << "PLAYING OPPONENT: " << opponents[opponentIndex]->Name() << endl;
#endif

            for(int currentGame=0;currentGame<numGamesPerOpponent;currentGame++)
            {
                createGoPlayers(true);
                GoPlayer *opponent = opponents[opponentIndex];

                //Initialize random
                {
                    //Initialize the random seed (Must be positive, 0 means something special)
                    SgRandom::SetSeed(1000 + 1000*currentGame);
                }

                //Initialize the board
                {
                    board->Init(goBoardSize);
                    self->UpdateSubscriber();
                    opponent->UpdateSubscriber();
                }

                bool hyperNEATPassed=false;
                bool opponentPassed=false;

                //Play alternating moves as HyperNEAT and the opponent until the game is decided
                for(int round=0;;round++)
                {
#if DEBUG_PRINT_GAME_IN_PROGRESS
                    cout << "ON ROUND: " << round << endl;
#endif

                    //Alright, let HyperNEAT move
                    SgPoint selfMove;
                    {
                        selfMove = goBook.LookupMove(*board);

                        if(selfMove == SG_NULLMOVE)
                        {
                            currentExperiment = this;
                            defaultValueHeuristic = globalGetMoveHeuristic;
                            SgTimeRecord dummy(true,1e50);
                            selfMove = self->GenMove(dummy,board->ToPlay());
                            defaultValueHeuristic = NULL;
                            currentExperiment = NULL;
                        }
                        else
                        {
                            //cout << "BOOK MOVE\n";
                        }
                    }

                    if(selfMove == SG_PASS)
                    {
                        hyperNEATPassed=true;
#if DEBUG_PRINT_GAME_IN_PROGRESS
                        cout << "HyperNEAT passes\n";
#endif
                    }
                    else if(selfMove==SG_RESIGN)
                    {
						//For now, treat resign as a pass
						selfMove=SG_PASS;
                        hyperNEATPassed=true;
                    }
                    else
                    {
                        hyperNEATPassed=false;
#if DEBUG_PRINT_GAME_IN_PROGRESS
                            cout << "HyperNEAT plays at: " << SgPointUtil::Col(selfMove) << ", " << SgPointUtil::Row(selfMove) << endl;
#endif
                    }
                    board->Play(selfMove);
                    self->UpdateSubscriber();
                    opponent->UpdateSubscriber();

                    if(opponentPassed && hyperNEATPassed)
                    {
                        //The game is over, both players passed
                        break;
                    }

                    if(round==20)
                    {
                        int breakme=0;
                    }

#if DEBUG_PRINT_GAME_IN_PROGRESS
                    printBoard(board);
#endif

                    //Alright, hyperneat has moved, now let opponent move
                    SgPoint opponentMove;
                    {
                        opponentMove = goBook.LookupMove(*board);

                        if(opponentMove == SG_NULLMOVE)
                        {
                            SgTimeRecord dummy(true,1e50);
                            opponentMove = opponent->GenMove(dummy,opponent->Board().ToPlay());
                        }
                        else
                        {
                            //cout << "BOOK MOVE\n";
                        }
                    }

                    if(opponentMove==SG_PASS)
                    {
#if DEBUG_PRINT_GAME_IN_PROGRESS
                        cout << "Opponent passes\n";
#endif
                        opponentPassed=true;
                    }
                    else if(opponentMove==SG_RESIGN)
                    {
						//For now, treat resign as a pass
						opponentMove=SG_PASS;
                        opponentPassed=true;
                    }
                    else
                    {
#if DEBUG_PRINT_GAME_IN_PROGRESS
                        cout << "Opponent plays at: " << SgPointUtil::Col(opponentMove) << ", " << SgPointUtil::Row(opponentMove) << endl;
#endif
                        opponentPassed=false;
                    }

                    board->Play(opponentMove);
                    self->UpdateSubscriber();
                    opponent->UpdateSubscriber();

                    if(opponentPassed && hyperNEATPassed)
                    {
                        //The game is over, both players passed
                        break;
                    }

#if DEBUG_PRINT_GAME_IN_PROGRESS
                    printBoard(board);
#endif

                    int breakme=0;
                }

                {
                    SgPointSet dummySet;
                    float score=0;
                    bool retval = GoBoardUtil::ScorePosition(self->Board(),dummySet,score);

					if(!retval)
					{
						throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
					}

                    if(score>0)
                    {
                        wins[opponentIndex]++;
                        stats.wins++;
                        //cout << "HyperNEAT Wins. Win percent: " << float(stats.wins*100)/(stats.wins+stats.losses) << endl;
                        //cout.flush();

                        //If HyperNEAT wins, hardcode the score.  This prevents overfitting
                        score = goBoardSize*goBoardSize*6;
#if 0
                        cout << "HYPERNEAT WINS GAME AGAINST " << opponent->Name() << endl;
                        printBoard(&board);
#endif
                    }
                    else 
                    {
                        stats.losses++;
                        //cout << "HyperNEAT Loses/ties. Win percent: " << float(stats.wins*100)/(stats.wins+stats.losses) << endl;
                        //cout.flush();
                    }


#if DEBUG_PRINT_GAME_IN_PROGRESS
                    cout << "FINAL SCORE: "  << score << endl;
                    //CREATE_PAUSE("");
#endif
                    if(score<=-goBoardSize*goBoardSize*2)
                    {
                        throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
                    }

                    individual->reward(score+goBoardSize*goBoardSize*2);

                    if(score>0)
                    {
			//Get a bonus for winning
			individual->reward(goBoardSize*goBoardSize);
                        //cout << "GOT A POSITIVE SCORE: " << score << " PLAYING OPPONENT: " << opponents[opponentIndex]->Name() << " / " << currentGame << endl;
                        //printBoard();
                        //CREATE_PAUSE("");
                    }

                    if(!retval)
                    {
                        throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
                    }
                }

                int numHyperNEATStones=0;
                for(int r=0;r<goBoardSize;r++)
                {
                    for(int c=0;c<goBoardSize;c++)
                    {
                        SgBoardColor boardColor = board->GetColor(SgPointUtil::Pt(c+1,r+1));
                        if(boardColor==SG_BLACK)
                        {
                            numHyperNEATStones++;
                        }
                    }
                }

                /*
                if(numHyperNEATStones)
                {
                cout << "NUM HYPERNEAT STONES: " << numHyperNEATStones << endl;
                printBoard();
                }
                */
                //individual->reward(numHyperNEATStones*20);

                if(individual->getFitness()<0)
                {
                    throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
                }

            }
        }

        int totalWins=0;
        int totalGames = opponents.size()*numGamesPerOpponent;
        for(int opponentIndex=0;opponentIndex<int(opponents.size());opponentIndex++)
        {
            totalWins += wins[opponentIndex];
        }
        float winPercent = ((totalWins*100.0f)/totalGames);
        if(winPercent >= BEST_WIN_PERCENT)
        {
            BEST_WIN_PERCENT = winPercent;
            cout << "BEST WIN PERCENT: " << ((totalWins*100.0f)/totalGames) << "%. Fitness: " << individual->getFitness() << ".\n";
        }
        if(totalWins>=int(opponents.size()*numGamesPerOpponent))
        {
            cout << "WON EVERY GAME!!!!!\n";
        }
        if(totalWins>=int(opponents.size()*numGamesPerOpponent)/2)
        {
            cout << "WON HALF OF GAMES!!!!!\n";
        }

        individual->setUserData(stats.toString());

#ifndef _DEBUG
		//In case post-hoc tests are done later on
		SgDebugToWindow();
#endif
		//cout << "...done!\n";
                //cout << "EVALUATION FINISHED\n";
	}

    void GoExperiment::processIndividualPostHoc(shared_ptr<NEAT::GeneticIndividual> individual)
    {
        int prevNumGames = numGamesPerOpponent;
        numGamesPerOpponent = 1000;
        shared_ptr<NEAT::GeneticGeneration> dummy;
        group.push_back(individual);
        processGroup(dummy);
        group.pop_back();
        numGamesPerOpponent = prevNumGames;

        cout << "POST HOC STATS (W/L): " << individual->getUserData() << endl;
    }

#ifndef HCUBE_NOGUI
    void GoExperiment::createIndividualImage(wxDC &drawContext,shared_ptr<NEAT::GeneticIndividual> individual)
    {
    }

    bool GoExperiment::handleMousePress(wxMouseEvent& event,wxSize &bitmapSize)
    {
        return true;
    }
#endif

    Experiment* GoExperiment::clone()
    {
        GoExperiment* experiment = new GoExperiment(experimentName,threadID);

        return experiment;
    }

    void GoExperiment::resetGenerationData(shared_ptr<NEAT::GeneticGeneration> generation)
    {
        //generation->setUserData(new TicTacToeStats());
    }

    void GoExperiment::addGenerationData(shared_ptr<NEAT::GeneticGeneration> generation,shared_ptr<NEAT::GeneticIndividual> individual)
    {
        //TicTacToeStats* ticTacToeStats = (TicTacToeStats*)generation->getUserData();

        //if (individual->getUserData())
        //(*ticTacToeStats) += *((TicTacToeStats*)individual->getUserData());
    }

    void GoExperiment::generateMoves(SgEvaluatedMoves& eval)
    {
        if(goBoardSize != self->Board().Size())
        {
            cout << "DIFFERENT SIZE BOARD IS NOT SUPPORTED YET!\n";
            throw CREATE_LOCATEDEXCEPTION_INFO("DIFFERENT SIZE BOARD IS NOT SUPPORTED YET!");
        }

        //First, have HyperNEAT generate a move
		substrates[0].getNetwork()->reinitialize();
        for(int r=0;r<goBoardSize;r++)
        {
            for(int c=0;c<goBoardSize;c++)
            {
                SgBoardColor boardColor = self->Board().GetColor(SgPointUtil::Pt(c+1,r+1));
#if USE_SINGLE_LAYER_FOR_PIECES
                if(boardColor==SG_BLACK)
                {
                    substrates[0].setValue( Node(r,c,0) , 1 );
                }
                else if(boardColor==SG_WHITE)
                {
                    substrates[0].setValue( Node(r,c,0) , -1 );
                }
                else //c==SG_EMPTY
                {
                    substrates[0].setValue( Node(r,c,0) , 0 );
                }
#else
                if(boardColor==SG_BLACK)
                {
                    substrates[0].setValue( Node(r,c,0) , 1 );
                }
                else
                {
                    substrates[0].setValue( Node(r,c,0) , -1 );
                }

                if(boardColor==SG_WHITE)
                {
                    substrates[0].setValue( Node(r,c,1) , 1 );
                }
                else
                {
                    substrates[0].setValue( Node(r,c,1) , -1 );
                }
#endif
            }
        }
        substrates[0].getNetwork()->dummyActivation();
        substrates[0].getNetwork()->update();

        //Get a sorted list of moves to make
        moves.clear();
        for(int r=0;r<goBoardSize;r++)
        {
            for(int c=0;c<goBoardSize;c++)
            {
                SgPoint p = SgPointUtil::Pt(c+1,r+1);
                if(
                    self->Board().IsLegal(p)
                    )
                {
                    int index = substrates[0].getLayerIndex("Output");
                    float moveMotivation = substrates[0].getValue(Node(r,c,index));

                    if(moveMotivation>0)
                    {
                        moves.push_back(MoveRating(moveMotivation,p));
                    }
                }
            }
        }
        sort(moves.begin(),moves.end());

        int curSize = eval.moveListSize();
        for(int a=int(moves.size())-1;a>=0;a--)
        {
            //Since the values are floats, don't bother with them and just add the best move
            eval.AddMove(moves[a].move,1000);
            if(curSize != eval.moveListSize())
            {
                //Move was accepted, we are done
                break;
            }
        }
    }

    float GoExperiment::getMoveHeuristic(const GoBoard *board, SgPoint p)
    {
        //printBoard(board);

        if(goBoardSize != board->Size())
        {
            cout << "DIFFERENT SIZE BOARD IS NOT SUPPORTED YET!\n";
            throw CREATE_LOCATEDEXCEPTION_INFO("DIFFERENT SIZE BOARD IS NOT SUPPORTED YET!");
        }

        bool boardChanged=false;
        bool boardEmpty=true;
        //First, have HyperNEAT generate a move
        for(int r=0;r<goBoardSize;r++)
        {
            for(int c=0;c<goBoardSize;c++)
            {
                SgBoardColor boardColor = board->GetColor(SgPointUtil::Pt(c+1,r+1));
#if USE_SINGLE_LAYER_FOR_PIECES
                if(boardColor==SG_BLACK)
                {
                    if( substrates[0].getValue( Node(r,c,0) ) != 1.0f)
                    {
                        boardChanged = true;
                    }
                    boardEmpty = false;
                    substrates[0].setValue( Node(r,c,0) , 1.0f );
                }
                else if(boardColor==SG_WHITE)
                {
                    if( substrates[0].getValue( Node(r,c,0) ) != -1.0f)
                    {
                        boardChanged = true;
                    }
                    boardEmpty = false;
                    substrates[0].setValue( Node(r,c,0) , -1.0f );
                }
                else //c==SG_EMPTY
                {
                    if( substrates[0].getValue( Node(r,c,0) ) != 0.0f)
                    {
                        boardChanged = true;
                    }
                    substrates[0].setValue( Node(r,c,0) , 0.0f );
                }
#else
                if(boardColor==SG_BLACK)
                {
                    substrates[0].setValue( Node(r,c,0) , 1 );
                }
                else
                {
                    substrates[0].setValue( Node(r,c,0) , -1 );
                }

                if(boardColor==SG_WHITE)
                {
                    substrates[0].setValue( Node(r,c,1) , 1 );
                }
                else
                {
                    substrates[0].setValue( Node(r,c,1) , -1 );
                }
#endif
            }
        }

        if(boardChanged)
        {
            int breakme=0;
        }

        if(boardChanged || boardEmpty)
        {
            //cout << "Board Changed or empty, evaluating\n";
            substrates[0].getNetwork()->dummyActivation();
            substrates[0].getNetwork()->update();
        }

        int index = substrates[0].getLayerIndex("Output");
        float rawValue = substrates[0].getValue(Node( SgPointUtil::Row(p)-1 , SgPointUtil::Col(p)-1 ,index));

        //negative values don't make sense, so scale to be [0-1]
        return (rawValue+1.0) / 2.0;
    }

    void GoExperiment::setGoBoardSize(int newSize,bool enableUCT)
    {
        if(goBoardSize==newSize && uctEnabled==enableUCT)
            return;
		cout << "SETTING BOARD SIZE TO : " << newSize << "\n";

        goBoardSize = newSize;
        uctEnabled = enableUCT;

        layerInfo = NEAT::LayeredSubstrateInfo();

        //Piece input layer (a)
        layerInfo.layerSizes.push_back(JGTL::Vector2<int>(goBoardSize,goBoardSize));
        layerInfo.layerValidSizes.push_back(JGTL::Vector2<int>(goBoardSize,goBoardSize));
        layerInfo.layerNames.push_back("WhitePieces");
        layerInfo.layerIsInput.push_back(true);
        layerInfo.layerLocations.push_back(JGTL::Vector3<float>(0,0,0));

        //Enemy piece input layer (b) if USE_SINGLE_LAYER_FOR_PIECES is 0
        //layerInfo.layerSizes.push_back(JGTL::Vector2<int>(5,5));
        //layerInfo.layerValidSizes.push_back(JGTL::Vector2<int>(goBoardSize,goBoardSize));
        //layerInfo.layerNames.push_back("BlackPieces");
        //layerInfo.layerIsInput.push_back(true);
        //layerInfo.layerLocations.push_back(JGTL::Vector3<float>(3,0,0));

        //OutOFBounds input layer (c)
        //layerInfo.layerSizes.push_back(JGTL::Vector2<int>(goBoardSize+4,goBoardSize+4));
        //layerInfo.layerNames.push_back("OutOfBounds");
        //layerInfo.layerIsInput.push_back(true);
        //layerInfo.layerLocations.push_back(JGTL::Vector3<float>(-6,0,0));

        //ProcessingLayer (d)
        layerInfo.layerSizes.push_back(JGTL::Vector2<int>(goBoardSize,goBoardSize));
        layerInfo.layerValidSizes.push_back(JGTL::Vector2<int>(goBoardSize,goBoardSize));
        layerInfo.layerNames.push_back("Processing1");
        layerInfo.layerIsInput.push_back(false);
        layerInfo.layerLocations.push_back(JGTL::Vector3<float>(0,4,0));

        //OutputLayer (e)
        layerInfo.layerSizes.push_back(JGTL::Vector2<int>(goBoardSize,goBoardSize));
        layerInfo.layerValidSizes.push_back(JGTL::Vector2<int>(goBoardSize,goBoardSize));
        layerInfo.layerNames.push_back("Output");
        layerInfo.layerIsInput.push_back(false);
        layerInfo.layerLocations.push_back(JGTL::Vector3<float>(0,8,0));

        //inputs connect to hidden
        layerInfo.layerAdjacencyList.push_back(std::pair<string,string>("WhitePieces","Processing1"));
        layerInfo.layerAdjacencyList.push_back(std::pair<string,string>("BlackPieces","Processing1"));
        layerInfo.layerAdjacencyList.push_back(std::pair<string,string>("OutOfBounds","Processing1"));

        //hidden connects to output
        layerInfo.layerAdjacencyList.push_back(std::pair<string,string>("Processing1","Output"));

        layerInfo.normalize = true;
        layerInfo.useOldOutputNames = false;

		int scalingType = int(NEAT::Globals::getSingleton()->getParameterValue("GoScalingType")+0.1);
		if(
			scalingType == GO_SCALING_DELAYED_ABSOLUTE_NO_RANGE ||
			scalingType == GO_SCALING_DELTA_ONLY_NO_RANGE
			)
		{
			layerInfo.maxConnectionLength = 5;
		}
		else
		{
			layerInfo.maxConnectionLength = goBoardSize;
		}

		cout << "SCALING TYPE: " << scalingType << "\n";
        substrates[0] = NEAT::LayeredSubstrate<GoNEATDatatype>();
        substrates[0].setLayerInfo(layerInfo);

        createGoPlayers(enableUCT);
    }

    void GoExperiment::createGoPlayers(bool enableUCT)
    {
        if(board)
        {
            delete board;
            board = NULL;
        }

        board = new GoBoard(goBoardSize);

        if(self)
        {
            delete self;
            self = NULL;
        }

        for(int a=0;a<(int)opponents.size();a++)
        {
            delete opponents[a];
        }
        opponents.clear();

        if(enableUCT==false)
        {
        //This guy is not very good in 5x5 Go (Loses on generation 2)
        //opponents.push_back(new SpGreedyPlayer(*board));

        //This guy is not very good in 5x5 Go (Loses on generation 2)
        //opponents.push_back(new SpAveragePlayer(*board));

        //This guy is crap in 5x5 Go
        //opponents.push_back(new SpCapturePlayer(*board));

        //This guy is crap in 5x5 Go
        //opponents.push_back(new SpDumbTacticalPlayer(*board));

        //This guy is not very good in 5x5 Go (Loses on generation 2)
        //opponents.push_back(new SpInfluencePlayer(*board));

        //This guy is crap in 5x5 Go
        //opponents.push_back(new SpLadderPlayer(*board));

        //This guy is crap in 5x5 Go
        opponents.push_back(new SpLibertyPlayer(*board));

        //This guy is crap in 5x5 Go
        //opponents.push_back(new SpMaxEyePlayer(*board));

        //This guy is not very good in 5x5 Go (Loses on generation 2)
        //opponents.push_back(new SpMinLibPlayer(*board));

        //This guy is crap in 5x5 Go
        //opponents.push_back(new SpRandomPlayer(*board));

        //This guy is crap in 5x5 Go
        //opponents.push_back(new SpSafePlayer(*board));

            //0-Ply hyperneat player
            self = new HyperNEATPlayer(*board,this);
        }
        else
        {
        GoUctPlayer<GoUctGlobalSearch<GoUctPlayoutPolicy<GoUctBoard>,
            GoUctPlayoutPolicyFactory<GoUctBoard> >,
            GoUctGlobalSearchState<GoUctPlayoutPolicy<GoUctBoard> > > *uctPlayer;

        self = uctPlayer = new
            GoUctPlayer<GoUctGlobalSearch<GoUctPlayoutPolicy<GoUctBoard>,
            GoUctPlayoutPolicyFactory<GoUctBoard> >,
            GoUctGlobalSearchState<GoUctPlayoutPolicy<GoUctBoard> > >(*board);
		uctPlayer->Search().SetMoveSelect(SG_UCTMOVESELECT_VALUE);
        //uctPlayer->SetMaxGames( (UCT_MAX_GAMES*2)/3 );
        uctPlayer->SetMaxGames( UCT_MAX_GAMES );
        uctPlayer->SetIgnoreClock(true);
        uctPlayer->SetWriteDebugOutput(false);
        //uctPlayer->SetUseRootFilter(false);
        //RAVE is supposed to generalize between moves.  HyperNEAT should be able to do this instead of RAVE.
        uctPlayer->GlobalSearch().SetRave(false);

        uctPlayer = new
            GoUctPlayer<GoUctGlobalSearch<GoUctPlayoutPolicy<GoUctBoard>,
            GoUctPlayoutPolicyFactory<GoUctBoard> >,
                GoUctGlobalSearchState<GoUctPlayoutPolicy<GoUctBoard> > >(*board);
		uctPlayer->Search().SetMoveSelect(SG_UCTMOVESELECT_VALUE);
        uctPlayer->SetMaxGames(UCT_MAX_GAMES);
        uctPlayer->SetIgnoreClock(true);
        uctPlayer->SetWriteDebugOutput(false);
        //uctPlayer->SetUseRootFilter(false);
        //RAVE is supposed to generalize between moves.  HyperNEAT should be able to do this instead of RAVE.
        uctPlayer->GlobalSearch().SetRave(false);

        opponents.push_back(uctPlayer);
        }

        board->Rules().SetCaptureDead(true);
        self->Board().Rules().SetCaptureDead(true);

        for(int a=0;a<(int)opponents.size();a++)
        {
            opponents[a]->Board().Rules().SetCaptureDead(true);
        }

    }
}
