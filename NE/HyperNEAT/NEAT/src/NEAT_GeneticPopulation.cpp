#include "NEAT_Defines.h"

#include "NEAT_GeneticPopulation.h"

#include "NEAT_GeneticGeneration.h"

#ifdef EPLEX_INTERNAL
#include "NEAT_CoEvoGeneticGeneration.h"
#endif

#include "NEAT_GeneticIndividual.h"
#include "NEAT_Random.h"

namespace NEAT
{

    GeneticPopulation::GeneticPopulation()
            : onGeneration(0)
    {
            generations.push_back(
                shared_ptr<GeneticGeneration>(
                    new GeneticGeneration(0)
                )
            );
    }

    GeneticPopulation::GeneticPopulation(string fileName)
            : onGeneration(-1)
    {
        TiXmlDocument doc(fileName);

        bool loadStatus;

        if (iends_with(fileName,".gz"))
        {
            loadStatus = doc.LoadFileGZ();
        }
        else
        {
            loadStatus = doc.LoadFile();
        }

        if (!loadStatus)
        {
            throw CREATE_LOCATEDEXCEPTION_INFO("Error trying to load the XML file!");
        }

        TiXmlElement *root;

        root = doc.FirstChildElement();

        {
            TiXmlElement *generationElement = root->FirstChildElement("GeneticGeneration");

            while (generationElement)
            {
                generations.push_back(shared_ptr<GeneticGeneration>(new GeneticGeneration(generationElement)));

                generationElement = generationElement->NextSiblingElement("GeneticGeneration");
                onGeneration++;
            }
        }

        if (onGeneration<0)
        {
            throw CREATE_LOCATEDEXCEPTION_INFO("Tried to load a population with no generations!");
        }
        cout << "Loaded " << (onGeneration+1) << " Generations\n";

        adjustFitness();
    }

#ifdef EPLEX_INTERNAL
    GeneticPopulation::GeneticPopulation(shared_ptr<CoEvoExperiment> experiment)
            : onGeneration(0)
    {
        if (experiment)
        {
            generations.push_back(
                static_pointer_cast<GeneticGeneration>(
                    shared_ptr<CoEvoGeneticGeneration>(
                        new CoEvoGeneticGeneration(0,experiment)
                    )
                )
            );
        }
        else
        {
            generations.push_back(
                shared_ptr<GeneticGeneration>(
                    new GeneticGeneration(0)
                )
            );
        }
    }

    GeneticPopulation::GeneticPopulation(
        string fileName,
        shared_ptr<CoEvoExperiment> experiment
    )
            : onGeneration(-1)
    {
        TiXmlDocument doc(fileName);

        bool loadStatus;

        if (iends_with(fileName,".gz"))
        {
            loadStatus = doc.LoadFileGZ();
        }
        else
        {
            loadStatus = doc.LoadFile();
        }

        if (!loadStatus)
        {
            throw CREATE_LOCATEDEXCEPTION_INFO("Error trying to load the XML file!");
        }

        TiXmlElement *root;

        root = doc.FirstChildElement();

        /**NOTE**
        Do not mix GeneticGeneration objects with CoEvoGeneticGeneration objects in the same xml file.\
        */

        {
            TiXmlElement *generationElement = root->FirstChildElement("CoEvoGeneticGeneration");

            while (generationElement)
            {
                if (!experiment)
                {
                    throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: TRIED TO CREATE A COEVOLUTION WITHOUT AN EXPERIMENT!");
                }

                generations.push_back(shared_ptr<GeneticGeneration>(
                                          new CoEvoGeneticGeneration(generationElement,experiment))
                                     );
                generationElement = generationElement->NextSiblingElement("CoEvoGeneticGeneration");
                onGeneration++;
            }
        }

        if (onGeneration<0)
        {
            throw CREATE_LOCATEDEXCEPTION_INFO("Tried to load a population with no generations!");
        }

        adjustFitness();
    }
#endif

    GeneticPopulation::~GeneticPopulation()
    {
        while (!species.empty())
            species.erase(species.begin());

        while (!extinctSpecies.empty())
            extinctSpecies.erase(extinctSpecies.begin());
    }

    int GeneticPopulation::getIndividualCount(int generation)
    {
        if (generation==-1)
            generation=int(onGeneration);

        return generations[generation]->getIndividualCount();
    }

    void GeneticPopulation::addIndividual(shared_ptr<GeneticIndividual> individual)
    {
        generations[onGeneration]->addIndividual(individual);
    }

