//----------------------------------------------------------------------------
/** @file SgRestorer.h
    Change and restore stack-based variables.
*/
//----------------------------------------------------------------------------

#ifndef SG_RESTORER_H
#define SG_RESTORER_H

//----------------------------------------------------------------------------

/** A SgRestorer<T> variable saves the current state of a variable of type
    T and restores it to that saved value upon leaving the scope in an
    exception-safe way.
*/
template <class T>
class SgRestorer
{
public:
    explicit SgRestorer(T* oldState)
        : m_variable(oldState),
          m_oldState(*oldState)
    { }

    ~SgRestorer()
    {
        *m_variable = m_oldState;
    }

private:
    T* m_variable;

    T m_oldState;

    /** Not implemented */
    SgRestorer(const SgRestorer&);

    /** Not implemented */
    SgRestorer& operator=(const SgRestorer&);
};

//----------------------------------------------------------------------------

/** Saves the current state of a variable of type T and asserts that the
    saved value equals the value upon leaving the scope.
*/
template <class T>
class SgAssertRestored
{
public:
    explicit SgAssertRestored(T* oldState)
        : m_variable(oldState),
          m_oldState(*oldState)
    { }

    ~SgAssertRestored()
    {
        SG_ASSERT(*m_variable == m_oldState);
    }

private:
    T* m_variable;

    T m_oldState;

    /** Not implemented. */
    SgAssertRestored(const SgAssertRestored&);

    /** Not implemented. */
    SgAssertRestored& operator=(const SgAssertRestored&);
};

//----------------------------------------------------------------------------

#endif // SG_RESTORER_H
