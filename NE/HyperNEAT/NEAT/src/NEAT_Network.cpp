#include "NEAT_Defines.h"

#include "NEAT_Network.h"

#include "NEAT_Random.h"

#include "NEAT_GeneticIndividual.h"

#include "NEAT_Globals.h"

namespace NEAT
{
    template class Network<float>; // explicit instantiation
    template class Network<double>; // explicit instantiation
}
