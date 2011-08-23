#include "mpi.h"

#include "HCUBE_MPIEvaluationSet.h"

#define MPI_EVALUATION_SET_DEBUG (0)

namespace HCUBE
{
    boost::mutex mpiMutex;

    void MPIEvaluationSet::run()
    {
#if MPI_EVALUATION_SET_DEBUG
        cout << targetProcessor << " MAIN) Starting EvaluationSet\n";
#endif

        try
        {
            //Process individuals sequentially
            running=true;

            vector<shared_ptr<NEAT::GeneticIndividual> >::iterator tmpIterator;

            ostringstream ostr;

            if (targetProcessor!=0)
            {
                int testCount = 0;

#ifdef EPLEX_INTERNAL
                if (dynamic_cast<NEAT::CoEvoExperiment*>(experiment.get()))
                {
                    shared_ptr<NEAT::CoEvoGeneticGeneration> gen =
                        static_pointer_cast<NEAT::CoEvoGeneticGeneration>(generation);

                    testCount = gen->getTestCount();
                }
#endif

                ostr << testCount << " ";

#ifdef EPLEX_INTERNAL
                if (dynamic_cast<NEAT::CoEvoExperiment*>(experiment.get()))
                {
#if MPI_EVALUATION_SET_DEBUG
                    cout << targetProcessor << " MAIN) Sending tests...\n";
#endif
                    shared_ptr<NEAT::CoEvoGeneticGeneration> gen =
                        static_pointer_cast<NEAT::CoEvoGeneticGeneration>(generation);

                    int testCount = gen->getTestCount();

                    for (int a=0;a<testCount;a++)
                    {
                        shared_ptr<NEAT::GeneticIndividual> testInd =
                            gen->getTest(a);

                        testInd->dump(ostr);
                    }
                }
#endif
            }

            ostr << individualCount << " ";

#if MPI_EVALUATION_SET_DEBUG
            cout << targetProcessor << " MAIN) Collecting individuals...\n";
#endif

            tmpIterator = individualIterator;
            for (int a=0;a<individualCount;a++,tmpIterator++)
            {
                while (!running)
                {
                    boost::xtime xt;
                    boost::xtime_get(&xt, boost::TIME_UTC);
                    xt.sec += 1;
                    boost::thread::sleep(xt); // Sleep for 1 second
                }

                if (targetProcessor==0)
                {
                    //main processor, just perform the experiment
                }
                else
                {
                    //other processor, package the individuals for transfer
                    (*tmpIterator)->dump(ostr);
                }
            }

            if (targetProcessor==0)
            {
            }
            else
            {

#if MPI_EVALUATION_SET_DEBUG
                cout << targetProcessor << " MAIN) Sending individuals...\n";
#endif

                string str = ostr.str();

                char *buffer = new char[str.length()+1];

                memcpy(buffer,str.c_str(),str.length());

                buffer[str.length()] = '\0'; //null terminate

                int lengthInt = (int)str.length() + 1;

                {
                    boost::mutex::scoped_lock lock(mpiMutex);
                    MPI_Send (&lengthInt,1,MPI_INT,targetProcessor,INDIVIDUAL_GENOMES_TAG,MPI_COMM_WORLD);
                    MPI_Send (buffer,lengthInt,MPI_CHAR,targetProcessor,INDIVIDUAL_GENOMES_TAG,MPI_COMM_WORLD);
                }

                delete[] buffer;
            }
        }
        catch (string s)
        {
            cout << "ERROR: " << s << endl;
            //CREATE_PAUSE(s);
        }
        catch (const char *s)
        {
            cout << "ERROR: " << s << endl;
            //CREATE_PAUSE(s);
        }
        catch (const std::exception &ex)
        {
            cout << "ERROR: " << ex.what() << endl;
            //CREATE_PAUSE(ex.what());
        }
        catch (...)
        {
            cout << "An unknown exception has occured!" << endl;
        }
    }

