#include "HCUBE_Defines.h"

#include "Experiments/HCUBE_RobotArmLatticeExperiment.h"

#define ROBOT_ARM_LATTICE_EXPERIMENT_DEBUG (0)

namespace HCUBE
{
    using namespace NEAT;

    RobotArmLatticeExperiment::RobotArmLatticeExperiment(string _experimentName,int _threadID)
            :
            Experiment(_experimentName,_threadID),
            userx(1),
            usery(1),
            scale(4)
    {}

    NEAT::GeneticPopulation* RobotArmLatticeExperiment::createInitialPopulation(int populationSize)
    {
        NEAT::GeneticPopulation* population = new NEAT::GeneticPopulation();

        vector<GeneticNodeGene> genes;

        genes.push_back(GeneticNodeGene("Bias","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("Angle1","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("Angle2","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("ArmAngle1","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("ArmAngle2","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("Strength","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));

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

    double RobotArmLatticeExperiment::processEvaluation(shared_ptr<const NEAT::GeneticIndividual> individual,double x,double y)
    {
        return processEvaluation(individual,NULL,x,y);
    }

    double RobotArmLatticeExperiment::processEvaluation(shared_ptr<const NEAT::GeneticIndividual> individual,wxDC *drawContext,double realx,double realy)
    {
        NEAT::FastNetwork<double> network = individual->spawnFastPhenotypeStack<double>();

        int halfSizeX=0;
        int halfSizeY=0;

#ifndef HCUBE_NOGUI
        if (drawContext)
        {
            halfSizeX = drawContext->GetSize().x/2;
            halfSizeY = drawContext->GetSize().y/2;

            drawContext->Clear();

            drawContext->SetPen(wxPen(*wxBLUE,2,wxSOLID));

            drawContext->DrawLine(
                int(halfSizeX+5*scale),
                halfSizeY,
                int(halfSizeX+3*scale),
                int(halfSizeY-2*scale)
            );

            drawContext->DrawLine(
                int(halfSizeX+5*scale),
                halfSizeY,
                int(halfSizeX+7*scale),
                int(halfSizeY-2*scale)
            );

            drawContext->DrawLine(
                int(halfSizeX-5*scale),
                halfSizeY,
                int(halfSizeX-3*scale),
                int(halfSizeY-2*scale)
            );

            drawContext->DrawLine(
                int(halfSizeX-5*scale),
                halfSizeY,
                int(halfSizeX-7*scale),
                int(halfSizeY-2*scale)
            );

            drawContext->SetPen(wxPen(*wxGREEN,2,wxSOLID));

            drawContext->DrawLine(
                int(halfSizeX+5*scale),
                halfSizeY,
                int(halfSizeX+realx*scale),
                int(halfSizeY-realy*scale)
            );

            drawContext->DrawLine(
                int(halfSizeX-5*scale),
                halfSizeY,
                int(halfSizeX+realx*scale),
                int(halfSizeY-realy*scale)
            );

            drawContext->SetPen(wxPen(*wxBLACK,5,wxSOLID));
        }
#endif

        double bestStrength=-INT_MAX;
        double bestArmAngle1,bestArmAngle2;

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

        for (double testArmAngle1=-M_PI;testArmAngle1<M_PI;testArmAngle1+=0.2)
        {
            for (double testArmAngle2=-M_PI;testArmAngle2<M_PI;testArmAngle2+=0.2)
            {
                network.reinitialize();

                network.setValue("Angle1",leftEyeAngle);
                network.setValue("Angle2",rightEyeAngle);
                network.setValue("ArmAngle1",testArmAngle1);
                network.setValue("ArmAngle2",testArmAngle2);
                network.setValue("Bias",0.3);

                network.update();

                double strength = network.getValue("Strength");

                if (strength>bestStrength)
                {
                    bestStrength = strength;
                    bestArmAngle1 = testArmAngle1;
                    bestArmAngle2 = testArmAngle2;
                }
            }
        }

        double armAngle1 = bestArmAngle1;
        double armAngle2 = bestArmAngle2;
        double x=0.0,y=0.0;
        double oldx,oldy;

        {
            armAngle1+=M_PI/2.0;

            if (armAngle1>M_PI)
            {
                armAngle1 -= 2.0*M_PI;
            }

#if ROBOT_ARM_LATTICE_EXPERIMENT_DEBUG
            cout << "ANGLES: " << armAngle1 << ' ' << armAngle2 << endl;
#endif

            oldx=x;
            oldy=y;
            x += 10*cos(armAngle1);
            y += 10*sin(armAngle1);

            if (drawContext)
            {
                drawContext->DrawLine(
                    int(halfSizeX+oldx*scale),
                    int(halfSizeY-oldy*scale),
                    int(halfSizeX+x*scale),
                    int(halfSizeY-y*scale)
                );
            }

#if ROBOT_ARM_LATTICE_EXPERIMENT_DEBUG
            cout << "ELBOW POSITION: " << x << "," << y << endl;
#endif

            oldx=x;
            oldy=y;
            x += 10*cos(armAngle2+armAngle1);
            y += 10*sin(armAngle2+armAngle1);

            if (drawContext)
            {
                drawContext->DrawLine(
                    int(halfSizeX+oldx*scale),
                    int(halfSizeY-oldy*scale),
                    int(halfSizeX+x*scale),
                    int(halfSizeY-y*scale)
                );
            }

            if (drawContext)
            {
                drawContext->DrawCircle(
                    int(halfSizeX+x*scale),
                    int(halfSizeY-y*scale),
                    5
                );
            }

#if ROBOT_ARM_LATTICE_EXPERIMENT_DEBUG
            cout << "HAND POSITION: " << x << "," << y << endl;
            cout << "EXPECTED POSITION: " << realx << ',' << realy << endl;
#endif

            double distanceSquared = (((x-realx)*(x-realx))+((y-realy)*(y-realy)));

#if ROBOT_ARM_LATTICE_EXPERIMENT_DEBUG
            cout << distanceSquared << " " << max(0.0,(100-distanceSquared)) << endl;
#endif

#ifndef HCUBE_NOGUI
            if (drawContext)
            {
                drawContext->SetPen(wxPen(*wxRED,5,wxSOLID));

                drawContext->DrawLine(
                    int(halfSizeX+realx*scale),
                    int(halfSizeY-realy*scale),
                    int(halfSizeX+realx*scale),
                    int(halfSizeY-(realy-0.001)*scale)
                );
            }
#endif

            return (max(0.0,(30-(distanceSquared))));
        }
    }

    void RobotArmLatticeExperiment::processGroup(shared_ptr<NEAT::GeneticGeneration> generation)
    {
        shared_ptr<NEAT::GeneticIndividual> individual = group.front();
        //You get 10 points just for entering the game, wahooo!
        individual->setFitness(10);

        int counter=0;

        for (int intx=-10;intx<=10;intx+=2)
        {
            for (int inty=0;inty<=20;inty+=2)
            {
                individual->reward(processEvaluation(individual,NULL,intx,inty));
            }
        }

#if ROBOT_ARM_LATTICE_EXPERIMENT_DEBUG
        {
            cout << "Finished with evaluation!\n";
            cout << individual->getFitness();
            CREATE_PAUSE(string("Error!: ")+toString(__LINE__));
        }
#endif
        //cout << "Finished with evaluation!\n";

    }

#ifndef HCUBE_NOGUI
    bool RobotArmLatticeExperiment::handleMousePress(wxMouseEvent& event,wxSize &bitmapSize)
    {
        wxPoint clickPoint = event.GetPosition();
        cout << "handling mouse press at " << clickPoint.x << ' ' << clickPoint.y << endl;
        userx = (clickPoint.x-bitmapSize.x/2)/scale;
        usery = max(0.0,(bitmapSize.y/2-clickPoint.y)/scale);
        cout << "User point set to: " << userx << ' ' << usery << endl;
        return true;
    }

    void RobotArmLatticeExperiment::createIndividualImage(wxDC &drawContext,shared_ptr<NEAT::GeneticIndividual> individual)
    {
        processEvaluation(individual,&drawContext,userx,usery);
    }
#endif

    Experiment* RobotArmLatticeExperiment::clone()
    {
        RobotArmLatticeExperiment* experiment = new RobotArmLatticeExperiment(*this);

        return experiment;
    }

}
