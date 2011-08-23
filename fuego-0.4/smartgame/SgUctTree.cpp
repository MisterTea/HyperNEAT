//----------------------------------------------------------------------------
/** @file SgUctTree.cpp
    See SgUctTree.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgUctTree.h"

#include <boost/format.hpp>
#include "SgDebug.h"
#include "SgTimer.h"

using namespace std;
using boost::format;
using boost::shared_ptr;

//----------------------------------------------------------------------------

SgUctAllocator::~SgUctAllocator()
{
    if (m_start != 0)
    {
        Clear();
        std::free(m_start);
    }
}

bool SgUctAllocator::Contains(const SgUctNode& node) const
{
    return (&node >= m_start && &node < m_finish);
}

void SgUctAllocator::Swap(SgUctAllocator& allocator)
{
    swap(m_start, allocator.m_start);
    swap(m_finish, allocator.m_finish);
    swap(m_endOfStorage, allocator.m_endOfStorage);
}

void SgUctAllocator::SetMaxNodes(std::size_t maxNodes)
{
    if (m_start != 0)
    {
        Clear();
        std::free(m_start);
    }
    void* ptr = std::malloc(maxNodes * sizeof(SgUctNode));
    if (ptr == 0)
        throw std::bad_alloc();
    m_start = static_cast<SgUctNode*>(ptr);
    m_finish = m_start;
    m_endOfStorage = m_start + maxNodes;
}

//----------------------------------------------------------------------------

SgUctTree::SgUctTree()
    : m_maxNodes(0),
      m_root(SG_NULLMOVE)
{
}

void SgUctTree::AddVirtualLoss(const std::vector<const SgUctNode*>& nodes)
{
    for (size_t i = 0; i < nodes.size(); ++i)
    {
        const SgUctNode* father = (i > 0 ? nodes[i-1] : 0);
        AddGameResult(*nodes[i], father, 1.0); // loss for us = win for child
        AddRaveValue(*nodes[i], 0.0, 1.0); // loss for us
    }
}

void SgUctTree::ApplyFilter(std::size_t allocatorId, const SgUctNode& node,
                            const vector<SgMove>& rootFilter)
{
    SG_ASSERT(Contains(node));
    SG_ASSERT(Allocator(allocatorId).HasCapacity(node.NuChildren()));
    if (! node.HasChildren())
        return;

    SgUctAllocator& allocator = Allocator(allocatorId);
    const SgUctNode* firstChild = allocator.Finish();

    int nuChildren = 0;
    for (SgUctChildIterator it(*this, node); it; ++it)
    {
        SgMove move = (*it).Move();
        if (find(rootFilter.begin(), rootFilter.end(), move)
            == rootFilter.end())
        {
            SgUctNode* child = allocator.CreateOne(move);
            child->CopyDataFrom(*it);
            int childNuChildren = (*it).NuChildren();
            child->SetNuChildren(childNuChildren);
            if (childNuChildren > 0)
                child->SetFirstChild((*it).FirstChild());
            ++nuChildren;
        }
    }

    SgUctNode& nonConstNode = const_cast<SgUctNode&>(node);
    // Write order dependency: SgUctSearch in lock-free mode assumes that
    // m_firstChild is valid if m_nuChildren is greater zero
    SgSynchronizeThreadMemory();
    nonConstNode.SetFirstChild(firstChild);
    SgSynchronizeThreadMemory();
    nonConstNode.SetNuChildren(nuChildren);
}

void SgUctTree::SetChildren(std::size_t allocatorId, const SgUctNode& node,
                            const vector<SgMove>& moves)
{
    SG_ASSERT(Contains(node));
    SG_ASSERT(Allocator(allocatorId).HasCapacity(moves.size()));
    SG_ASSERT(node.HasChildren());

    SgUctAllocator& allocator = Allocator(allocatorId);
    const SgUctNode* firstChild = allocator.Finish();

    int nuChildren = 0;
    for (size_t i = 0; i < moves.size(); ++i)
    {
        bool found = false;
        for (SgUctChildIterator it(*this, node); it; ++it)
        {
            SgMove move = (*it).Move();
            if (move == moves[i])
            {
                found = true;
                SgUctNode* child = allocator.CreateOne(move);
                child->CopyDataFrom(*it);
                int childNuChildren = (*it).NuChildren();
                child->SetNuChildren(childNuChildren);
                if (childNuChildren > 0)
                    child->SetFirstChild((*it).FirstChild());
                ++nuChildren;
                break;
            }
        }
        if (! found)
        {
            allocator.CreateOne(moves[i]);
            ++nuChildren;
        }
    }
    SG_ASSERT((size_t)nuChildren == moves.size());

    SgUctNode& nonConstNode = const_cast<SgUctNode&>(node);
    // Write order dependency: SgUctSearch in lock-free mode assumes that
    // m_firstChild is valid if m_nuChildren is greater zero
    SgSynchronizeThreadMemory();
    nonConstNode.SetFirstChild(firstChild);
    SgSynchronizeThreadMemory();
    nonConstNode.SetNuChildren(nuChildren);
}

void SgUctTree::CheckConsistency() const
{
    for (SgUctTreeIterator it(*this); it; ++it)
        if (! Contains(*it))
            ThrowConsistencyError(str(format("! Contains(%1%)") % &(*it)));
}

void SgUctTree::Clear()
{
    for (size_t i = 0; i < NuAllocators(); ++i)
        Allocator(i).Clear();
    m_root = SgUctNode(SG_NULLMOVE);
}

/** Check if node is in tree.
    Only used for assertions. May not be available in future implementations.
*/
bool SgUctTree::Contains(const SgUctNode& node) const
{
    if (&node == &m_root)
        return true;
    for (size_t i = 0; i < NuAllocators(); ++i)
        if (Allocator(i).Contains(node))
            return true;
    return false;
}

