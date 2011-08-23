//----------------------------------------------------------------------------
/** @file SgUctTreeUtil.cpp
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgUctTreeUtil.h"

#include <iomanip>
#include "SgUctSearch.h"
#include "SgWrite.h"

using namespace std;

//----------------------------------------------------------------------------

SgUctTreeStatistics::SgUctTreeStatistics()
{
    Clear();
}

void SgUctTreeStatistics::Clear()
{
    m_nuNodes = 0;
    for (size_t i = 0; i < MAX_MOVECOUNT; ++i)
        m_moveCounts[i] = 0;
    m_biasRave.Clear();
}

void SgUctTreeStatistics::Compute(const SgUctTree& tree)
{
    Clear();
    for (SgUctTreeIterator it(tree); it; ++it)
    {
        const SgUctNode& node = *it;
        ++m_nuNodes;
        size_t count = node.MoveCount();
        if (count < SgUctTreeStatistics::MAX_MOVECOUNT)
            ++m_moveCounts[count];
        if (! node.HasChildren())
            continue;
        for (SgUctChildIterator it(tree, node); it; ++it)
        {
            const SgUctNode& child = *it;
            if (child.HasRaveValue() && child.HasMean())
            {
                float childValue = SgUctSearch::InverseEval(child.Mean());
                float biasRave = child.RaveValue() - childValue;
                m_biasRave.Add(biasRave);
            }
        }
    }
}

void SgUctTreeStatistics::Write(ostream& out) const
{
    out << SgWriteLabel("NuNodes") << m_nuNodes << '\n';
    for (size_t i = 0; i < MAX_MOVECOUNT; ++i)
    {
        ostringstream label;
        label << "MoveCount[" << i << ']';
        int percent = m_moveCounts[i] * 100 / m_nuNodes;
        out << SgWriteLabel(label.str()) << setw(2) << right << percent
            << "%\n";
    }
    out << SgWriteLabel("BiasRave");
    m_biasRave.Write(out);
    out << '\n';
}

std::ostream& operator<<(ostream& out, const SgUctTreeStatistics& stat)
{
    stat.Write(out);
    return out;
}

//----------------------------------------------------------------------------

void SgUctTreeUtil::ExtractSubtree(const SgUctTree& tree, SgUctTree& target,
                                   const std::vector<SgMove>& sequence,
                                   bool warnTruncate, double maxTime)
{
    target.Clear();
    const SgUctNode* node = &tree.Root();
    for (vector<SgMove>::const_iterator it = sequence.begin();
         it != sequence.end(); ++it)
    {
        SgMove mv = *it;
        node = SgUctTreeUtil::FindChildWithMove(tree, *node, mv);
        if (node == 0)
            return;
    }
    tree.ExtractSubtree(target, *node, warnTruncate, maxTime);
}

const SgUctNode* SgUctTreeUtil::FindChildWithMove(const SgUctTree& tree,
                                                  const SgUctNode& node,
                                                  SgMove move)
{
    if (! node.HasChildren())
        return 0;
    for (SgUctChildIterator it(tree, node); it; ++it)
    {
        const SgUctNode& child = *it;
        if (child.Move() == move)
            return &child;
    }
    return 0;
}

//----------------------------------------------------------------------------
