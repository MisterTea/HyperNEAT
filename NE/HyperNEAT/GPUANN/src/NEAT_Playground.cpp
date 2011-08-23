/*
 * Copyright 1993-2009 NVIDIA Corporation.  All rights reserved.
 *
 * NVIDIA Corporation and its licensors retain all intellectual property and 
 * proprietary rights in and to this software and related documentation. 
 * Any use, reproduction, disclosure, or distribution of this software 
 * and related documentation without an express license agreement from
 * NVIDIA Corporation is strictly prohibited.
 *
 * Please refer to the applicable NVIDIA end user license agreement (EULA) 
 * associated with this source code for terms and conditions that govern 
 * your use of this NVIDIA software.
 * 
 */

// *********************************************************************
// oclVectorAdd Notes:  
//
// A simple OpenCL API demo application that implements 
// element by element vector addition between 2 float arrays. 
//
// Runs computations with OpenCL on the GPU device and then checks results 
// against basic host CPU/C++ computation.
//
// Uses some 'shr' and 'ocl' functions from oclUtils and shrUtils libraries for 
// compactness, but these are NOT required libs for OpenCL developement in general.
// *********************************************************************

#include "NEAT.h"

#include <boost/filesystem.hpp>
#include <boost/progress.hpp>
#include <boost/thread/xtime.hpp>

#ifdef __APPLE__
// common SDK header for standard utilities and system libs 
#include <OpenCL/cl.h>
#else
// common SDK header for standard utilities and system libs 
#include <CL/cl.h>
#include <CL/clext.h>
#endif

// Round Up Division function
size_t shrRoundUp(int group_size, int global_size) 
{
    int r = global_size % group_size;
    if(r == 0) 
    {
        return global_size;
    } 
	else 
    {
        return global_size + group_size - r;
    }
}

// Helper function to init data arrays 
// *********************************************************************
void shrFillArray(float* pfData, int iSize)
{
    int i; 
    const float fScale = 1.0f / (float)(RAND_MAX+1);
    for (i = 0; i < iSize; ++i) 
    {
        pfData[i] = fScale * rand();
    }
}

// Helper function to check diffs between 2 same-sized result arrays and count/report the errors
// *********************************************************************
int shrDiffArray(const float* pfResult1, const float* pfResult2, int iNumElements)
{
    // sweep through arrays and count differences between test array and golden
    int iErrorCount = 0, i;
    for (i = 0; i < iNumElements; i++) 
    {
        if (fabsf(pfResult2[i] - pfResult1[i]) > 1e-2) 
        {
			cout << "Index (" << i << "): CPU: " << pfResult1[i] << " GPU: " << pfResult2[i] << endl;
            iErrorCount++;
        }
    }
    return iErrorCount;
}

//////////////////////////////////////////////////////////////////////////////
//! Loads a Program file and prepends the cPreamble to the code.
//!
//! @return the source string if succeeded, 0 otherwise
//! @param cFilename        program filename
//! @param cPreamble        code that is prepended to the loaded file, typically a set of #defines or a header
//! @param szFinalLength    returned length of the code string
//////////////////////////////////////////////////////////////////////////////
char* oclLoadProgSource(const char* cFilename, const char* cPreamble, size_t* szFinalLength)
{
    // locals 
    FILE* pFileStream = NULL;
    size_t szSourceLength;

    // open the OpenCL source code file
    #ifdef _WIN32   // Windows version
        if(fopen_s(&pFileStream, cFilename, "rb") != 0) 
        {       
            return NULL;
        }
    #else           // Linux version
        pFileStream = fopen(cFilename, "rb");
        if(pFileStream == 0) 
        {       
            return NULL;
        }
    #endif

    size_t szPreambleLength = strlen(cPreamble);

    // get the length of the source code
    fseek(pFileStream, 0, SEEK_END); 
    szSourceLength = ftell(pFileStream);
    fseek(pFileStream, 0, SEEK_SET); 

    // allocate a buffer for the source code string and read it in
    char* cSourceString = (char *)malloc(szSourceLength + szPreambleLength + 1); 
    memcpy(cSourceString, cPreamble, szPreambleLength);
    if (fread((cSourceString) + szPreambleLength, szSourceLength, 1, pFileStream) != 1)
    {
        fclose(pFileStream);
        free(cSourceString);
        return 0;
    }

    // close the file and return the total length of the combined (preamble + source) string
    fclose(pFileStream);
    if(szFinalLength != 0)
    {
        *szFinalLength = szSourceLength + szPreambleLength;
    }
    cSourceString[szSourceLength + szPreambleLength] = '\0';

    return cSourceString;
}

