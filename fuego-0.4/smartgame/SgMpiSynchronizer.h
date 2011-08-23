//----------------------------------------------------------------------------
/** @file SgMpiSynchronizer.h
*/
//----------------------------------------------------------------------------

#ifndef SG_MPISYNCHRONIZER_H
#define SG_MPISYNCHRONIZER_H

#include <iostream>
#include <string>
#include <boost/shared_ptr.hpp>
#include "SgMove.h"
#include "SgSystem.h"

//----------------------------------------------------------------------------

class SgUctSearch;

class SgUctThreadState;

class SgUctGameInfo;

/** Interface for mpi synchronizers. */
class SgMpiSynchronizer 
{
public:
    virtual ~SgMpiSynchronizer();

    virtual std::string ToNodeFilename(const std::string &filename) const = 0;

    virtual bool IsRootProcess() const = 0;

    virtual void OnStartSearch(SgUctSearch &search) = 0;
    
    virtual void OnEndSearch(SgUctSearch &search) = 0;

    virtual void OnThreadStartSearch(SgUctSearch &search, 
                                     SgUctThreadState &state) = 0;

    virtual void OnThreadEndSearch(SgUctSearch &search, 
                                   SgUctThreadState &state) = 0;

    virtual void OnSearchIteration(SgUctSearch &search, 
                                   std::size_t gameNumber,
                                   int threadId, 
                                   const SgUctGameInfo& info) = 0;

    virtual void OnStartPonder() = 0;

    virtual void OnEndPonder() = 0;

    virtual void WriteStatistics(std::ostream& out) const = 0;

    virtual void SynchronizeUserAbort(bool &flag) = 0;

    virtual void SynchronizePassWins(bool &flag) = 0;

    virtual void SynchronizeEarlyPassPossible(bool &flag) = 0;

    virtual void SynchronizeMove(SgMove &move) = 0;

    virtual void SynchronizeValue(float &value) = 0;

    virtual void SynchronizeSearchStatus(float &value, bool &earlyAbort, 
                                         std::size_t &rootMoveCount) = 0;
};

typedef boost::shared_ptr<SgMpiSynchronizer> SgMpiSynchronizerHandle;

//----------------------------------------------------------------------------

/** Synchronizer with empty implementation. */
class SgMpiNullSynchronizer : public SgMpiSynchronizer 
{
public:
    SgMpiNullSynchronizer();

    virtual ~SgMpiNullSynchronizer();

    static SgMpiSynchronizerHandle Create();

    virtual std::string ToNodeFilename(const std::string &filename) const;

    virtual bool IsRootProcess() const;

    virtual void OnStartSearch(SgUctSearch &search);
    
    virtual void OnEndSearch(SgUctSearch &search);

    virtual void OnThreadStartSearch(SgUctSearch &search, 
                                     SgUctThreadState &state);

    virtual void OnThreadEndSearch(SgUctSearch &search, 
                                   SgUctThreadState &state);

    virtual void OnSearchIteration(SgUctSearch &search, std::size_t gameNumber,
                                   int threadId, const SgUctGameInfo& info);

    virtual void OnStartPonder();

    virtual void OnEndPonder();

    virtual void WriteStatistics(std::ostream& out) const;
    
    virtual void SynchronizeUserAbort(bool &flag);

    virtual void SynchronizePassWins(bool &flag);

    virtual void SynchronizeEarlyPassPossible(bool &flag);

    virtual void SynchronizeMove(SgMove &move);

    virtual void SynchronizeValue(float &value);

    virtual void SynchronizeSearchStatus(float &value, bool &earlyAbort, 
                                         std::size_t &rootMoveCount);

};

// Ensure forward definitions
// are eventually declared
#include "SgUctSearch.h"

//----------------------------------------------------------------------------

#endif // SG_MPISYNCHRONIZER_H
