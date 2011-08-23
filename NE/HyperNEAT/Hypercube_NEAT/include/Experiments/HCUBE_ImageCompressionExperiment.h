#ifndef HCUBE_IMAGECOMPRESSIONEXPERIMENT_H_INCLUDED
#define HCUBE_IMAGECOMPRESSIONEXPERIMENT_H_INCLUDED

#include "Experiments/HCUBE_Experiment.h"

namespace HCUBE
{
    struct Pixel
    {
        int r,g,b;
        //int a;
    };

    class ImageCompressionExperiment : public Experiment
    {
    public:
    protected:
#if CHECKERS_EXPERIMENT_ENABLE_BIASES
        NEAT::FastBiasNetwork<float> substrate;
#else
        NEAT::FastNetwork<float> substrate;
#endif

        Pixel smallest,biggest,spread;

		shared_ptr<const NEAT::GeneticIndividual> substrateIndividual;

        int numNodesX,numNodesY;

        Pixel rawimage[256][256];

        Pixel tmpimage[256][256];

    public:
        ImageCompressionExperiment(string _experimentName,int _threadID);

        virtual ~ImageCompressionExperiment()
        {}

        virtual NEAT::GeneticPopulation* createInitialPopulation(int populationSize);

        virtual void populateSubstrate(
            shared_ptr<NEAT::GeneticIndividual> individual
        );

        virtual void processGroup(shared_ptr<NEAT::GeneticGeneration> generation);

        virtual void processIndividualPostHoc(shared_ptr<NEAT::GeneticIndividual> individual);
/*
#ifndef HCUBE_NOGUI
        virtual void createIndividualImage(wxDC &drawContext,shared_ptr<NEAT::GeneticIndividual> individual);

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

        virtual void resetGenerationData(shared_ptr<NEAT::GeneticGeneration> generation);

        virtual void addGenerationData(shared_ptr<NEAT::GeneticGeneration> generation,shared_ptr<NEAT::GeneticIndividual> individual);
    };

}

#endif // HCUBE_TICTACTOEGAMEEXPERIMENT_H_INCLUDED

