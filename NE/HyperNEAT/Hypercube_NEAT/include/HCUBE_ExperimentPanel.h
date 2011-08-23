#ifndef HCUBE_EXPERIMENTPANEL_H_INCLUDED
#define HCUBE_EXPERIMENTPANEL_H_INCLUDED

#include "HCUBE_Defines.h"

#include "HCUBE_ExperimentRun.h"

namespace HCUBE
{
    /**
    * This is the panel containing the UI for selecting a particular individual and viewing
    * or analyzing that individual.
    */
    class ExperimentPanel : public wxPanel
    {
    public:
    protected:
        wxBoxSizer *mainSizer;

        wxSpinCtrl *generationControl,*individualControl;

        wxButton *viewIndividualButton,*analyzeIndividualButton;

        int populationSize;

    public:
        ExperimentPanel(
            wxWindow *parent,
            wxWindowID winid = wxID_ANY,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxTAB_TRAVERSAL | wxNO_BORDER,
            const wxString& name = wxPanelNameStr);

        void setPopulationSize(int newPopulationSize);

        void updateNumGenerations(int numGenerations);

        void viewIndividual(wxCommandEvent& WXUNUSED(event));

        void postHocIndividual(wxCommandEvent& WXUNUSED(event));

        DECLARE_EVENT_TABLE()

        enum ExperimentPanelIDs
        {
            ID_ViewIndividualButton=0,
            ID_AnalyzeIndividualButton=1
        };
    protected:
    };
}

#endif // HCUBE_EXPERIMENTPANEL_H_INCLUDED
