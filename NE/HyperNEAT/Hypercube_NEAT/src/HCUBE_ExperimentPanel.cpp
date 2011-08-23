#include "HCUBE_Defines.h"

#ifndef HCUBE_NOGUI

#include "HCUBE_ExperimentPanel.h"

#include "HCUBE_MainFrame.h"

namespace HCUBE
{
    BEGIN_EVENT_TABLE(ExperimentPanel, wxPanel)
        EVT_BUTTON(ID_ViewIndividualButton, ExperimentPanel::viewIndividual)
        EVT_BUTTON(ID_AnalyzeIndividualButton, ExperimentPanel::postHocIndividual)
    END_EVENT_TABLE()

    ExperimentPanel::ExperimentPanel(
        wxWindow *parent,
        wxWindowID winid,
        const wxPoint& pos,
        const wxSize& size,
        long style,
        const wxString& name
    )
            :
            wxPanel(
                parent,
                winid,
                pos,
                size,
                style,
                name),

            populationSize(0)

    {
        mainSizer = new wxBoxSizer(wxHORIZONTAL);

        generationControl = new wxSpinCtrl(
            this,
            -1,
            CSTR_TO_WXSTRING("1"),
            //wxPoint(50,50),
            wxDefaultPosition,
            //wxSize(100,20),
            wxDefaultSize,
            wxSP_ARROW_KEYS,
            0,
            10000,
            0,
            _T("GenerationControl")
        );

        individualControl = new wxSpinCtrl(
            this,
            -1,
            CSTR_TO_WXSTRING("1"),
            //wxPoint(160,50),
            wxDefaultPosition,
            //wxSize(100,20),
            wxDefaultSize,
            wxSP_ARROW_KEYS,
            0,
            10000,
            0,
            _T("IndividualControl")
        );

        viewIndividualButton = new wxButton(
            this,
            ID_ViewIndividualButton,
            CSTR_TO_WXSTRING("View Individual")
            //wxPoint(300,50),
            //wxSize(80,20)
        );

        analyzeIndividualButton = new wxButton(
            this,
            ID_AnalyzeIndividualButton,
            CSTR_TO_WXSTRING("Analyze Individual")
            //wxPoint(300,50),
            //wxSize(80,20)
        );

        mainSizer->Add(generationControl,1,wxALIGN_CENTER|wxALL,10);
        mainSizer->Add(individualControl,1,wxALIGN_CENTER|wxALL,10);
        mainSizer->Add(viewIndividualButton,0,wxALIGN_CENTER|wxALL,10);
        mainSizer->Add(analyzeIndividualButton,0,wxALIGN_CENTER|wxALL,10);

        SetSizer( mainSizer );
    }

    void ExperimentPanel::viewIndividual(wxCommandEvent& WXUNUSED(event))
    {
        cout << "Viewing individual " << individualControl->GetValue() << " from generation " << generationControl->GetValue() << endl;
        //((MainFrame*)GetParent())->viewIndividual(generationControl->GetValue()-1,individualControl->GetValue()-1);
    }

    void ExperimentPanel::postHocIndividual(wxCommandEvent& WXUNUSED(event))
    {
        cout << "Viewing individual " << individualControl->GetValue() << " from generation " << generationControl->GetValue() << endl;
        //((MainFrame*)GetParent())->analyzeIndividual(generationControl->GetValue()-1,individualControl->GetValue()-1);
    }

}

#endif
