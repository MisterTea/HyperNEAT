//----------------------------------------------------------------------------
/** @file GoUctPlayoutPolicy.h
*/
//----------------------------------------------------------------------------

#ifndef GOUCT_PLAYOUTPOLICY_H
#define GOUCT_PLAYOUTPOLICY_H

#include <iostream>
#include <boost/array.hpp>
#include "GoBoardUtil.h"
#include "GoEyeUtil.h"
#include "GoUctPatterns.h"
#include "GoUctPureRandomGenerator.h"

//----------------------------------------------------------------------------

/** Parameters for GoUctPlayoutPolicy. */
class GoUctPlayoutPolicyParam
{
public:
    /** Enable collection of statistics.
        Has a negative impact on performance. Default is false.
    */
    bool m_statisticsEnabled;

    /** Use Nakade heuristic.
        See section 6.2 of: Chatriot, Gelly, Hoock, Perez, Rimmel, Teytaud:
        <a href="http://www.lri.fr/~teytaud/eg.pdf">
        Combining expert, offline, transient and online learning in
        Monte-Carlo exploration</a>
    */
    bool m_useNakadeHeuristic;

    /** See GoUctPureRandomGenerator::GenerateFillboardMove.
        Default is 0
    */
    int m_fillboardTries;

    GoUctPlayoutPolicyParam();
};

//----------------------------------------------------------------------------

/** Move types used in GoUctPlayoutPolicy. */
enum GoUctPlayoutPolicyType
{
    GOUCT_FILLBOARD,

    GOUCT_NAKADE,

    GOUCT_ATARI_CAPTURE,

    GOUCT_ATARI_DEFEND,

    GOUCT_LOWLIB,

    GOUCT_PATTERN,

    GOUCT_CAPTURE,

    GOUCT_RANDOM,

    GOUCT_SELFATARI_CORRECTION,

    GOUCT_CLUMP_CORRECTION,

    GOUCT_PASS,

    _GOUCT_NU_DEFAULT_PLAYOUT_TYPE
};

const char* GoUctPlayoutPolicyTypeStr(GoUctPlayoutPolicyType type);

//----------------------------------------------------------------------------

/** Statistics collected by GoUctPlayoutPolicy */
struct GoUctPlayoutPolicyStat
{
    /** Number of moves generated. */
    std::size_t m_nuMoves;

    /** Length of sequences of consecutive non-pure-random moves. */
    SgUctStatistics m_nonRandLen;

    /** Length of list of equivalent best moves.
        Does not include the length of the move list for pure random moves.
    */
    SgUctStatistics m_moveListLen;

    /** Number of moves of a certain type. */
    boost::array<std::size_t,_GOUCT_NU_DEFAULT_PLAYOUT_TYPE> m_nuMoveType;

    void Clear();

    void Write(std::ostream& out) const;
};

//----------------------------------------------------------------------------

/** Default playout policy for usage in GoUctGlobalSearch.
    Parametrized by the board class to make it usable with both GoBoard
    and GoUctBoard.
    If all heuristics are disabled, the policy plays purely random moves.
    The order and types of the heuristics are inspired by the first
    technical report about the MoGo program.
    Instances of this class must be thread-safe during a search.
*/
template<class BOARD>
class GoUctPlayoutPolicy
{
public:
    /** Constructor.
        @param bd
        @param param The parameters. The policy stores a reference to @c param
        to allow changing the parameters of a group of playout policies later.
        Therefore the lifetime of @c param must exceed the lifetime of the
        policy.
    */
    GoUctPlayoutPolicy(const BOARD& bd, const GoUctPlayoutPolicyParam& param);


    /** @name Functions needed by all playout policies. */
    // @{

    /** Generate a move
        Generates a random move in the following order:
        -# Atari heuristic (if enabled)
        -# Proximity heuristic (if enabled) (using patterns if enabled)
        -# Capture heuristic (if enabled)
        -# Purely random
    */
    SgPoint GenerateMove();

    void EndPlayout();

    void StartPlayout();

    void OnPlay();

    /** Return the type of the last move generated. */
    GoUctPlayoutPolicyType MoveType() const;

