//----------------------------------------------------------------------------
/** @file SgBookBuilder.cpp 
 */
//----------------------------------------------------------------------------

#include "SgBookBuilder.h"
#include "SgDebug.h"
#include "SgPoint.h"
#include "SgTimer.h"
#include <sstream>
#include <boost/numeric/conversion/bounds.hpp>

//----------------------------------------------------------------------------

//------------------------------------------------------------------------

/** Priority of newly created leaves. */
const float LEAF_PRIORITY = 0.0;

//------------------------------------------------------------------------

bool SgBookNode::IsTerminal() const
{
    if (m_value < -100 || m_value > 100)
        return true;
    return false;
}

bool SgBookNode::IsLeaf() const
{
    return m_count == 0;
}

std::string SgBookNode::ToString() const
{
    std::ostringstream os;
    os << std::showpos << std::fixed << std::setprecision(6);
    os << "Val " << m_value;
    os << std::noshowpos << " ExpP " << m_priority;
    os << std::showpos << " Heur " << m_heurValue << " Cnt " << m_count;
    return os.str();
}

void SgBookNode::FromString(const std::string& str)
{
    std::istringstream is(str);
    std::string dummy;
    is >> dummy;
    is >> m_value;
    is >> dummy;
    is >> m_priority;
    is >> dummy;
    is >> m_heurValue;
    is >> dummy;
    is >> m_count;
}

//----------------------------------------------------------------------------

SgBookBuilder::SgBookBuilder()
    : m_alpha(50),
      m_use_widening(true),
      m_expand_width(16),
      m_expand_threshold(1000),
      m_flush_iterations(100)
{
}

SgBookBuilder::~SgBookBuilder()
{
}

//----------------------------------------------------------------------------

void SgBookBuilder::StartIteration()
{
    // DEFAULT IMPLEMENTATION DOES NOTHING
}

void SgBookBuilder::EndIteration()
{
    // DEFAULT IMPLEMENTATION DOES NOTHING
}

void SgBookBuilder::BeforeEvaluateChildren()
{
    // DEFAULT IMPLEMENTATION DOES NOTHING
}

void SgBookBuilder::AfterEvaluateChildren()
{
    // DEFAULT IMPLEMENTATION DOES NOTHING
}

void SgBookBuilder::Init()
{
    // DEFAULT IMPLEMENTATION DOES NOTHING
}

void SgBookBuilder::Fini()
{
    // DEFAULT IMPLEMENTATION DOES NOTHING
}

void SgBookBuilder::Expand(int numExpansions)
{
    m_num_evals = 0;
    m_num_widenings = 0;

    SgTimer timer;
    Init();
    EnsureRootExists();
    int num = 0;
    for (; num < numExpansions; ++num) 
    {
        {
            std::ostringstream os;
            os << "\n--Iteration " << num << "--\n";
            PrintMessage(os.str());
        }
        {
            SgBookNode root;
            GetNode(root);
            if (root.IsTerminal()) 
            {
                PrintMessage("Root is terminal!\n");
                break;
            }
        }
        StartIteration();
        std::vector<SgMove> pv;
        DoExpansion(pv);
        EndIteration();

        if (num && (num % m_flush_iterations) == 0) 
            FlushBook();
    }
    FlushBook();
    Fini();
    timer.Stop();
    double elapsed = timer.GetTime();
    std::ostringstream os;
    os << '\n'
       << "Statistics\n"
       << "Total Time     " << elapsed << '\n'
       << "Expansions     " << num 
       << std::fixed << std::setprecision(2) 
       << " (" << (num / elapsed) << "/s)\n"
       << "Evaluations    " << m_num_evals 
       << std::fixed << std::setprecision(2)
       << " (" << (m_num_evals / elapsed) << "/s)\n"
       << "Widenings      " << m_num_widenings << '\n';
    PrintMessage(os.str());
}

