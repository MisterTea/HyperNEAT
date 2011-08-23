#ifndef HCUBE_TICTACTOEEXPERIMENT_H_INCLUDED
#define HCUBE_TICTACTOEEXPERIMENT_H_INCLUDED

#ifndef DOXYGEN_PROCESSOR

#include "Experiments/HCUBE_Experiment.h"

namespace HCUBE
{
    /*class Node
    {
    public:
        int x,y,z;

        Node(int _x,int _y,int _z)
        :
        x(_x),
        y(_y),
        z(_z)
        {
        }

        Node()
        :
        x(0),
        y(0),
        z(0)
        {
        }

        bool operator<(const Node &other) const
        {
            return (
            (x<other.x) ||
            (x==other.x && y<other.y) ||
            (x==other.x && y==other.y && z<other.z)
            );
        }
    };*/

    class TicTacToeExperiment : public Experiment
    {
    public:
    protected:
        NEAT::FastBiasNetwork<double> substrate;
        int numNodesX,numNodesY;
        int userBoardState;

        map<Node,string> nameLookup;

    public:
        TicTacToeExperiment(string _experimentName,int _threadID);

        virtual ~TicTacToeExperiment()
        {}

        virtual NEAT::GeneticPopulation* createInitialPopulation(int populationSize);

        void generateSubstrate();

        void populateSubstrate(shared_ptr<const NEAT::GeneticIndividual> individual);

        double processEvaluation(
            shared_ptr<NEAT::GeneticIndividual> individual,
            wxDC *drawContext,
            int boardState
        );

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

#endif // HCUBE_TICTACTOEEXPERIMENT_H_INCLUDED
