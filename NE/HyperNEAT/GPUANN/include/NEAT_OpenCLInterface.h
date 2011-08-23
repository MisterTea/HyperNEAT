#ifndef __NEAT_OPENCL_INTERFACE_H__
#define __NEAT_OPENCL_INTERFACE_H__

#include "NEAT.h"

#include "JGTL_Singleton.h"

#ifdef __APPLE__
// Common headers:  Cross-API utililties and OpenCL header
#include <OpenCL/cl.h>
#else
// Common headers:  Cross-API utililties and OpenCL header
#include <CL/cl.h>
#include <CL/clext.h>
#endif

namespace NEAT
{
	class OpenCLInterface : public JGTL::Singleton<OpenCLInterface>
	{
	public:
	protected:
		// OpenCL Vars
		cl_context cxGPUContext;        // OpenCL context
		cl_command_queue cqCommandQue;  // OpenCL command que
		cl_device_id* cdDevices;        // OpenCL device list    
		cl_program cpProgram;           // OpenCL program
		cl_kernel ckKernel;             // OpenCL kernel
		cl_mem cmDevSrcA;               // OpenCL device source buffer A
		cl_mem cmDevSrcB;               // OpenCL device source buffer B 
		cl_mem cmDevDst;                // OpenCL device destination buffer 
		size_t szGlobalWorkSize;        // 1D var for Total # of work items
		size_t szLocalWorkSize;		    // 1D var for # of work items in the work group	
		size_t szParmDataBytes;			// Byte size of context information
		size_t szKernelLength;			// Byte size of kernel code
		cl_int ciErr1, ciErr2;			// Error code var
		string cPathAndName;      // var for full paths to data, src, etc.
		char* cSourceCL;         // Buffer to hold source for compilation 

	public:
		static OpenCLInterface* createInstance()
		{
			instance = new OpenCLInterface();
			return instance;
		}

	protected:

	private:
		OpenCLInterface();

		virtual ~OpenCLInterface();
	};
}

#endif
