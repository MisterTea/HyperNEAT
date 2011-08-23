#include "Experiments/HCUBE_FindClusterBiasExperiment.h"

#define FIND_CLUSTER_EXPERIMENT_DEBUG (0)

#define FIND_CLUSTER_EXPERIMENT_USE_DELTAS (0)

namespace HCUBE
{
    using namespace NEAT;

    FindClusterBiasExperiment::FindClusterBiasExperiment(string _experimentName)
            :
            Experiment(_experimentName),
            numNodesX(11),
            numNodesY(11),
            user1(1,1),
            user2(4,4),
            userDisplayOutputLinkWeights(-1,-1),
            userDisplayOutputs(false),
            clicked1Last(true),
            CELL_SIZE(15),
            sizeMultiplier(1)
    {
        generateSubstrate();
    }

    NEAT::GeneticPopulation* FindClusterBiasExperiment::createInitialPopulation(int populationSize)
    {
        NEAT::GeneticPopulation* population = new NEAT::GeneticPopulation();

        vector<GeneticNodeGene> genes;

        genes.push_back(GeneticNodeGene("Bias","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("X1","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("Y1","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("X2","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("Y2","NetworkSensor",0,false));
#if FIND_CLUSTER_EXPERIMENT_USE_DELTAS
        genes.push_back(GeneticNodeGene("DeltaX","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("DeltaY","NetworkSensor",0,false));
#endif
        genes.push_back(GeneticNodeGene("Output","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));

        for (int a=0;a<populationSize;a++)
        {
            shared_ptr<GeneticIndividual> individual(new GeneticIndividual(genes,true,1.0));

            for (int b=0;b<0;b++)
            {
                individual->testMutate();
            }

            population->addIndividual(individual);
        }

        return population;
    }

    void FindClusterBiasExperiment::generateSubstrate()
    {
        cout << "Generating substrate...";
        boost::object_pool<NEAT::NetworkNode> networkNodePool;
        boost::object_pool<NEAT::NetworkLink> networkLinkPool;

        NEAT::NetworkNode *nodes = NULL;
        NEAT::NetworkLink *links = NULL;
        double *nodeBiases = NULL;

        try
        {
            nodes = new NEAT::NetworkNode[numNodesX*numNodesY];
            links = new NEAT::NetworkLink[numNodesX*numNodesY*numNodesX*numNodesY];
            nodeBiases = new double[numNodesX*numNodesY];
        }
        catch (exception e)
        {
            cout << e.what() << endl;
            CREATE_PAUSE(string("Error!: ")+toString(__LINE__));
        }

        memset(nodeBiases,0,sizeof(double)*numNodesX*numNodesY);

        nameLookup.clear();

        cout << "Creating nodes...\n";

        int counter=0;

        for (int y1=0;y1<numNodesY;y1++)
        {
            for (int x1=0;x1<numNodesX;x1++)
            {
                //cout << (y1-numNodesY/2) << '/' << (x1-numNodesX/2) << endl;
                nameLookup[pair<int,int>(y1-numNodesY/2,x1-numNodesX/2)] = toString(y1-numNodesY/2)+string("/")+toString(x1-numNodesX/2);
                nodes[counter++] = NetworkNode(nameLookup[pair<int,int>(y1-numNodesY/2,x1-numNodesX/2)]);

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

        cout << "Creating links...\n";

        counter=0;

        for (int y1=0;y1<numNodesY;y1++)
        {
            for (int x1=0;x1<numNodesX;x1++)
            {
                for (int y2=0;y2<numNodesY;y2++)
                {
                    for (int x2=0;x2<numNodesX;x2++)
                    {
                        try
                        {
                            links[counter++] =  NetworkLink(&nodes[y1*numNodesX+x1],&nodes[y2*numNodesX+x2],0.0);
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
                        catch (exception e)
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

        cout << "Creating FastNetwork\n";

        substrate = NEAT::FastBiasNetwork(
                        nodes,
                        numNodesX*numNodesY,
                        links,
                        numNodesX*numNodesY*numNodesX*numNodesY,
                        nodeBiases
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

        delete[] nodes;
        delete[] links;
        delete[] nodeBiases;
    }

    void FindClusterBiasExperiment::populateSubstrate(shared_ptr<const NEAT::GeneticIndividual> individual)
    {
        cout << "Populating substrate...";
        NEAT::FastNetwork network = individual->spawnFastPhenotypeStack();
        progress_timer t;

        int linkCount=0;

        int counter=0;
        double x1Val,y1Val,x2Val,y2Val;
        for (int y1=0;y1<numNodesY;y1++)
        {
            for (int x1=0;x1<numNodesX;x1++)
            {
                for (int y2=0;y2<numNodesY;y2++)
                {
                    for (int x2=0;x2<numNodesX;x2++)
                    {
                        x1Val = (x1-numNodesX/2)/double((numNodesX-1)/2);
                        y1Val = (y1-numNodesY/2)/double((numNodesY-1)/2);
                        x2Val = (x2-numNodesX/2)/double((numNodesX-1)/2);
                        y2Val = (y2-numNodesY/2)/double((numNodesY-1)/2);

                        network.reinitialize();
                        network.setValue("X1",x1Val);
                        network.setValue("Y1",y1Val);
                        network.setValue("X2",x2Val);
                        network.setValue("Y2",y2Val);

                        if (network.hasNode("DeltaX"))
                        {
                            network.setValue("DeltaX",x2Val-x1Val);
                            network.setValue("DeltaY",y2Val-y1Val);
                        }

                        network.setValue("Bias",0.3);
                        network.update();


                        double greyVal = network.getValue("Output");

                        //substrate.getLink(counter)->weight = ( greyVal*3.0 );

                        /*
                        if (greyVal>0.0)
                            substrate.getLink(counter)->weight = ( (max(0,greyVal-0.2)/0.8)*3.0 );
                        else
                            substrate.getLink(counter)->weight = ( (min(0,greyVal+0.2)/0.8)*3.0 );
                        */

                        if (fabs(greyVal)>0.2)
                        {
                            if (greyVal>0.0)
                                substrate.getLink(counter)->weight = ( ((greyVal-0.2)/0.8)*3.0 );
                            else
                                substrate.getLink(counter)->weight = ( ((greyVal+0.2)/0.8)*3.0 );
                            linkCount++;
                        }
                        else
                        {
                            substrate.getLink(counter)->weight = (0.0);
                        }

                        counter++;
                    }
                }
            }
        }

        cout << "done!\n";

        cout << "Number of expressed links: " << linkCount << endl;
    }

    double FindClusterBiasExperiment::processEvaluation(
        shared_ptr<NEAT::GeneticIndividual> individual,
        wxDC *drawContext,
        int x1,
        int y1,
        int x1Big,
        int y1Big
    )
    {
#if FIND_CLUSTER_EXPERIMENT_DEBUG
        cout << "EVALUATION: Testing " << x1 << ',' << y1 << " and big " << x1Big << ',' << y1Big << endl;
        CREATE_PAUSE(string("Error!: ")+toString(__LINE__));
#endif

        int halfSizeX=0;
        int halfSizeY=0;

#ifndef HCUBE_NOGUI
        if (drawContext)
        {
            drawContext->Clear();

            halfSizeX = drawContext->GetSize().x/2;
            halfSizeY = drawContext->GetSize().y/2;

            drawContext->SetPen(wxPen(wxColor(128,128,128),1,wxSOLID));

            drawContext->SetDeviceOrigin(-CELL_SIZE/2,-CELL_SIZE/2);

            if (userDisplayOutputLinkWeights.x<0)
            {
                for (int y2=0;y2<numNodesY;y2++)
                {
                    for (int x2=0;x2<numNodesX;x2++)
                    {
                        drawContext->DrawRectangle(
                            (x2-numNodesX/2)*CELL_SIZE+halfSizeX,
                            (y2-numNodesY/2)*CELL_SIZE+halfSizeY,
                            CELL_SIZE,
                            CELL_SIZE
                        );
                    }
                }
            }

            drawContext->SetPen(wxNullPen);
        }
#endif

#if FIND_CLUSTER_EXPERIMENT_DEBUG
        cout << "Initializing substrate\n";
        CREATE_PAUSE(string("Error!: ")+toString(__LINE__));
#endif

        substrate.reinitialize();
        substrate.dummyActivation();

#if FIND_CLUSTER_EXPERIMENT_DEBUG
        cout << "Setting values\n";
        CREATE_PAUSE(string("Error!: ")+toString(__LINE__));
#endif

        /*
        substrate.setValue(nameLookup[pair<int,int>((y1-numNodesY/2),(x1-numNodesX/2))],1.0);
        if (drawContext)
        {
            drawContext->SetPen(wxPen(wxColor(0,0,0),2,wxSOLID));
            drawContext->DrawRectangle((x1-numNodesX/2)*CELL_SIZE+halfSizeX,(y1-numNodesY/2)*CELL_SIZE+halfSizeY,CELL_SIZE,CELL_SIZE);
        }
        */

        int smallRadius;
        int bigRadius;

        double filledValue;

        if (sizeMultiplier==1)
        {
            smallRadius=0;
            bigRadius=1;
            filledValue=1.0;
        }
        else if (sizeMultiplier==3)
        {
            smallRadius=1;
            bigRadius=4;
            filledValue= (11.0/33.0)*(11.0/33.0);
        }
        else if (sizeMultiplier==5)
        {
            smallRadius=2;
            bigRadius=7;
            filledValue= (11.0/55.0)*(11.0/55.0);
        }
        else if (sizeMultiplier==9)
        {
            smallRadius=4;
            bigRadius=13;
            filledValue= (11.0/66.0)*(11.0/66.0);
        }
        else
        {
            throw CREATE_LOCATEDEXCEPTION_INFO("Unsupported size multiplier!");
        }

        for (int mody=-1*smallRadius;mody<=1*smallRadius;mody++)
            for (int modx=-1*smallRadius;modx<=1*smallRadius;modx++)
            {
                try
                {
                    substrate.setValue(
                        nameLookup[pair<int,int>((y1+mody)-numNodesY/2,
                                                 (x1+modx)-numNodesX/2)],
                        filledValue
                    );
                }
                catch (...)
                {
                    cout << "INVALID LOCATION: " << ((y1Big+mody)-numNodesY/2) << ',' << ((x1Big+modx)-numNodesX/2) << endl;
                }

#ifndef HCUBE_NOGUI
                if (drawContext&&userDisplayOutputLinkWeights.x<0&&!userDisplayOutputs)
                {
                    drawContext->SetPen(*wxBLACK);
                    drawContext->SetBrush(wxBrush(*wxBLACK,wxSOLID));

                    drawContext->DrawRectangle(
                        (x1+modx-numNodesX/2)*CELL_SIZE+halfSizeX,
                        (y1+mody-numNodesY/2)*CELL_SIZE+halfSizeY,CELL_SIZE,CELL_SIZE
                    );
                }
#endif
            }

        for (int mody=-1*bigRadius;mody<=1*bigRadius;mody++)
            for (int modx=-1*bigRadius;modx<=1*bigRadius;modx++)
            {
                try
                {
                    substrate.setValue(
                        nameLookup[pair<int,int>((y1Big+mody)-numNodesY/2,
                                                 (x1Big+modx)-numNodesX/2)],
                        filledValue
                    );
                }
                catch (...)
                {
                    cout << "INVALID LOCATION: " << ((y1Big+mody)-numNodesY/2) << ',' << ((x1Big+modx)-numNodesX/2) << endl;
                }

#ifndef HCUBE_NOGUI
                if (drawContext&&userDisplayOutputLinkWeights.x<0&&!userDisplayOutputs)
                {
                    drawContext->SetBrush(wxBrush(*wxBLACK,wxSOLID));

                    drawContext->DrawRectangle(
                        (x1Big+modx-numNodesX/2)*CELL_SIZE+halfSizeX,
                        (y1Big+mody-numNodesY/2)*CELL_SIZE+halfSizeY,CELL_SIZE,CELL_SIZE
                    );
                }
#endif
            }

#if FIND_CLUSTER_EXPERIMENT_DEBUG
        cout << "Updating Substrate\n";
        CREATE_PAUSE(string("Error!: ")+toString(__LINE__));
#endif

#ifndef HCUBE_NOGUI
        if (drawContext)
        {
            drawContext->SetPen(wxNullPen);
            drawContext->SetBrush(wxBrush(*wxBLACK,wxTRANSPARENT));
        }
#endif

        substrate.update(1);

        double largestValue = -INT_MAX;
        double smallestValue = INT_MAX;
        int largesty,largestx;

        for (int y2=0;y2<numNodesY;y2++)
        {
            for (int x2=0;x2<numNodesX;x2++)
            {
                double value = substrate.getValue(nameLookup[pair<int,int>((y2-numNodesY/2),(x2-numNodesX/2))]);

                if (value > largestValue)
                {
                    largestValue = value;
                    largestx = x2;
                    largesty = y2;
                }
                else if (value==largestValue)
                {
                    //cout << "ERROR: value and largestValue are equal!!!\n";
                    //cout << value << ' ' << largestValue << endl;
                    //CREATE_PAUSE(string("Error!: ")+toString(__LINE__));
                    /*
                    if(
                    max(0,30- ( (x2-x1Big)*(x2-x1Big) + (y2-y1Big)*(y2-y1Big) ) )
                    >
                    max(0,30- ( (largestx-x1Big)*(largestx-x1Big) + (largesty-y1Big)*(largesty-y1Big) ) )
                    )
                    {
                        largestValue = value;
                        largestx = x2;
                        largesty = y2;

                    }
                    */
                }

                if (value<smallestValue)
                {
                    smallestValue=value;
                }

            }
        }

#ifndef HCUBE_NOGUI
        if (userDisplayOutputs)
        {
            for (int y2=0;y2<numNodesY;y2++)
            {
                for (int x2=0;x2<numNodesX;x2++)
                {
                    double value = substrate.getValue(nameLookup[pair<int,int>((y2-numNodesY/2),(x2-numNodesX/2))]);

                    /*
                    if (value>0.0)
                    {
                        if (255-int(floor(fabs(value)*255.0))<0||255-int(floor(fabs(value)*255.0))>255)
                        {
                            cout << "OOPS!!!! BAD COLOR!\n";
                        }

                        drawContext->SetBrush(
                            wxBrush(
                                wxColor(
                                    255-int(floor(fabs(value)*255.0)),
                                    255-int(floor(fabs(value)*255.0)),
                                    255-int(floor(fabs(value)*255.0))
                                )
                            )
                        );
                    }
                    else
                    {
                        if (255-int(floor(fabs(value)*255.0))<0||255-int(floor(fabs(value)*255.0))>255)
                        {
                            cout << "OOPS!!!! BAD COLOR!\n";
                        }

                        drawContext->SetBrush(
                            wxBrush(
                                wxColor(
                                    255-int(floor(fabs(value)*255.0)),
                                    255-int(floor(fabs(value)*255.0)),
                                    255-int(floor(fabs(value)*255.0))
                                ),
                                wxCROSSDIAG_HATCH
                            )
                        );
                    }
                    */

                    double fixedValue = value-smallestValue;

                    double normalizedValue = fixedValue / (largestValue-smallestValue);


                    if (normalizedValue<0.999)
                    {
                        normalizedValue=0;
                    }
                    else
                    {
                        normalizedValue= (normalizedValue-0.999)/0.001;
                    }

                    if (normalizedValue>0.1)
                    {
                        drawContext->SetBrush(
                            wxBrush(
                                wxColor(
                                    255-int(floor((normalizedValue)*255.0)),
                                    255-int(floor((normalizedValue)*255.0)),
                                    255-int(floor((normalizedValue)*255.0))
                                )
                            )
                        );

                        drawContext->SetPen(*wxTRANSPARENT_PEN);

                        drawContext->DrawRectangle(
                            (x2-numNodesX/2)*CELL_SIZE+halfSizeX,
                            (y2-numNodesY/2)*CELL_SIZE+halfSizeY,
                            CELL_SIZE,
                            CELL_SIZE
                        );

                        drawContext->SetPen(wxNullPen);
                        drawContext->SetBrush(wxNullBrush);
                    }
                }
            }
        }

        if (drawContext)
        {
            string fromNodeName = nameLookup[pair<int,int>((userDisplayOutputLinkWeights.y-numNodesY/2),(userDisplayOutputLinkWeights.x-numNodesX/2))];
            if (userDisplayOutputLinkWeights.x>=0)
            {
                for (int y2=0;y2<numNodesY;y2++)
                {
                    for (int x2=0;x2<numNodesX;x2++)
                    {
                        double weight =
                            substrate.getLink(
                                //fromNodeName,
                                //nameLookup[pair<int,int>((y2-numNodesY/2),(x2-numNodesX/2))]
                                userDisplayOutputLinkWeights.y*numNodesX*numNodesY*numNodesX+
                                userDisplayOutputLinkWeights.x*numNodesY*numNodesX+
                                y2*numNodesX+
                                x2
                            )->weight;

                        /*double weight =
                            substrate.getLink(
                                (
                                    toString(userDisplayOutputLinkWeights.y-numNodesY/2) +
                                    string("/") +
                                    toString(userDisplayOutputLinkWeights.x-numNodesX/2)
                                ),
                                (
                                    toString(y2-numNodesY/2) +
                                    string("/") +
                                    toString(x2-numNodesX/2)
                                )
                            )->weight;*/

                        drawContext->SetPen(wxNullPen);
                        if (weight>0.2)
                        {
                            if (255-int(floor(fabs(weight)/3.0*255.0))<0||255-int(floor(fabs(weight)/3.0*255.0))>255)
                            {
                                cout << "OOPS!!!! BAD COLOR! " << weight << endl;
                            }

                            drawContext->SetBrush(
                                wxBrush(
                                    wxColor(
                                        255-int(floor(fabs(weight)/3.0*255.0)),
                                        255-int(floor(fabs(weight)/3.0*255.0)),
                                        255-int(floor(fabs(weight)/3.0*255.0))
                                    )
                                )
                            );
                        }
                        else if (weight<-0.2)
                        {
                            if (255-int(floor(fabs(weight)/3.0*255.0))<0||255-int(floor(fabs(weight)/3.0*255.0))>255)
                            {
                                cout << "OOPS!!!! BAD COLOR!" << weight << endl;
                            }

                            drawContext->SetBrush(
                                wxBrush(
                                    wxColor(
                                        255-int(floor(fabs(weight)/3.0*255.0)),
                                        255-int(floor(fabs(weight)/3.0*255.0)),
                                        255-int(floor(fabs(weight)/3.0*255.0))
                                    ),
                                    wxCROSSDIAG_HATCH
                                )
                            );
                        }

                        if (fabs(weight)>0.2)
                        {
                            drawContext->SetPen(*wxTRANSPARENT_PEN);
                            drawContext->DrawRectangle(
                                (x2-numNodesX/2)*CELL_SIZE+halfSizeX,
                                (y2-numNodesY/2)*CELL_SIZE+halfSizeY,
                                CELL_SIZE,
                                CELL_SIZE
                            );
                            drawContext->SetPen(wxNullPen);
                        }



                        /*wxFont font(8, wxROMAN, wxNORMAL, wxNORMAL, false);
                        drawContext->SetFont(font);

                        drawContext->DrawText(
                            toString(floor(value*10+0.50001)/10),
                            (x2-numNodesX/2)*CELL_SIZE+halfSizeX+1,
                            (y2-numNodesY/2)*CELL_SIZE+halfSizeY+1);*/
                    }
                }
            }

            drawContext->SetPen(wxPen(*wxRED,2,wxSOLID));
            drawContext->SetBrush(wxBrush(*wxRED,wxSOLID));

            /*
            drawContext->DrawRectangle(
            (largestx-numNodesX/2)*CELL_SIZE+halfSizeX,
            (largesty-numNodesY/2)*CELL_SIZE+halfSizeY,
            CELL_SIZE,
            CELL_SIZE
            );
            */

            int targetRadius = 10;//CELL_SIZE;

            if (userDisplayOutputLinkWeights.x>=0)
            {
                drawContext->DrawCircle(
                    (userDisplayOutputLinkWeights.x-numNodesX/2)*CELL_SIZE+halfSizeX+CELL_SIZE/2,
                    (userDisplayOutputLinkWeights.y-numNodesY/2)*CELL_SIZE+halfSizeY+CELL_SIZE/2,
                    targetRadius/2
                );
            }
            else if (userDisplayOutputs)
            {
                drawContext->SetPen(wxPen(*wxWHITE,3,wxSOLID));

                drawContext->DrawLine(
                    (largestx-numNodesX/2)*CELL_SIZE+halfSizeX+CELL_SIZE/2 - 10,
                    (largesty-numNodesY/2)*CELL_SIZE+halfSizeY+CELL_SIZE/2 - 10,
                    (largestx-numNodesX/2)*CELL_SIZE+halfSizeX+CELL_SIZE/2 + 10,
                    (largesty-numNodesY/2)*CELL_SIZE+halfSizeY+CELL_SIZE/2 + 10
                );

                drawContext->DrawLine(
                    (largestx-numNodesX/2)*CELL_SIZE+halfSizeX+CELL_SIZE/2 + 10,
                    (largesty-numNodesY/2)*CELL_SIZE+halfSizeY+CELL_SIZE/2 - 10,
                    (largestx-numNodesX/2)*CELL_SIZE+halfSizeX+CELL_SIZE/2 - 10,
                    (largesty-numNodesY/2)*CELL_SIZE+halfSizeY+CELL_SIZE/2 + 10
                );
            }
            else
            {
                drawContext->DrawCircle(
                    (largestx-numNodesX/2)*CELL_SIZE+halfSizeX+CELL_SIZE/2,
                    (largesty-numNodesY/2)*CELL_SIZE+halfSizeY+CELL_SIZE/2,
                    targetRadius/2
                );
            }

        }
#endif

        if (individual->getUserData())
        {
            double x1Normal = (x1-numNodesX/2)/double((numNodesX-1)/2);
            double y1Normal = (y1-numNodesY/2)/double((numNodesY-1)/2);

            double x1BigNormal = (x1Big-numNodesX/2)/double((numNodesX-1)/2);
            double y1BigNormal = (y1Big-numNodesY/2)/double((numNodesY-1)/2);

            double largestxNormal = (largestx-numNodesX/2)/double((numNodesX-1)/2);
            double largestyNormal = (largesty-numNodesY/2)/double((numNodesY-1)/2);

            Vector2<double> baitVec(x1Normal,y1Normal);
            Vector2<double> targetVec(x1BigNormal,y1BigNormal);
            Vector2<double> actualVec(largestxNormal,largestyNormal);
            ((FindClusterBiasStats*)individual->getUserData())->addStat(
                baitVec,
                targetVec,
                actualVec
            );
        }

        //cout << "Trial Complete: " << x1 << ',' << y1 << " and " << x1Big << ',' << y1Big << ".  Score: "
        //<< max(0,30- ( (largestx-x1Big)*(largestx-x1Big) + (largesty-y1Big)*(largesty-y1Big) ) ) << endl;

#ifndef HCUBE_NOGUI
        if (drawContext)
            drawContext->SetDeviceOrigin(0,0);
#endif

        return max(0,30- ( (largestx-x1Big)*(largestx-x1Big) + (largesty-y1Big)*(largesty-y1Big) ) );
    }

    void FindClusterBiasExperiment::processGroup(shared_ptr<NEAT::GeneticGeneration> generation)
    {
        shared_ptr<NEAT::GeneticIndividual> individual = group.front();
        /*{
            mutex::scoped_lock scoped_lock(*Globals::ioMutex);
            cout << "Starting Evaluation on object:" << this << endl;
            cout << "Running on individual " << individual << endl;
        }*/

        //You get 10 points just for entering the game, wahooo!
        individual->setFitness(10);
        individual->setUserData(new FindClusterBiasStats());

        populateSubstrate(individual);

        double fitness=0;

        double maxFitness = 0;

        bool solved=true;

        for (int y1=1;y1<numNodesY;y1+=2)
        {
            for (int x1=1;x1<numNodesX;x1+=2)
            {
                int y1Big = (y1+numNodesY/2)%numNodesY;
                int x1Big = (x1+numNodesX/2)%numNodesX;

                if (y1Big==0)
                    y1Big++;
                else if (y1Big+1==numNodesY)
                    y1Big--;

                if (x1Big==0)
                    x1Big++;
                else if (x1Big+1==numNodesX)
                    x1Big--;

                if (x1>0&&x1+1<numNodesX)
                {
                    fitness += processEvaluation(individual,NULL,x1,y1,x1,y1Big);
                    maxFitness += 30;
#if FIND_CLUSTER_EXPERIMENT_DEBUG
                    cout << "Testing " << x1 << ',' << y1 << " and big " << x1 << ',' << y1Big << "\n";
#endif
                }
                else
                {
#if FIND_CLUSTER_EXPERIMENT_DEBUG
                    cout << "Tried to test " << x1 << ',' << y1 << " and big " << x1 << ',' << y1Big << " but big was out of range\n";
#endif
                }

                if (y1>0&&y1+1<numNodesY)
                {
                    fitness += processEvaluation(individual,NULL,x1,y1,x1Big,y1);
                    maxFitness += 30;
#if FIND_CLUSTER_EXPERIMENT_DEBUG
                    cout << "Testing " << x1 << ',' << y1 << " and big " << x1Big << ',' << y1 << "\n";
#endif
                }
                else
                {
#if FIND_CLUSTER_EXPERIMENT_DEBUG
                    cout << "Tried to test " << x1 << ',' << y1 << " and big " << x1Big << ',' << y1 << " but big was out of range\n";
#endif
                }

                fitness += processEvaluation(individual,NULL,x1,y1,x1Big,y1Big);
#if FIND_CLUSTER_EXPERIMENT_DEBUG
                cout << "Testing " << x1 << ',' << y1 << " and big " << x1Big << ',' << y1Big << "\n";
#endif
                maxFitness += 30;
#if FIND_CLUSTER_EXPERIMENT_DEBUG
                CREATE_PAUSE(string("Error!: ")+toString(__LINE__));
#endif

            }
        }

        //cout << "Individual Evaluation complete!\n";

        //cout << maxFitness << endl;

        individual->reward(fitness);

        if (fitness >= maxFitness*.95)
        {
            cout << "PROBLEM DOMAIN SOLVED!!!\n";
        }
    }

    void FindClusterBiasExperiment::processIndividualPostHoc(shared_ptr<NEAT::GeneticIndividual> individual)
    {
        /*{
            mutex::scoped_lock scoped_lock(*Globals::ioMutex);
            cout << "Starting Evaluation on object:" << this << endl;
            cout << "Running on individual " << individual << endl;
        }*/

        //You get 10 points just for entering the game, wahooo!
        individual->setFitness(10);
        individual->setUserData(new FindClusterBiasStats());


        populateSubstrate(individual);

        double fitness=0;

        double maxFitness = 0;

        bool solved=true;

        for (int y1=0;y1<numNodesY;y1+=sizeMultiplier)
        {
            //cout << '*';
            for (int x1=0;x1<numNodesX;x1+=sizeMultiplier)
            {
                //cout << '_';
                for (int y1Big=0;y1Big<numNodesY;y1Big+=sizeMultiplier)
                {
                    //cout << '.';
                    for (int x1Big=0;x1Big<numNodesX;x1Big+=sizeMultiplier)
                    {
                        int smallRadius;
                        int bigRadius;

                        if (sizeMultiplier==1)
                        {
                            smallRadius=0;
                            bigRadius=1;
                        }
                        else if (sizeMultiplier==3)
                        {
                            smallRadius=1;
                            bigRadius=4;
                        }
                        else if (sizeMultiplier==5)
                        {
                            smallRadius=2;
                            bigRadius=7;
                        }
                        else if (sizeMultiplier==9)
                        {
                            smallRadius=4;
                            bigRadius=13;
                        }
                        else
                        {
                            throw CREATE_LOCATEDEXCEPTION_INFO("Unsupported size multiplier!");
                        }

                        int dist = smallRadius+1+bigRadius+1;

                        if ( ((x1-x1Big)*(x1-x1Big) + (y1-y1Big)*(y1-y1Big)) < (dist*dist))
                            continue;

                        if (y1Big-bigRadius<0)
                            continue;
                        else if (y1Big+bigRadius>=numNodesY)
                            continue;

                        if (x1Big-bigRadius<0)
                            continue;
                        else if (x1Big+bigRadius>=numNodesX)
                            continue;

                        if (y1-smallRadius<0)
                            continue;
                        else if (y1+smallRadius>=numNodesY)
                            continue;

                        if (x1-smallRadius<0)
                            continue;
                        else if (x1+smallRadius>=numNodesX)
                            continue;

                        fitness += processEvaluation(individual,NULL,x1,y1,x1Big,y1Big);

                        maxFitness += 30;

                    }
                }
            }
        }

        //cout << "Individual Evaluation complete!\n";

        //cout << maxFitness << endl;

        individual->reward(fitness);

        if (fitness >= maxFitness*.95)
        {
            cout << "PROBLEM DOMAIN SOLVED!!!\n";
        }
    }

#ifndef HCUBE_NOGUI
    bool FindClusterBiasExperiment::handleMousePress(wxMouseEvent& event,wxSize &bitmapSize)
    {
        wxPoint clickPoint = event.GetPosition();

        int halfSizeX = bitmapSize.x/2;
        int halfSizeY = bitmapSize.y/2;

        cout << "SIZES " << endl;

        cout << clickPoint.x << ',' << clickPoint.y << endl;

        cout << bitmapSize.x << ',' << bitmapSize.y << endl;

        switch (event.GetButton())
        {
        case wxMOUSE_BTN_LEFT:
        {
            if (
                clickPoint.x+60>bitmapSize.x &&
                clickPoint.y+20>bitmapSize.y
            )
            {
                increaseResolution();
            }
            else if (
                clickPoint.x+60>bitmapSize.x &&
                clickPoint.y-20<0
            )
            {
                decreaseResolution();
            }
            else if (
                clickPoint.x < 85 &&
                clickPoint.y < 20
            )
            {
                string directoryString;

                directoryString = string("images/")+experimentName+string("/");
                replace_all(directoryString," ","_");

                for (int a=0;a<directoryString.size();a++)
                {
                    if (directoryString[a]!='/'&&!isalnum(directoryString[a]))
                    {
                        directoryString[a]='_';
                    }
                }

                cout << directoryString << endl;

                if (!boost::filesystem::exists(directoryString))
                {
                    boost::filesystem::create_directory( directoryString );
                }

                {
                    userDisplayOutputs = false;

                    userDisplayOutputLinkWeights.x=-1;
                    userDisplayOutputLinkWeights.y=-1;

                    {
                        wxBitmap tmpBitmap(165,165);

                        {
                            wxMemoryDC tmpDC(tmpBitmap);

                            processEvaluation(
                                lastIndividualSeen,
                                &tmpDC,
                                user1.x,
                                user1.y,
                                user2.x,
                                user2.y
                            );

                            tmpDC.SelectObject(wxNullBitmap);
                        }

                        tmpBitmap.SaveFile(
                            directoryString+string("Lowres_Input.png"),
                            wxBITMAP_TYPE_PNG
                        );
                    }

                    userDisplayOutputLinkWeights.x=user1.x;
                    userDisplayOutputLinkWeights.y=user1.y;

                    {
                        wxBitmap tmpBitmap(165,165);

                        {
                            wxMemoryDC tmpDC(tmpBitmap);

                            processEvaluation(
                                lastIndividualSeen,
                                &tmpDC,
                                user1.x,
                                user1.y,
                                user2.x,
                                user2.y
                            );

                            tmpDC.SelectObject(wxNullBitmap);
                        }

                        tmpBitmap.SaveFile(
                            directoryString+string("Lowres_LinkWeights1.png"),
                            wxBITMAP_TYPE_PNG
                        );
                    }

                    userDisplayOutputLinkWeights.x=user2.x;
                    userDisplayOutputLinkWeights.y=user2.y;

                    {
                        wxBitmap tmpBitmap(165,165);

                        {
                            wxMemoryDC tmpDC(tmpBitmap);

                            processEvaluation(
                                lastIndividualSeen,
                                &tmpDC,
                                user1.x,
                                user1.y,
                                user2.x,
                                user2.y
                            );

                            tmpDC.SelectObject(wxNullBitmap);
                        }

                        tmpBitmap.SaveFile(
                            directoryString+string("Lowres_LinkWeights2.png"),
                            wxBITMAP_TYPE_PNG
                        );
                    }

                    userDisplayOutputLinkWeights.x=-1;
                    userDisplayOutputLinkWeights.y=-1;

                    userDisplayOutputs = true;

                    {
                        wxBitmap tmpBitmap(165,165);

                        {
                            wxMemoryDC tmpDC(tmpBitmap);

                            processEvaluation(
                                lastIndividualSeen,
                                &tmpDC,
                                user1.x,
                                user1.y,
                                user2.x,
                                user2.y
                            );

                            tmpDC.SelectObject(wxNullBitmap);
                        }

                        tmpBitmap.SaveFile(
                            directoryString+string("Lowres_Output.png"),
                            wxBITMAP_TYPE_PNG
                        );
                    }

                    increaseResolution();

                    userDisplayOutputs = false;

                    {
                        wxBitmap tmpBitmap(165,165);

                        {
                            wxMemoryDC tmpDC(tmpBitmap);

                            processEvaluation(
                                lastIndividualSeen,
                                &tmpDC,
                                user1.x,
                                user1.y,
                                user2.x,
                                user2.y
                            );

                            tmpDC.SelectObject(wxNullBitmap);
                        }

                        tmpBitmap.SaveFile(
                            directoryString+string("Midres_Input.png"),
                            wxBITMAP_TYPE_PNG
                        );
                    }

                    userDisplayOutputLinkWeights.x=user1.x;
                    userDisplayOutputLinkWeights.y=user1.y;

                    {
                        wxBitmap tmpBitmap(165,165);

                        {
                            wxMemoryDC tmpDC(tmpBitmap);

                            processEvaluation(
                                lastIndividualSeen,
                                &tmpDC,
                                user1.x,
                                user1.y,
                                user2.x,
                                user2.y
                            );

                            tmpDC.SelectObject(wxNullBitmap);
                        }

                        tmpBitmap.SaveFile(
                            directoryString+string("Midres_LinkWeights1.png"),
                            wxBITMAP_TYPE_PNG
                        );
                    }

                    userDisplayOutputLinkWeights.x=user2.x;
                    userDisplayOutputLinkWeights.y=user2.y;

                    {
                        wxBitmap tmpBitmap(165,165);

                        {
                            wxMemoryDC tmpDC(tmpBitmap);

                            processEvaluation(
                                lastIndividualSeen,
                                &tmpDC,
                                user1.x,
                                user1.y,
                                user2.x,
                                user2.y
                            );

                            tmpDC.SelectObject(wxNullBitmap);
                        }

                        tmpBitmap.SaveFile(
                            directoryString+string("Midres_LinkWeights2.png"),
                            wxBITMAP_TYPE_PNG
                        );
                    }

                    userDisplayOutputLinkWeights.x=-1;
                    userDisplayOutputLinkWeights.y=-1;

                    userDisplayOutputs = true;

                    {
                        wxBitmap tmpBitmap(165,165);

                        {
                            wxMemoryDC tmpDC(tmpBitmap);

                            processEvaluation(
                                lastIndividualSeen,
                                &tmpDC,
                                user1.x,
                                user1.y,
                                user2.x,
                                user2.y
                            );

                            tmpDC.SelectObject(wxNullBitmap);
                        }

                        tmpBitmap.SaveFile(
                            directoryString+string("Midres_Output.png"),
                            wxBITMAP_TYPE_PNG
                        );
                    }

                    userDisplayOutputs = false;

                    increaseResolution();

                    userDisplayOutputs = false;

                    {
                        wxBitmap tmpBitmap(165,165);

                        {
                            wxMemoryDC tmpDC(tmpBitmap);

                            processEvaluation(
                                lastIndividualSeen,
                                &tmpDC,
                                user1.x,
                                user1.y,
                                user2.x,
                                user2.y
                            );

                            tmpDC.SelectObject(wxNullBitmap);
                        }

                        tmpBitmap.SaveFile(
                            directoryString+string("Highres_Input.png"),
                            wxBITMAP_TYPE_PNG
                        );
                    }

                    userDisplayOutputLinkWeights.x=user1.x;
                    userDisplayOutputLinkWeights.y=user1.y;

                    {
                        wxBitmap tmpBitmap(165,165);

                        {
                            wxMemoryDC tmpDC(tmpBitmap);

                            processEvaluation(
                                lastIndividualSeen,
                                &tmpDC,
                                user1.x,
                                user1.y,
                                user2.x,
                                user2.y
                            );

                            tmpDC.SelectObject(wxNullBitmap);
                        }

                        tmpBitmap.SaveFile(
                            directoryString+string("Highres_LinkWeights1.png"),
                            wxBITMAP_TYPE_PNG
                        );
                    }

                    userDisplayOutputLinkWeights.x=user2.x;
                    userDisplayOutputLinkWeights.y=user2.y;

                    {
                        wxBitmap tmpBitmap(165,165);

                        {
                            wxMemoryDC tmpDC(tmpBitmap);

                            processEvaluation(
                                lastIndividualSeen,
                                &tmpDC,
                                user1.x,
                                user1.y,
                                user2.x,
                                user2.y
                            );

                            tmpDC.SelectObject(wxNullBitmap);
                        }

                        tmpBitmap.SaveFile(
                            directoryString+string("Highres_LinkWeights2.png"),
                            wxBITMAP_TYPE_PNG
                        );
                    }

                    userDisplayOutputLinkWeights.x=-1;
                    userDisplayOutputLinkWeights.y=-1;

                    userDisplayOutputs = true;

                    {
                        wxBitmap tmpBitmap(165,165);

                        {
                            wxMemoryDC tmpDC(tmpBitmap);

                            processEvaluation(
                                lastIndividualSeen,
                                &tmpDC,
                                user1.x,
                                user1.y,
                                user2.x,
                                user2.y
                            );

                            tmpDC.SelectObject(wxNullBitmap);
                        }

                        tmpBitmap.SaveFile(
                            directoryString+string("Highres_Output.png"),
                            wxBITMAP_TYPE_PNG
                        );
                    }

                    userDisplayOutputs = false;

                    decreaseResolution();

                    decreaseResolution();
                }


            }
            else if (
                clickPoint.x<85 &&
                clickPoint.y+20>bitmapSize.y
            )
            {
                cout << "TODO: Make Single Save function\n";
            }
            else
            {
                cout << "Got a move-block request!\n";

                if (clicked1Last)
                {
                    Vector2<int> oldUser2 = user2;

                    user2.x = clickPoint.x;
                    user2.y = clickPoint.y;
                    user2.x += CELL_SIZE/2;
                    user2.y += CELL_SIZE/2;
                    user2.x-=halfSizeX;
                    user2.y-=halfSizeY;
                    user2.x= int(floor(double(user2.x)/CELL_SIZE));
                    user2.y= int(floor(double(user2.y)/CELL_SIZE));
                    user2.x+=numNodesX/2;
                    user2.y+=numNodesY/2;

                    cout << user2.x << ',' << user2.y << " <-- CLICK TARGET 2!\n";

                    if (
                        user2.x>numNodesX ||
                        user2.x<0 ||
                        user2.y>numNodesY ||
                        user2.y<0
                    )
                    {
                        user2 = oldUser2;
                    }
                    else
                    {
                        cout << "Updating block!\n";
                        clicked1Last = !clicked1Last;
                    }
                }
                else
                {
                    Vector2<int> oldUser1 = user1;

                    user1.x = clickPoint.x;
                    user1.y = clickPoint.y;
                    user1.x-=halfSizeX;
                    user1.y-=halfSizeY;
                    user1.x += CELL_SIZE/2;
                    user1.y += CELL_SIZE/2;
                    user1.x= int(floor(double(user1.x)/CELL_SIZE));
                    user1.y= int(floor(double(user1.y)/CELL_SIZE));
                    user1.x+=numNodesX/2;
                    user1.y+=numNodesY/2;

                    cout << user1.x << ',' << user1.y << " <-- CLICK TARGET 1!\n";

                    if (
                        user1.x>numNodesX ||
                        user1.x<0 ||
                        user1.y>numNodesY ||
                        user1.y<0
                    )
                    {
                        user1 = oldUser1;
                    }
                    else
                    {
                        cout << "Updating block!\n";
                        clicked1Last = !clicked1Last;
                    }
                }

            }
        }
        break;
        case wxMOUSE_BTN_RIGHT:
        {
            wxPoint tmpPoint;
            tmpPoint.x = clickPoint.x;
            tmpPoint.y = clickPoint.y;
            tmpPoint.x-=halfSizeX;
            tmpPoint.y-=halfSizeY;
            tmpPoint.x += CELL_SIZE/2;
            tmpPoint.y += CELL_SIZE/2;
            tmpPoint.x= int(floor(double(tmpPoint.x)/CELL_SIZE));
            tmpPoint.y= int(floor(double(tmpPoint.y)/CELL_SIZE));
            tmpPoint.x+=numNodesX/2;
            tmpPoint.y+=numNodesY/2;

            cout << "Clicked on " << tmpPoint.x << ',' << tmpPoint.y << endl;

            if (tmpPoint.x==userDisplayOutputLinkWeights.x&&tmpPoint.y==userDisplayOutputLinkWeights.y)
            {
                userDisplayOutputLinkWeights.x=-1;
                userDisplayOutputLinkWeights.y=-1;
            }
            else
            {
                userDisplayOutputLinkWeights.x = tmpPoint.x;
                userDisplayOutputLinkWeights.y = tmpPoint.y;
            }

            userDisplayOutputs = false;
        }
        break;
        case wxMOUSE_BTN_MIDDLE:
        {
            cout << "TOGGLING USERDISPLAYOUTPUTS\n";
            userDisplayOutputs = !userDisplayOutputs;
        }
        break;
        }

        return true;
    }

    bool FindClusterBiasExperiment::handleMouseMotion(wxMouseEvent& event,wxDC &temp_dc,shared_ptr<NEAT::GeneticIndividual> individual)
    {
        wxPoint clickPoint = event.GetPosition();

        int halfSizeX = temp_dc.GetSize().x/2;
        int halfSizeY = temp_dc.GetSize().y/2;

        wxPoint tmpPoint;
        tmpPoint.x = clickPoint.x;
        tmpPoint.y = clickPoint.y;
        tmpPoint.x-=halfSizeX;
        tmpPoint.y-=halfSizeY;
        tmpPoint.x += CELL_SIZE/2;
        tmpPoint.y += CELL_SIZE/2;
        tmpPoint.x= int(floor(double(tmpPoint.x)/CELL_SIZE));
        tmpPoint.y= int(floor(double(tmpPoint.y)/CELL_SIZE));
        tmpPoint.x+=numNodesX/2;
        tmpPoint.y+=numNodesY/2;

        if (tmpPoint.x<0||tmpPoint.y<0||tmpPoint.x>=numNodesX||tmpPoint.y>=numNodesY)
        {
            //Out of range
            return false;
        }

        if (userDisplayOutputLinkWeights.x>=0)
        {
            //cout << "Looking at link weight outputs\n";
            if (tmpPoint.x!=userDisplayOutputLinkWeights.x||tmpPoint.y!=userDisplayOutputLinkWeights.y)
            {
                userDisplayOutputLinkWeights.x = tmpPoint.x;
                userDisplayOutputLinkWeights.y = tmpPoint.y;
                createIndividualImage(temp_dc,individual);
                return true;
            }
        }

        return false;
    }

    void FindClusterBiasExperiment::createIndividualImage(wxDC &drawContext,shared_ptr<NEAT::GeneticIndividual> individual)
    {
        individual->setUserData(new FindClusterBiasStats());

        try
        {
            if (lastIndividualSeen!=individual)
            {
                cout << "Repopulating substrate\n";
                populateSubstrate(individual);
                lastIndividualSeen = individual;
            }

            {
                //cout << "Performing evaluation.  time: ";
                //progress_timer t;
                processEvaluation(
                    individual,
                    &drawContext,
                    user1.x,
                    user1.y,
                    user2.x,
                    user2.y
                );
            }
            //cout << endl;

            wxCoord w, h;
            drawContext.GetSize(&w, &h);

            drawContext.DrawText(
                "Save images",
                2,
                2
            );

            drawContext.DrawText(
                "Inc. Res",
                w-60,
                h-20
            );

            drawContext.DrawText(
                "Dec. Res",
                w-60,
                0
            );
        }
        catch (string s)
        {
            cout << "ERROR: " << s << endl;
        }
    }
#endif

    void FindClusterBiasExperiment::increaseResolution()
    {
        if (sizeMultiplier==1)
        {
            numNodesX *= 3; //((numNodesX-1)*2)+1;
            numNodesY *= 3; //((numNodesY-1)*2)+1;
            sizeMultiplier*=3;
            CELL_SIZE/=3;
            user1.x*=3;
            user1.y*=3;
            user2.x*=3;
            user2.y*=3;
            generateSubstrate();
            if (lastIndividualSeen)
                populateSubstrate(lastIndividualSeen);
        }
        else if (sizeMultiplier==3)
        {
            numNodesX = (numNodesX*5)/3; //((numNodesX-1)*2)+1;
            numNodesY = (numNodesY*5)/3; //((numNodesY-1)*2)+1;
            sizeMultiplier= (sizeMultiplier*5)/3;
            CELL_SIZE =(CELL_SIZE*3)/5;
            user1.x = (user1.x*5)/3;
            user1.y = (user1.y*5)/3;
            user2.x = (user2.x*5)/3;
            user2.y = (user2.y*5)/3;
            generateSubstrate();
            if (lastIndividualSeen)
                populateSubstrate(lastIndividualSeen);
        }
    }

    void FindClusterBiasExperiment::decreaseResolution()
    {
        if (sizeMultiplier==3)
        {
            numNodesX /= 3;//((numNodesX-1)/2)+1;
            numNodesY /= 3;//((numNodesY-1)/2)+1;
            sizeMultiplier/=3;
            CELL_SIZE*=3;
            user1.x/=3;
            user1.y/=3;
            user2.x/=3;
            user2.y/=3;
            generateSubstrate();
            if (lastIndividualSeen)
                populateSubstrate(lastIndividualSeen);
        }
        else if (sizeMultiplier==5)
        {
            numNodesX = (numNodesX*3)/5; //((numNodesX-1)*2)+1;
            numNodesY = (numNodesY*3)/5; //((numNodesY-1)*2)+1;
            sizeMultiplier= (sizeMultiplier*3)/5;
            CELL_SIZE =(CELL_SIZE*5)/3;
            user1.x = (user1.x*3)/5;
            user1.y = (user1.y*3)/5;
            user2.x = (user2.x*3)/5;
            user2.y = (user2.y*3)/5;
            generateSubstrate();
            if (lastIndividualSeen)
                populateSubstrate(lastIndividualSeen);
        }
    }

    Experiment* FindClusterBiasExperiment::clone()
    {
        FindClusterBiasExperiment* experiment = new FindClusterBiasExperiment(*this);

        return experiment;
    }

}
