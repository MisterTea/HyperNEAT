#include "HCUBE_Defines.h"

#include "Experiments/HCUBE_SpatialCasinoExperiment.h"

#define SPATIAL_CASINO_EXPERIMENT_DEBUG (0)

#define SPATIAL_USE_THETA (1)

#define SPATIAL_CASINO_USE_FALLOFF (1)

#define SPATIAL_CASINO_BASE_CASE (1)

namespace HCUBE
{
    using namespace NEAT;

    SpatialCasinoExperiment::SpatialCasinoExperiment(string _experimentName,int _threadID)
            :
            Experiment(_experimentName,_threadID)
    {
        randomProbabilitiesForPlay = (float*)malloc(sizeof(float)*TOTAL_RANDOM_NUMBERS);
        randomPayoffs = (float*)malloc(sizeof(float)*TOTAL_RANDOM_NUMBERS);

        /*
        NEAT::Random &random = NEAT::Globals::getSingleton()->getRandom();

        ofstream outfile("RandomNumbers.txt");

        for(int a=0;a<TOTAL_RANDOM_NUMBERS;a++)
        {
            if(a%100==0)
                cout << a << endl;
            //randomNumbers[a] = 
            outfile << random.getRandomWithinRange(0,100) << endl;
        }
        */

        base_generator_type generator(1000);

        real_distribution_type realDist(0,1);
        variate_generator<base_generator_type,real_distribution_type> realGen(generator,realDist);

        cout << "Generating random number table";
        for(int a=0;a<TOTAL_RANDOM_NUMBERS;a++)
        {
            if(a%100000==0)
                cout << '.';

            randomProbabilitiesForPlay[a] = realGen();
            randomPayoffs[a] = realGen();
        }
        cout << "Done!\n";
    }

    SpatialCasinoExperiment::~SpatialCasinoExperiment()
    {
        if(randomProbabilitiesForPlay)
            free(randomProbabilitiesForPlay);

        if(randomPayoffs)
            free(randomPayoffs);
    }

