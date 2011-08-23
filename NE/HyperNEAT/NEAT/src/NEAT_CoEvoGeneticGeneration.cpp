#include "NEAT_Defines.h"

#include "NEAT_CoEvoGeneticGeneration.h"

#include "NEAT_GeneticLinkGene.h"
#include "NEAT_Globals.h"

#ifdef EPLEX_INTERNAL

#define DEBUG_REMOVE_STALE_TESTS (1)

//Because you need to recompute fitnesses every time a test is deleted, delete a bunch at a time.
#define DEBUG_MIN_TEST_ALERT_SIZE (15)

#define DEBUG_MIN_TEST_SIZE (10)

namespace NEAT
{
    CoEvoGeneticGeneration::CoEvoGeneticGeneration(int _generationNumber,shared_ptr<CoEvoExperiment> _experiment)
        :
    GeneticGeneration(_generationNumber),
        experiment(_experiment)
    {}

    CoEvoGeneticGeneration::CoEvoGeneticGeneration(
        CoEvoGeneticGeneration *previousGeneration,
        const vector<shared_ptr<GeneticIndividual> > &newIndividuals,
        int _generationNumber,
        const vector<shared_ptr<GeneticIndividual> > &newTests,
        const vector< vector<bool> > &newTestResults,
        const vector< vector<double> > &newTestFitnesses,
        shared_ptr<CoEvoExperiment> _experiment
        )
        :
    GeneticGeneration(previousGeneration,newIndividuals,_generationNumber),
        tests(newTests),
        testResults(newTestResults),
        testFitnesses(newTestFitnesses),
        experiment(_experiment)
    {}

    shared_ptr<GeneticGeneration> CoEvoGeneticGeneration::produceNextGeneration(
        const vector<shared_ptr<GeneticIndividual> > &newIndividuals,
        int _generationNumber
        )
    {
        return shared_ptr<GeneticGeneration>(
            new CoEvoGeneticGeneration(
            this,
            newIndividuals,
            _generationNumber,
            tests,
            testResults,
            testFitnesses,
            experiment
            )
            );
    }

    CoEvoGeneticGeneration::CoEvoGeneticGeneration(
        TiXmlElement *generationElement,
        shared_ptr<CoEvoExperiment> _experiment
        )
        :
    GeneticGeneration(generationElement),
        experiment(_experiment)
    {
        TiXmlElement *individualElement = generationElement->FirstChildElement("Test");

        while ( individualElement!=NULL )
        {
            tests.push_back(shared_ptr<GeneticIndividual>(new GeneticIndividual(individualElement)));
            individualElement = individualElement->NextSiblingElement("Test");
        }

    }

    CoEvoGeneticGeneration::CoEvoGeneticGeneration(const CoEvoGeneticGeneration &other)
        :
    GeneticGeneration(other),
        tests(other.tests),
        testResults(other.testResults),
        testFitnesses(other.testFitnesses),
        experiment(other.experiment)
    {
    }

    CoEvoGeneticGeneration& CoEvoGeneticGeneration::operator=(const CoEvoGeneticGeneration &other)
    {
        GeneticGeneration::operator=(other);

        tests = other.tests;

        testResults = other.testResults;
        testFitnesses = other.testFitnesses;

        experiment = other.experiment;

        return *this;
    }

    CoEvoGeneticGeneration::~CoEvoGeneticGeneration()
    {}

    shared_ptr<GeneticIndividual> CoEvoGeneticGeneration::addTest(shared_ptr<GeneticIndividual> test)
    {
        shared_ptr<GeneticIndividual> newTest(
            new GeneticIndividual(*(test.get()))
            );

        tests.push_back(newTest);

        vector<bool> tmpResults;
        vector<double> tmpFitnesses;

        for (int a=0;a<(int)tests.size();a++)
        {
            tmpResults.push_back(false);
            tmpFitnesses.push_back(0.0);
        }

        for (int a=0;a<(int)testResults.size();a++)
        {
            testResults[a].push_back(false);
            testFitnesses[a].push_back(0.0);
        }

        testResults.push_back(tmpResults);
        testFitnesses.push_back(tmpFitnesses);

        return newTest;
    }