    shared_ptr<GeneticIndividual> GeneticPopulation::getIndividual(int individualIndex,int generation)
    {
        //cout << a << ',' << generation << endl;

        if (generation==-1)
        {
            //cout << "NO GEN GIVEN: USING onGeneration\n";
            generation=int(onGeneration);
        }

        if (generation>=int(generations.size())||individualIndex>=generations[generation]->getIndividualCount())
        {
            cout << "GET_INDIVIDUAL: GENERATION OUT OF RANGE!\n";
            throw CREATE_LOCATEDEXCEPTION_INFO("GET_INDIVIDUAL: GENERATION OUT OF RANGE!\n");
        }

        return generations[generation]->getIndividual(individualIndex);
    }

    vector<shared_ptr<GeneticIndividual> >::iterator GeneticPopulation::getIndividualIterator(int a,int generation)
    {
        if (generation==-1)
            generation=int(onGeneration);

        if (generation>=int(generations.size())||a>=generations[generation]->getIndividualCount())
        {
            throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: Generation out of range!\n");
        }

        return generations[generation]->getIndividualIterator(a);
    }

    shared_ptr<GeneticIndividual> GeneticPopulation::getBestAllTimeIndividual()
    {
        shared_ptr<GeneticIndividual> bestIndividual;

        for (int a=0;a<(int)generations.size();a++)
        {
            for (int b=0;b<generations[a]->getIndividualCount();b++)
            {
                shared_ptr<GeneticIndividual> individual = generations[a]->getIndividual(b);
                if (bestIndividual==NULL||bestIndividual->getFitness()<=individual->getFitness())
                    bestIndividual = individual;
            }
        }

        return bestIndividual;
    }

    shared_ptr<GeneticIndividual> GeneticPopulation::getBestIndividualOfGeneration(int generation)
    {
        shared_ptr<GeneticIndividual> bestIndividual;

        if (generation==-1)
            generation = int(generations.size())-1;

        for (int b=0;b<generations[generation]->getIndividualCount();b++)
        {
            shared_ptr<GeneticIndividual> individual = generations[generation]->getIndividual(b);
            if (bestIndividual==NULL||bestIndividual->getFitness()<individual->getFitness())
                bestIndividual = individual;
        }

        return bestIndividual;
    }

    void GeneticPopulation::speciate()
    {
        double compatThreshold = Globals::getSingleton()->getParameterValue("CompatibilityThreshold");

        for (int a=0;a<generations[onGeneration]->getIndividualCount();a++)
        {
            shared_ptr<GeneticIndividual> individual = generations[onGeneration]->getIndividual(a);

            bool makeNewSpecies=true;

            for (int b=0;b<(int)species.size();b++)
            {
                double compatibility = species[b]->getBestIndividual()->getCompatibility(individual);
                if (compatibility<compatThreshold)
                {
                    //Found a compatible species
                    individual->setSpeciesID(species[b]->getID());
                    makeNewSpecies=false;
                    break;
                }
            }

            if (makeNewSpecies)
            {
                //Make a new species.  The process of making a new speceis sets the ID for the individual.
                shared_ptr<GeneticSpecies> newSpecies(new GeneticSpecies(individual));
                species.push_back(newSpecies);
            }
        }

        int speciesTarget = int(Globals::getSingleton()->getParameterValue("SpeciesSizeTarget"));

        double compatMod;

        if ((int)species.size()<speciesTarget)
        {
            compatMod = -Globals::getSingleton()->getParameterValue("CompatibilityModifier");
        }
        else if ((int)species.size()>speciesTarget)
        {
            compatMod = +Globals::getSingleton()->getParameterValue("CompatibilityModifier");
        }
        else
        {
            compatMod=0.0;
        }

        if (compatThreshold<(fabs(compatMod)+0.3)&&compatMod<0.0)
        {
            //This is to keep the compatibility threshold from going ridiculusly small.
            if (compatThreshold<0.001)
                compatThreshold = 0.001;

            compatThreshold/=2.0;
        }
        else if (compatThreshold<(compatMod+0.3))
        {
            compatThreshold*=2.0;
        }
        else
        {
            compatThreshold+=compatMod;
        }

        Globals::getSingleton()->setParameterValue("CompatibilityThreshold",compatThreshold);
    }

    void GeneticPopulation::setSpeciesMultipliers()
    {}

