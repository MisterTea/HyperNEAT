#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include "NEAT_Defines.h"
#include "NEAT_STL.h"
#include "NEAT_Random.h"
#include "tinyxmlplus.h"

/* #defines */
#define LAST_GENERATION  (-1)

enum ActivationFunction
{
    ACTIVATION_FUNCTION_SIGMOID = 0,
    ACTIVATION_FUNCTION_SIN,
    ACTIVATION_FUNCTION_COS,
    ACTIVATION_FUNCTION_GAUSSIAN,
    ACTIVATION_FUNCTION_SQUARE,
    ACTIVATION_FUNCTION_ABS_ROOT,
    ACTIVATION_FUNCTION_LINEAR,
    ACTIVATION_FUNCTION_ONES_COMPLIMENT,
    ACTIVATION_FUNCTION_END
};

extern const char *activationFunctionNames[ACTIVATION_FUNCTION_END];

namespace NEAT
{
    class Globals
    {
    protected:
        NEAT_DLL_EXPORT static Globals *singleton;

        int nodeCounter,linkCounter,speciesCounter;

        vector<shared_ptr<GeneticLinkGene> > linkGenesThisGeneration;

		StackMap<string,double,4096> parameters;

        Random random;

		int extraActivationUpdates;

		bool signedActivation;

		bool useTanhSigmoid;
    public:
        static inline Globals *getSingleton()
        {
            if (!singleton)
                throw CREATE_LOCATEDEXCEPTION_INFO("You didn't initialize Globals before using it!");

            return singleton;
        }

        static inline Globals *init()
        {
            if (singleton)
                delete singleton;

            singleton = new Globals();

            return singleton;
        }

        static inline Globals *init(string filename)
        {
            if (singleton)
                delete singleton;

            singleton = new Globals(filename);

            return singleton;
        }

        static inline Globals *init(TiXmlElement *root)
        {
            if (singleton)
                delete singleton;

            singleton = new Globals(root);

            return singleton;
        }

        static inline void deinit()
        {
            if (singleton)
                delete singleton;
        }

        NEAT_DLL_EXPORT void assignNodeID(GeneticNodeGene *testNode);

        NEAT_DLL_EXPORT void assignLinkID(GeneticLinkGene *testLink,bool ignoreHistory=false);

        NEAT_DLL_EXPORT void clearLinkHistory();

        NEAT_DLL_EXPORT int generateSpeciesID();

        NEAT_DLL_EXPORT void addParameter(string name,double value);

        NEAT_DLL_EXPORT bool hasParameterValue(const string &name);

		NEAT_DLL_EXPORT double getParameterValue(const char *cname);

        NEAT_DLL_EXPORT double getParameterValue(string name);

        NEAT_DLL_EXPORT void setParameterValue(string name,double value);

        inline StackMap<string,double,4096>::iterator getMapBegin()
        {
            return parameters.begin();
        }

        inline StackMap<string,double,4096>::iterator getMapEnd()
        {
            return parameters.end();
        }

        NEAT_DLL_EXPORT void initRandom();

        inline Random &getRandom()
        {
            return random;
        }

        NEAT_DLL_EXPORT void seedRandom(unsigned int newSeed);

        NEAT_DLL_EXPORT void dump(TiXmlElement *root);

		inline int getExtraActivationUpdates()
		{
			return extraActivationUpdates;
		}

		inline bool hasSignedActivation()
		{
			return signedActivation;
		}

		inline bool isUsingTanhSigmoid()
		{
			return useTanhSigmoid;
		}

    protected:
        NEAT_DLL_EXPORT Globals();

        NEAT_DLL_EXPORT Globals(TiXmlElement *root);

        NEAT_DLL_EXPORT Globals(string fileName);

        NEAT_DLL_EXPORT virtual ~Globals();

        int generateNodeID();

        int generateLinkID();

		void cacheParameters();
    };

}

#endif
