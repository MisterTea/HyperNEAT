#ifndef HCUBE_NETWORKPANEL_H_INCLUDED
#define HCUBE_NETWORKPANEL_H_INCLUDED

#include "HCUBE_Defines.h"

namespace HCUBE
{
    /**
    * NetworkPanel aims to present a graphical view of ANNs or CPPNs from an individual
    */
    class NetworkPanel : public wxPanel
    {
    public:
    protected:
        wxBitmap outputBitmap;

    public:
        NetworkPanel(
            wxWindow* parent,
            int id = wxID_ANY,
            wxPoint pos = wxDefaultPosition,
            wxSize size = wxDefaultSize,
            int style = wxTAB_TRAVERSAL
        );

        void createNetworkImage(shared_ptr<const NEAT::GeneticIndividual> individual);

        void paint(wxPaintEvent& event);

        void handleMouseClick(wxMouseEvent& event);

        DECLARE_EVENT_TABLE()

    protected:
    };
}


#endif // HCUBE_NETWORKPANEL_H_INCLUDED
