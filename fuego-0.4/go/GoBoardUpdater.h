//----------------------------------------------------------------------------
/** @file GoBoardUpdater.h */
//----------------------------------------------------------------------------

#ifndef GO_BOARDUPDATER_H
#define GO_BOARDUPDATER_H

#include <vector>

class GoBoard;
class SgNode;

//----------------------------------------------------------------------------

/** Updates a board to a node in a game tree.
    This could be a function, but is a class to reuse a locally used list for
    efficiency. The update is always done from scratch (initializing the board
    and applying all changes from the root node to the current node).
*/
class GoBoardUpdater
{
public:
    void Update(const SgNode* node, GoBoard& bd);

private:
    /** Local variable used in Update().
        Member variable for avoiding frequent new memory allocations.
    */
    std::vector<const SgNode*> m_nodes;
};

//----------------------------------------------------------------------------

#endif // GO_BOARDUPDATER_H
