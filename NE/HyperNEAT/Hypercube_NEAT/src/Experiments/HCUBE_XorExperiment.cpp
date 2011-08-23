#include "HCUBE_Defines.h"

#include "Experiments/HCUBE_XorExperiment.h"

using namespace NEAT;

namespace HCUBE
{
    XorExperiment::XorExperiment(string _experimentName,int _threadID)
            :
            Experiment(_experimentName,_threadID)
    {}

    NEAT::GeneticPopulation* XorExperiment::createInitialPopulation(int populationSize)
    {
        GeneticPopulation *population = new GeneticPopulation();
        vector<GeneticNodeGene> genes;

        genes.push_back(GeneticNodeGene("Bias","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("X1","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("X2","NetworkSensor",0,false));
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

        cout << "Finished creating population\n";
        return population;
    }

    void XorExperiment::processGroup(shared_ptr<NEAT::GeneticGeneration> generation)
    {
        NEAT::FastNetwork<float> network = group[0]->spawnFastPhenotypeStack<float>();

        group[0]->reward(10);

        for (int x1=0;x1<2;x1++)
        {
            for (int x2=0;x2<2;x2++)
            {
                network.reinitialize();

                network.setValue("X1",x1);
                network.setValue("X2",x2);
                network.setValue("Bias",0.3f);

                network.update();

                double value = network.getValue("Output");

                double expectedValue = (double)(x1 ^ x2);

                group[0]->reward(5000*(2-fabs(value-expectedValue)));
            }
        }
    }

    void XorExperiment::processIndividualPostHoc(shared_ptr<NEAT::GeneticIndividual> individual)
    {
        NEAT::FastNetwork<float> network = individual->spawnFastPhenotypeStack<float>();

        //TODO Put in userdata

        double fitness = 10.0;
        double maxFitness = 10.0;

        for (int x1=0;x1<2;x1++)
        {
            for (int x2=0;x2<2;x2++)
            {
                network.reinitialize();

                network.setValue("X1",x1);
                network.setValue("X2",x2);
                network.setValue("Bias",0.3f);

                network.update();

                double value = network.getValue("Output");

                double expectedValue = (double)(x1 ^ x2);

                fitness += (5000*(2-fabs(value-expectedValue)));
                maxFitness += 5000*2;
            }
        }

        cout << "POST HOC ANALYSIS: " << fitness << "/" << maxFitness << endl;
    }

    Experiment* XorExperiment::clone()
    {
        XorExperiment* experiment = new XorExperiment(*this);

        return experiment;
    }
}
