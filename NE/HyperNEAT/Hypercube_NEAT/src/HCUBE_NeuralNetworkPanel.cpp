#include "HCUBE_Defines.h"

#ifndef HCUBE_NOGUI

#include "HCUBE_NeuralNetworkPanel.h"

#include "HCUBE_NetworkPanel.h"
#include "HCUBE_NeuralNetworkWeightGrid.h"

namespace HCUBE
{

    using namespace NEAT;

    /*BEGIN_EVENT_TABLE(NeuralNetworkPanel, wxPanel)
    EVT_PAINT(NeuralNetworkPanel::paint)
    EVT_LEFT_UP(NeuralNetworkPanel::handleMouseClick)
    END_EVENT_TABLE()*/

    NeuralNetworkPanel::NeuralNetworkPanel( wxWindow* parent, int id, wxPoint pos, wxSize size, int style )
            :
            NeuralNetworkPanelTemplate( parent, id, pos, size, style )//,
            //outputBitmap(size.x,size.y)
    {}

    void NeuralNetworkPanel::setTarget(shared_ptr<NEAT::GeneticIndividual> individual)
    {
        networkVisualizationPanel->createNetworkImage(individual);
        networkAdjacency->setTarget(individual);
    }

    void NeuralNetworkPanel::setReadOnly(bool value)
    {
        networkAdjacency->setReadOnly(value);
    }

    bool NeuralNetworkPanel::isReadOnly()
    {
        return networkAdjacency->isReadOnly();
    }

    /*void NeuralNetworkPanel::createNetworkImage(shared_ptr<const NEAT::GeneticIndividual> individual)
    {
        {
            // Create a memory DC
            wxMemoryDC dc;

            cout << "CREATING OUTPUT BITMAP OF SIZE " << dc.GetSize().x << ',' << dc.GetSize().y << endl;

            cout << "PANEL SIZE?: " << GetSize().x << ',' << GetSize().y << '\n';

            if(
                outputBitmap.GetWidth()!=GetSize().x ||
                outputBitmap.GetHeight()!=GetSize().y
            )
            {
                outputBitmap = wxBitmap(GetSize().x,GetSize().y);
            }

            dc.SelectObject(outputBitmap);

            dc.Clear();

            if (individual)
            {
                cout << "AN INDIVIDUAL EXISTS!\n";

                vector<wxPoint> nodeCoords;

                int linksShown=0;

                vector< pair<double,int> > positionHistogram;

                int windowWidth = 480;
                int windowHeight = 360;

                int spaceAmount = windowWidth/(1+individual->getMaxNodePositionOccurance());

                for (int a=0;a<individual->getNodesCount();a++)
                {
                    const GeneticNodeGene *node = individual->getNode(a);
                    int xCoord=-1;
                    int yCoord = int(windowHeight - (node->getDrawingPosition()* (windowHeight-30)) )-15;
                    //cout << yCoord << '/' << node->getDrawingPosition() << endl;
                    for (int b=0;b<positionHistogram.size();b++)
                    {
                        if (fabs(positionHistogram[b].first-node->getDrawingPosition())<0.01)
                        {
                            positionHistogram[b].second++;
                            xCoord = 15+positionHistogram[b].second*spaceAmount + int(120*(.5-fabs(.5-node->getDrawingPosition())));
                        }
                    }

                    if (xCoord==-1)
                    {
                        positionHistogram.push_back(pair<double,int>(node->getDrawingPosition(),0));

                        xCoord = 15+positionHistogram[positionHistogram.size()-1].second*spaceAmount + int(120*(.5-fabs(.5-node->getDrawingPosition())));
                    }

                    nodeCoords.push_back(wxPoint(xCoord,yCoord));

                    dc.DrawRectangle(xCoord-5,yCoord-5,10,10);
                    if (toString(node->getName()).size()!=0)
                    {
                        dc.DrawText(toString(node->getName()).c_str(),xCoord+7, yCoord);
                    }
                    else
                    {
                        dc.DrawText(activationFunctionNames[node->getActivationFunction()],xCoord+7, yCoord);
                    }
                }

                //cout << individual->getLinksCount() << endl;
                for (int a=0;a<individual->getLinksCount();a++)
                {
                    const GeneticLinkGene *link = individual->getLink(a);
                    wxPoint p1,p2;
                    for (int a=0;a<individual->getNodesCount();a++)
                    {
                        const GeneticNodeGene *node = individual->getNode(a);

                        if (node->getID()==link->getFromNodeID())
                        {
                            p1 = nodeCoords[a];
                        }
                        if (node->getID()==link->getToNodeID())
                        {
                            p2 = nodeCoords[a];
                        }
                    }

                    //cout << p1 << '\t' << p2 << endl;

                    wxColour lineColor;

                    double weight = link->getWeight();

                    double minWeight = NEAT::Globals::getSingleton()->getParameterValue("LinkGeneMinimumWeightForPhentoype");

                    linksShown++;

                    if (p1.y>=p2.y)
                    {
                        if (weight<0)
                            lineColor = *wxRED;
                        else
                            lineColor = *wxBLACK;
                    }
                    else
                    {
                        if (weight<0)
                            lineColor = *wxGREEN;
                        else
                            lineColor = *wxBLUE;
                    }

                    if (p1==p2)
                    {
                        for (int a=0;a<int(ceil(fabs(weight)));a++)
                        {
                            int shift = (int(a)/2);
                            if ((a%2)==1)
                                shift *= -1;

                            dc.SetPen(lineColor);
                            int x = min(p1.x+shift,p2.x+shift+20);
                            int width = abs((p1.x+shift)-(p2.x+shift+20));
                            int y = min(p1.y+shift+10,p2.y+shift-20);
                            int height = abs((p1.y+shift+10)-(p2.y+shift-10));
                            dc.DrawRectangle(x,y,width,height);
                            dc.SetPen(*wxBLACK);
                        }
                    }
                    else
                    {
                        int pixels = int(ceil(fabs(weight+0.001)));
                        wxPoint halfway;
                        halfway.x = p1.x+(p2.x-p1.x)/2;
                        halfway.y = p1.y+(p2.y-p1.y)/2;

                        if (p1.y>=p2.y)
                            halfway.y+=5;
                        else
                            halfway.y-=5;

                        for (int a=0;a<pixels;a++)
                        {
                            int shift = (int(a)/2);
                            if ((a%2)==1)
                                shift *= -1;

                            dc.SetPen(lineColor);
                            dc.DrawLine(p1.x+shift,p1.y,halfway.x+shift,halfway.y);
                            dc.DrawLine(halfway.x+shift,halfway.y,p2.x+shift,p2.y);
                            dc.SetPen(*wxBLACK);
                        }
                    }
                }
            }
        }

        Refresh();
    }

    void NeuralNetworkPanel::paint(wxPaintEvent& event)
    {
        wxPaintDC dc(this);

        dc.Clear();

        if(outputBitmap.IsOk())
        {
            dc.DrawBitmap(outputBitmap,0,0,true);
        }
        else
        {
            cout << __FILE__ << ": Bitmap is not ok to draw\n";
        }
    }

    void NeuralNetworkPanel::handleMouseClick(wxMouseEvent& event)
    {}*/
}

#endif

