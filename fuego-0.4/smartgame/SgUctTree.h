//----------------------------------------------------------------------------
/** @file SgUctTree.h
    Class SgUctTree and strongly related classes.
*/
//----------------------------------------------------------------------------

#ifndef SG_UCTTREE_H
#define SG_UCTTREE_H

#include <stack>
#include <boost/shared_ptr.hpp>
#include "SgMove.h"
#include "SgStatistics.h"

class SgTimer;

//----------------------------------------------------------------------------

typedef SgStatisticsBase<float,std::size_t> SgUctStatisticsBase;

typedef SgStatisticsBase<volatile float,volatile std::size_t>
    SgUctStatisticsBaseVolatile;

//----------------------------------------------------------------------------

/** Used for node creation. */
struct SgMoveInfo
{
    /** Move for the child. */
    SgMove m_move;

    /** Value of node after node is created. 
        Value is from child's perspective, so the value stored here
        must be the inverse of the evaluation from the parent's
        perspective. 
    */
    float m_value;

    /** Count of node after node is created. */
    std::size_t m_count;

    /** Rave value of move after node is created from viewpoint of
        parent node.
        Value should not be inverted to child's perspective.
    */
    float m_raveValue;

    /** Rave count of move after node is created. */
    float m_raveCount;

    SgMoveInfo();

    SgMoveInfo(SgMove move);

    SgMoveInfo(SgMove move, float value, std::size_t count,
               float raveValue, float raveCount);
};

inline SgMoveInfo::SgMoveInfo()
    : m_value(0.0),
      m_count(0),
      m_raveValue(0.0),
      m_raveCount(0.0)
{
}

inline SgMoveInfo::SgMoveInfo(SgMove move)
    : m_move(move),
      m_value(0.0),
      m_count(0),
      m_raveValue(0.0),
      m_raveCount(0.0)
{
}

inline SgMoveInfo::SgMoveInfo(SgMove move, float value, std::size_t count,
                              float raveValue, float raveCount)
    : m_move(move),
      m_value(value),
      m_count(count),
      m_raveValue(raveValue),
      m_raveCount(raveCount)
{
}

//----------------------------------------------------------------------------

/** Types of proven nodes. */
typedef enum 
{
    /** Node is not a proven win or loss. */
    SG_NOT_PROVEN,
    
    /** Node is a proven win. */
    SG_PROVEN_WIN,

    /** Node is a proven loss. */
    SG_PROVEN_LOSS

} SgProvenNodeType;

//----------------------------------------------------------------------------

/** Node used in SgUctTree.
    All data members are declared as volatile to avoid that the compiler
    re-orders writes, which can break assumptions made by SgUctSearch in
    lock-free mode (see @ref sguctsearchlockfree). For example, the search
    relies on the fact that m_firstChild is valid, if m_nuChildren is greater
    zero or that the mean value of the move and RAVE value statistics is valid
    if the corresponding count is greater zero.
    @ingroup sguctgroup
*/
class SgUctNode
{
public:
    /** Initializes node with given move, value and count. */
    SgUctNode(const SgMoveInfo& info);

    /** Add game result.
        @param eval The game result (e.g. score or 0/1 for win loss)
    */
    void AddGameResult(float eval);

    /** Adds a game result count times. */
    void AddGameResults(float eval, std::size_t count);

    /** Add other nodes results to this node's. */
    void MergeResults(const SgUctNode& node);

    /** Removes a game result.
        @param eval The game result (e.g. score or 0/1 for win loss)
    */
    void RemoveGameResult(float eval);

    /** Removes a game result count times. */
    void RemoveGameResults(float eval, std::size_t count);

    /** Number of times this node was visited.
        This corresponds to the sum of MoveCount() of all children.
        It can be different from MoveCount() of this position, if prior
        knowledge initialization of the children is used.
    */
    std::size_t PosCount() const;

