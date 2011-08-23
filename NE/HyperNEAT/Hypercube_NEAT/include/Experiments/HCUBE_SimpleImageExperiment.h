#ifndef HCUBE_SIMPLEIMAGEEXPERIMENT_H_INCLUDED
#define HCUBE_SIMPLEIMAGEEXPERIMENT_H_INCLUDED

#ifndef DOXYGEN_PROCESSOR

#include "Experiments/HCUBE_Experiment.h"

namespace HCUBE
{

    class SimpleImageExperiment : public Experiment
    {
    public:
    protected:
        NEAT::FastNetwork<double> network2,network3;
        int numNodesX,numNodesY;
        int renderIterations;

    public:
        SimpleImageExperiment(string _experimentName,int _threadID)
                :
                Experiment(_experimentName,_threadID),
                renderIterations(0)
        {}

        virtual ~SimpleImageExperiment()
        {}

        virtual NEAT::GeneticPopulation* createInitialPopulation(int populationSize);

        virtual void processGroup(shared_ptr<NEAT::GeneticGeneration> generation);

#ifndef HCUBE_NOGUI
        virtual void createIndividualImage(wxDC &drawContext,shared_ptr<NEAT::GeneticIndividual> individual);

        virtual bool handleMousePress(wxMouseEvent& event,wxSize &bitmapSize)
        {
            return false;
        }
#endif

        virtual inline bool performUserEvaluations()
        {
            return false;
        }

        virtual inline bool isDisplayGenerationResult()
        {
            return false;
        }

        inline void setRenderIterations(int _renderIterations)
        {
            renderIterations = _renderIterations;
        }

        virtual Experiment* clone();
    };

}


#endif

#endif // HCUBE_SIMPLEIMAGEEXPERIMENT_H_INCLUDED
