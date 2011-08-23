#include "HCUBE_Defines.h"

#include "Experiments/HCUBE_SpatialExperiment.h"

#define FIND_CLUSTER_EXPERIMENT_DEBUG (0)

#define FIND_CLUSTER_EXPERIMENT_USE_DELTAS (0)

#define SPATIAL_USE_THETA (1)

namespace HCUBE
{
    using namespace NEAT;

    SpatialExperiment::SpatialExperiment(string _experimentName,int _threadID)
            :
            Experiment(_experimentName,_threadID),
            resolution(151)
    {}

    NEAT::GeneticPopulation* SpatialExperiment::createInitialPopulation(int populationSize)
    {
        NEAT::GeneticPopulation* population = new NEAT::GeneticPopulation();

        vector<GeneticNodeGene> genes;

        genes.push_back(GeneticNodeGene("Bias","NetworkSensor",0,false));
#ifdef SPATIAL_USE_VECTOR_NETWORK
        genes.push_back(GeneticNodeGene("Cartesian","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("Polar","NetworkSensor",0,false));
#else
        genes.push_back(GeneticNodeGene("X1","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("Y1","NetworkSensor",0,false));
        genes.push_back(GeneticNodeGene("D","NetworkSensor",0,false));
#if SPATIAL_USE_THETA
        genes.push_back(GeneticNodeGene("Theta","NetworkSensor",0,false));
#endif
#endif

        genes.push_back(GeneticNodeGene("Output","NetworkOutputNode",1,false,ACTIVATION_FUNCTION_SIGMOID));

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

    double SpatialExperiment::processEvaluation(
        shared_ptr<NEAT::GeneticIndividual> individual,
        wxDC *drawContext
    )
    {
#ifndef HCUBE_NOGUI

#if FIND_CLUSTER_EXPERIMENT_DEBUG
        cout << "EVALUATION: Testing " << x1 << ',' << y1 << " and big " << x1Big << ',' << y1Big << endl;
        CREATE_PAUSE(string("Error!: ")+toString(__LINE__));
#endif

        int halfSizeX=0;
        int halfSizeY=0;

        int fullSizeX=0;
        int fullSizeY=0;

        Vector2<int> increment;

        int tmpresolution = resolution;

#ifndef HCUBE_NOGUI
        if (drawContext)
        {
            drawContext->Clear();

            halfSizeX = drawContext->GetSize().x/2;
            halfSizeY = drawContext->GetSize().y/2;

            fullSizeX = drawContext->GetSize().x;
            fullSizeY = drawContext->GetSize().y;

            tmpresolution = min(drawContext->GetSize().x,drawContext->GetSize().y);

            if (tmpresolution%2==0)
                tmpresolution++; //stick with odd resolutions.

            //if(tmpresolution>resolution)
            //tmpresolution = resolution;

            //increment.x = 2*(drawContext->GetSize().x/tmpresolution);
            //increment.y = 2*(drawContext->GetSize().y/tmpresolution);

            //increment.x = min(increment.x,increment.y);
            //increment.y = min(increment.x,increment.y);

            increment.x = 1;
            increment.y = 1;

            drawContext->SetPen(wxPen(wxColor(128,128,128),1,wxSOLID));
        }
#endif

#if FIND_CLUSTER_EXPERIMENT_DEBUG
        cout << "Initializing substrate\n";
        CREATE_PAUSE(string("Error!: ")+toString(__LINE__));
#endif

#if FIND_CLUSTER_EXPERIMENT_DEBUG
        cout << "Setting values\n";
        CREATE_PAUSE(string("Error!: ")+toString(__LINE__));
#endif

        /*
        substrate.setValue(nameLookup[pair<int,int>((y1-numNodesY/2),(x1-numNodesX/2))],1.0);
        if (drawContext)
        {
            drawContext->SetPen(wxPen(wxColor(0,0,0),2,wxSOLID));
            drawContext->DrawRectangle((x1-numNodesX/2)*CELL_SIZE+halfSizeX,(y1-numNodesY/2)*CELL_SIZE+halfSizeY,CELL_SIZE,CELL_SIZE);
        }
        */

        cout << "Populating substrate...";
#ifdef SPATIAL_USE_VECTOR_NETWORK
        substrate = individual->spawnVectorPhenotypeStack<double>();
#else
        substrate = individual->spawnFastPhenotypeStack();
#endif

        progress_timer t;

        int linkCount=0;

        int counter=0;
        double x1Val=0,y1Val=0;

        cout << "DISPLAYING IMAGE IN THE RANGE: " << endl;
        cout << ((0-tmpresolution/2)/double((tmpresolution-1)/2)) << ','
        << ((0-tmpresolution/2)/double((tmpresolution-1)/2)) << endl;
        cout << "TO" << endl;
        cout << (((tmpresolution-1)-tmpresolution/2)/double((tmpresolution-1)/2)) << ','
        << (((tmpresolution-1)-tmpresolution/2)/double((tmpresolution-1)/2)) << endl;

        cout << "TMPRESOLUTION: " << tmpresolution << endl;


        for (int y1=0;y1<tmpresolution;y1++)
        {
            for (int x1=0;x1<tmpresolution;x1++)
            {
                //Remap to [-1,1]
                if (tmpresolution>1)
                {
                    x1Val = -1.0 + (double(x1)/(tmpresolution-1))*2.0;
                }
                else
                {
                    x1Val = 0.0;
                }

                if (tmpresolution>1)
                {
                    y1Val = -1.0 + (double(y1)/(tmpresolution-1))*2.0;
                }
                else
                {
                    y1Val = 0.0;
                }

                //Old method for normalizing
                //x1Val = (x1-tmpresolution/2)/double((tmpresolution-1)/2);
                //y1Val = (y1-tmpresolution/2)/double((tmpresolution-1)/2);

                substrate.reinitialize();

#ifdef SPATIAL_USE_VECTOR_NETWORK
                substrate.setValue("Cartesian",Vector2<double>(x1Val,y1Val));
                substrate.setValue("Polar",Vector2<double>(((x1Val*x1Val) + (y1Val*y1Val))/2.0,sin(atan2(y1Val,x1Val))));
#else
                substrate.setValue("X1",x1Val);
                substrate.setValue("Y1",y1Val);
                substrate.setValue("D", ((x1Val*x1Val) + (y1Val*y1Val))/2.0 );
#if SPATIAL_USE_THETA
                substrate.setValue("Theta", sin(atan2(y1Val,x1Val))  );
#endif
#endif
                //cout << x1Val << ',' << y1Val << ": " << atan2(y1Val,x1Val) << endl;
                //system("PAUSE");

                substrate.setValue("Bias",Vector2<double>(0.3,0.3));
                substrate.update();


                //double greyVal = substrate.getValue("Output").getMagnitudeSquared();
                double redVal = substrate.getValue("Output").x;
                if (redVal>1.0) redVal = 1.0;

                double greenVal = substrate.getValue("Output").y;
                if (greenVal>1.0) greenVal = 1.0;

                //greyVal = (greyVal+1.0)/2.0; //scaled to be positive

                //cout << greyVal << endl;

                //I could use unsigned char here, but I was lazy and didn't want to typedef unsigned char uchar
                //for windows.
                int redValChar = int(floor(fabs(redVal*255)+0.5));
                int greenValChar = int(floor(fabs(greenVal*255)+0.5));

                if (drawContext)
                {
                    drawContext->SetPen(*wxTRANSPARENT_PEN);
                    drawContext->SetBrush(wxBrush(wxColor(redValChar,greenValChar,0),wxSOLID));
                    drawContext->DrawRectangle(
                        x1*increment.x,
                        y1*increment.y,
                        increment.x,
                        increment.y
                    );
                }
            }
        }

#endif
        return 0;
    }

    void SpatialExperiment::processGroup(shared_ptr<NEAT::GeneticGeneration> generation)
    {
        shared_ptr<NEAT::GeneticIndividual> individual = group.front();
        /*{
            cout << "Starting Evaluation on object:" << this << endl;
            cout << "Running on individual " << individual << endl;
        }*/

        //You get 10 points just for entering the game, wahooo!
        individual->setFitness(10);

        processEvaluation(individual,NULL);

        //cout << "Individual Evaluation complete!\n";

        //cout << maxFitness << endl;

        //individual->reward(fitness);

        //if (fitness >= maxFitness*.95)
        {
            //cout << "PROBLEM DOMAIN SOLVED!!!\n";
        }
    }

#ifndef HCUBE_NOGUI
    bool SpatialExperiment::handleMousePress(wxMouseEvent& event,wxSize &bitmapSize)
    {
        wxPoint clickPoint = event.GetPosition();

        int halfSizeX = bitmapSize.x/2;
        int halfSizeY = bitmapSize.y/2;

        cout << clickPoint.x << ',' << clickPoint.y << endl;

        switch (event.GetButton())
        {
        case wxMOUSE_BTN_LEFT:
        {
            if (
                clickPoint.x+60>bitmapSize.x &&
                clickPoint.y+20>bitmapSize.y
            )
            {
                increaseResolution();
            }
            else if (
                clickPoint.x+60>bitmapSize.x &&
                clickPoint.y-20<0
            )
            {
                decreaseResolution();
            }
            else if (
                clickPoint.x < 85 &&
                clickPoint.y < 20
            )
            {
                string directoryString;

                if (experimentName.length())
                    directoryString = string("images/")+experimentName+string("/");
                else
                    directoryString = string("images/");
                replace_all(directoryString," ","_");

                for (int a=0;a<(int)directoryString.size();a++)
                {
                    if (directoryString[a]!='/'&&!isalnum(directoryString[a]))
                    {
                        directoryString[a]='_';
                    }
                }

                cout << directoryString << endl;

                if (!boost::filesystem::exists(directoryString))
                {
                    boost::filesystem::create_directory( directoryString );
                }

                {
                    wxBitmap tmpBitmap(1440,900);

                    {
                        wxMemoryDC tmpDC(tmpBitmap);

                        processEvaluation(
                            lastIndividualSeen,
                            &tmpDC
                        );

                        tmpDC.SelectObject(wxNullBitmap);
                    }

                    tmpBitmap.SaveFile(
                        STRING_TO_WXSTRING(directoryString+string("Lowres_Input.png")),
                        wxBITMAP_TYPE_PNG
                    );
                }


            }
            else if (
                clickPoint.x<85 &&
                clickPoint.y+20>bitmapSize.y
            )
            {
                cout << "TODO: Make Single Save function\n";
            }
            else
            {
                cout << "Got a move-block request!\n";
            }
        }
        break;
        case wxMOUSE_BTN_RIGHT:
        {
            wxPoint tmpPoint;
            tmpPoint.x = clickPoint.x;
            tmpPoint.y = clickPoint.y;

            cout << "Clicked on " << tmpPoint.x << ',' << tmpPoint.y << endl;

        }
        break;
        case wxMOUSE_BTN_MIDDLE:
        {}
        break;
        }

        return false; //mouse presses can't affect the image
    }

    bool SpatialExperiment::handleMouseMotion(wxMouseEvent& event,wxDC &temp_dc,shared_ptr<NEAT::GeneticIndividual> individual)
    {
        wxPoint clickPoint = event.GetPosition();

        int halfSizeX = temp_dc.GetSize().x/2;
        int halfSizeY = temp_dc.GetSize().y/2;

        wxPoint tmpPoint;
        tmpPoint.x = clickPoint.x;
        tmpPoint.y = clickPoint.y;

        return false;
    }

    void SpatialExperiment::createIndividualImage(wxDC &drawContext,shared_ptr<NEAT::GeneticIndividual> individual)
    {
        cout << "CREATING IMAGE!!!\n";
        try
        {
            {
                //cout << "Performing evaluation.  time: ";
                //progress_timer t;
                processEvaluation(
                    individual,
                    &drawContext
                );
            }
            //cout << endl;

            wxCoord w, h;
            drawContext.GetSize(&w, &h);

            drawContext.DrawText(
                _T("Save images"),
                2,
                2
            );

            drawContext.DrawText(
                _T("Inc. Res"),
                w-60,
                h-20
            );

            drawContext.DrawText(
                _T("Dec. Res"),
                w-60,
                0
            );
        }
        catch (string s)
        {
            cout << "ERROR: " << s << endl;
        }
    }
#endif

    void SpatialExperiment::increaseResolution()
    {
        resolution*=3;
    }

    void SpatialExperiment::decreaseResolution()
    {
        if (resolution>1)
            resolution/=3;
    }

    Experiment* SpatialExperiment::clone()
    {
        SpatialExperiment* experiment = new SpatialExperiment(*this);

        return experiment;
    }

}
