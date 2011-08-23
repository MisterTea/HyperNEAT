//----------------------------------------------------------------------------
/** @file SgNode.h
    Trees of nodes with properties.

    Module SgNode defines trees and operations on those trees. The abstract
    data type 'Node' represents a node in a tree; each node has a list of
    properties associated with it. Typical properties are moves, territory,
    values, and comments.

    Each node in the tree corresponds to a board position. The move leading
    to a position is stored in that node. The root node is the position where
    no move has as yet been played. Root nodes cannot have any brothers.
    There is no concept of "current position", as each node encodes both the
    tree it is in and its relation to other nodes in that tree. (The current
    position in the game is handled by GoGame.)

    The special node pointer 0 has no father, no sons, no brother,
    and no properties.
    It is usually returned from procedures to indicate failure.
    Because the tree is usually traversed depth-first, it is safe to assume
    that it is faster to go to the leftmost than to the rightmost son, and
    faster to go to the right brother than to the left brother.
*/
//----------------------------------------------------------------------------

#ifndef SG_NODE_H
#define SG_NODE_H

#include <string>
#include "SgProp.h"
#include "SgPointSet.h"
#include "SgVector.h"

//----------------------------------------------------------------------------

/** Node in tree.
    Procedures for moving around in the tree return 0 if the desti-
    nation doesn't exist.
    References to "left" and "right" picture the tree with the root at the
    top and the main line of play going down the left side.
*/
class SgNode
{
public:
    enum Direction {
        PREVIOUS,
        NEXT,
        NEXT_RIGHTMOST,
        PREV_DEPTHFIRST,
        NEXT_DEPTHFIRST,
        PREV_TERMINAL,
        NEXT_TERMINAL,
        PREV_BRANCH,
        NEXT_BRANCH,
        LEFT_BROTHER,
        RIGHT_BROTHER,
        MAIN_BRANCH,
        START_OF_GAME,
        END_OF_GAME
    };

    SgNode();

    ~SgNode();

    /** Return a newly allocated copy of this node and its subtree. */
    SgNode* CopyTree() const;

    /**  @todo: distinguish SgVector<SgPoint>, SgVector<int> etc. */
    // @todo add const& version when conversion is done.
    // const SgVector<SgPoint>& VectorProp(SgPropID prop) const;
    SgVector<SgPoint> VectorProp(SgPropID prop) const;

    bool HasFather() const
    {
        return (m_father != 0);
    }

    bool IsRoot() const
    {
        return (m_father == 0);
    }

    bool HasLeftBrother() const;

    bool HasRightBrother() const
    {
        return (m_brother != 0);
    }

    bool HasSon() const
    {
        return (m_son != 0);
    }

    /** HasLeftBrother(node) OR HasRightBrother(node) */
    bool HasBrother() const
    {
        return HasLeftBrother() || HasRightBrother();
    }

    /** True of the node is on the main branch of the tree, that is
        none of its ancestors has a left brother.
    */
    bool IsOnMain() const;

    /** NOT HasSon(node) */
    bool IsTerminal() const
    {
        return (m_son == 0);
    }

    /** NumSons(node) >= 2 */
    bool IsBranchPoint() const
    {
        return (m_son && m_son->m_brother);
    }

    int NumSons() const;

    int NumLeftBrothers() const;

    SgNode* Root() const;

    SgNode* Father() const
    {
        return m_father;
    }

    SgNode* LeftBrother() const;

    SgNode* RightBrother() const
    {
        return m_brother;
    }

    SgNode* LeftMostSon() const
    {
        return m_son;
    }

    SgNode* RightMostSon() const;

    /** Depth-first traversal of the tree. */
    SgNode* NextDepthFirst() const;

    /** Inverse operation of NextDepthFirst. */
    SgNode* PrevDepthFirst() const;

    /** Return the next node in the given direction.
        Returns 0 if there is no such node.
    */
    SgNode* NodeInDirection(Direction dir) const;

    /** Return whether this node contains a property that matches the given
        text.
        Doesn't handle text representing special properties.
    */
    bool ContainsText(const std::string& findText);

    /** Returns all nodes on path from this node up to the root. */
    void PathToRoot(SgVectorOf<SgNode>* path) const;

    /** Find the closest common ancestor of the two nodes.
        Returns the path from this node to 'node': how many times
        to go one node back, and a vector of the nodes to execute from
        there (excluding the common ancestor).
    */
    void ShortestPathTo(SgNode* node, int* numBack,
                        SgVectorOf<SgNode>* path) const;

    /** Promote this node to first son.
        Moves all its left brothers one position to the right.
    */
    void PromoteNode();

    /** The tree above this node is changed such that this node is on
        the main path.
        (first in depth-first search).
    */
    void PromotePath();

    /** Deletes all nodes below this node, but not this node itself. */
    void DeleteSubtree();

    /** Deletes everything below the current node except the main branch. */
    void DeleteBranches();

    /** Deletes the tree that this node is part of.
        Same as:
        @verbatim
        SgNode* root = Root();
        root->DeleteSubtree();
        delete root;
        @endverbatim
    */
    void DeleteTree();