/** TODO: handle terminal states! */
void SgBookBuilder::Cover(int requiredExpansions,
                          const std::vector< std::vector<SgMove> >& lines)
{
    m_num_evals = 0;
    m_num_widenings = 0;
    std::size_t newLines = 0;
    SgTimer timer;
    Init();
    int num = 0;
    for (std::size_t i = 0; i < lines.size(); ++i)
    {
        const std::size_t size = lines[i].size();
        for (std::size_t j = 0; j <= size; ++j)
        {
            int expansionsToDo = requiredExpansions;
            SgBookNode node;
            if (GetNode(node))
                expansionsToDo = requiredExpansions - node.m_count;
            else
            {
                EnsureRootExists();
                newLines++;
            }
            for (int k = 0; k < expansionsToDo; ++k)
            {
                {
                    std::ostringstream os;
                    os << "\n--Iteration " << num << ": " 
                       << (i + 1) << '/' << lines.size() << ' '
                       << (j + 1) << '/' << (size + 1) << ' '
                       << (k + 1) << '/' << expansionsToDo << "--\n";
                    PrintMessage(os.str());
                }

                StartIteration();
                std::vector<SgMove> pv;
                DoExpansion(pv);
                EndIteration();

                num++;
                if (num % m_flush_iterations == 0)
                    FlushBook();
            }
            if (j < lines[i].size())
                PlayMove(lines[i][j]);
        }
        for (std::size_t j = 0; j < size; ++j)
        {
            UndoMove(lines[i][size - 1 - j]);
            SgBookNode node;
            GetNode(node);
            UpdateValue(node);
            UpdatePriority(node);
        }
    }
    FlushBook();
    Fini();
    timer.Stop();
    double elapsed = timer.GetTime();
    std::ostringstream os;
    os << '\n'
       << "Statistics\n"
       << "Total Time     " << elapsed << '\n'
       << "Expansions     " << num 
       << std::fixed << std::setprecision(2) 
       << " (" << (num / elapsed) << "/s)\n"
       << "Evaluations    " << m_num_evals 
       << std::fixed << std::setprecision(2)
       << " (" << (m_num_evals / elapsed) << "/s)\n"
       << "Widenings      " << m_num_widenings << '\n'
       << "New Lines      " << newLines << '\n';
    PrintMessage(os.str());
}

void SgBookBuilder::Refresh()
{
    m_num_evals = 0;
    m_num_widenings = 0;
    m_value_updates = 0;
    m_priority_updates = 0;
    m_internal_nodes = 0;
    m_leaf_nodes = 0;
    m_terminal_nodes = 0;

    SgTimer timer;
    Init();
    ClearAllVisited();
    Refresh(true);
    FlushBook();
    Fini();
    timer.Stop();

    double elapsed = timer.GetTime();
    std::ostringstream os;
    os << '\n'
       << "Statistics\n"
       << "Total Time       " << elapsed << '\n'
       << "Value Updates    " << m_value_updates << '\n'
       << "Priority Updates " << m_priority_updates << '\n'
       << "Internal Nodes   " << m_internal_nodes << '\n'
       << "Terminal Nodes   " << m_terminal_nodes << '\n'
       << "Leaf Nodes       " << m_leaf_nodes << '\n'
       << "Evaluations      " << m_num_evals 
       << std::fixed << std::setprecision(2)
       << " (" << (m_num_evals / elapsed) << "/s)\n"
       << "Widenings        " << m_num_widenings << '\n';
    PrintMessage(os.str());
}

void SgBookBuilder::IncreaseWidth()
{
    if (!m_use_widening)
    {
        PrintMessage("Widening not enabled!\n");
        return;
    }

    m_num_evals = 0;
    m_num_widenings = 0;

    SgTimer timer;
    Init();
    ClearAllVisited();
    IncreaseWidth(true);
    FlushBook();
    Fini();
    timer.Stop();
    double elapsed = timer.GetTime();

    std::ostringstream os;
    os << '\n'
       << "Statistics\n"
       << "Total Time     " << elapsed << '\n'
       << "Widenings      " << m_num_widenings << '\n'
       << "Evaluations    " << m_num_evals 
       << std::fixed << std::setprecision(2)
       << " (" << (m_num_evals / elapsed) << "/s)\n";
    PrintMessage(os.str());
}

//----------------------------------------------------------------------------

/** Creates a node for each of the leaf's first count children that
    have not been created yet. Returns true if at least one new node
    was created, false otherwise. */
