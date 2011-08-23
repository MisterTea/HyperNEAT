#ifndef HCUBE_NEURALNETWORKWEIGHTGRID_H_INCLUDED
#define HCUBE_NEURALNETWORKWEIGHTGRID_H_INCLUDED

// Define WX_GCH in order to support precompiled headers with GCC compiler.
// You have to create the header "wx_pch.h" and include all files needed
// for compile your gui inside it.
// Then, compile it and place the file "wx_pch.h.gch" into the same
// directory that "wx_pch.h".
#ifdef WX_GCH
#include <wx_pch.h>
#else
#include <wx/wx.h>
#endif

#include <wx/panel.h>
#include <wx/tglbtn.h>
#include <wx/button.h>
#include <wx/notebook.h>
#include <wx/grid.h>

#include "HCUBE_Defines.h"

namespace HCUBE
{

    class NeuralNetworkWeightGrid : public wxGrid
    {
    public:
    protected:
        shared_ptr<NEAT::GeneticIndividual> individual;

        bool readOnly;

    public:
        NeuralNetworkWeightGrid( wxWindow *parent,
                                 wxWindowID id,
                                 const wxPoint& pos = wxDefaultPosition,
                                 const wxSize& size = wxDefaultSize,
                                 long style = wxWANTS_CHARS,
                                 const wxString& name = wxGridNameStr );

        void setTarget(shared_ptr<NEAT::GeneticIndividual> _individual);

        inline void setReadOnly(bool value)
        {
            readOnly = value;

            EnableEditing(value);
        }

        inline bool isReadOnly()
        {
            return readOnly;
        }

        void cellChanged(wxGridEvent& event);

    protected:

        DECLARE_EVENT_TABLE()
    };

}

#endif // HCUBE_NEURALNETWORKWEIGHTGRID_H_INCLUDED
