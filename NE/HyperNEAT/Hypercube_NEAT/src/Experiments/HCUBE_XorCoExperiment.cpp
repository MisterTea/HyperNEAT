#include "HCUBE_Defines.h"

#include "Experiments/HCUBE_XorCoExperiment.h"

#ifdef EPLEX_INTERNAL

using namespace NEAT;

namespace HCUBE
{
    XorCoExperiment::XorCoExperiment(string _experimentName,int _threadID)
            :
            Experiment(_experimentName,_threadID)
    {}

    NEAT::GeneticPopulation* XorCoExperiment::createInitialPopulation(int populationSize)
    {
        GeneticPopulation *population = new GeneticPopulation(
            shared_ptr<XorCoExperiment>((XorCoExperiment*)this->clone())
        );
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

        shared_ptr<NEAT::CoEvoGeneticGeneration> coEvoGeneration =
            static_pointer_cast<NEAT::CoEvoGeneticGeneration>(population->getGeneration());

        //Create the tests
        for (int a=0;a<(population->getIndividualCount()/10);a++)
        {
            shared_ptr<NEAT::GeneticIndividual> individual(new GeneticIndividual(genes,true,1.0));

            //This function clones the individual to make a test
            coEvoGeneration->addTest(individual);
        }

        coEvoGeneration->bootstrap();

        cout << "Double-Check\n";
        for (int a=0;a<coEvoGeneration->getTestCount();a++)
        {
            cout << "Test fitness: " << coEvoGeneration->getTest(a)->getFitness() << endl;
        }
        cout << "done!\n";

        cout << "Finished creating population\n";
        return population;
    }

    pair<double,double> XorCoExperiment::playGame(
        shared_ptr<GeneticIndividual> firstPlayer,
        shared_ptr<GeneticIndividual> secondPlayer
    )
    {
        double fit1=10.0,fit2=10.0;

        double score1=0.0,score2=0.0;

        {
            NEAT::FastNetwork<float> network = firstPlayer->spawnFastPhenotypeStack<float>();

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

                    score1 += 500*(2-fabs(value-expectedValue));

                }
            }
        }

        {
            NEAT::FastNetwork<float> network = secondPlayer->spawnFastPhenotypeStack<float>();

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

                    score2 += 500*(2-fabs(value-expectedValue));

                }
            }
        }

        if (score1 > score2)
        {
            //firstPlayer->reward(fit1);
            //secondPlayer->reward(fit2/2.0);
            fit1 += 10000;
            fit2 += 2000;
        }
        else if (score1 < score2)
        {
            //firstPlayer->reward(fit1/2.0);
            //secondPlayer->reward(fit2);
            fit1 += 2000;
            fit2 += 10000;
        }
        else //tie
        {
            //firstPlayer->reward(fit1/2.0);
            //secondPlayer->reward(fit2/2.0);
            fit1 += 2000;
            fit2 += 2000;
        }

        return pair<double,double>(fit1,fit2);
    }

    void XorCoExperiment::processGroup(shared_ptr<NEAT::GeneticGeneration> generation)
    {
        shared_ptr<NEAT::CoEvoGeneticGeneration> coEvoGeneration =
            static_pointer_cast<NEAT::CoEvoGeneticGeneration>(generation);

        shared_ptr<GeneticIndividual> individual = group[0];

        individual->setFitness(0);

        //Play all the tests, but do not change test fitness
        for (int a=0;a<coEvoGeneration->getTestCount();a++)
        {
            pair<double,double> rewards = playGame(
                                              individual,
                                              coEvoGeneration->getTest(a)
                                          );

            individual->reward(rewards.first);
        }

        //Now, an individual plays all the test cases but they only play each other,
        //so they have one less game, so scale the fitness accordingly.
        double scaledFitness = ((double)individual->getFitness()*(coEvoGeneration->getTestCount()-1))/double(coEvoGeneration->getTestCount());

        individual->setFitness(scaledFitness);
    }

    void XorCoExperiment::processIndividualPostHoc(shared_ptr<NEAT::GeneticIndividual> individual)
    {
        NEAT::FastNetwork<float> network = individual->spawnFastPhenotypeStack<float>();

        //TODOL Put in userdata

        double fitness = 10.0;
        double maxFitness = 10.0;

        cout << "POST HOC ANALYSIS:" << endl;

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

                fitness += (500*(2-fabs(value-expectedValue)));
                cout << (500*(2-fabs(value-expectedValue))) << endl;
                maxFitness += 500*2;
            }
        }
        cout << "Sum: " << fitness << endl;
        cout << endl;

    }

    Experiment* XorCoExperiment::clone()
    {
        XorCoExperiment* experiment = new XorCoExperiment(*this);

        return experiment;
    }

    void XorCoExperiment::addGenerationData(shared_ptr<NEAT::GeneticGeneration> generation,shared_ptr<NEAT::GeneticIndividual> individual)
    {
    }
}

#endif
