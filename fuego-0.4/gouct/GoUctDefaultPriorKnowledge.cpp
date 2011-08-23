//----------------------------------------------------------------------------
/** @file GoUctDefaultPriorKnowledge.cpp
    See GoUctDefaultPriorKnowledge.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctDefaultPriorKnowledge.h"

using namespace std;

//----------------------------------------------------------------------------

namespace {

bool SetsAtari(const GoBoard& bd, SgPoint p)
{
    SG_ASSERT(bd.IsEmpty(p)); // Already checked
    SgBlackWhite opp = SgOppBW(bd.ToPlay());
    if (bd.NumNeighbors(p, opp) == 0)
        return false;
    if (! bd.IsBorder(p + SG_NS) && bd.GetColor(p + SG_NS) == opp
        && bd.NumLiberties(p + SG_NS) == 2)
        return true;
    if (! bd.IsBorder(p - SG_NS) && bd.GetColor(p - SG_NS) == opp
        && bd.NumLiberties(p - SG_NS) == 2)
        return true;
    if (! bd.IsBorder(p + SG_WE) && bd.GetColor(p + SG_WE) == opp
        && bd.NumLiberties(p + SG_WE) == 2)
        return true;
    if (! bd.IsBorder(p - SG_WE) && bd.GetColor(p - SG_WE) == opp
        && bd.NumLiberties(p - SG_WE) == 2)
        return true;
    return false;
}

} // namespace

//----------------------------------------------------------------------------

GoUctKnowledge::GoUctKnowledge(const GoBoard& bd)
    : m_bd(bd)
{
}

GoUctKnowledge::~GoUctKnowledge()
{
}

void GoUctKnowledge::Add(SgPoint p, float value, size_t count)
{
    m_values[p].Add(value, count);
}

void GoUctKnowledge::Initialize(SgPoint p, float value, size_t count)
{
    m_values[p].Initialize(value, count);
}

void GoUctKnowledge::ClearValues()
{
    for (std::size_t i = 0; i < SG_PASS+1; ++i)
        m_values[i].Clear();
}

void GoUctKnowledge::TransferValues(std::vector<SgMoveInfo>& outmoves) const
{
    for (std::size_t i = 0; i < outmoves.size(); ++i) 
    {
        SgMove p = outmoves[i].m_move;
        if (m_values[p].IsDefined())
        {
            outmoves[i].m_count = m_values[p].Count();
            outmoves[i].m_value =
                SgUctSearch::InverseEval(m_values[p].Mean());
            outmoves[i].m_raveCount = m_values[p].Count();
            outmoves[i].m_raveValue = m_values[p].Mean();
        }
    }
}

//----------------------------------------------------------------------------

GoUctDefaultPriorKnowledge::GoUctDefaultPriorKnowledge(const GoBoard& bd,
                              const GoUctPlayoutPolicyParam& param)
    : GoUctKnowledge(bd),
      m_policy(bd, param)
{
}

void GoUctDefaultPriorKnowledge::AddLocalityBonus(GoPointList& emptyPoints,
                                                  bool isSmallBoard)
{
    SgPoint last = m_bd.GetLastMove();
    if (last != SG_NULLMOVE && last != SG_PASS)
    {
        SgPointArray<int> dist = GoBoardUtil::CfgDistance(m_bd, last, 3);
        const size_t count = (isSmallBoard ? 4 : 5);
        for (GoPointList::Iterator it(emptyPoints); it; ++it)
        {
            const SgPoint p = *it;
            switch (dist[p])
            {
            case 1:
                Add(p, 1.0, count);
                break;
            case 2:
                Add(p, 0.6, count);
                break;
            case 3:
                Add(p, 0.6, count);
                break;
            default:
                Add(p, 0.1, count);
                break;
            }
        }
        Add(SG_PASS, 0.1, count);
    }
}

/** Find global moves that match a playout pattern or set a block into atari.
    @param[out] pattern
    @param[out] atari
    @param[out] empty As a side effect, this function finds all empty points
    on the board
    @return @c true if any such moves was found
*/
bool GoUctDefaultPriorKnowledge::FindGlobalPatternAndAtariMoves(
                                                     SgPointSet& pattern,
                                                     SgPointSet& atari,
                                                     GoPointList& empty) const
{
    SG_ASSERT(empty.IsEmpty());
    const GoUctPatterns<GoBoard>& patterns = m_policy.Patterns();
    bool result = false;
    for (GoBoard::Iterator it(m_bd); it; ++it)
        if (m_bd.IsEmpty(*it))
        {
            empty.PushBack(*it);
            if (patterns.MatchAny(*it))
            {
                pattern.Include(*it);
                result = true;
            }
            if (SetsAtari(m_bd, *it))
            {
                atari.Include(*it);
                result = true;
            }
        }
    return result;
}