    // @} // @name


    /** @name Statistics */
    // @{

    /** Return current statistics.
        The statistics are only collected, if enabled with
        EnableStatistics().
    */
    //const GoUctPlayoutPolicyStat& Statistics() const;
	const GoUctPlayoutPolicyStat& Statistics(SgBlackWhite color) const;

    void ClearStatistics();

    // @} // @name


    /** Return the list of equivalent best moves from last move generation.
        The played move was randomly selected from this list.
    */
    GoPointList GetEquivalentBestMoves() const;

    /** Make pattern matcher available for other uses.
        Avoids that a user of the playout policy who also wants to use the
        pattern matcher for other purposes needs to allocate a second
        matcher (Use case: prior knowledge)
    */
    const GoUctPatterns<BOARD>& Patterns() const;

private:
    /** A function that possibly corrects a given point */
    typedef bool (*Corrector)(const BOARD&, SgPoint&);

    /** Incrementally keeps track of blocks in atari. */
    class CaptureGenerator
    {
    public:
        CaptureGenerator(const BOARD& bd);

        void StartPlayout();

        void OnPlay();

        /** Generate capture moves.
            @param[out] moves The resulting list of capture moves. The passed
            in list is expected to be empty.
        */
        void Generate(GoPointList& moves);

    private:
        const BOARD& m_bd;

        /** Anchor stones of blocks that need to be checked for atari. */
        std::vector<SgPoint> m_candidates;
    };

    /** Use patterns around last own move, too */
    static const bool SECOND_LAST_MOVE_PATTERNS = true;

    /** Shift move to neighbor if it would make an ugly clump.
        See GoUctUtil::DoClumpCorrection
    */
    static const bool USE_CLUMP_CORRECTION = false;

    static const bool DEBUG_CORRECT_MOVE = false;

    const BOARD& m_bd;

    const GoUctPlayoutPolicyParam& m_param;

    GoUctPatterns<BOARD> m_patterns;

    /** m_moves have already been checked, skip GeneratePoint test.  */
    bool m_checked;

    /** Type of the last generated move. */
    GoUctPlayoutPolicyType m_moveType;

    /** See GoUctPlayoutPolicyStat::m_nonRandLen. */
    std::size_t m_nonRandLen;

    /** Last move.
        Stored in member variable to avoid multiple calls to
        GoBoard::GetLastMove during GenerateMove.
    */
    SgPoint m_lastMove;

    /** List of equivalent best moves generated by the policy.
        The highest priority heuristic will generate all moves in this list.
        Moves in this list are not yet checked, if they are legal.
        This list is not used in GenerateMove(), if a pure random move
        is generated.
    */
    GoPointList m_moves;

    SgRandom m_random;

    /** Balancer for GoUctUtil::IsMutualAtari(). */
    mutable SgBalancer m_balancer;
    
    CaptureGenerator m_captureGenerator;

    GoUctPureRandomGenerator<BOARD> m_pureRandomGenerator;

    SgBWArray<GoUctPlayoutPolicyStat> m_statistics;

    /** Try to correct the proposed move, typically by moving it to a
        'better' point such as other liberty or neighbor.
        Examples implemented: self-ataries, clumps.
    */
    bool CorrectMove(Corrector corrFunction,
                     SgPoint& mv, GoUctPlayoutPolicyType moveType);

    /** Captures if last move was self-atari */
    bool GenerateAtariCaptureMove();

    /** Generate escapes if last move was atari. */
    bool GenerateAtariDefenseMove();

    /** Generate low lib moves around lastMove */
    bool GenerateLowLibMove(SgPoint lastMove);

    bool GenerateNakadeMove();

    void GenerateNakadeMove(SgPoint p);

    /** Generate pattern move around last two moves */
    bool GeneratePatternMove();

    void GeneratePatternMove(SgPoint p);

    void GeneratePatternMove2(SgPoint p, SgPoint lastMove);

    void GeneratePureRandom();

    bool GeneratePoint(SgPoint p) const;

