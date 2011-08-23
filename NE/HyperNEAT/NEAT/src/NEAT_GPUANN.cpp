#include "NEAT_Defines.h"

#include "NEAT_GPUANN.h"

#include "NEAT_Random.h"

#include "NEAT_GeneticIndividual.h"

#include "NEAT_Globals.h"

#include "NEAT_GeneticLinkGene.h"
#include "NEAT_GeneticNodeGene.h"

#ifdef __APPLE__
// common SDK header for standard utilities and system libs 
#include <OpenCL/cl.h>
#else
// common SDK header for standard utilities and system libs 
#include <CL/cl.h>
#include <CL/cl_ext.h>
#endif

#define DEBUG_ACTIVATION_CALCULATION (0)

#define DEBUG_NETWORK_CREATION (0)

#define DEBUG_NETWORK_UPDATE (0)

boost::mutex gpuMutex;
bool first=true;

// OpenCL Vars
cl_context cxGPUContext;        // OpenCL context
cl_command_queue cqCommandQue;  // OpenCL command que
cl_device_id* cdDevices;        // OpenCL device list    
cl_program cpProgram;           // OpenCL program
cl_kernel ckKernel;             // OpenCL kernel
cl_mem cmActivationLevels;               // OpenCL device source buffer A
cl_mem cmFromLinkWeights1;                // OpenCL device destination buffer 
cl_mem cmFromLinkWeights2;                // OpenCL device destination buffer 
size_t szGlobalWorkSize;        // 1D var for Total # of work items
size_t szSharedSize;        // 1D var for Total # of work items
size_t szLocalWorkSize;		    // 1D var for # of work items in the work group	
size_t szParmDataBytes;			// Byte size of context information
size_t szKernelLength;			// Byte size of kernel code
cl_int ciErr1, ciErr2;			// Error code var
string cPathAndName;      // var for full paths to data, src, etc.
char* cSourceCL = NULL;         // Buffer to hold source for compilation 
cl_event execEvent;
cl_platform_id cpPlatform;      // OpenCL platform
cl_device_id cdDevice;          // OpenCL device
boost::filesystem::path BASE_EXE_DIRECTORY;

void Cleanup (int iExitCode);

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

namespace NEAT
{
    extern double signedSigmoidTable[6001];
    extern double unsignedSigmoidTable[6001];

    GPUANN *lastANNUpdated = NULL;

