#ifndef HCUBE_DEFINES_H_INCLUDED
#define HCUBE_DEFINES_H_INCLUDED

#ifdef _MSC_VER
#pragma warning (disable : 4251)
#pragma warning (disable : 4996)
#endif

/** THESE ARE THE ONLY INCLUDES FROM THIS PROJECT THAT SHOULD BE IN THIS FILE! **/
#include "HCUBE_STL.h"
#include "HCUBE_Boost.h"

#include "JGTL_StackMap.h"
#include "JGTL_Vector2.h"
#include "JGTL_Vector3.h"
#include "JGTL_Index3.h"
#include "JGTL_StringConverter.h"
#include "JGTL_LocatedException.h"
using namespace JGTL;

#ifdef _MSC_VER
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection procedures
//#define _INC_MALLOC      // exclude standard memory alloc procedures
#include <stdlib.h>
#include <crtdbg.h>
#endif
#endif

#ifdef HCUBE_NOGUI

class wxDC
{
public:
    void Clear()
    {}
    void DrawText(string s,int i,int j)
    {}
    void DrawText(char c,int i,int j)
    {}
    void DrawRectangle(int a,int b,int c,int d)
    {}
    void DrawLine(int a,int b,int c,int d)
    {}
    void DrawCircle(int a,int b,int c)
    {}
    void SetDeviceOrigin(int a,int b)
    {}
    Vector2<int> GetSize()
    {
        return Vector2<int>();
    }
};

#define STRING_TO_WXSTRING(STRING) (STRING)

#define CSTR_TO_WXSTRING(STRING) (STRING)

#define WXSTRING_TO_STRING(WXSTRING) (STRING)

#define _T(STRING) (STRING)

#else

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
# include "wx/wx.h"
#endif

#include "wx/spinctrl.h"
#include "wx/bookctrl.h"
#include "wx/bitmap.h"

#define STRING_TO_WXSTRING(STRING) \
(wxString( (STRING).c_str(),wxConvUTF8))

#define CSTR_TO_WXSTRING(STRING) \
(wxString( STRING,wxConvUTF8))

#define WXSTRING_TO_STRING(WXSTRING) \
((string)WXSTRING.mb_str(wxConvUTF8))

#endif

#include "NEAT.h"

#define HASBIT(NUMBER,BIT) (((NUMBER)&(1<<(BIT)))>0)
#define SETBIT(NUMBER,BIT) ((NUMBER)|=(1<<(BIT)))
#define UNSETBIT(NUMBER,BIT) ((NUMBER)&=~(1<<(BIT)))
#define TOGGLEBIT(NUMBER,BIT) ((NUMBER)^=(1<<(BIT)))

namespace HCUBE
{
	extern int NUM_THREADS;

	extern const int EXPERIMENT_BLOCKS_GUI;

    /** typedefs **/
    typedef unsigned int uint;
    typedef unsigned long long ulong;
    typedef unsigned char uchar;
    typedef unsigned short ushort;
    typedef JGTL::Vector3<int> Node;
    typedef map<Node,string > NodeMap;

    /** class Prototypes **/
    class Experiment;
    class ExperimentRun;

    class MainFrame;
    class ExperimentPanel;

    class UserEvaluationFrame;

    class EvaluationPanel;
    class NetworkPanel;
}

/** enums **/

enum ExperimentType
{
    EXPERIMENT_XOR=0,                                   //0
#ifdef EPLEX_INTERNAL
    EXPERIMENT_COXOR=1,                                 //1
    EXPERIMENT_MAKE_MAZE=2,                             //2
    EXPERIMENT_ROOM_GENERATOR=3,                        //3
    EXPERIMENT_CONNECTIVITY_GRAPH=4,                    //4
    EXPERIMENT_SIMPLE_IMAGE=5,                          //5
#endif
    EXPERIMENT_TIC_TAC_TOE=6,                           //6
#ifdef EPLEX_INTERNAL
    EXPERIMENT_ROBOT_ARM=7,                             //7
    EXPERIMENT_ROBOT_ARM_LATTICE=8,                     //8
#endif
    EXPERIMENT_TIC_TAC_TOE_GAME=9,                      //9
    EXPERIMENT_TIC_TAC_TOE_NO_GEOM_GAME=10,             //10
    EXPERIMENT_FIND_POINT_EXPERIMENT=11,                //11
    EXPERIMENT_FIND_CLUSTER_EXPERIMENT=12,              //12
    EXPERIMENT_FIND_CLUSTER_NO_GEOM_EXPERIMENT=13,      //13
#ifdef EPLEX_INTERNAL
    EXPERIMENT_SPATIAL_CPPN=14,                         //14
#endif
    EXPERIMENT_CHECKERS=15,                             //15
    EXPERIMENT_CHECKERS_NO_GEOM=16,                     //16
#ifdef EPLEX_INTERNAL
    EXPERIMENT_COCHECKERS=17,                           //17
    EXPERIMENT_OTHELLO=18,                              //18
    EXPERIMENT_OTHELLO_CO=19,                           //19
    EXPERIMENT_SCALABLE_CHECKERS=20,                    //20
    EXPERIMENT_CHECKERS_FOGEL=21,                       //21
    EXPERIMENT_CHECKERS_PRUNING=22,                     //22
    EXPERIMENT_SPATIAL_CASINO=23,                       //23
#endif
    EXPERIMENT_CHECKERS_ORIGINAL_FOGEL=24,              //24
#ifdef EPLEX_INTERNAL
    EXPERIMENT_IMAGE_COMPRESSION=25,                    //25
    EXPERIMENT_FIND_CLUSTER_BP=26,						//26
    EXPERIMENT_GO=27,									//27
#endif
    EXPERIMENT_CHECKERS_SUBSTRATE_GEOM=28,				//28
#ifdef EPLEX_INTERNAL
    EXPERIMENT_BINARY_COMPRESSION=29,                   //29
#endif
    EXPERIMENT_END
};

#endif // HCUBE_DEFINES_H_INCLUDED
