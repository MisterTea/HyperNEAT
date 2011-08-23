#include "NEAT_Defines.h"

#include "NEAT_LayeredSubstrate.h"

#include "NEAT_GeneticIndividual.h"

#include "Board.h"

#define LAYERED_SUBSTRATE_DEBUG (0)

#define DEBUG_USE_DELTAS_ON_LONG_RANGE (1)

#define DEBUG_NO_LONG_RANGE_LINKS (0)

#define DEBUG_MAX_DELTA_RANGE (2)

namespace NEAT
{
template<class NetworkDataType>
class LinkWeightPair
{
 public:
  JGTL::Vector3<int> fromPos;
  NetworkDataType weight;

  LinkWeightPair(
      JGTL::Vector3<int> _fromPos,
      NetworkDataType _weight
      )
      :
      fromPos(_fromPos),
      weight(_weight)
  {
  }
};

template< class NetworkDataType >
LayeredSubstrate<NetworkDataType>::LayeredSubstrate()
{
}

template< class NetworkDataType >
void LayeredSubstrate<NetworkDataType>::setLayerInfo(LayeredSubstrateInfo layerInfo)
{
  layerSizes = layerInfo.layerSizes;
  layerValidSizes = layerInfo.layerValidSizes;
  layerNames = layerInfo.layerNames;

  layerAdjacencyList.clear();
  for(int a=0;a<int(layerInfo.layerAdjacencyList.size());a++)
  {
    int from=-1;
    int to=-1;
    for(int b=0;b<int(layerNames.size());b++)
    {
      if(layerInfo.layerAdjacencyList[a].first == layerNames[b])
      {
        from = b;
      }
      if(layerInfo.layerAdjacencyList[a].second == layerNames[b])
      {
        to = b;
      }
    }
    if(from==-1 || to==-1)
    {
      //Layer(s) don't exist, skip
      cout << "WARNING: Adjacency map between layers " << layerInfo.layerAdjacencyList[a].first 
           << " and " << layerInfo.layerAdjacencyList[a].second << " cannot be created: Layers do not exist\n";
      continue;
    }
    layerAdjacencyList.push_back(JGTL::Vector2<int>(from,to));
  }

  layerIsInput = layerInfo.layerIsInput;
  layerLocations = layerInfo.layerLocations;
  normalize = layerInfo.normalize;
  useOldOutputNames = layerInfo.useOldOutputNames;
  maxDeltaLength = layerInfo.maxDeltaLength;
  maxConnectionLength = layerInfo.maxConnectionLength;

  /*
    for(int a=0;a<int(layerSizes.size());a++)
    {
    cout << layerSizes[a] << endl;
    }
    for(int a=0;a<int(layerAdjacencyList.size());a++)
    {
    cout << layerAdjacencyList[a] << endl;
    }
    for(int a=0;a<int(layerIsInput.size());a++)
    {
    cout << layerIsInput[a] << endl;
    }
    for(int a=0;a<int(layerLocations.size());a++)
    {
    cout << layerLocations[a] << endl;
    }
    cout << "Booleans:\n";
    cout << normalize << endl;
    cout << useOldOutputNames << endl;
  */
}

template< class NetworkDataType >
void LayeredSubstrate<NetworkDataType>::populateSubstrate(
    shared_ptr<NEAT::GeneticIndividual> individual
    )
{
  nameLookup.clear();

  NEAT::FastNetwork<NetworkDataType> cppn = individual->spawnFastPhenotypeStack<NetworkDataType>();

  int linkCounter=0;

#if LAYERED_SUBSTRATE_DEBUG
  double linkChecksum=0.0;
#endif

  int connectionCount=0;

  map<JGTL::Vector3<int>, vector<LinkWeightPair<NetworkDataType> > > allIncomingLinks;

  vector<NetworkLayer<NetworkDataType> > layers;
  for(int a=0;a<int(layerNames.size());a++)
  {
    vector<int> fromLayers;
    for(int b=0;b<int(layerAdjacencyList.size());b++)
    {
      if(layerAdjacencyList[b].y==a)
      {
        fromLayers.push_back(layerAdjacencyList[b].x);
      }
    }
    layers.push_back(NetworkLayer<NetworkDataType>(layerNames[a],layerValidSizes[a].x*layerValidSizes[a].y,layerValidSizes[a].x,fromLayers,layerValidSizes));
  }

  for (int z1=0;z1<(int)layerSizes.size();z1++)
  {
    for (int z2=0;z2<(int)layerSizes.size();z2++)
    {
      if(z1==z2)
        continue;

      string outputNodeName;
      if(useOldOutputNames)
      {
        outputNodeName = string("Output_")+(char('a'+z1))+(char('a'+z2));
      }
      else
      {
        outputNodeName = string("Output_")+layerNames[z1]+string("_")+layerNames[z2];
      }

      if(cppn.hasNode(outputNodeName)==false)
      {
        continue;
      }

      JGTL::Vector2<int> validInputStart = (layerSizes[z1] - layerValidSizes[z1])/2;
      JGTL::Vector2<int> validInputEnd = ((layerSizes[z1] - layerValidSizes[z1])/2) + layerValidSizes[z1];

      JGTL::Vector2<int> validOutputStart = (layerSizes[z2] - layerValidSizes[z2])/2;
      JGTL::Vector2<int> validOutputEnd = ((layerSizes[z2] - layerValidSizes[z2])/2) + layerValidSizes[z2];

      for (int y1=validInputStart.y;y1<validInputEnd.y;y1++)
      {
        for (int x1=validInputStart.x;x1<validInputEnd.x;x1++)
        {
          for (int y2=validOutputStart.y;y2<validOutputEnd.y;y2++)
          {
            for (int x2=validOutputStart.x;x2<validOutputEnd.x;x2++)
            {
              int chessDistance = max(abs(x1-x2),abs(y1-y2));

              if(chessDistance>maxConnectionLength)
              {
                continue;
              }



#if DEBUG_NO_LONG_RANGE_LINKS
              if(z1==0 && max(abs(x2-x1),abs(y2-y1))>DEBUG_MAX_DELTA_RANGE)
              {
                continue;
              }
#endif

              /*Remap the nodes to the [-1,1] domain*/
              NetworkDataType x1normal,y1normal,x2normal,y2normal;

              if (layerSizes[z1].x>1)
              {
                x1normal = -1.0f + (NetworkDataType(x1)/(layerSizes[z1].x-1))*2.0f;
              }
              else
              {
                x1normal = 0.0f;
              }

              if (layerSizes[z1].y>1)
              {
                y1normal = -1.0f + (NetworkDataType(y1)/(layerSizes[z1].y-1))*2.0f;
              }
              else
              {
                y1normal = 0.0f;
              }

              if (layerSizes[z2].x>1)
              {
                x2normal = -1.0f + (NetworkDataType(x2)/(layerSizes[z2].x-1))*2.0f;
              }
              else
              {
                x2normal = 0.0f;
              }

              if (layerSizes[z2].y>1)
              {
                y2normal = -1.0f + (NetworkDataType(y2)/(layerSizes[z2].y-1))*2.0f;
              }
              else
              {
                y2normal = 0.0f;
              }

              cppn.reinitialize();
              if (cppn.hasNode("X1"))
              {
                cppn.setValue("X1",x1normal);
                cppn.setValue("Y1",y1normal);
              }
              if (cppn.hasNode("X2"))
              {
                cppn.setValue("X2",x2normal);
                cppn.setValue("Y2",y2normal);
              }
              if(cppn.hasNode("DeltaX"))
              {
                if(
#if DEBUG_USE_DELTAS_ON_LONG_RANGE
#else
                       max(abs(x2-x1),abs(y2-y1))<=DEBUG_MAX_DELTA_RANGE && 
#endif
                       chessDistance<=maxDeltaLength
                       )
                {
                  //cout << x1 << ',' << x2 << ',' << y1 << ',' << y2 << endl;
                  //cout << x1normal << ',' << x2normal << ',' << y1normal << ',' << y2normal << endl;
                  //cout << "DeltaX:" << (x2normal-x1normal)
                  //<< ", DeltaY: " << (y2normal-y1normal)
                  //<< endl;
                  cppn.setValue("DeltaX",x2normal-x1normal);
                  cppn.setValue("DeltaY",y2normal-y1normal);
                }
                else
                {
                  cppn.setValue("DeltaX",0);
                  cppn.setValue("DeltaY",0);
                }
              }

              if(cppn.hasNode("Bias"))
              {
                cppn.setValue("Bias",(NetworkDataType)0.3);
              }

              cppn.update();

              NetworkDataType output;

              output = cppn.getValue(outputNodeName);

              output = convertOutputToWeight(output);

              JGTL::Vector3<int> inputNode(x1,y1,z1);
              JGTL::Vector3<int> outputNode(x2,y2,z2);
              if(allIncomingLinks.find(outputNode)==allIncomingLinks.end())
              {
                allIncomingLinks[outputNode] = vector<LinkWeightPair<NetworkDataType> >();
              }

              vector<LinkWeightPair<NetworkDataType> > &incomingLinks = allIncomingLinks[outputNode];
              if(fabs(output)>0.0)
              {
                incomingLinks.push_back(LinkWeightPair<NetworkDataType> (inputNode,output));
              }

              linkCounter++;

#if LAYERED_SUBSTRATE_ENABLE_BIASES
              throw CREATE_LOCATEDEXCEPTION_INFO("NOT SUPPORTED YET");
              if (x1==0&&y1==0&&z1==0)
              {
                NetworkDataType nodeBias;

                if (z2==1)
                {
                  nodeBias = network.getValue("Bias_b");

                  nodeBias = convertOutputToWeight(nodeBias);

                  /*{
                    cout << "Setting bias for "
                    << nameLookup[Node(x2-layerSizes[z2].x/2,y2-layerSizes[z2].y/2,1)]
                    << endl;
                    cout << "Bias: " << nodeBias << endl;
                    CREATE_PAUSE("");
                    }*/

                  substrate.setBias(
                      *nameLookup.getData(Node(x2,y2,z2)),
                      nodeBias
                      );
                }
                else if (z2==2)
                {
                  nodeBias = network.getValue("Bias_c");

                  nodeBias = convertOutputToWeight(nodeBias);

                  /*{
                    cout << "Setting bias for "
                    << nameLookup[Node(x2-layerSizes[z2].x/2,y2-layerSizes[z2].y/2,2)]
                    << endl;
                    cout << "Bias: " << nodeBias << endl;
                    CREATE_PAUSE("");
                    }*/

                  substrate.setBias(
                      *nameLookup.getData(Node(x2,y2,z2)),
                      nodeBias
                      );
                }
                else
                {
                  throw CREATE_LOCATEDEXCEPTION_INFO("wtf");
                }
              }
#endif
            }
          }
        }
      }
    }
  }

  if(normalize)
  {
    //cout << "NORMALIZING\n";
    //Normalize (and potentially delete) connections
    typename map<JGTL::Vector3<int>,  vector< LinkWeightPair<NetworkDataType> > >::iterator it;
    for(
            it = allIncomingLinks.begin();
            it!=allIncomingLinks.end();
            it++
            )
    {
                
      //Normalize
      NetworkDataType sumSq=0;
      vector<LinkWeightPair<NetworkDataType> > &incomingLinks = it->second;
      for(int b=0;b<(int)incomingLinks.size();b++)
      {
        sumSq += incomingLinks[b].weight*incomingLinks[b].weight;
      }
      NetworkDataType magnitude = sqrt(sumSq);

      //Divide by magnitude & delete
      for(int b=int(incomingLinks.size())-1;b>=0;b--)
      {
        //Normalize to 3.0
        incomingLinks[b].weight = incomingLinks[b].weight*3.0f/magnitude;

        if(fabs(incomingLinks[b].weight)<0.05)
        {
          //The weight is too small, kill it
          incomingLinks.erase(incomingLinks.begin()+b);
        }
      }

      //Renormalize
      sumSq=0;
      incomingLinks = it->second;
      for(int b=0;b<(int)incomingLinks.size();b++)
      {
        sumSq += incomingLinks[b].weight*incomingLinks[b].weight;
      }
      magnitude = sqrt(sumSq);
      for(int b=int(incomingLinks.size())-1;b>=0;b--)
      {
        //Normalize to 3.0
        incomingLinks[b].weight = incomingLinks[b].weight*3.0f/magnitude;
      }
    }
  }

#if 0
  //Create network from connections
  int numLinks=0;
  typename map<JGTL::Vector3<int>,  vector< LinkWeightPair<NetworkDataType> > >::iterator it;
  for(
          it = allIncomingLinks.begin();
          it!=allIncomingLinks.end();
          it++
          )
  {
    vector<LinkWeightPair<NetworkDataType> > &incomingLinks = it->second;
    for(int b=0;b<(int)incomingLinks.size();b++)
    {
      numLinks++;
    }
  }

  int numNodes = 0;
  for(int a=0;a<(int)layerSizes.size();a++)
  {
    numNodes += (layerSizes[a].x*layerSizes[a].y);
    //cout << "LAYER SIZE: " << layerSizes[a] << endl;
  }

  //cout << "NUM NODES: " << numNodes << endl;
  //cout << "NUM LINKS: " << numLinks << endl;

  NetworkNode* tmpNodes = new NetworkNode[numNodes];
  NetworkLink* tmpLinks = new NetworkLink[numLinks];

  map<Node,int> nodeToIndex;

  int currentNodeIndex=0;
  for (int z1=0;z1<(int)layerSizes.size();z1++)
  {
    for (int y1=0;y1<layerSizes[z1].y;y1++)
    {
      for (int x1=0;x1<layerSizes[z1].x;x1++)
      {
        Node curNode(x1,y1,z1);
        nodeToIndex[curNode] = currentNodeIndex;

        string name = (toString(x1)+string("/")+toString(y1)+string("/")+toString(z1));
        nameLookup[curNode] = name;

        bool update;
        if (layerIsInput[z1])
        {
          update=false; //base level nodes are constant
        }
        else
        {
          update=true;
        }

        if(currentNodeIndex>=numNodes)
        {
          throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
        }

        tmpNodes[currentNodeIndex] = NetworkNode(name,update);

        currentNodeIndex++;
      }
    }
  }

  int currentLinkIndex=0;
  for(
          it = allIncomingLinks.begin();
          it!=allIncomingLinks.end();
          it++
          )
  {
    vector<LinkWeightPair<NetworkDataType> > &incomingLinks = it->second;
    Node node = it->first;
    NetworkNode *toNode = &tmpNodes[ nodeToIndex[ node ] ];
    for(int b=0;b<(int)incomingLinks.size();b++)
    {
      NetworkNode *fromNode = &tmpNodes[ nodeToIndex[ incomingLinks[b].fromPos ] ];

      tmpLinks[currentLinkIndex] = NetworkLink(fromNode,toNode,true,incomingLinks[b].weight);
      currentLinkIndex++;
    }
  }

  if(currentNodeIndex!=numNodes || currentLinkIndex!=numLinks)
  {
    throw CREATE_LOCATEDEXCEPTION_INFO("OOPS!");
  }

  originalNetwork = NEAT::FastNetwork<NetworkDataType>(
      tmpNodes,
      numNodes,
      tmpLinks,
      numLinks
      );
#endif

  network = NEAT::FastLayeredNetwork<NetworkDataType>(layers);
#ifdef USE_GPU
  gpuNetwork = NEAT::GPUANN(layers);
#endif

  typename map<JGTL::Vector3<int>,  vector< LinkWeightPair<NetworkDataType> > >::iterator it;
  for(
          it = allIncomingLinks.begin();
          it!=allIncomingLinks.end();
          it++
          )
  {
    vector<LinkWeightPair<NetworkDataType> > &incomingLinks = it->second;
    Node toNode = it->first;

    for(int b=0;b<(int)incomingLinks.size();b++)
    {
      Node fromNode = incomingLinks[b].fromPos;
      NetworkDataType weight = incomingLinks[b].weight;

      network.setLink(fromNode,toNode,weight);
#ifdef USE_GPU
      gpuNetwork.setLink(fromNode,toNode,weight);
#endif
    }

  }

#if 0
  delete[] tmpNodes;
  delete[] tmpLinks;
#endif

#if 0
  for(float a=-1;a<=1;a+=0.1)
  {
    {
      int z1=0;
      for (int y1=0;y1<layerSizes[z1].y;y1++)
      {
        for (int x1=0;x1<layerSizes[z1].x;x1++)
        {
          network.setValue(Node(x1,y1,z1),0.1);
          gpuNetwork.setValue(Node(x1,y1,z1),0.1);
        }
      }
    }

    {
      boost::progress_timer t;
      for(int b=0;b<1000;b++)
      {
        network.update();
      }
    }

    {
      boost::progress_timer t;
      for(int b=0;b<1000;b++)
      {
        gpuNetwork.update();
      }
    }

    for (int z1=2;z1<(int)layerSizes.size();z1++)
    {
      for (int y1=0;y1<layerSizes[z1].y;y1++)
      {
        for (int x1=0;x1<layerSizes[z1].x;x1++)
        {
          Node curNode(x1,y1,z1);
          NetworkDataType newValue = network.getValue(curNode);
          NetworkDataType gpuValue = gpuNetwork.getValue(curNode);
          if( fabs(newValue-gpuValue)>1e-3 )
          {
            cout << "NETWORKS DO NOT MATCH: " << newValue << " " << gpuValue << endl;
            int FAILED=0;
          }
        }
      }
    }

    ::system("PAUSE");

  }

  int breakme=0;
  //cout << "CONNECTION COUNT: " << connectionCount << endl;
#endif
}

template< class NetworkDataType >
NetworkDataType LayeredSubstrate<NetworkDataType>::getValue(const Node &node)
{
#ifdef USE_GPU
  JGTL::Vector2<int> validInputStart = (layerSizes[node.z] - layerValidSizes[node.z])/2;
  return gpuNetwork.getValue( Node(node.x+validInputStart.x,node.y+validInputStart.y,node.z) );
#else
  JGTL::Vector2<int> validInputStart = (layerSizes[node.z] - layerValidSizes[node.z])/2;
  return network.getValue( Node(node.x+validInputStart.x,node.y+validInputStart.y,node.z) );
#endif
}

template< class NetworkDataType >
void LayeredSubstrate<NetworkDataType>::setValue(const Node &node,NetworkDataType _value)
{
#ifdef USE_GPU
  JGTL::Vector2<int> validInputStart = (layerSizes[node.z] - layerValidSizes[node.z])/2;
  return gpuNetwork.setValue( Node(node.x+validInputStart.x,node.y+validInputStart.y,node.z) ,_value);
#else
  JGTL::Vector2<int> validInputStart = (layerSizes[node.z] - layerValidSizes[node.z])/2;
  return network.setValue( Node(node.x+validInputStart.x,node.y+validInputStart.y,node.z) ,_value);
#endif
}

template< class NetworkDataType >
void LayeredSubstrate<NetworkDataType>::getWeightRGB(float &r,float &g,float &b,const Node &currentNode,const Node &sourceNode)
{
  //Use the link weight to determine the color
  NetworkDataType weight = network.getLink(sourceNode,currentNode);
		
  if(fabs(weight)<1e-3)
  {
    float outputVal = float(weight);
			
    //Cap weight
    outputVal = max(-3.0f,min(3.0f,outputVal));
			
    if(outputVal<0)
    {
      r = (fabs(outputVal));
      g = 0;
      b = 0;
    }
    else
    {
      r = (outputVal);
      g = (outputVal);
      b = (outputVal);
    }
  }
  else
  {
    //No link exists between the selected node and the current node
    r = g = b = 0;
  }
}
		
template< class NetworkDataType >
void LayeredSubstrate<NetworkDataType>::getActivationRGB(float &r,float &g,float &b,const Node &currentNode)
{
  //Use the node activation level
		
  float outputVal = float(network.getValue(currentNode));
		
  //Cap outputVal
  outputVal = max(-1.0f,min(1.0f,outputVal));
		
  if(outputVal<0)
  {
    r = (fabs(outputVal));
    g = 0;
    b = 0;
  }
  else
  {
    r = (outputVal);
    g = (outputVal);
    b = (outputVal);
  }
}
	
template< class NetworkDataType >
void LayeredSubstrate<NetworkDataType>::dumpWeightsFrom(string directoryname,Node sourceNode)
{
  directoryname +=
      string("/WeightsFrom_") + 
      replace_all_copy(toString(sourceNode)," ","_") + 
      string("/");
		
  boost::filesystem::create_directories(directoryname);
		
  for(int z=0;z<(int)getNumLayers();z++)
  {
    LibBoard::Board board;
			
    LibBoard::Board::Unit unit = (LibBoard::Board::Unit)1;
    float scale = 0.5;
			
#if SCALE_THIRD_LAYER
    if(z==2)
    {
      scale *= 8;
    }
#endif
			
    board.setUnit(scale,unit);
			
    float maxgreyval = -100;
    float mingreyval = 100;
			
    for(int y=0;y<getLayerSize(z).y;y++)
    {
      float offsetY = getLayerSize(z).y/-2.0f;
      for(int x=0;x<getLayerSize(z).x;x++)
      {
        float greyval = 0;
					
        if(z==0 && ((x+y)%2 == 1))
        {
          greyval=0;
        }
        else
        {
          Node currentNode(x,y,z);
						
          float offsetX = getLayerSize(z).x/-2.0f;
						
          float r,g,b;
						
          getWeightRGB(r,g,b,currentNode,sourceNode);
						
          bool negative=false;
          if(fabsf(g)<1e-6 && fabsf(r)>1e-6)
          {
            g = b = r;
            negative=true;
          }
						
          greyval = g;
        }
					
        maxgreyval = max(maxgreyval,greyval);
        mingreyval = min(mingreyval,greyval);
      }
    }
			
    if(fabsf(maxgreyval-mingreyval)<1e-3)
    {
      maxgreyval=1;
      mingreyval=0;
    }
			
    for(int y=0;y<getLayerSize(z).y;y++)
    {
      float offsetY = getLayerSize(z).y/-2.0f;
      for(int x=0;x<getLayerSize(z).x;x++)
      {
        float greyval;
					
        float offsetX = getLayerSize(z).x/-2.0f;
        bool negative=false;
					
        if(z==0 && ((x+y)%2 == 1))
        {
          greyval=0;
        }
        else
        {
          Node currentNode(x,y,z);
						
          float r,g,b;
						
          getWeightRGB(r,g,b,currentNode,sourceNode);
						
          if(fabsf(g)<1e-6 && fabsf(r)>1e-6)
          {
            g = b = r;
            negative=true;
          }
						
          //At this point, we know r=g=b
          greyval = r;
        }
					
        greyval -= mingreyval;
        greyval /= (maxgreyval-mingreyval);
        greyval *= 0.5;
					
        float tlx = x+offsetX;
        float tly = y+offsetY;
					
        board.setPenColorRGBf( greyval, greyval, greyval );
        board.setLineWidth( 0.0 );
        if(negative)
        {
          //vector<LibBoard::Point> points;
          //points.push_back(LibBoard::Point(tlx+0.5,tly));
          //points.push_back(LibBoard::Point(tlx,tly+0.5));
          //points.push_back(LibBoard::Point(tlx,tly+1.0));
          //points.push_back(LibBoard::Point(tlx+1.0,tly+1.0));
          //points.push_back(LibBoard::Point(tlx+1.0,tly));
          //points.push_back(LibBoard::Point(tlx+0.5,tly));
          //board.fillPolyline(points);
						
          board.fillRectangle(tlx+0.5f , tly, 0.5f, 1.0f );
          board.fillRectangle(tlx , tly-0.5f, 0.5f, 0.5f );
          board.fillTriangle(tlx+0.5f,tly, tlx+0.5f,tly+0.5f-1.0f,  tlx,tly+0.5f-1.0f );
        }
        else
        {
          board.fillRectangle(tlx , tly, 1.0, 1.0 );
        }
      }
    }
			
    if(z==sourceNode.z)
    {
      float offsetY = getLayerSize(z).y/-2.0f;
      float offsetX = getLayerSize(z).x/-2.0f;
      board.setPenColorRGBf( 0.0, 0.0, 0.0 );
      board.setLineWidth( 80.0 );
      board.drawCircle( sourceNode.x+offsetX + 0.5f, (sourceNode.y-1)+offsetY + 0.5f, 0.75f );
      board.setPenColorRGBf( 1.0, 1.0, 0.0 );
      board.setLineWidth( 60.0 );
      board.drawCircle( sourceNode.x+offsetX + 0.5f, (sourceNode.y-1)+offsetY + 0.5f, 0.75f );
    }
			
    board.saveEPS( 
        (
            directoryname + 
            string("Layer") + toString(z) + string(".eps")).c_str() 
        );
    board.saveFIG( 
        (
            directoryname + 
            string("Layer") + toString(z) + string(".fig")).c_str() 
        );
    board.saveSVG( 
        (
            directoryname + 
            string("Layer") + toString(z) + string(".svg")).c_str() 
        );
  }
}

template< class NetworkDataType >
void LayeredSubstrate<NetworkDataType>::dumpActivationLevels(string directoryname)
{
  int imageIndex=0;
  while(boost::filesystem::exists(
            directoryname +
            string("/Activation_") + toString(imageIndex,3) +
            string("/layer0.svg")
            ))
  {
    imageIndex++;
  }
			
  directoryname +=
      string("/Activation_") + toString(imageIndex,3) + string("/");
		
  boost::filesystem::create_directories(directoryname);
		
  for(int z=0;z<(int)getNumLayers();z++)
  {
    LibBoard::Board board;
			
    LibBoard::Board::Unit unit = (LibBoard::Board::Unit)1;
    float scale = 0.5;
			
#if SCALE_THIRD_LAYER
    if(z==2)
    {
      scale *= 8;
    }
#endif
			
    board.setUnit(scale,unit);
			
    float maxgreyval = -100;
    float mingreyval = 100;
			
    for(int y=0;y<getLayerSize(z).y;y++)
    {
      float offsetY = getLayerSize(z).y/-2.0f;
      for(int x=0;x<getLayerSize(z).x;x++)
      {
        float greyval = 0;
					
        if(z==0 && ((x+y)%2 == 1))
        {
          greyval=0;
        }
        else
        {
          Node currentNode(x,y,z);
						
          float offsetX = getLayerSize(z).x/-2.0f;
						
          float r,g,b;
						
          getActivationRGB(r,g,b,currentNode);
						
          bool negative=false;
          if(fabsf(g)<1e-6 && fabsf(r)>1e-6)
          {
            g = b = r;
            negative=true;
          }
						
          greyval = g;
        }
					
        maxgreyval = max(maxgreyval,greyval);
        mingreyval = min(mingreyval,greyval);
      }
    }
			
    if(fabsf(maxgreyval-mingreyval)<1e-3)
    {
      maxgreyval=1;
      mingreyval=0;
    }
			
    for(int y=0;y<getLayerSize(z).y;y++)
    {
      float offsetY = getLayerSize(z).y/-2.0f;
      for(int x=0;x<getLayerSize(z).x;x++)
      {
        float greyval;
					
        float offsetX = getLayerSize(z).x/-2.0f;
        bool negative=false;
					
        if(z==0 && ((x+y)%2 == 1))
        {
          greyval=0;
        }
        else
        {
          Node currentNode(x,y,z);
						
          float r,g,b;
						
          getActivationRGB(r,g,b,currentNode);
						
          if(fabsf(g)<1e-6 && fabsf(r)>1e-6)
          {
            g = b = r;
            negative=true;
          }
						
          //At this point, we know r=g=b
          greyval = r;
        }
					
        greyval -= mingreyval;
        greyval /= (maxgreyval-mingreyval);
        greyval *= 0.5;
					
        float tlx = x+offsetX;
        float tly = y+offsetY;
					
        board.setPenColorRGBf( greyval, greyval, greyval );
        board.setLineWidth( 0.0 );
        if(negative)
        {
          //vector<LibBoard::Point> points;
          //points.push_back(LibBoard::Point(tlx+0.5,tly));
          //points.push_back(LibBoard::Point(tlx,tly+0.5));
          //points.push_back(LibBoard::Point(tlx,tly+1.0));
          //points.push_back(LibBoard::Point(tlx+1.0,tly+1.0));
          //points.push_back(LibBoard::Point(tlx+1.0,tly));
          //points.push_back(LibBoard::Point(tlx+0.5,tly));
          //board.fillPolyline(points);
						
          board.fillRectangle(tlx+0.5f , tly, 0.5f, 1.0f );
          board.fillRectangle(tlx , tly-0.5f, 0.5f, 0.5f );
          board.fillTriangle(tlx+0.5f,tly, tlx+0.5f,tly+0.5f-1.0f,  tlx,tly+0.5f-1.0f );
        }
        else
        {
          board.fillRectangle(tlx , tly, 1.0f, 1.0f );
        }
      }
    }
			
    board.saveEPS( 
        (
            directoryname + 
            string("Layer") + toString(z) + string(".eps")).c_str() 
        );
    board.saveFIG( 
        (
            directoryname + 
            string("Layer") + toString(z) + string(".fig")).c_str() 
        );
    board.saveSVG( 
        (
            directoryname + 
            string("Layer") + toString(z) + string(".svg")).c_str() 
        );
  }
}

template class LayeredSubstrate<float>; // explicit instantiation
//template class LayeredSubstrate<double>; // explicit instantiation
}
