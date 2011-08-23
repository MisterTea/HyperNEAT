//----------------------------------------------------------------------------
/** @file GoEyeCount.cpp
    See GoEyeCount.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoEyeCount.h"

#include <algorithm>
#include "SgWrite.h"

using std::min;
using std::max;

void GoEyeCount::SetLocalSeki()
{
    SetEyes(2, 2);
    m_isLocalSeki = true;
    m_maybeLocalSeki = true;
}

void GoEyeCount::Normalize()
{
    if (m_minEyes > m_maxEyes)
        m_maxEyes = m_minEyes;
    if (m_minEyes > m_minPotEyes)
        m_minPotEyes = m_minEyes;
    if (m_maxEyes > m_maxPotEyes)
        m_maxPotEyes = m_maxEyes;
    if (m_minPotEyes > m_maxPotEyes)
        m_maxPotEyes = m_minPotEyes;
    
    if (m_minEyes > 2)
        m_minEyes = 2;
    if (m_maxEyes > 2)
        m_maxEyes = 2;
    if (m_minPotEyes > 2)
        m_minPotEyes = 2;
    if (m_maxPotEyes > 2)
        m_maxPotEyes = 2;
}

void GoEyeCount::AddIndependent(const GoEyeCount& from)
{
    m_minEyes += from.m_minEyes;
    m_maxEyes += from.m_maxEyes;
    m_minEyes = max(m_minEyes, min(m_minPotEyes, from.m_minPotEyes));
    m_maxEyes = max(m_maxEyes, min(m_maxPotEyes, from.m_maxPotEyes));
    m_minPotEyes += from.m_minPotEyes;
    m_maxPotEyes += from.m_maxPotEyes;
}

void GoEyeCount::NumericalAdd(const GoEyeCount& from)
{
    m_minEyes += from.m_minEyes;
    m_maxEyes += from.m_maxEyes;
    m_minPotEyes += from.m_minPotEyes;
    m_maxPotEyes += from.m_maxPotEyes;
}

void GoEyeCount::AddPotential(const GoEyeCount& from)
{
    m_minPotEyes += from.MinPotEyes();
    m_maxPotEyes += from.MaxPotEyes();
}

std::ostream& operator<<(std::ostream& stream, const GoEyeCount& s)
{
    stream  << SgWriteLabel("Eyes")
            << '[' << s.MinEyes()
            << ".." << s.MaxEyes() << "]\n"
            << SgWriteLabel("PotEyes")
            << '[' << s.MinPotEyes()
            << ".." << s.MaxPotEyes() << "]\n"
            << SgWriteLabel("LocalSeki") << s.IsLocalSeki() << '\n'
            << SgWriteLabel("MaybeLocalSeki") << s.MaybeLocalSeki()
            << '\n';
    return stream;
}

//----------------------------------------------------------------------------