// Host buffers for demo
// *********************************************************************

// OpenCL Vars
cl_context cxGPUContext;        // OpenCL context
cl_command_queue cqCommandQue;  // OpenCL command que
cl_device_id* cdDevices;        // OpenCL device list    
cl_program cpProgram;           // OpenCL program
cl_kernel ckKernel;             // OpenCL kernel
cl_mem cmNodeReverseAdjacencyList;               // OpenCL device source buffer A
cl_mem cmLinkWeightAdjacencyList;               // OpenCL device source buffer B 
cl_mem cmCurrentNodeValues;                // OpenCL device destination buffer 
cl_mem cmNewNodeValues;                // OpenCL device destination buffer 
size_t szGlobalWorkSize;        // 1D var for Total # of work items
size_t szLocalWorkSize;		    // 1D var for # of work items in the work group	
size_t szParmDataBytes;			// Byte size of context information
size_t szKernelLength;			// Byte size of kernel code
cl_int ciErr1, ciErr2;			// Error code var
string cPathAndName;      // var for full paths to data, src, etc.
char* cSourceCL = NULL;         // Buffer to hold source for compilation 

// Forward Declarations
// *********************************************************************
void ANN(
		 cl_int** nodeReverseAdjacencyList, 
		 cl_float** linkWeightAdjacencyList, 
		 cl_float* currentNodeValues, 
		 cl_float* newNodeValues, 
		 int numNodes
		 );
void Cleanup (int iExitCode);

boost::filesystem::path BASE_EXE_DIRECTORY;

