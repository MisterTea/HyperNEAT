//----------------------------------------------------------------------------
/** @file SgBookBuilder.hpp
 */
//----------------------------------------------------------------------------

#ifndef SG_BOOKBUILDER_HPP
#define SG_BOOKBUILDER_HPP

#include "SgSystem.h"
#include "SgMove.h"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <vector>

//----------------------------------------------------------------------------

/** @defgroup openingbook Automatic Opening Book Construction
    Game independent Opening Book Construction.

    Code is based on Thomas R. Lincke's paper "Strategies for the
    Automatic Construction of Opening Books" published in 2001.
    
    We make the following adjustments:
    - Neither side is assumed to be the book player, so the expansion
      formula is identical for all nodes (see page 80 of the paper). In other
      words, both sides can play sub-optimal moves.
    - A single node for each state is stored, such that transpositions
      are not re-computed. Hence the book forms a DAG of states, not a tree.
    - Progressive widening is used on internal nodes to restrict the 
      search initially. 

    We also think there is a typo with respect to the formula of epo_i on
    page 80. Namely, since p_i is the negamax of p_{s_j}s, then we should
    sum the values to find the distance from optimal, not subtract. That is,
    we use epo_i = 1 + min(s_j) (epb_{s_j} + alpha*(p_i + p_{s_j}) instead.
*/

//------------------------------------------------------------------------

/** Priority of newly created leaves. */
extern const float LEAF_PRIORITY;

//------------------------------------------------------------------------

//----------------------------------------------------------------------------

/** State in the Opening Book.
    @ingroup openingbook
 */
class SgBookNode
{
public:
    /** Heuristic value of this state. */
    float m_heurValue;

    /** Minmax value of this state. */
    float m_value;

    /** Expansion priority. */
    float m_priority;

    /** Number of times this node was explored. */
    unsigned m_count;
    
    //------------------------------------------------------------------------

    SgBookNode();

    SgBookNode(float heuristicValue);

    SgBookNode(const std::string& str);

    /** Returns true if this node is a leaf in the opening book, ie,
        its count is zero. */
    bool IsLeaf() const;

    /** Returns true if propagated value is a win or a loss. */
    bool IsTerminal() const;

    /** Increment the node's counter. */
    void IncrementCount();

    /** Outputs node in string form. */
    std::string ToString() const;

private:
    void FromString(const std::string& str);
};

inline SgBookNode::SgBookNode()
{
}

inline SgBookNode::SgBookNode(float heuristicValue)
    : m_heurValue(heuristicValue),
      m_value(heuristicValue),
      m_priority(LEAF_PRIORITY),
      m_count(0)
{
}

inline SgBookNode::SgBookNode(const std::string& str)
{
    FromString(str);
}

inline void SgBookNode::IncrementCount()
{
    m_count++;
}

/** Extends standard stream output operator for SgBookNodes. */
inline std::ostream& operator<<(std::ostream& os, const SgBookNode& node)
{
    os << node.ToString();
    return os;
}

//----------------------------------------------------------------------------

/** @page bookrefresh Book Refresh
    @ingroup openingbook

    Due to transpositions, it is possible that a node's value changes,
    but because the node has not been revisited yet the information is
    not passed to its parent. Refreshing the book forces these
    propagations.

    SgBookBuilder::Refresh() computes the correct propagation value for
    all internal nodes given the current set of leaf nodes. A node in
    which SgBookNode::IsLeaf() is true is treated as a leaf even
    if it has children in the book (ie, children from transpositions)
*/

//----------------------------------------------------------------------------

/** Base class for automated book building.

    @ingroup openingbook
*/
class SgBookBuilder
{
public:
    SgBookBuilder();

    virtual ~SgBookBuilder();

    //---------------------------------------------------------------------

    /** Expands the book by expanding numExpansions leaves. */
    void Expand(int numExpansions);

    /** Ensures each node in each line has at least the given number
        of expansions. */
    void Cover(int requiredExpansions,
               const std::vector< std::vector<SgMove> >& lines);

    /** Propagates leaf values up through the entire tree.  
        @ref bookrefresh. */
    void Refresh();

    /** Performs widening on all internal nodes that require it. Use
        this after increasing ExpandWidth() or decreasing
        ExpandThreshold() on an already existing book to update all
        the internal nodes with the new required width. Will do
        nothing unless parameters were changed accordingly.
        
        Does not propagate values up tree, run Refresh() afterwards to
        do so. */
    void IncreaseWidth();

    //---------------------------------------------------------------------    

    float ComputePriority(const SgBookNode& parent, const float childValue,
                          const float childPriority) const;

    /** Returns the evaluation from other player's perspective. */
    virtual float InverseEval(float eval) const = 0;

    /** Returns true if the eval is a loss. */
    virtual bool IsLoss(float eval) const = 0;

