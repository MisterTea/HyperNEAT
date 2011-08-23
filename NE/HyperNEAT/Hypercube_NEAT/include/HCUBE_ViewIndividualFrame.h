#ifndef HCUBE_VIEWINDIVIDUALFRAME_H_INCLUDED
#define HCUBE_VIEWINDIVIDUALFRAME_H_INCLUDED

#include "HCUBE_Defines.h"

#include "HCUBE_GUITemplate.h"

namespace HCUBE
{
    class ViewIndividualFrame : public ViewIndividualFrameTemplate
    {
    public:
    protected:
        shared_ptr<Experiment> experiment;
        shared_ptr<NEAT::GeneticIndividual> individual;

    public:
        ViewIndividualFrame(
            shared_ptr<Experiment> _experiment,
            shared_ptr<NEAT::GeneticIndividual> _individual,
            wxWindow* parent,
            int id = wxID_ANY,
            wxString title = wxEmptyString,
            wxPoint pos = wxDefaultPosition,
            wxSize size = wxSize( 640,480 ),
            int style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL
        );

        virtual ~ViewIndividualFrame();

    protected:
    };
}

#endif // HCUBE_VIEWINDIVIDUALFRAME_H_INCLUDED
