#include "NEAT_Defines.h"

#include "NEAT_Globals.h"

#include "NEAT_GeneticNodeGene.h"
#include "NEAT_GeneticLinkGene.h"

#define DEBUG_NEAT_GLOBALS (0)

#include <boost/algorithm/string.hpp>

const char* activationFunctionNames[ACTIVATION_FUNCTION_END] =
{
    "SIGMOID",
    "SIN",
    "COS",
    "GAUSSIAN",
    "SQUARE",
    "ABS_ROOT",
    "LINEAR",
    "ONES_COMPLIMENT"
};

namespace NEAT
{
    double signedSigmoidTable[6001];
    double unsignedSigmoidTable[6001];

    Globals *Globals::singleton = NULL;
    void Globals::assignNodeID(GeneticNodeGene *testNode)
    {
        testNode->setID(generateNodeID());
    }

    void Globals::assignLinkID(GeneticLinkGene *testLink,bool ignoreHistory)
    {
        if (ignoreHistory)
        {
            testLink->setID(generateLinkID());
        }
        for (int a=0;a<(int)linkGenesThisGeneration.size();a++)
        {
            shared_ptr<GeneticLinkGene> link = linkGenesThisGeneration[a];
            if (link->getFromNodeID()==testLink->getFromNodeID()&&link->getToNodeID()==testLink->getToNodeID())
            {
                testLink->setID( link->getID() );
                return;
            }
        }
        testLink->setID(generateLinkID());
        linkGenesThisGeneration.push_back(shared_ptr<GeneticLinkGene>(new GeneticLinkGene(*testLink)));
    }

    void Globals::clearLinkHistory()
    {
        linkGenesThisGeneration.clear();
    }

    int Globals::generateSpeciesID()
    {
        return speciesCounter++;
    }

    int Globals::generateNodeID()
    {
        return nodeCounter++;
    }

    int Globals::generateLinkID()
    {
        return linkCounter++;
    }

    void Globals::addParameter(string name,double value)
    {
        parameters.insert(name,value);
    }

    void Globals::setParameterValue(string name,double value)
    {
        parameters.insert(name,value);

        cacheParameters();
    }

    Globals::Globals()
            :
            nodeCounter(0),
            linkCounter(0),
            speciesCounter(0)
    {
        cout << "Populating sigmoid table...";
        for (int a=0;a<6001;a++)
        {
            signedSigmoidTable[a] = ((1 / (1+exp(-((a-3000)/1000.0)))) - 0.5)*2.0;
            unsignedSigmoidTable[a] = 1 / (1+exp(-((a-3000)/1000.0)));
        }
        cout << "done!\n";

        cout << "Loading Parameter data from defaults" << endl;

        parameters.insert("PopulationSize",120.0);
        parameters.insert("MaxGenerations",600.0);
        parameters.insert("DisjointCoefficient",2.0);
        parameters.insert("ExcessCoefficient", 2.0);
        parameters.insert("WeightDifferenceCoefficient", 1.0);
        parameters.insert("FitnessCoefficient", 0.0);
        parameters.insert("CompatibilityThreshold", 6.0);
        parameters.insert("CompatibilityModifier", 0.3);
        parameters.insert("SpeciesSizeTarget", 8.0);
        parameters.insert("DropoffAge", 15.0);
        parameters.insert("vAgeSignificance",	1.0);
        parameters.insert("SurvivalThreshold", 0.2);
        parameters.insert("MutateAddNodeProbability", 0.03);
        parameters.insert("MutateAddLinkProbability", 0.3);
        parameters.insert("MutateDemolishLinkProbability", 0.00);
        parameters.insert("MutateLinkWeightsProbability", 0.8);
        parameters.insert("MutateOnlyProbability", 0.25);
        parameters.insert("MutateLinkProbability", 0.1);
        parameters.insert("AllowAddNodeToRecurrentConnection", 0.0);
        parameters.insert("SmallestSpeciesSizeWithElitism", 5.0);
        parameters.insert("MutateSpeciesChampionProbability", 0.0);
        parameters.insert("MutationPower", 2.5);
        parameters.insert("AdultLinkAge", 18.0);
        parameters.insert("AllowRecurrentConnections", 0.0);
        parameters.insert("AllowSelfRecurrentConnections", 0.0);
        parameters.insert("ForceCopyGenerationChampion", 1.0);
        parameters.insert("LinkGeneMinimumWeightForPhentoype", 0.0);
        parameters.insert("GenerationDumpModulo", 10.0);
        parameters.insert("RandomSeed", -1.0);
        parameters.insert("ExtraActivationFunctions", 9.0);
        parameters.insert("AddBiasToHiddenNodes", 0.0);
        parameters.insert("SignedActivation", 1.0);
        parameters.insert("ExtraActivationUpdates", 9.0);
        parameters.insert("OnlyGaussianHiddenNodes", 0.0);
        parameters.insert("ExperimentType", 15.0);

		cacheParameters();

		initRandom();
    }