bool SgBookBuilder::ExpandChildren(std::size_t count)
{
    // It is possible the state is determined, even though it was
    // already evaluated. This can happen in Hex: it is not very likey
    // if the evaluation function is reasonably heavyweight, but if
    // just using fillin and vcs, it is possible that the fillin
    // prevents a winning vc from being created.
    float value = 0;
    std::vector<SgMove> children;
    if (GenerateMoves(children, value))
    {
        PrintMessage("ExpandChildren: State is determined!\n");
        WriteNode(SgBookNode(value));
        return false;
    }
    std::size_t limit = std::min(count, children.size());
    std::vector<SgMove> childrenToDo;
    for (std::size_t i = 0; i < limit; ++i)
    {
        PlayMove(children[i]);
        SgBookNode child;
        if (!GetNode(child))
            childrenToDo.push_back(children[i]);
        UndoMove(children[i]);
    }
    if (!childrenToDo.empty())
    {
        BeforeEvaluateChildren();
        std::vector<std::pair<SgMove, float> > scores;
        EvaluateChildren(childrenToDo, scores);
        AfterEvaluateChildren();
        for (std::size_t i = 0; i < scores.size(); ++i)
        {
            PlayMove(scores[i].first);
            WriteNode(scores[i].second);
            UndoMove(scores[i].first);
        }
        m_num_evals += childrenToDo.size();
        return true;
    }
    else
        PrintMessage("Children already evaluated.\n");
    return false;
}

std::size_t SgBookBuilder::NumChildren(const std::vector<SgMove>& legal)
{
    std::size_t num = 0;
    for (size_t i = 0; i < legal.size(); ++i) 
    {
	PlayMove(legal[i]);
	SgBookNode child;
        if (GetNode(child))
            ++num;
        UndoMove(legal[i]);
    }
    return num;
}

void SgBookBuilder::UpdateValue(SgBookNode& node, 
                                const std::vector<SgMove>& legal)
{
    bool hasChild = false;
    float bestValue = boost::numeric::bounds<float>::lowest();
    for (std::size_t i = 0; i < legal.size(); ++i)
    {
	PlayMove(legal[i]);
	SgBookNode child;
        if (GetNode(child))
        {
            hasChild = true;
            float value = InverseEval(Value(child));
            if (value > bestValue)
		bestValue = value;
        }
        UndoMove(legal[i]);
    }
    if (hasChild)
        node.m_value = bestValue;
}

/** Updates the node's value, taking special care if the value is a
    loss. In this case, widenings are performed until a non-loss child
    is added or no new children are added. The node is then set with
    the proper value. */
void SgBookBuilder::UpdateValue(SgBookNode& node)
{
    while (true)
    {
        std::vector<SgMove> legal;
        GetAllLegalMoves(legal);
        UpdateValue(node, legal);
        if (!IsLoss(Value(node)))
            break;
        // Round up to next nearest multiple of m_expand_width that is
        // larger than the current number of children.
        std::size_t numChildren = NumChildren(legal);
        std::size_t width = (numChildren / m_expand_width + 1) 
            * m_expand_width;
        {
            std::ostringstream os;
            os << "Forced Widening[" << numChildren << "->" << width << "]\n";
            PrintMessage(os.str());
        }
        if (!ExpandChildren(width))
            break;
        ++m_num_widenings;
    }
}

float SgBookBuilder::ComputePriority(const SgBookNode& parent,
                                     const float childValue,
                                     const float childPriority) const
{
    float delta = parent.m_value - InverseEval(childValue);
    SG_ASSERT(delta >= 0.0);
    return m_alpha * delta + childPriority + 1;
}

/** Re-computes node's priority and returns the best child to
    expand. Requires that UpdateValue() has been called on this
    node. Returns SG_NULLMOVE if node has no children. */