void SgUctTree::CopyPruneLowCount(SgUctTree& target, std::size_t minCount,
                                  bool warnTruncate, double maxTime) const
{
    size_t allocatorId = 0;
    SgTimer timer;
    bool abort = false;
    CopySubtree(target, target.m_root, m_root, minCount, allocatorId,
                warnTruncate, abort, timer, maxTime);
}

/** Recursive function used by SgUctTree::ExtractSubtree and
    SgUctTree::CopyPruneLowCount.
    @param target The target tree.
    @param targetNode The target node; it is already created but the content
    not yet copied
    @param node The node in the source tree to be copied.
    @param minCount The minimum count (SgUctNode::MoveCount()) of a non-root
    node in the source tree to copy
    @param currentAllocatorId The current node allocator. Will be incremented
    in each call to CopySubtree to use node allocators of target tree evenly.
    @param warnTruncate Print warning to SgDebug() if tree was
    truncated (e.g due to reassigning nodes to different allocators)
    @param[in,out] abort Flag to abort copying. Must be initialized to false
    by top-level caller
    @param timer
    @param maxTime See ExtractSubtree()
*/
void SgUctTree::CopySubtree(SgUctTree& target, SgUctNode& targetNode,
                            const SgUctNode& node, std::size_t minCount,
                            std::size_t& currentAllocatorId,
                            bool warnTruncate, bool& abort, SgTimer& timer,
                            double maxTime) const
{
    SG_ASSERT(Contains(node));
    SG_ASSERT(target.Contains(targetNode));
    targetNode.CopyDataFrom(node);

    if (! node.HasChildren() || node.MoveCount() < minCount)
        return;

    SgUctAllocator& targetAllocator = target.Allocator(currentAllocatorId);
    int nuChildren = node.NuChildren();
    if (! abort)
    {
        if (! targetAllocator.HasCapacity(nuChildren))
        {
            // This can happen even if target tree has same maximum number of
            // nodes, because allocators are used differently.
            if (warnTruncate)
                SgDebug() <<
                "SgUctTree::CopySubtree: Truncated (allocator capacity)\n";
            abort = true;
        }
        if (timer.IsTimeOut(maxTime, 10000))
        {
            if (warnTruncate)
                SgDebug() << "SgUctTree::CopySubtree: Truncated (max time)\n";
            abort = true;
        }
        if (SgUserAbort())
        {
            if (warnTruncate)
                SgDebug() << "SgUctTree::CopySubtree: Truncated (aborted)\n";
            abort = true;
        }
    }
    if (abort)
    {
        // Don't copy the children and set the pos count to zero (should
        // reflect the sum of children move counts)
        targetNode.SetPosCount(0);
        return;
    }

    SgUctNode* firstTargetChild = targetAllocator.Finish();
    targetNode.SetFirstChild(firstTargetChild);
    targetNode.SetNuChildren(nuChildren);

    // Create target nodes first (must be contiguous in the target tree)
    targetAllocator.CreateN(nuChildren);

    // Recurse
    SgUctNode* targetChild = firstTargetChild;
    for (SgUctChildIterator it(*this, node); it; ++it, ++targetChild)
    {
        const SgUctNode& child = *it;
        ++currentAllocatorId; // Cycle to use allocators uniformly
        if (currentAllocatorId >= target.NuAllocators())
            currentAllocatorId = 0;
        CopySubtree(target, *targetChild, child, minCount, currentAllocatorId,
                    warnTruncate, abort, timer, maxTime);
    }
}

