// OpenCL Kernel Function for element by element vector addition
__kernel void ANN(
__global int* nodeReverseAdjacencyList,
__global float* linkWeightAdjacencyList,
__global const float* currentNodeValues,
__global float* newNodeValues,
constant int numNodes,
__local float* currentNodeValuesLocal
)
{
	/*
	int ind=get_local_id(0);
	for(;ind<numNodes;ind+=get_local_size(0))
	{
		currentNodeValuesLocal[ind] = currentNodeValues[ind];
	}
	barrier(CLK_LOCAL_MEM_FENCE);
	*/

	// get index into global data array
	int currentToNode = get_global_id(0);
	int sizeToJump = get_global_size(0);
	
	for(;currentToNode<numNodes;currentToNode+=sizeToJump)
	{
	
	// bound check (equivalent to the limit on a 'for' loop for standard/serial C code
	if (currentToNode >= numNodes)
	{   
		return; 
	}
	
	float output = 0;
	int currentFromNode = 0;
	
	int colOffset = currentToNode;
	int fromNode;
	float linkWeight;
	for(currentFromNode=0 ; currentFromNode<128 ; currentFromNode++)
	{
		fromNode = nodeReverseAdjacencyList[currentFromNode*numNodes + colOffset];
		if(fromNode<0)
		{
			break;
		}
		linkWeight = linkWeightAdjacencyList[currentFromNode*numNodes + colOffset];
		output += currentNodeValues[fromNode] * linkWeight;
	}
	newNodeValues[currentToNode] = output;
	
	}
}

