#include "HCUBE_Defines.h"

#include "HCUBE_ExperimentRun.h"

#include <boost/python.hpp>
using namespace boost::python;

extern int HyperNEAT_main(int argc,char **argv);

int PythonMain(python::list args)
{
    int argc = python::len(args);

    char** argv = new char*[argc];

    for(int a=0;a<argc;a++)
    {
        argv[a] = python::extract<char*>(args[a]);
    }

    return HyperNEAT_main(argc,argv);
}

BOOST_PYTHON_MODULE(HCUBE)
{
    def("HyperNEAT_main", PythonMain);
}
