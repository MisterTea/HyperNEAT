#include "HCUBE_Defines.h"

#ifndef HCUBE_NOGUI

#include "HCUBE_UserEvaluationFrame.h"

#include "HCUBE_ExperimentRun.h"
#include "HCUBE_EvaluationPanel.h"

#include "HCUBE_NeuralNetworkPanel.h"

namespace HCUBE
{
    BEGIN_EVENT_TABLE(UserEvaluationFrame, UserEvaluationFrameTemplate)
        EVT_LEFT_UP(UserEvaluationFrame::handleMouseClick)
        //EVT_IDLE(UserEvaluationFrame::handleIdleEvent)
    END_EVENT_TABLE()

    UserEvaluationFrame::UserEvaluationFrame(
        ExperimentRun *_experimentRun,
        wxWindow* parent,
        int id,
        wxString title,
        wxPoint pos,
        wxSize size,
        int style
    )
            :
            UserEvaluationFrameTemplate( parent, id, title, pos, size, style ),
            experimentRun(_experimentRun)
    {
        voteButtons.push_back(voteButton0);
        voteButtons.push_back(voteButton1);
        voteButtons.push_back(voteButton2);
        voteButtons.push_back(voteButton3);
        voteButtons.push_back(voteButton4);
        voteButtons.push_back(voteButton5);
        voteButtons.push_back(voteButton6);
        voteButtons.push_back(voteButton7);

        evaluationPanels.push_back(evaluationPanel0);
        evaluationPanels.push_back(evaluationPanel1);
        evaluationPanels.push_back(evaluationPanel2);
        evaluationPanels.push_back(evaluationPanel3);
        evaluationPanels.push_back(evaluationPanel4);
        evaluationPanels.push_back(evaluationPanel5);
        evaluationPanels.push_back(evaluationPanel6);
        evaluationPanels.push_back(evaluationPanel7);
    }

    void UserEvaluationFrame::updateIndividual(shared_ptr<NEAT::GeneticIndividual> individual)
    {
        for (int a=0;a<8;a++)
        {
            if (experimentRun->getIndividual(a)==individual)
            {
                cout << __FILE__ << ":" << __LINE__ << endl;
                evaluationPanels[a]->setTarget(experimentRun->getExperiment(),experimentRun->getIndividual(a));
            }
        }

        networkPanel->setTarget(individual);

        new thread(
            boost::bind(
                &EvaluationPanel::setTarget,
                highResPanel,
                experimentRun->getExperiment(),
                individual
            )
        );
        //highResPanel->setTarget(experimentRun->getExperiment(),individual);
    }

    void UserEvaluationFrame::updateEvaluationPanels()
    {
        for (int a=0;a<8;a++)
        {
            cout << __FILE__ << ":" << __LINE__ << endl;
            evaluationPanels[a]->setTarget(experimentRun->getExperiment(),experimentRun->getIndividual(a));
        }
        Show(TRUE);
    }

    void UserEvaluationFrame::advanceGeneration( wxCommandEvent& event )
    {
        for (int a=0;a<8;a++)
        {
            cout << __FILE__ << ":" << __LINE__ << endl;
            double fitness = voteButtons[a]->GetValue()?1000.0:10.0;
            experimentRun->getIndividual(a)->setFitness(fitness);
            voteButtons[a]->SetValue(false);
        }

        experimentRun->setRunning(true);
        Show(FALSE);
        event.Skip();
    }

    void UserEvaluationFrame::resetVotes( wxCommandEvent& event )
    {
        for (int a=0;a<8;a++)
        {
            voteButtons[a]->SetValue(false);
        }

        event.Skip();
    }

    void UserEvaluationFrame::handleMouseClick(wxMouseEvent& event)
    {
        for (int a=0;a<8;a++)
        {
            if (event.GetEventObject()==evaluationPanels[a])
            {
                cout << "CLICKED ON PANEL " << a << endl;
                networkPanel->setTarget(evaluationPanels[a]->getIndividual());

                new thread(
                    boost::bind(
                        &EvaluationPanel::setTarget,
                        highResPanel,
                        experimentRun->getExperiment(),
                        evaluationPanels[a]->getIndividual()
                    )
                );

                //new thread( boost::bind(&UserEvaluationFrame::processHighResImage,this,evaluationPanels[a]->getIndividual()) );
                //processHighResImage(evaluationPanels[a]->getIndividual());
            }
        }

        cout << "Clicked!\n";
        event.Skip();
    }

    void UserEvaluationFrame::processHighResImage(shared_ptr<NEAT::GeneticIndividual> individual)
    {
        highResPanel->setTarget(experimentRun->getExperiment(),individual);
    }
}

#endif

