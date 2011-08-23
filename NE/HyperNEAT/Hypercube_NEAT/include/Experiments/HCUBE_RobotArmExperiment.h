#ifndef HCUBE_ROBOTARMEXPERIMENT_H_INCLUDED
#define HCUBE_ROBOTARMEXPERIMENT_H_INCLUDED

#ifndef DOXYGEN_PROCESSOR

#include "Experiments/HCUBE_Experiment.h"

namespace HCUBE
{

    class RobotArmExperiment : public Experiment
    {
    public:
    protected:
        double userx,usery,scale;


    public:
        RobotArmExperiment(string _experimentName,int _threadID);

        virtual ~RobotArmExperiment()
        {}

        virtual NEAT::GeneticPopulation* createInitialPopulation(int populationSize);

        virtual void processGroup(shared_ptr<NEAT::GeneticGeneration> generation);

#ifndef HCUBE_NOGUI
        virtual void createIndividualImage(wxDC &drawContext,shared_ptr<NEAT::GeneticIndividual> individual);

        virtual bool handleMousePress(wxMouseEvent& event,wxSize &bitmapSize);
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

#endif // HCUBE_ROBOTARMEXPERIMENT_H_INCLUDED