    /** Number of times the move leading to this position was chosen.
        This count will be different from PosCount(), if prior knowledge
        initialization is used.
    */
    std::size_t MoveCount() const;

    /** Get first child.
        @note This information is an implementation detail of how SgUctTree
        manages nodes. Use SgUctChildIterator to access children nodes.
    */
    const SgUctNode* FirstChild() const;

    /** Does the node have at least one child? */
    bool HasChildren() const;

    /** Average game result.
        Requires: HasMean()
     */
    float Mean() const;

    /** True, if mean value is defined (move count not zero) */
    bool HasMean() const;

    /** Get number of children.
        @note This information is an implementation detail of how SgUctTree
        manages nodes. Use SgUctChildIterator to access children nodes.
    */
    int NuChildren() const;

    /** See FirstChild() */
    void SetFirstChild(const SgUctNode* child);

    /** See NuChildren() */
    void SetNuChildren(int nuChildren);

    /** Increment the position count.
        See PosCount()
    */
    void IncPosCount();

    /** Decrement the position count.
        See PosCount()
    */
    void DecPosCount();

    void SetPosCount(std::size_t value);

    /** Initialize value with prior knowledge. */
    void InitializeValue(float value, std::size_t count);

    /** Copy data from other node.
        Copies all data, apart from the children information (first child
        and number of children).
    */
    void CopyDataFrom(const SgUctNode& node);

    /** Get move.
        Requires: Node has a move (is not root node)
    */
    SgMove Move() const;

    /** Get RAVE count.
        @see SgUctSearch::Rave().
    */
    float RaveCount() const;

    /** Get RAVE mean value.
        Requires: HasRaveValue()
        @see SgUctSearch::Rave().
    */
    float RaveValue() const;

    bool HasRaveValue() const;

    /** Add a game result value to the RAVE value.
        @see SgUctSearch::Rave().
    */
    void AddRaveValue(float value, float weight);

    /** Removes a rave result. */
    void RemoveRaveValue(float value);

    void RemoveRaveValue(float value, float weight);

    /** Initialize RAVE value with prior knowledge. */
    void InitializeRaveValue(float value, float count);

    /** Returns the last time knowledge was computed. */
    std::size_t KnowledgeCount() const;

    /** Set that knowledge has been computed at count. */
    void SetKnowledgeCount(std::size_t count);

    /** Returns true if node is a proven node. */
    bool IsProven() const;

    bool IsProvenWin() const;

    bool IsProvenLoss() const;

    SgProvenNodeType ProvenNodeType() const;

    void SetProvenNodeType(SgProvenNodeType type);

private:
    SgStatisticsBase<volatile float,volatile std::size_t> m_statistics;

    const SgUctNode* volatile m_firstChild;

    volatile int m_nuChildren;

    volatile SgMove m_move;

    /** RAVE statistics.
        Uses double for count to allow adding fractional values if RAVE
        updates are weighted.
    */
    SgStatisticsBase<volatile float,volatile float> m_raveValue;

    volatile std::size_t m_posCount;

    volatile std::size_t m_knowledgeCount;

    volatile SgProvenNodeType m_provenType;
};

inline SgUctNode::SgUctNode(const SgMoveInfo& info)
    : m_statistics(info.m_value, info.m_count),
      m_nuChildren(0),
      m_move(info.m_move),
      m_raveValue(info.m_raveValue, info.m_raveCount),
      m_posCount(0),
      m_knowledgeCount(0),
      m_provenType(SG_NOT_PROVEN)
{
    // m_firstChild is not initialized, only defined if m_nuChildren > 0
}

inline void SgUctNode::AddGameResult(float eval)
{
    m_statistics.Add(eval);
}

inline void SgUctNode::AddGameResults(float eval, std::size_t count)
{
    m_statistics.Add(eval, count);
}

