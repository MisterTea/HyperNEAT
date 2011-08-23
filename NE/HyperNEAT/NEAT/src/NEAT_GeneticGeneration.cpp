#include "NEAT_Defines.h"

#include "NEAT_GeneticGeneration.h"

#include "NEAT_GeneticLinkGene.h"
#include "NEAT_Globals.h"

namespace NEAT
{
    GeneticGeneration::GeneticGeneration(int _generationNumber)
            :
            generationNumber(_generationNumber),
            sortedByFitness(false),
            isCached(false)
    {}

    GeneticGeneration::GeneticGeneration(
        GeneticGeneration *previousGeneration,
        const vector<shared_ptr<GeneticIndividual> > &newIndividuals,
        int _generationNumber)
            :
            individuals(newIndividuals),
            generationNumber(_generationNumber),
            sortedByFitness(false),
            isCached(false)
    {}

    GeneticGeneration::GeneticGeneration(TiXmlElement *generationElement)
            :
      isCached(true),
      sortedByFitness(true)
    {
        generationNumber = atoi(generationElement->Attribute("GenNumber"));

        generationElement->Attribute("AverageFitness",&cachedAverageFitness);

        TiXmlElement *individualElement = generationElement->FirstChildElement("Individual");

        do
        {
            individuals.push_back(shared_ptr<GeneticIndividual>(new GeneticIndividual(individualElement)));

            individualElement = individualElement->NextSiblingElement("Individual");
        }
        while ( individualElement!=NULL );

    }

    GeneticGeneration::GeneticGeneration(const GeneticGeneration &other)
    {
        //cout << "Invoking copy constructor...";
        generationNumber = other.generationNumber;
        sortedByFitness = other.sortedByFitness;

        isCached = other.isCached;
        cachedAverageFitness = other.cachedAverageFitness;

		userData = other.userData;

        individuals = other.individuals;

        //cout << "done!\n";
    }

    shared_ptr<GeneticGeneration> GeneticGeneration::produceNextGeneration(
        const vector<shared_ptr<GeneticIndividual> > &newIndividuals,
        int _generationNumber
        )
    {
        return shared_ptr<GeneticGeneration>(
            new GeneticGeneration(
            this,
            newIndividuals,
            _generationNumber
            )
            );
    }

    GeneticGeneration& GeneticGeneration::operator=(const GeneticGeneration &other)
    {
        //cout << "Invoking assignment operator...";
        generationNumber = other.generationNumber;
        sortedByFitness = other.sortedByFitness;

        isCached = other.isCached;
        cachedAverageFitness = other.cachedAverageFitness;

		userData = other.userData;

        individuals = other.individuals;

        //cout << "done!\n";
        return *this;
    }

    GeneticGeneration::~GeneticGeneration()
    {
    }

    void GeneticGeneration::setAttributes(TiXmlElement *generationElement)
    {
        generationElement->SetAttribute("GenNumber",int(generationNumber));

        generationElement->SetAttribute("UserData",userData);

        double totalFitness=0;

        vector<int> speciesIDs;

        for (int a=0;a<(int)individuals.size();a++)
        {
            totalFitness += individuals[a]->getFitness();

            for (int b=0;b<=(int)speciesIDs.size();b++)
            {
                if (b==(int)speciesIDs.size())
                {
                    speciesIDs.push_back(individuals[a]->getSpeciesID());
                    break;
                }
                else if (speciesIDs[b]==individuals[a]->getSpeciesID())
                {
                    break;
                }
            }
        }

        if (isCached)
        {
            generationElement->SetDoubleAttribute("AverageFitness",cachedAverageFitness);
        }
        else
        {
            generationElement->SetDoubleAttribute("AverageFitness",totalFitness/individuals.size());
        }

        generationElement->SetAttribute("SpeciesCount",int(speciesIDs.size()));

    }

    void GeneticGeneration::dump(TiXmlElement *generationElement,bool includeGenes)
    {
        setAttributes(generationElement);

        for (int a=0;a<(int)individuals.size();a++)
        {
            TiXmlElement *individualElement = new TiXmlElement("Individual");

            individuals[a]->dump(individualElement,includeGenes);

            generationElement->LinkEndChild(individualElement);
        }
    }

    void GeneticGeneration::dumpBest(TiXmlElement *generationElement,bool includeGenes)
    {
        setAttributes(generationElement);

        shared_ptr<GeneticIndividual> bestIndividual=individuals[0];

        for (int a=1;a<(int)individuals.size();a++)
        {
            if (individuals[a]->getFitness()>bestIndividual->getFitness())
                bestIndividual = individuals[a];
        }

        TiXmlElement *individualElement = new TiXmlElement("Individual");

        bestIndividual->dump(individualElement,includeGenes);

        generationElement->LinkEndChild(individualElement);
    }

    shared_ptr<GeneticIndividual> GeneticGeneration::mateIndividuals(int i1,int i2)
    {
        shared_ptr<GeneticIndividual> ind1 = individuals[i1];
        shared_ptr<GeneticIndividual> ind2 = individuals[i2];

        return shared_ptr<GeneticIndividual>(new GeneticIndividual(ind1,ind2));
    }

    double GeneticGeneration::getCompatibility(int i1,int i2)
    {
        shared_ptr<GeneticIndividual> ind1 = individuals[i1];
        shared_ptr<GeneticIndividual> ind2 = individuals[i2];

        return ind1->getCompatibility(ind2);
    }

    void GeneticGeneration::sortByFitness()
    {
        for (int a=0;a<(int)individuals.size();a++)
        {
            for (int b=0;b<((int)individuals.size()-(a+1));b++)
            {
                if (individuals[b]->getFitness()<individuals[b+1]->getFitness())
                {
                    shared_ptr<GeneticIndividual> ind = individuals[b];
                    individuals[b] = individuals[b+1];
                    individuals[b+1] = ind;
                }
            }
        }

        sortedByFitness=true;
    }

//Gets the generation champion.  Based on unadjusted Fitness
    shared_ptr<GeneticIndividual> GeneticGeneration::getGenerationChampion()
    {
        shared_ptr<GeneticIndividual> bestIndividual;

        for (int b=0;b<(int)individuals.size();b++)
        {
            if (!bestIndividual||individuals[b]->getFitness()>bestIndividual->getFitness())
            {
                bestIndividual = individuals[b];
            }
        }

        return bestIndividual;
    }

    void GeneticGeneration::cleanup()
    {
        if (!sortedByFitness)
        {
            throw CREATE_LOCATEDEXCEPTION_INFO("You aren't supposed to acll this until you sort by fitness!");
        }

        while (individuals.size()>1)
        {
            //cout << "CLEANING UP!\n";
            //Never delete the generation champion (remember that they are sorted by fitness at this point).

            individuals.erase(individuals.begin()+1);
        }
    }

    void GeneticGeneration::randomizeIndividualOrder()
    {
        double *randPos = new double[individuals.size()];

        for (int a=0;a<int(individuals.size());a++)
        {
            randPos[a] = Globals::getSingleton()->getRandom().getRandomDouble();
        }

        for (int a=0;a<int(individuals.size());a++)
        {
            for (int b=0;b<int(individuals.size())-1;b++)
            {
                if (randPos[b] > randPos[b+1])
                {
                    swap(randPos[b],randPos[b+1]);
                    swap(individuals[b],individuals[b+1]);
                }
            }
        }

        sortedByFitness=false;

        delete[] randPos;
    }
}
