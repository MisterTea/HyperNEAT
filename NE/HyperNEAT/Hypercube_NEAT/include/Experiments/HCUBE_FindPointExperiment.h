#ifndef HCUBE_FINDPOINTEXPERIMENT_H_INCLUDED
#define HCUBE_FINDPOINTEXPERIMENT_H_INCLUDED

#ifndef DOXYGEN_PROCESSOR

#include "Experiments/HCUBE_Experiment.h"

namespace HCUBE
{
    class FindPointExperiment : public Experiment
    {
    public:
    protected:
        NEAT::FastNetwork<double> substrate;
        int numNodesX,numNodesY;

    public:
        FindPointExperiment(string _experimentName,int _threadID)
                :
                Experiment(_experimentName,_threadID)
        {}

        virtual ~FindPointExperiment()
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

        virtual Experiment* clone();
    };
}

#endif

#endif // HCUBE_FINDPOINTEXPERIMENT_H_INCLUDED