    SgNode* NewFather();

    SgNode* NewRightBrother();

    SgNode* NewLeftMostSon();

    /** Insert a node at the appropriate place in the tree. */
    SgNode* NewRightMostSon();

    /** Append this tree to '*n'. */
    void AppendTo(SgNode* n);

    /** Add a new node and add all trees in 'roots' as subtrees of that
        node.
    */
    static SgNode* LinkTrees(const SgVectorOf<SgNode>& roots);

    SgPropList& Props()
    {
        return m_props;
    }

    const SgPropList& Props() const
    {
        return m_props;
    }

    void Add(const SgProp* prop)
    {
        // Check for root properties in root or one below root (linked nodes
        // in game collection).
        SG_ASSERT(! prop->Flag(SG_PROPCLASS_ROOT) || ! HasFather()
                  || ! Father()->HasFather());
        m_props.Add(prop);
    }

    SgProp* Get(SgPropID id) const
    {
        return m_props.Get(id);
    }

    /** HasProp also handles abstract node properties like SG_PROP_TERMINAL
        and SG_PROP_BRANCH, while Get only returns real properties.
    */
    bool HasProp(SgPropID id) const;

    /** has SG_PROP_MOVE_BLACK or SG_PROP_MOVE_WHITE */
    bool HasNodeMove() const;

    /** Player of move.
        REQUIRES: HasNodeMove()
    */
    SgBlackWhite NodePlayer() const;

    SgPoint NodeMove() const;

    /** Return the most recent node that has the given property, starting
        backwards from this node.
        Return this node if it has the wanted property; return the root of the
        tree if no node with that property was found.
    */
    SgNode* TopProp(SgPropID id) const;

    /** Return the value of the given property at this node.
        Or 0 if there is no such property. The property must be of class
        SgPropInt (or a derived class).
    */
    int GetIntProp(SgPropID id) const;

    /** return if property exists. If true, return its value in *value */
    bool GetIntProp(SgPropID id, int* value) const;

    /** Value of the given property at this node.
        Returns 0 if there is no such property.
        The property must be of class SgPropReal.
    */
    double GetRealProp(SgPropID id) const;

    /** Set the value of the given property at this node to 'value'.
        Create such a property if it doesn't exist yet. The property must be
        of class SgPropInt (or a derived class).
    */
    void SetIntProp(SgPropID id, int value);

    /** Set the value of the given property at this node.
        Create  such a property if it doesn't exist yet.
        The property must be of class SgPropReal.
        @param  id
        @param value
        @param precision Precision; 0 means default precision (6)
    */
    void SetRealProp(SgPropID id, double value, int precision = 0);

    /** Set the value of the given property at this node to 'value'.
        Create such a property if it doesn't exist yet. The property must be
        of class SgPropText (or a derived class).
    */
    void SetStringProp(SgPropID id, const std::string& value);

    /** Return the value of the given property at this node.
        Or 0 if there is no such property. The property must be of class
        SgPropText (or a derived class).
    */
    bool GetStringProp(SgPropID id, std::string* value) const;

    /** Set the value of the given property at this node to 'value'.
        Create such a property if it doesn't exist yet. The property must be
        of class SgPropPointList (or a derived class).
    */
    void SetListProp(SgPropID id, const SgVector<SgPoint>& value);
    void SetListProp(SgPropID id, const SgPointSet& value);

    /** Add comment to existing SG_PROP_COMMENT of this node, or create a new
        SG_PROP_COMMENT with this text.
    */
    void AddComment(const std::string& comment);

    /** If it exists, copy the property with the given 'id' from 'sourceNode'
        to this node, and return it.
        If the property doesn't exist, return 0.
    */
    SgProp* CopyPropFrom(const SgNode& sourceNode, SgPropID id);

    /** Copy all properties from 'sourceNode' to this node. */
    void CopyAllPropsFrom(const SgNode& sourceNode);

    /** Add a move property to this node with 'move' played by 'player'.
        @return the property added.
        @warning Only for games which use SgMoveProp
        @todo Too game dependent for a member of SgNode, move somewhere else
    */
    SgProp* AddMoveProp(SgMove move, SgBlackWhite player);

    /** Return the player, if explicitely set.
        REQUIRES: Get(SG_PROP_PLAYER)
    */
    SgBlackWhite Player() const;

    /** Count the nodes in this subtree and sets SG_PROP_NUM_NODES for each
        interior node in the subtree that has at least two sons, plus for this
        node if 'fSetPropOnThisNode' is set.
        Return the number of nodes in the subtree, including this node.
    */
    int CountNodes(bool fSetPropOnThisNode);

    static void CopySubtree(const SgNode* node, SgNode* copy);

#ifndef NDEBUG
    /** Total number of nodes allocated, still in use. */
    static void GetStatistics(int* numAlloc, int* numUsed);
#endif

    static void MemCheck();

