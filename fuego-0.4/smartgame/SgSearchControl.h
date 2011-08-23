//----------------------------------------------------------------------------
/** @file SgSearchControl.h
    Search control for searchengine.

    Provides base class SgSearchControl and several 
    basic serch control strategies.
    An SgSearchControl object is installed into SgSearch or derived engine.
*/
//----------------------------------------------------------------------------

#ifndef SG_SEARCHCONTROL_H
#define SG_SEARCHCONTROL_H

//----------------------------------------------------------------------------

/** Resource control used in class SgSearch. */
class SgSearchControl
{
public:
    SgSearchControl();

    virtual ~SgSearchControl();

    /** Check if search should be aborted.
        Called at each node.
    */
    virtual bool Abort(double elapsedTime, int numNodes) = 0;

    /** Check if next iteration should be started.
        Called before each iteration.
        The default implementation always returns true.
        @param depth The depth of the next iteration.
        @param elapsedTime The elapsed time in seconds.
        @param numNodes The number of nodes visited.
    */
    virtual bool StartNextIteration(int depth, double elapsedTime,
                                    int numNodes);

private:
    /** Not implemented */
    SgSearchControl(const SgSearchControl&);

    /** Not implemented */
    SgSearchControl& operator=(const SgSearchControl&);
};

inline SgSearchControl::SgSearchControl()
{
}

//----------------------------------------------------------------------------

/** Example of a simple search abort class: abort when time has expired. */
class SgTimeSearchControl
    : public SgSearchControl
{
public:
    SgTimeSearchControl(double maxTime);

    virtual ~SgTimeSearchControl();

    virtual bool Abort(double elapsedTime, int ignoreNumNodes);

    double GetMaxTime() const;

    void SetMaxTime(double maxTime);

private:
    double m_maxTime;

    /** Not implemented */
    SgTimeSearchControl(const SgTimeSearchControl&);

    /** Not implemented */
    SgTimeSearchControl& operator=(const SgTimeSearchControl&);
};

inline double SgTimeSearchControl::GetMaxTime() const
{
    return m_maxTime;
}

inline void SgTimeSearchControl::SetMaxTime(double maxTime)
{
    m_maxTime = maxTime;
}

//----------------------------------------------------------------------------

/** Example of a simple search abort class: abort when node limit
    is reached.
*/
class SgNodeSearchControl
    : public SgSearchControl
{
public:
    SgNodeSearchControl(int maxNumNodes);

    virtual ~SgNodeSearchControl();

    virtual bool Abort(double ignoreElapsedTime, int numNodes);

    void SetMaxNumNodes(int maxNumNodes);

private:
    int m_maxNumNodes;

    /** Not implemented */
    SgNodeSearchControl(const SgNodeSearchControl&);

    /** Not implemented */
    SgNodeSearchControl& operator=(const SgNodeSearchControl&);
};

inline void SgNodeSearchControl::SetMaxNumNodes(int maxNumNodes)
{
    m_maxNumNodes = maxNumNodes;
}

//----------------------------------------------------------------------------

/** Abort when either time or node limit is reached. */
class SgCombinedSearchControl
    : public SgSearchControl
{
public:
    SgCombinedSearchControl(double maxTime, int maxNumNodes);

    virtual ~SgCombinedSearchControl();

    virtual bool Abort(double elapsedTime, int numNodes);

private:
    double m_maxTime;

    int m_maxNumNodes;

    /** Not implemented */
    SgCombinedSearchControl(const SgCombinedSearchControl&);

    /** Not implemented */
    SgCombinedSearchControl& operator=(const SgCombinedSearchControl&);
};

inline SgCombinedSearchControl::SgCombinedSearchControl(double maxTime,
                                                        int maxNumNodes)
    : m_maxTime(maxTime),
      m_maxNumNodes(maxNumNodes)
{
}

//----------------------------------------------------------------------------

/** Abort when time limit is reached AND a number of nodes were searched. */
class SgRelaxedSearchControl
    : public SgSearchControl
{
public:
    static const int MIN_NODES_PER_SECOND = 1000;

    SgRelaxedSearchControl(double maxTime);

    virtual ~SgRelaxedSearchControl();

    virtual bool Abort(double elapsedTime, int numNodes);

private:
    double m_maxTime;

    /** Not implemented */
    SgRelaxedSearchControl(const SgRelaxedSearchControl&);

    /** Not implemented */
    SgRelaxedSearchControl& operator=(const SgRelaxedSearchControl&);
};

inline SgRelaxedSearchControl::SgRelaxedSearchControl(double maxTime)
    : m_maxTime(maxTime)
{
}

//----------------------------------------------------------------------------

#endif // SG_SEARCHCONTROL_H