    /** Does playing on a liberty increase number of liberties for block?
        If yes, add to m_moves.
        Disabled if both liberties are simple chain libs, e.g. bamboo.
    */
    void PlayGoodLiberties(SgPoint block);

    /** see GoUctUtil::SelectRandom */
    SgPoint SelectRandom();

    /** Add statistics for most recently generated move. */
    void UpdateStatistics();
};

template<class BOARD>
GoUctPlayoutPolicy<BOARD>::CaptureGenerator::CaptureGenerator(const BOARD& bd)
    : m_bd(bd)
{
    m_candidates.reserve(GO_MAX_NUM_MOVES);
}

template<class BOARD>
void GoUctPlayoutPolicy<BOARD>::CaptureGenerator::StartPlayout()
{
    m_candidates.clear();
    for (typename BOARD::Iterator it(m_bd); it; ++it)
    {
        const SgPoint p = *it;
        if (m_bd.Occupied(p) && m_bd.Anchor(p) == p && m_bd.InAtari(p))
            m_candidates.push_back(p);
    }
}

template<class BOARD>
void GoUctPlayoutPolicy<BOARD>::CaptureGenerator::OnPlay()
{
    SgPoint lastMove = m_bd.GetLastMove();
    if (lastMove == SG_NULLMOVE || lastMove == SG_PASS)
        return;
    if (m_bd.OccupiedInAtari(lastMove))
        m_candidates.push_back(m_bd.Anchor(lastMove));
    if (m_bd.NumNeighbors(lastMove, m_bd.ToPlay()) == 0)
        return;
    if (m_bd.OccupiedInAtari(lastMove + SG_NS))
        m_candidates.push_back(m_bd.Anchor(lastMove + SG_NS));
    if (m_bd.OccupiedInAtari(lastMove - SG_NS))
        m_candidates.push_back(m_bd.Anchor(lastMove - SG_NS));
    if (m_bd.OccupiedInAtari(lastMove + SG_WE))
        m_candidates.push_back(m_bd.Anchor(lastMove + SG_WE));
    if (m_bd.OccupiedInAtari(lastMove - SG_WE))
        m_candidates.push_back(m_bd.Anchor(lastMove - SG_WE));
}

template<class BOARD>
void GoUctPlayoutPolicy<BOARD>::CaptureGenerator::Generate(GoPointList& moves)
{
    SG_ASSERT(moves.IsEmpty());
    const SgBlackWhite opp = m_bd.Opponent();
    // For efficiency reasons, this function does not check, if the same
    // move is generated multiple times (and will therefore played with
    // higher probabilty, if there are also other capture moves), because in
    // nearly all cases, there is zero or one global capture move on the
    // board. Most captures are done immediately by the atari heuristic
    for (size_t i = 0; i < m_candidates.size(); ++i)
    {
        const SgPoint p = m_candidates[i];
        if (! m_bd.OccupiedInAtari(p))
        {
            m_candidates[i] = m_candidates[m_candidates.size() - 1];
            m_candidates.pop_back();
            --i;
            continue;
        }
        if (m_bd.GetColor(p) == opp)
            moves.PushBack(m_bd.TheLiberty(p));
    }
}

template<class BOARD>
GoUctPlayoutPolicy<BOARD>::GoUctPlayoutPolicy(const BOARD& bd,
                                        const GoUctPlayoutPolicyParam& param)
    : m_bd(bd),
      m_param(param),
      m_patterns(bd),
      m_checked(false),
      m_balancer(100), 
      m_captureGenerator(bd),
      m_pureRandomGenerator(bd, m_random)
{
	ClearStatistics();
}

template<class BOARD>
void GoUctPlayoutPolicy<BOARD>::ClearStatistics()
{
    m_statistics[SG_BLACK].Clear();
    m_statistics[SG_WHITE].Clear();
}

template<class BOARD>
bool GoUctPlayoutPolicy<BOARD>::CorrectMove(
                    Corrector corrFunction,
                    SgPoint& mv, GoUctPlayoutPolicyType moveType)
{
#ifdef _DEBUG
    const SgPoint oldMv = mv;
#endif
    if (! corrFunction(m_bd, mv))
        return false;

    m_moves.SetTo(mv);
    m_moveType = moveType;

#ifdef _DEBUG
    if (DEBUG_CORRECT_MOVE)
        SgDebug() << m_bd
                  << "Replace " << SgWriteMove(oldMv, m_bd.ToPlay())
                  << " by " << SgWriteMove(mv, m_bd.ToPlay()) << '\n';
#endif
    return true;
}

