//----------------------------------------------------------------------------
/** @file GoAutoBook.h
 */
//----------------------------------------------------------------------------

#ifndef GOAUTOBOOK_H
#define GOAUTOBOOK_H

#include <cmath>
#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include "SgBookBuilder.h"
#include "SgThreadedWorker.h"
#include "GoBoard.h"
#include "GoBoardSynchronizer.h"

//----------------------------------------------------------------------------

/** Tracks canonical hash. */
class GoAutoBookState
{
public:
    GoAutoBookState(const GoBoard& brd);

    ~GoAutoBookState();

    GoBoard& Board();

    const GoBoard& Board() const;

    void Play(SgMove move);

    void Undo();

    SgHashCode GetHashCode() const;

    void Synchronize();

private:
    GoBoardSynchronizer m_synchronizer;

    GoBoard m_brd[8]; 

    SgHashCode m_hash;

    void ComputeHashCode();
}; 

inline GoBoard& GoAutoBookState::Board()
{
    return m_brd[0];
}

inline const GoBoard& GoAutoBookState::Board() const
{
    return m_brd[0];
}

//----------------------------------------------------------------------------

typedef enum 
{
    /** Select move with highest count. */
    GO_AUTOBOOK_SELECT_COUNT,

    /** Select move with highest value. */
    GO_AUTOBOOK_SELECT_VALUE

} GoAutoBookMoveSelectType;

struct GoAutoBookParam
{
    /** Required 'count' before it can be used by the player when
        generating moves. */
    std::size_t m_usageCountThreshold;

    GoAutoBookMoveSelectType m_selectType;

    GoAutoBookParam();        
};

//----------------------------------------------------------------------------

class GoAutoBook
{
public:
    GoAutoBook(const std::string& filename,
               const GoAutoBookParam& param);

    ~GoAutoBook();

    bool Get(const GoAutoBookState& state, SgBookNode& node) const;

    void Put(const GoAutoBookState& state, const SgBookNode& node);

    void Flush();

    void Save(const std::string& filename) const;

    SgMove LookupMove(const GoBoard& brd) const;

    SgMove FindBestChild(GoAutoBookState& state) const;

    void Merge(const GoAutoBook& other);

    void AddDisabledLines(const std::set<SgHashCode>& disabled);

    /** Copies a truncated version of the book into other. */
    void TruncateByDepth(int depth, GoAutoBookState& state,
                         GoAutoBook& other) const;

    /** Parses a worklist from a stream. */
    static std::vector< std::vector<SgMove> > ParseWorkList(std::istream& in);

private:
    typedef std::map<SgHashCode, SgBookNode> Map;

    Map m_data;

    const GoAutoBookParam& m_param;

    std::set<SgHashCode> m_disabled;

    std::string m_filename;

    void TruncateByDepth(int depth, GoAutoBookState& state, 
                         GoAutoBook& other, 
                         std::set<SgHashCode>& seen) const;
};

inline void GoAutoBook::AddDisabledLines(const std::set<SgHashCode>& disabled)
{
    m_disabled.insert(disabled.begin(), disabled.end());
    SgDebug() << "Disabled " << disabled.size() << " lines.\n";
}

//----------------------------------------------------------------------------

#endif // GOAUTOBOOK_H
