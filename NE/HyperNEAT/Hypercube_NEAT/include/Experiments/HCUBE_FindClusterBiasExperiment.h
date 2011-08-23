#ifndef HCUBE_FINDCLUSTERBIASEXPERIMENT_H_INCLUDED
#define HCUBE_FINDCLUSTERBIASEXPERIMENT_H_INCLUDED

#include "Experiments/HCUBE_Experiment.h"

#include "HCUBE_Vector2.h"

namespace HCUBE
{
    class FindClusterBiasExperiment : public Experiment
    {
    public:
    protected:
        NEAT::FastBiasNetwork substrate;
        int numNodesX,numNodesY;

        Vector2<int> user1,user2;

        Vector2<int> userDisplayOutputLinkWeights;

        bool userDisplayOutputs;

        bool clicked1Last;

        int CELL_SIZE;

        map<pair<int,int>,string> nameLookup;

        int sizeMultiplier;

    public:
        FindClusterBiasExperiment(string _experimentName);

        virtual ~FindClusterBiasExperiment()
        {}

        virtual NEAT::GeneticPopulation* createInitialPopulation(int populationSize);

        virtual void generateSubstrate();

        virtual void populateSubstrate(shared_ptr<const NEAT::GeneticIndividual> individual);

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

        void increaseResolution();

        void decreaseResolution();

        virtual Experiment* clone();
    };

    class FindClusterBiasStats : public NEAT::Stringable
    {
    public:
        vector<Vector2<double> > baitLocation,targetLocation,actualLocation;


        FindClusterBiasStats()
        {
            baitLocation.reserve(1000);
            targetLocation.reserve(1000);
            actualLocation.reserve(1000);
        }

        virtual ~FindClusterBiasStats()
        {}

        void addStat(Vector2<double> &_bait,Vector2<double> &_target,Vector2<double> &_actual)
        {
            baitLocation.push_back(_bait);
            targetLocation.push_back(_target);
            actualLocation.push_back(_actual);
        }

        virtual string toString() const
        {
            std::ostringstream oss;

            for (int a=0;a<targetLocation.size();a++)
            {
                oss << "Bait: " << baitLocation[a].x << ' ' << baitLocation[a].y << ' ';
                oss << "Target: " << targetLocation[a].x << ' ' << targetLocation[a].y << ' ';
                oss << "Actual: "  << actualLocation[a].x << ' ' << actualLocation[a].y;
                oss << " / ";
            }
            return oss.str();
        }

        virtual string toMultiLineString() const
        {
            std::ostringstream oss;

            int numWrong=0;
            double totalDistanceOff=0,distanceOff=0;

            for (int a=0;a<targetLocation.size();a++)
            {
                oss << "Bait: " << baitLocation[a].x << ' ' << baitLocation[a].y << ' ';
                oss << "Target: " << targetLocation[a].x << ' ' << targetLocation[a].y << ' ';
                oss << "Actual: "  << actualLocation[a].x << ' ' << actualLocation[a].y;
                oss << endl;

                distanceOff =
                    sqrt(
                        (actualLocation[a].x-targetLocation[a].x)*(actualLocation[a].x-targetLocation[a].x) +
                        (actualLocation[a].y-targetLocation[a].y)*(actualLocation[a].y-targetLocation[a].y)
                    );

                totalDistanceOff += distanceOff;

                if (distanceOff>.6)
                    numWrong++;
            }

            oss << "Total_distance_off: " << totalDistanceOff << endl;
            oss << "Average_distance_off: " << totalDistanceOff/actualLocation.size() << endl;
            oss << "Number_wrong: " << numWrong << endl;
            oss << "Percent_wrong: " << (numWrong*10000)/actualLocation.size()/100.0 << endl;

            return oss.str();
        }

        virtual string summaryHeaderToString() const
        {
            std::ostringstream oss;

            oss << "Total_distance_off: ";
            oss << "Average_distance_off: ";
            oss << "Number_wrong: ";
            oss << "Percent_wrong: ";

            return oss.str();
        }

        virtual string summaryToString() const
        {
            std::ostringstream oss;

            int numWrong=0;
            double totalDistanceOff=0,distanceOff=0;

            for (int a=0;a<targetLocation.size();a++)
            {
                distanceOff =
                    sqrt(
                        (actualLocation[a].x-targetLocation[a].x)*(actualLocation[a].x-targetLocation[a].x) +
                        (actualLocation[a].y-targetLocation[a].y)*(actualLocation[a].y-targetLocation[a].y)
                    );

                totalDistanceOff += distanceOff;

                if (distanceOff>.6)
                    numWrong++;
            }

            oss << totalDistanceOff << " ";
            oss << totalDistanceOff/actualLocation.size() << " ";
            oss << numWrong << " ";
            oss << (numWrong*10000)/actualLocation.size()/100.0;

            return oss.str();
        }

        virtual Stringable *clone() const
        {
            return new FindClusterBiasStats(*this);
        }
    };
}

#endif // HCUBE_FINDCLUSTERBIASEXPERIMENT_H_INCLUDED
