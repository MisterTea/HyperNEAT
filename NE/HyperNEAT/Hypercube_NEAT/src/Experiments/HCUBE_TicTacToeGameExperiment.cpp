#include "HCUBE_Defines.h"

#include "Experiments/HCUBE_TicTacToeGameExperiment.h"

#include "Experiments/HCUBE_TicTacToeShared.h"

#define TIC_TAC_TOE_GAME_EXPERIMENT_DEBUG (0)

extern int SMART_ATTACKER;
extern int SMART_DEFENDER;

namespace HCUBE
{
    using namespace NEAT;

    TicTacToeGameExperiment::TicTacToeGameExperiment(string _experimentName,int _threadID)
            :
            Experiment(_experimentName,_threadID),
            numNodesX(9),
            numNodesY(9)
    {
        generateSubstrate();
    }

    GeneticPopulation* TicTacToeGameExperiment::createInitialPopulation(int populationSize)
    {
        GeneticPopulation *population = new GeneticPopulation();
        vector<GeneticNodeGene> genes;

        genes.push_back(GeneticNodeGene("Bias","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("X1","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("Y1","NetworkSensor",0,false));
        //genes.push_back(GeneticNodeGene("X2","NetworkSensor",0,false));
        //genes.push_back(GeneticNodeGene("Y2","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("DeltaX","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("DeltaY","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("Output_ab","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
        genes.push_back(GeneticNodeGene("Output_bc","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
        genes.push_back(GeneticNodeGene("Bias_b","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
        genes.push_back(GeneticNodeGene("Bias_c","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));


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

    void TicTacToeGameExperiment::generateSubstrate()
    {
        cout << "Generating substrate...";
        boost::object_pool<NEAT::NetworkNode> networkNodePool;
        boost::object_pool<NEAT::NetworkLink> networkLinkPool;

        NEAT::NetworkNode *nodes = NULL;
        NEAT::NetworkLink *links = NULL;
        double *nodeBiases = NULL;

        nodes = (NEAT::NetworkNode*)malloc(sizeof(NEAT::NetworkNode)*numNodesX*numNodesY*3);
        links = (NEAT::NetworkLink*)malloc(sizeof(NEAT::NetworkLink)*numNodesX*numNodesY*numNodesX*numNodesY*2);
        nodeBiases = new double[numNodesX*numNodesY*3];

        nameLookup.clear();

        cout << "Creating nodes...\n";

        int nodeCounter=0;

        for (int y1=0;y1<numNodesY;y1++)
        {
            for (int x1=0;x1<numNodesX;x1++)
            {
                for (int z=0;z<3;z++)
                {
                    int xmod = x1-numNodesX/2;
                    int ymod = y1-numNodesY/2;
                    Node node(xmod,ymod,z);
                    //cout << (y1-numNodesY/2) << '/' << (x1-numNodesX/2) << endl;
                    nameLookup[node] = (toString(xmod)+string("/")+toString(ymod) + string("/") + toString(z));
                    new(&nodes[nodeCounter]) NetworkNode(nameLookup[node]);
                    nodeBiases[nodeCounter]=0.0;
                    nodeCounter++;

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

        cout << "Creating links...\n";

        int linkCounter=0;

        for (int y1=0;y1<numNodesY;y1++)
        {
            for (int x1=0;x1<numNodesX;x1++)
            {
                for (int y2=0;y2<numNodesY;y2++)
                {
                    for (int x2=0;x2<numNodesX;x2++)
                    {
                        for (int z1=0;z1<2;z1++)
                        {
                            int z2 = z1+1;
                            //for (int z2=z1+1;z2<=z1+1;z2++)
                            {
                                try
                                {
                                    new (&links[linkCounter]) NetworkLink(
                                        &nodes[y1*numNodesX*3 + x1*3 + z1],
                                        &nodes[y2*numNodesX*3 + x2*3 + z2],
                                        0.0
                                    );

                                    linkCounter++;
                                }
                                catch (const char *c)
                                {
                                    cout << c << endl;
                                    CREATE_PAUSE(string("Error!: ")+toString(__LINE__));
                                }
                                catch (string s)
                                {
                                    cout << s << endl;
                                    CREATE_PAUSE(string("Error!: ")+toString(__LINE__));
                                }
                                catch(const std::exception &e)
                                {
                                    cout << e.what() << endl;
                                    CREATE_PAUSE(string("Error!: ")+toString(__LINE__));
                                }
                                catch (...)
                                {
                                    cout << "AN EXCEPTION HAS OCCURED!\n";
                                    CREATE_PAUSE(string("Error!: ")+toString(__LINE__));
                                }
                            }
                        }
                    }
                }
            }
        }

        cout << "Creating FastNetwork\n";

        substrate = NEAT::FastNetwork<double>(
                        nodes,
                        nodeCounter,
                        links,
                        linkCounter
                    );

        /*
        while (!nodes.empty())
        {
        delete nodes[0];
        nodes.erase(nodes.begin());
        }

        while (!links.empty())
        {
        delete links[0];
        links.erase(links.begin());
        }
        */
        cout << "done!\n";

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
        delete[] nodeBiases;
    }

    void TicTacToeGameExperiment::populateSubstrate(shared_ptr<NEAT::GeneticIndividual> individual)
    {
        NEAT::FastNetwork<double> network = individual->spawnFastPhenotypeStack<double>();

        int linkCounter=0;
        for (int y1=0;y1<numNodesY;y1++)
        {
            for (int x1=0;x1<numNodesX;x1++)
            {
                for (int y2=0;y2<numNodesY;y2++)
                {
                    for (int x2=0;x2<numNodesX;x2++)
                    {
                        double x1normal = (x1-numNodesX/2)/double((numNodesX-1)/2);
                        double y1normal = (y1-numNodesY/2)/double((numNodesY-1)/2);

                        double x2normal = (x2-numNodesX/2)/double((numNodesX-1)/2);
                        double y2normal = (y2-numNodesY/2)/double((numNodesY-1)/2);

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
                        network.setValue("Bias",0.3);
                        network.update();

                        double outputAB = network.getValue("Output_ab");
                        double outputBC = network.getValue("Output_bc");

                        if (fabs(outputAB)>0.2)
                        {
                            if (outputAB>0.0)
                                substrate.getLink(linkCounter)->weight = ( ((outputAB-0.2)/0.8)*3.0 );
                            else
                                substrate.getLink(linkCounter)->weight = ( ((outputAB+0.2)/0.8)*3.0 );
                        }
                        else
                        {
                            substrate.getLink(linkCounter)->weight = (0.0);
                        }

                        linkCounter++;

                        if (fabs(outputBC)>0.2)
                        {
                            if (outputBC>0.0)
                                substrate.getLink(linkCounter)->weight = ( ((outputBC-0.2)/0.8)*3.0 );
                            else
                                substrate.getLink(linkCounter)->weight = ( ((outputBC+0.2)/0.8)*3.0 );
                        }
                        else
                        {
                            substrate.getLink(linkCounter)->weight = (0.0);
                        }

                        linkCounter++;

                        if (x2==0&&y2==0)
                        {
                            double nodeBias;

                            nodeBias = network.getValue("Bias_b");

                            //substrate.setBias(
                                //nameLookup[Node(x1-numNodesX/2,y1-numNodesY/2,1)],
                                //nodeBias
                            //);

                            nodeBias = network.getValue("Bias_c");

                            //substrate.setBias(
                                //nameLookup[Node(x1-numNodesX/2,y1-numNodesY/2,2)],
                                //nodeBias
                            //);

                        }

                    }
                }
            }
        }
    }

    int checkWin(int xBoardState,int oBoardState)
    {
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
    }

    double TicTacToeGameExperiment::processEvaluation(
        wxDC *drawContext,
        int_gen_type &move_gen
    )
    {
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
                    int boardx = (x+(numNodesX/2))/3;
                    int boardy = (y+(numNodesY/2))/3;
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
                    int boardx = (x+(numNodesX/2))/3;
                    int boardy = (y+(numNodesY/2))/3;

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

    void TicTacToeGameExperiment::processGroup(shared_ptr<NEAT::GeneticGeneration> generation)
    {
        shared_ptr<NEAT::GeneticIndividual> individual = group.front();

        //You get 10 points just for entering the game, wahooo!
        individual->setFitness(10);

        populateSubstrate(individual);

        shared_ptr<TicTacToeStats> individualStats = shared_ptr<TicTacToeStats>(new TicTacToeStats());

        searchTree.getFullResults(individualStats.get(),substrate,numNodesX,numNodesY,nameLookup);

        double totalFitness;
        totalFitness = individualStats->wins + individualStats->ties;

        individual->reward( max(0.0,totalFitness) );

        individual->setUserData(individualStats->toString());
	}

    void TicTacToeGameExperiment::processIndividualPostHoc(shared_ptr<NEAT::GeneticIndividual> individual)
    {
        //You get 10 points just for entering the game, wahooo!
        individual->setFitness(10);

        populateSubstrate(individual);

        shared_ptr<TicTacToeStats> individualStats = shared_ptr<TicTacToeStats>(new TicTacToeStats());

        searchTree.getFullResults(individualStats.get(),substrate,numNodesX,numNodesY,nameLookup);

        double totalFitness;
        totalFitness = individualStats->wins + individualStats->ties;

        individual->reward( max(0.0,totalFitness) );

        individual->setUserData(individualStats->toString());
	}

#ifndef HCUBE_NOGUI
    void TicTacToeGameExperiment::createIndividualImage(wxDC &drawContext,shared_ptr<NEAT::GeneticIndividual> individual)
    {
        base_generator_type generator = base_generator_type(static_cast<unsigned int>(std::clock()%1000000000));

        int_gen_type move_gen = int_gen_type(generator, int_distribution_type(0, 8));

        if (lastIndividualSeen!=individual)
        {
            cout << "Repopulating substrate\n";
            populateSubstrate(individual);
            lastIndividualSeen = individual;
        }

        processEvaluation(&drawContext,move_gen);
    }

    bool TicTacToeGameExperiment::handleMousePress(wxMouseEvent& event,wxSize &bitmapSize)
    {
        wxPoint clickPoint = event.GetPosition();
        int x = clickPoint.x/20-1;
        int y = clickPoint.y/20-1;
        return true;
    }
#endif

    Experiment* TicTacToeGameExperiment::clone()
    {
        TicTacToeGameExperiment* experiment = new TicTacToeGameExperiment(*this);

        return experiment;
    }

    void TicTacToeGameExperiment::resetGenerationData(shared_ptr<NEAT::GeneticGeneration> generation)
    {
		TicTacToeStats stats;
        generation->setUserData(stats.toString());
    }

    void TicTacToeGameExperiment::addGenerationData(shared_ptr<NEAT::GeneticGeneration> generation,shared_ptr<NEAT::GeneticIndividual> individual)
    {
        TicTacToeStats ticTacToeStats = TicTacToeStats(generation->getUserData());

        if (individual->getUserData().length())
            ticTacToeStats += TicTacToeStats(individual->getUserData());

		generation->setUserData(ticTacToeStats.toString());
    }
}
