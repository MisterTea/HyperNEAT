#include "HCUBE_Defines.h"

namespace HCUBE
{
#if defined(_DEBUG) || defined(USE_GPU)
	int NUM_THREADS = 1;
#else
    //Automatically sets to the # of threads.
	//int NUM_THREADS = 1;
	int NUM_THREADS = boost::thread::hardware_concurrency();
#endif

	const int EXPERIMENT_BLOCKS_GUI  = 0;
}
