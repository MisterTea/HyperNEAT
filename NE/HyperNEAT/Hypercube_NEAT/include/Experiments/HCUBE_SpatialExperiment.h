#ifndef HCUBE_SPATIALEXPERIMENT_H_INCLUDED
#define HCUBE_SPATIALEXPERIMENT_H_INCLUDED

#ifndef DOXYGEN_PROCESSOR

#include "Experiments/HCUBE_Experiment.h"

//#include "HCUBE_Vector2.h"

#define SPATIAL_USE_VECTOR_NETWORK (1)

namespace HCUBE
{
    class SpatialExperiment : public Experiment
    {
    public:
    protected:
#ifdef SPATIAL_USE_VECTOR_NETWORK
        NEAT::VectorNetwork<double> substrate;
#else
        NEAT::FastNetwork<double> substrate;
#endif
        int resolution;

    public:
        SpatialExperiment(string _experimentName,int _threadID);

        virtual ~SpatialExperiment()
        {}

        virtual NEAT::GeneticPopulation* createInitialPopulation(int populationSize);

        double processEvaluation(
            shared_ptr<NEAT::GeneticIndividual> individual,
            wxDC *drawContext
        );

        virtual void processGroup(shared_ptr<NEAT::GeneticGeneration> generation);

#ifndef HCUBE_NOGUI
        virtual bool handleMousePress(wxMouseEvent& event,wxSize &bitmapSize);

        virtual void createIndividualImage(wxDC &drawContext,shared_ptr<NEAT::GeneticIndividual> individual);

        virtual bool handleMouseMotion(wxMouseEvent& event,wxDC &temp_dc,shared_ptr<NEAT::GeneticIndividual> individual);
#endif

        virtual inline bool performUserEvaluations()
        {
            return true;
        }

        virtual inline bool isDisplayGenerationResult()
        {
            return true;
        }

        void increaseResolution();

        void decreaseResolution();

        virtual Experiment* clone();
    };
}

#endif

#endif // HCUBE_SPATIALEXPERIMENT_H_INCLUDED