    void CoEvoGeneticGeneration::removeTest(int index)
    {
        /*
        vector<shared_ptr<GeneticIndividual> >::iterator loc = find(tests.begin(),tests.end(),test);

        if (loc != tests.end())
        {
        tests.erase(loc);
        }
        */

        cout << "Removing test\n";

        for (int a=0;a<getTestCount();a++)
        {
            if (a==index)
            {
                continue;
            }

            testResults[a].erase(testResults[a].begin()+index);
            testFitnesses[a].erase(testFitnesses[a].begin()+index);
        }

        tests.erase(tests.begin()+index);

        testResults.erase(testResults.begin()+index);
        testFitnesses.erase(testFitnesses.begin()+index);

        //recompute the new fitnesses

        for (int a=0;a<getTestCount();a++)
        {
            shared_ptr<GeneticIndividual> testA = getTest(a);

            testA->setFitness(0);

            for (int b=0;b<getTestCount();b++)
            {
                if (b==a)
                    continue;

                //shared_ptr<GeneticIndividual> testB = getTest(b);
                testA->reward(testFitnesses[a][b]);

            }
        }
    }

    double CoEvoGeneticGeneration::getAverageTestFitness()
    {
        double avgFitness=0.0;

        for (int a=0;a<getTestCount();a++)
        {
            avgFitness += getTest(a)->getFitness();
        }

        avgFitness /= getTestCount();

        return avgFitness;
    }

    double CoEvoGeneticGeneration::getMaxTestFitness()
    {
        double maxFitness=0.0;

        for (int a=0;a<getTestCount();a++)
        {
            maxFitness = max(maxFitness,getTest(a)->getFitness());
        }

        return maxFitness;
    }

    void CoEvoGeneticGeneration::replaceLowestTest(shared_ptr<GeneticIndividual> indToReplace)
    {
        if (getTestCount()==0)
        {
            throw CREATE_LOCATEDEXCEPTION_INFO("Tried to replace a test when there weren't any tests!");
        }

        int lowestFitnessIndex=0;
        double lowestFitness=getTest(0)->getFitness();

        for (int a=1;a<getTestCount();a++)
        {
            if (lowestFitness > getTest(a)->getFitness())
            {
                lowestFitness = getTest(a)->getFitness();
                lowestFitnessIndex = a;
            }
        }

        tests.erase(tests.begin()+lowestFitnessIndex);
        addTest(indToReplace);
    }

    void CoEvoGeneticGeneration::dump(TiXmlElement *generationElement,bool includeGenes)
    {
        GeneticGeneration::dump(generationElement,includeGenes);

        for (int a=0;a<(int)tests.size();a++)
        {
            TiXmlElement *individualElement = new TiXmlElement("Test");

            tests[a]->dump(individualElement,includeGenes);

            generationElement->LinkEndChild(individualElement);
        }
    }

    void CoEvoGeneticGeneration::dumpBest(TiXmlElement *generationElement,bool includeGenes)
    {
        GeneticGeneration::dumpBest(generationElement,includeGenes);
    }

    void CoEvoGeneticGeneration::cleanup()
    {
        GeneticGeneration::cleanup();

        //Blow away all the tests.
        tests.clear();
    }

    void CoEvoGeneticGeneration::bootstrap()
    {
        //Play all the tests against each other.
        cout << "Bootstrapping tests...";
        for (int a=0;a<getTestCount();a++)
        {
            shared_ptr<GeneticIndividual> testA = getTest(a);

            testA->setFitness(0.0);
        }

        for (int a=0;a<getTestCount();a++)
        {
            shared_ptr<GeneticIndividual> testA = getTest(a);

            for (int b=(a+1);b<getTestCount();b++)
            {
                shared_ptr<GeneticIndividual> testB = getTest(b);

                pair<double,double> rewards = experiment->playGame(
                    testA,
                    testB
                    );

                testA->reward(rewards.first);
                testB->reward(rewards.second);

                if (rewards.first > rewards.second)
                {
                    /*
                    NOTE:
                    I'm not really interested in tieing.  If two players tie, I don't really care what happens.
                    Basically, try to make games which do not have ties in fitness very often!
                    */

                    testResults[a][b] = true;
                    testResults[b][a] = false;
                }
                else
                {
                    testResults[a][b] = false;
                    testResults[b][a] = true;
                }

                testFitnesses[a][b] = rewards.first;
                testFitnesses[b][a] = rewards.second;
            }

            cout << "Test fitness: " << getTest(a)->getFitness() << endl;
        }

    }

