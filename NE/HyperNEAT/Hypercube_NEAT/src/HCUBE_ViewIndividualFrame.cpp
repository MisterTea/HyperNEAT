#include "HCUBE_Defines.h"

#ifndef HCUBE_NOGUI

#include "HCUBE_ViewIndividualFrame.h"

#include "Experiments/HCUBE_Experiment.h"

#include "HCUBE_EvaluationPanel.h"
#include "HCUBE_NeuralNetworkPanel.h"

namespace HCUBE
{

    /*
    BEGIN_EVENT_TABLE(ViewIndividualFrame, wxFrame)
    EVT_PAINT(ViewIndividualFrame::paint)
    EVT_LEFT_UP(ViewIndividualFrame::handleMouseClick)
    END_EVENT_TABLE()
    */


    ViewIndividualFrame::ViewIndividualFrame(
        shared_ptr<Experiment> _experiment,
        shared_ptr<NEAT::GeneticIndividual> _individual,
        wxWindow* parent,
        int id,
        wxString title,
        wxPoint pos,
        wxSize size,
        int style
    )
            :
            ViewIndividualFrameTemplate(parent,id,title,pos,size,style),
            experiment(_experiment),
            individual(_individual)//,
            //outputBitmap(512,256)
    {
        evaluationPanel->setTarget(experiment,individual);
        networkPanel->setTarget(individual);
    }

    ViewIndividualFrame::~ViewIndividualFrame()
    {
    }

}

#endif
