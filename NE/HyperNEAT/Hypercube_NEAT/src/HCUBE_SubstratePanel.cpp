#include "HCUBE_Defines.h"

#ifndef HCUBE_NOGUI

#include "HCUBE_SubstratePanel.h"

#include "Experiments/HCUBE_Experiment.h"

#include "wx/dcmemory.h"

namespace HCUBE
{

    BEGIN_EVENT_TABLE(SubstratePanel, wxPanel)
        EVT_PAINT(SubstratePanel::paint)
        EVT_LEFT_UP(SubstratePanel::handleMouseClick)
    END_EVENT_TABLE()


    SubstratePanel::SubstratePanel(
        Experiment *_experiment,
        const NEAT::GeneticIndividual *_individual,
        wxWindow* _parent,
        const wxPoint& pos,
        const wxSize& size
    )
            :
            wxPanel(_parent, -1, pos, size),
            experiment(_experiment),
            individual(_individual),
            outputBitmap(512,256),
            selectedRow(-1),
            selectedCol(-1)
    {
        // Create a memory DC
        wxMemoryDC temp_dc;

        temp_dc.SelectObject(outputBitmap);

        //experiment->createSubstrateImage(temp_dc);
    }

    void SubstratePanel::paint(wxPaintEvent& event)
    {
        wxPaintDC dc(this);

        dc.DrawBitmap(outputBitmap,0,0,true);
    }

    void SubstratePanel::handleMouseClick(wxMouseEvent& event)
    {
        cout << event.GetX() << ',' << event.GetY() << endl;

        //experiment->createSubstrateImage(temp_dc);

        this->Refresh();
    }

}

#endif
