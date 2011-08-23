#ifndef HCUBE_ROBOTARMLATTICEEXPERIMENT_H_INCLUDED
#define HCUBE_ROBOTARMLATTICEEXPERIMENT_H_INCLUDED

#ifndef DOXYGEN_PROCESSOR

#include "Experiments/HCUBE_Experiment.h"

namespace HCUBE
{

    class RobotArmLatticeExperiment : public Experiment
    {
    public:
    protected:
        double userx,usery,scale;


    public:
        RobotArmLatticeExperiment(string _experimentName,int _threadID);

        virtual ~RobotArmLatticeExperiment()
        {}

        virtual NEAT::GeneticPopulation* createInitialPopulation(int populationSize);

        virtual void processGroup(shared_ptr<NEAT::GeneticGeneration> generation);

        virtual double processEvaluation(shared_ptr<const NEAT::GeneticIndividual> individual,wxDC *drawContext,double x,double y);

        virtual double processEvaluation(shared_ptr<const NEAT::GeneticIndividual> individual,double x,double y);

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

#endif // HCUBE_ROBOTARMLATTICEEXPERIMENT_H_INCLUDED