template<class BOARD>
void GoUctPlayoutPolicy<BOARD>::EndPlayout()
{
}

template<class BOARD>
bool GoUctPlayoutPolicy<BOARD>::GenerateAtariCaptureMove()
{
    SG_ASSERT(! SgIsSpecialMove(m_lastMove));
    if (m_bd.InAtari(m_lastMove))
    {
        SgMove mv = m_bd.TheLiberty(m_lastMove);
        m_moves.PushBack(mv);
        return true;
    }
    return false;
}

template<class BOARD>
bool GoUctPlayoutPolicy<BOARD>::GenerateAtariDefenseMove()
{
    SG_ASSERT(m_moves.IsEmpty());
    SG_ASSERT(! SgIsSpecialMove(m_lastMove));
    SgBlackWhite toPlay = m_bd.ToPlay();
    if (m_bd.NumNeighbors(m_lastMove, toPlay) == 0)
        return false;
    SgSList<SgPoint,4> anchorList;
    for (SgNb4Iterator it(m_lastMove); it; ++it)
    {
        if (m_bd.GetColor(*it) != toPlay || ! m_bd.InAtari(*it))
            continue;
        SgPoint anchor = m_bd.Anchor(*it);
        if (anchorList.Contains(anchor))
            continue;
        anchorList.PushBack(anchor);

        // Check if move on last liberty would escape the atari
        SgPoint theLiberty = m_bd.TheLiberty(anchor);
        if (! GoBoardUtil::SelfAtari(m_bd, theLiberty))
            m_moves.PushBack(theLiberty);

        // Capture adjacent blocks
        for (GoAdjBlockIterator<BOARD> it2(m_bd, anchor, 1); it2; ++it2)
        {
            SgPoint oppLiberty = m_bd.TheLiberty(*it2);
            // If opponent's last liberty is not my last liberty, we know
            // that we will have two liberties after capturing (my last
            // liberty + at least one stone captured). If both last liberties
            // are the same, we already checked above with
            // GoBoardUtil::SelfAtari(theLiberty), if the move escapes the
            // atari
            if (oppLiberty != theLiberty)
                m_moves.PushBack(oppLiberty);
        }
    }
    return ! m_moves.IsEmpty();
}

template<class BOARD>
void GoUctPlayoutPolicy<BOARD>::PlayGoodLiberties(SgPoint block)
{
    SgPoint ignoreOther;
    if (! GoBoardUtil::IsSimpleChain(m_bd, block, ignoreOther))
        for (typename BOARD::LibertyIterator it(m_bd, block); it; ++it)
            if (  GoUctUtil::GainsLiberties(m_bd, block, *it)
               && ! GoBoardUtil::SelfAtari(m_bd, *it)
               )
                m_moves.PushBack(*it);
}

template<class BOARD>
bool GoUctPlayoutPolicy<BOARD>::GenerateLowLibMove(SgPoint lastMove)
{
    SG_ASSERT(! SgIsSpecialMove(lastMove));
    SG_ASSERT(! m_bd.IsEmpty(lastMove));
    SG_ASSERT(m_moves.IsEmpty());
    
    const SgBlackWhite toPlay = m_bd.ToPlay();

    // take liberty of last move
    if (m_bd.NumLiberties(lastMove) == 2)
    {
        const SgPoint anchor = m_bd.Anchor(lastMove);
        PlayGoodLiberties(anchor);
    }

    if (m_bd.NumNeighbors(lastMove, toPlay) != 0)
    {
        // play liberties of neighbor blocks
        SgSList<SgPoint,4> ourLowLibBlocks;
        for (SgNb4Iterator it(lastMove); it; ++it)
        {
            if (m_bd.GetColor(*it) == toPlay
                && m_bd.NumLiberties(*it) == 2)
            {
                const SgPoint anchor = m_bd.Anchor(*it);
                if (! ourLowLibBlocks.Contains(anchor))
                {
                    ourLowLibBlocks.PushBack(anchor);
                    PlayGoodLiberties(anchor);
                }
            }
        }
    }

    return ! m_moves.IsEmpty();
}

