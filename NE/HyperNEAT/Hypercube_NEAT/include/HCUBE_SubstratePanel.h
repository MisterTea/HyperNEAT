#ifndef HCUBE_SUBSTRATEPANEL_H_INCLUDED
#define HCUBE_SUBSTRATEPANEL_H_INCLUDED

#include "HCUBE_Defines.h"

namespace HCUBE
{
    class SubstratePanel : public wxPanel
    {
    public:
    protected:
        Experiment *experiment;
        shared_ptr<const NEAT::GeneticIndividual> individual;
        wxBitmap outputBitmap;
        int selectedRow,selectedCol;

    public:
        SubstratePanel(
            Experiment *_experiment,
            const NEAT::GeneticIndividual *_individual,
            wxWindow* _parent=NULL,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize
        );

        void paint(wxPaintEvent& event);

        void handleMouseClick(wxMouseEvent& event);

        DECLARE_EVENT_TABLE()

    protected:
    };
}

#endif // HCUBE_SUBSTRATEPANEL_H_INCLUDED
