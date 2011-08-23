#include "HCUBE_Defines.h"

#ifndef HCUBE_NOGUI

#include "HCUBE_NeuralNetworkWeightGrid.h"

#include "HCUBE_UserEvaluationFrame.h"

using namespace NEAT;

namespace HCUBE
{
    BEGIN_EVENT_TABLE(NeuralNetworkWeightGrid, wxGrid)
        EVT_GRID_CELL_CHANGE(NeuralNetworkWeightGrid::cellChanged)
    END_EVENT_TABLE()

    NeuralNetworkWeightGrid::NeuralNetworkWeightGrid(
        wxWindow *parent,
        wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        long style,
        const wxString& name
    )
            :
            wxGrid(parent,id,pos,size,style,name)
    {}

    void NeuralNetworkWeightGrid::setTarget(shared_ptr<NEAT::GeneticIndividual> _individual)
    {
        if (individual==_individual)
        {
            return;
        }

        individual = _individual;

        DeleteCols(0,GetNumberCols());
        DeleteRows(0,GetNumberRows());

        InsertRows(0,individual->getNodesCount());
        InsertCols(0,individual->getNodesCount());

        for (int a=0;a<individual->getNodesCount();a++)
        {
            const GeneticNodeGene *node = individual->getNode(a);

            SetColLabelValue(a,STRING_TO_WXSTRING(toString(node->getID())));
            SetRowLabelValue(a,STRING_TO_WXSTRING(toString(node->getID())));
            SetColSize(a,2);
        }

        for (int a=0;a<individual->getNodesCount();a++)
        {
            for (int b=0;b<individual->getNodesCount();b++)
            {
                const GeneticNodeGene *source = individual->getNode(a);
                const GeneticNodeGene *dest = individual->getNode(b);

                if (individual->linkExists(source->getID(),dest->getID()))
                {
                    SetCellValue(a,b,STRING_TO_WXSTRING(toString(individual->getLink(source->getID(),dest->getID())->getWeight())));
                }
                else
                {
                    SetCellValue(a,b,_T("0"));
                }
            }
        }

        ForceRefresh();
    }

    void NeuralNetworkWeightGrid::cellChanged(wxGridEvent& event)
    {
        cout << "CELL CHANGED!\n";

        double newValue = stringTo<double>(WXSTRING_TO_STRING(GetCellValue(event.GetRow(),event.GetCol())));

        const GeneticNodeGene *source = individual->getNode(event.GetRow());
        const GeneticNodeGene *dest = individual->getNode(event.GetCol());

        if (individual->linkExists(source->getID(),dest->getID()))
        {
            if (fabs(newValue)>1e-6)
            {
                //link still exists, just change the weight

                individual->getLink(source->getID(),dest->getID())->setWeight(newValue);
            }
            else
            {
                //link is gone, for now just set to 0

                individual->getLink(source->getID(),dest->getID())->setWeight(0.0);

                //individual->removeLink(source->getID(),dest->getID());
            }
        }
        else
        {
            if (fabs(newValue)>1e-6)
            {
                //Create a new link

                individual->addLink(GeneticLinkGene(source->getID(),dest->getID(),newValue));
            }
            else
            {
                //Link didn't exist and it still doesn't.  Do nothing
            }
        }

        wxWindow *parent = GetParent();

        bool finished=false;

        do
        {
            UserEvaluationFrame *userFrame = static_cast<UserEvaluationFrame*>(parent);

            if (userFrame)
            {
                userFrame->updateIndividual(individual);
                finished=true;
            }
            else
            {
                cout << "DYNAMIC CAST FAILED!\n";
            }

            parent = parent->GetParent();
        }
        while (!finished&&parent);

        //cout << "Could not find managing window!  Oh shiz!\n";
    }
}

#endif

