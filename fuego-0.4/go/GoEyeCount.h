//----------------------------------------------------------------------------
/** @file GoEyeCount.h
    Upper and lower bounds on number of eyes and potential eyes.
    Also track whether an area can form a local seki.
    
    @todo the implementation of seki support is incomplete.
    @todo needs unit test cases.
*/
//----------------------------------------------------------------------------

#ifndef GO_EYECOUNT_H
#define GO_EYECOUNT_H

#include <ostream>

//----------------------------------------------------------------------------

/** Quick summary of eye status
    @todo there are more cases, see Landman's paper
    in Games of no Chance.
    Will add them later as needed.
*/
enum GoEyeStatus
{
    EYE_UNKNOWN,
    EYE_NONE,
    EYE_HALF,
    EYE_ONE,
    EYE_ONE_AND_HALF,
    EYE_TWO
};

//----------------------------------------------------------------------------

/** upper and lower bounds on number of eyes and potential eyes */
class GoEyeCount
{
public:
    GoEyeCount()
        : m_minEyes(0),
          m_maxEyes(0),
          m_minPotEyes(0),
          m_maxPotEyes(0),
          m_isLocalSeki(false),
          m_maybeLocalSeki(false)
    { }
    
    GoEyeCount(int minEyes, int maxEyes, int minPotEyes, int maxPotEyes) 
        : m_minEyes(minEyes), 
          m_maxEyes(maxEyes), 
          m_minPotEyes(minPotEyes),
          m_maxPotEyes(maxPotEyes),
          m_isLocalSeki(false),
          m_maybeLocalSeki(false)
    { }
    
    int MinEyes() const
    {
        return m_minEyes;
    }
    
    int MaxEyes() const 
    {
        return m_maxEyes;
    }
    
    int MinPotEyes() const 
    {
        return m_minPotEyes;
    }
    
    int MaxPotEyes() const 
    {
        return m_maxPotEyes;
    }
    
    bool IsLocalSeki() const 
    {
        return m_isLocalSeki;
    }
    
    bool MaybeLocalSeki() const
    {
        return m_maybeLocalSeki;
    }

    void Clear()
    {
        m_minEyes = m_maxEyes = m_minPotEyes = m_maxPotEyes = 0;
    }
    
    /** unknown eye count: min = 0, max = 2 */
    void SetUnknown()
    {
        m_minEyes = m_minPotEyes = 0;
        m_maxEyes = m_maxPotEyes = 2;
    }
    
    void SetMinEyes(int eyes)
    {
        m_minEyes = eyes;
    }
    
    void SetMaxEyes(int eyes)
    {
        m_maxEyes = eyes;
    }
    
    void SetExactEyes(int eyes)
    {
        m_minEyes = m_maxEyes = eyes;
    }
    
    void SetMinPotEyes(int eyes)
    {
        m_minPotEyes = eyes;
    }
    
    void SetMaxPotEyes(int eyes)
    {
        m_maxPotEyes = eyes;
    }
    
    void SetExactPotEyes(int eyes)
    {
        m_minPotEyes = m_maxPotEyes = eyes;
    }

    void SetEyes(int eyes, int potEyes)
    {
        SetExactEyes(eyes);
        SetExactPotEyes(potEyes);
    }
    
    /** locally, at least a seki for defender */
    void SetLocalSeki();
    
    /** Could be a seki for defender but not sure. */
    void SetMaybeLocalSeki()
    {
        m_maybeLocalSeki = true;
    }
    
    /** Make sure all eye counts are <=2 and consistent */
    void Normalize();
    
    /** Compute total eye count of two independent areas.
        This takes sente and gote into account - if there are
        two independent gote eyes, then it is one sure eye.
    */
    void AddIndependent(const GoEyeCount& from);
    
    /** In contrast to AddIndependent, just adds numbers */
    void NumericalAdd(const GoEyeCount& from);
    
    void AddPotential(const GoEyeCount& from);

private:
    int m_minEyes;
    int m_maxEyes;
    int m_minPotEyes;
    int m_maxPotEyes;
    bool m_isLocalSeki;
    bool m_maybeLocalSeki;
};

std::ostream& operator<<(std::ostream& stream, const GoEyeCount& s);

//----------------------------------------------------------------------------

#endif // GO_EYECOUNT_H
