//----------------------------------------------------------------------------
/** @file SgFastLog.h
    Fast logarithm.
*/
//----------------------------------------------------------------------------

#ifndef SG_FASTLOG_H
#define SG_FASTLOG_H

//----------------------------------------------------------------------------

/** Fast logarithm.
    Computes a fast single precision logarithm based on a lookup table.

    O. Vinyals, G. Friedland, N. Mirghafori: Revisiting a basic function on
    current CPUs: A fast logarithm implementation with adjustable accuracy.
    http://www.icsi.berkeley.edu/pubs/techreports/TR-07-002.pdf

    The implementation assumes that the platform uses IEEE 754 floats.
*/
class SgFastLog
{
public:
    SgFastLog(int mantissaBits);

    ~SgFastLog();

    /** Get natural logarithm. */
    float Log(float val) const;

private:
    union IntFloat
    {
        int m_int;

        float m_float;
    };

    static const int MAX_MANTISSA_BITS = 23;

    const int m_mantissaBitsDiff;

    float* m_lookupTable;

    /** Not implemented. */
    SgFastLog(const SgFastLog&);

    /** Not implemented. */
    SgFastLog& operator=(const SgFastLog&);
};

inline float SgFastLog::Log(float val) const
{
    IntFloat x;
    x.m_float = val;
    int logTwo = ((x.m_int >> MAX_MANTISSA_BITS) & 255) - 127;
    x.m_int &= 0x7FFFFF;
    x.m_int >>= m_mantissaBitsDiff;
    return ((m_lookupTable[x.m_int] + logTwo) * 0.69314718f);
}

//----------------------------------------------------------------------------

#endif // SG_FASTLOG_H
