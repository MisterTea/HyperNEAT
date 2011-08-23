

__kernel void perceptronLayer(

__global float *activationLevels,

int layerWidth1,
int layerHeight1,
__global float *fromLinkWeights1,
int layerWidth2,
int layerHeight2,
__global float *fromLinkWeights2,
int layerWidth3,
int layerHeight3,

__local float *shared
)
{
	// get index into global data array
	int iGID = get_global_id(0);
	int iLID = get_local_id(0);
	int i;
	
	int layerSize1=layerHeight1*layerWidth1;
	int layerSize2=layerHeight2*layerWidth2;
	int layerSize3=layerHeight3*layerWidth3;
	
	if(iLID < layerSize1)
	{
	    shared[iLID] = activationLevels[iLID];
	}
	if(iLID==0)
	{
	    for(i=get_local_size(0);i<layerSize1;i++)
	    {
	        shared[i] = activationLevels[i];
	    }
	}
	
	barrier(CLK_LOCAL_MEM_FENCE);
	
	// bound check (equivalent to the limit on a 'for' loop for standard/serial C code
	shared[iGID+layerSize1]=0.0f;
	if (iGID < layerSize2)
	{   
	    int sharedOutputStart = layerSize1;
	    int linkWeightStart = iGID*layerSize1;
	    
	    for(i=0;i<layerSize1;i++)
	    {
	        shared[iGID+layerSize1] += shared[i]*fromLinkWeights1[linkWeightStart + i];
	    }
	}
	    
	barrier(CLK_LOCAL_MEM_FENCE);

	if (iGID < layerSize2)
	{   
	    shared[iGID] = (2.0 / (1.0 + exp(-shared[iGID+layerSize1]))) - 1.0;
	}

	barrier(CLK_LOCAL_MEM_FENCE);
	
	float answer=0.0f;
	int linkWeightStart;
	// bound check (equivalent to the limit on a 'for' loop for standard/serial C code
	if (iGID < layerSize3)
	{   
	    linkWeightStart = iGID*layerSize2;
	    
	    for(i=0;i<layerSize2;i++)
	    {
	        answer += shared[i]*fromLinkWeights2[linkWeightStart + i];
	    }
	    
	    activationLevels[iGID] = (2.0 / (1.0 + exp(-answer))) - 1.0;
	}
}

