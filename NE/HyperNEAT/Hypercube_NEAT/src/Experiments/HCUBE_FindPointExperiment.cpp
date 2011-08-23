#include "HCUBE_Defines.h"

#include "Experiments/HCUBE_FindPointExperiment.h"

namespace HCUBE
{
    using namespace NEAT;

    NEAT::GeneticPopulation* FindPointExperiment::createInitialPopulation(int populationSize)
    {
        cout << "CREATING POPULATION\n";
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
        numNodesX=11;
        vector<NEAT::NetworkNode *> nodes;
        vector<NEAT::NetworkLink *> links;

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

        substrate = NEAT::FastNetwork<double>(nodes,links);

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


        return population;
    }

    void FindPointExperiment::processGroup(shared_ptr<NEAT::GeneticGeneration> generation)
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
                                substrate.getLink(counter)->weight = ( ((greyVal-0.2)/0.8)*3.0 );
                            else
                                substrate.getLink(counter)->weight = ( ((greyVal+0.2)/0.8)*3.0 );
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

        double fitness=0;

        double maxFitness = 0;

        bool solved=true;

        for (int y1=0;y1<numNodesY;y1+=2)
        {
            for (int x1=0;x1<numNodesX;x1+=2)
            {
                substrate.reinitialize();
                substrate.dummyActivation();

                substrate.setValue(toString(y1-numNodesY/2)+string("/")+toString(x1-numNodesX/2),1.0);

                substrate.updateFixedIterations(1);

                double largestValue = -INT_MAX;
                int largesty,largestx;

                for (int y2=0;y2<numNodesY;y2++)
                {
                    for (int x2=0;x2<numNodesX;x2++)
                    {
                        double value = substrate.getValue(toString(y2-numNodesY/2)+string("/")+toString(x2-numNodesX/2));
                        if (value > largestValue)
                        {
                            largestValue = value;
                            largestx = x2;
                            largesty = y2;
                        }
                    }
                }

                fitness += max(0,30- ( (largestx-x1)*(largestx-x1) + (largesty-y1)*(largesty-y1) ) );

                maxFitness += 30;

            }
        }

        //cout << "Individual Evaluation complete!\n";

        //cout << maxFitness << endl;

        individual->reward(fitness);

        if (fitness >= maxFitness*.9)
        {
            cout << "PROBLEM DOMAIN SOLVED!!!\n";
        }
    }

#ifndef HCUBE_NOGUI
    void FindPointExperiment::createIndividualImage(wxDC &drawContext,shared_ptr<NEAT::GeneticIndividual> individual)
    {}
#endif

    Experiment* FindPointExperiment::clone()
    {
        FindPointExperiment* experiment = new FindPointExperiment(*this);

        return experiment;
    }

}