    /** Returns the value of the state according this node.
        Ie, takes into account swap moves, etc. */
    virtual float Value(const SgBookNode& node) const = 0;

    //---------------------------------------------------------------------    

    /** The parameter alpha controls state expansion (big values give
        rise to deeper lines, while small values perform like a
        BFS). */
    float Alpha() const;

    /** See Alpha() */
    void SetAlpha(float alpha);

    /** Expand only the top ExpandWidth() children of a node
        initially, and after every ExpansionThreshold() visits add
        ExpandWidth() more children. */
    bool UseWidening() const;

    /** See UseWidening() */
    void SetUseWidening(bool flag);
    
    /** See UseWidening() */
    std::size_t ExpandWidth() const;

    /** See UseWidening() */
    void SetExpandWidth(std::size_t width);

    /** See UseWidening() */
    std::size_t ExpandThreshold() const;

    /** See UseWidening() */
    void SetExpandThreshold(std::size_t threshold);

protected:
    /** See Alpha() */
    float m_alpha;

    /** See UseWidening() */
    bool m_use_widening;

    /** See UseWidening() */
    std::size_t m_expand_width;

    /** See UseWidening() */
    std::size_t m_expand_threshold;
    
    /** Number of iterations after which the db is flushed to disk. */
    std::size_t m_flush_iterations;

    //------------------------------------------------------------------------

    virtual void PrintMessage(std::string msg) = 0;

    virtual void PlayMove(SgMove move) = 0;

    virtual void UndoMove(SgMove move) = 0;

    /** Reads node. Returns false if node does not exist. */
    virtual bool GetNode(SgBookNode& node) const = 0;

    /** Writes node. */
    virtual void WriteNode(const SgBookNode& node) = 0;

    virtual void FlushBook() = 0;

    /** If current state does not exist, evaluate it and store in the
        book. */
    virtual void EnsureRootExists() = 0;

    /** Generates the set of moves to use in the book for this state. */
    virtual bool GenerateMoves(std::vector<SgMove>& moves, float& value) = 0;

    /** Returns all legal moves; should be a superset of those moves 
        returned by GenerateMoves() */
    virtual void GetAllLegalMoves(std::vector<SgMove>& moves) = 0;

    /** Evaluate the children of the current state, return the values
        in a vector of pairs. */
    virtual void EvaluateChildren(const std::vector<SgMove>& childrenToDo,
                    std::vector<std::pair<SgMove, float> >& scores) = 0;

    /** Hook function: called before any work is done. 
        Default implementation does nothing. */
    virtual void Init();

    /** Hook function: called after all work is complete. 
        Default implementation does nothing. */
    virtual void Fini();

    /** Hook function: called at start of iteration.
        Default implementation does nothing. */
    virtual void StartIteration();
    
    /** Hook function: called at end of iteration. 
        Default implementation does nothing. */
    virtual void EndIteration();

    virtual void BeforeEvaluateChildren();

    virtual void AfterEvaluateChildren();

    virtual void ClearAllVisited() = 0;

    virtual void MarkAsVisited() = 0;
    
    virtual bool HasBeenVisited() = 0;

private:
    std::size_t m_num_evals;

    std::size_t m_num_widenings;

    std::size_t m_value_updates;

    std::size_t m_priority_updates;

    std::size_t m_internal_nodes;

    std::size_t m_leaf_nodes;

    std::size_t m_terminal_nodes;

    //---------------------------------------------------------------------

    std::size_t NumChildren(const std::vector<SgMove>& legal);

    void UpdateValue(SgBookNode& node, const std::vector<SgMove>& legal);

    void UpdateValue(SgBookNode& node);

    SgMove UpdatePriority(SgBookNode& node);

    void DoExpansion(std::vector<SgMove>& pv);

    bool Refresh(bool root);

    void IncreaseWidth(bool root);
    
    bool ExpandChildren(std::size_t count);
};

//----------------------------------------------------------------------------

inline float SgBookBuilder::Alpha() const
{
    return m_alpha;
}

inline void SgBookBuilder::SetAlpha(float alpha)
{
    m_alpha = alpha;
}

inline bool SgBookBuilder::UseWidening() const
{
    return m_use_widening;
}

inline void SgBookBuilder::SetUseWidening(bool flag)
{
    m_use_widening = flag;
}

inline std::size_t SgBookBuilder::ExpandWidth() const
{
    return m_expand_width;
}

inline void SgBookBuilder::SetExpandWidth(std::size_t width)
{
    m_expand_width = width;
}

inline std::size_t SgBookBuilder::ExpandThreshold() const
{
    return m_expand_threshold;
}

inline void SgBookBuilder::SetExpandThreshold(std::size_t threshold)
{
    m_expand_threshold = threshold;
}

//----------------------------------------------------------------------------

#endif // SG_BOOKBUILDER_HPP
