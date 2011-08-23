//----------------------------------------------------------------------------
/** @file GoMotive.h
    Enumerations GoMotive and GoModifier.
 */
//----------------------------------------------------------------------------

#ifndef GO_MOTIVE_H
#define GO_MOTIVE_H

#include <bitset>
#include <iosfwd>

//----------------------------------------------------------------------------

/** Move motives.
    These motives were originally used in the Explorer Go program.
*/
enum GoMotive
{
    /** Dummy motive used as placeholder */
    GO_MOT_NONE,

    /** Random move. currently unused. */
    GO_MOT_RANDOM,

    /** Capture block tactically. */
    GO_MOT_CAPTURE,

    /** Escape with block tactically. */
    GO_MOT_ESCAPE,

    /** Connect blocks or chains */
    GO_MOT_CONNECT,

    /** Cut chains. */
    GO_MOT_CUT,

    /** Move to divide area */
    GO_MOT_TO_DIVIDE,

    /** Urgent pattern move */
    GO_MOT_URGENT,

    /** Expand group or zone */
    GO_MOT_EXPAND,

    /** Stabilize block */
    GO_MOT_STABILIZE,

    /** Reduce zone (or group?) */
    GO_MOT_REDUCE,

    /** Defend zone */
    GO_MOT_DEFEND,

    /** Invade zone */
    GO_MOT_INVADE,

    /** Sente endgame move */
    GO_MOT_SENTE,

    /** Forced reply or forced pattern move */
    GO_MOT_FORCED,

    /** Move to make one eye */
    GO_MOT_ONEYE,

    /** Move to make two eyes */
    GO_MOT_TWOEYE,

    /** Endgame move */
    GO_MOT_YOSE,

    /** Attack group */
    GO_MOT_ATTACK,

    /** Defend group */
    GO_MOT_SAVE,

    /** Double atari */
    GO_MOT_DOUBLEATARI,

    /** Dtari or other tactical threat */
    GO_MOT_ATARI,

    /** Ko threat */
    GO_MOT_KOTHREAT,

    /** Total number of motives */
    _GO_NU_MOTIVE
};

typedef std::bitset<_GO_NU_MOTIVE> GoMotives;

std::ostream& operator<<(std::ostream& out, GoMotive motive);

//----------------------------------------------------------------------------

/** Move motive modifier. */
enum GoModifier
{
    GO_MOD_NORMAL,

    GO_MOD_VERY_BIG,

    GO_MOD_BIG,

    GO_MOD_SMALL,

    GO_MOD_VERY_SMALL,

    _GO_NU_MODIFIER
};

std::ostream& operator<<(std::ostream& out, GoModifier modifier);

//----------------------------------------------------------------------------

#endif // GO_MOTIVE_H