// Main function 
// *********************************************************************
int main(int argc,char** argv)
{
	BASE_EXE_DIRECTORY = boost::filesystem::path(argv[0]).parent_path().parent_path();
#ifdef __APPLE__
	BASE_EXE_DIRECTORY = BASE_EXE_DIRECTORY.parent_path().parent_path().parent_path();
#endif

	int numNodes = 4096;

    // start logs 
    //printf( "%s Starting...\n\n# of elements per Array \t= %i\n", argv[0], iNumElements); 

    // set and log Global and Local work size dimensions
    szLocalWorkSize = 128;
    szGlobalWorkSize = 2048;
	int workGroupCount = (szGlobalWorkSize + (szLocalWorkSize-1))/szLocalWorkSize;

    //szGlobalWorkSize = shrRoundUp((int)szLocalWorkSize, numNodes);  // rounded up to the nearest multiple of the LocalWorkSize

    printf( "Global Work Size \t\t= %u\nLocal Work Size \t\t= %u\n# of Work Groups \t\t= %u\n\n", 
           szGlobalWorkSize, szLocalWorkSize, (szGlobalWorkSize % szLocalWorkSize + szGlobalWorkSize/szLocalWorkSize)); 

    // Allocate and initialize host arrays 
    printf(  "Allocate and Init Host Mem...\n"); 
	cl_int** nodeReverseAdjacencyList = (cl_int**)malloc(sizeof(cl_int*)*128);
	cl_int* nodeReverseAdjacencyListData = (cl_int*)malloc(sizeof(cl_int)*128*numNodes);
	for(int a=0;a<128;a++)
	{
		nodeReverseAdjacencyList[a] = &(nodeReverseAdjacencyListData[a*numNodes]);
	}
	for(int a=0;a<128;a++)
	{
		for(int b=0;b<numNodes;b++)
		{
			nodeReverseAdjacencyList[a][b]=-1;
		}
	}
	for(int a=0;a<64;a++)
	{
		for(int b=0;b<numNodes;b++)
		{
			//if(a>b)
			{
				nodeReverseAdjacencyList[a][b]=a;
			}
		}
	}

	cl_float** linkWeightAdjacencyList = (cl_float**)malloc(sizeof(cl_float*)*numNodes);
	cl_float* linkWeightAdjacencyListData = (cl_float*)malloc(sizeof(cl_float)*128*numNodes);
	for(int a=0;a<128;a++)
	{
		linkWeightAdjacencyList[a] = &(linkWeightAdjacencyListData[a*numNodes]);
	}
	for(int a=0;a<128;a++)
	{
		//shrFillArray(linkWeightAdjacencyList[a], numNodes);
		for(int b=0;b<numNodes;b++)
		{
			linkWeightAdjacencyList[a][b]=1.0;
		}
	}

	cl_float* currentNodeValues = (cl_float*)malloc(sizeof(cl_float)*numNodes);
	memset(currentNodeValues,0,sizeof(cl_float) * numNodes);
    //shrFillArray(currentNodeValues, numNodes);
	for(int b=0;b<numNodes;b++)
	{
		currentNodeValues[b]=1.0;
	}

	cl_float* newNodeValues = (cl_float*)malloc(sizeof(cl_float)*numNodes);
	memset(newNodeValues,0,sizeof(cl_float) * numNodes);

	cl_float* newNodeValuesGolden = (cl_float*)malloc(sizeof(cl_float)*numNodes);
	memset(newNodeValuesGolden,0,sizeof(cl_float) * numNodes);

	boost::xtime startTime,curTime;
    const int MILLISECONDS_PER_SECOND = 1000;
    const int NANOSECONDS_PER_SECOND = 1000000000;
    const int NANOSECONDS_PER_MILLISECOND = 1000000;

#if 1
	// Create the OpenCL context on a GPU device
    cxGPUContext = clCreateContextFromType(0, CL_DEVICE_TYPE_GPU, NULL, NULL, &ciErr1);
    printf( "clCreateContextFromType...\n"); 
    if (ciErr1 != CL_SUCCESS)
    {
        printf( "Error in clCreateContextFromType, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        Cleanup(EXIT_FAILURE);
    }

    // Get the list of GPU devices associated with context
    ciErr1 = clGetContextInfo(cxGPUContext, CL_CONTEXT_DEVICES, 0, NULL, &szParmDataBytes);
    cdDevices = (cl_device_id*)malloc(szParmDataBytes);
    ciErr1 |= clGetContextInfo(cxGPUContext, CL_CONTEXT_DEVICES, szParmDataBytes, cdDevices, NULL);
    printf( "clGetContextInfo...\n"); 
    if (ciErr1 != CL_SUCCESS)
    {
        printf( "Error in clGetContextInfo, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        Cleanup(EXIT_FAILURE);
    }

	for(int a=0;a<(szParmDataBytes/sizeof(cl_device_id));a++)
	{
		cl_uint numComputeUnits;
		clGetDeviceInfo(cdDevices[a],CL_DEVICE_MAX_COMPUTE_UNITS,4,&numComputeUnits,NULL);
		printf("NUM COMPUTE UNITS: %d\n",numComputeUnits);
	}

    // Create a command-queue
    cqCommandQue = clCreateCommandQueue(cxGPUContext, cdDevices[0], CL_QUEUE_PROFILING_ENABLE, &ciErr1);
    printf( "clCreateCommandQueue...\n"); 
    if (ciErr1 != CL_SUCCESS)
    {
        printf( "Error in clCreateCommandQueue, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        Cleanup(EXIT_FAILURE);
    }

	cl_uint numImageFormats;
	cl_image_format imageFormats[1024];
	clGetSupportedImageFormats(cxGPUContext,CL_MEM_READ_ONLY,CL_MEM_OBJECT_IMAGE2D,1024,imageFormats,&numImageFormats);

	for(int a=0;a<(int)numImageFormats;a++)
	{
		cout << imageFormats[a].image_channel_data_type << ", " << imageFormats[a].image_channel_order << endl;
	}

	/*
    // Allocate the OpenCL buffer memory objects for source and result on the device GMEM
	{
		cl_image_format format;
		format.image_channel_order = CL_RGBA;
		format.image_channel_data_type = CL_SIGNED_INT32;
		cmNodeReverseAdjacencyList = clCreateImage2D(cxGPUContext, CL_MEM_READ_ONLY, &format ,128/4,numNodes,0,NULL,&ciErr1);
	}
	{
		cl_image_format format;
		format.image_channel_order = CL_RGBA;
		format.image_channel_data_type = CL_FLOAT;
		cmLinkWeightAdjacencyList = clCreateImage2D(cxGPUContext, CL_MEM_READ_ONLY, &format ,128/4,numNodes,0,NULL, &ciErr2);
	}
    ciErr1 |= ciErr2;
	*/

    cmNodeReverseAdjacencyList = clCreateBuffer(cxGPUContext, CL_MEM_READ_ONLY, sizeof(cl_int) * numNodes * 128, NULL, &ciErr2);
    ciErr1 |= ciErr2;
    cmLinkWeightAdjacencyList = clCreateBuffer(cxGPUContext, CL_MEM_READ_ONLY, sizeof(cl_float) * numNodes * 128, NULL, &ciErr2);
    ciErr1 |= ciErr2;
    cmCurrentNodeValues = clCreateBuffer(cxGPUContext, CL_MEM_READ_ONLY, sizeof(cl_float) * numNodes, NULL, &ciErr2);
    ciErr1 |= ciErr2;
    cmNewNodeValues = clCreateBuffer(cxGPUContext, CL_MEM_WRITE_ONLY, sizeof(cl_float) * numNodes, NULL, &ciErr2);
    ciErr1 |= ciErr2;

    printf( "clCreateBuffer...\n"); 
    if (ciErr1 != CL_SUCCESS)
    {
        printf( "Error in clCreateBuffer, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        Cleanup(EXIT_FAILURE);
    }
    
    // Read the OpenCL kernel in from source file
    printf( "oclLoadProgSource (%s)...\n", (BASE_EXE_DIRECTORY / string("../out/OpenCL/ANN.cl")).string().c_str()); 
    cPathAndName = (BASE_EXE_DIRECTORY / string("../out/OpenCL/ANN.cl")).string();
    cSourceCL = oclLoadProgSource(cPathAndName.c_str(), "", &szKernelLength);

    // Create the program
    cpProgram = clCreateProgramWithSource(cxGPUContext, 1, (const char **)&cSourceCL, &szKernelLength, &ciErr1);
    printf( "clCreateProgramWithSource...\n"); 
    if (ciErr1 != CL_SUCCESS)
    {
        printf( "Error in clCreateProgramWithSource, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        Cleanup(EXIT_FAILURE);
    }

    // Build the program
    ciErr1 = clBuildProgram(cpProgram, 0, NULL, NULL, NULL, NULL);
    printf( "clBuildProgram...\n"); 
    if (ciErr1 != CL_SUCCESS)
    {
        printf( "Error in clBuildProgram, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
		printf( "Error: %d\n",int(ciErr1));
        Cleanup(EXIT_FAILURE);
    }

    // Create the kernel
    ckKernel = clCreateKernel(cpProgram, "ANN", &ciErr1);
    printf( "clCreateKernel (ANN)...\n"); 
    if (ciErr1 != CL_SUCCESS)
    {
        printf( "Error in clCreateKernel, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
        Cleanup(EXIT_FAILURE);
    }

    // --------------------------------------------------------
    // Start Core sequence... copy input data to GPU, compute, copy results back

    // Asynchronous write of data to GPU device
    //ciErr1 = clEnqueueWriteBuffer(cqCommandQue, cmDevSrcA, CL_TRUE, 0, sizeof(cl_float) * iNumElements, srcA, 0, NULL, NULL);
	size_t usedOrigin[3] = {0,0,0};
	size_t usedRegion[3] = {128/4,numNodes,1};
	ciErr1 = 0;
    //ciErr1 |= clEnqueueWriteImage(cqCommandQue, cmNodeReverseAdjacencyList, CL_TRUE, usedOrigin, usedRegion, 0, 0, nodeReverseAdjacencyListData, 0, NULL, NULL);
    //ciErr1 |= clEnqueueWriteImage(cqCommandQue, cmLinkWeightAdjacencyList, CL_TRUE, usedOrigin, usedRegion, 0, 0, linkWeightAdjacencyListData, 0, NULL, NULL);
    ciErr1 |= clEnqueueWriteBuffer(cqCommandQue, cmNodeReverseAdjacencyList, CL_TRUE, 0, sizeof(cl_int) * numNodes * 128, nodeReverseAdjacencyListData, 0, NULL, NULL);
    ciErr1 |= clEnqueueWriteBuffer(cqCommandQue, cmLinkWeightAdjacencyList, CL_TRUE, 0, sizeof(cl_float) * numNodes * 128, linkWeightAdjacencyListData, 0, NULL, NULL);
    ciErr1 |= clEnqueueWriteBuffer(cqCommandQue, cmCurrentNodeValues, CL_TRUE, 0, sizeof(cl_float) * numNodes, currentNodeValues, 0, NULL, NULL);
    printf( "clEnqueueWriteBuffer (SrcA and SrcB)...\n"); 
    if (ciErr1 != CL_SUCCESS)
    {
        printf( "Error in clEnqueueWriteBuffer, Line %u in file %s error %d !!!\n\n", __LINE__, __FILE__,ciErr1);
        Cleanup(EXIT_FAILURE);
    }

	clFinish(cqCommandQue);
	
	// Set the Argument values
	ciErr1 = clSetKernelArg(ckKernel, 0, sizeof(cl_mem), (void*)&cmNodeReverseAdjacencyList);
	ciErr1 |= clSetKernelArg(ckKernel, 1, sizeof(cl_mem), (void*)&cmLinkWeightAdjacencyList);
	ciErr1 |= clSetKernelArg(ckKernel, 2, sizeof(cl_mem), (void*)&cmCurrentNodeValues);
	ciErr1 |= clSetKernelArg(ckKernel, 3, sizeof(cl_mem), (void*)&cmNewNodeValues);
	ciErr1 |= clSetKernelArg(ckKernel, 4, sizeof(cl_int), (void*)&numNodes);
	ciErr1 |= clSetKernelArg(ckKernel, 5, sizeof(cl_float)*1, (void*)NULL);
	//printf( "clSetKernelArg 0 - 3...\n\n"); 
	if (ciErr1 != CL_SUCCESS)
	{
		printf( "Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
		Cleanup(EXIT_FAILURE);
	}

	clFinish(cqCommandQue);

	boost::xtime_get(&startTime, boost::TIME_UTC);
	{
		printf("LAUNCHING KERNEL\n");
		cl_ulong nanoSecondsSpentProcessing=0;
		cl_ulong start, end;
		cl_event execEvent;
		for(int a=0;a<500;a++)
		{
			// Launch kernel
			ciErr1 = clEnqueueNDRangeKernel(cqCommandQue, ckKernel, 1, NULL, &szGlobalWorkSize, &szLocalWorkSize, 0, NULL, &execEvent);

			//printf( "clEnqueueNDRangeKernel (ANN)...\n"); 
			if (ciErr1 != CL_SUCCESS)
			{
				printf( "Error in clEnqueueNDRangeKernel, Line %u in file %s (Error %d)!!!\n\n", __LINE__, __FILE__, ciErr1);
				Cleanup(EXIT_FAILURE);
			}
			
			clFinish(cqCommandQue);

			clGetEventProfilingInfo(execEvent, CL_PROFILING_COMMAND_END,
				sizeof(cl_ulong), &end, NULL);
			clGetEventProfilingInfo(execEvent, CL_PROFILING_COMMAND_START,
				sizeof(cl_ulong), &start, NULL);
			nanoSecondsSpentProcessing += (end - start);
		}
		boost::xtime_get(&curTime, boost::TIME_UTC);
		int msTimeDiff = int(nanoSecondsSpentProcessing / NANOSECONDS_PER_MILLISECOND);
			//(curTime.sec-startTime.sec)*MILLISECONDS_PER_SECOND + 
			//(curTime.nsec-startTime.nsec)/NANOSECONDS_PER_MILLISECOND;
		cout << "Seconds to run: " << float(msTimeDiff)/1000.0 << endl;
	}

	// Synchronous/blocking read of results, and check accumulated errors
	ciErr1 = clEnqueueReadBuffer(cqCommandQue, cmNewNodeValues, CL_TRUE, 0, sizeof(cl_float) * numNodes, newNodeValues, 0, NULL, NULL);
	printf( "clEnqueueReadBuffer (Dst)...\n\n"); 
	if (ciErr1 != CL_SUCCESS)
	{
		printf("ERROR: %d\n",ciErr1);
		printf( "Error in clEnqueueReadBuffer, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
		Cleanup(EXIT_FAILURE);
	}
#endif

    //--------------------------------------------------------

    // Compute and compare results for golden-host and report errors and pass/fail
    printf( "Comparing against Host/C++ computation...\n\n"); 
	boost::xtime_get(&startTime, boost::TIME_UTC);
	{
		ANN(nodeReverseAdjacencyList,linkWeightAdjacencyList,currentNodeValues,newNodeValuesGolden,numNodes);
		boost::xtime_get(&curTime, boost::TIME_UTC);
		int msTimeDiff = 
			(curTime.sec-startTime.sec)*MILLISECONDS_PER_SECOND + 
			(curTime.nsec-startTime.nsec)/NANOSECONDS_PER_MILLISECOND;
		cout << "Seconds to run: " << float(msTimeDiff)/1000.0 << endl;
	}
    int iNumErrors = shrDiffArray(newNodeValuesGolden, newNodeValues, numNodes);
    printf( "TEST %s\t (Error Count = %i)\n\n ", (iNumErrors == 0) ? "PASSED" : "FAILED !!!", iNumErrors);

    // Cleanup and leave
    Cleanup (EXIT_SUCCESS);
}

void Cleanup (int iExitCode)
{
    // Cleanup allocated objects
    printf( "Starting Cleanup...\n\n");
    if(cdDevices)free(cdDevices);
    if(cSourceCL)free(cSourceCL);
	if(ckKernel)clReleaseKernel(ckKernel);  
    if(cpProgram)clReleaseProgram(cpProgram);
    if(cqCommandQue)clReleaseCommandQueue(cqCommandQue);
    if(cxGPUContext)clReleaseContext(cxGPUContext);
    if(cmNodeReverseAdjacencyList)clReleaseMemObject(cmNodeReverseAdjacencyList);
    if(cmLinkWeightAdjacencyList)clReleaseMemObject(cmLinkWeightAdjacencyList);
    if(cmCurrentNodeValues)clReleaseMemObject(cmCurrentNodeValues);
    if(cmNewNodeValues)clReleaseMemObject(cmNewNodeValues);

    // finalize logs and leave
    printf( "oclVectorAdd.exe Exiting...\nPress <Enter> to Quit\n");
    getchar();
    exit (iExitCode);
}

// "Golden" Host processing vector addition function for comparison purposes
// *********************************************************************
void ANN(
		 cl_int** nodeReverseAdjacencyList, 
		 cl_float** linkWeightAdjacencyList, 
		 cl_float* currentNodeValues, 
		 cl_float* newNodeValues, 
		 int numNodes
		 )
{
	for(int runCount=0;runCount<500;runCount++)
	{
		for(int currentToNode = 0; currentToNode < numNodes; currentToNode++) 
		{
			// bound check (equivalent to the limit on a 'for' loop for standard/serial C code
			if (currentToNode >= numNodes)
			{   
				return; 
			}
			
			int currentFromNode = 0;
			
			float output = 0;
			for(currentFromNode=0 ; currentFromNode<numNodes ; currentFromNode++)
			{
				if(nodeReverseAdjacencyList[currentFromNode][currentToNode]<0)
					break;
				output += currentNodeValues[nodeReverseAdjacencyList[currentFromNode][currentToNode]] * linkWeightAdjacencyList[currentFromNode][currentToNode];
				//output += nodeReverseAdjacencyList[currentToNode][currentFromNode];
			}
			newNodeValues[currentToNode] = output;
		}
	}
}
