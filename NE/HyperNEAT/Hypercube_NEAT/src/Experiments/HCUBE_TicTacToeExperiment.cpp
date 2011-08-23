#include "HCUBE_Defines.h"

#include "Experiments/HCUBE_TicTacToeExperiment.h"

#define TIC_TAC_TOE_EXPERIMENT_DEBUG (0)

namespace HCUBE
{
    using namespace NEAT;

    extern int checkWin(int xBoardState,int oBoardState);

    TicTacToeExperiment::TicTacToeExperiment(string _experimentName,int _threadID)
            :
            Experiment(_experimentName,_threadID),
            numNodesX(9),
            numNodesY(9),
            userBoardState(0)
    {
        generateSubstrate();
    }

    GeneticPopulation* TicTacToeExperiment::createInitialPopulation(int populationSize)
    {
        GeneticPopulation *population = new GeneticPopulation();
        vector<GeneticNodeGene> genes;

        genes.push_back(GeneticNodeGene("Bias","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("X1","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("Y1","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("X2","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("Y2","NetworkSensor",0,false));
        //genes.push_back(GeneticNodeGene("DeltaX","NetworkSensor",0,false));
        //genes.push_back(GeneticNodeGene("DeltaY","NetworkSensor",0,false));
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

    void TicTacToeExperiment::generateSubstrate()
    {
        cout << "Generating substrate...";
        boost::object_pool<NEAT::NetworkNode> networkNodePool;
        boost::object_pool<NEAT::NetworkLink> networkLinkPool;

        NEAT::NetworkNode *nodes = NULL;
        NEAT::NetworkLink *links = NULL;
        double *nodeBiases = NULL;

        try
        {
            nodes = (NEAT::NetworkNode*)malloc(sizeof(NEAT::NetworkNode)*numNodesX*numNodesY*3);
            links = (NEAT::NetworkLink*)malloc(sizeof(NEAT::NetworkLink)*numNodesX*numNodesY*numNodesX*numNodesY*2);
            nodeBiases = new double[numNodesX*numNodesY*3];
        }
        catch(const std::exception &e)
        {
            cout << e.what() << endl;
            CREATE_PAUSE(string("Error!: ")+toString(__LINE__));
        }

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
//cout << nodes[y1*numNodesX*3 + x1*3 + z1].getName() << " -> " << nodes[y2*numNodesX*3 + x2*3 + z2].getName() << endl;
                                    new (&links[linkCounter++]) NetworkLink(
                                        &nodes[y1*numNodesX*3 + x1*3 + z1],
                                        &nodes[y2*numNodesX*3 + x2*3 + z2],
                                        0.0
                                    );
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

        cout << "Creating FastBiasNetwork\n";

        substrate = NEAT::FastBiasNetwork<double>(
                        nodes,
                        nodeCounter,
                        links,
                        linkCounter,
                        nodeBiases
                    );

        cout << "Testing new substrate\n";
        substrate.getLink("-4/-4/0","-4/-4/1");

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

    void TicTacToeExperiment::populateSubstrate(shared_ptr<const NEAT::GeneticIndividual> individual)
    {
        NEAT::FastNetwork<double> network = individual->spawnFastPhenotypeStack<double>();

        for (int y1=0;y1<numNodesY;y1++)
        {
            for (int x1=0;x1<numNodesX;x1++)
            {
                for (int y2=0;y2<numNodesY;y2++)
                {
                    for (int x2=0;x2<numNodesX;x2++)
                    {
                        int xmod = x1-numNodesX/2;
                        int ymod = y1-numNodesY/2;

                        double x1normal = (x1-numNodesX/2)/double((numNodesX-1)/2);
                        double y1normal = (y1-numNodesY/2)/double((numNodesY-1)/2);

                        double x2normal = (x2-numNodesX/2)/double((numNodesX-1)/2);
                        double y2normal = (y2-numNodesY/2)/double((numNodesY-1)/2);

                        network.reinitialize();
                        network.setValue("X1",x1normal);
                        network.setValue("Y1",y1normal);
                        network.setValue("X2",x2normal);
                        network.setValue("Y2",y2normal);
                        if (network.hasNode("DeltaX"))
                        {
                            network.setValue("DeltaX",x2normal-x1normal);
                            network.setValue("DeltaY",y2normal-y1normal);
                        }
                        network.setValue("Bias",0.3);
                        network.update();

                        double outputAB = network.getValue("Output_ab");
                        double outputBC = network.getValue("Output_bc");

                        NetworkIndexedLink<double>* link;

                        link = substrate.getLink(
                            nameLookup[Node(xmod,ymod,0)],
                            nameLookup[Node(xmod,ymod,1)]
                        );

                        if (fabs(outputAB)>0.2)
                        {
                            if (outputAB>0.0)
                                link->weight = ( ((outputAB-0.2)/0.8)*3.0 );
                            else
                                link->weight = ( ((outputAB+0.2)/0.8)*3.0 );
                        }
                        else
                        {
                            link->weight = (0.0);
                        }

                        link = substrate.getLink(
                            nameLookup[Node(xmod,ymod,1)],
                            nameLookup[Node(xmod,ymod,2)]
                        );

                        if (fabs(outputBC)>0.2)
                        {
                            if (outputBC>0.0)
                                link->weight = ( ((outputBC-0.2)/0.8)*3.0 );
                            else
                                link->weight = ( ((outputBC+0.2)/0.8)*3.0 );
                        }
                        else
                        {
                            link->weight = (0.0);
                        }

                        if (x2==0&&y2==0)
                        {
                            double nodeBias;

                            nodeBias = network.getValue("Bias_b");

                            substrate.setBias(
                                nameLookup[Node(x1-numNodesX/2,y1-numNodesY/2,1)],
                                nodeBias
                            );

                            nodeBias = network.getValue("Bias_c");

                            substrate.setBias(
                                nameLookup[Node(x1-numNodesX/2,y1-numNodesY/2,2)],
                                nodeBias
                            );

                        }

                    }
                }
            }
        }
    }

    double TicTacToeExperiment::processEvaluation(
        shared_ptr<NEAT::GeneticIndividual> individual,
        wxDC *drawContext,
        int boardState
    )
    {
        if (drawContext)drawContext->Clear();
        double fitness = 0;

        double totalFitness=0;

        {
#if TIC_TAC_TOE_EXPERIMENT_DEBUG
            {
                cout << boardState << endl;
            }
#endif
            int xCount=0;
            for (int y=0;y<3;y++)
            {
                for (int x=0;x<3;x++)
                {
                    if ((HASBIT(boardState,y*3+(x))))
                        xCount++;
#if TIC_TAC_TOE_EXPERIMENT_DEBUG
                    if ((HASBIT(boardState,y*3+(x))))
                    {
                        cout << 'X';
                    }
                    else
                    {
                        cout << 'O';
                    }
#endif
                }
#if TIC_TAC_TOE_EXPERIMENT_DEBUG
                cout << endl;
#endif
            }
#if TIC_TAC_TOE_EXPERIMENT_DEBUG
            cout << endl;
#endif

            //Check to see if it is a valid board
            if (xCount!=5)
                cout << "Invalid board!  Results might not be accurate\n";

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
                    substrate.setValue( nameLookup[Node(x,y,0)] , ((HASBIT(boardState,(boardy)*3+(boardx)))*2.0)-1.0 );

                    bool isAnX = HASBIT(boardState,(boardy)*3+(boardx));

                    if (drawContext)drawContext->DrawText((isAnX?_T("X"):_T("O")),(boardx+2)*20,(boardy+2)*20);

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

#if TIC_TAC_TOE_EXPERIMENT_DEBUG
            {
                cout << "Set initial values\n";
                //cout << "Press enter to continue\n";string line;getline(cin,line);
            }
#endif

            bool brainThinksItWon=false;

            for (int y=(-numNodesY/2);y<=numNodesY/2;y++)
            {
                for (int x=(-numNodesX/2);x<=numNodesX/2;x++)
                {
                    double value = substrate.getValue( nameLookup[Node(x,y,2)] );

                    if (value>0.2)
                    {
                        brainThinksItWon=true;
                        break;
                    }
                }
            }

            if (drawContext)
            {
                if (brainThinksItWon)
                {
                    drawContext->DrawText(_T("Brain thinks X won"),(1)*20,(5)*20);

                    if (checkWin(boardState,0)==1)
                        drawContext->DrawText(_T("Brain is right!"),(1)*20,(7)*20);

                }
                else
                {
                    drawContext->DrawText(_T("Brain thinks X Didn't win"),(1)*20,(5)*20);

                    if (checkWin(boardState,0)==0)
                        drawContext->DrawText(_T("Brain is right!"),(1)*20,(7)*20);

                }
            }

            double fitness=0.0;

            if ( (checkWin(boardState,0)==1) && (brainThinksItWon) )
            {
                fitness = 1.0;
            }
            else if ( (checkWin(boardState,0)!=1) && (!brainThinksItWon) )
            {
                fitness = 5.0;
            }

            return fitness;

            /*
            This is old code which checked for a win in a certain row.  I'm not there yet.
            for (int y=(-numNodesY/2);y<=numNodesY/2;y++)
            {
                for (int x=(-numNodesX/2);x<=numNodesX/2;x++)
                {
                    if (x>-2&&y>-2&&x<2&&y<2)
                        continue;

                    int xCount=0;

                    if (x==-2||y==-2)
                    {
                        //Don't check left side (yet)
                        continue;
                    }

                    if (abs(x)==(numNodesX/2)) //x is on the edge
                    {
                        if (y==-(numNodesY/2))
                        {}
                        else if (y==(numNodesY/2))
                        {}
                        else
                        {
                            //horizontal line
                            for (int newx=0;newx<3;newx++)
                            {
                                if ((HASBIT(boardState,(y+1)*3+newx)))
                                {
                                    xCount++;
                                }
                            }

            #if TIC_TAC_TOE_EXPERIMENT_DEBUG
                            if (xCount>2)
                            {
                                cout << "Horizontal line at y:" << (y+1) << endl;
                            }
            #endif

                        }
                    }

                    if (abs(y)==(numNodesY/2)) //y is on the edge
                    {
                        if (x==-(numNodesY/2))
                        {}
                        else if (x==(numNodesY/2))
                        {}
                        else
                        {
                            //vertical line
                            for (int newy=0;newy<3;newy++)
                            {
                                if ((HASBIT(boardState,newy*3+(x+1))))
                                {
                                    xCount++;
                                }
                            }

            #if TIC_TAC_TOE_EXPERIMENT_DEBUG
                            if (xCount>2)
                            {
                                cout << "Vertical line at x:" << (x+1) << endl;
                            }
            #endif

                        }
                    }

                    double answer = substrate.getValue(toString(y)+string("/")+toString(x));

                    if (xCount>2)
                    {
                        totalFitness+=5;

                        if (answer>=0.6)
                        {
                            fitness += 5;
                            if (drawContext)drawContext->DrawText('P',(x+2)*20,(y+2)*20);
                        }
                        else
                        {
                            if (drawContext)drawContext->DrawText('F',(x+2)*20,(y+2)*20);
                        }
                    }
                    *//* This was commented out before
else if (xCount>1)
{
if(substrate->getNode(toString(y)+string("/")+toString(x))->getValue()>0.5)
individual->reward(0);
else if(substrate->getNode(toString(y)+string("/")+toString(x))->getValue()>0.2)
individual->reward(5);
else if(substrate->getNode(toString(y)+string("/")+toString(x))->getValue()>0.0)
individual->reward(10);
}
*//*
else
{
totalFitness+=1;

if (answer<0.6)
{
fitness += 1;
if (drawContext)drawContext->DrawText('P',(x+2)*20,(y+2)*20);
}
else
{
if (drawContext)drawContext->DrawText('F',(x+2)*20,(y+2)*20);
}
}


}

}
*/

#if TIC_TAC_TOE_EXPERIMENT_DEBUG
            /*{
                cout << "Done checking board\n";
            }*/
#endif
        }

#if TIC_TAC_TOE_EXPERIMENT_DEBUG
        {
            totalFitness *= totalFitness;
        }
#endif

        fitness *= fitness;

#if TIC_TAC_TOE_EXPERIMENT_DEBUG
        return totalFitness;
#else
        return fitness;
#endif
    }

    void TicTacToeExperiment::processGroup(shared_ptr<NEAT::GeneticGeneration> generation)
    {
        shared_ptr<NEAT::GeneticIndividual> individual = group.front();
        //You get 10 points just for entering the game, wahooo!
        individual->setFitness(10);

        populateSubstrate(individual);

        int totalBoards=0;

        int totalFitness=0;

        int boardsXWin=0;

        for (int boardState=0;boardState<512;boardState++)
        {
#if TIC_TAC_TOE_EXPERIMENT_DEBUG
            {
                cout << boardState << endl;
            }
#endif
            int xCount=0;
            for (int y=0;y<3;y++)
            {
                for (int x=0;x<3;x++)
                {
                    if ((HASBIT(boardState,y*3+(x))))
                        xCount++;
                    /*
                    #if TIC_TAC_TOE_EXPERIMENT_DEBUG
                                            if ((HASBIT(boardState,y*3+(x))))
                                            {
                                                cout << 'X';
                                            }
                                            else
                                            {
                                                cout << 'O';
                                            }
                    #endif
                    */
                }
                /*
                #if TIC_TAC_TOE_EXPERIMENT_DEBUG
                                    cout << endl;
                #endif
                */
            }
            /*
            #if TIC_TAC_TOE_EXPERIMENT_DEBUG
                            cout << endl;
            #endif
            */

            //Check to see if it is a valid board
            if (xCount!=5)
                continue;

            totalBoards++;

            if (checkWin(boardState,0)==1)
            {
                boardsXWin++;
            }

            //cout << (string("Processing Evaluation: ")+toString(totalBoards)+toString("\n"));
            individual->reward(processEvaluation(individual,NULL,boardState));

        }

#if TIC_TAC_TOE_EXPERIMENT_DEBUG
        {
            cout << "Total Boards: " << totalBoards << endl;
            cout << "X Win Boards: " << boardsXWin << endl;
            cout << "Total Fitness: " << individual->getFitness() << endl;
            cout << "DONE EVALUATIONS!\n";
            cout << "Press enter to continue\n";
            string line;
            getline(cin,line);
        }
#endif

    }

#ifndef HCUBE_NOGUI
    void TicTacToeExperiment::createIndividualImage(wxDC &drawContext,shared_ptr<NEAT::GeneticIndividual> individual)
    {
        if (lastIndividualSeen!=individual)
        {
            cout << "Repopulating substrate\n";
            populateSubstrate(individual);
            lastIndividualSeen = individual;
        }

        processEvaluation(individual,&drawContext,userBoardState);
    }

    bool TicTacToeExperiment::handleMousePress(wxMouseEvent& event,wxSize &bitmapSize)
    {
        wxPoint clickPoint = event.GetPosition();
        int x = (clickPoint.x/20) - 2;
        int y = (clickPoint.y/20) - 2;

        cout << "Clicked on row " << y << " and column " << x << endl;

        if (x>=0&&y>=0&&x<3&&y<3)
            TOGGLEBIT(userBoardState,(y*3)+x);

        return true;
    }
#endif

    Experiment* TicTacToeExperiment::clone()
    {
        TicTacToeExperiment* experiment = new TicTacToeExperiment(*this);

        return experiment;
    }

}