inline void SgUctNode::MergeResults(const SgUctNode& node)
{
    if (node.m_statistics.IsDefined())
        m_statistics.Add(node.m_statistics.Mean(), node.m_statistics.Count());
    if (node.m_raveValue.IsDefined())
        m_raveValue.Add(node.m_raveValue.Mean(), node.m_raveValue.Count());
}

inline void SgUctNode::RemoveGameResult(float eval)
{
    m_statistics.Remove(eval);
}

inline void SgUctNode::RemoveGameResults(float eval, std::size_t count)
{
    m_statistics.Remove(eval, count);
}

inline void SgUctNode::AddRaveValue(float value, float weight)
{
    m_raveValue.Add(value, weight);
}

inline void SgUctNode::RemoveRaveValue(float value)
{
    m_raveValue.Remove(value);
}

inline void SgUctNode::RemoveRaveValue(float value, float weight)
{
    m_raveValue.Remove(value, weight);
}

inline void SgUctNode::CopyDataFrom(const SgUctNode& node)
{
    m_statistics = node.m_statistics;
    m_move = node.m_move;
    m_raveValue = node.m_raveValue;
    m_posCount = node.m_posCount;
    m_knowledgeCount = node.m_knowledgeCount;
    m_provenType = node.m_provenType;
}

inline const SgUctNode* SgUctNode::FirstChild() const
{
    SG_ASSERT(HasChildren()); // Otherwise m_firstChild is undefined
    return m_firstChild;
}

inline bool SgUctNode::HasChildren() const
{
    return (m_nuChildren > 0);
}

inline bool SgUctNode::HasMean() const
{
    return m_statistics.IsDefined();
}

inline bool SgUctNode::HasRaveValue() const
{
    return m_raveValue.IsDefined();
}

inline void SgUctNode::IncPosCount()
{
    ++m_posCount;
}

inline void SgUctNode::DecPosCount()
{
    --m_posCount;
}

inline void SgUctNode::InitializeValue(float value, std::size_t count)
{
    m_statistics.Initialize(value, count);
}

inline void SgUctNode::InitializeRaveValue(float value, float count)
{
    m_raveValue.Initialize(value, count);
}

inline float SgUctNode::Mean() const
{
    return m_statistics.Mean();
}

inline SgMove SgUctNode::Move() const
{
    SG_ASSERT(m_move != SG_NULLMOVE);
    return m_move;
}

inline std::size_t SgUctNode::MoveCount() const
{
    return m_statistics.Count();
}

inline int SgUctNode::NuChildren() const
{
    return m_nuChildren;
}

inline std::size_t SgUctNode::PosCount() const
{
    return m_posCount;
}

inline float SgUctNode::RaveCount() const
{
    return m_raveValue.Count();
}

inline float SgUctNode::RaveValue() const
{
    return m_raveValue.Mean();
}

inline void SgUctNode::SetFirstChild(const SgUctNode* child)
{
    m_firstChild = child;
}

inline void SgUctNode::SetNuChildren(int nuChildren)
{
    SG_ASSERT(nuChildren >= 0);
    m_nuChildren = nuChildren;
}

inline void SgUctNode::SetPosCount(std::size_t value)
{
    m_posCount = value;
}

inline std::size_t SgUctNode::KnowledgeCount() const
{
    return m_knowledgeCount;
}

inline void SgUctNode::SetKnowledgeCount(std::size_t count)
{
    m_knowledgeCount = count;
}

inline bool SgUctNode::IsProven() const
{
    return m_provenType != SG_NOT_PROVEN;
}

inline bool SgUctNode::IsProvenWin() const
{
    return m_provenType == SG_PROVEN_WIN;
}

inline bool SgUctNode::IsProvenLoss() const
{
    return m_provenType == SG_PROVEN_LOSS;
}

inline SgProvenNodeType SgUctNode::ProvenNodeType() const
{
    return m_provenType;
}

inline void SgUctNode::SetProvenNodeType(SgProvenNodeType type)
{
    m_provenType = type;
}

