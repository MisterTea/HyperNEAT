#include "HCUBE_Defines.h"

#include "Experiments/HCUBE_CheckersExperiment.h"

#define CHECKERS_EXPERIMENT_DEBUG (0)

#define DEBUG_CHECK_HAND_CODED_HEURISTIC (0)

#define DEBUG_USE_BOARD_EVALUATION_CACHE (1)

#define DEBUG_DUMP_BOARD_LEAF_EVALUATIONS (0)

#define DEBUG_DIRECT_LINKS (0)

#define DEBUG_USE_ABSOLUTE (1)

#define DEBUG_USE_DELTAS (1)

#define DEBUG_DO_ITERATIVE_DEEPENING (0)

#define BASE_EVOLUTION_SEARCH_DEPTH (4)

#define NEAT_SEARCH_HANDICAP (-2)

#define HANDCODED_PLAYER_TESTS (6)

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

ofstream handCodedEvalStream;
ofstream handCodedTreeStream;
ofstream hyperNEATEvalStream;
ofstream hyperNEATTreeStream;

#define DEBUG_SHOW_HYPERNEAT_ALTERNATIVES (0)

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

    mutex cakeMutex;

    CheckersExperiment::CheckersExperiment(string _experimentName,int _threadID)
        :
    Experiment(_experimentName,_threadID),
        currentSubstrateIndex(0),
        from(255,255),
        DEBUG_USE_HANDCODED_EVALUATION(0),
        DEBUG_USE_HYPERNEAT_EVALUATION(0),
        chanceToMakeSecondBestMove(0.0),
		dumpEvaluationImages(false),
		cakeRandomSeed(1000)
    {
        searchInfo.repcheck = NULL;
        //boardEvaluationCaches[0].resize(10000);
        //boardEvaluationCaches[1].resize(10000);

        numNodesX[0] = numNodesY[0] = 8;
        numNodesX[1] = numNodesY[1] = 8;
        numNodesX[2] = numNodesY[2] = 1;

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

        layerInfo.layerSizes.push_back(Vector2<int>(1,1));
        layerInfo.layerIsInput.push_back(false);
        layerInfo.layerLocations.push_back(Vector3<float>(0,8,0));
        layerInfo.layerNames.push_back("Output");

        layerInfo.layerAdjacencyList.push_back(std::pair<string,string>("Input","Processing"));
        layerInfo.layerAdjacencyList.push_back(std::pair<string,string>("Processing","Output"));

        layerInfo.normalize = false;
        layerInfo.useOldOutputNames = true;
		layerInfo.layerValidSizes = layerInfo.layerSizes;

        for(int a=0;a<2;a++)
        {
            substrates[a].setLayerInfo(layerInfo);
        }
    }

    CheckersExperiment::~CheckersExperiment()
    {
        if(searchInfo.repcheck)
            free(searchInfo.repcheck);
    }

    GeneticPopulation* CheckersExperiment::createInitialPopulation(int populationSize)
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

    void CheckersExperiment::populateSubstrate(
        shared_ptr<NEAT::GeneticIndividual> individual,
        int substrateNum
        )
    {
        NEAT::LayeredSubstrate<CheckersNEATDatatype>* substrate;
        if (substrateNum>=2)
        {
            throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: INVALID SUBSTRATE INDEX!");
        }

        if (substrateIndividuals[substrateNum]==individual)
        {
            //Don't bother remaking the same substrate
            return;
        }

        substrateIndividuals[substrateNum]=individual;

        substrate = &substrates[substrateNum];

        //Clear the evaluation cache
        for (int a=0;a<65536;a++)
        {
            boardEvaluationCaches[substrateNum][a].clear();
            boardEvaluationCaches[substrateNum][a].reserve(0);
        }

        //Clear the state list caches
        for (int a=0;a<65536;a++)
        {
            boardStateLists[substrateNum][BLACK][a].clear();
            boardStateLists[substrateNum][BLACK][a].reserve(0);
        }
        for (int a=0;a<65536;a++)
        {
            boardStateLists[substrateNum][WHITE][a].clear();
            boardStateLists[substrateNum][WHITE][a].reserve(0);
        }

        substrate->populateSubstrate(individual);
    }

    CheckersNEATDatatype CheckersExperiment::processEvaluation(
        wxDC *drawContext
        )
    {
        return 5.0;
    }

    //For evaluation.  Positive means winning for black, and negative means
    //winning for white.
    pair<CheckersNEATDatatype,int> CheckersExperiment::evaluateLeafWhite(uchar b[8][8])
    {
        if (DEBUG_USE_HYPERNEAT_EVALUATION)
        {
            throw CREATE_LOCATEDEXCEPTION_INFO("WRONG TIME FOR HYPERNEAT EVALUATION!");
        }

        //cout << "FOUND A WHITE EVALUATION!\n";
        int colorToMove = WHITE;

        CheckersNEATDatatype retval=0.0;

        const int turn=2;   //color to move gets +turn
        const int brv=3;    //multiplier for back rank
        const int kcv=5;    //multiplier for kings in center
        const int mcv=1;    //multiplier for men in center

        const int mev=1;    //multiplier for men on edge
        const int kev=5;    //multiplier for kings on edge
        const int cramp=5;  //multiplier for cramp

        const int opening=-2; // multipliers for tempo
        const int midgame=-1;
        const int endgame=2;
        const int intactCheckersNEATDatatypecorner=3;

        bool printJGResults=false;

        if (colorToMove==BLACK)
        {
            retval += turn;
        }
        else
        {
            retval -= turn;
        }

        /* cramp */

        if (b[0][4]==(BLACK|MAN) && b[1][5]==(WHITE|MAN)) retval+=cramp;
        if (b[7][3]==(WHITE|MAN) && b[6][2]==(BLACK|MAN)) retval-=cramp;

        int code=0;

        /* back rank guard */

        if (b[0][0] & MAN) code++;
        if (b[2][0] & MAN) code+=2;
        if (b[4][0] & MAN) code+=4;
        if (b[6][0] & MAN) code+=8;
        switch (code)
        {
        case 0:
            code=0;
            break;
        case 1:
            code=-1;
            break;
        case 2:
            code=1;
            break;
        case 3:
            code=0;
            break;
        case 4:
            code=1;
            break;
        case 5:
            code=1;
            break;
        case 6:
            code=2;
            break;
        case 7:
            code=1;
            break;
        case 8:
            code=1;
            break;
        case 9:
            code=0;
            break;
        case 10:
            code=7;
            break;
        case 11:
            code=4;
            break;
        case 12:
            code=2;
            break;
        case 13:
            code=2;
            break;
        case 14:
            code=9;
            break;
        case 15:
            code=8;
            break;
        }
        int backrank=code;


        code=0;
        if (b[1][7] & MAN) code+=8;
        if (b[3][7] & MAN) code+=4;
        if (b[5][7] & MAN) code+=2;
        if (b[7][7] & MAN) code++;
        switch (code)
        {
        case 0:
            code=0;
            break;
        case 1:
            code=-1;
            break;
        case 2:
            code=1;
            break;
        case 3:
            code=0;
            break;
        case 4:
            code=1;
            break;
        case 5:
            code=1;
            break;
        case 6:
            code=2;
            break;
        case 7:
            code=1;
            break;
        case 8:
            code=1;
            break;
        case 9:
            code=0;
            break;
        case 10:
            code=7;
            break;
        case 11:
            code=4;
            break;
        case 12:
            code=2;
            break;
        case 13:
            code=2;
            break;
        case 14:
            code=9;
            break;
        case 15:
            code=8;
            break;
        }
        backrank-=code;
        retval+=brv*backrank;

        int value[17];
        memset(value,0,sizeof(int)*17);

        value[WHITE|MAN] = 1;
        value[WHITE|KING] = 16;

        value[BLACK|MAN] = 256;
        value[BLACK|KING] = 4096;

        code=0;
        for (int boardx=0;boardx<8;boardx++)
        {
            for (int boardy=(boardx%2);boardy<8;boardy+=2)
            {
                code += value[b[boardx][boardy]];
            }
        }

        int nwm = code % 16;
        int nwk = (code>>4) % 16;
        int nbm = (code>>8) % 16;
        int nbk = (code>>12) % 16;


        int v1=100*nbm+130*nbk;
        int v2=100*nwm+130*nwk;

        retval+=v1-v2;                       /*material values*/
        retval+=(250*(v1-v2))/(v1+v2);      /*favor exchanges if in material plus*/

        int nm=nbm+nwm;
        int nk=nbk+nwk;

        /* intact CheckersNEATDatatype corner */
        if (b[6][0]==(BLACK|MAN))
        {
            if (b[5][1]==(BLACK|MAN) || b[7][1]==(BLACK|MAN))
                retval+=intactCheckersNEATDatatypecorner;
        }

        if (b[1][7]==(WHITE|MAN))
        {
            if (b[0][6]==(WHITE|MAN) || b[2][6]==(WHITE|MAN))
                retval-=intactCheckersNEATDatatypecorner;
        }

        int nbmc=0,nbkc=0,nwmc=0,nwkc=0;

        int nbme=0,nbke=0,nwme=0,nwke=0;

        int tempo=0;

        for (int x=0;x<8;x++)
        {
            for (int y=(x%2);y<8;y+=2)
            {
                if (x>=2&&x<=5&&y>=2&&y<=5)
                {
                    /* center control */
                    if (b[x][y] == (BLACK|MAN)) nbmc++;
                    if (b[x][y] == (BLACK|KING)) nbkc++;
                    if (b[x][y] == (WHITE|MAN)) nwmc++;
                    if (b[x][y] == (WHITE|KING)) nwkc++;
                }
                else if (x==0||x==7||y==0||y==7)
                {
                    /* edge */
                    if (b[x][y] == (BLACK|MAN)) nbme++;
                    if (b[x][y] == (BLACK|KING)) nbke++;
                    if (b[x][y] == (WHITE|MAN)) nwme++;
                    if (b[x][y] == (WHITE|KING)) nwke++;
                }

                /* tempo */
                if (b[x][y] == (BLACK|MAN))
                {
                    tempo += y;
                }
                if (b[x][y] == (WHITE|MAN))
                {
                    tempo -= (7-y);
                }
            }
        }

        retval+=(nbmc-nwmc)*mcv;
        retval+=(nbkc-nwkc)*kcv;

        retval-=(nbme-nwme)*mev;
        retval-=(nbke-nwke)*kev;

        if (nm>=16) retval+=opening*tempo;
        if ((nm<=15) && (nm>=12)) retval+=midgame*tempo;
        if (nm<9) retval+=endgame*tempo;

        if (nbk+nbm>nwk+nwm && nwk<3)
        {
            if (b[0][6]== (WHITE|KING)) retval-=15;
            if (b[1][7]== (WHITE|KING)) retval-=15;
            if (b[6][0]== (WHITE|KING)) retval-=15;
            if (b[7][1]== (WHITE|KING)) retval-=15;
        }
        if (nwk+nwm>nbk+nbm && nbk<3)
        {
            if (b[0][6]== (BLACK|KING)) retval+=15;
            if (b[1][7]== (BLACK|KING)) retval+=15;
            if (b[6][0]== (BLACK|KING)) retval+=15;
            if (b[7][1]== (BLACK|KING)) retval+=15;
        }

        int stonesinsystem=0;

        /* the move */
        if (nwm+nwk-nbk-nbm==0)
        {
            if (colorToMove==BLACK)
            {
                for (int y=0;y<8;y+=2)
                {
                    for (int x=(y%2);x<8;x+=2)
                    {
                        if (b[x][y]!=0) stonesinsystem++;
                    }
                }
                if (stonesinsystem % 2)
                {
                    if (nm+nk<=12) retval++;
                    if (nm+nk<=10) retval++;
                    if (nm+nk<=8) retval+=2;
                    if (nm+nk<=6) retval+=2;
                }
                else
                {
                    if (nm+nk<=12) retval--;
                    if (nm+nk<=10) retval--;
                    if (nm+nk<=8) retval-=2;
                    if (nm+nk<=6) retval-=2;
                }
            }
            else
            {
                for (int y=1;y<8;y+=2)
                {
                    for (int x=(y%2);x<8;x+=2)
                    {
                        if (b[x][y]!=0) stonesinsystem++;
                    }
                }
                if ((stonesinsystem % 2) == 0)
                {
                    if (nm+nk<=12) retval++;
                    if (nm+nk<=10) retval++;
                    if (nm+nk<=8) retval+=2;
                    if (nm+nk<=6) retval+=2;
                }
                else
                {
                    if (nm+nk<=12) retval--;
                    if (nm+nk<=10) retval--;
                    if (nm+nk<=8) retval-=2;
                    if (nm+nk<=6) retval-=2;
                }
            }
        }


#if DEBUG_CHECK_HAND_CODED_HEURISTIC
        {
            int board[256];
            int color = colorToMove;
            int i,j;
            /* initialize board */
            for (i=0;i<46;i++)
                board[i]=OCCUPIED;
            for (i=5;i<=40;i++)
                board[i]=FREE;
            board[5]=b[0][0];
            board[6]=b[2][0];
            board[7]=b[4][0];
            board[8]=b[6][0];
            board[10]=b[1][1];
            board[11]=b[3][1];
            board[12]=b[5][1];
            board[13]=b[7][1];
            board[14]=b[0][2];
            board[15]=b[2][2];
            board[16]=b[4][2];
            board[17]=b[6][2];
            board[19]=b[1][3];
            board[20]=b[3][3];
            board[21]=b[5][3];
            board[22]=b[7][3];
            board[23]=b[0][4];
            board[24]=b[2][4];
            board[25]=b[4][4];
            board[26]=b[6][4];
            board[28]=b[1][5];
            board[29]=b[3][5];
            board[30]=b[5][5];
            board[31]=b[7][5];
            board[32]=b[0][6];
            board[33]=b[2][6];
            board[34]=b[4][6];
            board[35]=b[6][6];
            board[37]=b[1][7];
            board[38]=b[3][7];
            board[39]=b[5][7];
            board[40]=b[7][7];
            for (i=5;i<=40;i++)
                if (board[i]==0) board[i]=FREE;
            for (i=9;i<=36;i+=9)
                board[i]=OCCUPIED;
            /* board initialized */

            int eval;
            int v1,v2;
            int nbm,nbk,nwm,nwk;
            int nbmc=0,nbkc=0,nwmc=0,nwkc=0;
            int nbme=0,nbke=0,nwme=0,nwke=0;
            int code=0;
            static const int value[17]=
            {
                0,0,0,0,0,1,256,0,0,16,4096,0,0,0,0,0,0
            };
            static const int edge[14]=
            {
                5,6,7,8,13,14,22,23,31,32,37,38,39,40
            };
            static const int center[8]=
            {
                15,16,20,21,24,25,29,30
            };
            static const int row[41]=
            {
                0,0,0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,0,3,3,3,3,4,4,4,4,0,5,5,5,5,6,6,6,6,0,7,7,7,7
            };
            static const int safeedge[4]=
            {
                8,13,32,37
            };

            int tempo=0;
            int nm,nk;

            const int turn=2;   //color to move gets +turn
            const int brv=3;    //multiplier for back rank
            const int kcv=5;    //multiplier for kings in center
            const int mcv=1;    //multiplier for men in center

            const int mev=1;    //multiplier for men on edge
            const int kev=5;    //multiplier for kings on edge
            const int cramp=5;  //multiplier for cramp

            const int opening=-2; // multipliers for tempo
            const int midgame=-1;
            const int endgame=2;
            const int intactCheckersNEATDatatypecorner=3;


            int backrank;

            int stonesinsystem=0;

#ifdef STATISTICS
            evaluations++;
#endif


            for (i=5;i<=40;i++)
                code+=value[board[i]];

            nwm = code % 16;
            nwk = (code>>4) % 16;
            nbm = (code>>8) % 16;
            nbk = (code>>12) % 16;


            v1=100*nbm+130*nbk;
            v2=100*nwm+130*nwk;

            eval=v1-v2;                       /*material values*/
            eval+=(250*(v1-v2))/(v1+v2);      /*favor exchanges if in material plus*/

            nm=nbm+nwm;
            nk=nbk+nwk;
            /*--------- fine evaluation below -------------*/

            if (color == BLACK) eval+=turn;
            else eval-=turn;
            /*    (white)
            37  38  39  40
            32  33  34  35
            28  29  30  31
            23  24  25  26
            19  20  21  22
            14  15  16  17
            10  11  12  13
            5   6   7   8
            (black)   */
            /* cramp */
            if (board[23]==(BLACK|MAN) && board[28]==(WHITE|MAN)) eval+=cramp;
            if (board[22]==(WHITE|MAN) && board[17]==(BLACK|MAN)) eval-=cramp;

            /* back rank guard */

            code=0;
            if (board[5] & MAN) code++;
            if (board[6] & MAN) code+=2;
            if (board[7] & MAN) code+=4;
            if (board[8] & MAN) code+=8;
            switch (code)
            {
            case 0:
                code=0;
                break;
            case 1:
                code=-1;
                break;
            case 2:
                code=1;
                break;
            case 3:
                code=0;
                break;
            case 4:
                code=1;
                break;
            case 5:
                code=1;
                break;
            case 6:
                code=2;
                break;
            case 7:
                code=1;
                break;
            case 8:
                code=1;
                break;
            case 9:
                code=0;
                break;
            case 10:
                code=7;
                break;
            case 11:
                code=4;
                break;
            case 12:
                code=2;
                break;
            case 13:
                code=2;
                break;
            case 14:
                code=9;
                break;
            case 15:
                code=8;
                break;
            }
            backrank=code;


            code=0;
            if (board[37] & MAN) code+=8;
            if (board[38] & MAN) code+=4;
            if (board[39] & MAN) code+=2;
            if (board[40] & MAN) code++;
            switch (code)
            {
            case 0:
                code=0;
                break;
            case 1:
                code=-1;
                break;
            case 2:
                code=1;
                break;
            case 3:
                code=0;
                break;
            case 4:
                code=1;
                break;
            case 5:
                code=1;
                break;
            case 6:
                code=2;
                break;
            case 7:
                code=1;
                break;
            case 8:
                code=1;
                break;
            case 9:
                code=0;
                break;
            case 10:
                code=7;
                break;
            case 11:
                code=4;
                break;
            case 12:
                code=2;
                break;
            case 13:
                code=2;
                break;
            case 14:
                code=9;
                break;
            case 15:
                code=8;
                break;
            }
            backrank-=code;
            eval+=brv*backrank;


            /* intact CheckersNEATDatatype corner */
            if (board[8]==(BLACK|MAN))
            {
                if (board[12]==(BLACK|MAN) || board[13]==(BLACK|MAN))
                    eval+=intactCheckersNEATDatatypecorner;
            }

            if (board[37]==(WHITE|MAN))
            {
                if (board[32]==(WHITE|MAN) || board[33]==(WHITE|MAN))
                    eval-=intactCheckersNEATDatatypecorner;
            }
            /*    (white)
            37  38  39  40
            32  33  34  35
            28  29  30  31
            23  24  25  26
            19  20  21  22
            14  15  16  17
            10  11  12  13
            5   6   7   8
            (black)   */

            /* center control */
            for (i=0;i<8;i++)
            {
                if (board[center[i]] != FREE)
                {
                    if (board[center[i]] == (BLACK|MAN)) nbmc++;
                    if (board[center[i]] == (BLACK|KING)) nbkc++;
                    if (board[center[i]] == (WHITE|MAN)) nwmc++;
                    if (board[center[i]] == (WHITE|KING)) nwkc++;
                }
            }
            eval+=(nbmc-nwmc)*mcv;
            eval+=(nbkc-nwkc)*kcv;

            /*edge*/
            for (i=0;i<14;i++)
            {
                if (board[edge[i]] != FREE)
                {
                    if (board[edge[i]] == (BLACK|MAN)) nbme++;
                    if (board[edge[i]] == (BLACK|KING)) nbke++;
                    if (board[edge[i]] == (WHITE|MAN)) nwme++;
                    if (board[edge[i]] == (WHITE|KING)) nwke++;
                }
            }
            eval-=(nbme-nwme)*mev;
            eval-=(nbke-nwke)*kev;



            /* tempo */
            for (i=5;i<41;i++)
            {
                if (board[i]== (BLACK | MAN))
                    tempo+=row[i];
                if (board[i]== (WHITE | MAN))
                    tempo-=7-row[i];
            }

            if (nm>=16) eval+=opening*tempo;
            if ((nm<=15) && (nm>=12)) eval+=midgame*tempo;
            if (nm<9) eval+=endgame*tempo;



            for (i=0;i<4;i++)
            {
                if (nbk+nbm>nwk+nwm && nwk<3)
                {
                    if (board[safeedge[i]]== (WHITE|KING))
                        eval-=15;
                }
                if (nwk+nwm>nbk+nbm && nbk<3)
                {
                    if (board[safeedge[i]]==(BLACK|KING))
                        eval+=15;
                }
            }





            /* the move */
            if (nwm+nwk-nbk-nbm==0)
            {
                if (color==BLACK)
                {
                    for (i=5;i<=8;i++)
                    {
                        for (j=0;j<4;j++)
                        {
                            if (board[i+9*j]!=FREE) stonesinsystem++;
                        }
                    }
                    if (stonesinsystem % 2)
                    {
                        if (nm+nk<=12) eval++;
                        if (nm+nk<=10) eval++;
                        if (nm+nk<=8) eval+=2;
                        if (nm+nk<=6) eval+=2;
                    }
                    else
                    {
                        if (nm+nk<=12) eval--;
                        if (nm+nk<=10) eval--;
                        if (nm+nk<=8) eval-=2;
                        if (nm+nk<=6) eval-=2;
                    }
                }
                else
                {
                    for (i=10;i<=13;i++)
                    {
                        for (j=0;j<4;j++)
                        {
                            if (board[i+9*j]!=FREE) stonesinsystem++;
                        }
                    }
                    if ((stonesinsystem % 2) == 0)
                    {
                        if (nm+nk<=12) eval++;
                        if (nm+nk<=10) eval++;
                        if (nm+nk<=8) eval+=2;
                        if (nm+nk<=6) eval+=2;
                    }
                    else
                    {
                        if (nm+nk<=12) eval--;
                        if (nm+nk<=10) eval--;
                        if (nm+nk<=8) eval-=2;
                        if (nm+nk<=6) eval-=2;
                    }
                }
            }

            if (eval != retval)
            {
                printJGResults=true;
                cout << "SIMPLEAI HEURISTIC SAYS: " << eval << endl;
                cout << nbm << ',' << nbk << ',' << nwm << ',' << nwk << endl;
                cout << nbmc << ',' << nbkc << ',' << nwmc << ',' << nwkc << endl;
                cout << nbme << ',' << nbke << ',' << nwme << ',' << nwke << endl;
                cout << tempo << endl;
                cout << stonesinsystem << endl;
            }
        }
#endif

        if (printJGResults)
        {
            cout << "JG HEURISTIC SAYS: " << retval << endl;
            cout << nbm << ',' << nbk << ',' << nwm << ',' << nwk << endl;
            cout << nbmc << ',' << nbkc << ',' << nwmc << ',' << nwkc << endl;
            cout << nbme << ',' << nbke << ',' << nwme << ',' << nwke << endl;
            cout << tempo << endl;
            cout << stonesinsystem << endl;
            printBoard(b);
            CREATE_PAUSE("");
        }
#if CHECKERS_EXPERIMENT_DEBUG
        cout << "JG HEURISTIC SAYS: " << retval << endl;
        cout << nbm << ',' << nbk << ',' << nwm << ',' << nwk << endl;
        cout << nbmc << ',' << nbkc << ',' << nwmc << ',' << nwkc << endl;
        cout << nbme << ',' << nbke << ',' << nwme << ',' << nwke << endl;
        cout << tempo << endl;
        printBoard(b);
        CREATE_PAUSE("");
#endif

        switch (handCodedType)
        {
        case 0:
            {
                //FIRST CASE: NO CHANGE
            }
            break;
        case 1:
            {
                //SECOND CASE, RIGHT BIAS
                for (int x=0;x<8;x++)
                {
                    for (int y=(x%2);y<8;y+=2)
                    {
                        if (b[x][y]&BLACK)
                        {
                            retval += 0.005*x;
                        }
                        else if (b[x][y]&WHITE)
                        {
                            retval -= 0.005*x;
                        }
                    }
                }
            }
            break;
        case 2:
            {
                //THIRD CASE, LEFT BIAS
                for (int x=0;x<8;x++)
                {
                    for (int y=(x%2);y<8;y+=2)
                    {
                        if (b[x][y]&BLACK)
                        {
                            retval += 0.005*(8-x);
                        }
                        else if (b[x][y]&WHITE)
                        {
                            retval -= 0.005*(8-x);
                        }
                    }
                }
            }
            break;
        case 3:
            {
                //FOURTH CASE, BOTTOM BIAS
                for (int x=0;x<8;x++)
                {
                    for (int y=(x%2);y<8;y+=2)
                    {
                        if (b[x][y]&BLACK)
                        {
                            retval += 0.005*(y);
                        }
                        else if (b[x][y]&WHITE)
                        {
                            retval -= 0.005*(y);
                        }
                    }
                }
            }
            break;
        case 4:
            {
                //FIFTH CASE, TOP BIAS
                for (int x=0;x<8;x++)
                {
                    for (int y=(x%2);y<8;y+=2)
                    {
                        if (b[x][y]&BLACK)
                        {
                            retval += 0.005*(8-y);
                        }
                        else if (b[x][y]&WHITE)
                        {
                            retval -= 0.005*(8-y);
                        }
                    }
                }
            }
            break;
        }

		if(dumpEvaluationImages)
		{
			handCodedEvalStream << "Evaluation #" << numHandCodedEvaluations << ":" << endl;
			printBoard(b,handCodedEvalStream);
			handCodedEvalStream << "Evaluation Score: " << retval << endl << endl << endl;
		}
		numHandCodedEvaluations++;

        return pair<CheckersNEATDatatype,int>(retval,numHandCodedEvaluations-1);
    }

    pair<CheckersNEATDatatype,int> CheckersExperiment::evaluateLeafHyperNEAT(
        uchar b[8][8]
    )
    {
        NEAT::LayeredSubstrate<CheckersNEATDatatype>* substrate = &substrates[currentSubstrateIndex];

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

            substrate->getNetwork()->reinitialize();
            substrate->getNetwork()->dummyActivation();

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
                            substrate->setValue( (Node(x,y,0)) , -0.75 );
                        }
                        else if ( (b[boardx][boardy]&MAN) )
                        {
                            substrate->setValue( (Node(x,y,0)) , -0.5 );
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
                            substrate->setValue( (Node(x,y,0)) , 0.75 );
                        }
                        else if ( (b[boardx][boardy]&MAN) )
                        {
                            substrate->setValue( (Node(x,y,0)) , 0.5 );
                        }
                        else
                        {
                            throw CREATE_LOCATEDEXCEPTION_INFO("Could not determine piece type (man/king)!");
                        }
                    }
                    else
                    {
                        //cout << "FOUND NOTHING\n";
                        substrate->setValue( (Node(x,y,0)) , 0.0 );
                    }

                }
            }

            substrate->getNetwork()->update();
            output = substrate->getValue((Node(0,0,2)));

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

		if(dumpEvaluationImages)
		{
			hyperNEATEvalStream << "Evaluation #" << numHyperNEATEvaluations << ":" << endl;
			printBoard(b,hyperNEATEvalStream,substrate->getNetwork(),nameLookup);
			hyperNEATEvalStream << "Evaluation Score: " << output << endl << endl << endl;
		}
		numHyperNEATEvaluations++;

