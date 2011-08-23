#ifndef HCUBE_EVALUATIONPANEL_H_INCLUDED
#define HCUBE_EVALUATIONPANEL_H_INCLUDED

#include "HCUBE_Defines.h"

namespace HCUBE
{
    /**
     * The Evaluation Panel allows experiments to draw to the GUI and respond to user events.
     */
    class EvaluationPanel : public wxPanel
    {
    public:
    protected:
        shared_ptr<Experiment> experiment,backupExperiment;
        shared_ptr<NEAT::GeneticIndividual> individual,backupIndividual;
        wxBitmap *outputBitmap;
        bool bitmapLocked;

    public:
        EvaluationPanel(
            wxWindow* parent,
            int id = wxID_ANY,
            wxPoint pos = wxDefaultPosition,
            wxSize size = wxDefaultSize,
            int style = wxTAB_TRAVERSAL
        );

        virtual ~EvaluationPanel()
        {
            delete outputBitmap;
        }

        /**
        * Gets the individual that is updating this evaluation panel
        */
        inline shared_ptr<NEAT::GeneticIndividual> getIndividual()
        {
            return individual;
        }

        /**
        * Sets the individual that is updating this evaluation panel
        */
        void setTarget(shared_ptr<Experiment> _experiment,shared_ptr<NEAT::GeneticIndividual> _individual);

        /**
        * (Re)paint the panel as appropriate
        */
        void paint(wxPaintEvent& event);

        /**
        * Pass mouse clicks to the experiment
        */
        void handleMouseClick(wxMouseEvent& event);

        /**
        * Pass mouse motion to the experiment
        */
        void handleMouseMotion(wxMouseEvent& event);

        DECLARE_EVENT_TABLE()

    protected:
        /**
        * This class cannot be copied
        */
        EvaluationPanel(const EvaluationPanel &other)
        {}

        /**
        * This class cannot be copied
        */
        const EvaluationPanel &operator=(const EvaluationPanel &other)
        {
            return *this;
        }
    };
}


#endif // HCUBE_EVALUATIONPANEL_H_INCLUDED
