#ifndef HCUBE_FINDCLUSTERNOGEOMEXPERIMENT_H_INCLUDED
#define HCUBE_FINDCLUSTERNOGEOMEXPERIMENT_H_INCLUDED

#include "Experiments/HCUBE_FindClusterExperiment.h"

namespace HCUBE
{
    class FindClusterNoGeomExperiment : public Experiment
    {
    public:
    protected:
        NEAT::FastNetwork<double> substrate;
        int numNodesX,numNodesY;

        Vector2<int> user1,user2;
        bool clicked1Last;

        int CELL_SIZE;

        map<pair<int,int>,string> nameLookup;

        int sizeMultiplier;


    public:
        FindClusterNoGeomExperiment(string _experimentName,int _threadID)
                :
                Experiment(_experimentName,_threadID),
                user1(1,1),
                user2(4,4),
                clicked1Last(true),
                CELL_SIZE(20),
                sizeMultiplier(1)
        {
            numNodesY=11;
            numNodesX=11;
            generateSubstrate();
        }

        virtual ~FindClusterNoGeomExperiment()
        {}

        virtual NEAT::GeneticPopulation* createInitialPopulation(int populationSize);

        virtual void generateSubstrate();

        virtual void populateSubstrate(shared_ptr<NEAT::GeneticIndividual> individual);

        double processEvaluation(
            shared_ptr<NEAT::GeneticIndividual> individual,
            wxDC *drawContext,
            int x1,
            int y1,
            int x1Big,
            int y1Big
        );

        virtual void processGroup(shared_ptr<NEAT::GeneticGeneration> generation);

        virtual void processIndividualPostHoc(shared_ptr<NEAT::GeneticIndividual> individual);

#ifndef HCUBE_NOGUI
        virtual bool handleMousePress(wxMouseEvent& event,wxSize &bitmapSize);

        virtual void createIndividualImage(wxDC &drawContext,shared_ptr<NEAT::GeneticIndividual> individual);
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


#endif // HCUBE_FINDCLUSTERNOGEOMEXPERIMENT_H_INCLUDED