    void GeneticPopulation::adjustFitness()
    {
        //This function sorts the individuals by fitness
        generations[onGeneration]->sortByFitness();

        speciate();

        for (int a=0;a<(int)species.size();a++)
        {
            species[a]->resetIndividuals();
        }

        //This function sorts the individuals by fitness
        generations[onGeneration]->sortByFitness();

        for (int a=0;a<generations[onGeneration]->getIndividualCount();a++)
        {
            shared_ptr<GeneticIndividual> individual = generations[onGeneration]->getIndividual(a);

            getSpecies(individual->getSpeciesID())->addIndividual(individual);
        }

        for (int a=0;a<(int)species.size();a++)
        {
            if (species[a]->getIndividualCount()==0)
            {
                extinctSpecies.push_back(species[a]);
                species.erase(species.begin()+a);
                a--;
            }
        }

        for (int a=0;a<(int)species.size();a++)
        {
            species[a]->setMultiplier();
            species[a]->setFitness();
            species[a]->incrementAge();
        }

        //This function sorts the individuals by fitness again, now that speciation has taken place.
        generations[onGeneration]->sortByFitness();
    }

    void GeneticPopulation::produceNextGeneration()
    {
#ifdef EPLEX_INTERNAL
        if (equals(generations[onGeneration]->getTypeName(),"CoEvoGeneticGeneration"))
        {
            //We are dealing with coevolution, update tests

            static_pointer_cast<CoEvoGeneticGeneration>(generations[onGeneration])->updateTests();
        }
#endif

        cout << "In produce next generation loop...\n";
        //This clears the link history so future links with the same toNode and fromNode will have different IDs
        Globals::getSingleton()->clearLinkHistory();

        int numParents = int(generations[onGeneration]->getIndividualCount());

        for(int a=0;a<numParents;a++)
        {
            if(generations[onGeneration]->getIndividual(a)->getFitness() < 1e-6)
            {
                throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: Fitness must be a positive number!\n");
            }
        }

        double totalFitness=0;

        for (int a=0;a<(int)species.size();a++)
        {
            totalFitness += species[a]->getAdjustedFitness();
        }
        int totalOffspring=0;
        for (int a=0;a<(int)species.size();a++)
        {
            double adjustedFitness = species[a]->getAdjustedFitness();
            int offspring = int(adjustedFitness/totalFitness*numParents);
            totalOffspring+=offspring;
            species[a]->setOffspringCount(offspring);
        }
        //cout << "Pausing\n";
        //system("PAUSE");
        //Some offspring were truncated.  Give these to the best individuals
        while (totalOffspring<numParents)
        {
            for (int a=0;totalOffspring<numParents&&a<generations[onGeneration]->getIndividualCount();a++)
            {
                shared_ptr<GeneticIndividual> ind = generations[onGeneration]->getIndividual(a);
                shared_ptr<GeneticSpecies> gs = getSpecies(ind->getSpeciesID());
                gs->setOffspringCount(gs->getOffspringCount()+1);
                totalOffspring++;

                /*
                //Try to give 2 offspring to the very best individual if it only has one offspring.
                //This fixes the problem where the best indiviudal sits in his own species
                //and duplicates every generation.
                if(a==0&&gs->getOffspringCount()==1&&totalOffspring<numParents)
                {
                gs->setOffspringCount(gs->getOffspringCount()+1);
                totalOffspring++;
                }*/

            }
        }
        for (int a=0;a<(int)species.size();a++)
        {
            cout << "Species ID: " << species[a]->getID() << " Age: " << species[a]->getAge() << " last improv. age: " << species[a]->getAgeOfLastImprovement() << " Fitness: " << species[a]->getFitness() << "*" << species[a]->getMultiplier() << "=" << species[a]->getAdjustedFitness() <<  " Size: " << int(species[a]->getIndividualCount()) << " Offspring: " << int(species[a]->getOffspringCount()) << endl;
        }

        //This is the new generation
        vector<shared_ptr<GeneticIndividual> > babies;

        double totalIndividualFitness=0;

        for (int a=0;a<(int)species.size();a++)
        {
            species[a]->setReproduced(false);
        }

        int smallestSpeciesSizeWithElitism
        = int(Globals::getSingleton()->getParameterValue("SmallestSpeciesSizeWithElitism"));
        double mutateSpeciesChampionProbability
        = Globals::getSingleton()->getParameterValue("MutateSpeciesChampionProbability");
        bool forceCopyGenerationChampion
        = (
              Globals::getSingleton()->getParameterValue("ForceCopyGenerationChampion")>
              Globals::getSingleton()->getRandom().getRandomDouble()
          );

        for (int a=0;a<generations[onGeneration]->getIndividualCount();a++)
        {
            //Go through and add the species champions
            shared_ptr<GeneticIndividual> ind = generations[onGeneration]->getIndividual(a);
            shared_ptr<GeneticSpecies> species = getSpecies(ind->getSpeciesID());
            if (!species->isReproduced())
            {
                species->setReproduced(true);
                //This is the first and best organism of this species to be added, so it's the species champion
                //of this generation
                if (ind->getFitness()>species->getBestIndividual()->getFitness())
                {
                    //We have a new all-time species champion!
                    species->setBestIndividual(ind);
                    cout << "Species " << species->getID() << " has a new champ with fitness " << species->getBestIndividual()->getFitness() << endl;
                }

                if ((a==0&&forceCopyGenerationChampion)||(species->getOffspringCount()>=smallestSpeciesSizeWithElitism))
                {
                    //Copy species champion.
                    bool mutateChampion;
                    if (Globals::getSingleton()->getRandom().getRandomDouble()<mutateSpeciesChampionProbability)
                        mutateChampion = true;
                    else
                        mutateChampion = false;
                    babies.push_back(shared_ptr<GeneticIndividual>(new GeneticIndividual(ind,mutateChampion)));
                    species->decrementOffspringCount();
                }

                if (a==0)
                {
                    species->updateAgeOfLastImprovement();
                }
            }
            totalIndividualFitness+=ind->getFitness();
        }
        double averageFitness = totalIndividualFitness/generations[onGeneration]->getIndividualCount();
        cout<<"Generation "<<int(onGeneration)<<": "<<"overall_average = "<<averageFitness<<endl;
        cout << "Champion fitness: " << generations[onGeneration]->getIndividual(0)->getFitness() << endl;

        if (generations[onGeneration]->getIndividual(0)->getUserData().length())
        {
            cout << "Champion data: " << generations[onGeneration]->getIndividual(0)->getUserData() << endl;
        }
        cout << "# of Species: " << int(species.size()) << endl;
        cout << "compat threshold: " << Globals::getSingleton()->getParameterValue("CompatibilityThreshold") << endl;

        for (int a=0;a<(int)species.size();a++)
        {
            //cout << "Making babies\n";
            species[a]->makeBabies(babies);
        }
        if ((int)babies.size()!=generations[onGeneration]->getIndividualCount())
        {
            cout << "Population size changed!\n";
            throw CREATE_LOCATEDEXCEPTION_INFO("Population size changed!");
        }

        //cout << "Making new generation\n";
        shared_ptr<GeneticGeneration> newGeneration(generations[onGeneration]->produceNextGeneration(babies,onGeneration+1));
        //cout << "Done Making new generation!\n";

        /*for(int a=0;a<4;a++)
        {
        for(int b=0;b<4;b++)
        {
        cout << babies[a]->getCompatibility(babies[b]) << '\t';
        }

        cout << endl;
        }*/

        generations.push_back(newGeneration);
        onGeneration++;
    }