    void CoEvoGeneticGeneration::updateTests()
    {
        //Here's my first shot at co-evolution

        //First, get the max fitness of the test cases.
        double maxFitness = getMaxTestFitness();

        for (int a=0;a<(int)individuals.size();a++)
        {
            shared_ptr<GeneticIndividual> individual = individuals[a];

            //If the individual is better than the best test case
            if (individual->getFitness() >= maxFitness)
            {
                cout << "***Adding Test*** Individual: " << individual->getFitness() << " Max: " << maxFitness << endl;

                //Add the individual
                shared_ptr<GeneticIndividual> test = addTest(individual);

                /*
                NOTE:
                Because of this next line, new tests need to be added to the end of the vector!
                */
                int newTestIndex = getTestCount()-1;

                //Play games so that it's still true that all tests have played each other
                //This is important because it's needed for the other tests' fitnesses to be accurate.
                test->setFitness(0);
                for (int a=0;a<getTestCount();a++)
                {

                    shared_ptr<GeneticIndividual> testA = getTest(a);

                    if (test != getTest(a))
                    {
                        //cout << "*";
                        pair<double,double> rewards =
                            experiment->playGame(
                            test,
                            testA
                            );

                        test->reward(rewards.first);

                        testA->reward(rewards.second);

                        /*
                        NOTE:
                        I'm not really interested in tieing.  If two players tie, I don't really care what happens.
                        Basically, try to make games which do not have ties in fitness very often!
                        */
                        if (rewards.first > rewards.second)
                        {
                            testResults[a][newTestIndex] = true;
                            testResults[newTestIndex][a] = false;
                        }
                        else
                        {
                            testResults[a][newTestIndex] = false;
                            testResults[newTestIndex][a] = true;
                        }

                        testFitnesses[a][newTestIndex] = rewards.first;
                        testFitnesses[newTestIndex][a] = rewards.second;
                    }
                }

                //Only allow adding one individual every generation
                break;
            }
        }

#if DEBUG_REMOVE_STALE_TESTS
        if ( (getTestCount()>=DEBUG_MIN_TEST_ALERT_SIZE) )
        {
            for (int a=0;a<getTestCount()&&(getTestCount()>DEBUG_MIN_TEST_SIZE);a++)
            {
                for (int b=0;b<getTestCount();b++)
                {
                    if (a==b)
                        continue;

                    if (getTestResult(a,b))
                    {
                        //a has won a match, we can't delete a (yet).
                        break;
                    }
                    else if (b+1==getTestCount())
                    {
                        //a has never won a match, delete a
                        removeTest(a);
                        a--;
                    }
                }
            }

            for (int a=0;a<getTestCount()&&(getTestCount()>DEBUG_MIN_TEST_SIZE);a++)
            {
                for (int b=a+1;b<getTestCount();b++)
                {
                    if (a==b)
                        continue;

                    bool doRemoveTest=true;

                    for (int t=0;t<getTestCount();t++)
                    {
                        if (t==a || t==b)
                            continue;

                        if (getTestResult(t,a) != getTestResult(t,b))
                        {
                            doRemoveTest=false;
                            break;
                        }
                    }

                    if (doRemoveTest)
                    {
                        //test results are the same, delete
                        removeTest(a);
                        a--;
                        break;
                    }
                }
            }

            while ( (getTestCount()>DEBUG_MIN_TEST_SIZE) )
            {
                //we still need to remove some tests, remove older tests first
                removeTest(0);
            }
        }
#endif
    }

    bool CoEvoGeneticGeneration::getTestResult(int t1,int t2)
    {
        return testResults[t1][t2];
    }
}

#endif