    void MPIEvaluationSet::collectData()
    {
#if MPI_EVALUATION_SET_DEBUG
        cout << targetProcessor << " MAIN) Starting EvaluationSet\n";
#endif

        try
        {
            //Process individuals sequentially
            running=true;

            vector<shared_ptr<NEAT::GeneticIndividual> >::iterator tmpIterator;

            ostringstream ostr;

            tmpIterator = individualIterator;
            for (int a=0;a<individualCount;a++,tmpIterator++)
            {
                while (!running)
                {
                    boost::xtime xt;
                    boost::xtime_get(&xt, boost::TIME_UTC);
                    xt.sec += 1;
                    boost::thread::sleep(xt); // Sleep for 1 second
                }

                if (targetProcessor==0)
                {
                    //main processor, just perform the experiment
                    experiment->addIndividualToGroup(*tmpIterator);

                    if (experiment->getGroupSize()==experiment->getGroupCapacity())
                    {
                        //cout << "Processing group...\n";
                        experiment->processGroup(generation);
                        //cout << "Done Processing group\n";
                        experiment->clearGroup();
                    }

                    if ((*tmpIterator)->getFitness() <= 1e-6)
                    {
                        throw CREATE_LOCATEDEXCEPTION_INFO("ERROR, 0 Fitness");
                    }
                }
            }

            if (targetProcessor==0)
            {
                if (experiment->getGroupSize()>0)
                {
                    //Oops, maybe you specified a bad # of threads?
                    throw CREATE_LOCATEDEXCEPTION_INFO("Error, individuals were left over after run finished!");
                }
            }
            else
            {

#if MPI_EVALUATION_SET_DEBUG
                cout << targetProcessor << " MAIN) Waiting for results...\n";
#endif

				int msgSize = 0;
				char *buffer = NULL;

                {
                    boost::mutex::scoped_lock lock(mpiMutex);
                    MPI_Status Stat;
                    //MPI_Recv (newFitness,individualCount,MPI_DOUBLE,targetProcessor,NEW_FITNESSES_TAG,MPI_COMM_WORLD,&Stat);
					MPI_Recv (&msgSize,1,MPI_INT,targetProcessor,NEW_INDIVIDUALS_TAG,MPI_COMM_WORLD,&Stat);

#if MPI_EVALUATION_SET_DEBUG
					cout << targetProcessor << " MAIN) Got Message of size " << msgSize << "...\n";
#endif

					buffer = new char[msgSize];
					MPI_Recv (buffer,msgSize,MPI_CHAR,targetProcessor,NEW_INDIVIDUALS_TAG,MPI_COMM_WORLD,&Stat);
                }
				istringstream istr(buffer);

                tmpIterator = individualIterator;
                for (int a=0;a<individualCount;a++,tmpIterator++)
                {
					(*tmpIterator) = shared_ptr<NEAT::GeneticIndividual>(
										new NEAT::GeneticIndividual(istr)
										);
				}

#if MPI_EVALUATION_SET_DEBUG
                cout << targetProcessor << " MAIN) Received new fitness values\n";
#endif

				/*
                tmpIterator = individualIterator;
                for (int a=0;a<individualCount;a++,tmpIterator++)
                {
                    if (fabs(newFitness[a])<1e-6)
                    {
                        throw CREATE_LOCATEDEXCEPTION_INFO("Error, 0 fitness!");
                    }
#if MPI_EVALUATION_SET_DEBUG
                    cout << targetProcessor << " MAIN) fitness: " << newFitness[a] << "\n";
#endif
                    (*tmpIterator)->setFitness(newFitness[a]);
                }
				*/


                delete[] buffer;
                //delete[] newFitness;
            }

#if MPI_EVALUATION_SET_DEBUG
            cout << targetProcessor << " MAIN) Done with EvaluationSet!\n";
#endif

            finished=true;
        }
        catch (string s)
        {
            cout << "ERROR: " << s << endl;
            //CREATE_PAUSE(s);
        }
        catch (const char *s)
        {
            cout << "ERROR: " << s << endl;
            //CREATE_PAUSE(s);
        }
        catch (const std::exception &ex)
        {
            cout << "ERROR: " << ex.what() << endl;
            //CREATE_PAUSE(ex.what());
        }
        catch (...)
        {
            cout << "An unknown exception has occured!" << endl;
        }
    }
}
