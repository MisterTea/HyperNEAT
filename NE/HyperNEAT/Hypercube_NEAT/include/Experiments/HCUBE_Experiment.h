#ifndef HCUBE_EXPERIMENT_H_INCLUDED
#define HCUBE_EXPERIMENT_H_INCLUDED

#include "HCUBE_Defines.h"

namespace HCUBE
{
    class Experiment
    {
    protected:
        string experimentName;

        bool displayGenerationResult;

        shared_ptr<NEAT::GeneticIndividual> lastIndividualSeen;

        vector<shared_ptr<NEAT::GeneticIndividual> > group;

		int threadID;

        NEAT::LayeredSubstrateInfo layerInfo;

    public:
        Experiment(string _experimentName,int _threadID)
                :
                experimentName(_experimentName),
                displayGenerationResult(true),
				threadID(_threadID)
        {}

        virtual ~Experiment()
        {}

        string getExperimentName() const
        {
            return experimentName;
        }

        void setExperimentName(string _experimentName)
        {
            experimentName = _experimentName;
        }

        inline void setLastIndividualSeen(shared_ptr<NEAT::GeneticIndividual> _lastIndividualSeen)
        {
            lastIndividualSeen = _lastIndividualSeen;
        }

        virtual NEAT::GeneticPopulation* createInitialPopulation(int populationSize) = 0;

        virtual int getGroupCapacity()
        {
            return 1;
        }

        inline int getGroupSize()
        {
            return int(group.size());
        }

        inline shared_ptr<NEAT::GeneticIndividual> getGroupMember(int index)
        {
            return group[index];
        }

        inline void clearGroup()
        {
            group.clear();
        }

        inline void addIndividualToGroup(shared_ptr<NEAT::GeneticIndividual> individual)
        {
            group.push_back(individual);
        }

		virtual void preprocessIndividual(
			shared_ptr<NEAT::GeneticGeneration> generation,
			shared_ptr<NEAT::GeneticIndividual> individual
			)
		{}

        virtual void processGroup(shared_ptr<NEAT::GeneticGeneration> generation) = 0;

        virtual void processIndividualPostHoc(shared_ptr<NEAT::GeneticIndividual> individual)
        {}

#ifndef HCUBE_NOGUI
        virtual void createIndividualImage(wxDC &drawContext,shared_ptr<NEAT::GeneticIndividual> individual)
        {
            return;
        }
#endif

        virtual bool performUserEvaluations()
        {
            return false;
        }

#ifndef HCUBE_NOGUI
        /**
         * handleMousePress: returns true if the window needs to be refreshed
         */
        virtual bool handleMousePress(wxMouseEvent& event,wxSize &bitmapSize)
        {
            return false;
        }

        /**
         * handleMouseMotion: returns true if the window needs to be refreshed
         */
        virtual bool handleMouseMotion(wxMouseEvent& event,wxDC &temp_dc,shared_ptr<NEAT::GeneticIndividual> individual)
        {
            return false;
        }
#endif

        virtual inline bool isDisplayGenerationResult()
        {
            return displayGenerationResult;
        }

        virtual inline void setDisplayGenerationResult(bool _displayGenerationResult)
        {
            displayGenerationResult=_displayGenerationResult;
        }

        virtual inline void toggleDisplayGenerationResult()
        {
            displayGenerationResult=!displayGenerationResult;
        }

#ifndef HCUBE_NOGUI
        inline void drawPixel(int x,int y,int relativeResolution,wxColour** localBuffer,wxColour value)
        {
            //you want to draw a pixel at x,y if x,y was over 32, so multiply if it isn't

            int mody,modx;
            for (mody=0;mody<relativeResolution;mody++)
            {
                for (modx=0;modx<relativeResolution;modx++)
                {
                    localBuffer[(y*relativeResolution)+mody][(x*relativeResolution)+modx] = value;
                }
            }
        }
#endif

        virtual Experiment* clone() = 0;

        virtual void resetGenerationData(shared_ptr<NEAT::GeneticGeneration> generation)
        {}

        virtual void addGenerationData(shared_ptr<NEAT::GeneticGeneration> generation,shared_ptr<NEAT::GeneticIndividual> individual)
        {}

        const NEAT::LayeredSubstrateInfo &getLayerInfo()
        {
            return layerInfo;
        }
    };
}

#endif // HCUBE_EXPERIMENT_H_INCLUDED
