#include "HCUBE_Defines.h"

#include "Experiments/HCUBE_ImageCompressionExperiment.h"

#define INPUT_PPM "waveNM_diff.ppm"

#define DIFF_PPM "waveNM_diff2.ppm"

#define OUTPUT_PPM "waveNM_output2.ppm"

#define FITNESS_THRESHOLD (9000)

namespace HCUBE
{
	using namespace NEAT;

	ImageCompressionExperiment::ImageCompressionExperiment(string _experimentName,int _threadID)
		:
	Experiment(_experimentName,_threadID),
		numNodesX(256),
		numNodesY(256)
	{
		smallest.r = smallest.g = smallest.b = 0;

		biggest.r = biggest.g = biggest.b = 255;

		ifstream infile(INPUT_PPM);
		string line;
		for(int a=0;a<4;a++)
		{
			//skip header
			getline(infile,line);
		}

		for(int y=0;y<256;y++)
		{
			for(int x=0;x<256;x++)
			{
				infile >> rawimage[y][x].r >> rawimage[y][x].g >> rawimage[y][x].b;

				smallest.r = min(smallest.r,rawimage[y][x].r);
				smallest.g = min(smallest.g,rawimage[y][x].g);
				smallest.b = min(smallest.b,rawimage[y][x].b);

				biggest.r = max(biggest.r,rawimage[y][x].r);
				biggest.g = max(biggest.g,rawimage[y][x].g);
				biggest.b = max(biggest.b,rawimage[y][x].b);
			}
		}

		spread.r = biggest.r-smallest.r;
		spread.g = biggest.g-smallest.g;
		spread.b = biggest.b-smallest.b;

		//Normalize the image to the new scale
		/*
		for(int y=0;y<256;y++)
		{
		for(int x=0;x<256;x++)
		{
		rawimage[y][x].r -= smallest.r;
		rawimage[y][x].g -= smallest.g;
		rawimage[y][x].b -= smallest.b;

		if(spread.r)
		{
		rawimage[y][x].r = (rawimage[y][x].r*255)/spread.r;
		}
		else
		{
		rawimage[y][x].r=0;
		}

		if(spread.g)
		{
		rawimage[y][x].g = (rawimage[y][x].g*255)/spread.g;
		}
		else
		{
		rawimage[y][x].g=0;
		}

		if(spread.b)
		{
		rawimage[y][x].b = (rawimage[y][x].b*255)/spread.b;
		}
		else
		{
		rawimage[y][x].b=0;
		}
		}
		}
		*/
	}

	GeneticPopulation* ImageCompressionExperiment::createInitialPopulation(int populationSize)
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

	void ImageCompressionExperiment::populateSubstrate(
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

		double totalFitness = 0;

		Pixel biggestSpread;
		biggestSpread.r = biggestSpread.g = biggestSpread.b = 0;

		for (int y=0;y<numNodesY;y++)
		{
			for (int x=0;x<numNodesX;x++)
			{
				/*Remap the nodes to the [-1,1] domain*/
				float xnormal,ynormal;

				if (numNodesX>1)
				{
					xnormal = -1.0 + (float(x)/(numNodesX-1))*2.0;
				}
				else
				{
					xnormal = 0.0;
				}

				if (numNodesY>1)
				{
					ynormal = -1.0 + (float(y)/(numNodesY-1))*2.0;
				}
				else
				{
					ynormal = 0.0;
				}

				network.reinitialize();
				network.setValue("X",xnormal);
				network.setValue("Y",ynormal);

				if(network.hasNode("Bias"))
				{
					network.setValue("Bias",(float)0.3);
				}

				network.update();

				{
					float unsignedVal = (network.getValue("Output_1R")+1.0)/2.0;
					tmpimage[y][x].r = smallest.r + unsignedVal*(spread.r);
				}
				{
					float unsignedVal = (network.getValue("Output_1G")+1.0)/2.0;
					tmpimage[y][x].g = smallest.g + unsignedVal*(spread.g);
				}
				{
					float unsignedVal = (network.getValue("Output_1B")+1.0)/2.0;
					tmpimage[y][x].b = smallest.b + unsignedVal*(spread.b);
				}
				//tmpimage[y][x].a = 128 + int(network.getValue("Output_1A")*128);

				totalFitness += 256-abs(rawimage[y][x].r-tmpimage[y][x].r);
				totalFitness += 256-abs(rawimage[y][x].g-tmpimage[y][x].g);
				totalFitness += 256-abs(rawimage[y][x].b-tmpimage[y][x].b);
				//totalFitness += 256-fabs(rawimage[y][x].a-tmpimage[y][x].a);
			}
		}

		individual->setFitness(totalFitness/256.0/256.0/256.0/3.0*10000.0);

		if(individual->getFitness()>FITNESS_THRESHOLD)
		{
			{
				//Write out the PPM
				ofstream outfile(OUTPUT_PPM);

				outfile << "P3\n# CREATOR: The GIMP's PNM Filter Version 1.0\n256 256\n255\n";

				for(int y=0;y<256;y++)
				{
					for(int x=0;x<256;x++)
					{
#if 0
						outfile 
							<< abs(rawimage[y][x].r-tmpimage[y][x].r) << "\n" 
							<< abs(rawimage[y][x].g-tmpimage[y][x].g) << "\n" 
							<< abs(rawimage[y][x].b-tmpimage[y][x].b) << "\n";
#else
						outfile 
							<< (tmpimage[y][x].r) << "\n" 
							<< (tmpimage[y][x].g) << "\n" 
							<< (tmpimage[y][x].b) << "\n";
#endif
					}
				}

				outfile.flush();
			}

			{
				//Write out the PPM
				ofstream outfile(DIFF_PPM);

				outfile << "P3\n# CREATOR: The GIMP's PNM Filter Version 1.0\n256 256\n255\n";

				for(int y=0;y<256;y++)
				{
					for(int x=0;x<256;x++)
					{
#if 0
						outfile 
							<< abs(rawimage[y][x].r-tmpimage[y][x].r) << "\n" 
							<< abs(rawimage[y][x].g-tmpimage[y][x].g) << "\n" 
							<< abs(rawimage[y][x].b-tmpimage[y][x].b) << "\n";
#else
						outfile 
							<< (rawimage[y][x].r-tmpimage[y][x].r) << "\n" 
							<< (rawimage[y][x].g-tmpimage[y][x].g) << "\n" 
							<< (rawimage[y][x].b-tmpimage[y][x].b) << "\n";
#endif
					}
				}

				outfile.flush();
			}

			exit(1);
		}
	}

