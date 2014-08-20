from Defines import *

class SubstrateRenderer(object):
    
    SIZE = 2.5;

    LINK_THICKNESS = 0.05;
    
    def __init__(self,layeredSubstrate,hardcodedInputs={}):
        self.layeredSubstrate = layeredSubstrate
        self.hardcodedInputs = hardcodedInputs
        self.nodeSelected = (-1,-1,-1)
        self.networkDirty = True
        self.nodeScreenRects = []
        
        for inputNode,inputValue in self.hardcodedInputs.iteritems():
            self.layeredSubstrate.setValue(
                                           tupleToVector3Int(inputNode),
                                           inputValue
                                           )

    def update(self):
        if self.networkDirty:

            network = self.layeredSubstrate.getNetwork();

            network.reinitialize()

            for inputNode,inputValue in self.hardcodedInputs.iteritems():
                self.layeredSubstrate.setValue(
                                               tupleToVector3Int(inputNode),
                                               inputValue
                                               )
                
            network.update()
            self.networkDirty=False;

    def render(self):
        #print "REFRESHING SUBSTRATE"
        del self.nodeScreenRects[:]

        experimentType = getExperimentType()
        
        for z in xrange(0,self.layeredSubstrate.getNumLayers()):
            glTranslatef(
                          self.layeredSubstrate.getLayerLocation(z)[0],
                          self.layeredSubstrate.getLayerLocation(z)[1],
                          self.layeredSubstrate.getLayerLocation(z)[2]
                          )
            glBegin(GL_QUADS)
            for y in xrange(0,self.layeredSubstrate.getLayerSize(z)[1]):
                offsetY = self.layeredSubstrate.getLayerSize(z)[1]/-2.0 + self.layeredSubstrate.getLayerLocation(z)[2];
                for x in xrange(0,self.layeredSubstrate.getLayerSize(z)[0]):
                    
                    if experimentType == 15 \
                    or experimentType == 16 \
                    or experimentType == 21 \
                    or experimentType == 24:
                        if z==0 and (x+y)%2 == 1:
                            continue
                    
                    currentNode = (x,y,z)

                    offsetX = self.layeredSubstrate.getLayerSize(z)[0]/-2.0 + self.layeredSubstrate.getLayerLocation(z)[0]

                    color = self.getRGB(currentNode,self.nodeSelected);

                    drawPos = Vector2(x+offsetX,y+offsetY)
                    
                    
                    glColor3f(0,0,0)
                    posx,posy,posz = drawPos.x-0.1,-0.05,drawPos.y-0.1
                    #rectTopLeft = rectBottomRight = gluProject(x,y,z)                    
                    glVertex3f(posx,posy,posz);
                    glVertex3f((drawPos.x+1)+0.1,-0.05,drawPos.y-0.1);
                    glVertex3f((drawPos.x+1)+0.1,-0.05,(drawPos.y+1)+0.1);
                    glVertex3f(drawPos.x-0.1,-0.05,(drawPos.y+1)+0.1);
                    
                    glColor3f(color[0],color[1],color[2])
                    glVertex3f(drawPos.x,0,drawPos.y);
                    glVertex3f((drawPos.x+1),0,drawPos.y);
                    glVertex3f((drawPos.x+1),0,(drawPos.y+1));
                    glVertex3f(drawPos.x,0,(drawPos.y+1));
            glEnd()
            glTranslatef(
                          -self.layeredSubstrate.getLayerLocation(z)[0],
                          -self.layeredSubstrate.getLayerLocation(z)[1],
                          -self.layeredSubstrate.getLayerLocation(z)[2]
                          )
            
        windowHeight = glutGet(GLUT_WINDOW_HEIGHT)
        for z in xrange(0,self.layeredSubstrate.getNumLayers()):
            glTranslatef(
                          self.layeredSubstrate.getLayerLocation(z)[0],
                          self.layeredSubstrate.getLayerLocation(z)[1],
                          self.layeredSubstrate.getLayerLocation(z)[2]
                          )
            for y in xrange(0,self.layeredSubstrate.getLayerSize(z)[1]):
                offsetY = self.layeredSubstrate.getLayerSize(z)[1]/-2.0 + self.layeredSubstrate.getLayerLocation(z)[2];
                for x in xrange(0,self.layeredSubstrate.getLayerSize(z)[0]):
                    #if z==0 and (x+y)%2 == 1:
                        #continue
                    
                    currentNode = (x,y,z)

                    offsetX = self.layeredSubstrate.getLayerSize(z)[0]/-2.0 + self.layeredSubstrate.getLayerLocation(z)[0]

                    color = self.getRGB(currentNode,self.nodeSelected);

                    drawPos = Vector2(x+offsetX,y+offsetY)
                    
                    #For some reason the y axis is flipped, so we have to reverse it
                    posx,posy,posz = drawPos.x,0,drawPos.y
                    rectTopLeft = rectBottomRight = gluProject(posx,posy,posz)
                    rectTopLeft = (rectTopLeft[0],(windowHeight-1)-rectTopLeft[1])
                    rectBottomRight = (rectBottomRight[0],(windowHeight-1)-rectBottomRight[1])
                    posx,posy,posz = (drawPos.x+1),0,drawPos.y
                    screenPos = gluProject(posx,posy,posz)
                    screenPos = (screenPos[0],(windowHeight-1)-screenPos[1])
                    rectTopLeft = (min(rectTopLeft[0],screenPos[0]),min(rectTopLeft[1],screenPos[1]))
                    rectBottomRight = (max(rectBottomRight[0],screenPos[0]),max(rectBottomRight[1],screenPos[1]))
                    posx,posy,posz = (drawPos.x+1),0,(drawPos.y+1)
                    screenPos = gluProject(posx,posy,posz)
                    screenPos = (screenPos[0],(windowHeight-1)-screenPos[1])
                    rectTopLeft = (min(rectTopLeft[0],screenPos[0]),min(rectTopLeft[1],screenPos[1]))
                    rectBottomRight = (max(rectBottomRight[0],screenPos[0]),max(rectBottomRight[1],screenPos[1]))
                    posx,posy,posz = drawPos.x,0,(drawPos.y+1)
                    screenPos = gluProject(posx,posy,posz)
                    screenPos = (screenPos[0],(windowHeight-1)-screenPos[1])
                    rectTopLeft = (min(rectTopLeft[0],screenPos[0]),min(rectTopLeft[1],screenPos[1]))
                    rectBottomRight = (max(rectBottomRight[0],screenPos[0]),max(rectBottomRight[1],screenPos[1]))
                    
                    self.nodeScreenRects.append(((rectTopLeft,rectBottomRight),currentNode))
            glTranslatef(
                          -self.layeredSubstrate.getLayerLocation(z)[0],
                          -self.layeredSubstrate.getLayerLocation(z)[1],
                          -self.layeredSubstrate.getLayerLocation(z)[2]
                          )
        self.nodeScreenRects.reverse()


    def getRGB(self,currentNode,sourceNode):
        if sourceNode[0]>=0:
            sourceNodeName = self.layeredSubstrate.getNodeName(sourceNode)
            currentNodeName = self.layeredSubstrate.getNodeName(currentNode)
            
            #Use the link weight to determine the color
            if self.layeredSubstrate.getNetwork().hasLink(
                sourceNodeName,
                currentNodeName
                ):
                linkWeight = \
                    self.layeredSubstrate.getNetwork().getLinkWeight(
                    sourceNodeName,
                    currentNodeName
                    )
            elif self.layeredSubstrate.getNetwork().hasLink(
                currentNodeName,
                sourceNodeName
                ):
                linkWeight = \
                    self.layeredSubstrate.getNetwork().getLinkWeight(
                    currentNodeName,
                    sourceNodeName
                    )
            else:
                linkWeight = 0
            
            outputVal = linkWeight
            
            #Cap weight
            outputVal = max(-3.0,min(3.0,outputVal))
            
        else: #Use the activation level
            
            outputVal = \
                (self.layeredSubstrate.getNetwork().getValue(self.layeredSubstrate.getNodeName(currentNode)))
            
            #Cap activation level
            outputVal = max(-1.0,min(1.0,outputVal))
            
        if outputVal<0:
            r = abs(outputVal)
            g = 0.0
            b = 0.0
        else:
            r = outputVal
            g = outputVal
            b = outputVal
        
        #print r,g,b
        return (r,g,b)
         

