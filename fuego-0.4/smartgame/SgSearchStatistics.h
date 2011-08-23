//----------------------------------------------------------------------------
/** @file SgSearchStatistics.h
    Search statistics
*/
//----------------------------------------------------------------------------

#ifndef SG_SEARCHSTATISTICS_H
#define SG_SEARCHSTATISTICS_H

#include <iosfwd>

//----------------------------------------------------------------------------

/** Statistics used in class SgSearch */
class SgSearchStatistics
{
public:
    SgSearchStatistics();

    SgSearchStatistics(const SgSearchStatistics& stat);

    virtual ~SgSearchStatistics();

    SgSearchStatistics& operator=(const SgSearchStatistics& rhs);

    SgSearchStatistics& operator+=(const SgSearchStatistics& rhs);

    /** Set the number of nodes and leafs searched to zero. */
    void Clear();

    int DepthReached() const;

    virtual SgSearchStatistics* Duplicate() const;

    void IncNumEvals();

    void IncNumMoves();

    void IncNumNodes();

    void IncNumPassMoves();

    int NumEvals() const;

    int NumMoves() const;

    int NumNodes() const;

    double NumEvalsPerSecond() const;

    double NumNodesPerSecond() const;

    int NumPassMoves() const;

    void SetDepthReached(int depthReached);

    /** Set the time used to the given value.
        Only needed because doesn't keep track of real time used, and some
        searches might want to report the real time rather than the thread
        time.
    */
    void SetTimeUsed(double timeUsed);

    double TimeUsed() const;

private:
    int m_numNodes;
    
    int m_numEvals;
    
    int m_numMoves;
    
    int m_numPass;

    int m_depthReached;

    double m_timeUsed;
};

std::ostream& operator<<(std::ostream& stream,
                         const SgSearchStatistics &w);

inline int SgSearchStatistics::DepthReached() const
{
    return m_depthReached;
}

inline void SgSearchStatistics::IncNumEvals()
{
    ++m_numEvals;
}

inline void SgSearchStatistics::IncNumMoves()
{
    ++m_numMoves;
}

inline void SgSearchStatistics::IncNumNodes()
{
    ++m_numNodes;
}

inline void SgSearchStatistics::IncNumPassMoves()
{
    ++m_numPass;
}

inline int SgSearchStatistics::NumEvals() const
{
    return m_numEvals;
}

inline int SgSearchStatistics::NumMoves() const
{
    return m_numMoves;
}

inline int SgSearchStatistics::NumNodes() const
{
    return m_numNodes;
}

inline int SgSearchStatistics::NumPassMoves() const
{
    return m_numPass;
}

inline void SgSearchStatistics::SetDepthReached(int depthReached)
{
    m_depthReached = depthReached;
}

inline void SgSearchStatistics::SetTimeUsed(double timeUsed)
{
    m_timeUsed = timeUsed;
}

inline double SgSearchStatistics::TimeUsed() const
{
    return m_timeUsed;
}

//----------------------------------------------------------------------------

#endif // SG_SEARCHSTATISTICS_H