float (*defaultValueHeuristic)(const GoBoard *board, SgPoint p) = NULL;

void 
GoUctDefaultPriorKnowledge::ProcessPosition(std::vector<SgMoveInfo>& outmoves)
{
	//if(defaultValueHeuristic) cout << "Using hyperneat\n";
    m_policy.StartPlayout();
    m_policy.GenerateMove();
    GoUctPlayoutPolicyType type = m_policy.MoveType();
    bool isFullBoardRandom =
        (type == GOUCT_RANDOM || type == GOUCT_FILLBOARD);
    SgPointSet pattern;
    SgPointSet atari;
    GoPointList empty;
    bool anyHeuristic = FindGlobalPatternAndAtariMoves(pattern, atari, empty);

    // The initialization values/counts are mainly tuned by selfplay
    // experiments and games vs MoGo Rel 3 and GNU Go 3.6 on 9x9 and 19x19.
    // If different values are used for the small and large board, the ones
    // from the 9x9 experiments are used for board sizes < 15, the ones from
    // 19x19 otherwise.
    const bool isSmallBoard = (m_bd.Size() < 15);

    Initialize(SG_PASS, 0.1, isSmallBoard ? 9 : 18);
    if (isFullBoardRandom && ! anyHeuristic)
    {
        for (GoBoard::Iterator it(m_bd); it; ++it)
        {
            SgPoint p = *it;
            if (! m_bd.IsEmpty(p))
                continue;
            if (GoBoardUtil::SelfAtari(m_bd, *it))
                Initialize(*it, 0.1, isSmallBoard ? 9 : 18);
            else
                m_values[p].Clear(); // Don't initialize
        }
    }
    else if (isFullBoardRandom && anyHeuristic)
    {
        for (GoBoard::Iterator it(m_bd); it; ++it)
        {
            SgPoint p = *it;
            if (! m_bd.IsEmpty(p))
                continue;
            if (GoBoardUtil::SelfAtari(m_bd, *it))
                Initialize(*it, 0.1, isSmallBoard ? 9 : 18);
            else if (atari[*it])
                Initialize(*it, 1.0, 3);
            else if (pattern[*it] && defaultValueHeuristic==NULL)
                Initialize(*it, 0.9, 3);
            else
            {
                float value = 0.5;
                float moveCountMult = 1.0;
                if(defaultValueHeuristic)
                {
                    //cout << "Using heuristic to override value " << value;
                    value = defaultValueHeuristic(&m_bd,p);
                    moveCountMult = 2.0f;
                    //cout << " to value " << value << endl;
                }
                //JJG: THIS IS WHERE YOU WOULD PUT HYPERNEAT VALUES!!!
                Initialize(*it, value, int(3*moveCountMult) );
            }
        }
    }
    else
    {
        for (GoBoard::Iterator it(m_bd); it; ++it)
        {
            SgPoint p = *it;
            if (! m_bd.IsEmpty(p))
                continue;
            if (GoBoardUtil::SelfAtari(m_bd, *it))
                Initialize(*it, 0.1, isSmallBoard ? 9 : 18);
            else if (atari[*it])
                Initialize(*it, 0.8, isSmallBoard ? 9 : 18);
            else if (pattern[*it] && defaultValueHeuristic==NULL)
                Initialize(*it, 0.6, isSmallBoard ? 9 : 18);
            else
            {
                float value = 0.4;
                float moveCountMult = 1.0;
                if(defaultValueHeuristic)
                {
                    //cout << "Using heuristic to override value " << value;
                    value = defaultValueHeuristic(&m_bd,p);
                    moveCountMult = 2.0f;
                    //cout << " to value " << value << endl;
                }
                //JJG: THIS IS WHERE YOU WOULD PUT HYPERNEAT VALUES!!!
                Initialize(*it, value, isSmallBoard ? int(9*moveCountMult) : int(18*moveCountMult) );
            }
        }
        GoPointList moves = m_policy.GetEquivalentBestMoves();
        for (GoPointList::Iterator it(moves); it; ++it)
            Initialize(*it, 1.0, isSmallBoard ? 9 : 18);
    }
    //AddLocalityBonus(empty, isSmallBoard);
    m_policy.EndPlayout();

    TransferValues(outmoves);
}

//----------------------------------------------------------------------------
