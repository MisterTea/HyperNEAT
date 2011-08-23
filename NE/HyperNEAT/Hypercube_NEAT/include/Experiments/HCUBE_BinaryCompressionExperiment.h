#ifndef HCUBE_BINARYCOMPRESSIONEXPERIMENT_H_INCLUDED
#define HCUBE_BINARYCOMPRESSIONEXPERIMENT_H_INCLUDED

#include "Experiments/HCUBE_Experiment.h"

namespace HCUBE
{
    class BinaryCompressionExperiment : public Experiment
    {
    public:
    protected:
#if CHECKERS_EXPERIMENT_ENABLE_BIASES
        NEAT::LayeredSubstrate<float> substrate;
#else
        NEAT::LayeredSubstrate<float> substrate;
#endif

		shared_ptr<const NEAT::GeneticIndividual> substrateIndividual;

        signed char* rawdata;
        float* rawdataNormalized;
        int fileSize;

        NEAT::LayeredSubstrateInfo layerInfo;

    public:
        BinaryCompressionExperiment(string _experimentName,int _threadID);

        virtual ~BinaryCompressionExperiment();

        virtual NEAT::GeneticPopulation* createInitialPopulation(int populationSize);

        virtual void populateSubstrate(
            shared_ptr<NEAT::GeneticIndividual> individual
        );

        virtual void processGroup(shared_ptr<NEAT::GeneticGeneration> generation);

        virtual void processIndividualPostHoc(shared_ptr<NEAT::GeneticIndividual> individual);
/*
#ifndef HCUBE_NOGUI
        virtual void createIndividualBinary(wxDC &drawContext,shared_ptr<NEAT::GeneticIndividual> individual);

        virtual bool handleMousePress(wxMouseEvent& event,wxSize &bitmapSize);
#endif
*/
        virtual inline bool performUserEvaluations()
        {
            return false;
        }

        virtual inline bool isDisplayGenerationResult()
        {
            return false;
        }

        virtual Experiment* clone();
    };

}

#endif // HCUBE_TICTACTOEGAMEEXPERIMENT_H_INCLUDED

