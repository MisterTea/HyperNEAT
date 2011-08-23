#ifndef HCUBE_SPATIALCASINOEXPERIMENT_H_INCLUDED
#define HCUBE_SPATIALCASINOEXPERIMENT_H_INCLUDED

#include "Experiments/HCUBE_Experiment.h"

#define CASINO_ROWS    (16)
#define CASINO_COLS   (16)

#define NUM_TRIALS (10)

#define TRIAL_LENGTH (1000)

#define PAYOFF_STEPS (20000)

#define TOTAL_RANDOM_NUMBERS (CASINO_ROWS*CASINO_COLS*NUM_TRIALS*TRIAL_LENGTH)

namespace HCUBE
{
    class SpatialCasinoExperiment : public Experiment
    {
    public:
    protected:
        NEAT::FastNetwork<double> substrate;

        float probabilityForPlay[CASINO_ROWS][CASINO_COLS];
        float payoffFunction[PAYOFF_STEPS+1];

        float* randomProbabilitiesForPlay;
        float* randomPayoffs;

    public:
        SpatialCasinoExperiment(string _experimentName,int _threadID);

        virtual ~SpatialCasinoExperiment();

        virtual NEAT::GeneticPopulation* createInitialPopulation(int populationSize);

        virtual void processGroup(shared_ptr<NEAT::GeneticGeneration> generation);

        virtual void processIndividualPostHoc(shared_ptr<NEAT::GeneticIndividual> individual);

#ifndef HCUBE_NOGUI
        virtual bool handleMousePress(wxMouseEvent& event,wxSize &bitmapSize);

        virtual void createIndividualImage(wxDC &drawContext,shared_ptr<NEAT::GeneticIndividual> individual);

        virtual bool handleMouseMotion(wxMouseEvent& event,wxDC &temp_dc,shared_ptr<NEAT::GeneticIndividual> individual);
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


#endif // HCUBE_SPATIALCASINOEXPERIMENT_H_INCLUDED
