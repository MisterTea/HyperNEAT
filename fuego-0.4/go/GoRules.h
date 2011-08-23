//----------------------------------------------------------------------------
/** @file GoRules.h
    Class GoRules.
*/
//----------------------------------------------------------------------------

#ifndef GO_RULES_H
#define GO_RULES_H

#include <iosfwd>
#include <string>
#include "GoKomi.h"

//----------------------------------------------------------------------------

/** Parameters describing game rules and handicap. */
class GoRules
{
public:
    /** Ko rule. */
    enum KoRule
    {
        /** Positional superko.
            Full board repetition is forbidden, independent on who is to play.
        */
        POS_SUPERKO,

        /** Only repetition of the position two moves ago is forbidden. */
        SIMPLEKO,

        /** Situational superko.
            Full board repetition is forbidden, including who is to play.
        */
        SUPERKO
    };

    GoRules(int handicap = 0, const GoKomi& komi = GoKomi(6.5),
            bool japanese = false, bool twoPassesEndGame = true);

    bool operator==(const GoRules& rules) const;

    bool operator!=(const GoRules& rules) const;

    /** @name Set and query rule details */
    // @{

    /** Default is false. */
    bool AllowSuicide() const;

    void SetAllowSuicide(bool allowSuicide);

    /** Whether it necessary to capture dead stones.
        With some rules all un-captured stones count as alive.
        Default is false.
    */
    bool CaptureDead() const;

    /** See CaptureDead() */
    void SetCaptureDead(bool captureDead);

    KoRule GetKoRule() const;

    void SetKoRule(KoRule koRule);

    int Handicap() const;

    void SetHandicap(int handicap);

    /** True if using Japanese style handicap. */
    bool JapaneseHandicap() const;

    void SetJapaneseHandicap(bool japaneseHandicap);

    /** True if using Japanese style scoring.
        Japanese style scoring counts territory and prisoners, but not
        own stones.
    */
    bool JapaneseScoring() const;

    void SetJapaneseScoring(bool japaneseScoring);

    const GoKomi& Komi() const;

    void SetKomi(const GoKomi& komi);

    /** True if two passes end the game, false if 3 passes needed. */
    bool TwoPassesEndGame() const;

    void SetTwoPassesEndGame(bool twoPassesEndGame);

    /** Each handicap stone counts as an extra komi point for white.
        This extra komi point is not included in the komi settings. Used by
        the KGS Go server. Default is false.
    */
    bool ExtraHandicapKomi() const;

    /** See ExtraHandicapKomi() */
    void SetExtraHandicapKomi(bool enable);

    // @} // name


    /** Set several rule settings according to rule name.
        Currently supported:
        <table>
        <tr>
        <th>Name</th>
        <th>Suicide</th>
        <th>JapaneseHandicap</th>
        <th>JapaneseScoring</th>
        <th>KoRule</th>
        <th>CaptureDead</th>
        <th>ExtraHandicapKomi</th>
        </tr>
        <tr>
        <td>cgos</td>
        <td>no</td>
        <td>no</td>
        <td>no</td>
        <td>positional superko</td>
        <td>yes</td>
        <td>no</td>
        </tr>
        <tr>
        <td>chinese</td>
        <td>no</td>
        <td>no</td>
        <td>no</td>
        <td>positional superko</td>
        <td>no</td>
        <td>no</td>
        </tr>
        <tr>
        <td>japanese</td>
        <td>no</td>
        <td>yes</td>
        <td>yes</td>
        <td>simple</td>
        <td>no</td>
        <td>no</td>
        </tr>
        <tr>
        <td>kgs</td>
        <td>no</td>
        <td>no</td>
        <td>no</td>
        <td>positional superko</td>
        <td>no</td>
        <td>yes</td>
        </tr>
        </table>
        @param namedRules The named rules.
        @exception SgException If rule name is not known.
     */
    void SetNamedRules(const std::string& namedRules);

private:
    bool m_allowSuicide;

    bool m_captureDead;

    bool m_japaneseScoring;

    /** See ExtraHandicapKomi() */
    bool m_extraHandicapKomi;

    /** Initial handicap for this game. */
    int m_handicap;

    /** The komi. */
    GoKomi m_komi;

    bool m_japaneseHandicap;

    bool m_twoPassesEndGame;

    KoRule m_koRule;
};

inline bool GoRules::operator!=(const GoRules& rules) const
{
    return ! (*this == rules);
}

inline bool GoRules::AllowSuicide() const
{
    return m_allowSuicide;
}

inline bool GoRules::CaptureDead() const
{
    return m_captureDead;
}

inline bool GoRules::ExtraHandicapKomi() const
{
    return m_extraHandicapKomi;
}

inline GoRules::KoRule GoRules::GetKoRule() const
{
    return m_koRule;
}

inline int GoRules::Handicap() const
{
    return m_handicap;
}

inline bool GoRules::JapaneseHandicap() const
{
    return m_japaneseHandicap;
}

inline bool GoRules::JapaneseScoring() const
{
    return m_japaneseScoring;
}

inline const GoKomi& GoRules::Komi() const
{
    return m_komi;
}

inline void GoRules::SetAllowSuicide(bool allowSuicide)
{
    m_allowSuicide = allowSuicide;
}

inline void GoRules::SetCaptureDead(bool captureDead)
{
    m_captureDead = captureDead;
}

inline void GoRules::SetExtraHandicapKomi(bool enable)
{
    m_extraHandicapKomi = enable;
}

inline void GoRules::SetHandicap(int handicap)
{
    SG_ASSERT(handicap >= 0);
    m_handicap = handicap;
}

inline void GoRules::SetJapaneseHandicap(bool japaneseHandicap)
{
    m_japaneseHandicap = japaneseHandicap;
}

inline void GoRules::SetJapaneseScoring(bool japaneseScoring)
{
    m_japaneseScoring = japaneseScoring;
}

inline void GoRules::SetKomi(const GoKomi& komi)
{
    m_komi = komi;
}

inline void GoRules::SetKoRule(KoRule koRule)
{
    m_koRule = koRule;
}

inline void GoRules::SetTwoPassesEndGame(bool twoPassesEndGame)
{
    m_twoPassesEndGame = twoPassesEndGame;
}

inline bool GoRules::TwoPassesEndGame() const
{
    return m_twoPassesEndGame;
}

std::ostream& operator<<(std::ostream& out, GoRules::KoRule koRule);

//----------------------------------------------------------------------------

#endif // GO_RULES_H