SgMove SgBookBuilder::UpdatePriority(SgBookNode& node)
{
    bool hasChild = false;
    float bestPriority = boost::numeric::bounds<float>::highest();
    SgMove bestChild = SG_NULLMOVE;
    std::vector<SgMove> legal;
    GetAllLegalMoves(legal);
    for (std::size_t i = 0; i < legal.size(); ++i)
    {
	PlayMove(legal[i]);
	SgBookNode child;
        if (GetNode(child))
        {
            hasChild = true;
            // Must adjust child value for swap, but not the parent
            // because we are comparing with the best child's value,
            // ie, the minmax value.
            float childValue = Value(child);
            float childPriority = child.m_priority;
            float priority = ComputePriority(node, childValue, childPriority);
            if (priority < bestPriority)
            {
                bestPriority = priority;
                bestChild = legal[i];
            }
        }
        UndoMove(legal[i]);
    }
    if (hasChild)
        node.m_priority = bestPriority;
    return bestChild;
}

void SgBookBuilder::DoExpansion(std::vector<SgMove>& pv)
{
    SgBookNode node;
    if (!GetNode(node))
        SG_ASSERT(false);
    if (node.IsTerminal())
        return;
    if (node.IsLeaf())
    {
        ExpandChildren(m_expand_width);
    }
    else
    {
        // Widen this non-terminal non-leaf node if necessary
        if (m_use_widening && (node.m_count % m_expand_threshold == 0))
        {
            std::size_t width = (node.m_count / m_expand_threshold + 1)
                              * m_expand_width;
            ++m_num_widenings;
            ExpandChildren(width);
        }
        // Compute value and priority. It's possible this node is newly
        // terminal if one of its new children is a winning move.
        GetNode(node);
        UpdateValue(node);
        SgMove mostUrgent = UpdatePriority(node);
        WriteNode(node);

        // Recurse on most urgent child only if non-terminal.
        if (!node.IsTerminal())
        {
            PlayMove(mostUrgent);
            pv.push_back(mostUrgent);
            DoExpansion(pv);
            pv.pop_back();
            UndoMove(mostUrgent);
        }
    }
    GetNode(node);
    UpdateValue(node);
    UpdatePriority(node);
    node.IncrementCount();
    WriteNode(node);
}

//----------------------------------------------------------------------------

/** Refresh's each child of the given state. UpdateValue() and
    UpdatePriority() are called on internal nodes. Returns true if
    state exists in book.  
    @ref bookrefresh
*/
bool SgBookBuilder::Refresh(bool root)
{
    if (HasBeenVisited())
        return true;
    MarkAsVisited();
    SgBookNode node;
    if (!GetNode(node))
        return false;
    if (node.IsLeaf())
    {
        m_leaf_nodes++;
        if (node.IsTerminal())
            m_terminal_nodes++;
        return true;
    }
    double oldValue = Value(node);
    double oldPriority = node.m_priority;
    std::vector<SgMove> legal;
    GetAllLegalMoves(legal);
    for (std::size_t i = 0; i < legal.size(); ++i)
    {
        PlayMove(legal[i]);
        Refresh(false);
        if (root)
        {
            std::ostringstream os;
            os << "Finished " << SgWritePoint(legal[i]) << '\n';
            PrintMessage(os.str());
        }
        UndoMove(legal[i]);
    }
    UpdateValue(node);
    UpdatePriority(node);
    if (fabs(oldValue - Value(node)) > 0.0001)
        m_value_updates++;
    if (fabs(oldPriority - node.m_priority) > 0.0001)
        m_priority_updates++;
    WriteNode(node);
    if (node.IsTerminal())
        m_terminal_nodes++;
    else
        m_internal_nodes++;
    return true;
}

//----------------------------------------------------------------------------

void SgBookBuilder::IncreaseWidth(bool root)
{
    if (HasBeenVisited())
        return;
    MarkAsVisited();
    SgBookNode node;
    if (!GetNode(node))
        return;
    if (node.IsTerminal() || node.IsLeaf())
        return;
    std::vector<SgMove> legal;
    GetAllLegalMoves(legal);
    for (std::size_t i = 0; i < legal.size(); ++i)
    {
        PlayMove(legal[i]);
        IncreaseWidth(false);
        if (root)
        {
            std::ostringstream os;
            os << "Finished " << SgWritePoint(legal[i]) << '\n';
            PrintMessage(os.str());
        }
        UndoMove(legal[i]);
    }
    std::size_t width = (node.m_count / m_expand_threshold + 1)
        * m_expand_width;
    if (ExpandChildren(width))
        ++m_num_widenings;
}

//----------------------------------------------------------------------------
