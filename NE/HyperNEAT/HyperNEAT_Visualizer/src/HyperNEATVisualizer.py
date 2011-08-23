from Defines import *

from SubstrateRenderer import *

#populationFileName = "C:/Programming/NE/HyperNEAT/out/Results/GoNoScaling1000Gens/testGoNoScaling_T2610_Euler_Run$RUN_NUMBER$.xm_best.xml.gz"

#populationFileName = "C:/Programming/NE/HyperNEAT/out/Results/GoNoScaling_T2718/testGoNoScaling_T2718_Hilbert_Run$RUN_NUMBER$.xml.backup.xml.gz"
populationFileName = "../../out/Results/GoScalingBasic_T2718/GoScalingBasic_T2718_Euler_Run$RUN_NUMBER$.xml.backup.xml.gz"

outputDirName = "../../out/images"
#outputDirName = "/Users/pawn/Programming/NE/HyperNEAT/out/images"

CAMERA_SPEED = 5

class HyperNEATVisualizer(object):
    def __init__(self):
        # Number of the glut window.
        self.window = 0
        self.substrateRenderer = None
        self.eyePos = (0,15,-15)
        self.lookAtPos = (0,0,0)
        self.distance = sqrt(15*15 + 15*15)
        self.upDirection = (0,1,0)
        self.translateVelocity = (0,0,0)
        self.lookdownAngle = 45.0*pi/180.0
        self.turnAngle = pi
        self.lookdownVelocity = 0
        self.turnVelocity = 0
        self.distanceVelocity = 0
        self.width = 800
        self.height = 600
        self.mousePos = (0,0)
        self.currentGeneration = 0
        self.currentIndividual = 0
        self.currentRun = 1
    
    # A general OpenGL initialization function.  Sets all of the initial parameters. 
    def InitGL(self,Width, Height):                # We call this right after our OpenGL window is created.
        glEnable(GL_TEXTURE_2D)
        glClearColor(0.0, 0.0, 0.0, 0.0)    # This Will Clear The Background Color To Black
        glClearDepth(1.0)                    # Enables Clearing Of The Depth Buffer
        glDepthFunc(GL_LESS)                # The Type Of Depth Test To Do
        glEnable(GL_DEPTH_TEST)                # Enables Depth Testing
        glShadeModel(GL_SMOOTH)                # Enables Smooth Color Shading
        glEnable (GL_LINE_SMOOTH)
        
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()                    # Reset The Projection Matrix
                                            # Calculate The Aspect Ratio Of The Window
        gluPerspective(45.0, float(Width)/float(Height), 0.1, 100.0)
    
        glMatrixMode(GL_MODELVIEW)
    
    # The function called when our window is resized (which shouldn't happen if you enable fullscreen, below)
    def ReSizeGLScene(self,Width, Height):
        if Height == 0:                        # Prevent A Divide By Zero If The Window Is Too Small 
            Height = 1
    
        self.width = Width
        self.height = Height
        
        glViewport(0, 0, Width, Height)        # Reset The Current Viewport And Perspective Transformation
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        gluPerspective(45.0, float(Width)/float(Height), 0.1, 100.0)
        #glRotatef(90,1.0,0,0)    
        #glTranslatef(0, -15.0, 0)

        self.toCameraVector = Vector3(self.distance,0,0)
        
        #Adjust for lookdown angle
        self.toCameraVector.x = self.distance*cos(self.lookdownAngle)
        self.toCameraVector.y = self.distance*sin(self.lookdownAngle)
        
        #Now adjust for heading
        self.toCameraVector.z = self.toCameraVector.x*cos(self.turnAngle)
        self.toCameraVector.x = self.toCameraVector.y*sin(self.turnAngle)
        
        gluLookAt(
                  self.lookAtPos[0]+self.toCameraVector.x, self.lookAtPos[1]+self.toCameraVector.y, self.lookAtPos[2]+self.toCameraVector.z, 
                  self.lookAtPos[0], self.lookAtPos[1], self.lookAtPos[2], 
                  self.upDirection[0], self.upDirection[1], self.upDirection[2]
                  )
            
        glMatrixMode(GL_MODELVIEW)
        
    def renderString(self,loc,string):
        glRasterPos2f(loc[0],loc[1])        
        for character in string:
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15,ord(character))        
    
    # The main drawing function. 
    def DrawGLScene(self):
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        gluPerspective(45.0, float(self.width)/float(self.height), 0.1, 100.0)
        #glRotatef(90,1.0,0,0)    
        #glTranslatef(0, -15.0, 0)

        self.toCameraVector = Vector3(self.distance,0,0)
        
        #Adjust for lookdown angle
        self.toCameraVector.x = self.distance*cos(self.lookdownAngle)
        self.toCameraVector.y = self.distance*sin(self.lookdownAngle)
        
        #Now adjust for heading
        self.toCameraVector.z = self.toCameraVector.x*cos(self.turnAngle)
        self.toCameraVector.x = self.toCameraVector.x*sin(self.turnAngle)
        
        gluLookAt(
                  self.lookAtPos[0]+self.toCameraVector.x, self.lookAtPos[1]+self.toCameraVector.y, self.lookAtPos[2]+self.toCameraVector.z, 
                  self.lookAtPos[0], self.lookAtPos[1], self.lookAtPos[2], 
                  self.upDirection[0], self.upDirection[1], self.upDirection[2]
                  )
            
        glMatrixMode(GL_MODELVIEW)
        
        glClearColor(0.0,0.5,0.75,1.0)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)    # Clear The Screen And The Depth Buffer
        glLoadIdentity()                    # Reset The View
        
        self.substrateRenderer.update()
        self.substrateRenderer.render()
    
        #glTranslatef(0.0,0.0,-5.0)            # Move Into The Screen
    
        glMatrixMode(GL_MODELVIEW)
        glPushMatrix()
        glLoadIdentity()
        
        glMatrixMode(GL_PROJECTION)
        glPushMatrix()
        glLoadIdentity()
        glOrtho( 0, 640, 480, 0, -2000, 2000 )

        glDisable(GL_DEPTH_TEST)
        
        glColor4f(1,1,1,1)
        
        self.renderString((60,400),'Run:        '+str(self.currentRun))        
        self.renderString((60,430),'Generation: '+str(self.currentGeneration+1)+'/'+str(self.population.getGenerationCount()))        
        self.renderString((60,460),'Individual: '+str(self.currentIndividual+1)+'/'+str(self.population.getIndividualCount(self.currentGeneration)))
        
        glEnable(GL_DEPTH_TEST)
        
        glMatrixMode(GL_MODELVIEW)
        glPopMatrix()
    
        glMatrixMode(GL_PROJECTION)
        glPopMatrix()

        #  since this is double buffered, swap the buffers to display what just got drawn. 
        glutSwapBuffers()
        
    def mouseMoved(self,x,y):
        self.mousePos = (x,y)
        
    def mouseChanged(self,button,state,x,y):
        if state:
            self.mousePressed(button, x, y)
        else:
            self.mouseReleased(button, x, y)
        
    def mousePressed(self,button,x,y):
        pass
    
    def mouseReleased(self,button,x,y):
        print 'Clicked on pixel',(x,y)
        for rectNodePair in self.substrateRenderer.nodeScreenRects:
            rect,node = rectNodePair[0],rectNodePair[1]
            if rect[0][0]<=x and rect[1][0]>=x and rect[0][1]<=y and rect[1][1]>=y:
                if node[2]>0:
                    #Don't do anything if the user didn't click on an input node
                    break
                
                print 'Clicked on node',node
                print self.substrateRenderer.hardcodedInputs.get(node,0.0),'->',
                print button
                if button == GLUT_LEFT_BUTTON:
                    modifier = 0.5
                elif button == GLUT_RIGHT_BUTTON:
                    modifier = -0.5
                else:
                    modifier = 0.0
                self.substrateRenderer.hardcodedInputs[node] = \
                    self.substrateRenderer.hardcodedInputs.get(node,0.0) + modifier 
                print self.substrateRenderer.hardcodedInputs.get(node,0.0)
                self.substrateRenderer.networkDirty = True
                break
        pass
    
    # The function called whenever a key is pressed. Note the use of Python tuples to pass in: (key, x, y)  
    def keyPressed(self,*args):
        if args[0] == '\033': #Escape key
            pass
        elif args[0]==GLUT_KEY_UP:
            self.lookdownVelocity += CAMERA_SPEED
        elif args[0]==GLUT_KEY_DOWN:
            self.lookdownVelocity -= CAMERA_SPEED
        elif args[0]==GLUT_KEY_LEFT:
            self.turnVelocity -= CAMERA_SPEED
        elif args[0]==GLUT_KEY_RIGHT:
            self.turnVelocity += CAMERA_SPEED
        elif args[0]=='q':
            self.distanceVelocity -= CAMERA_SPEED*0.25
        elif args[0]=='e':
            self.distanceVelocity += CAMERA_SPEED*0.25
        else:
            print args

    # The function called whenever a key is pressed. Note the use of Python tuples to pass in: (key, x, y)  
    def keyReleased(self,*args):
        print args
        # If escape is pressed, kill everything.
        if args[0] == '\033':
            glutDestroyWindow(self.window)
            sys.exit()
        elif args[0]==GLUT_KEY_UP:
            self.lookdownVelocity -= CAMERA_SPEED
        elif args[0]==GLUT_KEY_DOWN:
            self.lookdownVelocity += CAMERA_SPEED
        elif args[0]==GLUT_KEY_LEFT:
            self.turnVelocity += CAMERA_SPEED
        elif args[0]==GLUT_KEY_RIGHT:
            self.turnVelocity -= CAMERA_SPEED
        elif args[0]=='q':
            self.distanceVelocity += CAMERA_SPEED*0.25
        elif args[0]=='e':
            self.distanceVelocity -= CAMERA_SPEED*0.25
        elif args[0]=='a':
            self.substrate.dumpWeightsFrom(
			    outputDirName,
			    NEAT_Vector3(
				    self.substrateRenderer.nodeSelected[0],
				    self.substrateRenderer.nodeSelected[1],
				    self.substrateRenderer.nodeSelected[2]
				    )
			    )
            self.substrate.dumpActivationLevels(outputDirName)
        elif args[0]=='=':
            self.currentRun += 1
            self.loadPopulation()
            self.loadIndividual()
        elif args[0]=='-':
            self.currentRun = max(0,self.currentRun-1)
            self.loadPopulation()
            self.loadIndividual()
        elif args[0]=='[' or args[0]=='{':
            if (glutGetModifiers()&GLUT_ACTIVE_SHIFT)==0:
                self.currentGeneration = max(0,self.currentGeneration-1)
                self.loadIndividual()
            else:
                self.currentGeneration = max(0,self.currentGeneration-10)
                self.loadIndividual()
        elif args[0]==']' or args[0]=='}':
            if (glutGetModifiers()&GLUT_ACTIVE_SHIFT)==0:
                self.currentGeneration = min(self.population.getGenerationCount()-1,self.currentGeneration+1)
                self.loadIndividual()
            else:
                self.currentGeneration = min(self.population.getGenerationCount()-1,self.currentGeneration+10)
                self.loadIndividual()
        elif args[0]==',' or args[0]=='<':
            if (glutGetModifiers()&GLUT_ACTIVE_SHIFT)==0:
                self.currentIndividual = max(0,self.currentIndividual-1)
                self.loadIndividual()
            else:
                self.currentIndividual = max(0,self.currentIndividual-10)
                self.loadIndividual()
        elif args[0]=='.' or args[0]=='>':
            if (glutGetModifiers()&GLUT_ACTIVE_SHIFT)==0:
                self.currentIndividual = min(self.population.getIndividualCount(self.currentGeneration)-1,self.currentIndividual+1)
                self.loadIndividual()
            else:
                self.currentIndividual = min(self.population.getIndividualCount(self.currentGeneration)-1,self.currentIndividual+10)
                self.loadIndividual()
        else:
            print args
            
    def loadPopulation(self):
        while True:
            try:
                self.population = loadFromPopulation(populationFileName.replace('$RUN_NUMBER$',('%d' % self.currentRun)))
                break
            except:
                self.currentRun += 1
                if self.currentRun>=1000:
                    raise Exception("Error loading XML file(s)")
    
    def loadIndividual(self):
        if self.substrateRenderer is not None:
            hardcodedInputs = self.substrateRenderer.hardcodedInputs
        else:
            hardcodedInputs = {}
            
            experimentType = getExperimentType()
            
            if experimentType == 15 \
            or experimentType == 16 \
            or experimentType == 21 \
            or experimentType == 24:
                for x in xrange(0,8,2):
                    for y in xrange(0,3):
                        hardcodedInputs[(x+y%2,y,0)] = 0.5
                for x in xrange(0,8,2):
                    for y in xrange(5,8):
                        hardcodedInputs[(x+y%2,y,0)] = -0.5
                    
        print 'Generation:',self.currentGeneration
        self.substrate.populateSubstrate(self.population.getIndividual(self.currentIndividual,self.currentGeneration))
        print 'CREATING SUBSTRATE RENDERER'
        self.substrateRenderer = SubstrateRenderer(self.substrate,hardcodedInputs)
            
    def initNEAT(self):
        print "INIT NEAT1"
        self.loadPopulation()
        
        print "INIT NEAT2"
        self.substrate = LayeredSubstrate()
        
        print "INIT NEAT3"
        layerSizes = [(8,8),(8,8),(1,1)]
        layerAdjacencyList = [(0,1),(1,2)]
        layerIsInput = [True,False,False]
        layerLocations = [(0,0,0),(0,4,0),(0,8,0)]
        normalize = False
        useOldOutputNames = True
        
        self.substrate.setLayerInfoFromCurrentExperiment()
        print "INIT NEAT4"
        self.loadIndividual()
        print "INIT NEAT5"
        
        
    def update(self,value):
        #print 'updating'
        self.lookdownAngle += (self.lookdownVelocity*10.0/1000.0)
        self.lookdownAngle = min(pi/2,max(0,self.lookdownAngle))
        
        self.turnAngle += (self.turnVelocity*10.0/1000.0)
        while self.turnAngle>pi:
            self.turnAngle -= 2*pi
        while self.turnAngle<-pi:
            self.turnAngle += 2*pi
            
        self.distance += self.distanceVelocity
        self.distance = max(1,self.distance) 
        #print self.lookdownAngle

        #print self.mousePos
        self.substrateRenderer.nodeSelected = (-1,-1,-1)
        for rectNodePair in self.substrateRenderer.nodeScreenRects:
            rect,node = rectNodePair[0],rectNodePair[1]
            if rect[0][0]<=self.mousePos[0] and rect[1][0]>=self.mousePos[0] \
            and rect[0][1]<=self.mousePos[1] and rect[1][1]>=self.mousePos[1]:
                self.substrateRenderer.nodeSelected = node
                break
            
        glutTimerFunc(10,self.update,0)
        
    def main(self):
        self.initNEAT()
        
        # For now we just pass glutInit one empty argument. I wasn't sure what should or could be passed in (tuple, list, ...)
        # Once I find out the right stuff based on reading the PyOpenGL source, I'll address this.
        glutInit(sys.argv)
        
        # Select type of Display mode:   
        #  Double buffer 
        #  RGBA color
        # Alpha components supported 
        # Depth buffer
        glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH)
        
        # get a 640 x 480 window 
        glutInitWindowSize(640, 480)
        
        # the window starts at the upper left corner of the screen 
        glutInitWindowPosition(0, 0)
        
        # Okay, like the C version we retain the window id to use when closing, but for those of you new
        # to Python (like myself), remember this assignment would make the variable local and not global
        # if it weren't for the global declaration at the start of main.
        self.window = glutCreateWindow("HyperNEAT Substrate Visualizer")
    
        glutIgnoreKeyRepeat(1)
        
        # Register the drawing function with glut, BUT in Python land, at least using PyOpenGL, we need to
        # set the function pointer and invoke a function to actually register the callback, otherwise it
        # would be very much like the C version of the code.    
        glutDisplayFunc(self.DrawGLScene)
        
        # Uncomment this line to get full screen.
        # glutFullScreen()
    
        # When we are doing nothing, redraw the scene.
        glutIdleFunc(self.DrawGLScene)
        
        # Register the function called when our window is resized.
        glutReshapeFunc(self.ReSizeGLScene)
        
        # Register the function called when the keyboard is pressed.  
        glutKeyboardFunc(self.keyPressed)
        glutSpecialFunc(self.keyPressed)
        # Register the function called when the keyboard is released.  
        glutKeyboardUpFunc(self.keyReleased)
        glutSpecialUpFunc(self.keyReleased)
        
        glutMouseFunc(self.mouseChanged)
        glutMotionFunc(self.mouseMoved)
        glutPassiveMotionFunc(self.mouseMoved)
    
        # Initialize our window. 
        self.InitGL(640, 480)
        
        glutTimerFunc(10,self.update,0)
    
        # Start Event Processing Engine    
        glutMainLoop()

if __name__ == "__main__":
    initializeHyperNEAT()
    print(os.getcwd())
    visualizer = HyperNEATVisualizer()
    visualizer.main()
    del visualizer
    cleanupHyperNEAT()
    sys.exit(0)

    
