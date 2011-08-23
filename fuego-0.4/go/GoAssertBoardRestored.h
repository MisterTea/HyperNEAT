//----------------------------------------------------------------------------
/** @file GoAssertBoardRestored.h */
//----------------------------------------------------------------------------

#ifndef GO_ASSERTBOARDRESTORED_H
#define GO_ASSERTBOARDRESTORED_H

#include "GoBoard.h"

//----------------------------------------------------------------------------

/** Assert that the board has been restored to previous state. */
class GoAssertBoardRestored
{
public:
    /** Constructor for later explicit call of Init() */
    GoAssertBoardRestored();

    /** Constructor, calls Init(). */
    GoAssertBoardRestored(const GoBoard& bd);

    /** Destructor, calls CheckRestored(). */
    ~GoAssertBoardRestored();

    /** Checks with assertions that the state of the board is the same
        as it was at the last call to Init() or the constructor.
    */
    void AssertRestored();

    void Init(const GoBoard& bd);

    /** Set to a state, in which the destructor does not call
        AssertRestored() anymore.
    */
    void Clear();

private:
#ifndef NDEBUG
    const GoBoard* m_bd;

    int m_size;

    SgBlackWhite m_toPlay;

    /** Hash code for this board position. */
    SgHashCode m_hash;

    /** Current move number. */
    int m_moveNumber;

    SgBWArray<int> m_numStones;

    GoRules m_rules;

    bool m_allowKoRepetition;

    bool m_allowAnyRepetition;

    bool m_koModifiesHash;

    SgEmptyBlackWhite m_koColor;

    int m_koLevel;

    SgEmptyBlackWhite m_koLoser;
#endif // NDEBUG

    /** Not implemented. */
    GoAssertBoardRestored(const GoAssertBoardRestored&);

    /** Not implemented. */
    GoAssertBoardRestored& operator=(const GoAssertBoardRestored&);
};

inline GoAssertBoardRestored::GoAssertBoardRestored()
{
#ifndef NDEBUG
    m_bd = 0;
#endif
}

inline GoAssertBoardRestored::GoAssertBoardRestored(const GoBoard& bd)
{
    SG_DEBUG_ONLY(bd);
#ifndef NDEBUG
    Init(bd);
#endif
}

inline GoAssertBoardRestored::~GoAssertBoardRestored()
{
#ifndef NDEBUG
    AssertRestored();
#endif
}

inline void GoAssertBoardRestored::AssertRestored()
{
#ifndef NDEBUG
    if (m_bd == 0)
        return;
    SG_ASSERT(m_bd->Size() == m_size);
    SG_ASSERT(m_bd->ToPlay() == m_toPlay);
    SG_ASSERT(m_bd->GetHashCode() == m_hash);
    SG_ASSERT(m_bd->MoveNumber() == m_moveNumber);
    SG_ASSERT(m_bd->TotalNumStones(SG_BLACK) == m_numStones[SG_BLACK]);
    SG_ASSERT(m_bd->TotalNumStones(SG_WHITE) == m_numStones[SG_WHITE]);
    SG_ASSERT(m_bd->Rules() == m_rules);
    SG_ASSERT(m_bd->KoRepetitionAllowed() == m_allowKoRepetition);
    SG_ASSERT(m_bd->AnyRepetitionAllowed() == m_allowAnyRepetition);
    SG_ASSERT(m_bd->KoModifiesHash() == m_koModifiesHash);
    SG_ASSERT(m_bd->KoColor() == m_koColor);
    SG_ASSERT(m_bd->KoLevel() == m_koLevel);
    SG_ASSERT(m_bd->KoLoser() == m_koLoser);
#endif
}

inline void GoAssertBoardRestored::Clear()
{
#ifndef NDEBUG
    m_bd = 0;
#endif
}

inline void GoAssertBoardRestored::Init(const GoBoard& bd)
{
    SG_DEBUG_ONLY(bd);
#ifndef NDEBUG
    m_bd = &bd;
    m_size = bd.Size();
    m_toPlay = bd.ToPlay();
    m_hash = bd.GetHashCode();
    m_moveNumber = bd.MoveNumber();
    m_numStones = bd.TotalNumStones();
    m_rules = bd.Rules();
    m_allowKoRepetition = bd.KoRepetitionAllowed();
    m_allowAnyRepetition = bd.AnyRepetitionAllowed();
    m_koModifiesHash = bd.KoModifiesHash();
    m_koColor = bd.KoColor();
    m_koLevel = bd.KoLevel();
    m_koLoser = bd.KoLoser();
#endif
}

//----------------------------------------------------------------------------

#endif // GO_ASSERTBOARDRESTORED_H
