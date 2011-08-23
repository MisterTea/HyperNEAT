//----------------------------------------------------------------------------
/** @file GoUctPureRandomGenerator.h */
//----------------------------------------------------------------------------

#ifndef GOUCT_PURERANDOMGENERATOR_H
#define GOUCT_PURERANDOMGENERATOR_H

#include <vector>
#include "GoBoard.h"
#include "GoUctUtil.h"
#include "SgWrite.h"

//----------------------------------------------------------------------------

/** Randomly select from empty points on the board.
    Finds and shuffles the empty points on the board at the beginning
    to avoid repeated loops over the board to find empty points.
*/
template<class BOARD>
class GoUctPureRandomGenerator
{
public:
    GoUctPureRandomGenerator(const BOARD& bd, SgRandom& random);

    /** Finds and shuffles the empty points currently on the board. */
    void Start();

    /** Update state.
        Must be called after each play on the board.
    */
    void OnPlay();

    /** Return a list of points that are currently potentially empty.
        As a side-benefit, the generator can be used to get the list of empty
        points on the board to speed up full-board loops over empty points
        or to get a shuffled list of the empty points (e.g. for finding
        legal moves when expanding a node in the in-tree-phase of UCT).
        Points in the list are candidates, they still have to be tested, if
        they are really empty.
    */
    const std::vector<SgPoint>& Candidates() const;

    /** Generate a pure random move.
        Randomly select an empty point on the board that fulfills
        GoUctUtil::GeneratePoint() for the color currently to play on the
        board.
    */
    SgPoint Generate(SgBalancer& balancer);

    /** Generate a move using the fillboard heuristic.
        Tries @c numberTries times to select a point on the board and
        returns it, if it is empty and all adjacent and diagonal neighbors are
        empty. Otherwise it returns SG_NULLMOVE. Using this heuristic before
        any other heuristics is helpful to increase playout diversity on large
        boards. See section 6.1 of:
        Chatriot, Gelly, Hoock, Perez, Rimmel, Teytaud:
        <a href="http://www.lri.fr/~teytaud/eg.pdf">
        Combining expert, offline, transient and online learning in
        Monte-Carlo exploration</a>
     */
    SgPoint GenerateFillboardMove(int numberTries);

private:
    const BOARD& m_bd;

    float m_invNuPoints;

    float m_nuEmptyFloat;

    SgRandom& m_random;

    /** Points that are potentially empty. */
    std::vector<SgPoint> m_candidates;

    bool Empty3x3(SgPoint p) const;

    void CheckConsistency() const;

    void Insert(SgPoint p);
};

template<class BOARD>
GoUctPureRandomGenerator<BOARD>::GoUctPureRandomGenerator(const BOARD& bd,
                                                          SgRandom& random)
    : m_bd(bd),
      m_random(random)
{
    m_candidates.reserve(GO_MAX_NUM_MOVES);
}

template<class BOARD>
inline const std::vector<SgPoint>&
GoUctPureRandomGenerator<BOARD>::Candidates()
    const
{
    return m_candidates;
}

template<class BOARD>
inline void GoUctPureRandomGenerator<BOARD>::CheckConsistency() const
{
#if 0 // Expensive check, enable only for debugging
    for (GoBoard::Iterator it(m_bd); it; ++it)
    {
        SgPoint p = *it;
        if (m_bd.IsEmpty(p))
            if (find(m_candidates.begin(), m_candidates.end(), p)
                == m_candidates.end())
            {
                SgDebug() << m_bd
                          << "Candidates: " << SgWritePointList(m_candidates)
                          << "does not contain: " << SgWritePoint(p)
                          << "\nm_bd.CapturedStones(): "
                          << SgWriteSPointList<SG_MAX_ONBOARD + 1>
                                                   (m_bd.CapturedStones())
                          << "Last move: "
                          << SgWritePoint(m_bd.GetLastMove()) << '\n';
                SG_ASSERT(false);
            }
    }
#endif
}