//----------------------------------------------------------------------------

/** Allocater for nodes used in the implementation of SgUctTree.
    Each thread has its own node allocator to allow lock-free usage of
    SgUctTree.
    @ingroup sguctgroup
*/
class SgUctAllocator
{
public:
    SgUctAllocator();

    ~SgUctAllocator();

    void Clear();

    /** Does the allocator have the capacity for n more nodes? */
    bool HasCapacity(std::size_t n) const;

    std::size_t NuNodes() const;

    std::size_t MaxNodes() const;

    void SetMaxNodes(std::size_t maxNodes);

    /** Check if allocator contains node.
        This function uses pointer comparisons. Since the result of
        comparisons for pointers to elements in different containers
        is platform-dependent, it is only guaranteed that it returns true,
        if not node belongs to the allocator, but not that it returns false
        for nodes not in the allocator.
    */
    bool Contains(const SgUctNode& node) const;

    const SgUctNode* Start() const;

    SgUctNode* Finish();

    const SgUctNode* Finish() const;

    /** Create a new node at the end of the storage.
        REQUIRES: HasCapacity(1)
        @param move The constructor argument.
        @return A pointer to new newly created node.
    */
    SgUctNode* CreateOne(SgMove move);

    /** Create a number of new nodes with a given list of moves at the end of
        the storage. Returns the sum of counts of moves.
        REQUIRES: HasCapacity(moves.size())
        @param moves The list of moves.
    */
    std::size_t Create(const std::vector<SgMoveInfo>& moves);

    /** Create a number of new nodes at the end of the storage.
        REQUIRES: HasCapacity(n)
        @param n The number of nodes to create.
    */
    void CreateN(std::size_t n);

    void Swap(SgUctAllocator& allocator);

private:
    SgUctNode* m_start;

    SgUctNode* m_finish;

    SgUctNode* m_endOfStorage;

    /** Not implemented.
        Cannot be copied because array contains pointers to elements.
        Use Swap() instead.
    */
    SgUctAllocator& operator=(const SgUctAllocator& tree);
};

inline SgUctAllocator::SgUctAllocator()
{
    m_start = 0;
}

inline void SgUctAllocator::Clear()
{
    if (m_start != 0)
    {
        for (SgUctNode* it = m_start; it != m_finish; ++it)
            it->~SgUctNode();
        m_finish = m_start;
    }
}

inline SgUctNode* SgUctAllocator::CreateOne(SgMove move)
{
    SG_ASSERT(HasCapacity(1));
    new(m_finish) SgUctNode(move);
    return (m_finish++);
}

inline std::size_t SgUctAllocator::Create(
                                         const std::vector<SgMoveInfo>& moves)
{
    SG_ASSERT(HasCapacity(moves.size()));
    std::size_t count = 0;
    for (std::vector<SgMoveInfo>::const_iterator it = moves.begin();
         it != moves.end(); ++it, ++m_finish)
    {
        new(m_finish) SgUctNode(*it);
        count += it->m_count;
    }
    return count;
}

inline void SgUctAllocator::CreateN(std::size_t n)
{
    SG_ASSERT(HasCapacity(n));
    SgUctNode* newFinish = m_finish + n;
    for ( ; m_finish != newFinish; ++m_finish)
        new(m_finish) SgUctNode(SG_NULLMOVE);
}

inline SgUctNode* SgUctAllocator::Finish()
{
    return m_finish;
}

inline const SgUctNode* SgUctAllocator::Finish() const
{
    return m_finish;
}

inline bool SgUctAllocator::HasCapacity(std::size_t n) const
{
    return (m_finish + n <= m_endOfStorage);
}

inline std::size_t SgUctAllocator::MaxNodes() const
{
    return m_endOfStorage - m_start;
}

inline std::size_t SgUctAllocator::NuNodes() const
{
    return m_finish - m_start;
}

inline const SgUctNode* SgUctAllocator::Start() const
{
    return m_start;
}