void SgUctTree::CreateAllocators(std::size_t nuThreads)
{
    Clear();
    m_allocators.clear();
    for (size_t i = 0; i < nuThreads; ++i)
    {
        boost::shared_ptr<SgUctAllocator> allocator(new SgUctAllocator());
        m_allocators.push_back(allocator);
    }
}

void SgUctTree::DumpDebugInfo(std::ostream& out) const
{
    out << "Root " << &m_root << '\n';
    for (size_t i = 0; i < NuAllocators(); ++i)
        out << "Allocator " << i
            << " size=" << Allocator(i).NuNodes()
            << " start=" << Allocator(i).Start()
            << " finish=" << Allocator(i).Finish() << '\n';
}

void SgUctTree::ExtractSubtree(SgUctTree& target, const SgUctNode& node,
                               bool warnTruncate, double maxTime) const
{
    SG_ASSERT(Contains(node));
    SG_ASSERT(&target != this);
    SG_ASSERT(target.MaxNodes() == MaxNodes());
    target.Clear();
    size_t allocatorId = 0;
    SgTimer timer;
    bool abort = false;
    CopySubtree(target, target.m_root, node, 0, allocatorId, warnTruncate,
                abort, timer, maxTime);
}

void SgUctTree::MergeChildren(std::size_t allocatorId, const SgUctNode& node,
                              const std::vector<SgMoveInfo>& moves,
                              bool deleteChildTrees)
{
    SG_ASSERT(Contains(node));
    // Parameters are const-references, because only the tree is allowed
    // to modify nodes
    SgUctNode& nonConstNode = const_cast<SgUctNode&>(node);
    size_t nuNewChildren = moves.size();

    if (nuNewChildren == 0)
    {
        // Write order dependency
        nonConstNode.SetNuChildren(0);
        SgSynchronizeThreadMemory();
        nonConstNode.SetFirstChild(0);
        return;
    }

    SgUctAllocator& allocator = Allocator(allocatorId);
    SG_ASSERT(allocator.HasCapacity(nuNewChildren));

    const SgUctNode* newFirstChild = allocator.Finish();
    std::size_t parentCount = allocator.Create(moves);
    
    // Update new children with data in old children
    for (std::size_t i = 0; i < moves.size(); ++i) 
    {
        SgUctNode* newChild = const_cast<SgUctNode*>(&newFirstChild[i]);
        for (SgUctChildIterator it(*this, node); it; ++it)
        {
            const SgUctNode& oldChild = *it;
            if (oldChild.Move() == moves[i].m_move)
            {
                newChild->MergeResults(oldChild);
                newChild->SetKnowledgeCount(oldChild.KnowledgeCount());
                if (! deleteChildTrees)
                {
                    newChild->SetPosCount(oldChild.PosCount());
                    parentCount += oldChild.MoveCount();
                    if (oldChild.HasChildren())
                    {
                        newChild->SetFirstChild(oldChild.FirstChild());
                        newChild->SetNuChildren(oldChild.NuChildren());
                    }
                }
                break;
            }
        }
    }
    nonConstNode.SetPosCount(parentCount);

    // Write order dependency: We do not want an SgUctChildIterator to
    // run past the end of a node's children, which can happen if one
    // is created between the two statements below. We modify node in
    // such a way so as to avoid that.
    SgSynchronizeThreadMemory();
    if (nonConstNode.NuChildren() < (int)nuNewChildren)
    {
        nonConstNode.SetFirstChild(newFirstChild);
        SgSynchronizeThreadMemory();
        nonConstNode.SetNuChildren(nuNewChildren);
    }
    else
    {
        nonConstNode.SetNuChildren(nuNewChildren);
        SgSynchronizeThreadMemory();
        nonConstNode.SetFirstChild(newFirstChild);
    }
}