    /** Location of node in tree.
        This function builds a unique string from the location of a node
        in the tree. It is a list of integers separated by dots, each integer
        represents the child index (counting from 1) for each node in
        the path from the root to the given node.
        For example
        @verbatim
        root -- node1 -- node2
             |
             +- node3

        root  "1"
        node1 "1.1"
        node2 "1.1.1"
        node3 "1.2"
        @endverbatim
        A null node pointer has the TreeIndex "NIL".
    */
    static std::string TreeIndex(const SgNode* node);

private:
    //friend class SgNodeIterator;

    SgNode* m_son;

    SgNode* m_father;

    SgNode* m_brother;

    SgPropList m_props;

    bool m_marked;

    void LinkWithBrother(SgNode* node);

    void Mark()
    {
        m_marked = true;
    }

    void Unmark()
    {
        m_marked = false;
    }

    bool IsMarked() const
    {
        return m_marked;
    }

    /** Not implemented. */
    SgNode(const SgNode&);

    /** Not implemented. */
    SgNode& operator=(const SgNode&);

#ifndef NDEBUG
    static int s_alloc;

    static int s_free;
#endif
};

//----------------------------------------------------------------------------

/** Iterator for iterating through all the sons of a SgNode */
class SgSonNodeIterator
{
public:
    SgSonNodeIterator(SgNode* node)
        : m_nextNode(node->LeftMostSon())
    { }

    void operator++()
    {
        m_nextNode = m_nextNode->RightBrother();
    }

    SgNode* operator*() const
    {
        SG_ASSERT(m_nextNode);
        return m_nextNode;
    }

    operator bool() const
    {
        return m_nextNode != 0;
    }

private:
    SgNode* m_nextNode;

    /** Not implemented. */
    SgSonNodeIterator(const SgSonNodeIterator&);

    /** Not implemented. */
    SgSonNodeIterator& operator=(const SgSonNodeIterator&);
};

//----------------------------------------------------------------------------

/** Iterator for iterating through all the sons of a Node */
class SgSonNodeConstIterator
{
public:
    SgSonNodeConstIterator(const SgNode* node)
        : m_nextNode(node->LeftMostSon())
    { }

    void operator++()
    {
        m_nextNode = m_nextNode->RightBrother();
    }

    const SgNode* operator*() const
    {
        SG_ASSERT(m_nextNode);
        return m_nextNode;
    }

    operator bool() const
    {
        return m_nextNode != 0;
    }

private:
    SgNode* m_nextNode;

    /** Not implemented. */
    SgSonNodeConstIterator(const SgSonNodeConstIterator&);

    /** Not implemented. */
    SgSonNodeConstIterator& operator=(const SgSonNodeConstIterator&);
};

//----------------------------------------------------------------------------

/** Iterator for iterating through all nodes in subtree. */
class SgNodeIterator
{
public:
    /** Create an iterator for iterating through all nodes in the subtree
        of 'rootOfSubtree', including the node itself.
        If 'preOrder' (default), return internal nodes before their sons; if
        'postOrder', then return the leaves before the internal nodes.
    */
    SgNodeIterator(SgNode* rootOfSubtree, bool postOrder = false);

    /** Abort the iteration.
        The next call to operator bool will return false.
    */
    void Abort()
    {
        m_nextNode = 0;
    }

    void operator++()
    {
        SG_ASSERT(m_nextNode);
        Next();
    }

    SgNode* operator*() const
    {
        SG_ASSERT(m_nextNode);
        return m_nextNode;
    };

    operator bool() const
    {
        return m_nextNode != 0;
    }

private:
    /** Find next node in the tree. Return false when done with all nodes. */
    bool Next();

    bool m_postOrder;

    SgNode* const m_rootOfSubtree;

    SgNode* m_nextNode;

    /** Not implemented. */
    SgNodeIterator(const SgNodeIterator&);

    /** Not implemented. */
    SgNodeIterator& operator=(const SgNodeIterator&);
};

//----------------------------------------------------------------------------

/** Iterator for iterating through all nodes in subtree. */
class SgNodeConstIterator
{
public:
    /** Create an iterator for iterating through all nodes in the subtree
        of 'rootOfSubtree', including the node itself.
        If 'preOrder' (default), return internal nodes before their sons; if
        'postOrder', then return the leaves before the internal nodes.
    */
    SgNodeConstIterator(const SgNode* rootOfSubtree, bool postOrder = false);

    /** Find next node in the tree. Return false when done with all nodes. */
    bool Next();

    /** Abort the iteration.
        The next call to operator bool will return false.
    */
    void Abort()
    {
        m_nextNode = 0;
    }

    void operator++()
    {
        SG_ASSERT(m_nextNode);
        Next();
    }

    const SgNode* operator*() const
    {
        SG_ASSERT(m_nextNode);
        return m_nextNode;
    };

    operator bool() const
    {
        return m_nextNode != 0;
    }

private:
    bool m_postOrder;

    const SgNode* const m_rootOfSubtree;

    const SgNode* m_nextNode;

    /** Not implemented. */
    SgNodeConstIterator(const SgNodeConstIterator&);

    /** Not implemented. */
    SgNodeConstIterator& operator=(const SgNodeConstIterator&);
};

//----------------------------------------------------------------------------

#endif // SG_NODE_H
