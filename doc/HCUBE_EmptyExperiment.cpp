#include "HCUBE_Defines.h"

#include "Experiments/HCUBE_EmptyExperiment.h"

#define DEBUG_DIRECT_LINKS (0)

#define DEBUG_USE_ABSOLUTE (1)

#define DEBUG_USE_DELTAS (1)

#define DEBUG_ENABLE_BIASES (0)

#define DO_REGULAR_RUN_FOR_POSTHOC (1)

namespace HCUBE
{
    using namespace NEAT;

    EmptyExperiment::EmptyExperiment(string _experimentName,int _threadID)
        :
    Experiment(_experimentName,_threadID)
    {
	MAKE_AN_ENTRY_FOR_EACH_LAYER

        layerInfo.layerSizes.push_back(Vector2<int>(PUT_LAYER_WIDTH,PUT_LAYER_LENGTH));
        layerInfo.layerIsInput.push_back( PUT_TRUE_ONLY_IF_LAYER_IS_INPUT_LAYER_AND_ACTIVATION_LEVELS_ARE_USER_DEFINED );
        layerInfo.layerLocations.push_back(Vector3<float>(PUT_LAYER_LOCATION_IN_3D_FOR_SUBSTRATE_VISUALIZER));
        layerInfo.layerNames.push_back( GIVE_THE_LAYER_A_UNIQUE_NAME );

	THIS_IS_AN_EXAMPLE:
        layerInfo.layerSizes.push_back(Vector2<int>(8,8));
        layerInfo.layerIsInput.push_back(false);
        layerInfo.layerLocations.push_back(Vector3<float>(0,4,0));
        layerInfo.layerNames.push_back("Processing");


	ADD_ADJACENCY_INFORMATION_FOR_THE_CREATION_OF_LINKS
        layerInfo.layerAdjacencyList.push_back(std::pair<string,string>(SOURCE_LAYER, DESTINATION_LAYER));

	THIS_IS_AN_EXAMPLE:
        layerInfo.layerAdjacencyList.push_back(std::pair<string,string>("Input","Processing"));

	PUT_TRUE_IF_ALL_INCOMING_LINK_WEIGHTS_ARE_NORMALIZED
        layerInfo.normalize = false;

	THIS_SHOULD_ONLY_BE_TRUE_FOR_LEGACY_EXPERIMENTS
        layerInfo.useOldOutputNames = false;

	THIS_CAN_BE_USED_FOR_DISCRETE_SUBSTRATE_EXTRAPOLATION
	layerInfo.layerValidSizes = layerInfo.layerSizes;

        substrates.setLayerInfo(layerInfo);
    }

    EmptyExperiment::~EmptyExperiment()
    {
    }

    GeneticPopulation* EmptyExperiment::createInitialPopulation(int populationSize)
    {
        GeneticPopulation *population = new GeneticPopulation();
        vector<GeneticNodeGene> genes;

        genes.push_back(GeneticNodeGene("Bias","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("X1","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("Y1","NetworkSensor",0,false));
#if DEBUG_USE_ABSOLUTE
        genes.push_back(GeneticNodeGene("X2","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("Y2","NetworkSensor",0,false));
#endif
#if DEBUG_USE_DELTAS
        genes.push_back(GeneticNodeGene("DeltaX","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("DeltaY","NetworkSensor",0,false));
#endif
        genes.push_back(GeneticNodeGene("Output_ab","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
        genes.push_back(GeneticNodeGene("Output_bc","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
#if DEBUG_DIRECT_LINKS
        genes.push_back(GeneticNodeGene("Output_ac","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
#endif
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

            population->addIndividual(individual);
        }

        cout << "Finished creating population\n";
        return population;
    }

    void CheckersExperiment::populateSubstrate(
        shared_ptr<NEAT::GeneticIndividual> individual
        )
    {
        if (substrateIndividual==individual)
        {
            //Don't bother remaking the same substrate
            return;
        }

        substrateIndividual=individual;

        substrate->populateSubstrate(individual);
    }

    void CheckersExperiment::processGroup(shared_ptr<NEAT::GeneticGeneration> generation)
    {
        //cout << "Processing evaluation...\n";
        shared_ptr<NEAT::GeneticIndividual> individual = group.front();
        //You get 10 points just for being processed, wahooo!
        individual->setFitness(10);

        populateSubstrate(individual);

	USE_THE_SUBSTRATE_TO_EVALUATE_THE_INDIVIDUAL_AND_ASSIGN_A_FITNESS_WITH_THE_INDIVIDUAL::REWARD(...)_FUNCTION
    }

    void CheckersExperiment::processIndividualPostHoc(shared_ptr<NEAT::GeneticIndividual> individual)
    {
#if DO_REGULAR_RUN_FOR_POSTHOC
        //cout << "INDIVIDUAL FITNESS BEFORE: " << individual->getFitness() << endl;
        clearGroup();
        addIndividualToGroup(individual);
        individual->setUserData(CheckersStats().toString());

        shared_ptr<GeneticGeneration> dummy;
        processGroup(dummy);
#else
        //cout << "INDIVIDUAL FITNESS BEFORE: " << individual->getFitness() << endl;
        clearGroup();
        addIndividualToGroup(individual);
        individual->setUserData(shared_ptr<CheckersStats>(new CheckersStats()));

	THIS_SECTION_WILL_DO_VALIDATION_POST_HOC
	THE_DEFAULT_IS_TO_RUN_100_EVALUATIONS

        for (int a=0;a<100;a++)
        {
            shared_ptr<GeneticGeneration> dummy;
            processGroup(dummy);
            //cout << "INDIVIDUAL FITNESS: " << individual->getFitness() << endl;
        }
#endif
    }

#ifndef HCUBE_NOGUI
    void CheckersExperiment::createIndividualImage(wxDC &drawContext,shared_ptr<NEAT::GeneticIndividual> individual)
    {
        if (lastIndividualSeen!=individual)
        {
            //cout << "Repopulating substrate\n";
            populateSubstrate(individual);
            lastIndividualSeen = individual;
        }

        drawContext.Clear();

	THIS_SECTION_DRAWS_THE_GUI_FOR_A_PARTICULAR_EXPERIMENT
    }

    bool CheckersExperiment::handleMousePress(wxMouseEvent& event,wxSize &bitmapSize)
    {
	THIS_SECTION_HANDLES_A_GUI_MOUSE_PRESS

        return true;
    }
#endif

    Experiment* EmptyExperiment::clone()
    {
	THIS_SECTION_CLONES_THE_EXPERIMENT_SO_IT_CAN_BE_RUN_BY_MULTIPLE_THREADS_IN_PARALLEL

        EmptyExperiment* experiment = new EmptyExperiment(*this);

        return experiment;
    }

    void EmptyExperiment::resetGenerationData(shared_ptr<NEAT::GeneticGeneration> generation)
    {
        THIS_SECTION_CLEARS_GENERATION_STATISTICS
    }

    void EmptyExperiment::addGenerationData(shared_ptr<NEAT::GeneticGeneration> generation,shared_ptr<NEAT::GeneticIndividual> individual)
    {
        THIS_SECTION_UPDATES_GENERATION_STATISTICS_AFTER_EACH_INDIVIDUAL_EVALUATION
    }
}
