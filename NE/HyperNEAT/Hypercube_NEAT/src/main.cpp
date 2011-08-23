#include "HCUBE_Defines.h"

#include "JGTL_CommandLineParser.h"
  
#ifndef HCUBE_NOGUI
# include "HCUBE_MainApp.h"
#endif

#include "HCUBE_ExperimentRun.h"

#include "Experiments/HCUBE_Experiment.h"
#include "Experiments/HCUBE_FindClusterExperiment.h"
#include "Experiments/HCUBE_CheckersExperiment.h"

#ifdef EPLEX_INTERNAL
#include "Experiments/HCUBE_OthelloExperiment.h"
#endif

#include "cakepp.h"

#include "SgInit.h"
#include "GoInit.h"

#ifndef HCUBE_NOGUI
namespace HCUBE
{
    IMPLEMENT_APP_NO_MAIN(MainApp)
        }
#endif

#define EPOCHS_PER_PRINT (100000)

int HyperNEAT_main(int argc,char **argv)
{
    //This is for memory debugging
    //_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); 
    //_CrtSetBreakAlloc(1378);

    char str[1024];
    initcake(str);
    SgInit();
    GoInit();

    int retval=0;

    CommandLineParser commandLineParser(argc,argv);

#if 1

#if 0
    try
#endif
    {
        if (argc<=1)
        {
#ifndef HCUBE_NOGUI
            cout << "Starting wxEntry...\n";
            retval = wxEntry(argc, argv);
#else
            cout << "You must pass the parameters and the output file as command "
                 << "parameters!\n";
            cout << "(If you intended to run in GUI mode, please run cmake and set the USE_GUI flag to true)\n";
#endif
        }
        else if (argc==2)
        {
            //Run the post-hoc analysis on last generation
            cout << "Running post-hoc analysis on: " << argv[1] << endl;
            HCUBE::ExperimentRun experimentRun;
			
            experimentRun.setupExperimentInProgress(
													string(argv[1]),
													""
													);
			
            int numGenerations = experimentRun.getPopulation()->getGenerationCount();
			
            HCUBE::Experiment *experiment = experimentRun.getExperiment()->clone();
			
            /*
			 {
			 string outString = (erase_tail_copy(string(argv[1]),4)+string("_fitness.out"));
			 cout << "Creating file " << outString << endl;
			 ofstream outfile( outString.c_str() );
			 string previousSummary;
			 
			 bool doOnce=false;
			 
			 for (int generation=0;generation<numGenerations;generation++)
			 {
			 //CREATE_PAUSE(string("Error!: ")+toString(__LINE__));
			 
			 //CREATE_PAUSE(string("Error!: ")+toString(__LINE__));
			 
			 if ( generation &&
			 (*(experimentRun.getIndividual(generation,0).get())) == (*(experimentRun.getIndividual(generation-1,0).get())) )
			 {
			 outfile << (generation+1) << ' ' << previousSummary << endl;
			 continue;
			 }
			 
			 shared_ptr<NEAT::GeneticIndividual> indiv =
			 shared_ptr<NEAT::GeneticIndividual>(
			 new NEAT::GeneticIndividual(
			 *(experimentRun.getIndividual(generation,0).get())
			 )
			 );
			 
			 //CREATE_PAUSE(string("Error!: ")+toString(__LINE__));
			 
			 cout << "Beginning fitness evaluation " << (generation+1) << "/" << numGenerations << "...";
			 cout.flush();
			 experiment->addIndividualToGroup(indiv);
			 experiment->processGroup(experimentRun.getGeneration(generation));
			 experiment->clearGroup();
			 cout << "done!\n";
			 
			 //CREATE_PAUSE(string("Error!: ")+toString(__LINE__));
			 
			 if (indiv->getUserData())
			 {
			 if (!doOnce)
			 {
			 doOnce=true;
			 outfile
			 << "#Generation: "
			 << indiv->getUserData()->summaryHeaderToString()
			 << endl;
			 }
			 
			 previousSummary = indiv->getUserData()->summaryToString();
			 outfile << (generation+1) << ' ' << previousSummary << endl;
			 }
			 else
			 {
			 throw CREATE_LOCATEDEXCEPTION_INFO("No user data!\n");
			 }
			 
			 }
			 }
			 */
			
            {
                string outString = (erase_tail_copy(string(argv[1]),3)+string("_fitness.out"));
                cout << "Creating file " << outString << endl;
                ofstream outfile( outString.c_str() );
                string previousSummary;
				
                bool doOnce=false;
				
				int firstGen = 0;
				//int firstGen = numGenerations-1;
                for (int generation=firstGen;generation<numGenerations;generation++)
                {
                    //CREATE_PAUSE(string("Error!: ")+toString(__LINE__));
					
                    //CREATE_PAUSE(string("Error!: ")+toString(__LINE__));
					
#if 0
                    if ( generation &&
						(*(experimentRun.getIndividual(generation,0).get())) == (*(experimentRun.getIndividual(generation-1,0).get())) )
                    {
                        outfile << (generation+1) << ' ' << previousSummary << endl;
                        continue;
                    }
#endif
					
                    shared_ptr<NEAT::GeneticIndividual> indiv =
					shared_ptr<NEAT::GeneticIndividual>(
														new NEAT::GeneticIndividual(
																					*(experimentRun.getIndividual(generation,0).get())
																					)
														);
					
					if(generation!=firstGen)
					{
						shared_ptr<NEAT::GeneticIndividual> lastindiv =
                        shared_ptr<NEAT::GeneticIndividual>(
															new NEAT::GeneticIndividual(
																						*(experimentRun.getIndividual(generation-1,0).get())
																						)
															);
						
						if(fabsf(indiv->getFitness()-lastindiv->getFitness())<1e-3)
						{
							//cout << "Skipping redundant individual\n";
							continue;
						}
					}
					
                    //CREATE_PAUSE(string("Error!: ")+toString(__LINE__));
					
                    //cout << "Beginning post-hoc evaluation " << (generation+1) << "/" << numGenerations << "...";
                    //experiment->processIndividualPostHoc(indiv);
                    //cout << "done!\n";
					
                    //CREATE_PAUSE(string("Error!: ")+toString(__LINE__));
					
#if 0
                    if (indiv->getUserData().length())
                    {
                        if (!doOnce)
                        {
                            doOnce=true;
                            outfile
							<< "#Generation: "
							<< indiv->getUserData()
							<< endl;
                        }
						
                        previousSummary = indiv->getUserData();
                        outfile << (generation+1) << ' ' << previousSummary << endl;
                    }
                    else
                    {
                        if (!doOnce)
                        {
                            doOnce=true;
                            outfile
							<< "#Generation: "
							<< "Fitness:"
							<< endl;
                        }
						
                        previousSummary = toString(indiv->getFitness());
                        outfile << (generation+1) << ' ' << previousSummary << endl;
                    }
#endif
					
					cout << "Beginning post-hoc evaluation " << (generation+1) << "/" << numGenerations << "...";
					experiment->processIndividualPostHoc(indiv);
					cout << indiv->getUserData() << endl;
					cout << "done!\n";
					
					//CREATE_PAUSE(string("Error!: ")+toString(__LINE__));
					
#if 0
					if (indiv->getUserData())
					{
						if (!doOnce)
						{
							doOnce=true;
							outfile
							<< "#Generation: "
							<< indiv->getUserData()->summaryHeaderToString()
							<< endl;
						}
						
						previousSummary = indiv->getUserData()->summaryToString();
						outfile << (generation+1) << ' ' << previousSummary << endl;
					}
					else
					{
						if (!doOnce)
						{
							doOnce=true;
							outfile
							<< "#Generation: "
							<< "Fitness:"
							<< endl;
						}
						
						previousSummary = toString(indiv->getFitness());
						outfile << (generation+1) << ' ' << previousSummary << endl;
					}
#endif
				}
			}
		}
        else if (
				 commandLineParser.HasSwitch("-P")
				 )
        {
			string inputFile = commandLineParser.GetSafeArgument("-P",0,"input.dat");
            //Run the post-hoc analysis
            cout << "Running post-hoc analysis on: " << inputFile << endl;
            HCUBE::ExperimentRun experimentRun;
			
            experimentRun.setupExperimentInProgress(
													inputFile,
													""
													);
			
            int numGenerations = experimentRun.getPopulation()->getGenerationCount();
			
            HCUBE::Experiment *experiment = experimentRun.getExperiment()->clone();
			
			{
				int firstGen = 0;
				//int firstGen = numGenerations-1;
				bool ignoreFirstIfElitism=false;
				
				if(commandLineParser.HasSwitch("-G"))
				{
					firstGen = atoi(commandLineParser.GetSafeArgument("-G",0,"0").c_str())-1;
					if(firstGen<numGenerations)
						numGenerations = firstGen+1;
				}
		
				if(firstGen && commandLineParser.HasSwitch("-S"))
					ignoreFirstIfElitism=true;
				
                for (int generation=firstGen;generation<numGenerations;generation++)
                {
                    shared_ptr<NEAT::GeneticIndividual> indiv =
					shared_ptr<NEAT::GeneticIndividual>(
														new NEAT::GeneticIndividual(
																					*(experimentRun.getIndividual(generation,0).get())
																					)
														);
					
					if(generation!=firstGen || ignoreFirstIfElitism)
					{
						shared_ptr<NEAT::GeneticIndividual> lastindiv =
                        shared_ptr<NEAT::GeneticIndividual>(
															new NEAT::GeneticIndividual(
																						*(experimentRun.getIndividual(generation-1,0).get())
																						)
															);
						
						if(fabsf(indiv->getFitness()-lastindiv->getFitness())<1e-3)
						{
							//cout << "Skipping redundant individual\n";
							continue;
						}
					}
					
					cout << "Beginning post-hoc evaluation " << (generation+1) << "/" << numGenerations << "..." << endl;
					experiment->processIndividualPostHoc(indiv);
					cout << indiv->getUserData() << endl;
					cout << "done!\n";
				}
			}
		}
		else if(
    commandLineParser.HasSwitch("-I") &&
    commandLineParser.HasSwitch("-O")
    )
{
    NEAT::Globals::init(commandLineParser.GetSafeArgument("-I",0,"input.dat"));

    if(commandLineParser.HasSwitch("-R"))
    {
        NEAT::Globals::getSingleton()->seedRandom(stringTo<unsigned int>(commandLineParser.GetSafeArgument("-R",0,"0")));
    }

    int experimentType = int(NEAT::Globals::getSingleton()->getParameterValue("ExperimentType")+0.001);

    cout << "Loading Experiment: " << experimentType << endl;

    HCUBE::ExperimentRun experimentRun;

    experimentRun.setupExperiment(experimentType,commandLineParser.GetSafeArgument("-O",0,"output.xml"));

    cout << "Experiment set up\n";

    experimentRun.createPopulation();

    experimentRun.setCleanup(true);

    cout << "Population Created\n";

    experimentRun.start();
}
else if(
    commandLineParser.HasSwitch("-C")
    )
{
    HCUBE::ExperimentRun experimentRun;

    string backupFile = commandLineParser.GetArgument("-C",0);
    string outputFile = backupFile.substr(0,backupFile.length()-14);
			
    //This also calls NEAT::Globals::init(...)
    experimentRun.setupExperimentInProgress(backupFile,outputFile);

    cout << "Experiment set up\n";

    experimentRun.setCleanup(true);
    experimentRun.start();
}
else
{
    cout << "Syntax for passing command-line options to HyperNEAT (do not actually type '(' or ')' ):\n";
    cout << "./HyperNEAT [-R (seed)] -I (datafile) -O (outputfile)\n";
}
}
#if 0
catch (const std::exception &ex)
{
    cout << "CAUGHT ERROR AT " << __FILE__ << " : " << __LINE__ << endl;
    cout << "An exception has occured: " << ex.what() << endl;
}
catch (string s)
{
    cout << "CAUGHT ERROR AT " << __FILE__ << " : " << __LINE__ << endl;
    cout << "An exception has occured: " << s << endl;
}
catch (...)
{
    cout << "CAUGHT ERROR AT " << __FILE__ << " : " << __LINE__ << endl;
    cout << "An error has occured!\n";
}
#endif

NEAT::Globals::deinit();

#endif

exitcake();

GoFini();
SgFini();

return retval;
}

int main(int argc,char **argv)
{
    HyperNEAT_main(argc,argv);
}