template<class BOARD>
SG_ATTR_FLATTEN SgPoint GoUctPlayoutPolicy<BOARD>::GenerateMove()
{
    m_moves.Clear();
    m_checked = false;

    SgPoint mv = SG_NULLMOVE;

    if (m_param.m_fillboardTries > 0)
    {
        m_moveType = GOUCT_FILLBOARD;
        mv = m_pureRandomGenerator.GenerateFillboardMove(
                                                    m_param.m_fillboardTries);
    }

    m_lastMove = m_bd.GetLastMove();
    if (mv == SG_NULLMOVE
        && ! SgIsSpecialMove(m_lastMove) // skip if Pass or Null
        && ! m_bd.IsEmpty(m_lastMove) // skip if move was suicide
       )
    {
        if (m_param.m_useNakadeHeuristic && GenerateNakadeMove())
        {
            m_moveType = GOUCT_NAKADE;
            mv = SelectRandom();
        }
        if (mv == SG_NULLMOVE && GenerateAtariCaptureMove())
        {
            m_moveType = GOUCT_ATARI_CAPTURE;
            mv = SelectRandom();
        }
        if (mv == SG_NULLMOVE && GenerateAtariDefenseMove())
        {
            m_moveType = GOUCT_ATARI_DEFEND;
            mv = SelectRandom();
        }
        if (mv == SG_NULLMOVE && GenerateLowLibMove(m_lastMove))
        {
            m_moveType = GOUCT_LOWLIB;
            mv = SelectRandom();
        }
        if (mv == SG_NULLMOVE && GeneratePatternMove())
        {
            m_moveType = GOUCT_PATTERN;
            mv = SelectRandom();
        }
    }
    if (mv == SG_NULLMOVE)
    {
        m_moveType = GOUCT_CAPTURE;
        m_captureGenerator.Generate(m_moves);
        mv = SelectRandom();
    }
    if (mv == SG_NULLMOVE)
    {
        m_moveType = GOUCT_RANDOM;
        mv = m_pureRandomGenerator.Generate(m_balancer);
    }

    if (mv == SG_NULLMOVE)
    {
        m_moveType = GOUCT_PASS;
        mv = SG_PASS;
    }
    else
    {
        SG_ASSERT(m_bd.IsLegal(mv));
        m_checked = CorrectMove(GoUctUtil::DoSelfAtariCorrection, mv,
                                GOUCT_SELFATARI_CORRECTION);
        if (USE_CLUMP_CORRECTION && ! m_checked)
            CorrectMove(GoUctUtil::DoClumpCorrection, mv,
                        GOUCT_CLUMP_CORRECTION);
    }
    SG_ASSERT(m_bd.IsLegal(mv));
    SG_ASSERT(mv == SG_PASS || ! m_bd.IsSuicide(mv));

    if (m_param.m_statisticsEnabled)
        UpdateStatistics();

    return mv;
}

/** Nakade heuristic.
    If there is a region of three empty points adjacent to last move, play in
    the center of the region.
*/
template<class BOARD>
bool GoUctPlayoutPolicy<BOARD>::GenerateNakadeMove()
{
    SG_ASSERT(m_moves.IsEmpty());
    SG_ASSERT(! SgIsSpecialMove(m_lastMove));
    GenerateNakadeMove(m_lastMove + SG_NS);
    GenerateNakadeMove(m_lastMove - SG_NS);
    GenerateNakadeMove(m_lastMove + SG_WE);
    GenerateNakadeMove(m_lastMove - SG_WE);
    // Ignore duplicates in move list, happens rarely
    return ! m_moves.IsEmpty();
}

