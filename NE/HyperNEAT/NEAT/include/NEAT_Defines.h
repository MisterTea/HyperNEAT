#ifndef NEAT_DEFINES_H_INCLUDED
#define NEAT_DEFINES_H_INCLUDED

#define EPLEX_INTERNAL

#ifdef _MSC_VER
#pragma warning (disable : 4251)
#pragma warning (disable : 4996)
#endif

#include "tinyxmlplus.h"

#include "NEAT_STL.h"

#include "JGTL_LocatedException.h"
#include "JGTL_Vector2.h"
#include "JGTL_Vector3.h"
#include "JGTL_StringConverter.h"
#include "JGTL_StackMap.h"
#include "JGTL_Index3.h"
using namespace JGTL;

#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/progress.hpp>
#include <boost/thread/mutex.hpp>

using namespace boost;

#ifdef _MSC_VER
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection procedures
//#define _INC_MALLOC      // exclude standard memory alloc procedures
#include <stdlib.h>
#include <crtdbg.h>
#endif
#endif

#if 1 //Don't bother with DLL anymore, too much work
#define NEAT_DLL_EXPORT
#else
#if defined( __CYGWIN__ )
#  ifdef BUILD_NEAT_DLL
#    define NEAT_DLL_EXPORT
#  else
#    define NEAT_DLL_EXPORT
#  endif
#elif defined( _WIN32 )
#  ifdef BUILD_NEAT_DLL
#    define NEAT_DLL_EXPORT __declspec( dllexport )
#  else
#    define NEAT_DLL_EXPORT __declspec( dllimport )
#  endif
#else
#  ifdef BUILD_NEAT_DLL
#    define NEAT_DLL_EXPORT
#  else
#    define NEAT_DLL_EXPORT
#  endif
#endif
#endif

#define EPLEX_INTERNAL

namespace NEAT
{
    /** typedefs **/
    typedef unsigned int uint;
    typedef unsigned long long ulong;
    typedef unsigned char uchar;
    typedef unsigned short ushort;

    typedef JGTL::Vector3<int> Node;
    typedef map<Node,string > NodeMap;

	class NetworkOutputNode;
    class NetworkNode;
    class NetworkLink;

    class GeneticIndividual;
    class GeneticGeneration;

    class GeneticLinkGene;
    class GeneticNodeGene;

#ifdef EPLEX_INTERNAL
    class CoEvoExperiment;
#endif

}

#endif // NEAT_DEFINES_H_INCLUDED
