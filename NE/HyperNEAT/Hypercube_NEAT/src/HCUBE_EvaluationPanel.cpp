#include "HCUBE_Defines.h"

#ifndef HCUBE_NOGUI

#include "HCUBE_EvaluationPanel.h"

#include "Experiments/HCUBE_Experiment.h"

#include "wx/dcmemory.h"

namespace HCUBE
{

    BEGIN_EVENT_TABLE(EvaluationPanel, wxPanel)
        EVT_PAINT(EvaluationPanel::paint)
        EVT_LEFT_UP(EvaluationPanel::handleMouseClick)
        EVT_RIGHT_UP(EvaluationPanel::handleMouseClick)
        EVT_MIDDLE_UP(EvaluationPanel::handleMouseClick)
        EVT_MOTION(EvaluationPanel::handleMouseMotion)
    END_EVENT_TABLE()

    EvaluationPanel::EvaluationPanel( wxWindow* parent, int id, wxPoint pos, wxSize size, int style )
            :
            wxPanel( parent, id, pos, size, style ),
            bitmapLocked(false)
    {
        this->SetSizeHints( wxDefaultSize, wxDefaultSize );
        wxBoxSizer* bSizer6;
        bSizer6 = new wxBoxSizer( wxVERTICAL );
        outputBitmap = new wxBitmap(size.x,size.y);
        SetSizer(bSizer6);
        Layout();
    }

    /*EvaluationPanel::EvaluationPanel(
        Experiment *_experiment,
        shared_ptr<NEAT::GeneticIndividual> _individual,
        wxWindow* _parent,
        const wxPoint& pos,
        const wxSize& size
    )
            :
            wxPanel(_parent, -1, pos, size),
            individual(_individual),
            experiment(_experiment),
            outputBitmap(512,256)
    {
        cout << "Creating Evaluation Panel...";
        // Create a memory DC
        wxMemoryDC temp_dc;

        temp_dc.SelectObject(outputBitmap);

        experiment->createIndividualImage(temp_dc,individual);
        cout << "done!\n";
    }*/

    void EvaluationPanel::setTarget(shared_ptr<Experiment> _experiment,shared_ptr<NEAT::GeneticIndividual> _individual)
    {
        if (bitmapLocked)
        {
            cout << "SETTING BACKUPINDIVIDUAL!\n";
            backupIndividual = _individual;
            backupExperiment = _experiment;
            return;
        }
        else
        {
            cout << "PAINTING TO BITMAP!\n";
            experiment = _experiment;
            individual = _individual;
        }

        bitmapLocked=true;
        do
        {
            if (backupIndividual)
            {
                individual = backupIndividual;
                experiment = backupExperiment;
                backupIndividual.reset();
            }

            // Create a memory DC
            wxMemoryDC temp_dc;
            cout << GetSize().x << '/' << GetSize().y << endl;

            if (
                !outputBitmap->IsOk() ||
                outputBitmap->GetWidth()!=GetSize().x ||
                outputBitmap->GetHeight()!=GetSize().y
            )
            {
                delete outputBitmap;
                outputBitmap = new wxBitmap(GetSize().x,GetSize().y);
            }

            temp_dc.SelectObject(*outputBitmap);

            experiment->createIndividualImage(temp_dc,individual);
        }
        while (backupIndividual);

        bitmapLocked=false;
        Refresh();
    }

    void EvaluationPanel::paint(wxPaintEvent& event)
    {
        if (!bitmapLocked)
        {
            bitmapLocked=true;
            cout << "PAINTING BITMAP TO SCREEN!\n";
            wxPaintDC dc(this);

            if (outputBitmap->IsOk())
            {
                dc.DrawBitmap(*outputBitmap,0,0,true);
            }
            else
            {
                cout << "BITMAP IS NOT OK TO DRAW!\n";
            }
            bitmapLocked=false;
        }
    }

    void EvaluationPanel::handleMouseClick(wxMouseEvent& event)
    {
        //cout << event.GetX() << ',' << event.GetY() << endl;

        if (event.GetX()>=outputBitmap->GetWidth() || event.GetY()>=outputBitmap->GetHeight())
        {
            //out of the bitmap, ignore
            return;
        }

        if (!bitmapLocked)
        {
            if (experiment)
            {
                experiment->setLastIndividualSeen(individual);
                wxSize size(outputBitmap->GetWidth(),outputBitmap->GetHeight());
                if (experiment->handleMousePress(event,size))
                {
                    wxMemoryDC temp_dc;
                    temp_dc.SelectObject(*outputBitmap);

                    experiment->createIndividualImage(temp_dc,individual);

                    this->Refresh();
                }
            }
        }

        if (!event.ShouldPropagate()) event.ResumePropagation(1); //this event should propagate

        event.Skip(); //Allow this event to propagate
    }

    void EvaluationPanel::handleMouseMotion(wxMouseEvent& event)
    {
        //cout << event.GetX() << ',' << event.GetY() << endl;

        if (event.GetX()>=outputBitmap->GetWidth() || event.GetY()>=outputBitmap->GetHeight())
        {
            //out of the bitmap, ignore
            return;
        }

        if (!bitmapLocked)
        {
            wxMemoryDC temp_dc;
            temp_dc.SelectObject(*outputBitmap);

            if (experiment && experiment->handleMouseMotion(event,temp_dc,individual))
                this->Refresh();
        }

        if (!event.ShouldPropagate()) event.ResumePropagation(1); //this event should propagate

        event.Skip(); //Allow this event to propagate
    }
}

#endif
