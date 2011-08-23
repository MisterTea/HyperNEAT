#include "HCUBE_Defines.h"

#include "Experiments/HCUBE_RobotArmExperiment.h"

#define ROBOT_ARM_EXPERIMENT_DEBUG (0)

namespace HCUBE
{
    using namespace NEAT;

    RobotArmExperiment::RobotArmExperiment(string _experimentName,int _threadID)
            :
            Experiment(_experimentName,_threadID),
            userx(1),
            usery(1),
            scale(4)
    {}

    NEAT::GeneticPopulation* RobotArmExperiment::createInitialPopulation(int populationSize)
    {
        NEAT::GeneticPopulation* population = new NEAT::GeneticPopulation();

        vector<GeneticNodeGene> genes;

        genes.push_back(GeneticNodeGene("Bias","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("Angle1","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("Angle2","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("ArmAngle1","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));
        genes.push_back(GeneticNodeGene("ArmAngle2","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));

        for (int a=0;a<populationSize;a++)
        {
            shared_ptr<GeneticIndividual> individual(new GeneticIndividual(genes,true,1.0));

            for (int b=0;b<0;b++)
            {
                individual->testMutate();
            }

            population->addIndividual(individual);
        }

        return population;
    }

    void RobotArmExperiment::processGroup(shared_ptr<NEAT::GeneticGeneration> generation)
    {
        shared_ptr<NEAT::GeneticIndividual> individual = group.front();
        /*{
            cout << "Starting Evaluation on object:" << this << endl;
            cout << "Running on individual " << individual << endl;
        }*/

        //You get 10 points just for entering the game, wahooo!
        individual->setFitness(10);

        NEAT::FastNetwork<double> network = individual->spawnFastPhenotypeStack<double>();

        int counter=0;

        for (int intx=-20;intx<=20;intx++)
        {
            for (int inty=0;inty<=20;inty++)
            {
                network.reinitialize();

                double realx = intx/1.0;
                double realy = inty/1.0;

                double leftEyeAngle = (atan2(realy,realx+5)-M_PI/2.0);
                double rightEyeAngle = (atan2(realy,realx-5)-M_PI/2.0);

                if (leftEyeAngle<-M_PI)
                {
                    leftEyeAngle += 2*M_PI;
                }

                if (rightEyeAngle<-M_PI)
                {
                    rightEyeAngle += 2*M_PI;
                }

                network.setValue("Angle1",leftEyeAngle);
                network.setValue("Angle2",rightEyeAngle);
                network.setValue("Bias",0.3);
                network.update();

                double armAngle1 = M_PI*network.getValue("ArmAngle1");
                double armAngle2 = M_PI*network.getValue("ArmAngle2");

                double x=0.0,y=0.0;

                {
                    armAngle1+=M_PI/2.0;

                    if (armAngle1>M_PI)
                    {
                        armAngle1 -= 2.0*M_PI;
                    }

#if ROBOT_ARM_EXPERIMENT_DEBUG
                    cout << "ANGLES: " << armAngle1 << ' ' << armAngle2 << endl;
#endif

                    x += 10*cos(armAngle1);
                    y += 10*sin(armAngle1);

#if ROBOT_ARM_EXPERIMENT_DEBUG
                    cout << "ELBOW POSITION: " << x << "," << y << endl;
#endif

                    x += 10*cos(armAngle2+armAngle1);
                    y += 10*sin(armAngle2+armAngle1);

#if ROBOT_ARM_EXPERIMENT_DEBUG
                    cout << "HAND POSITION: " << x << "," << y << endl;
                    cout << "EXPECTED POSITION: " << realx << ',' << realy << endl;
#endif

                    double distanceSquared = (((x-realx)*(x-realx))+((y-realy)*(y-realy)));

#if ROBOT_ARM_EXPERIMENT_DEBUG
                    cout << distanceSquared << " " << max(0.0,(100-distanceSquared)) << endl;
                    CREATE_PAUSE(string("Error!: ")+toString(__LINE__));
#endif

                    individual->reward(max(0.0,(100-(distanceSquared))));
                }
            }
        }

    }

#ifndef HCUBE_NOGUI
    bool RobotArmExperiment::handleMousePress(wxMouseEvent& event,wxSize &bitmapSize)
    {
        wxPoint clickPoint = event.GetPosition();
        cout << "handling mouse press at " << clickPoint.x << ' ' << clickPoint.y << endl;
        userx = (clickPoint.x-bitmapSize.x/2)/scale;
        usery = max(0.0,(bitmapSize.y/2-clickPoint.y)/scale);
        cout << "User point set to: " << userx << ' ' << usery << endl;
        return true;
    }

    void RobotArmExperiment::createIndividualImage(wxDC &drawContext,shared_ptr<NEAT::GeneticIndividual> individual)
    {
        drawContext.Clear();

        NEAT::FastNetwork<double> network = individual->spawnFastPhenotypeStack<double>();

        int counter=0;

        int halfSizeX = drawContext.GetSize().x/2;
        int halfSizeY = drawContext.GetSize().y/2;

        double realx = userx;
        double realy = usery;

        drawContext.SetPen(wxPen(*wxBLUE,2,wxSOLID));

        drawContext.DrawLine(
            int(halfSizeX+5*scale),
            halfSizeY,
            int(halfSizeX+3*scale),
            int(halfSizeY-2*scale)
        );

        drawContext.DrawLine(
            int(halfSizeX+5*scale),
            halfSizeY,
            int(halfSizeX+7*scale),
            int(halfSizeY-2*scale)
        );

        drawContext.DrawLine(
            int(halfSizeX-5*scale),
            halfSizeY,
            int(halfSizeX-3*scale),
            int(halfSizeY-2*scale)
        );

        drawContext.DrawLine(
            int(halfSizeX-5*scale),
            halfSizeY,
            int(halfSizeX-7*scale),
            int(halfSizeY-2*scale)
        );

        drawContext.SetPen(wxPen(*wxGREEN,2,wxSOLID));

        drawContext.DrawLine(
            int(halfSizeX+5*scale),
            halfSizeY,
            int(halfSizeX+realx*scale),
            int(halfSizeY-realy*scale)
        );

        drawContext.DrawLine(
            int(halfSizeX-5*scale),
            halfSizeY,
            int(halfSizeX+realx*scale),
            int(halfSizeY-realy*scale)
        );

        cout << "REAL VALUES: " << realx << ' ' << realy << endl;

        double leftEyeAngle = (atan2(realy,realx+5)-M_PI/2.0);
        double rightEyeAngle = (atan2(realy,realx-5)-M_PI/2.0);

        if (leftEyeAngle<-M_PI)
        {
            leftEyeAngle += 2*M_PI;
        }

        if (rightEyeAngle<-M_PI)
        {
            rightEyeAngle += 2*M_PI;
        }

        cout << "ANGLES:\n";
        cout << leftEyeAngle << endl;
        cout << rightEyeAngle << endl;

        network.reinitialize();
        network.setValue("Angle1",leftEyeAngle);
        network.setValue("Angle2",rightEyeAngle);
        network.setValue("Bias",0.3);
        network.update();


        double armAngle1 = M_PI*network.getValue("ArmAngle1");
        double armAngle2 = M_PI*network.getValue("ArmAngle2");

        double x=0.0,y=0.0;

        double oldx=0.0,oldy=0.0;

        drawContext.SetPen(wxPen(*wxBLACK,5,wxSOLID));

        {
            cout << "RELATIVE ANGLES: " << armAngle1 << ' ' << armAngle2 << endl;

            armAngle1+=M_PI/2.0;

            if (armAngle1>M_PI)
            {
                armAngle1 -= 2.0*M_PI;
            }

            cout << "ABSOLUTE ANGLES: " << armAngle1 << ' ' << armAngle2 << endl;

            oldx = x;
            oldy = y;
            x += 10*cos(armAngle1);
            y += 10*sin(armAngle1);

            drawContext.DrawLine(
                int(halfSizeX+oldx*scale),
                int(halfSizeY-oldy*scale),
                int(halfSizeX+x*scale),
                int(halfSizeY-y*scale)
            );

            cout << "ELBOW POSITION: " << x << "," << y << endl;

            oldx = x;
            oldy = y;
            x += 10*cos(armAngle2+armAngle1);
            y += 10*sin(armAngle2+armAngle1);

            drawContext.DrawLine(
                int(halfSizeX+oldx*scale),
                int(halfSizeY-oldy*scale),
                int(halfSizeX+x*scale),
                int(halfSizeY-y*scale)
            );

            drawContext.DrawCircle(
                int(halfSizeX+x*scale),
                int(halfSizeY-y*scale),
                5
            );

            cout << "HAND POSITION: " << x << "," << y << endl;
        }

        double distanceSquared = (((x-realx)*(x-realx))+((y-realy)*(y-realy)));
        cout << "Distance Squared: " << distanceSquared << endl;

        cout << "EXPECTED FITNESS: " << (max(0.0,(100-distanceSquared))) << endl;

        cout << "ACTUAL FITNESS: " << individual->getFitness() << endl;

        drawContext.SetPen(wxPen(*wxRED,5,wxSOLID));

        drawContext.DrawLine(
            int(halfSizeX+realx*scale),
            int(halfSizeY-realy*scale),
            int(halfSizeX+realx*scale),
            int(halfSizeY-(realy-0.001)*scale)
        );

    }
#endif

    Experiment* RobotArmExperiment::clone()
    {
        RobotArmExperiment* experiment = new RobotArmExperiment(*this);

        return experiment;
    }

}
