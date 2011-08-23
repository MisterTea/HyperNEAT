#include "HCUBE_Defines.h"

#ifndef HCUBE_NOGUI

#include "HCUBE_MainApp.h"

#include "HCUBE_MainFrame.h"

namespace HCUBE
{
    bool MainApp::OnInit()
    {
        wxInitAllImageHandlers();

        MainFrame *frame = new MainFrame( NULL );

        frame->Show(TRUE);
        SetTopWindow(frame);		
		
        return true;
    }
}

#endif