template<class BOARD>
void GoUctPlayoutPolicy<BOARD>::GenerateNakadeMove(SgPoint p)
{
    SgBlackWhite toPlay = m_bd.ToPlay();
    if (m_bd.IsEmpty(p) && m_bd.NumNeighbors(p, toPlay) == 0)
    {
        int numEmptyNeighbors = m_bd.NumEmptyNeighbors(p);
        if (numEmptyNeighbors == 2)
        {
            int n = 0;
            for (SgNb4Iterator it(p); it; ++it)
                if (m_bd.IsEmpty(*it))
                {
                    if (m_bd.NumEmptyNeighbors(*it) != 1
                        || m_bd.NumNeighbors(*it, toPlay) > 0)
                        return;
                    if (++n > 2)
                        break;
                }
            m_moves.PushBack(p);
        }
        else if (numEmptyNeighbors == 1)
        {
            for (SgNb4Iterator it(p); it; ++it)
                if (m_bd.IsEmpty(*it))
                {
                    if (m_bd.NumEmptyNeighbors(*it) != 2
                        || m_bd.NumNeighbors(*it, toPlay) > 0)
                        return;
                    for (SgNb4Iterator it2(*it); it2; ++it2)
                        if (m_bd.IsEmpty(*it2) && *it2 != p)
                        {
                            if (m_bd.NumEmptyNeighbors(*it2) == 1
                                && m_bd.NumNeighbors(*it2, toPlay) == 0)
                                m_moves.PushBack(*it);
                            break;
                        }
                    break;
                }

        }
    }
}

/** Pattern heuristic.
    Use patterns (only in 3x3 neighborhood of last move)
    @see GoUctPatterns
*/
template<class BOARD>
bool GoUctPlayoutPolicy<BOARD>::GeneratePatternMove()
{
    SG_ASSERT(m_moves.IsEmpty());
    SG_ASSERT(! SgIsSpecialMove(m_lastMove));
    GeneratePatternMove(m_lastMove + SG_NS - SG_WE);
    GeneratePatternMove(m_lastMove + SG_NS);
    GeneratePatternMove(m_lastMove + SG_NS + SG_WE);
    GeneratePatternMove(m_lastMove - SG_WE);
    GeneratePatternMove(m_lastMove + SG_WE);
    GeneratePatternMove(m_lastMove - SG_NS - SG_WE);
    GeneratePatternMove(m_lastMove - SG_NS);
    GeneratePatternMove(m_lastMove - SG_NS + SG_WE);
    if (SECOND_LAST_MOVE_PATTERNS)
    {
        const SgPoint lastMove2 = m_bd.Get2ndLastMove();
        if (! SgIsSpecialMove(lastMove2))
        {
            GeneratePatternMove2(lastMove2 + SG_NS - SG_WE, m_lastMove);
            GeneratePatternMove2(lastMove2 + SG_NS,         m_lastMove);
            GeneratePatternMove2(lastMove2 + SG_NS + SG_WE, m_lastMove);
            GeneratePatternMove2(lastMove2 - SG_WE,         m_lastMove);
            GeneratePatternMove2(lastMove2 + SG_WE,         m_lastMove);
            GeneratePatternMove2(lastMove2 - SG_NS - SG_WE, m_lastMove);
            GeneratePatternMove2(lastMove2 - SG_NS,         m_lastMove);
            GeneratePatternMove2(lastMove2 - SG_NS + SG_WE, m_lastMove);
        }
    }
    return ! m_moves.IsEmpty();
}

template<class BOARD>
inline void GoUctPlayoutPolicy<BOARD>::GeneratePatternMove(SgPoint p)
{
    if (m_bd.IsEmpty(p)
        && m_patterns.MatchAny(p)
        && ! GoBoardUtil::SelfAtari(m_bd, p))
        m_moves.PushBack(p);
}

template<class BOARD>
inline void GoUctPlayoutPolicy<BOARD>::GeneratePatternMove2(SgPoint p,
                                                            SgPoint lastMove)
{
    if (m_bd.IsEmpty(p)
        && ! SgPointUtil::In8Neighborhood(lastMove, p)
        && m_patterns.MatchAny(p)
        && ! GoBoardUtil::SelfAtari(m_bd, p))
        m_moves.PushBack(p);
}

