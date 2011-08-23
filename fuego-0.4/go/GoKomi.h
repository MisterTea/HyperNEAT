//----------------------------------------------------------------------------
/** @file GoKomi.h */
//----------------------------------------------------------------------------

#ifndef GO_KOMI_H
#define GO_KOMI_H

#include <string>
#include "SgException.h"

//----------------------------------------------------------------------------

/** Class wrapping a komi value.
    Supported komi values are "unknown" or multiples of 0.5.
*/
class GoKomi
{
public:
    /** Exception thrown if komi is constructed with invalid value. */
    class InvalidKomi
        : public SgException
    {
    public:
        /** Constructor.
            @param komi The invalid komi value.
        */
        InvalidKomi(float komi);

        /** Constructor.
            @param komi The invalid komi value.
        */
        InvalidKomi(const std::string& komi);
    };

    /** Construct komi with unknown value. */
    GoKomi();

    /** Construct komi from float.
        @param komi The komi, will be rounded to a multiple of 0.5.
    */
    GoKomi(float komi);

    /** Construct komi from string.
        @param komi The string, leading and trailing whitespaces will be
        ignored, empty for unknown komi, float otherwise. The float will be
        rounded to a multiple of 0.5.
        @throws InvalidKomi If string is not empty or contains a float.
    */
    GoKomi(const std::string& komi);

    GoKomi& operator=(const GoKomi& komi);

    bool operator==(const GoKomi& komi) const;

    bool operator!=(const GoKomi& komi) const;

    /** Check if komi is unknown.
        @return true, if komi has unknown value.
    */
    bool IsUnknown() const;

    /** Convert komi to float.
        @return The komi value as a float, zero if komi is unknown.
    */
    float ToFloat() const;

    /** Convert komi to string.
        @return The komi value as a string, empty string if komi is unknown.
    */
    std::string ToString() const;

private:
    bool m_isUnknown;

    /** Komi value stored as integer.
        Corresponds to the real komi multiplied by two.
    */
    int m_value;
};

inline std::ostream& operator<<(std::ostream& out, const GoKomi& komi)
{
    out << komi.ToString();
    return out;
}

inline GoKomi::GoKomi()
    : m_isUnknown(true),
      m_value(0)
{
}

inline GoKomi::GoKomi(float komi)
    : m_isUnknown(false),
      m_value(static_cast<int>(komi > 0 ?
                               komi * 2.f + 0.25f : komi * 2.f - 0.25f))
{
}

inline GoKomi& GoKomi::operator=(const GoKomi& komi)
{
    m_isUnknown = komi.m_isUnknown;
    m_value = komi.m_value;
    return *this;
}

inline bool GoKomi::operator==(const GoKomi& komi) const
{
    return (m_isUnknown == komi.m_isUnknown && m_value == komi.m_value);
}

inline bool GoKomi::operator!=(const GoKomi& komi) const
{
    return ! (*this == komi);
}

inline bool GoKomi::IsUnknown() const
{
    return m_isUnknown;
}

inline float GoKomi::ToFloat() const
{
    if (m_isUnknown)
        return 0.f;
    else
        return 0.5f * m_value;
}

//----------------------------------------------------------------------------

#endif // GO_KOMI_H