#if DEBUG_SHOW_HYPERNEAT_ALTERNATIVES
		boardEvaluationList.push_back(BoardEvaluation(b,output));
#endif

        return pair<CheckersNEATDatatype,int>(output,numHyperNEATEvaluations-1);
    }

    class CheckersBoardMoveState
    {
    public:
        CheckersNEATDatatype parentBeta;
        int depth,maxDepth;
        CheckersBoardMoveState()
        {}
        CheckersBoardMoveState(
            CheckersNEATDatatype _parentBeta,
            int _depth,
            int _maxDepth
            )
            :
        parentBeta(_parentBeta),
            depth(_depth),
            maxDepth(_maxDepth)
        {}
    };

    pair<CheckersNEATDatatype,int> CheckersExperiment::evaluatemax(uchar b[8][8],  CheckersNEATDatatype parentBeta, int depth,int maxDepth)
    {
        if (depth==0)
        {
            totalMoveList.clear();
#if DEBUG_DUMP_BOARD_LEAF_EVALUATIONS
            cout << "Creating new outfile\n";
            if (outfile) delete outfile;
            outfile = new ofstream("BoardEvaluations.txt");
#endif

        }

        int moveListCount;

        pair<CheckersNEATDatatype,int> alpha=pair<CheckersNEATDatatype,int>(CheckersNEATDatatype(INT_MIN),-1);

        bool foundJump;
        int bestMoveSoFarIndex=-1;

		int oldMoveListSize = (int)totalMoveList.size();

#if DEBUG_DO_ITERATIVE_DEEPENING
        bool useCachedTable=false;

        CheckersBoardState bState(b,BLACK);

        if (depth==0)
        {
        }
        else
        {
            vector<BoardStatePair> &bsVector = boardStateLists[currentSubstrateIndex][BLACK][bState.getShortHash()];
            vector<BoardStatePair>::iterator stateIterator = bsVector.begin();
            vector<BoardStatePair>::iterator stateEnd = bsVector.end();

            for (;stateIterator != stateEnd;stateIterator++)
            {
                if (stateIterator->first==bState)
                {
                    //We have a match
                    int depthSearched =
                        stateIterator->second.maxDepth - stateIterator->second.depth;

                    int depthToSearch = maxDepth - depth;

                    if (depthSearched >= depthToSearch)
                    {
                        //We've been down this path before (or we are currently
                        //going down it in another branch of the tree),
                        //just return the best score
                        if (stateIterator->second.getNumMoves()==0)
                        {
                            //No moves possible, return the min score
                            return INT_MIN/2;
                        }

                        return stateIterator->second.getHighestScore();
                    }
                    else
                    {
                        //We've been down this path before, but not to this level of detail.
                        //Use the previous alpha values, but continue
                        useCachedTable=true;

                        moveListCount = stateIterator->second.getNumMoves();

                        /* FOR evaluatemin() FUNCTION!
                        int index=0;
                        for(int a=(moveListCount-1);a>=0;a--)
                        {
                        //We have to iterate backwards because they are sorted lowest
                        //to highest score
                        moveList[index] = stateIterator->second.moves.getIndexData[a];
                        index++;
                        }
                        */

                        for (int a=0;a<moveListCount;a++)
                        {
                            totalMoveList.push_back(stateIterator->second.getMove(a));
                        }

                        foundJump = stateIterator->second.foundJump;

                        //Erase this, because it will get replaced.
                        bsVector.erase(stateIterator);

                        //Break out of the loop so we do not keep searching
                        //(especially because the vector has now been invalidated with
                        //the erase)
                        break;
                    }
                }
            }
        }
#endif

#if DEBUG_DO_ITERATIVE_DEEPENING
        if (!useCachedTable)
#endif
        {
            moveListCount = generateMoveList(totalMoveList,int(totalMoveList.size()),b,BLACK,foundJump);
        }
#if DEBUG_DO_ITERATIVE_DEEPENING
        else
        {
#if CHECKERS_DEBUG
            //Run a test to cross-verify moves
            HCUBE::CheckersMove tmpMoves[128];
            bool tmpFoundJump;

            int tmpMoveListCount = generateMoveList(b,tmpMoves,BLACK,tmpFoundJump);

            if (moveListCount != tmpMoveListCount)
            {
                for (int b=0;b<moveListCount;b++)
                {
                    cout << (int)moveList[b].from.x << ',' << (int)moveList[b].from.y << " -> ";
                    cout << (int)moveList[b].to.x << ',' << (int)moveList[b].to.y << endl;
                }

                cout << "***" << endl;

                for (int a=0;a<tmpMoveListCount;a++)
                {
                    cout << (int)tmpMoves[a].from.x << ',' << (int)tmpMoves[a].from.y << " -> ";
                    cout << (int)tmpMoves[a].to.x << ',' << (int)tmpMoves[a].to.y << endl;
                }

                throw CREATE_LOCATEDEXCEPTION_INFO("FOUND A DIFFERING NUMBER OF MOVES!");
            }

            if (foundJump != tmpFoundJump)
            {
                throw CREATE_LOCATEDEXCEPTION_INFO("FOUND A DIFFERING JUMP VALUE!");
            }

            for (int a=0;a<moveListCount;a++)
            {
                for (int b=0;b<=moveListCount;b++)
                {
                    if (b==moveListCount)
                    {
                        cout << tmpMoves[a].from.x << ',' << tmpMoves[a].from.y << endl;
                        cout << tmpMoves[a].to.x << ',' << tmpMoves[a].to.y << endl;

                        cout << "***" << endl;

                        for (int b=0;b<moveListCount;b++)
                        {
                            cout << moveList[b].from.x << ',' << moveList[b].from.y << endl;
                            cout << moveList[b].to.x << ',' << moveList[b].to.y << endl;

                        }

                        throw CREATE_LOCATEDEXCEPTION_INFO("ERROR! MISSING A MOVE!");
                    }

                    if (moveList[b]==tmpMoves[a])
                    {
                        break;
                    }
                }
            }
#endif
        }
#endif

#if DEBUG_DO_ITERATIVE_DEEPENING
        //We have a movelist, let's cache it.
        CheckersBoardStateData bStateData;

        bStateData.depth = depth;
        bStateData.maxDepth = maxDepth;
        bStateData.foundJump = foundJump;

        /*
        for(int a=0;a<moveListCount;a++)
        {
        bStateData.moves.insert(CheckersNEATDatatype(INT_MIN/2.0),moveList[a]);
        }

        boardStateLists[currentSubstrateIndex][BLACK][bState.getShortHash()].push_back(
        BoardStatePair(bState,bStateData)
        );
        */
#endif

        if (!moveListCount)
        {
            //No possible moves, this is a loss!
            return pair<CheckersNEATDatatype,int>(CheckersNEATDatatype(INT_MIN/2.0),-1);
        }

        if (depth==0 && moveListCount==1)
        {
            //Forced move, don't bother doing any evaluations
            secondBestMoveToMake = moveToMake = totalMoveList.at(oldMoveListSize);
            totalMoveList.erase(totalMoveList.begin()+oldMoveListSize,totalMoveList.end());
            return pair<CheckersNEATDatatype,int>(0,-1);
        }

#if 0
        //Check for duplicate moves
        for (int a=0;a<moveListCount;a++)
        {
            for (int b=0;b<moveListCount;b++)
            {
                if (a==b)
                    continue;

                if (moveList[a]==moveList[b])
                {
                    throw CREATE_LOCATEDEXCEPTION_INFO("OOPS!");
                }
            }
        }
#endif


#if CHECKERS_EXPERIMENT_DEBUG
        printBoard(b);
        cout << "Moves for black: " << endl;
        for (int a=0;a<moveListCount;a++)
        {
            cout << "MOVE: (" << ((int)moveList[a].from.x) << ',' << ((int)moveList[a].from.y) << ") -> "
                << "(" << ((int)moveList[a].to.x) << ',' << ((int)moveList[a].to.y) << ")" << endl;
        }
        CREATE_PAUSE("Done listing moves");
#endif

        if (depth >= maxDepth && DEBUG_USE_HYPERNEAT_EVALUATION && foundJump == false)
        {
            //This is a leaf node, return the neural network's evaluation
            totalMoveList.erase(totalMoveList.begin()+oldMoveListSize,totalMoveList.end());

			pair<CheckersNEATDatatype,int> retval = evaluateLeafHyperNEAT(b);

			if(dumpEvaluationImages)
			{
				for(int a=0;a<depth;a++)
				{
					handCodedTreeStream << ">";
				}
				handCodedTreeStream << " ";
				handCodedTreeStream << "[LEAF VALUE] " << retval.first << "/" << retval.second << endl;
			}

			return retval;
        }

        pair<CheckersNEATDatatype,int> childBeta;

        if (depth==0)
        {
            secondBestMoveToMake = moveToMake = totalMoveList.at(oldMoveListSize);
            childBetaForSecondBestMove = (CheckersNEATDatatype)(INT_MIN/2.0);
        }

		if(dumpEvaluationImages)
		{
			for(int a=0;a<depth;a++)
			{
				handCodedTreeStream << ">";
			}
			handCodedTreeStream << " ";
            handCodedTreeStream << "[# MOVES] " << moveListCount << endl;
		}

        for (int a=0;a<moveListCount;a++)
        {
            CheckersMove currentMove = totalMoveList.at(oldMoveListSize+a);

            makeMove(currentMove,b);

            int winner = getWinner(b);

            if (winner==BLACK)
            {
                //CREATE_PAUSE("FOUND WIN FOR BLACK!");
                reverseMove(currentMove,b);

                if (depth==0)
                    secondBestMoveToMake = moveToMake = currentMove;

				if(dumpEvaluationImages)
				{
					for(int a=0;a<depth;a++)
					{
						handCodedTreeStream << ">";
					}
					handCodedTreeStream << " ";
					handCodedTreeStream << "[FOUND WIN] " << (INT_MAX/2) << endl;
				}

				totalMoveList.erase(totalMoveList.begin()+oldMoveListSize,totalMoveList.end());
                return pair<CheckersNEATDatatype,int>(CheckersNEATDatatype(INT_MAX/2),-1);
            }

            childBeta = evaluatemin(b,alpha.first,depth+1,maxDepth);
            reverseMove(currentMove,b);

#if DEBUG_DO_ITERATIVE_DEEPENING
            bStateData.insertMove(childBeta,currentMove);
#endif

#if CHECKERS_EXPERIMENT_DEBUG
            for (int dd=0;dd<depth;dd++)
            {
                cout << "*";
            }
            cout << childBeta << endl;
#endif

            if (childBeta.first > alpha.first)
            {
#if CHECKERS_EXPERIMENT_DEBUG
                cout << "Found new alpha\n";
#endif
                if (depth==0)
                {
                    secondBestMoveToMake = moveToMake;
                    childBetaForSecondBestMove = alpha.first;
                }

                alpha = childBeta;
                bestMoveSoFarIndex = a;
                if (depth==0)
                {
                    //This means that this is the root max, so store the best move.
#if CHECKERS_EXPERIMENT_DEBUG
                    cout << "BLACK: MOVE_TO_MAKE: (" << moveList[a].from.x << ',' << moveList[a].from.y << ") -> "
                        << "(" << moveList[a].to.x << ',' << moveList[a].to.y << ")" << endl;
                    CREATE_PAUSE("SETTING MOVE_TO_MAKE");
#endif

                    moveToMake = currentMove;
                }
                else
                {
                    if (parentBeta <= childBeta.first)
                    {
#if CHECKERS_EXPERIMENT_DEBUG
                        cout << "Parent will never choose this path\n";
                        CREATE_PAUSE("");
#endif
                        //parent will never choose this alpha

                        //because we are ending prematurely, we have to fill the rest of the cached
                        //data

#if DEBUG_DO_ITERATIVE_DEEPENING
                        for (int restOfList=(a+1);restOfList<moveListCount;restOfList++)
                        {
                            bStateData.insertMove(CheckersNEATDatatype(INT_MIN/2.0),totalMoveList.at(oldMoveListSize+restOfList));
                        }

                        if (memcmp(bState.b,b,sizeof(uchar)*8*8))
                        {
                            throw CREATE_LOCATEDEXCEPTION_INFO("BOARD STATE CHANGED SOMEHOW!");
                        }

                        boardStateLists[currentSubstrateIndex][BLACK][bState.getShortHash()].push_back(
                            BoardStatePair(bState,bStateData)
                            );
#endif

						if(dumpEvaluationImages)
						{
							for(int a=0;a<depth;a++)
							{
								handCodedTreeStream << ">";
							}
							handCodedTreeStream << " ";
							handCodedTreeStream << "PRUNED BECAUSE OF VALUE: " << parentBeta << endl;
						}

						totalMoveList.erase(totalMoveList.begin()+oldMoveListSize,totalMoveList.end());
                        return childBeta;
                    }
                }
            }
            else
            {
                if (depth==0 && childBeta.first>childBetaForSecondBestMove)
                {
                    secondBestMoveToMake = currentMove;
                    childBetaForSecondBestMove = childBeta.first;
                }
            }
        }

#if DEBUG_DO_ITERATIVE_DEEPENING
        ushort shortHash = bState.getShortHash();
        boardStateLists[currentSubstrateIndex][BLACK][shortHash].push_back(
            BoardStatePair(bState,bStateData)
            );
#endif

		if(dumpEvaluationImages)
		{
			for(int a=0;a<depth;a++)
			{
				handCodedTreeStream << ">";
			}
			handCodedTreeStream << " ";
			handCodedTreeStream << "RETURNING VALUE: " << alpha.first << "/" << alpha.second << endl;
		}

        totalMoveList.erase(totalMoveList.begin()+oldMoveListSize,totalMoveList.end());
        return alpha;
    }

    pair<CheckersNEATDatatype,int> CheckersExperiment::evaluatemin(uchar b[8][8],  CheckersNEATDatatype parentAlpha, int depth,int maxDepth)
    {
        if (depth==0)
        {
            totalMoveList.clear();
#if DEBUG_DUMP_BOARD_LEAF_EVALUATIONS
            cout << "Creating new outfile\n";
            if (outfile) delete outfile;
            outfile = new ofstream("BoardEvaluations.txt");
#endif

        }

        int moveListCount;

        pair<CheckersNEATDatatype,int> beta(CheckersNEATDatatype(INT_MAX),-1);

        bool foundJump;
        int bestMoveSoFarIndex=-1;

		int oldMoveListSize = (int)totalMoveList.size();

#if DEBUG_DO_ITERATIVE_DEEPENING
        bool useCachedTable=false;

        CheckersBoardState bState(b,WHITE);

        if (depth==0)
        {
        }
        else
        {
            vector<BoardStatePair> &bsVector = boardStateLists[currentSubstrateIndex][WHITE][bState.getShortHash()];
            vector<BoardStatePair>::iterator stateIterator = bsVector.begin();
            vector<BoardStatePair>::iterator stateEnd = bsVector.end();

            for (;stateIterator != stateEnd;stateIterator++)
            {
                if (stateIterator->first==bState)
                {
                    //We have a match
                    int depthSearched =
                        stateIterator->second.maxDepth - stateIterator->second.depth;

                    int depthToSearch = maxDepth - depth;

                    if (depthSearched >= depthToSearch)
                    {
                        //We've been down this path before (or we are currently
                        //going down it in another branch of the tree),
                        //just return the best score
                        if (stateIterator->second.getNumMoves()==0)
                        {
                            //No moves possible, return the worst score
                            return CheckersNEATDatatype(INT_MAX/2.0);
                        }

                        return stateIterator->second.getLowestScore();
                    }
                    else
                    {
                        //We've been down this path before, but not to this level of detail.
                        //Use the previous alpha values, but continue
                        useCachedTable=true;

                        moveListCount = stateIterator->second.getNumMoves();

                        /* FOR evaluatemin() FUNCTION!
                        int index=0;
                        for(int a=(moveListCount-1);a>=0;a--)
                        {
                        //We have to iterate backwards because they are sorted lowest
                        //to highest score
                        moveList[index] = stateIterator->second.moves.getIndexData[a];
                        index++;
                        }
                        */

                        for (int a=0;a<moveListCount;a++)
                        {
                            totalMoveList.push_back(stateIterator->second.getMove(a));
                        }

                        foundJump = stateIterator->second.foundJump;

                        //Erase this, because it will get replaced.
                        bsVector.erase(stateIterator);

                        //Break out of the loop so we do not keep searching
                        //(especially because the vector has now been invalidated with
                        //the erase)
                        break;
                    }
                }
            }
        }
#endif

#if DEBUG_DO_ITERATIVE_DEEPENING
        if (!useCachedTable)
#endif
        {
            moveListCount = generateMoveList(totalMoveList,int(totalMoveList.size()),b,WHITE,foundJump);
        }
#if DEBUG_DO_ITERATIVE_DEEPENING
        else
        {
#if CHECKERS_DEBUG
            //Run a test to cross-verify moves
            HCUBE::CheckersMove tmpMoves[128];
            bool tmpFoundJump;

            int tmpMoveListCount = generateMoveList(b,tmpMoves,WHITE,tmpFoundJump);

            if (moveListCount != tmpMoveListCount)
            {
                for (int b=0;b<moveListCount;b++)
                {
                    cout << (int)moveList[b].from.x << ',' << (int)moveList[b].from.y << " -> ";
                    cout << (int)moveList[b].to.x << ',' << (int)moveList[b].to.y << endl;
                }

                cout << "***" << endl;

                for (int a=0;a<tmpMoveListCount;a++)
                {
                    cout << (int)tmpMoves[a].from.x << ',' << (int)tmpMoves[a].from.y << " -> ";
                    cout << (int)tmpMoves[a].to.x << ',' << (int)tmpMoves[a].to.y << endl;
                }

                throw CREATE_LOCATEDEXCEPTION_INFO("FOUND A DIFFERING NUMBER OF MOVES!");
            }

            if (foundJump != tmpFoundJump)
            {
                throw CREATE_LOCATEDEXCEPTION_INFO("FOUND A DIFFERING JUMP VALUE!");
            }

            for (int a=0;a<moveListCount;a++)
            {
                for (int b=0;b<=moveListCount;b++)
                {
                    if (b==moveListCount)
                    {
                        cout << tmpMoves[a].from.x << ',' << tmpMoves[a].from.y << endl;
                        cout << tmpMoves[a].to.x << ',' << tmpMoves[a].to.y << endl;

                        cout << "***" << endl;

                        for (int b=0;b<moveListCount;b++)
                        {
                            cout << moveList[b].from.x << ',' << moveList[b].from.y << endl;
                            cout << moveList[b].to.x << ',' << moveList[b].to.y << endl;

                        }

                        throw CREATE_LOCATEDEXCEPTION_INFO("ERROR! MISSING A MOVE!");
                    }

                    if (moveList[b]==tmpMoves[a])
                    {
                        break;
                    }
                }
            }
#endif
        }
#endif

#if DEBUG_DO_ITERATIVE_DEEPENING
        //We have a movelist, let's cache it.
        CheckersBoardStateData bStateData;

        bStateData.depth = depth;
        bStateData.maxDepth = maxDepth;
        bStateData.foundJump = foundJump;

        /*
        for(int a=0;a<moveListCount;a++)
        {
        bStateData.moves.insert(CheckersNEATDatatype(INT_MIN/2.0),moveList[a]);
        }

        boardStateLists[currentSubstrateIndex][BLACK][bState.getShortHash()].push_back(
        BoardStatePair(bState,bStateData)
        );
        */
#endif

        if (!moveListCount)
        {
            //No possible moves, this is a loss!
            return pair<CheckersNEATDatatype,int>(CheckersNEATDatatype(INT_MAX/2.0),-1);
        }

        if (depth==0 && moveListCount==1)
        {
            //Forced move, don't bother doing any evaluations
            secondBestMoveToMake = moveToMake = totalMoveList.at(oldMoveListSize);
            totalMoveList.erase(totalMoveList.begin()+oldMoveListSize,totalMoveList.end());
            return pair<CheckersNEATDatatype,int>(0,-1);
        }

#if 0
        //Check for duplicate moves
        for (int a=0;a<moveListCount;a++)
        {
            for (int b=0;b<moveListCount;b++)
            {
                if (a==b)
                    continue;

                if (moveList[a]==moveList[b])
                {
                    throw CREATE_LOCATEDEXCEPTION_INFO("OOPS!");
                }
            }
        }
#endif

#if CHECKERS_EXPERIMENT_DEBUG
        printBoard(b);
        cout << "Moves for black: " << endl;
        for (int a=0;a<moveListCount;a++)
        {
            cout << "MOVE: (" << ((int)moveList[a].from.x) << ',' << ((int)moveList[a].from.y) << ") -> "
                << "(" << ((int)moveList[a].to.x) << ',' << ((int)moveList[a].to.y) << ")" << endl;
        }
        CREATE_PAUSE("Done listing moves");
#endif

        if (depth>=maxDepth && DEBUG_USE_HANDCODED_EVALUATION && foundJump==false)
        {
            //This is a leaf node, return the hand-coded evaluation
            totalMoveList.erase(totalMoveList.begin()+oldMoveListSize,totalMoveList.end());
            pair<CheckersNEATDatatype,int> retval = evaluateLeafWhite(b);

			if(dumpEvaluationImages)
			{
				for(int a=0;a<depth;a++)
				{
					handCodedTreeStream << ">";
				}
				handCodedTreeStream << " ";
				handCodedTreeStream << "[LEAF VALUE] " << retval.first << "/" << retval.second << endl;
			}

			return retval;
        }

        pair<CheckersNEATDatatype,int> childAlpha;

        if (depth==0)
        {
            secondBestMoveToMake = moveToMake = totalMoveList.at(oldMoveListSize);
            childAlphaForSecondBestMove = (CheckersNEATDatatype)(INT_MAX/2.0);
        }

		if(dumpEvaluationImages)
		{
			for(int a=0;a<depth;a++)
			{
				handCodedTreeStream << ">";
			}
			handCodedTreeStream << " ";
            handCodedTreeStream << "[# MOVES] " << moveListCount << endl;
		}

        for (int a=0;a<moveListCount;a++)
        {
            CheckersMove currentMove = totalMoveList.at(oldMoveListSize+a);

            makeMove(currentMove,b);

            int winner = getWinner(b);

            if (winner==WHITE)
            {
                //CREATE_PAUSE("FOUND WIN FOR WHITE!");
                reverseMove(currentMove,b);

                if (depth==0)
                    secondBestMoveToMake = moveToMake = currentMove;

				totalMoveList.erase(totalMoveList.begin()+oldMoveListSize,totalMoveList.end());

				if(dumpEvaluationImages)
				{
					for(int a=0;a<depth;a++)
					{
						handCodedTreeStream << ">";
					}
					handCodedTreeStream << " ";
					handCodedTreeStream << "[FOUND WIN] " << (INT_MIN/2) << endl;
				}

				return pair<CheckersNEATDatatype,int>(CheckersNEATDatatype(INT_MIN/2),-1);
            }

            childAlpha = evaluatemax(b,beta.first,depth+1,maxDepth);
            reverseMove(currentMove,b);

#if DEBUG_DO_ITERATIVE_DEEPENING
            bStateData.insertMove(childAlpha,currentMove);
#endif

#if CHECKERS_EXPERIMENT_DEBUG
            for (int dd=0;dd<depth;dd++)
            {
                cout << "*";
            }
            cout << childAlpha << endl;
#endif

            if (childAlpha.first < beta.first)
            {
#if CHECKERS_EXPERIMENT_DEBUG
                cout << "Found new beta\n";
#endif
                if (depth==0)
                {
                    //Set the second best move to the old best move
                    secondBestMoveToMake = moveToMake;
                    childAlphaForSecondBestMove = beta.first;
                }

                beta = childAlpha;
                bestMoveSoFarIndex = a;
                if (depth==0)
                {
                    //This means that this is the root max, so store the best move.
#if CHECKERS_EXPERIMENT_DEBUG
                    cout << "WHITE: MOVE_TO_MAKE: (" << moveList[a].from.x << ',' << moveList[a].from.y << ") -> "
                        << "(" << moveList[a].to.x << ',' << moveList[a].to.y << ")" << endl;
                    CREATE_PAUSE("SETTING MOVE_TO_MAKE");
#endif

                    moveToMake = currentMove;
                }
                else
                {
                    if (parentAlpha >= childAlpha.first)
                    {
#if CHECKERS_EXPERIMENT_DEBUG
                        cout << "Parent will never choose this path\n";
                        CREATE_PAUSE("");
#endif
                        //parent will never choose this alpha

                        //because we are ending prematurely, we have to fill the rest of the cached
                        //data

#if DEBUG_DO_ITERATIVE_DEEPENING
                        for (int restOfList=(a+1);restOfList<moveListCount;restOfList++)
                        {
                            bStateData.insertMove(CheckersNEATDatatype(INT_MAX/2.0),totalMoveList.at(oldMoveListSize+restOfList));
                        }

                        if (memcmp(bState.b,b,sizeof(uchar)*8*8))
                        {
                            throw CREATE_LOCATEDEXCEPTION_INFO("BOARD STATE CHANGED SOMEHOW!");
                        }

                        boardStateLists[currentSubstrateIndex][WHITE][bState.getShortHash()].push_back(
                            BoardStatePair(bState,bStateData)
                            );
#endif

						if(dumpEvaluationImages)
						{
							for(int a=0;a<depth;a++)
							{
								handCodedTreeStream << ">";
							}
							handCodedTreeStream << " ";
							handCodedTreeStream << "PRUNED BECAUSE OF VALUE: " << parentAlpha << endl;
						}

						totalMoveList.erase(totalMoveList.begin()+oldMoveListSize,totalMoveList.end());
                        return beta;
                    }
                }
            }
            else
            {
                if (depth==0 && childAlpha.first<childAlphaForSecondBestMove)
                {
                    secondBestMoveToMake = currentMove;
                    childAlphaForSecondBestMove = childAlpha.first;
                }
            }
        }

#if DEBUG_DO_ITERATIVE_DEEPENING
        ushort shortHash = bState.getShortHash();
        boardStateLists[currentSubstrateIndex][WHITE][shortHash].push_back(
            BoardStatePair(bState,bStateData)
            );
#endif

		if(dumpEvaluationImages)
		{
			for(int a=0;a<depth;a++)
			{
				handCodedTreeStream << ">";
			}
			handCodedTreeStream << " ";
			handCodedTreeStream << "RETURNING VALUE: " << beta.first << "/" << beta.second << endl;
		}

        totalMoveList.erase(totalMoveList.begin()+oldMoveListSize,totalMoveList.end());
        return beta;
    }

    pair<CheckersNEATDatatype,int> CheckersExperiment::firstevaluatemax(
        uchar b[8][8],
        int maxDepth,
        double timeLimit
        )
    {
#if DEBUG_DO_ITERATIVE_DEEPENING
        /*
        for(int a=0;a<65536;a++)
        {
        boardStateLists[currentSubstrateIndex][BLACK][a].clear();
        }
        for(int a=0;a<65536;a++)
        {
        boardStateLists[currentSubstrateIndex][WHITE][a].clear();
        }
        */

        int useOdd = maxDepth%2;
        CheckersNEATDatatype retval;
        timer t;
        for (int a=2-useOdd;;a+=2)
        {
            retval = evaluatemax(b,CheckersNEATDatatype(INT_MAX/2.0),0,a);

            if (a+2>maxDepth || t.elapsed()>timeLimit)
            {
                //cout << "Ran to depth " << a << " (" << t.elapsed() << " sec.)" << endl;
                return retval;
            }
        }
#else
        numHyperNEATEvaluations=0;
		if(dumpEvaluationImages)
		{
			string filename3 = string("HyperNEATEvalStream") + toString(numHyperNEATStreams) + string(".txt");
			hyperNEATEvalStream.open(filename3.c_str());
			string filename2 = string("HandCodedTreeStream") + toString(numHandCodedStreams) + string(".txt");
			hyperNEATTreeStream.open(filename2.c_str());
		}

		pair<CheckersNEATDatatype,int> retval = evaluatemax(b,CheckersNEATDatatype(INT_MAX/2),0,maxDepth);

		if(dumpEvaluationImages)
		{
            hyperNEATEvalStream.close();
            numHyperNEATStreams++;
		}

        return retval;
#endif
    }

    CheckersNEATDatatype CheckersExperiment::firstevaluatemin(
        uchar b[8][8],
        int maxDepth,
        double timeLimit
        )
    {
#if DEBUG_DO_ITERATIVE_DEEPENING
        /*
        for(int a=0;a<65536;a++)
        {
        boardStateLists[currentSubstrateIndex][BLACK][a].clear();
        }
        for(int a=0;a<65536;a++)
        {
        boardStateLists[currentSubstrateIndex][WHITE][a].clear();
        }
        */

        int useOdd = ((maxDepth)%2);
        CheckersNEATDatatype retval;
        timer t;
        for (int a=2-useOdd;;a+=2)
        {
            retval = evaluatemin(b,CheckersNEATDatatype(INT_MIN/2.0),0,a);

            if (a+2>maxDepth || t.elapsed()>timeLimit)
            {
                //cout << "Ran to depth " << a << " (" << t.elapsed() << " sec.)" << endl;
                return retval;
            }
        }
#else
		if(dumpEvaluationImages)
		{
			numHandCodedEvaluations=0;
			string filename = string("HandCodedEvalStream") + toString(numHandCodedStreams) + string(".txt");
			handCodedEvalStream.open(filename.c_str());
			string filename2 = string("HandCodedTreeStream") + toString(numHandCodedStreams) + string(".txt");
			handCodedTreeStream.open(filename2.c_str());
		}

        CheckersNEATDatatype retval = evaluatemin(b,INT_MIN/2,0,maxDepth).first;

		if(dumpEvaluationImages)
		{
			handCodedEvalStream.close();
			handCodedTreeStream.close();
			numHandCodedStreams++;
		}

		return retval;
#endif
    }

    void CheckersExperiment::makeMoveCliche(uchar b[8][8],int colorToMove,int* retval)
	{
        int otherColor=BLACK;
        if(colorToMove==BLACK)
            otherColor=WHITE;
        //progress_timer t;
        DEBUG_USE_HANDCODED_EVALUATION = 1;
        DEBUG_USE_HYPERNEAT_EVALUATION = 0;
        currentSubstrateIndex=handCodedAISubstrateIndex;

	    boost::mutex::scoped_lock lock(cakeMutex);

        cout << "CLICHE BEFORE\n";
	    printBoard(b);

        srand(cakeRandomSeed);
	    //Play with cake
	    char output[255];
        memset(output,0,sizeof(char)*255);
	    int playnow=0;
        int bint[8][8];
        for(int r=0;r<8;r++) for(int c=0;c<8;c++) bint[r][c] = b[r][c];
        CBmove cbmove;
        CLICHE_MAXDEPTH = 4;//BASE_EVOLUTION_SEARCH_DEPTH;
        if(currentRound>=100)
        {
            CLICHE_MAXDEPTH = 8;
            if(currentRound>=150)
            {
                CLICHE_MAXDEPTH = 10;
            }
        }
        int clicheReturn = getmove(bint,colorToMove,1000000.0,output,&playnow,0,0,&cbmove);
        for(int r=0;r<8;r++) 
        {
            for(int c=0;c<8;c++) 
            {
                if(r==0 && (c==1||c==3))
                {
                    continue;
                }
                b[r][c] = bint[r][c];
            }
        }

        cout << "CLICHE AFTER\n";
	    printBoard(b);
	    //cout << "cliche Value: " << cakeReturn << " Output: " << output << endl;
	    //printBoard(b);
        //CREATE_PAUSE("");

	    //firstevaluatemin(b,BASE_EVOLUTION_SEARCH_DEPTH);
        //cout << "SimpleCheckers time: ";

        if(clicheReturn == LOSS)
        {
            //No moves to make
            *retval = otherColor;
        }
        else if(clicheReturn == WIN)
        {
            //No moves to make
            *retval = colorToMove;
        }
	    else
	    {
            *retval = -1;
    #if CHECKERS_EXPERIMENT_LOG_EVALUATIONS
		    memcpy(gameLog[moves],b,sizeof(uchar)*8*8);
		    if(cbmove.ismove)
		    {
			    sprintf((char*)moveLog[moves],"%dx%d",gridToIndex(cbmove.from.x,cbmove.from.y),gridToIndex(cbmove.to.x,cbmove.to.y));
		    }
		    else
		    {
			    sprintf((char*)moveLog[moves],"%d-%d",gridToIndex(cbmove.from.x,cbmove.from.y),gridToIndex(cbmove.to.x,cbmove.to.y));
		    }
		    moves++;
    #endif
	    }

        //Override what cliche/cake says
        *retval = getWinner(b,colorToMove);
    }

    void CheckersExperiment::makeMoveCake(uchar b[8][8],int colorToMove,int* retval)
	{
        int otherColor=BLACK;
        if(colorToMove==BLACK)
            otherColor=WHITE;
		//progress_timer t;
		DEBUG_USE_HANDCODED_EVALUATION = 1;
		DEBUG_USE_HYPERNEAT_EVALUATION = 0;
		currentSubstrateIndex=handCodedAISubstrateIndex;


		boost::mutex::scoped_lock lock(cakeMutex);
		srand(cakeRandomSeed);

        //cout << "CAKE BEFORE\n";
		//printBoard(b);

        //Play with cake
		POSITION position;
        HCUBE::ucharboardtobitboard(b,&position);
        position.color = colorToMove;
		char output[255];
		memset(output,0,sizeof(char)*255);
		int playnow=0;
        int depthToSearch = 2;//BASE_EVOLUTION_SEARCH_DEPTH;
        if(currentRound>=100)
        {
            depthToSearch = 8;
            if(currentRound>=150)
            {
                depthToSearch = 10;
            }
        }
		int cakeReturn = cake_getmove(&searchInfo,&position,DEPTH_BASED,10000000.0,depthToSearch,1000000,output,&playnow,0,1);

		//cout << "Cake Value: " << cakeReturn << " Output: " << output << endl;
		//cout << "Move: " << moveNotation << endl;
		HCUBE::ucharbitboardtoboard(position,b);
		//printBoard(b);
		//CREATE_PAUSE("");
        //cout << "CAKE AFTER\n";
		//printBoard(b);

		//firstevaluatemin(b,BASE_EVOLUTION_SEARCH_DEPTH);
		//cout << "SimpleCheckers time: ";

        //Override what cliche/cake says
        *retval = getWinner(b,colorToMove);

        if(*retval == -1)
        {
#if CHECKERS_EXPERIMENT_LOG_EVALUATIONS
			memcpy(gameLog[moves],b,sizeof(uchar)*8*8);
			//memcpy(moveLog[moves],moveNotation,sizeof(uchar)*64);
			moves++;
#endif
        }
	}

    void CheckersExperiment::processGroup(shared_ptr<NEAT::GeneticGeneration> generation)
    {
        //cout << "Processing evaluation...\n";
        shared_ptr<NEAT::GeneticIndividual> individual = group.front();
        //You get 10 points just for entering the game, wahooo!
        individual->setFitness(10);
		numHandCodedStreams=0;
        numHyperNEATStreams = 0;

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

        uchar b[8][8];

        //cout << "Playing games with HyperNEAT as black\n";
        for (handCodedType=0;handCodedType<HANDCODED_PLAYER_TESTS;handCodedType++)
        {
			cakeRandomSeed = 1000 + handCodedType;
            if(searchInfo.repcheck)
		        free(searchInfo.repcheck);
		    resetsearchinfo(&searchInfo);
		    // allocate memory for repcheck array
		    searchInfo.repcheck = (REPETITION*)malloc((MAXDEPTH+HISTORYOFFSET) * sizeof(REPETITION));

            resetBoard(b);

            int retval=-1;
			moves=0;

            double inGameFitness=0.0;

            for (currentRound=0;currentRound<CHECKERS_MAX_ROUNDS&&retval==-1;currentRound++)
            {
#if 1
				//Clear the evaluation cache
				for (int a=0;a<65536;a++)
				{
					boardEvaluationCaches[0][a].clear();
					boardEvaluationCaches[0][a].reserve(0);
				}
				//Clear the state list caches
				for (int a=0;a<65536;a++)
				{
					boardStateLists[0][BLACK][a].clear();
					boardStateLists[0][BLACK][a].reserve(0);
				}
				for (int a=0;a<65536;a++)
				{
					boardStateLists[0][WHITE][a].clear();
					boardStateLists[0][WHITE][a].reserve(0);
				}

				//Clear the evaluation cache
				for (int a=0;a<65536;a++)
				{
					boardEvaluationCaches[1][a].clear();
					boardEvaluationCaches[1][a].reserve(0);
				}
				//Clear the state list caches
				for (int a=0;a<65536;a++)
				{
					boardStateLists[1][BLACK][a].clear();
					boardStateLists[1][BLACK][a].reserve(0);
				}
				for (int a=0;a<65536;a++)
				{
					boardStateLists[1][WHITE][a].clear();
					boardStateLists[1][WHITE][a].reserve(0);
				}

                //cout << "Round: " << currentRound << endl;
                moveToMake = CheckersMove();
                secondBestMoveToMake = CheckersMove();
                DEBUG_USE_HANDCODED_EVALUATION = 0;
                DEBUG_USE_HYPERNEAT_EVALUATION = 1;
                currentSubstrateIndex=0;
                firstevaluatemax(b,/*BASE_EVOLUTION_SEARCH_DEPTH+NEAT_SEARCH_HANDICAP+handCodedType*2*/8);

#if CHECKERS_PRINT_ALTERNATE_MOVES
                printBoard(b);
                cout << "EVAL MAX:\n";
                cout << "MOVE TO MAKE: "
                    << (int)moveToMake.from.x << ','
                    << (int)moveToMake.from.y
                    << " -> " << (int)moveToMake.getFinalDestination().x << ','
                    << (int)moveToMake.getFinalDestination().y << endl;
                cout << "ALTERNATE MOVE TO MAKE: "
                    << (int)secondBestMoveToMake.from.x << ','
                    << (int)secondBestMoveToMake.from.y
                    << " -> " << (int)secondBestMoveToMake.getFinalDestination().x << ','
                    << (int)secondBestMoveToMake.getFinalDestination().y << endl;
                CREATE_PAUSE("");
#endif

#if CHECKERS_EXPERIMENT_DEBUG
                cout << "BLACK MAKING MOVE\n";

                printBoard(b);
#endif

                if (moveToMake.from.x==255)
                {
                    //black loses
                    retval = WHITE;
                }
                else
                {
                    makeMove(moveToMake,b);
                    retval = getWinner(b,WHITE);

#if CHECKERS_EXPERIMENT_LOG_EVALUATIONS
					memcpy(gameLog[moves],b,sizeof(uchar)*8*8);
					if(moveToMake.pieceCaptured)
					{
						sprintf((char*)moveLog[moves],"%dx%d",gridToIndex(moveToMake.from),gridToIndex(moveToMake.getFinalDestination()));
					}
					else
					{
						sprintf((char*)moveLog[moves],"%d-%d",gridToIndex(moveToMake.from),gridToIndex(moveToMake.getFinalDestination()));
					}
					moves++;
#endif
                }

#if CHECKERS_EXPERIMENT_DEBUG
                printBoard(b);
                CREATE_PAUSE("");
#endif
#endif
                if(retval==-1)
                {
                    //makeMoveCliche(b,BLACK,&retval);
                }

                if (retval==-1)
                {
                    //printBoard(b);

                    moveToMake = CheckersMove();
                    secondBestMoveToMake = CheckersMove();

#if 0
	for(int r=0;r<8;r++) for(int c=0;c<8;c++) if((r+c)%2==0) b[r][c] = 0;
	b[0][6] = b[2][2] = b[3][1] = b[5][7] = WHITE | MAN;
	b[3][3] = WHITE|KING;
	b[4][6] = b[3][5] = b[6][2] = BLACK|MAN;
#endif

#if DEBUG_PLAY_CAKE_INSTEAD_OF_CLICHE == 0
                    makeMoveCliche(b,WHITE,&retval);
                    /*
                    //CLICHE ENGINE
                    {
                        //progress_timer t;
                        DEBUG_USE_HANDCODED_EVALUATION = 1;
                        DEBUG_USE_HYPERNEAT_EVALUATION = 0;
                        currentSubstrateIndex=handCodedAISubstrateIndex;

						boost::mutex::scoped_lock lock(cakeMutex);
						//printBoard(b);
						//Play with cake
						char output[255];
                        memset(output,0,sizeof(char)*255);
						int playnow=0;
                        int bint[8][8];
                        for(int r=0;r<8;r++) for(int c=0;c<8;c++) bint[r][c] = b[r][c];
                        CBmove cbmove;
                        MAXDEPTH = BASE_EVOLUTION_SEARCH_DEPTH;
                        int cakeReturn = getmove(bint,WHITE,1000000.0,output,&playnow,0,0,&cbmove);
                        for(int r=0;r<8;r++) for(int c=0;c<8;c++) b[r][c] = bint[r][c];

						//cout << "cliche Value: " << cakeReturn << " Output: " << output << endl;
						//printBoard(b);
                        //CREATE_PAUSE("");

						//firstevaluatemin(b,BASE_EVOLUTION_SEARCH_DEPTH);
                        //cout << "SimpleCheckers time: ";

                        if(cakeReturn == LOSS)
                        {
                            //No moves to make
                            retval = BLACK;
                        }
						else
						{
#if CHECKERS_EXPERIMENT_LOG_EVALUATIONS
							memcpy(gameLog[moves],b,sizeof(uchar)*8*8);
							if(cbmove.ismove)
							{
								sprintf((char*)moveLog[moves],"%dx%d",gridToIndex(cbmove.from.x,cbmove.from.y),gridToIndex(cbmove.to.x,cbmove.to.y));
							}
							else
							{
								sprintf((char*)moveLog[moves],"%d-%d",gridToIndex(cbmove.from.x,cbmove.from.y),gridToIndex(cbmove.to.x,cbmove.to.y));
							}
							moves++;
#endif
						}
					}
                    */

#else
                    //CAKE ENGINE
					makeMoveCake(b,WHITE,&retval);
					//makeMoveCliche(b,WHITE,&retval);
#endif

#if CHECKERS_PRINT_ALTERNATE_MOVES
                    printBoard(b);
                    cout << "EVAL MIN:\n";
                    cout << "MOVE TO MAKE: "
                        << (int)moveToMake.from.x << ','
                        << (int)moveToMake.from.y
                        << " -> " << (int)moveToMake.getFinalDestination().x << ','
                        << (int)moveToMake.getFinalDestination().y << endl;
                    cout << "ALTERNATE MOVE TO MAKE: "
                        << (int)secondBestMoveToMake.from.x << ','
                        << (int)secondBestMoveToMake.from.y
                        << " -> " << (int)secondBestMoveToMake.getFinalDestination().x << ','
                        << (int)secondBestMoveToMake.getFinalDestination().y << endl;
                    CREATE_PAUSE("");
#endif

#if CHECKERS_EXPERIMENT_DEBUG
                    cout << "WHITE MAKING MOVE\n";

                    printBoard(b);
#endif

#if 0 //Doesn't apply to cake
                    if (moveToMake.from.x==255)
                    {
                        //white loses
                        cout << "WHITE LOSES BECAUSE THERE'S NO MOVES LEFT!\n";
                        retval = BLACK;
                        //printBoard(b);
                        CREATE_PAUSE("");
                    }
                    else
                    {
                        if (
                            chanceToMakeSecondBestMove > 0.01 &&
                            (
                            NEAT::Globals::getSingleton()->getRandom().getRandomDouble() <
                            chanceToMakeSecondBestMove
                            )
                            )
                        {
                            if (secondBestMoveToMake.from.x==255)
                            {
                                throw CREATE_LOCATEDEXCEPTION_INFO("THIS SHOULDN'T HAPPEN!");
                            }

#if CHECKERS_PRINT_ALTERNATE_MOVES
                            cout << "MADE SECOND BEST MOVE!!!!\n";
#endif

                            makeMove(secondBestMoveToMake,b);
                        }
                        else
                        {
                            makeMove(moveToMake,b);
                        }

                        retval = getWinner(b,BLACK);
                    }
#endif

#if CHECKERS_EXPERIMENT_DEBUG
                    printBoard(b);
                    CREATE_PAUSE("");
#endif
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
                cout << "WE WON!\n";
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

                if (retval==-1)
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

                //Reward losing.  This discourages ties
                individual->reward(10000);
            }
        }

        //CREATE_PAUSE("");

        if (PLAY_BOTH_SIDES)
        {
            //Now, let's do some trials where HyperNEAT evaluates for white
            //cout << "Playing games with HyperNEAT as white\n";
            for (handCodedType=0;handCodedType<HANDCODED_PLAYER_TESTS;handCodedType++)
            {

                resetBoard(b);

                int retval=-1;
                int rounds=0;

                for (rounds=0;rounds<CHECKERS_MAX_ROUNDS&&retval==-1;rounds++)
                {
                    //cout << "Round: " << rounds << endl;
                    moveToMake = CheckersMove();
                    secondBestMoveToMake = CheckersMove();
                    DEBUG_USE_HANDCODED_EVALUATION = 1;
                    DEBUG_USE_HYPERNEAT_EVALUATION = 0;
                    currentSubstrateIndex=handCodedAISubstrateIndex;
                    firstevaluatemax(b,BASE_EVOLUTION_SEARCH_DEPTH+1);

#if CHECKERS_EXPERIMENT_DEBUG
                    cout << "BLACK MAKING MOVE\n";

                    printBoard(b);
#endif

                    if (moveToMake.from.x==255)
                    {
                        //white loses
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

#if CHECKERS_EXPERIMENT_DEBUG
                    printBoard(b);
                    CREATE_PAUSE("");
#endif

                    if (retval==-1)
                    {
                        //printBoard(b);

                        moveToMake = CheckersMove();
                        secondBestMoveToMake = CheckersMove();
                        {
                            //progress_timer t;
                            DEBUG_USE_HANDCODED_EVALUATION = 0;
                            DEBUG_USE_HYPERNEAT_EVALUATION = 1;
                            currentSubstrateIndex=0;
                            firstevaluatemin(b,BASE_EVOLUTION_SEARCH_DEPTH+1+NEAT_SEARCH_HANDICAP);
                            //cout << "SimpleCheckers time: ";
                        }

                        /*
                        cout << "JGAI SAYS TO MAKE MOVE FROM ("
                        << moveToMake.from.x << ','
                        << moveToMake.from.y << ") -> (" << moveToMake.to.x
                        << ',' << moveToMake.to.y << ")\n";

                        char str[255];
                        int playnow=0;
                        int fromx,fromy,tox,toy;
                        uchar tmpBoard[8][8];
                        memcpy(tmpBoard,b,sizeof(uchar)*8*8);
                        getmove(tmpBoard,WHITE,1000.0,str,&playnow,0,0,fromx,fromy,tox,toy);

                        cout << "AI SAYS TO MAKE MOVE FROM (" << fromx << ','
                        << fromy << ") -> (" << tox << ',' << toy << ")\n";

                        printBoard(b);
                        CREATE_PAUSE("");
                        */

#if CHECKERS_EXPERIMENT_DEBUG
                        cout << "WHITE MAKING MOVE\n";

                        printBoard(b);
#endif

                        if (moveToMake.from.x==255)
                        {
                            //white loses
                            cout << "WHITE LOSES BECAUSE THERE'S NO MOVES LEFT!\n";
                            retval = BLACK;
                            printBoard(b);
                            CREATE_PAUSE("");
                        }
                        else
                        {
                            makeMove(moveToMake,b);
                            retval = getWinner(b,BLACK);
                        }

#if CHECKERS_EXPERIMENT_DEBUG
                        printBoard(b);
                        CREATE_PAUSE("");
#endif
                    }

#if CHECKERS_EXPERIMENT_LOG_EVALUATIONS
                    memcpy(gameLog[rounds*2+1],b,sizeof(uchar)*8*8);
#endif

                    int whiteMen,blackMen,whiteKings,blackKings;

                    //countPieces(gi.board,whiteMen,blackMen,whiteKings,blackKings);
                    countPieces(b,whiteMen,blackMen,whiteKings,blackKings);

                    //Reward for # of pieces at every turn.
                    individual->reward(2 * (12-whiteMen) );
                    individual->reward(2 * (blackMen) );

                    individual->reward(3 * (12-whiteKings) );
                    individual->reward(3 * (blackKings) );
                }

                if (retval==WHITE)
                {
                    //#if CHECKERS_EXPERIMENT_DEBUG
                    cout << "WE WON!\n";
#if CHECKERS_EXPERIMENT_LOG_EVALUATIONS
                    for (int a=0;a<rounds;a++)
                    {
                        printBoard(gameLog[a*2]);

                        //if (a+1<=rounds)
                        {
                            //White made the final move because white won
                            printBoard(gameLog[a*2+1]);
                        }
                    }
#endif
                    //CREATE_PAUSE("");
                    //#endif

                    individual->reward(40000);

                    //You get bonus fitness for every round you don't have to play
                    //if you win
                    int roundsLeftOver = CHECKERS_MAX_ROUNDS-rounds;
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

                    //Reward losing.  This discourages ties
                    individual->reward(10000);
                }

                int whiteMen,blackMen,whiteKings,blackKings;

                //countPieces(gi.board,whiteMen,blackMen,whiteKings,blackKings);
                countPieces(b,whiteMen,blackMen,whiteKings,blackKings);

                individual->reward(2 * (12-blackMen) );
                individual->reward(2 * (whiteMen) );

                individual->reward(3 * (12-blackKings) );
                individual->reward(3 * (whiteKings) );
            }
        }
    }

    void CheckersExperiment::processIndividualPostHoc(shared_ptr<NEAT::GeneticIndividual> individual)
    {
#if DO_REGULAR_RUN_FOR_POSTHOC
        //cout << "INDIVIDUAL FITNESS BEFORE: " << individual->getFitness() << endl;
        clearGroup();
        addIndividualToGroup(individual);
        individual->setUserData(CheckersStats().toString());

        chanceToMakeSecondBestMove=0.0;
		//dumpEvaluationImages = true;
        shared_ptr<GeneticGeneration> dummy;
        processGroup(dummy);
		//dumpEvaluationImages = false;
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
    }

#ifndef HCUBE_NOGUI
    void CheckersExperiment::createIndividualImage(wxDC &drawContext,shared_ptr<NEAT::GeneticIndividual> individual)
    {
        currentSubstrateIndex=0;

        if (lastIndividualSeen!=individual)
        {
            //cout << "Repopulating substrate\n";
            populateSubstrate(individual);
            lastIndividualSeen = individual;
        }

        drawContext.Clear();

        for (int x=0;x<8;x++)
        {
            for (int y=(x%2);y<8;y+=2)
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

                if (userEvaluationBoard[x][y]&FREE)
                {}
                else if (userEvaluationBoard[x][y]&BLACK)
                {
                    if (userEvaluationBoard[x][y]&MAN)
                    {
                        drawContext.DrawText(_T("b"),20+drawX*20,20+drawY*20);
                    }
                    else
                    {
                        drawContext.DrawText(_T("B"),20+drawX*20,20+drawY*20);
                    }
                }
                else if (userEvaluationBoard[x][y]&WHITE)
                {
                    if (userEvaluationBoard[x][y]&MAN)
                    {
                        drawContext.DrawText(_T("w"),20+drawX*20,20+drawY*20);
                    }
                    else
                    {
                        drawContext.DrawText(_T("W"),20+drawX*20,20+drawY*20);
                    }
                }
                else
                {
                    drawContext.DrawText(_T("-"),20+drawX*20,20+drawY*20);
                }
            }
        }

        int playerToMove;
        if (userEvaluationRound%2==0)
        {
            playerToMove = BLACK;
        }
        else
        {
            playerToMove = WHITE;
        }

        int winner = getWinner(userEvaluationBoard,playerToMove);

        if (winner==WHITE)
        {
            drawContext.DrawText(_T("WHITE WINS"),30,300);
        }
        else if (winner==BLACK)
        {
            drawContext.DrawText(_T("BLACK WINS"),30,300);
        }
        else
        {
            if (userEvaluationRound%2==0)
            {
                drawContext.DrawText(_T("Black's turn.  Click to see black's move"),30,300);
            }
            else if (from.x!=255)
            {
                drawContext.DrawText(_T("Move to?"),30,300);
            }
            else
            {
                drawContext.DrawText(_T("Move from?"),30,300);
            }
        }
    }

    bool CheckersExperiment::handleMousePress(wxMouseEvent& event,wxSize &bitmapSize)
    {
        int playerToMove;
        if (userEvaluationRound%2==0)
        {
            playerToMove = BLACK;
        }
        else
        {
            playerToMove = WHITE;
        }

        int winner = getWinner(userEvaluationBoard,playerToMove);

        if (winner==WHITE||winner==BLACK)
        {
            resetBoard(userEvaluationBoard);
            return true;
        }

        if (userEvaluationRound%2==0)
        {
            //HyperNEAT makes his move
            {
                moveToMake = CheckersMove();
                secondBestMoveToMake = CheckersMove();
                cout << __LINE__ << " CALLING EVALUATEMAX\n";
                DEBUG_USE_HANDCODED_EVALUATION = 0;
                DEBUG_USE_HYPERNEAT_EVALUATION = 1;
                currentSubstrateIndex=0;

#if DEBUG_SHOW_HYPERNEAT_ALTERNATIVES
				boardEvaluationList.clear();
#endif

				pair<CheckersNEATDatatype,int> retval = firstevaluatemax(userEvaluationBoard,BASE_EVOLUTION_SEARCH_DEPTH);

#if DEBUG_SHOW_HYPERNEAT_ALTERNATIVES
                if(retval.second>=0)
                {
                    int whiteMen;
                    int blackMen;
                    int whiteKings;
                    int blackKings;

                    countPieces(boardEvaluationList[retval.second].board,whiteMen,blackMen,whiteKings,blackKings);

                    for(int a=0;a<(int)boardEvaluationList.size();a++)
                    {
                        for(int b=1;b<(int)boardEvaluationList.size();b++)
                        {
                            if(boardEvaluationList[b-1].value>boardEvaluationList[b].value)
                            {
                                swap(boardEvaluationList[b-1],boardEvaluationList[b]);
                                if(retval.second==b-1)
                                {
                                    retval.second=b;
                                }
                                else if(retval.second==b)
                                {
                                    retval.second=b-1;
                                }
                            }
                        }
                    }

                    for(int a=0;a<(int)boardEvaluationList.size();a++)
                    {
                        int tmpwhiteMen;
                        int tmpblackMen;
                        int tmpwhiteKings;
                        int tmpblackKings;

                        countPieces(boardEvaluationList[a].board,tmpwhiteMen,tmpblackMen,tmpwhiteKings,tmpblackKings);

                        if(
                            tmpwhiteMen != whiteMen ||
                            tmpblackMen != blackMen ||
                            tmpwhiteKings  != whiteKings ||
                            tmpblackKings  != blackKings
                            )
                        {
                            /*
                            boardEvaluationList.erase(boardEvaluationList.begin()+a);
                            if(a<retval.second)
                            {
                            retval.second--;
                            }
                            a--;
                            */
                        }
                        else if(a==retval.second)
                        {
                            cout << "EVALUATION CHOSEN: " << a << " RATING: " << boardEvaluationList[a].value << " == " << boardEvaluationList[retval.second].value << endl;
                            printBoard(boardEvaluationList[a].board);
                        }
                        else
                        {
                            cout << "ALTERNATE EVALUATION: " << a << " RATING: " << boardEvaluationList[a].value << endl;
                            printBoard(boardEvaluationList[a].board);
                        }
                    }
                }
#endif
			}

            if (moveToMake.from.x==255)
            {
                //black loses
                cout << "BLACK LOSES!\n";
            }
            else
            {
                makeMove(moveToMake,userEvaluationBoard);
                userEvaluationRound++;
            }

        }
        else
        {
#if CHECKERS_EXPERIMENT_INTERACTIVE_PLAY
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

            if (from.x!=255)
            {
                bool foundJump;
                totalMoveList.clear();
                int numMoves = generateMoveList(
                    totalMoveList,
                    0,
                    userEvaluationBoard,
                    WHITE,
                    foundJump
                    );

                for (int a=0;a<numMoves;a++)
                {
                    if (
                        totalMoveList[a].from==from &&
                        totalMoveList[a].getFinalDestination()==boardPosition
                        )
                    {
                        //Move was a valid move, make it happen
                        makeMove(totalMoveList[a],userEvaluationBoard);
                        userEvaluationRound++;
                    }
                }

                from = Vector2<uchar>(255,255);
            }
            else
            {
                from = boardPosition;
                cout << " Added from " << (int)from.x << ',' << (int)from.y << endl;
            }
#else

#if 0
            //Heuristic makes his move
            {
                moveToMake = CheckersMove();
                secondBestMoveToMake = CheckersMove();
                cout << __LINE__ << " CALLING EVALUATEMIN\n";
                DEBUG_USE_HANDCODED_EVALUATION = 1;
                DEBUG_USE_HYPERNEAT_EVALUATION = 0;
                firstevaluatemin(userEvaluationBoard,BASE_EVOLUTION_SEARCH_DEPTH);

#if 0
                if(
                    NEAT::Globals::getSingleton()->getRandom().getRandomDouble() <
                    0.10*max(1,10-userEvaluationRound)
                    )
                {
                    cout << __FILE__ << ":" << __LINE__ << ": PICKING SECOND BEST MOVE!\n";
                    moveToMake = secondBestMoveToMake;
                }
#endif
            }

            if (moveToMake.from.x==255)
            {
                //white loses
                cout << "WHITE LOSES!\n";
            }
            else
            {
                makeMove(moveToMake,userEvaluationBoard);
                userEvaluationRound++;
            }
#endif

			//Cake makes his move
			{
				int retval=-1;
				makeMoveCake(userEvaluationBoard,WHITE,&retval);

				if(retval==BLACK)
				{
					//white loses
					cout << "WHITE LOSES!\n";
				}
                else if(retval==WHITE)
                {
                    //white wins
                    cout << "WHITE WINS\n";
                }
				else
				{
					userEvaluationRound++;
				}
			}
#endif
        }

        return true;
    }
#endif

    Experiment* CheckersExperiment::clone()
    {
        CheckersExperiment* experiment = new CheckersExperiment(*this);

        return experiment;
    }

    void CheckersExperiment::resetGenerationData(shared_ptr<NEAT::GeneticGeneration> generation)
    {
        //generation->setUserData(new TicTacToeStats());
    }

    void CheckersExperiment::addGenerationData(shared_ptr<NEAT::GeneticGeneration> generation,shared_ptr<NEAT::GeneticIndividual> individual)
    {
        //TicTacToeStats* ticTacToeStats = (TicTacToeStats*)generation->getUserData();

        //if (individual->getUserData())
        //(*ticTacToeStats) += *((TicTacToeStats*)individual->getUserData());
    }
}