//----------------------------------------------------------------------------

/** Tree used in SgUctSearch.
    The nodes can be accessed only by getting non-const references or modified
    through accessor functions of SgUctTree, therefore SgUctTree can guarantee
    the integrity of the tree structure.
    The tree can be used in a lock-free way during a search (see
    @ref sguctsearchlockfree).
    @ingroup sguctgroup
*/
class SgUctTree
{
public:
    friend class SgUctChildIterator;

    /** Constructor.
        Construct a tree. Before using the tree, CreateAllocators() and
        SetMaxNodes() must be called (in this order).
    */
    SgUctTree();

    /** Create node allocators for threads. */
    void CreateAllocators(std::size_t nuThreads);

    /** Add a game result.
        @param node The node.
        @param father The father (if not root) to update the position count.
        @param eval
    */
    void AddGameResult(const SgUctNode& node, const SgUctNode* father,
                       float eval);

    /** Adds a game result count times. */
    void AddGameResults(const SgUctNode& node, const SgUctNode* father,
                        float eval, std::size_t count);

    /** Removes a game result.
        @param node The node.
        @param father The father (if not root) to update the position count.
        @param eval
    */
    void RemoveGameResult(const SgUctNode& node, const SgUctNode* father,
                          float eval);

    /** Removes a game result count times. */
    void RemoveGameResults(const SgUctNode& node, const SgUctNode* father,
                           float eval, std::size_t count);

    /** Adds a virtual loss to the given nodes. */
    void AddVirtualLoss(const std::vector<const SgUctNode*>& nodes);

    /** Removes a virtual loss to the given nodes. */
    void RemoveVirtualLoss(const std::vector<const SgUctNode*>& nodes);

    void Clear();

    /** Return the current maximum number of nodes.
        This returns the maximum number of nodes as set by SetMaxNodes().
        See SetMaxNodes() why the real maximum number of nodes can be higher
        or lower.
    */
    std::size_t MaxNodes() const;

    /** Change maximum number of nodes.
        Also clears the tree. This will call SetMaxNodes() at each registered
        allocator with maxNodes / numberAllocators as an argument. The real
        maximum number of nodes can be higher (because the root node is
        owned by this class, not an allocator) or lower (if maxNodes is not
        a multiple of the number of allocators).
        @param maxNodes Maximum number of nodes
    */
    void SetMaxNodes(std::size_t maxNodes);

    /** Swap content with another tree.
        The other tree must have the same number of allocators and
        the same maximum number of nodes.
    */
    void Swap(SgUctTree& tree);

    bool HasCapacity(std::size_t allocatorId, std::size_t n) const;

    /** Create children nodes.
        Requires: Allocator(allocatorId).HasCapacity(moves.size())
    */
    void CreateChildren(std::size_t allocatorId, const SgUctNode& node,
                        const std::vector<SgMoveInfo>& moves);

    /** Merge new children with old.
        Requires: Allocator(allocatorId).HasCapacity(moves.size())
     */
    void MergeChildren(std::size_t allocatorId, const SgUctNode& node,
                       const std::vector<SgMoveInfo>& moves,
                       bool deleteChildTrees);

    /** Extract subtree to a different tree.
        The tree will be truncated if one of the allocators overflows (can
        happen due to reassigning nodes to different allocators), the given
        max time is exceeded or on SgUserAbort().
        @param[out] target The resulting subtree. Must have the same maximum
        number of nodes. Will be cleared before using.
        @param node The start node of the subtree.
        @param warnTruncate Print warning to SgDebug() if tree was truncated
        @param maxTime Truncate the tree, if the extraction takes longer than
        the given time
    */
    void ExtractSubtree(SgUctTree& target, const SgUctNode& node,
                   bool warnTruncate,
                   double maxTime = std::numeric_limits<double>::max()) const;