	void ImageCompressionExperiment::processGroup(shared_ptr<NEAT::GeneticGeneration> generation)
	{
		populateSubstrate(group[0]);

		/*
		if(generation->getGenerationNumber()==9)
		{
		cout << "LOCKING TOPOLOGY\n";
		shared_ptr<NEAT::GeneticIndividual> individual = group[0];

		//Lock the topology

		for(int a=0;a<individual->getNodesCount();a++)
		{
		if(!iequals(individual->getNode(a)->getType(),"NetworkSensor"))
		{
		if(
		!iequals(individual->getNode(a)->getType(),"NetworkOutputNode") ||
		(
		iequals(individual->getNode(a)->getName(),"Output_1R") ||
		iequals(individual->getNode(a)->getName(),"Output_1G") ||
		iequals(individual->getNode(a)->getName(),"Output_1B") //||
		//iequals(individual->getNode(a)->getName(),"Output_1A")
		)                        
		)
		{
		//Freeze hidden nodes and select output nodes
		individual->getNode(a)->setTopologyFrozen(true);
		}
		}
		}

		individual->getNode("Output_2R")->setTopologyFrozen(false);
		individual->getNode("Output_2R")->setEnabled(true);
		individual->getNode("Output_2G")->setTopologyFrozen(false);
		individual->getNode("Output_2G")->setEnabled(true);
		individual->getNode("Output_2B")->setTopologyFrozen(false);
		individual->getNode("Output_2B")->setEnabled(true);
		//individual->getNode("Output_2A")->setTopologyFrozen(false);
		//individual->getNode("Output_2A")->setEnabled(true);

		for(int a=0;a<individual->getLinksCount();a++)
		{
		individual->getLink(a)->setFixed(true);
		}
		}
		*/
	}

	void ImageCompressionExperiment::processIndividualPostHoc(shared_ptr<NEAT::GeneticIndividual> individual)
	{
		populateSubstrate(individual);

		{
			//Write out the PPM
			ofstream outfile(OUTPUT_PPM);

			outfile << "P3\n# CREATOR: The GIMP's PNM Filter Version 1.0\n256 256\n255\n";

			for(int y=0;y<256;y++)
			{
				for(int x=0;x<256;x++)
				{
#if 0
					outfile 
						<< abs(rawimage[y][x].r-tmpimage[y][x].r) << "\n" 
						<< abs(rawimage[y][x].g-tmpimage[y][x].g) << "\n" 
						<< abs(rawimage[y][x].b-tmpimage[y][x].b) << "\n";
#else
					outfile 
						<< (tmpimage[y][x].r) << "\n" 
						<< (tmpimage[y][x].g) << "\n" 
						<< (tmpimage[y][x].b) << "\n";
#endif
				}
			}

			outfile.flush();
		}

		{
			//Write out the PPM
			ofstream outfile(DIFF_PPM);

			outfile << "P3\n# CREATOR: The GIMP's PNM Filter Version 1.0\n256 256\n255\n";

			for(int y=0;y<256;y++)
			{
				for(int x=0;x<256;x++)
				{
#if 0
					outfile 
						<< abs(rawimage[y][x].r-tmpimage[y][x].r) << "\n" 
						<< abs(rawimage[y][x].g-tmpimage[y][x].g) << "\n" 
						<< abs(rawimage[y][x].b-tmpimage[y][x].b) << "\n";
#else
					outfile 
						<< (rawimage[y][x].r-tmpimage[y][x].r) << "\n" 
						<< (rawimage[y][x].g-tmpimage[y][x].g) << "\n" 
						<< (rawimage[y][x].b-tmpimage[y][x].b) << "\n";
#endif
				}
			}

			outfile.flush();
		}
	}

	Experiment* ImageCompressionExperiment::clone()
	{
		ImageCompressionExperiment* experiment = new ImageCompressionExperiment(*this);

		return experiment;
	}

	void ImageCompressionExperiment::resetGenerationData(shared_ptr<NEAT::GeneticGeneration> generation)
	{
		//generation->setUserData(new TicTacToeStats());
	}

	void ImageCompressionExperiment::addGenerationData(shared_ptr<NEAT::GeneticGeneration> generation,shared_ptr<NEAT::GeneticIndividual> individual)
	{
		//TicTacToeStats* ticTacToeStats = (TicTacToeStats*)generation->getUserData();

		//if (individual->getUserData())
		//(*ticTacToeStats) += *((TicTacToeStats*)individual->getUserData());
	}
}
