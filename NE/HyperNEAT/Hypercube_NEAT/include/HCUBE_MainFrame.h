#ifndef HCUBE_MAINFRAME_H_INCLUDED
#define HCUBE_MAINFRAME_H_INCLUDED

#ifndef HCUBE_NOGUI

#include "HCUBE_GUITemplate.h"

#include "HCUBE_Defines.h"

#include "HCUBE_ExperimentRun.h"
#include "HCUBE_ExperimentPanel.h"

namespace HCUBE
{
    /**
    * MainFrame contains the window frame where the base HyperNEAT GUI is contained
    */
    class MainFrame: public MainFrameTemplate
    {
    public:
    protected:

        ExperimentRun experimentRun;

        UserEvaluationFrame *userEvaluationFrame;

        string populationFileName;

    public:
        MainFrame(
            wxWindow* parent,
            int id = wxID_ANY,
            wxString title = wxT("Jason Gauci's HyperNEAT"),
            wxPoint pos = wxDefaultPosition,
            wxSize size = wxSize( 640,480 ),
            int style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL
        );

        virtual ~MainFrame();

        virtual void loadExperiment( wxCommandEvent& event );
        virtual void runExperiment( wxCommandEvent& event );
        virtual void restartExperiment( wxCommandEvent& event );
        virtual void pauseExperiment( wxCommandEvent& event );
        virtual void loadPopulation( wxCommandEvent& event );
        virtual void exitApplication( wxCommandEvent& event );
        virtual void aboutApplication( wxCommandEvent& event );
        virtual void viewIndividual( wxCommandEvent& event );
        virtual void analyzeIndividual( wxCommandEvent& event );

        /*void enableExperimentPanel(bool enabled=true)
        {
            experimentPanel->Enable(enabled);
        }*/

        void setPopulationSize(int newPopulationSize);

        void updateNumGenerations(int numGenerations);

        /*
        ExperimentPanel* getExperimentPanel()
        {
            return experimentPanel;
        }
        */

        UserEvaluationFrame *getUserEvaluationFrame()
        {
            return userEvaluationFrame;
        }

    protected:
    };

}

#endif

#endif // HCUBE_MAINFRAME_H_INCLUDED
