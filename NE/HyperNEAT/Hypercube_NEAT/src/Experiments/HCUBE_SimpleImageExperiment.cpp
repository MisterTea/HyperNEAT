#include "HCUBE_Defines.h"

#include "Experiments/HCUBE_SimpleImageExperiment.h"

namespace HCUBE
{
    using namespace NEAT;

    /*
    int testArray[3][5] =
        {
            {-1,     1,      -1,        -1,     1},
            {1,     1,      1,        -1,     -1},
            {-1,  -1,     -1,       1,      -1}
        };
    */

    NEAT::GeneticPopulation* SimpleImageExperiment::createInitialPopulation(int populationSize)
    {
        NEAT::GeneticPopulation* population = new NEAT::GeneticPopulation();

        vector<GeneticNodeGene> genes;

        genes.push_back(GeneticNodeGene("Bias","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("X1","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("Y1","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("X2","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("Y2","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("DeltaX","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("DeltaY","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("Output","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_LINEAR));

        for (int a=0;a<populationSize;a++)
        {
            shared_ptr<GeneticIndividual> individual(new GeneticIndividual(genes,true,1.0));

            for (int b=0;b<0;b++)
            {
                individual->testMutate();
            }

            population->addIndividual(individual);
        }

        numNodesY=11;
        numNodesX=5;
        vector<NEAT::NetworkNode *> nodes;
        vector<NEAT::NetworkLink *> links;

        for (int y1=0;y1<numNodesY;y1++)
        {
            for (int x1=0;x1<numNodesX;x1++)
            {
                //cout << (y1-numNodesY/2) << '/' << (x1-numNodesX/2) << endl;
                nodes.push_back(new NetworkNode(toString(y1-numNodesY/2)+string("/")+toString(x1-numNodesX/2)));

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

        for (int y1=0;y1<numNodesY;y1++)
        {
            for (int x1=0;x1<numNodesX;x1++)
            {
                for (int y2=0;y2<numNodesY;y2++)
                {
                    for (int x2=0;x2<numNodesX;x2++)
                    {
                        links.push_back(new NetworkLink(nodes[y1*numNodesX+x1],nodes[y2*numNodesX+x2],0.0));
                    }
                }
            }
        }

        network2 = NEAT::FastNetwork<double>(nodes,links);

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


        numNodesY=31;
        numNodesX=15;

        for (int y1=0;y1<numNodesY;y1++)
        {
            for (int x1=0;x1<numNodesX;x1++)
            {
                //cout << (y1-numNodesY/2) << '/' << (x1-numNodesX/2) << endl;
                nodes.push_back(new NetworkNode(toString(y1-numNodesY/2)+string("/")+toString(x1-numNodesX/2)));

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
            }
        }

        for (int y1=0;y1<numNodesY;y1++)
        {
            for (int x1=0;x1<numNodesX;x1++)
            {
                for (int y2=0;y2<numNodesY;y2++)
                {
                    for (int x2=0;x2<numNodesX;x2++)
                    {
                        links.push_back(new NetworkLink(nodes[y1*numNodesX+x1],nodes[y2*numNodesX+x2],0.0));
                    }
                }
            }
        }

        network3 = NEAT::FastNetwork<double>(nodes,links);

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

        numNodesY=11;
        numNodesX=5;

        return population;
    }

    double getFitness(double answer, int expectedResult)
    {
        switch (expectedResult)
        {
        case -1:
            return 5.0 * fabs(1.0 - answer);
        case 1:
            return 5.0 * fabs(-1.0 - answer);
        default:
            throw CREATE_LOCATEDEXCEPTION_INFO(string("Unsupported expectedResult: ")+toString(expectedResult));
        }
    }

    void SimpleImageExperiment::processGroup(shared_ptr<NEAT::GeneticGeneration> generation)
    {
        shared_ptr<NEAT::GeneticIndividual> individual = group.front();
        /*{
            cout << "Starting Evaluation on object:" << this << endl;
            cout << "Running on individual " << individual << endl;
        }*/

        //You get 10 points just for entering the game, wahooo!
        individual->setFitness(10);

        NEAT::FastNetwork<double> network = individual->spawnFastPhenotypeStack<double>();

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
                        network.setValue("DeltaX",x2Val-x1Val);
                        network.setValue("DeltaY",y2Val-y1Val);
                        network.setValue("Bias",0.3);
                        network.update();

                        double greyVal = ((((network.getValue("Output")))))*3.0;

                        if (fabs(greyVal)>0.2)
                        {
                            if (greyVal>0.0)
                                network2.getLink(counter)->weight = ( ((greyVal-0.2)/0.8)*3.0 );
                            else
                                network2.getLink(counter)->weight = ( ((greyVal+0.2)/0.8)*3.0 );
                        }
                        else
                        {
                            network2.getLink(counter)->weight = (0.0);
                        }

                        counter++;
                    }
                }
            }
        }

        double answer;

        int maxFitness=0;

        for (int inputString=0;inputString<32;inputString++)
            //int inputString=18;
        {

            network2.reinitialize();
            network2.dummyActivation();

            for (int y1=((numNodesY/2));y1<(numNodesY/2)+1;y1++)
            {
                for (int x1=0;x1<numNodesX;x1++)
                {
                    int value = HASBIT(inputString,x1)*2-1;
                    network2.setValue(toString(y1)+"/"+toString(x1-numNodesX/2) , value );
                }
            }

            for (int a=0;a<=1;a++)
            {
                if (a>0)
                {
                    //only run if we've had a chance to update at least once
                    for (int y1=((numNodesY/2));y1<numNodesY/2+1;y1++)
                    {
                        for (int x1=0;x1<numNodesX;x1++)
                        {
                            answer = network2.getValue(toString(-y1)+"/"+toString(x1-numNodesX/2));

                            double fitness = getFitness(answer,HASBIT(inputString,x1)*2-1);

                            individual->reward( fitness );

                            maxFitness += 100;

                            if (fitness<90.0)
                            {
                                //failed[0]=true;
                            }
                        }
                    }
                }

                network2.update();
            }

        }

        //individual->setFitness(individual->getFitness()*individual->getFitness());
        //cout << maxFitness << endl;

        /*network2.reinitialize();
        network2.dummyActivation();

        for (int a=0;a<=10;a++)
        {
            if (a)
            {
                //only run if we've had a chance to update at least once
                for (int y1=(numNodesY/2);y1<numNodesY/2+1;y1++)
                {
                    for (int x1=0;x1<numNodesX;x1++)
                    {
                        network2.getNode(toString(y1)+"/"+toString(x1-numNodesX/2))->setValue( testArray[1][x1] );
                    }
                }

                for (int y1=(numNodesY/2);y1<numNodesY/2+1;y1++)
                {
                    for (int x1=0;x1<numNodesX;x1++)
                    {
                        answer = network2.getNode(toString(-y1)+"/"+toString(x1-numNodesX/2))->getValue();

                        double fitness = getFitness(answer,testArray[1][x1]);

                        individual->reward( fitness );

                        if (fitness<90.0)
                        {
                            failed[1]=true;
                        }
                    }
                }
            }

            network2.update(1);
        }

        network2.reinitialize();
        network2.dummyActivation();

        for (int a=0;a<=10;a++)
        {
            if (a)
            {
                //only run if we've had a chance to update at least once
                for (int y1=(numNodesY/2);y1<numNodesY/2+1;y1++)
                {
                    for (int x1=0;x1<numNodesX;x1++)
                    {
                        network2.getNode(toString(y1)+"/"+toString(x1-numNodesX/2))->setValue( testArray[2][x1] );
                    }
                }

                for (int y1=(numNodesY/2);y1<numNodesY/2+1;y1++)
                {
                    for (int x1=0;x1<numNodesX;x1++)
                    {
                        answer = network2.getNode(toString(-y1)+"/"+toString(x1-numNodesX/2))->getValue();

                        double fitness = getFitness(answer,testArray[2][x1]);

                        individual->reward( fitness );

                        if (fitness<90.0)
                        {
                            failed[2]=true;
                        }
                    }
                }
            }

            network2.update(1);
        }*/


        /*if ((failed[0]+failed[1]+failed[2]) == 0)
        {
            cout << "INDIVIDUAL " << ind << " PASSED EVERY TEST!\n";
        }
        if ((failed[0]+failed[1]+failed[2]) == 1)
        {
            cout << "INDIVIDUAL " << ind << " ALMOST PASSED! : " << failed[0] << ' ' << failed[1] << ' ' << failed[2] << endl;
        }*/

        //delete network;

        /*GeneticIndividual *bestIndividual = population->getBestIndividualOfGeneration();

        for (int a=0;a<=10;a++)
        {
            setRenderIterations(a);

            RenderJob newJob(
                bestIndividual,
                2,
                localBuffer[a],
                dimension,
                1,
                &imageChanged[a]
            );

            createIndividualImage(newJob);
        }*/

        //cout << "DONE EVALUATION!\n";
    }

#ifndef HCUBE_NOGUI
    void SimpleImageExperiment::createIndividualImage(wxDC &drawContext,shared_ptr<NEAT::GeneticIndividual> individual)
    {
        try
        {
            cout << "Creating image...\n";
            drawContext.Clear();

            numNodesY = 31;
            numNodesX = 15;

            /*for (int y=0;y<job.dimension;y++)
            {
                for (int x=0;x<job.dimension;x++)
                {
                    job.localBuffer[y][x]=CL_Color(0,128,128);
                }
            }*/

            NEAT::FastNetwork<double> network = individual->spawnFastPhenotypeStack<double>();

            cout << "Creating Random Number\n";

            base_generator_type generator = base_generator_type(static_cast<unsigned int>(std::clock()%1000000000));

            int_gen_type move_gen = int_gen_type(generator, int_distribution_type(0, 31));

            int inputString = move_gen();

            cout << "TESTING INPUT STRING " << inputString << endl;

            int counter=0;
            double x1Val,y1Val,x2Val,y2Val;
            double answer;

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
                            network.setValue("DeltaX",x2Val-x1Val);
                            network.setValue("DeltaY",y2Val-y1Val);
                            network.setValue("Bias",0.3);
                            network.update();

                            double greyVal = network.getValue("Output");

                            if (fabs(greyVal)>0.2)
                            {
                                if (greyVal>0.0)
                                    network3.getLink(counter)->weight = ( ((greyVal-0.2)/0.8)*3.0 );
                                else
                                    network3.getLink(counter)->weight = ( ((greyVal+0.2)/0.8)*3.0 );
                            }
                            else
                            {
                                network3.getLink(counter)->weight = (0.0);
                            }

                            counter++;
                        }
                    }
                }
            }

            network3.reinitialize();
            network3.dummyActivation();

            for (int a=0;a<=1;a++)
            {
                drawContext.SetDeviceOrigin(100*((a%6)+(a>=6?1:0)),120*(a/6));

                for (int y1=((numNodesY/2));y1<(numNodesY/2)+1;y1++)
                {
                    for (int x1=0;x1<numNodesX;x1++)
                    {
                        //cout << "Setting value for " << (toString(y1)+"/"+toString(x1-numNodesX/2)) << endl;
                        network3.setValue(toString(y1)+"/"+toString(x1-numNodesX/2) , HASBIT(inputString,x1/3)*2-1 );
                    }
                }


                for (int y1=(-(numNodesY/2));y1<=(numNodesY/2);y1++)
                {
                    for (int x1=-(numNodesX/2);x1<=(numNodesX/2);x1++)
                    {
                        answer = network3.getValue(toString(y1)+"/"+toString(x1));

                        wxColour result;

                        if (answer<0)
                            result = wxColour(int(fabs(answer)*255),0,0);
                        else
                            result = wxColour(int(answer*255),int(answer*255),int(answer*255));

                        //cout << (x1+(numNodesX/2)) << ' ' << (y1+(numNodesY/2)) << ' ' << (job.dimension/12) << ' ' << answer << endl;
                        drawContext.SetBrush(wxBrush(result));
                        drawContext.DrawRectangle(5*(x1+(numNodesX/2)),5*(y1+(numNodesY/2)),5,5);
                        drawContext.SetBrush(wxNullBrush);
                    }
                }

                network3.update();
            }


            numNodesY = 11;
            numNodesX = 5;
            counter=0;

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
                            network.setValue("DeltaX",x2Val-x1Val);
                            network.setValue("DeltaY",y2Val-y1Val);
                            network.setValue("Bias",0.3);
                            network.update();

                            double greyVal = network.getValue("Output");

                            if (fabs(greyVal)>0.2)
                            {
                                if (greyVal>0.0)
                                    network2.getLink(counter)->weight = ( ((greyVal-0.2)/0.8)*3.0 );
                                else
                                    network2.getLink(counter)->weight = ( ((greyVal+0.2)/0.8)*3.0 );
                            }
                            else
                            {
                                network2.getLink(counter)->weight = (0.0);
                            }

                            counter++;
                        }
                    }
                }
            }

            network2.reinitialize();
            network2.dummyActivation();

            for (int a=0;a<=1;a++)
            {
                drawContext.SetDeviceOrigin(300+100*((a%6)+(a>=6?1:0)),50);

                for (int y1=((numNodesY/2));y1<(numNodesY/2)+1;y1++)
                {
                    for (int x1=0;x1<numNodesX;x1++)
                    {
                        //cout << "Setting value for " << (toString(y1)+"/"+toString(x1-numNodesX/2)) << endl;
                        network2.setValue(toString(y1)+"/"+toString(x1-numNodesX/2) , HASBIT(inputString,(x1))*2-1 );
                    }
                }


                for (int y1=(-(numNodesY/2));y1<=(numNodesY/2);y1++)
                {
                    for (int x1=-(numNodesX/2);x1<=(numNodesX/2);x1++)
                    {
                        answer = network2.getValue(toString(y1)+"/"+toString(x1));

                        wxColour result;

                        if (answer<0)
                            result = wxColour(int(fabs(answer)*255),0,0);
                        else
                            result = wxColour(int(answer*255),int(answer*255),int(answer*255));

                        //cout << (x1+(numNodesX/2)) << ' ' << (y1+(numNodesY/2)) << ' ' << (job.dimension/12) << ' ' << answer << endl;
                        drawContext.SetBrush(wxBrush(result));
                        drawContext.DrawRectangle(10*(x1+(numNodesX/2)),10*(y1+(numNodesY/2)),10,10);
                        drawContext.SetBrush(wxNullBrush);
                    }
                }

                network2.update();
            }

        }
        catch (string s)
        {
            cout << "ERROR: " << s << endl;
            CREATE_PAUSE(string("Error!: ")+toString(__LINE__));
        }
        catch (...)
        {
            cout << "ERROR!\n";
            CREATE_PAUSE(string("Error!: ")+toString(__LINE__));
        }

    }
#endif

    Experiment* SimpleImageExperiment::clone()
    {
        SimpleImageExperiment* experiment = new SimpleImageExperiment(*this);

        return experiment;
    }

}
