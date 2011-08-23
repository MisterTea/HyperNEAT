#include "HCUBE_Defines.h"

#include "HCUBE_Defines.h"

#include "Experiments/HCUBE_LogicBuilderExperiment.h"

namespace HCUBE
{
	using namespace NEAT;

	LogicBuilderExperiment::LogicBuilderExperiment(string _experimentName,int _threadID)
		:
	Experiment(_experimentName,_threadID)
	{
	}

	GeneticPopulation* LogicBuilderExperiment::createInitialPopulation(int populationSize)
	{
		GeneticPopulation *population = new GeneticPopulation();
		vector<GeneticNodeGene> genes;

		genes.push_back(GeneticNodeGene("Bias","NetworkSensor",0,false));
		genes.push_back(GeneticNodeGene("X","NetworkSensor",0,false));
		genes.push_back(GeneticNodeGene("Y","NetworkSensor",0,false));

		genes.push_back(GeneticNodeGene("Output_1R","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
		genes.push_back(GeneticNodeGene("Output_1G","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
		genes.push_back(GeneticNodeGene("Output_1B","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
		//genes.push_back(GeneticNodeGene("Output_1A","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));

		//genes.push_back(GeneticNodeGene("Output_2R","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
		//genes.push_back(GeneticNodeGene("Output_2G","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
		//genes.push_back(GeneticNodeGene("Output_2B","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
		//genes.push_back(GeneticNodeGene("Output_2A","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));

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

	void LogicBuilderExperiment::populateSubstrate(
		shared_ptr<NEAT::GeneticIndividual> individual
		)
	{
		if (substrateIndividual==individual)
		{
			//Don't bother remaking the same substrate
			return;
		}

		substrateIndividual=individual;

		NEAT::FastNetwork<float> network = individual->spawnFastPhenotypeStack<float>();
	}

	void LogicBuilderExperiment::processGroup(shared_ptr<NEAT::GeneticGeneration> generation)
	{
		populateSubstrate(group[0]);
	}

	void LogicBuilderExperiment::processIndividualPostHoc(shared_ptr<NEAT::GeneticIndividual> individual)
	{
	}

	Experiment* LogicBuilderExperiment::clone()
	{
		LogicBuilderExperiment* experiment = new LogicBuilderExperiment(*this);

		return experiment;
	}

	void LogicBuilderExperiment::resetGenerationData(shared_ptr<NEAT::GeneticGeneration> generation)
	{
		//generation->setUserData(new TicTacToeStats());
	}

	void LogicBuilderExperiment::addGenerationData(shared_ptr<NEAT::GeneticGeneration> generation,shared_ptr<NEAT::GeneticIndividual> individual)
	{
		//TicTacToeStats* ticTacToeStats = (TicTacToeStats*)generation->getUserData();

		//if (individual->getUserData())
		//(*ticTacToeStats) += *((TicTacToeStats*)individual->getUserData());
	}
}
