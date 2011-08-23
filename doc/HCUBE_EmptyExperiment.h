#ifndef HCUBE_EMPTYEXPERIMENT_H_INCLUDED
#define HCUBE_EMPTYEXPERIMENT_H_INCLUDED

#include "Experiments/HCUBE_Experiment.h"

namespace HCUBE
{
    class EmptyExperiment : public Experiment
    {
    public:
    protected:
        NEAT::LayeredSubstrate<CheckersNEATDatatype> substrate;

        shared_ptr<const NEAT::GeneticIndividual> substrateIndividual;

    public:
        EmptyExperiment(string _experimentName,int _threadID);

        virtual ~EmptyExperiment();

        virtual NEAT::GeneticPopulation* createInitialPopulation(int populationSize);

        virtual void populateSubstrate(
            shared_ptr<NEAT::GeneticIndividual> individual
        );

        virtual void processGroup(shared_ptr<NEAT::GeneticGeneration> generation);

        virtual void processIndividualPostHoc(shared_ptr<NEAT::GeneticIndividual> individual);

#ifndef HCUBE_NOGUI
        virtual void createIndividualImage(wxDC &drawContext,shared_ptr<NEAT::GeneticIndividual> individual);

        virtual bool handleMousePress(wxMouseEvent& event,wxSize &bitmapSize);
#endif

        virtual inline bool performUserEvaluations()
        {
            return PUT_TRUE_ONLY_IF_DOING_INTERACTIVE_EVOLUTION;
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

#endif