std::size_t SgUctTree::NuNodes() const
{
    size_t nuNodes = 1; // Count root node
    for (size_t i = 0; i < NuAllocators(); ++i)
        nuNodes += Allocator(i).NuNodes();
    return nuNodes;
}

void SgUctTree::RemoveVirtualLoss(const std::vector<const SgUctNode*>& nodes)
{
    for (size_t i = 0; i < nodes.size(); ++i)
    {
        const SgUctNode* father = (i > 0 ? nodes[i-1] : 0);
        RemoveGameResult(*nodes[i], father, 1.0); // see AddVirtualLoss()
        RemoveRaveValue(*nodes[i], 0.0, 1.0);
    }
}

void SgUctTree::SetMaxNodes(std::size_t maxNodes)
{
    Clear();
    size_t nuAllocators = NuAllocators();
    if (nuAllocators == 0)
    {
        SgDebug() << "SgUctTree::SetMaxNodes: no allocators registered\n";
        SG_ASSERT(false);
        return;
    }
    m_maxNodes = maxNodes;
    size_t maxNodesPerAlloc = maxNodes / nuAllocators;
    for (size_t i = 0; i < NuAllocators(); ++i)
        Allocator(i).SetMaxNodes(maxNodesPerAlloc);
}

void SgUctTree::Swap(SgUctTree& tree)
{
    SG_ASSERT(MaxNodes() == tree.MaxNodes());
    SG_ASSERT(NuAllocators() == tree.NuAllocators());
    swap(m_root, tree.m_root);
    for (size_t i = 0; i < NuAllocators(); ++i)
        Allocator(i).Swap(tree.Allocator(i));
}

void SgUctTree::ThrowConsistencyError(const string& message) const
{
    DumpDebugInfo(SgDebug());
    throw SgException("SgUctTree::ThrowConsistencyError: " + message);
}

//----------------------------------------------------------------------------

SgUctTreeIterator::SgUctTreeIterator(const SgUctTree& tree)
    : m_tree(tree),
      m_current(&tree.Root())
{
}

const SgUctNode& SgUctTreeIterator::operator*() const
{
    return *m_current;
}

void SgUctTreeIterator::operator++()
{
    if (m_current->HasChildren())
    {
        SgUctChildIterator* it = new SgUctChildIterator(m_tree, *m_current);
        m_stack.push(shared_ptr<SgUctChildIterator>(it));
        m_current = &(**it);
        return;
    }
    while (! m_stack.empty())
    {
        SgUctChildIterator& it = *m_stack.top();
        SG_ASSERT(it);
        ++it;
        if (it)
        {
            m_current = &(*it);
            return;
        }
        else
        {
            m_stack.pop();
            m_current = 0;
        }
    }
    m_current = 0;
}

SgUctTreeIterator::operator bool() const
{
    return (m_current != 0);
}

//----------------------------------------------------------------------------
