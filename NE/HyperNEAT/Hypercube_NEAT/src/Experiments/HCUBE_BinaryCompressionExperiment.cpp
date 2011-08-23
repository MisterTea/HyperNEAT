#include "HCUBE_Defines.h"

#include "Experiments/HCUBE_BinaryCompressionExperiment.h"

#define INPUT_FILE "3nin.rar"

#define OUTPUT_FILE "3nin.out"

#define MAX_FILE_SIZE (16*1024*1024)

#define FITNESS_THRESHOLD (9000)

namespace HCUBE
{
    using namespace NEAT;

    boost::mutex loadFileMutex;

    BinaryCompressionExperiment::BinaryCompressionExperiment(string _experimentName,int _threadID)
        :
    Experiment(_experimentName,_threadID)
    {
		rawdata = new signed char[MAX_FILE_SIZE];
		rawdataNormalized = new float[MAX_FILE_SIZE];

        boost::mutex::scoped_lock lock(loadFileMutex);

        FILE* rawFile = fopen("C:/Programming/NE/HyperNEAT/out/3nin.smc","rb");

		fileSize = (int)fread(rawdata,1,MAX_FILE_SIZE,rawFile);

        for(int a=0;a<fileSize;a++)
        {
            rawdataNormalized[a] = rawdata[a]/128.0;
        }

		fclose(rawFile);

        layerInfo = NEAT::LayeredSubstrateInfo();

        //Piece input layer (a)
        layerInfo.layerSizes.push_back(JGTL::Vector2<int>(1024,1));
        layerInfo.layerValidSizes.push_back(JGTL::Vector2<int>(1024,1));
        layerInfo.layerNames.push_back("Input");
        layerInfo.layerIsInput.push_back(true);
        layerInfo.layerLocations.push_back(JGTL::Vector3<float>(0,0,0));

        //OutputLayer (e)
        layerInfo.layerSizes.push_back(JGTL::Vector2<int>(1,1));
        layerInfo.layerValidSizes.push_back(JGTL::Vector2<int>(1,1));
        layerInfo.layerNames.push_back("Output");
        layerInfo.layerIsInput.push_back(false);
        layerInfo.layerLocations.push_back(JGTL::Vector3<float>(0,8,0));

        //inputs connect to hidden
        layerInfo.layerAdjacencyList.push_back(std::pair<string,string>("Input","Output"));

        layerInfo.normalize = true;
        layerInfo.useOldOutputNames = false;

        substrate = NEAT::LayeredSubstrate<float>();
        substrate.setLayerInfo(layerInfo);
    }

	BinaryCompressionExperiment::~BinaryCompressionExperiment()
	{
		delete[] rawdata;
        delete[] rawdataNormalized;
	}


    GeneticPopulation* BinaryCompressionExperiment::createInitialPopulation(int populationSize)
    {
        GeneticPopulation *population = new GeneticPopulation();
        vector<GeneticNodeGene> genes;

        genes.push_back(GeneticNodeGene("Bias","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("X1","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("X2","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("Y1","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("Y2","NetworkSensor",0,false));

        genes.push_back(GeneticNodeGene("Output_Input_Output","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));

#if DEBUG_ENABLE_BIASES
        genes.push_back(GeneticNodeGene("Bias_b","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
        genes.push_back(GeneticNodeGene("Bias_c","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
#endif

        for (int a=0;a<populationSize;a++)
        {
            shared_ptr<GeneticIndividual> individual(new GeneticIndividual(genes,true,1.0));

            for (int b=0;b<0;b++)
            {
                individual->testMutate();
            }

            //individual->getNode("Output_2R")->setTopologyFrozen(true);
            //individual->getNode("Output_2R")->setEnabled(false);
            //individual->getNode("Output_2G")->setTopologyFrozen(true);
            //individual->getNode("Output_2G")->setEnabled(false);
            //individual->getNode("Output_2B")->setTopologyFrozen(true);
            //individual->getNode("Output_2B")->setEnabled(false);
            //individual->getNode("Output_2A")->setTopologyFrozen(true);
            //individual->getNode("Output_2A")->setEnabled(false);

            population->addIndividual(individual);
        }

        cout << "Finished creating population\n";
        return population;
    }

    void BinaryCompressionExperiment::populateSubstrate(
        shared_ptr<NEAT::GeneticIndividual> individual
        )
    {
        if (substrateIndividual==individual)
        {
            //Don't bother remaking the same substrate
            return;
        }

        substrateIndividual=individual;
        substrate.populateSubstrate(individual);
    }

    int BEST_DIFFERENCE = 1000000000;

    void BinaryCompressionExperiment::processGroup(shared_ptr<NEAT::GeneticGeneration> generation)
    {
        shared_ptr<NEAT::GeneticIndividual> individual = group.front();

        //cout << "Processing evaluation...\n";
        individual->setFitness(10);

        populateSubstrate(individual);

        const int BYTES_TO_CHECK = fileSize;

        int difference = 0;
        int maxdifference = BYTES_TO_CHECK*256;

        //Window through each of the bytes, setting it as appropriate
        for(int currentByte=0;currentByte<BYTES_TO_CHECK;currentByte++)
        {
            substrate.getNetwork()->reinitialize();
            for(int a=0;a<1024;a++)
            {
                int readIndex = ((currentByte-1024)+a);
                if(readIndex<0)
                {
                    readIndex += fileSize;
                }
                substrate.setValue( Node(a,0,0), rawdataNormalized[readIndex] );
            }
            substrate.getNetwork()->dummyActivation();
            substrate.getNetwork()->update();
            float answer = substrate.getValue( Node(0,0,1) );
            char answerChar = (char)(max(-128.0,min(127.0,answer*128.0)));

            difference += abs(int(answerChar)-int(rawdata[currentByte]));
        }

        if(difference<BEST_DIFFERENCE)
        {
            cout << "Difference: " << difference << " .  Normalized: " << float(difference)/maxdifference << endl;
            BEST_DIFFERENCE = difference;
        }
        float tmpfit = float(maxdifference-difference)/1000.0;
        individual->reward(tmpfit*tmpfit);
    }

    void BinaryCompressionExperiment::processIndividualPostHoc(shared_ptr<NEAT::GeneticIndividual> individual)
    {
    }

    Experiment* BinaryCompressionExperiment::clone()
    {
        BinaryCompressionExperiment* experiment = new BinaryCompressionExperiment(*this);

        return experiment;
    }
}