    /** Get a copy of the tree with low count nodes pruned.
        The tree will be truncated if one of the allocators overflows (can
        happen due to reassigning nodes to different allocators), the given
        max time is exceeded or on SgUserAbort().
        @param[out] target The resulting tree. Must have the same maximum
        number of nodes. Will be cleared before using.
        @param minCount The minimum count (SgUctNode::MoveCount())
        @param warnTruncate Print warning to SgDebug() if tree was truncated
        @param maxTime Truncate the tree, if the extraction takes longer than
        the given time
    */
    void CopyPruneLowCount(SgUctTree& target, std::size_t minCount,
                   bool warnTruncate,
                   double maxTime = std::numeric_limits<double>::max()) const;

    const SgUctNode& Root() const;

    std::size_t NuAllocators() const;

    /** Total number of nodes.
        Includes the sum of nodes in all allocators plus the root node.
    */
    std::size_t NuNodes() const;

    /** Number of nodes in one of the allocators. */
    std::size_t NuNodes(std::size_t allocatorId) const;

    /** Add a game result value to the RAVE value of a node.
        @param node The node with the move
        @param value
        @param weight
        @see SgUctSearch::Rave().
    */
    void AddRaveValue(const SgUctNode& node, float value, float weight);

    /** Remove a game result from the RAVE value of a node.
        @param node The node with the move
        @param value
        @param weight
        @see SgUctSearch::Rave().
    */
    void RemoveRaveValue(const SgUctNode& node, float value, float weight);

    /** Initialize the value and count of a node. */
    void InitializeValue(const SgUctNode& node, float value,
                         std::size_t count);

    void SetPosCount(const SgUctNode& node, std::size_t posCount);

    /** Initialize the rave value and count of a move node with prior
        knowledge.
    */
    void InitializeRaveValue(const SgUctNode& node, float value, float count);

    /** Remove some children of a node according to a list of filtered moves.
        Requires: Allocator(allocatorId).HasCapacity(node.NuChildren()) <br>
        For efficiency, no reorganization of the tree is done to remove
        the dead subtrees (and NuNodes() will not report the real number of
        nodes in the tree). This function can be used in lock-free mode.
    */
    void ApplyFilter(std::size_t allocatorId, const SgUctNode& node,
                     const std::vector<SgMove>& rootFilter);

    /** Sets the children under node to be exactly those in moves,
        reusing the old children if possible. Children not in moves
        are pruned, children missing from moves are added as leaves.
        Requires: Allocator(allocatorId).HasCapacity(moves.size())
     */
    void SetChildren(std::size_t allocatorId, const SgUctNode& node,
                     const vector<SgMove>& moves);

    /** @name Functions for debugging */
    // @{

    /** Do some consistency checks.
        @throws SgException if inconsistencies are detected.
    */
    void CheckConsistency() const;

    /** Check if tree contains node.
        This function uses pointer comparisons. Since the result of
        comparisons for pointers to elements in different containers
        is platform-dependent, it is only guaranteed that it returns true,
        if not node belongs to the allocator, but not that it returns false
        for nodes not in the tree.
    */
    bool Contains(const SgUctNode& node) const;

    void DumpDebugInfo(std::ostream& out) const;

    // @} // @name

private:
    std::size_t m_maxNodes;

    SgUctNode m_root;

    /** Allocators.
        The elements are owned by the vector (shared_ptr is only used because
        auto_ptr should not be used with standard containers)
    */
    std::vector<boost::shared_ptr<SgUctAllocator> > m_allocators;

    /** Not implemented.
        Cannot be copied because allocators contain pointers to elements.
        Use SgUctTree::Swap instead.
    */
    SgUctTree& operator=(const SgUctTree& tree);

    SgUctAllocator& Allocator(std::size_t i);

    const SgUctAllocator& Allocator(std::size_t i) const;

