#ifndef HCUBE_MAINAPP_H_INCLUDED
#define HCUBE_MAINAPP_H_INCLUDED

#include "HCUBE_Defines.h"

namespace HCUBE
{
    /**
    * MainApp is the wxwidgets application layer containing the HyperNEAT GUI
    */
    class MainApp : public wxApp
    {
    public:
    protected:

    public:
        virtual bool OnInit();
    protected:
    };
}

#endif // HCUBE_MAINAPP_H_INCLUDED