    GPUANN::GPUANN(const vector<NetworkLayer<float> > &_layers)
        :
        FastLayeredNetwork<float>(_layers)
    {
        //Perform a sanity check on the layers
        for(size_t toLayer=0;toLayer<layers.size();toLayer++)
        {
            for(int a=0;a<(int)layers[toLayer].fromLayers.size();a++)
            {
                size_t fromLayer = layers[toLayer].fromLayers[a];

                if(fromLayer>=toLayer)
                {
                    throw CREATE_LOCATEDEXCEPTION_INFO("Network is not feed-forward!");
                }
            }
        }

        if(first)
        {
            first=false;
	        BASE_EXE_DIRECTORY = boost::filesystem::path("./");

            //Get an OpenCL platform
            ciErr1 = clGetPlatformIDs(1, &cpPlatform, NULL);

            printf("clGetPlatformID...\n"); 
            if (ciErr1 != CL_SUCCESS)
            {
                printf("Error in clGetPlatformID, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
                Cleanup(EXIT_FAILURE);
            }

            //Get the devices
            ciErr1 = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 1, &cdDevice, NULL);
            printf("clGetDeviceIDs...\n"); 
            if (ciErr1 != CL_SUCCESS)
            {
                printf("Error in clGetDeviceIDs, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
                Cleanup(EXIT_FAILURE);
            }

            //Create the context
            cxGPUContext = clCreateContext(0, 1, &cdDevice, NULL, NULL, &ciErr1);
            printf("clCreateContext...\n"); 
            if (ciErr1 != CL_SUCCESS)
            {
                printf("Error in clCreateContext, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
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

	        for(int a=0;a<(int)(szParmDataBytes/sizeof(cl_device_id));a++)
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

            // Read the OpenCL kernel in from source file
            printf( "oclLoadProgSource (%s)...\n", (BASE_EXE_DIRECTORY / string("./OpenCL/IteratedDotProduct.cl")).string().c_str()); 
            cPathAndName = (BASE_EXE_DIRECTORY / string("./OpenCL/IteratedDotProduct.cl")).string();
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

            cmActivationLevels = clCreateBuffer(cxGPUContext, CL_MEM_READ_WRITE, sizeof(cl_float)*4096 , NULL, &ciErr2);
            ciErr1 |= ciErr2;
            cmFromLinkWeights1 = clCreateBuffer(cxGPUContext, CL_MEM_READ_ONLY, sizeof(cl_float)*4096*4096 , NULL, &ciErr2);
            ciErr1 |= ciErr2;
            cmFromLinkWeights2 = clCreateBuffer(cxGPUContext, CL_MEM_READ_ONLY, sizeof(cl_float)*4096*4096 , NULL, &ciErr2);
            ciErr1 |= ciErr2;

            //printf( "clCreateBuffer...\n"); 
            if (ciErr1 != CL_SUCCESS)
            {
                printf( "Error in clCreateBuffer, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
                Cleanup(EXIT_FAILURE);
            }
            
            // Create the kernel
            ckKernel = clCreateKernel(cpProgram, "perceptronLayer", &ciErr1);
            //printf( "clCreateKernel (perceptronLayer)...\n"); 
            if (ciErr1 != CL_SUCCESS)
            {
                printf( "Error in clCreateKernel, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
                Cleanup(EXIT_FAILURE);
            }
        }
    }

    
    GPUANN::GPUANN()
    {
    }

    
    GPUANN::~GPUANN()
    {
    }

    void GPUANN::update()
    {
        boost::mutex::scoped_lock lock(gpuMutex);

        int largestLayerSize=0;
        for(vector<NetworkLayer<float> >::iterator layer = layers.begin();layer != layers.end();layer++)
        {
            largestLayerSize = max(largestLayerSize,(int)layer->nodeValues.size());
        }

        // set and log Global and Local work size dimensions
        szLocalWorkSize = 128;
        //int workGroupCount = (szGlobalWorkSize + (szLocalWorkSize-1))/szLocalWorkSize;

        szGlobalWorkSize = shrRoundUp((int)szLocalWorkSize, largestLayerSize);  // rounded up to the nearest multiple of the LocalWorkSize

        szSharedSize = (largestLayerSize*2) * sizeof(float);

        //printf( "Global Work Size \t\t= %u\nLocal Work Size \t\t= %u\n# of Work Groups \t\t= %u\n\n", 
               //szGlobalWorkSize, szLocalWorkSize, (szGlobalWorkSize % szLocalWorkSize + szGlobalWorkSize/szLocalWorkSize)); 

        // --------------------------------------------------------
        // Start Core sequence... copy input data to GPU, compute, copy results back

        // Asynchronous write of data to GPU device
        //ciErr1 = clEnqueueWriteBuffer(cqCommandQue, cmDevSrcA, CL_TRUE, 0, sizeof(cl_float) * iNumElements, srcA, 0, NULL, NULL);
        //size_t usedOrigin[3] = {0,0,0};
        //size_t usedRegion[3] = {128/4,szGlobalWorkSize,1};
        ciErr1 = 0;
        //ciErr1 |= clEnqueueWriteImage(cqCommandQue, cmNodeReverseAdjacencyList, CL_TRUE, usedOrigin, usedRegion, 0, 0, nodeReverseAdjacencyListData, 0, NULL, NULL);
        //ciErr1 |= clEnqueueWriteImage(cqCommandQue, cmLinkWeightAdjacencyList, CL_TRUE, usedOrigin, usedRegion, 0, 0, linkWeightAdjacencyListData, 0, NULL, NULL);
        ciErr1 |= clEnqueueWriteBuffer(cqCommandQue, cmActivationLevels, CL_FALSE, 0, sizeof(cl_int)*layers[0].nodeValues.size() , &(layers[0].nodeValues[0]) , 0, NULL, NULL);

        if(lastANNUpdated!=this)
        {
            lastANNUpdated=this;
            ciErr1 |= clEnqueueWriteBuffer(cqCommandQue, cmFromLinkWeights1, CL_TRUE, 0, sizeof(cl_float)*layers[0].nodeValues.size()*layers[1].nodeValues.size() , &(layers[1].fromWeights[0][0]) , 0, NULL, NULL);
            ciErr1 |= clEnqueueWriteBuffer(cqCommandQue, cmFromLinkWeights2, CL_TRUE, 0, sizeof(cl_float)*layers[1].nodeValues.size()*layers[2].nodeValues.size() , &(layers[2].fromWeights[0][0]) , 0, NULL, NULL);
        }

        //printf( "clEnqueueWriteBuffer (SrcA and SrcB)...\n"); 
        if (ciErr1 != CL_SUCCESS)
        {
            printf( "Error in clEnqueueWriteBuffer, Line %u in file %s error %d !!!\n\n", __LINE__, __FILE__,ciErr1);
            Cleanup(EXIT_FAILURE);
        }

        // Set the Argument values
        ciErr1 = clSetKernelArg(ckKernel, 0, sizeof(cl_mem), (void*)&cmActivationLevels);

        cl_int fromLayerWidth1 = layers[0].nodeStride;
        ciErr1 |= clSetKernelArg(ckKernel, 1, sizeof(cl_int), (void*)&fromLayerWidth1);
        cl_int fromLayerHeight1 = layers[0].nodeValues.size()/layers[0].nodeStride;
        ciErr1 |= clSetKernelArg(ckKernel, 2, sizeof(cl_int), (void*)&fromLayerHeight1);
        ciErr1 |= clSetKernelArg(ckKernel, 3, sizeof(cl_mem), (void*)&cmFromLinkWeights1);

        cl_int fromLayerWidth2 = layers[1].nodeStride;
        ciErr1 |= clSetKernelArg(ckKernel, 4, sizeof(cl_int), (void*)&fromLayerWidth2);
        cl_int fromLayerHeight2 = layers[1].nodeValues.size()/layers[1].nodeStride;
        ciErr1 |= clSetKernelArg(ckKernel, 5, sizeof(cl_int), (void*)&fromLayerHeight2);
        ciErr1 |= clSetKernelArg(ckKernel, 6, sizeof(cl_mem), (void*)&cmFromLinkWeights2);

        cl_int fromLayerWidth3 = layers[2].nodeStride;
        ciErr1 |= clSetKernelArg(ckKernel, 7, sizeof(cl_int), (void*)&fromLayerWidth3);
        cl_int fromLayerHeight3 = layers[2].nodeValues.size()/layers[2].nodeStride;
        ciErr1 |= clSetKernelArg(ckKernel, 8, sizeof(cl_int), (void*)&fromLayerHeight3);

        ciErr1 |= clSetKernelArg(ckKernel, 9, szSharedSize, NULL);

        //printf( "clSetKernelArg 0 - 3...\n\n"); 
        if (ciErr1 != CL_SUCCESS)
        {
            printf( "Error in clSetKernelArg, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
            Cleanup(EXIT_FAILURE);
        }

        // Launch kernel
        execEvent=0;
        ciErr1 = clEnqueueNDRangeKernel(
            cqCommandQue, 
            ckKernel, 
            1, 
            NULL, 
            &szGlobalWorkSize, 
            &szLocalWorkSize, 
            0, 
            NULL, 
            NULL
            );

        //printf( "clEnqueueNDRangeKernel (ANN)...\n"); 
        if (ciErr1 != CL_SUCCESS)
        {
            printf( "Error in clEnqueueNDRangeKernel, Line %u in file %s (Error %d)!!!\n\n", __LINE__, __FILE__, ciErr1);
            Cleanup(EXIT_FAILURE);
        }

        ciErr1 = clEnqueueReadBuffer(
            cqCommandQue, 
            cmActivationLevels, 
            CL_TRUE,
            0, 
            sizeof(cl_float)*layers[2].nodeValues.size() , 
            &(layers[2].nodeValues[0]), 
            0, 
            NULL, 
            NULL
            );

        //printf( "clEnqueueReadBuffer (Dst)...\n\n"); 
        if (ciErr1 != CL_SUCCESS)
        {
            printf("ERROR: %d\n",ciErr1);
            printf( "Error in clEnqueueReadBuffer, Line %u in file %s !!!\n\n", __LINE__, __FILE__);
            Cleanup(EXIT_FAILURE);
        }

    }
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

    // finalize logs and leave
    printf( "oclVectorAdd.exe Exiting...\nPress <Enter> to Quit\n");
    getchar();
}