    void CopySubtree(SgUctTree& target, SgUctNode& targetNode,
                     const SgUctNode& node, std::size_t minCount,
                     std::size_t& currentAllocatorId, bool warnTruncate,
                     bool& abort, SgTimer& timer, double maxTime) const;

    void ThrowConsistencyError(const std::string& message) const;
};

inline void SgUctTree::AddGameResult(const SgUctNode& node,
                                     const SgUctNode* father, float eval)
{
    SG_ASSERT(Contains(node));
    // Parameters are const-references, because only the tree is allowed
    // to modify nodes
    if (father != 0)
        const_cast<SgUctNode*>(father)->IncPosCount();
    const_cast<SgUctNode&>(node).AddGameResult(eval);
}

inline void SgUctTree::AddGameResults(const SgUctNode& node,
                                      const SgUctNode* father, float eval,
                                      std::size_t count)
{

    SG_ASSERT(Contains(node));
    // Parameters are const-references, because only the tree is allowed
    // to modify nodes
    if (father != 0)
        const_cast<SgUctNode*>(father)->SetPosCount(father->PosCount() 
                                                    + count);
    const_cast<SgUctNode&>(node).AddGameResults(eval, count);
}

inline void SgUctTree::CreateChildren(std::size_t allocatorId,
                                      const SgUctNode& node,
                                      const std::vector<SgMoveInfo>& moves)
{
    SG_ASSERT(Contains(node));
    // Parameters are const-references, because only the tree is allowed
    // to modify nodes
    SgUctNode& nonConstNode = const_cast<SgUctNode&>(node);
    size_t nuChildren = moves.size();
    SG_ASSERT(nuChildren > 0);
    SgUctAllocator& allocator = Allocator(allocatorId);
    SG_ASSERT(allocator.HasCapacity(nuChildren));

    // In lock-free multi-threading, a node can be expanded multiple times
    // (the later thread overwrites the children information of the previous
    // thread)
    SG_ASSERT(NuAllocators() > 1 || ! node.HasChildren());

    const SgUctNode* firstChild = allocator.Finish();
    
    std::size_t parentCount = allocator.Create(moves);

    // Write order dependency: SgUctSearch in lock-free mode assumes that
    // m_firstChild is valid if m_nuChildren is greater zero
    nonConstNode.SetPosCount(parentCount);
    SgSynchronizeThreadMemory();
    nonConstNode.SetFirstChild(firstChild);
    SgSynchronizeThreadMemory();
    nonConstNode.SetNuChildren(nuChildren);
}

inline void SgUctTree::RemoveGameResult(const SgUctNode& node,
                                        const SgUctNode* father, float eval)
{
    SG_ASSERT(Contains(node));
    // Parameters are const-references, because only the tree is allowed
    // to modify nodes
    if (father != 0)
        const_cast<SgUctNode*>(father)->DecPosCount();
    const_cast<SgUctNode&>(node).RemoveGameResult(eval);
}

inline void SgUctTree::RemoveGameResults(const SgUctNode& node,
                                         const SgUctNode* father, float eval,
                                         std::size_t count)
{
    SG_ASSERT(Contains(node));
    // Parameters are const-references, because only the tree is allowed
    // to modify nodes 
    if (father != 0)
        const_cast<SgUctNode*>(father)->SetPosCount(father->PosCount() 
                                                    - count);
    const_cast<SgUctNode&>(node).RemoveGameResults(eval, count);
}

inline void SgUctTree::AddRaveValue(const SgUctNode& node, float value,
                                    float weight)
{
    SG_ASSERT(Contains(node));
    // Parameters are const-references, because only the tree is allowed
    // to modify nodes
    const_cast<SgUctNode&>(node).AddRaveValue(value, weight);
}

inline void SgUctTree::RemoveRaveValue(const SgUctNode& node, float value,
                                       float weight)
{
    SG_UNUSED(weight);
    SG_ASSERT(Contains(node));
    // Parameters are const-references, because only the tree is allowed
    // to modify nodes
    const_cast<SgUctNode&>(node).RemoveRaveValue(value, weight);
}

