#ifdef WIN32
#include "windows.h"
#endif
#include "mpi.h"

#include "JGTL_CommandLineParser.h"

#include "HCUBE_MPIExperimentRun.h"
#include "HCUBE_MPIEvaluationSet.h"

#include "Experiments/HCUBE_Experiment.h"
#include "Experiments/HCUBE_FindClusterExperiment.h"
#include "Experiments/HCUBE_CheckersExperiment.h"

#include "cakepp.h"

#include "SgInit.h"
#include "GoInit.h"

#define DEBUG_MPI_MAIN (0)

void runOrContinueExperiment(HCUBE::MPIExperimentRun &experimentRun)
{
  int  numtasks, rank;

  MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);

  if (rank==0)
  {
    experimentRun.setCleanup(true);

    cout << rank << ") Population Created\n";

    experimentRun.start();

    int numThreads;

    MPI_Comm_size(MPI_COMM_WORLD,&numThreads);

    cout << "Number of threads to kill: " << numThreads << endl;

    for (rank = 1; rank < numThreads; ++rank) 
    {
      MPI_Send(0, 0, MPI_INT, rank, DIE_TAG, MPI_COMM_WORLD);
    }

  }
  else
  {
    shared_ptr<HCUBE::Experiment> experiment(
      experimentRun.getExperiment()->clone()
      );

    //cout << "Experiment pointer: " << experiment << endl;

    char *buffer=NULL;
    int bufferSize=0;

    int curGenNumber=0;
    if(experimentRun.getPopulation())
    {
        cout << "Resuming at generation: " << experimentRun.getPopulation()->getGenerationCount()-1 << endl;
        curGenNumber=experimentRun.getPopulation()->getGenerationCount()-1;
    }
    for(;;curGenNumber++)
    {
#if DEBUG_MPI_MAIN
      cout << rank << ") Listening for individual chunk size...\n";
#endif

      int msgSize;

      {
        MPI_Status Stat;
        MPI_Recv (&msgSize,1,MPI_INT,0,MPI_ANY_TAG,MPI_COMM_WORLD,&Stat);

        if(Stat.MPI_TAG==DIE_TAG)
        {
          break;
        }
      }

#if DEBUG_MPI_MAIN
      cout << rank << ") Got chunk size: " << msgSize << endl;
#endif

      if (msgSize==-1)
      {
        break;
      }

      if (msgSize==0)
      {
        continue; // ????  This shouldn't happen, but handle anyways
      }

      if (bufferSize<msgSize)
      {
        bufferSize = msgSize;
        buffer = (char*)realloc(buffer,bufferSize);
      }

      memset(buffer,0,bufferSize);

#if DEBUG_MPI_MAIN
      cout << rank << ") Getting buffer...\n";
#endif

      {
        MPI_Status Stat;
        MPI_Recv (buffer,msgSize,MPI_CHAR,0,INDIVIDUAL_GENOMES_TAG,MPI_COMM_WORLD,&Stat);
      }

#if DEBUG_MPI_MAIN
      cout << rank << ") Got Buffer\n";

      //cout << buffer << endl;
#endif

      istringstream istr(buffer);

#if DEBUG_MPI_MAIN
      cout << rank << ") Loaded stringstream\n";
#endif


      int testCount;
      istr >> testCount;


      shared_ptr<NEAT::GeneticGeneration> generation;

#ifdef EPLEX_INTERNAL
      if(testCount)
      {
        generation = shared_ptr<NEAT::GeneticGeneration>(
          new NEAT::CoEvoGeneticGeneration(
            0,
            dynamic_pointer_cast<NEAT::CoEvoExperiment>(experiment)
            )
          );
      }
      else
#endif
      {
        generation = shared_ptr<NEAT::GeneticGeneration>(
          new NEAT::GeneticGeneration(curGenNumber)
          );
      }

#if DEBUG_MPI_MAIN
      cout << rank << ") Test count: " << testCount << endl;
#endif

#ifdef EPLEX_INTERNAL
      for(int a=0;a<testCount;a++)
      {
        shared_ptr<NEAT::GeneticIndividual> testInd(new NEAT::GeneticIndividual(istr));

        shared_ptr<NEAT::CoEvoGeneticGeneration> coEvoGen =
          static_pointer_cast<NEAT::CoEvoGeneticGeneration>(generation);

        coEvoGen->addTestHack(testInd);
      }
#endif

      int individualCount;

      istr >> individualCount;

#if DEBUG_MPI_MAIN
      cout << rank << ") Individualcount: " << individualCount << endl;
#endif

      //double *newFitness = (double*)malloc(sizeof(double)*individualCount);
      ostringstream ostr;

#if DEBUG_MPI_MAIN
      cout << rank << ") Fitness buffer created\n";
#endif

      for (int a=0;a<individualCount;a+=experiment->getGroupCapacity())
      {
#if DEBUG_MPI_MAIN
		cout << rank << ") Adding to group...\n";
#endif

        for (int b=0;b<experiment->getGroupCapacity();b++)
        {
          shared_ptr<NEAT::GeneticIndividual> ind(
            new NEAT::GeneticIndividual(istr)
            );

          experiment->addIndividualToGroup(ind);
        }

#if DEBUG_MPI_MAIN
		cout << rank << ") Processing...\n";
#endif

        experiment->processGroup(generation);

#if DEBUG_MPI_MAIN
		cout << rank << ") Dumping...\n";
#endif

        for (int b=0;b<experiment->getGroupCapacity();b++)
        {
			experiment->getGroupMember(b)->dump(ostr);
          //newFitness[a+b] = experiment->getGroupMember(b)->getFitness();
        }

#if DEBUG_MPI_MAIN
		cout << rank << ") Clearing...\n";
#endif

		//Clear the experiment for the next individuals
        experiment->clearGroup();

#if DEBUG_MPI_MAIN
		cout << rank << ") End of process...\n";
#endif
	  }

#if DEBUG_MPI_MAIN
      cout << rank << ") Sending new fitness values\n";
#endif

        string str = ostr.str();

        char *buffer = new char[str.length()+1];

        memcpy(buffer,str.c_str(),str.length());

        buffer[str.length()] = '\0'; //null terminate

        int lengthInt = (int)str.length() + 1;

#if DEBUG_MPI_MAIN
      cout << rank << ") Sending message of size " << lengthInt << "\n";
#endif

      //MPI_Send (newFitness,individualCount,MPI_DOUBLE,0,NEW_FITNESSES_TAG,MPI_COMM_WORLD);
		MPI_Send (&lengthInt,1,MPI_INT,0,NEW_INDIVIDUALS_TAG,MPI_COMM_WORLD);
		MPI_Send (buffer,lengthInt,MPI_CHAR,0,NEW_INDIVIDUALS_TAG,MPI_COMM_WORLD);

#if DEBUG_MPI_MAIN
      cout << rank << ") Cleaning up new fitness values\n";
#endif

      //free(newFitness);
    }
  }
}