template<class BOARD>
inline bool GoUctPlayoutPolicy<BOARD>::GeneratePoint(SgPoint p) const
{
    return GoUctUtil::GeneratePoint(m_bd, m_balancer, p, m_bd.ToPlay());
}

template<class BOARD>
GoPointList GoUctPlayoutPolicy<BOARD>::GetEquivalentBestMoves() const
{
    GoPointList result;
    if (m_moveType == GOUCT_RANDOM)
    {
        for (typename BOARD::Iterator it(m_bd); it; ++it)
            if (m_bd.IsEmpty(*it) && GeneratePoint(*it))
                result.PushBack(*it);
    }
    // Move in m_moves are not checked yet, if legal etc.
    for (GoPointList::Iterator it(m_moves); it; ++it)
        if (m_checked || GeneratePoint(*it))
            result.PushBack(*it);
    return result;
}

template<class BOARD>
GoUctPlayoutPolicyType GoUctPlayoutPolicy<BOARD>::MoveType()
    const
{
    return m_moveType;
}

template<class BOARD>
void GoUctPlayoutPolicy<BOARD>::OnPlay()
{
    m_captureGenerator.OnPlay();
    m_pureRandomGenerator.OnPlay();
}


template<class BOARD>
const GoUctPatterns<BOARD>& GoUctPlayoutPolicy<BOARD>::Patterns()
    const
{
    return m_patterns;
}

template<class BOARD>
inline SgPoint GoUctPlayoutPolicy<BOARD>::SelectRandom()
{
    return GoUctUtil::SelectRandom(m_bd, m_bd.ToPlay(), m_moves, m_random, 
                                   m_balancer);
}

/*
template<class BOARD>
const GoUctPlayoutPolicyStat&
GoUctPlayoutPolicy<BOARD>::Statistics() const
{
    return Statistics(m_bd.ToPlay());
}
*/

template<class BOARD>
const GoUctPlayoutPolicyStat&
GoUctPlayoutPolicy<BOARD>::Statistics(SgBlackWhite color) const
{
    return m_statistics[color];
}

template<class BOARD>
void GoUctPlayoutPolicy<BOARD>::StartPlayout()
{
    m_captureGenerator.StartPlayout();
    m_pureRandomGenerator.Start();
    m_nonRandLen = 0;
}

template<class BOARD>
void GoUctPlayoutPolicy<BOARD>::UpdateStatistics()
{
    GoUctPlayoutPolicyStat& statistics = m_statistics[m_bd.ToPlay()];
    ++statistics.m_nuMoves;
    ++statistics.m_nuMoveType[m_moveType];
    if (m_moveType == GOUCT_RANDOM)
    {
        if (m_nonRandLen > 0)
        {
            statistics.m_nonRandLen.Add(m_nonRandLen);
            m_nonRandLen = 0;
        }
    }
    else
    {
        ++m_nonRandLen;
        statistics.m_moveListLen.Add(GetEquivalentBestMoves().Length());
    }
}

//----------------------------------------------------------------------------

template<class BOARD>
class GoUctPlayoutPolicyFactory
{
public:
    /** Constructor.
        @param param Playout policy parameters. Stores a reference. Lifetime
        of the argument must exceed the lifetime of this factory and created
        objects.
    */
    GoUctPlayoutPolicyFactory(const GoUctPlayoutPolicyParam& param);

    GoUctPlayoutPolicy<BOARD>* Create(const BOARD& bd);

private:
    const GoUctPlayoutPolicyParam& m_param;
};

template<class BOARD>
GoUctPlayoutPolicyFactory<BOARD>
::GoUctPlayoutPolicyFactory(const GoUctPlayoutPolicyParam& param)
    : m_param(param)
{
}

template<class BOARD>
GoUctPlayoutPolicy<BOARD>*
GoUctPlayoutPolicyFactory<BOARD>::Create(const BOARD& bd)
{
    return new GoUctPlayoutPolicy<BOARD>(bd, m_param);
}

//----------------------------------------------------------------------------

#endif // GOUCT_PLAYOUTPOLICY_H