inline SgUctAllocator& SgUctTree::Allocator(std::size_t i)
{
    SG_ASSERT(i < m_allocators.size());
    return *m_allocators[i];
}

inline const SgUctAllocator& SgUctTree::Allocator(std::size_t i) const
{
    SG_ASSERT(i < m_allocators.size());
    return *m_allocators[i];
}

inline bool SgUctTree::HasCapacity(std::size_t allocatorId,
                                   std::size_t n) const
{
    return Allocator(allocatorId).HasCapacity(n);
}

inline void SgUctTree::InitializeValue(const SgUctNode& node,
                                       float value, std::size_t count)
{
    SG_ASSERT(Contains(node));
    // Parameter is const-reference, because only the tree is allowed
    // to modify nodes
    const_cast<SgUctNode&>(node).InitializeValue(value, count);
}

inline void SgUctTree::InitializeRaveValue(const SgUctNode& node,
                                           float value, float count)
{
    SG_ASSERT(Contains(node));
    // Parameters are const-references, because only the tree is allowed
    // to modify nodes
    const_cast<SgUctNode&>(node).InitializeRaveValue(value, count);
}

inline std::size_t SgUctTree::MaxNodes() const
{
    return m_maxNodes;
}

inline std::size_t SgUctTree::NuAllocators() const
{
    return m_allocators.size();
}

inline std::size_t SgUctTree::NuNodes(std::size_t allocatorId) const
{
    return Allocator(allocatorId).NuNodes();
}

inline const SgUctNode& SgUctTree::Root() const
{
    return m_root;
}

inline void SgUctTree::SetPosCount(const SgUctNode& node,
                                   std::size_t posCount)
{
    SG_ASSERT(Contains(node));
    // Parameters are const-references, because only the tree is allowed
    // to modify nodes
    const_cast<SgUctNode&>(node).SetPosCount(posCount);
}

//----------------------------------------------------------------------------

/** Iterator over all children of a node.
    It was intentionally implemented to be used only, if at least one child
    exists (checked with an assertion), since in many use cases, the case
    of no children needs to be handled specially and should be checked
    before doing a loop over all children.
    @ingroup sguctgroup
*/
class SgUctChildIterator
{
public:
    /** Constructor.
        Requires: node.HasChildren()
    */
    SgUctChildIterator(const SgUctTree& tree, const SgUctNode& node);

    const SgUctNode& operator*() const;

    void operator++();

    operator bool() const;

private:
    const SgUctNode* m_current;

    const SgUctNode* m_last;
};

inline SgUctChildIterator::SgUctChildIterator(const SgUctTree& tree,
                                              const SgUctNode& node)
{
    SG_DEBUG_ONLY(tree);
    SG_ASSERT(tree.Contains(node));
    SG_ASSERT(node.HasChildren());
    m_current = node.FirstChild();
    m_last = m_current + node.NuChildren();
}

inline const SgUctNode& SgUctChildIterator::operator*() const
{
    return *m_current;
}

inline void SgUctChildIterator::operator++()
{
    ++m_current;
}

inline SgUctChildIterator::operator bool() const
{
    return (m_current < m_last);
}

//----------------------------------------------------------------------------

/** Iterator for traversing a tree depth-first.
    @ingroup sguctgroup
*/
class SgUctTreeIterator
{
public:
    SgUctTreeIterator(const SgUctTree& tree);

    const SgUctNode& operator*() const;

    void operator++();

    operator bool() const;

private:
    const SgUctTree& m_tree;

    const SgUctNode* m_current;

    /** Stack of child iterators.
        The elements are owned by the stack (shared_ptr is only used because
        auto_ptr should not be used with standard containers)
    */
    std::stack<boost::shared_ptr<SgUctChildIterator> > m_stack;
};

//----------------------------------------------------------------------------

#endif // SG_UCTTREE_H
