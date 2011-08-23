#include "HCUBE_Defines.h"

#include "Experiments/HCUBE_CheckersExperimentSubstrateGeom.h"

#define CHECKERS_EXPERIMENT_DEBUG (0)

namespace HCUBE
{
    using namespace NEAT;

    CheckersExperimentSubstrateGeom::CheckersExperimentSubstrateGeom(string _experimentName,int _threadID)
            :
            CheckersExperiment(_experimentName,_threadID)
    {
        generateSubstrate();
    }

    GeneticPopulation* CheckersExperimentSubstrateGeom::createInitialPopulation(int populationSize)
    {
        GeneticPopulation *population = new GeneticPopulation();
        vector<GeneticNodeGene> genes;
        vector<GeneticLinkGene> linkGenes;

        genes.push_back(GeneticNodeGene("Bias","NetworkSensor",0,false));

		//Create the input layer
		int inputStartID=-1;
		int inputEndID;
        for (int x=0;x<8;x++)
        {
            for (int y=0;y<8;y++)
            {
                if ((x+y)%2==0)
                {
                    Node node(x,y,0);
                    //cout << (y1-numNodesY/2) << '/' << (x1-numNodesX/2) << endl;
                    string name = (toString(x)+string("/")+toString(y) + string("/") + toString(0));
                    nameLookup[node] = name;
                    genes.push_back(GeneticNodeGene(name,"NetworkSensor",0,false));
					genes.back().setTopologyFrozen(true);
					if(inputStartID==-1)
						inputStartID = genes.back().getID();

					//set inputEndID all the time to ensure we set it to the last one
					inputEndID = genes.back().getID();
                }
            }
        }

		//Create the output layer
		Node node(0,0,2);
        //cout << (y1-numNodesY/2) << '/' << (x1-numNodesX/2) << endl;
        string name = (toString(0)+string("/")+toString(0) + string("/") + toString(2));
        nameLookup[node] = name;
        genes.push_back(GeneticNodeGene(name,"NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
		genes.back().setTopologyFrozen(true);
		int outputNodeID = genes.back().getID();

		//Create the hidden layer
        for (int x=0;x<8;x++)
        {
            for (int y=0;y<8;y++)
            {
                Node node(x,y,1);
                //cout << (y1-numNodesY/2) << '/' << (x1-numNodesX/2) << endl;
                string name = (toString(x)+string("/")+toString(y) + string("/") + toString(1));
                nameLookup[node] = name;
                genes.push_back(GeneticNodeGene(name,"HiddenNode",0,false));
				genes.back().setTopologyFrozen(true);
				int curNodeID = genes.back().getID();

				//While creating the hidden layer, connect up the links
				linkGenes.push_back(GeneticLinkGene(curNodeID,outputNodeID,0.0));

				for (int curInputID = inputStartID;curInputID <= inputEndID;curInputID++)
				{
					linkGenes.push_back(GeneticLinkGene(curInputID,curNodeID,0.0));
				}
			}
        }

        for (int a=0;a<populationSize;a++)
        {
            shared_ptr<GeneticIndividual> individual(new GeneticIndividual(genes,linkGenes,true,1.0));

            population->addIndividual(individual);
        }

        cout << "Finished creating population\n";
        return population;
    }

    void CheckersExperimentSubstrateGeom::generateSubstrate(int substrateNum)
    {}

    void CheckersExperimentSubstrateGeom::populateSubstrate(
        shared_ptr<NEAT::GeneticIndividual> individual,
        int substrateNum
    )
    {
        if (substrateIndividuals[substrateNum]==individual)
        {
            //Don't bother remaking the same substrate
            return;
        }

        substrateIndividuals[substrateNum]=individual;

        //Clear the evaluation cache
        for (int a=0;a<65536;a++)
        {
            boardEvaluationCaches[substrateNum][a].clear();
        }

        //Clear the state list caches
        for (int a=0;a<65536;a++)
        {
            boardStateLists[substrateNum][BLACK][a].clear();
        }
        for (int a=0;a<65536;a++)
        {
            boardStateLists[substrateNum][WHITE][a].clear();
        }

        networks[substrateNum] = individual->spawnFastPhenotypeStack<CheckersNEATDatatype>();
    }

    Experiment* CheckersExperimentSubstrateGeom::clone()
    {
        CheckersExperimentSubstrateGeom* experiment = new CheckersExperimentSubstrateGeom(*this);

        return experiment;
    }
}
