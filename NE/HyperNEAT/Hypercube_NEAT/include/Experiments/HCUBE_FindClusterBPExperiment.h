#ifndef HCUBE_FINDCLUSTERBP_H_INCLUDED
#define HCUBE_FINDCLUSTERBP_H_INCLUDED

#include "Experiments/HCUBE_Experiment.h"

//#include "HCUBE_Vector2.h"

namespace HCUBE
{
    class FindClusterBPExperiment : public Experiment
    {
    public:
    protected:
        NEAT::FastNetwork<double> CPPNNetwork;
        NEAT::FastNetwork<double> substrate;
        int numNodesX,numNodesY;

        Vector2<int> user1,user2;

        Vector2<int> userDisplayOutputLinkWeights;

        bool userDisplayOutputs;

        bool clicked1Last;

        int CELL_SIZE;

        map<Node,string> nameLookup;

        int sizeMultiplier;

    public:
        FindClusterBPExperiment(string _experimentName,int _threadID);

        virtual ~FindClusterBPExperiment()
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

    class FindClusterBPStats
    {
    public:
        vector<Vector2<double> > baitLocation,targetLocation,actualLocation;


        FindClusterBPStats()
        {
            baitLocation.reserve(1000);
            targetLocation.reserve(1000);
            actualLocation.reserve(1000);
        }

		FindClusterBPStats(const string &tmpstring)
		{
            baitLocation.reserve(1000);
            targetLocation.reserve(1000);
            actualLocation.reserve(1000);

			istringstream istr(tmpstring);
			int size;

			istr >> size;

			for(int a=0;a<size;a++)
			{
				string dummy;
				Vector2<double> bait,target,actual;

				istr >> dummy >> bait >> dummy >> target >> dummy >> actual >> dummy;

				baitLocation.push_back(bait);
				targetLocation.push_back(target);
				actualLocation.push_back(actual);
			}
		}

        virtual ~FindClusterBPStats()
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

			oss << targetLocation.size() << " ";
            for (int a=0;a<(int)targetLocation.size();a++)
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

			oss << targetLocation.size() << " ";
            for (int a=0;a<(int)targetLocation.size();a++)
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

            for (int a=0;a<(int)targetLocation.size();a++)
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
    };
}

#endif // HCUBE_FINDCLUSTERBP_H_INCLUDED
