//----------------------------------------------------------------------------
/** @file GoBook.h
    Opening book.
*/
//----------------------------------------------------------------------------

#ifndef GO_BOOK_H
#define GO_BOOK_H

#include <iosfwd>
#include <map>
#include <string>
#include <vector>
#include "GtpEngine.h"
#include "SgHash.h"
#include "SgPoint.h"

class GoBoard;

//----------------------------------------------------------------------------

/** Opening book.
    The file format contains a single line per entry, containing a board size,
    a sequence that leads to the position and, seperated with a pipe symbol,
    a list of moves to play in this position. Example:
    @verbatim
    19 | Q3 Q4
    19 P3 D16 | Q16
    19 Q3 | C3 C17 D3 D4 D16 Q16 R16 R17
    9 E5 D3 G4 | E7
    @endverbatim
    Equivalent positions are derived automatically by rotating and/or
    mirroring. If there are duplicates, because of sequences with move
    transpositions or rotating/mirroring, reading will throw an exception
    containing an error message with line number information of the
    duplicates.
*/

class GoGtpEngine;

class GoBook
{
public:
    class Entry
    {
    public:
        /** Board size. */
        int m_size;

        /** Line number in the original file. */
        int m_line;

        std::vector<SgPoint> m_sequence;

        std::vector<SgPoint> m_moves;

        /** Setup entry on a board. */
        void ApplyTo(GoBoard& bd) const;
    };

    /** Add a book move to the current position.
        @throws SgException if move cannot be added (illegal or move sequence
        to current position cannot be determined)
    */
    void Add(const GoBoard& bd, SgPoint move);

    void Clear();

    /** Deletes a book move in the current position.
        @throws SgException if the move is not a book move.
    */
    void Delete(const GoBoard& bd, SgPoint move);

    /** Get an entry.
        @param index The index of the entry
        @see NuEntries()
    */
    const Entry& GetEntry(std::size_t index) const;

    /** Return line number of current position in file the book was loaded
        from.
        @return Line number or 0, if current position is not in book or
        was not in book, when the book was loaded.
    */
    int Line(const GoBoard& bd) const;

    /** Randomly select a move for the current position.
        Returns SG_NULLMOVE if position is not in opening book.
    */
    SgPoint LookupMove(const GoBoard& bd) const;

    std::vector<SgPoint> LookupAllMoves(const GoBoard& bd) const;

    std::size_t NuEntries() const;

    /** Read book from stream.
        @param in
        @param streamName Name used for error messages (e.g. file name)
     */
    void Read(std::istream& in, const std::string& streamName = "");

    /** Read book from file. */
    void Read(const std::string& filename);

    void Write(std::ostream& out) const;

    void WriteInfo(std::ostream& out) const;

private:
    class MapEntry
    {
    public:
        /** Board size.
            The hash code is not enough to differentiate positions with
            different board sizes, since the empty board always has the
            same hash code.
        */
        int m_size;

        /** Index of the original (untransformed) book entry in m_entries. */
        std::size_t m_id;

        /** Rotation as used in SgPointUtil::Rotate() */
        int m_rotation;
    };

    typedef std::multimap<SgHashCode,MapEntry> Map;

    bool m_warningMaxSizeShown;

    int m_lineCount;

    std::string m_streamName;

    std::vector<Entry> m_entries;

    /** Mapping hash key to entries. */
    Map m_map;

    void InsertEntry(const std::vector<SgPoint>& sequence,
                     const std::vector<SgPoint>& moves, int size,
                     GoBoard& tempBoard, int line);

    const GoBook::MapEntry* LookupEntry(const GoBoard& bd) const;

    void ParseLine(const std::string& line, GoBoard& tempBoard);

    std::vector<SgPoint> ReadPoints(std::istream& in) const;

    void ThrowError(const std::string& message) const;
};

inline const GoBook::Entry& GoBook::GetEntry(std::size_t index) const
{
    SG_ASSERT(index < m_entries.size());
    return m_entries[index];
}

inline std::size_t GoBook::NuEntries() const
{
    return m_entries.size();
}

//----------------------------------------------------------------------------

/** GTP commands for GoBook. */
class GoBookCommands
{
public:
    GoBookCommands(GoGtpEngine &engine, const GoBoard& bd, GoBook& book);

    void AddGoGuiAnalyzeCommands(GtpCommand& cmd);

    void Register(GtpEngine& e);

    /** @page gobookcommands GoBookCommands
        - @link CmdAdd() @c book_add @endlink
        - @link CmdClear() @c book_clear @endlink
        - @link CmdDelete() @c book_delete @endlink
        - @link CmdInfo() @c book_info @endlink
        - @link CmdLoad() @c book_load @endlink
        - @link CmdMoves() @c book_moves @endlink
        - @link CmdPosition() @c book_position @endlink
        - @link CmdSave() @c book_save @endlink
        - @link CmdSaveAs() @c book_save_as @endlink
    */
    /** @name Command Callbacks */
    // @{
    // The callback functions are documented in the cpp file
    void CmdAdd(GtpCommand& cmd);
    void CmdClear(GtpCommand& cmd);
    void CmdDelete(GtpCommand& cmd);
    void CmdInfo(GtpCommand& cmd);
    void CmdLoad(GtpCommand& cmd);
    void CmdMoves(GtpCommand& cmd);
    void CmdPosition(GtpCommand& cmd);
    void CmdSave(GtpCommand& cmd);
    void CmdSaveAs(GtpCommand& cmd);
    // @} // @name

private:
    GoGtpEngine &m_engine;

    const GoBoard& m_bd;

    GoBook& m_book;

    std::string m_fileName;

    void PositionInfo(GtpCommand& cmd);
};

//----------------------------------------------------------------------------

#endif // GO_BOOK_H