int main(int argc,char **argv)
{
  //Override the number of threads if using MPI
  HCUBE::NUM_THREADS=1;

  char str[1024];
  initcake(str);
  SgInit();
  GoInit();

  int retval=0;

  CommandLineParser commandLineParser(argc,argv);

  try
  {
    /*
      for(int a=0;a<argc;a++)
      {
      cout << "ARGUMENT: " << argv[a] << endl;
      }
    */

    int  numtasks, rank, rc;

    rc = MPI_Init(&argc,&argv);

    if (rc != MPI_SUCCESS)
    {
      printf ("Error starting MPI program. Terminating.\n");
      MPI_Abort(MPI_COMM_WORLD, rc);
      return 1;
    }

    MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    cout << "NUM TASKS: " << numtasks << " RANK: " << rank << endl;

    for (int a=0;a<argc;a++)
    {
      cout << "ARGUMENT: " << argv[a] << endl;
    }

    if(
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

      HCUBE::MPIExperimentRun experimentRun;

      experimentRun.setupExperiment(experimentType,commandLineParser.GetSafeArgument("-O",0,"output.xml"));
      if(rank==0)
      {
        experimentRun.createPopulation();
      }

      cout << "Experiment set up\n";

      runOrContinueExperiment(experimentRun);
    }
    else if(
      commandLineParser.HasSwitch("-C") && 
      commandLineParser.HasSwitch("-O")
      )
    {
      HCUBE::MPIExperimentRun experimentRun;

      string backupFile = commandLineParser.GetArgument("-C",0);
      string outputFile = commandLineParser.GetArgument("-O",0);
			
      //This also calls NEAT::Globals::init(...)
      experimentRun.setupExperimentInProgress(backupFile,outputFile);

      cout << "Experiment set up\n";

      runOrContinueExperiment(experimentRun);
    }
    else
    {
      cout << rank << ") Invalid parameters (count: " << argc << ")!\n";
    }


    MPI_Finalize();

    exitcake();

    GoFini();
    SgFini();
  }
  catch (const std::exception &ex)
  {
    cout << "An exception has occured: " << ex.what() << endl;
  }
  catch (string s)
  {
    cout << "An exception has occured: " << s << endl;
  }
  catch (...)
  {
    cout << "An error has occured!\n";
  }

  return 0;
}