    Globals::Globals(string fileName)
            :
            nodeCounter(0),
            linkCounter(0),
            speciesCounter(0)
    {
        cout << "Populating sigmoid table...";
        for (int a=0;a<6001;a++)
        {
            signedSigmoidTable[a] = ((1 / (1+exp(-((a-3000)/1000.0)))) - 0.5)*2.0;
            unsignedSigmoidTable[a] = 1 / (1+exp(-((a-3000)/1000.0)));
        }
        cout << "done!\n";

        cout << "Loading Parameter data from " << fileName << endl;

        if (fileName==string(""))
        {
            return;
        }

        ifstream infile;
        infile.open(fileName.c_str());

        if (infile.is_open()==false)
        {
            throw CREATE_LOCATEDEXCEPTION_INFO(string("COULD NOT OPEN GLOBALS .dat FILE: ")+fileName);
        }

        string line="test";
        istringstream instream;
        while (getline(infile,line))
        {
#if DEBUG_NEAT_GLOBALS
            cout << "LINE: " << line << endl;
#endif
#ifdef linux
            if (int(line[line.size()-1])==13) //DOS line breaks
                line.erase(line.begin()+(int(line.size())-1));
#endif
            if (line[0]==';') //comment
            {
                continue;
            }
            if (line.size()==0)
            {
                continue;
            }
            istringstream input(line);
            string parameterName;
            double value=0.0;
            input >> parameterName >> value;
            parameters.insert(parameterName,value);
        }

		cacheParameters();

		initRandom();
    }

    Globals::Globals(TiXmlElement *root)
            :
            nodeCounter(-1),
            linkCounter(-1),
            speciesCounter(-1)
    {
        cout << "Populating sigmoid table...";
        for (int a=0;a<6001;a++)
        {
            signedSigmoidTable[a] = ((1 / (1+exp(-((a-3000)/1000.0)))) - 0.5)*2.0;
            unsignedSigmoidTable[a] = 1 / (1+exp(-((a-3000)/1000.0)));
        }
        cout << "done!\n";

        TiXmlAttribute *firstAttribute = root->FirstAttribute();

        while (firstAttribute)
        {
            if (iequals(firstAttribute->Name(),"NodeCounter"))
            {
                nodeCounter = firstAttribute->IntValue();
            }
            else if (iequals(firstAttribute->Name(),"LinkCounter"))
            {
                linkCounter = firstAttribute->IntValue();
            }
            else if (iequals(firstAttribute->Name(),"SpeciesCounter"))
            {
                speciesCounter = firstAttribute->IntValue();
            }
            else
            {
                addParameter( firstAttribute->Name() , firstAttribute->DoubleValue());
            }

            firstAttribute = firstAttribute->Next();
        }

        if (nodeCounter==-1 || linkCounter==-1 || speciesCounter==-1)
        {
            throw CREATE_LOCATEDEXCEPTION_INFO("MALFORMED XML!");
        }

		cacheParameters();

		initRandom();
    }

    void Globals::initRandom()
    {
        double randomSeed = getParameterValue("RandomSeed");
        if (randomSeed<0.0)
        {
            cout << "Seeding random generator to time\n";
            random = Random(); //use time as the seed
        }
        else
        {
            cout << "Seeing random generator with given seed: " << uint(randomSeed) << endl;
            random = Random(uint(randomSeed));
        }
    }

    void Globals::seedRandom(unsigned int newSeed)
    {
        cout << "Reseeding random with seed: " << newSeed << endl;
        random = Random(newSeed);
    }

    void Globals::dump(TiXmlElement *root)
    {
        root->SetAttribute("ActualRandomSeed",getRandom().getSeed());
        root->SetAttribute("NodeCounter",nodeCounter);
        root->SetAttribute("LinkCounter",linkCounter);
        root->SetAttribute("SpeciesCounter",speciesCounter);
        StackMap<string,double,4096>::iterator mapIterator = getMapBegin();
        StackMap<string,double,4096>::iterator mapEnd = getMapEnd();
        for (;mapIterator!=mapEnd;mapIterator++)
        {
            root->SetDoubleAttribute(
                mapIterator->first.c_str(),
                mapIterator->second
            );
        }
    }

	bool Globals::hasParameterValue(const string &name)
	{
		return parameters.hasKey(name);
	}

    double Globals::getParameterValue(const char *cname)
    {
		return parameters.getDataRef(cname);
    }

    double Globals::getParameterValue(string name)
    {
		return parameters.getDataRef(name);
    }

    Globals::~Globals()
    {}

	void Globals::cacheParameters()
	{
		cout << "ExtraActivationUpdates" << endl;
        extraActivationUpdates = int(getParameterValue("ExtraActivationUpdates"));

		cout << "SignedActivation" << endl;
		if(getParameterValue("SignedActivation")>0.5)
		{
			signedActivation = true;
		}
		else
		{
			signedActivation = false;
		}

		cout << "UseTanhSigmoid" << endl;
		if(hasParameterValue("UseTanhSigmoid") && getParameterValue("UseTanhSigmoid")>0.5)
		{
			useTanhSigmoid = true;
		}
		else
		{
			useTanhSigmoid = false;
		}
	}
}
