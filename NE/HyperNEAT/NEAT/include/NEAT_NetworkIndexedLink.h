#ifndef NEAT_NETWORKINDEXEDLINK_H_INCLUDED
#define NEAT_NETWORKINDEXEDLINK_H_INCLUDED

namespace NEAT
{
    template<class Type>
    class NetworkIndexedLink
    {
    public:
        int fromNode,toNode;
        Type weight;

        NetworkIndexedLink()
                :
                fromNode(-1),
                toNode(-1),
                weight(Type(0))
        {}
    };
}


#endif // NEAT_NETWORKINDEXEDLINK_H_INCLUDED