    void GeneticPopulation::dump(string filename,bool includeGenes,bool doGZ)
    {
        TiXmlDocument doc( filename );

        TiXmlElement *root = new TiXmlElement("Genetics");

        Globals::getSingleton()->dump(root);

        doc.LinkEndChild(root);

        for (int a=0;a<(int)generations.size();a++)
        {

            TiXmlElement *generationElementPtr = new TiXmlElement(generations[a]->getTypeName());

            root->LinkEndChild(generationElementPtr);

            generations[a]->dump(generationElementPtr,includeGenes);
        }

        if (doGZ)
        {
            doc.SaveFileGZ();
        }
        else
        {
            doc.SaveFile();
        }
    }

    void GeneticPopulation::dumpBest(string filename,bool includeGenes,bool doGZ)
    {
        TiXmlDocument doc( filename );

        TiXmlElement *root = new TiXmlElement("Genetics");

        Globals::getSingleton()->dump(root);

        doc.LinkEndChild(root);

        for (int a=0;a<int(generations.size())-1;a++)
        {

            TiXmlElement *generationElementPtr = new TiXmlElement(generations[a]->getTypeName());

            root->LinkEndChild(generationElementPtr);

            generations[a]->dumpBest(generationElementPtr,includeGenes);
        }

        if (generations.size())
        {
            //Always dump everyone from the final generation
            TiXmlElement *generationElementPtr = new TiXmlElement(generations[generations.size()-1]->getTypeName());
            generations[generations.size()-1]->dump(generationElementPtr,includeGenes);
            root->LinkEndChild(generationElementPtr);
        }

        if (doGZ)
        {
            doc.SaveFileGZ();
        }
        else
        {
            doc.SaveFile();
        }
    }

    void GeneticPopulation::cleanupOld(int generationSkip)
    {
        for (int a=0;a<onGeneration;a++)
        {
            if ( (a%generationSkip) == 0 )
                continue;

            generations[a]->cleanup();
        }
    }
}
