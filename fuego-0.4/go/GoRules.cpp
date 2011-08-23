//----------------------------------------------------------------------------
/** @file GoRules.cpp
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoRules.h"

#include <cmath>
#include <limits>
#include <iostream>
#include "SgException.h"

using namespace std;

//----------------------------------------------------------------------------

GoRules::GoRules(int handicap, const GoKomi& komi, bool japanese,
                 bool twoPassesEndGame)
    : m_allowSuicide(false),
      m_captureDead(false),
      m_japaneseScoring(japanese),
      m_extraHandicapKomi(false),
      m_handicap(handicap),
      m_komi(komi),
      m_japaneseHandicap(japanese),
      m_twoPassesEndGame(twoPassesEndGame),
      m_koRule(SUPERKO)
{
}

bool GoRules::operator==(const GoRules& rules) const
{
    return m_allowSuicide == rules.m_allowSuicide
        && m_captureDead == rules.m_captureDead
        && m_handicap == rules.m_handicap
        && m_komi == rules.m_komi
        && m_japaneseHandicap == rules.m_japaneseHandicap
        && m_japaneseScoring == rules.m_japaneseScoring
        && m_extraHandicapKomi == rules.m_extraHandicapKomi
        && m_twoPassesEndGame == rules.m_twoPassesEndGame
        && m_koRule == rules.m_koRule;
}

void GoRules::SetNamedRules(const std::string& namedRules)
{
    if (namedRules == "cgos")
    {
        SetAllowSuicide(false);
        SetJapaneseHandicap(false);
        SetJapaneseScoring(false);
        SetKoRule(POS_SUPERKO);
        SetCaptureDead(true);
        SetExtraHandicapKomi(false);
    }
    else if (namedRules == "chinese")
    {
        SetAllowSuicide(false);
        SetJapaneseHandicap(false);
        SetJapaneseScoring(false);
        SetKoRule(POS_SUPERKO);
        SetCaptureDead(false);
        SetExtraHandicapKomi(false);
    }
    else if (namedRules == "japanese")
    {
        SetAllowSuicide(false);
        SetJapaneseHandicap(true);
        SetJapaneseScoring(true);
        SetKoRule(SIMPLEKO);
        SetCaptureDead(false);
        SetExtraHandicapKomi(false);
    }
    else if (namedRules == "kgs")
    {
        SetAllowSuicide(false);
        SetJapaneseHandicap(false);
        SetJapaneseScoring(false);
        SetKoRule(POS_SUPERKO);
        SetCaptureDead(false);
        SetExtraHandicapKomi(true);
    }
    else
        throw SgException("Unknown rules: " + namedRules);
}

//----------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& out, GoRules::KoRule koRule)
{
    switch (koRule)
    {
    case GoRules::SIMPLEKO:
        out << "simple";
        break;
    case GoRules::SUPERKO:
        out << "superko";
        break;
    case GoRules::POS_SUPERKO:
        out << "positional_superko";
        break;
    default:
        SG_ASSERT(false);
    }
    return out;
}

//----------------------------------------------------------------------------