template<class BOARD>
inline bool GoUctPureRandomGenerator<BOARD>::Empty3x3(SgPoint p)
    const
{
    return (m_bd.NumEmptyNeighbors(p) == 4
            && m_bd.NumEmptyDiagonals(p) == 4);
}

template<class BOARD>
inline SgPoint GoUctPureRandomGenerator<BOARD>::Generate(SgBalancer& balancer)
{
    CheckConsistency();
    SgBlackWhite toPlay = m_bd.ToPlay();
    size_t i = m_candidates.size();
    while (true)
    {
        if (i == 0)
            break;
        --i;
        SgPoint p = m_candidates[i];
        if (! m_bd.IsEmpty(p))
        {
            m_candidates[i] = m_candidates[m_candidates.size() - 1];
            m_candidates.pop_back();
            continue;
        }
        if (GoUctUtil::GeneratePoint(m_bd, balancer, p, toPlay))
        {
            CheckConsistency();
            return p;
        }
    }
    CheckConsistency();
    return SG_NULLMOVE;
}

template<class BOARD>
inline SgPoint GoUctPureRandomGenerator<BOARD>::GenerateFillboardMove(
                                                              int numberTries)
{
    float effectiveTries = numberTries * m_nuEmptyFloat * m_invNuPoints;
    size_t i = m_candidates.size();
    while (effectiveTries > 1.f)
    {
        if (i == 0)
            return SG_NULLMOVE;
        --i;
        SgPoint p = m_candidates[i];
        if (! m_bd.IsEmpty(p))
        {
            m_candidates[i] = m_candidates[m_candidates.size() - 1];
            m_candidates.pop_back();
            continue;
        }
        if (Empty3x3(p))
            return p;
        effectiveTries -= 1.f;
    }
    // Remaning fractional number of tries
    if (m_random.Int(100) > 100 * effectiveTries)
        return SG_NULLMOVE;
    while (true)
    {
        if (i == 0)
            break;
        --i;
        SgPoint p = m_candidates[i];
        if (! m_bd.IsEmpty(p))
        {
            m_candidates[i] = m_candidates[m_candidates.size() - 1];
            m_candidates.pop_back();
            continue;
        }
        if (Empty3x3(p))
            return p;
        break;
    }
    return SG_NULLMOVE;
}

template<class BOARD>
inline void GoUctPureRandomGenerator<BOARD>::OnPlay()
{
    SgPoint lastMove = m_bd.GetLastMove();
    if (lastMove != SG_NULLMOVE && lastMove != SG_PASS
        && ! m_bd.IsEmpty(lastMove))
        m_nuEmptyFloat -= 1.f;
    const GoPointList& capturedStones = m_bd.CapturedStones();
    if (! capturedStones.IsEmpty())
    {
        // Don't remove stone played, too expensive, check later in Generate()
        // that generated point is still empty
        for (GoPointList::Iterator it(capturedStones); it; ++it)
            Insert(*it);
        m_nuEmptyFloat += capturedStones.Length();
    }
    CheckConsistency();
}

/** Insert new candidate at random place. */
template<class BOARD>
inline void GoUctPureRandomGenerator<BOARD>::Insert(SgPoint p)
{
    size_t size = m_candidates.size();
    if (size == 0)
        m_candidates.push_back(p);
    else
    {
        SgPoint& swapPoint = m_candidates[m_random.Int(size)];
        m_candidates.push_back(swapPoint);
        swapPoint = p;
    }
}

template<class BOARD>
inline void GoUctPureRandomGenerator<BOARD>::Start()
{
    m_nuEmptyFloat = 0;
    m_candidates.clear();
    for (typename BOARD::Iterator it(m_bd); it; ++it)
        if (m_bd.IsEmpty(*it))
        {
            Insert(*it);
            m_nuEmptyFloat += 1.f;
        }
    m_invNuPoints = 1.f / (m_bd.Size() * m_bd.Size());
    CheckConsistency();
}

//----------------------------------------------------------------------------

#endif // GOUCT_PURERANDOMGENERATOR_H