    NEAT::GeneticPopulation* SpatialCasinoExperiment::createInitialPopulation(int populationSize)
    {
        NEAT::GeneticPopulation* population = new NEAT::GeneticPopulation();

        vector<GeneticNodeGene> genes;

        genes.push_back(GeneticNodeGene("Bias","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("Input","NetworkSensor",0,false));

        genes.push_back(GeneticNodeGene("Output","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));

        for (int a=0;a<populationSize;a++)
        {
            shared_ptr<GeneticIndividual> individual(new GeneticIndividual(genes,true,1.0));

            for (int b=0;b<3;b++)
            {
                individual->testMutate();
            }

            population->addIndividual(individual);
        }

        return population;
    }

    void SpatialCasinoExperiment::processGroup(shared_ptr<NEAT::GeneticGeneration> generation)
    {
        shared_ptr<NEAT::GeneticIndividual> individual = group.front();

        substrate = individual->spawnFastPhenotypeStack<double>();

        int randomIndex = 0;
        int randomPayoffIndex = 0;

        individual->setFitness(0.0);

        double output;

#if SPATIAL_CASINO_EXPERIMENT_DEBUG
        cout << "Creating payoff table:";
#endif
        for (int a=0;a<=PAYOFF_STEPS;a++)
        {
#if SPATIAL_CASINO_EXPERIMENT_DEBUG
            if (a%100==0)
            {
                cout << ".";
            }
#endif

            substrate.reinitialize();

            if (substrate.hasNode("Bias"))
            {
                substrate.setValue("Bias",0.3);
            }

            substrate.setValue("Input", (a-PAYOFF_STEPS/2)/((double)PAYOFF_STEPS/2) );

            substrate.update();

            output = substrate.getValue("Output");

            output *= 10.0;

            //output is now in the range [-10,10]

            if (output<0.0)
            {
                int flag=0;
            }

            //less than 0 means 0
            output = max(0.0,output);

            payoffFunction[a] = output;
        }
#if SPATIAL_CASINO_EXPERIMENT_DEBUG
        cout << "Done" << endl;
#endif

        for (int trials=0;trials<NUM_TRIALS;trials++)
        {
#if SPATIAL_CASINO_EXPERIMENT_DEBUG
            //if (trials%10==0)
            {
                cout << "On Trial " << trials << endl;
            }
#endif

            double bank=100000.0;

            for (int r=0;r<CASINO_ROWS;r++)
            {
                for (int c=0;c<CASINO_COLS;c++)
                {
#if SPATIAL_CASINO_BASE_CASE
                    probabilityForPlay[r][c] = 1.0f;
#else
                    probabilityForPlay[r][c] = 0.75f;
#endif
                }
            }

            for (int timeStep=0;timeStep<TRIAL_LENGTH;timeStep++)
            {
                for (int r=0;r<CASINO_ROWS;r++)
                {
                    for (int c=0;c<CASINO_COLS;c++)
                    {
                        if(randomIndex >= TOTAL_RANDOM_NUMBERS || randomPayoffIndex >= TOTAL_RANDOM_NUMBERS)
                        {
                            throw CREATE_LOCATEDEXCEPTION_INFO("WTF");
                        }

                        //Get an input (between 0 and 1)
                        float input = randomPayoffs[randomPayoffIndex++];

                        if (randomProbabilitiesForPlay[randomIndex++] < probabilityForPlay[r][c])
                        {
                            //The patron wishes to play

                            //Scale the input to the payoff array
                            int index = int(input*PAYOFF_STEPS);

                            if (index<0||index>=PAYOFF_STEPS)
                            {
                                throw CREATE_LOCATEDEXCEPTION_INFO("RANGE ERROR!");
                            }

                            double output = payoffFunction[index];

                            //output = 0.2; //force max fitness

                            //Pay out (or collect)
                            bank += (1.0 - output);

                            //Provide feedback for AI

#if SPATIAL_CASINO_BASE_CASE
                            if(output < 0.2)
                            {
                             //People are really really discouraged if they do not win at least 0.2
                             probabilityForPlay[r][c]=0.0;
                            }
#else
                            //Distribute feedback to neighbors
                            int neighborCount=0;
                            for (int deltar = -2;deltar <= 2;deltar++)
                            {
                                for (int deltac = -2;deltac <= 2;deltac++)
                                {
                                    int newr = r+deltar;
                                    int newc = c+deltac;

                                    if (
                                        newr<0 ||
                                        newc<0 ||
                                        newr>=CASINO_ROWS ||
                                        newc>=CASINO_COLS)
                                    {
                                        //Out of bounds
                                        continue;
                                    }

                                    bool isActualPlayer = (!deltar) && (!deltac);

                                    if(!isActualPlayer)
                                    {
                                        neighborCount++;
                                    }
                                }
                            }

                            for (int deltar = -2;deltar <= 2;deltar++)
                            {
                                for (int deltac = -2;deltac <= 2;deltac++)
                                {
                                    int newr = r+deltar;
                                    int newc = c+deltac;

                                    if (
                                        newr<0 ||
                                        newc<0 ||
                                        newr>=CASINO_ROWS ||
                                        newc>=CASINO_COLS)
                                    {
                                        //Out of bounds
                                        continue;
                                    }

                                    bool isActualPlayer = (!deltar) && (!deltac);

                                    if (output<=0.01)
                                    {
                                        if (isActualPlayer)
                                        {
                                            probabilityForPlay[newr][newc]-=0.20f;
                                        }
                                    }
                                    else if (output < 1.0)
                                    {
                                        if (isActualPlayer)
                                        {
                                            probabilityForPlay[newr][newc]-=0.05f;
                                        }
                                    }
                                    else if (output < 2.0)
                                    {
#if SPATIAL_CASINO_USE_FALLOFF
                                        probabilityForPlay[newr][newc]+=0.05f/2.0f/neighborCount;
                                        if (isActualPlayer)
                                        {
                                            probabilityForPlay[newr][newc]+=0.05f/2.0f;
                                        }
#else
                                        if (isActualPlayer)
                                        {
                                            probabilityForPlay[newr][newc]+=0.05f;
                                        }
#endif
                                    }
                                    else
                                    {
#if SPATIAL_CASINO_USE_FALLOFF
                                        probabilityForPlay[newr][newc]+=0.20f/2.0f/neighborCount;
                                        if (isActualPlayer)
                                        {
                                            probabilityForPlay[newr][newc]+=0.20f/2.0f;
                                        }
#else
                                        if (isActualPlayer)
                                        {
                                            probabilityForPlay[newr][newc]+=0.20f;
                                        }
#endif
                                    }
                                }
                            }
#endif

                        }
                    }
                }
            }
            double reward = bank/NUM_TRIALS/CASINO_ROWS/CASINO_COLS;
#if SPATIAL_CASINO_EXPERIMENT_DEBUG
            cout << "Bank: " << bank << endl;
            cout << "Reward: " << reward << endl;
#endif

            individual->reward(reward);
        }

        individual->setFitness(max(10.0,individual->getFitness()));
    }

    void SpatialCasinoExperiment::processIndividualPostHoc(shared_ptr<NEAT::GeneticIndividual> individual)
    {
        substrate = individual->spawnFastPhenotypeStack<double>();

        ofstream outfile("Payoffs.txt");

        double output;

        cout << "Creating payoff table:";
        for (int a=0;a<=PAYOFF_STEPS;a++)
        {
            if (a%100==0)
            {
                cout << ".";
            }

            substrate.reinitialize();

            if (substrate.hasNode("Bias"))
            {
                substrate.setValue("Bias",0.3);
            }

            double input = (a-PAYOFF_STEPS/2)/((double)PAYOFF_STEPS/2);

            substrate.update();

            output = substrate.getValue("Output");

            output *= 10.0;

            //output is now in the range [-10,10]

            if (output<0.0)
            {
                int flag=0;
            }

            //less than 0 means 0
            output = max(0.0,output);

            payoffFunction[a] = output;


            outfile << a << ' ' << output << endl;
        }
        cout << "Done" << endl;

    }

#ifndef HCUBE_NOGUI
    bool SpatialCasinoExperiment::handleMousePress(wxMouseEvent& event,wxSize &bitmapSize)
    {
        return false; //mouse presses can't affect the image
    }

    bool SpatialCasinoExperiment::handleMouseMotion(wxMouseEvent& event,wxDC &temp_dc,shared_ptr<NEAT::GeneticIndividual> individual)
    {
        return false;
    }

    void SpatialCasinoExperiment::createIndividualImage(wxDC &drawContext,shared_ptr<NEAT::GeneticIndividual> individual)
    {
    }
#endif

    Experiment* SpatialCasinoExperiment::clone()
    {
        SpatialCasinoExperiment* experiment = new SpatialCasinoExperiment(*this);

        return experiment;
    }

}
