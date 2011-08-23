//----------------------------------------------------------------------------
/** @file GoBook.cpp
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoBook.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include "GoBoard.h"
#include "GoBoardUtil.h"
#include "GoGtpCommandUtil.h"
#include "GoGtpEngine.h"
#include "GoModBoard.h"
#include "SgDebug.h"
#include "SgException.h"
#include "SgWrite.h"
#include "SgRandom.h"

using namespace std;

//----------------------------------------------------------------------------

namespace {

template<typename T>
bool Contains(const vector<T>& v, const T& elem)
{
    typename vector<T>::const_iterator end = v.end();
    return (find(v.begin(), end, elem) != end);
}

template<typename T>
bool Erase(vector<T>& v, const T& elem)
{
    typename vector<T>::iterator end = v.end();
    typename vector<T>::iterator pos = find(v.begin(), end, elem);
    if (pos != end)
    {
        v.erase(pos);
        return true;
    }
    return false;
}

vector<SgPoint> GetSequence(const GoBoard& bd)
{
    vector<SgPoint> result;
    SgBlackWhite toPlay = SG_BLACK;
    for (int i = 0; i < bd.MoveNumber(); ++i)
    {
        GoPlayerMove move = bd.Move(i);
        if (move.Color() != toPlay)
            throw SgException("cannot add position "
                              "(non-alternating move sequence)");
        result.push_back(move.Point());
        toPlay = SgOppBW(toPlay);
    }
    return result;
}

} // namepsace

//----------------------------------------------------------------------------

void GoBook::Entry::ApplyTo(GoBoard& bd) const
{
    if (bd.Size() != m_size)
        bd.Init(m_size);
    GoBoardUtil::UndoAll(bd);
    for (vector<SgPoint>::const_iterator it = m_sequence.begin();
         it != m_sequence.end(); ++it)
    {
        SG_ASSERT(bd.IsLegal(*it));
        bd.Play(*it);
    }
}

//----------------------------------------------------------------------------

void GoBook::Add(const GoBoard& bd, SgPoint move)
{
    if (move != SG_PASS && bd.Occupied(move))
        throw SgException("point is not empty");
    if (! bd.IsLegal(move))
        throw SgException("move is not legal");
    const GoBook::MapEntry* mapEntry = LookupEntry(bd);
    if (mapEntry == 0)
    {
        vector<SgPoint> moves;
        moves.push_back(move);
        GoBoard tempBoard;
        InsertEntry(GetSequence(bd), moves, bd.Size(), tempBoard, 0);
    }
    else
    {
        size_t id = mapEntry->m_id;
        SG_ASSERT(id < m_entries.size());
        Entry& entry = m_entries[id];
        int invRotation = SgPointUtil::InvRotation(mapEntry->m_rotation);
        SgPoint rotMove = SgPointUtil::Rotate(invRotation, move, bd.Size());
        if (! Contains(entry.m_moves, rotMove))
            entry.m_moves.push_back(rotMove);
    }
}

void GoBook::Clear()
{
    m_entries.clear();
    m_map.clear();
}

void GoBook::Delete(const GoBoard& bd, SgPoint move)
{
    const GoBook::MapEntry* mapEntry = LookupEntry(bd);
    if (mapEntry == 0)
        return;
    size_t id = mapEntry->m_id;
    SG_ASSERT(id < m_entries.size());
    Entry& entry = m_entries[id];
    int invRotation = SgPointUtil::InvRotation(mapEntry->m_rotation);
    SgPoint rotMove = SgPointUtil::Rotate(invRotation, move, bd.Size());
    if (! Erase(entry.m_moves, rotMove))
        throw SgException("GoBook::Delete: move not found");
}

/** Insert a new position entry and all its transformations
    @param sequence A move sequence that leads to the position
    @param moves The moves to play in this position
    @param size
    @param tempBoard A local temporary work board, reused for efficiency
    (does not have to be initialized)
    @param line Line number of entry from the file (0 if unknown or entry is
    not from a file)
*/
void GoBook::InsertEntry(const vector<SgPoint>& sequence,
                         const vector<SgPoint>& moves, int size,
                         GoBoard& tempBoard, int line)
{
    if (moves.size() == 0)
        ThrowError("Line contains no moves");
    if (tempBoard.Size() != size)
        tempBoard.Init(size);
    Entry entry;
    entry.m_size = size;
    entry.m_line = line;
    entry.m_sequence = sequence;
    entry.m_moves = moves;
    m_entries.push_back(entry);
    size_t id = m_entries.size() - 1;
    vector<Map::iterator> newEntries;
    for (int rot = 0; rot < 8; ++rot)
    {
        GoBoardUtil::UndoAll(tempBoard);
        for (vector<SgPoint>::const_iterator it = sequence.begin();
             it != sequence.end(); ++it)
        {
            SgPoint p = SgPointUtil::Rotate(rot, *it, size);
            if (! tempBoard.IsLegal(p))
                ThrowError("Illegal move in variation");
            tempBoard.Play(p);
        }
        // It is enough to check the moves for legality for one rotation
        if (rot == 0)
            for (vector<SgPoint>::const_iterator it = moves.begin();
                 it != moves.end(); ++it)
                if (! tempBoard.IsLegal(SgPointUtil::Rotate(rot, *it, size)))
                    ThrowError("Illegal move in move list");
        MapEntry mapEntry;
        mapEntry.m_size = size;
        mapEntry.m_id = id;
        mapEntry.m_rotation = rot;
        SgHashCode hashCode = tempBoard.GetHashCodeInclToPlay();
        pair<Map::iterator,Map::iterator> e = m_map.equal_range(hashCode);
        bool isInNewEntries = false;
        for (Map::iterator it = e.first; it != e.second; ++it)
            if (it->second.m_size == size)
            {
                if (! Contains(newEntries, it))
                {
                    ostringstream o;
                    o << "Entry duplicates line "
                      << m_entries[it->second.m_id].m_line;
                    ThrowError(o.str());
                }
                isInNewEntries = true;
                break;
            }
        if (! isInNewEntries)
        {
            Map::iterator newIt =
                m_map.insert(Map::value_type(hashCode, mapEntry));
            newEntries.push_back(newIt);
        }
    }
}

int GoBook::Line(const GoBoard& bd) const
{
    const GoBook::MapEntry* mapEntry = LookupEntry(bd);
    if (mapEntry == 0)
        return 0;
    size_t id = mapEntry->m_id;
    SG_ASSERT(id < m_entries.size());
    return m_entries[id].m_line;
}

const GoBook::MapEntry* GoBook::LookupEntry(const GoBoard& bd) const
{
    SgHashCode hashCode = bd.GetHashCodeInclToPlay();
    typedef Map::const_iterator Iterator;
    pair<Iterator,Iterator> e = m_map.equal_range(hashCode);
    int size = bd.Size();
    for (Iterator it = e.first; it != e.second; ++it)
        if (it->second.m_size == size)
            return &it->second;
    return 0;
}

SgPoint GoBook::LookupMove(const GoBoard& bd) const
{
    vector<SgPoint> moves = LookupAllMoves(bd);
    int nuMoves = moves.size();
    if (nuMoves == 0)
        return SG_NULLMOVE;
    SgPoint p = moves[SgRandom::Global().Int(nuMoves)];
    return p;
}

vector<SgPoint> GoBook::LookupAllMoves(const GoBoard& bd) const
{
    vector<SgPoint> result;
    const GoBook::MapEntry* mapEntry = LookupEntry(bd);
    if (mapEntry == 0)
        return result;
    size_t id = mapEntry->m_id;
    SG_ASSERT(id < m_entries.size());
    const vector<SgPoint>& moves = m_entries[id].m_moves;
    const int rotation = mapEntry->m_rotation;
    const int size = mapEntry->m_size;
    for (vector<SgPoint>::const_iterator it = moves.begin();
         it != moves.end(); ++it)
    {
        SgPoint p = SgPointUtil::Rotate(rotation, *it, size);
        if (! bd.IsLegal(p))
        {
            // Should not happen with 64-bit hashes, but not impossible
            SgWarning() << "illegal book move (hash code collision?)\n";
            result.clear();
            break;
        }
        result.push_back(p);
    }
    return result;
}

void GoBook::ParseLine(const string& line, GoBoard& tempBoard)
{
    istringstream in(line);
    int size;
    in >> size;
    if (size < 1)
        ThrowError("Invalid size");
    if (size > SG_MAX_SIZE && ! m_warningMaxSizeShown)
    {
        SgDebug() << "GoBook::ParseLine: Ignoring size=" << size << '\n';
        m_warningMaxSizeShown = true;
        return;
    }
    vector<SgPoint> variation = ReadPoints(in);
    vector<SgPoint> moves = ReadPoints(in);
    InsertEntry(variation, moves, size, tempBoard, m_lineCount);
}

void GoBook::Read(istream& in, const string& streamName)
{
    Clear();
    m_warningMaxSizeShown = false;
    m_streamName = streamName;
    m_lineCount = 0;
    GoBoard tempBoard;
    while (in)
    {
        string line;
        getline(in, line);
        ++m_lineCount;
        if (line == "")
            continue;
        ParseLine(line, tempBoard);
    }
}

void GoBook::Read(const string& filename)
{
    ifstream in(filename.c_str());
    if (! in)
        throw SgException("Cannot find file " + filename);
    Read(in, filename);
}

vector<SgPoint> GoBook::ReadPoints(istream& in) const
{
    vector<SgPoint> result;
    while (true)
    {
        string s;
        in >> s;
        if (! in || s == "|")
            break;
        istringstream in2(s);
        SgPoint p;
        in2 >> SgReadPoint(p);
        if (! in2)
            ThrowError("Invalid point");
        result.push_back(p);
    }
    return result;
}

void GoBook::ThrowError(const string& message) const
{
    ostringstream out;
    if (m_streamName != "")
        out << m_streamName << ':';
    out << m_lineCount << ": " << message;
    throw SgException(out.str());
}

void GoBook::Write(ostream& out) const
{
    for (vector<Entry>::const_iterator it = m_entries.begin();
         it != m_entries.end(); ++it)
    {
        if (it->m_moves.empty())
        {
            SgDebug() << "pruning empty entry line=" << it->m_line << '\n';
            continue;
        }
        out << it->m_size << ' ';
        for (vector<SgPoint>::const_iterator it2 = it->m_sequence.begin();
             it2 != it->m_sequence.end(); ++it2)
            out << SgWritePoint(*it2) << ' ';
        out << '|';
        for (vector<SgPoint>::const_iterator it2 = it->m_moves.begin();
             it2 != it->m_moves.end(); ++it2)
            out << ' ' << SgWritePoint(*it2);
        out << '\n';
    }
}

void GoBook::WriteInfo(ostream& out) const
{
    out << SgWriteLabel("NuBasic") << m_entries.size() << '\n'
        << SgWriteLabel("NuTransformed") << m_map.size() << '\n';
}

//----------------------------------------------------------------------------

GoBookCommands::GoBookCommands(GoGtpEngine &engine, const GoBoard& bd, GoBook& book)
    : m_engine(engine),
      m_bd(bd),
      m_book(book)
{
}

void GoBookCommands::AddGoGuiAnalyzeCommands(GtpCommand& cmd)
{
    cmd <<
        "gfx/Book Add/book_add %p\n"
        "none/Book Clear/book_clear\n"
        "gfx/Book Delete/book_delete %p\n"
        "hstring/Book Info/book_info\n"
        "none/Book Load/book_load %r\n"
        "plist/Book Moves/book_moves\n"
        "gfx/Book Position/book_position\n"
        "none/Book Save/book_save\n"
        "none/Book Save As/book_save_as %w\n";
}

/** Add a move for the current position to the book.
    Arguments: point <br>
    Returns: Position information after the move deletion as in CmdPosition()
*/
void GoBookCommands::CmdAdd(GtpCommand& cmd)
{
    cmd.CheckNuArg(1);
    SgPoint p = GoGtpCommandUtil::PointArg(cmd, m_bd);
    try
    {
        m_book.Add(m_bd, p);
    }
    catch (const SgException& e)
    {
        throw GtpFailure(e.what());
    }
    PositionInfo(cmd);
}

void GoBookCommands::CmdClear(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    m_book.Clear();
}

/** Delete a move for the current position to the book.
    Arguments: point <br>
    Returns: Position information after the move deletion as in CmdPosition()
*/
void GoBookCommands::CmdDelete(GtpCommand& cmd)
{
    cmd.CheckNuArg(1);
    vector<SgPoint> moves = m_book.LookupAllMoves(m_bd);
    if (moves.empty())
        throw GtpFailure("book contains no moves for current position");
    SgPoint p = GoGtpCommandUtil::PointArg(cmd, m_bd);
    try
    {
        m_book.Delete(m_bd, p);
    }
    catch (const SgException& e)
    {
        throw GtpFailure(e.what()) << SgWritePoint(p)
                                   << " is not a book move";
    }
    PositionInfo(cmd);
}

/** Show information about current book. */
void GoBookCommands::CmdInfo(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    cmd << SgWriteLabel("FileName") << m_fileName << '\n';
    m_book.WriteInfo(cmd);
}

void GoBookCommands::CmdLoad(GtpCommand& cmd)
{
    cmd.CheckNuArg(1);
    m_fileName = cmd.Arg(0);
    try
    {
        m_book.Read(m_fileName);
    }
    catch (const SgException& e)
    {
        m_fileName = "";
        throw GtpFailure() << "loading opening book failed: " << e.what();
    }
}

void GoBookCommands::CmdMoves(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    vector<SgPoint> active = m_book.LookupAllMoves(m_bd);
    for (vector<SgPoint>::const_iterator it = active.begin();
         it != active.end(); ++it)
        cmd << SgWritePoint(*it) << ' ';
}

/** Show book information for current positions.
    This command is compatible with the GoGui analyze command type "gfx".
    Moves in the book for the current position are marked with a green
    color (active moves), moves that lead to a known position in the book
    with yellow (other moves). The status line shows the line number of the
    position in the file (0, if unknown) and the number of active and other
    moves.
*/
void GoBookCommands::CmdPosition(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    PositionInfo(cmd);
}

void GoBookCommands::CmdSave(GtpCommand& cmd)
{
    if (m_engine.MpiSynchronizer()->IsRootProcess())
    {
    cmd.CheckArgNone();
    if (m_fileName == "")
        throw GtpFailure("no filename associated with current book");
    ofstream out(m_fileName.c_str());
    m_book.Write(out);
    if (! out)
    {
        throw GtpFailure() << "error writing to file '" << m_fileName << "'";
    }
    }
}

void GoBookCommands::CmdSaveAs(GtpCommand& cmd)
{
    if (m_engine.MpiSynchronizer()->IsRootProcess())
    {
    cmd.CheckNuArg(1);
    m_fileName = cmd.Arg(0);
    ofstream out(m_fileName.c_str());
    m_book.Write(out);
    if (! out)
    {
        m_fileName = "";
        throw GtpFailure("write error");
    }
    }
}

void GoBookCommands::PositionInfo(GtpCommand& cmd)
{
    vector<SgPoint> active = m_book.LookupAllMoves(m_bd);
    vector<SgPoint> other;
    {
        GoModBoard modBoard(m_bd);
        GoBoard& bd = modBoard.Board();
        for (GoBoard::Iterator it(bd); it; ++it)
            if (bd.IsLegal(*it) && ! Contains(active, *it))
            {
                bd.Play(*it);
                if (! m_book.LookupAllMoves(bd).empty())
                    other.push_back(*it);
                bd.Undo();
            }
    }
    cmd << "COLOR green";
    for (vector<SgPoint>::const_iterator it = active.begin();
         it != active.end(); ++it)
        cmd << ' ' << SgWritePoint(*it);
    cmd << "\nCOLOR yellow";
    for (vector<SgPoint>::const_iterator it = other.begin();
         it != other.end(); ++it)
        cmd << ' ' << SgWritePoint(*it);
    int line = m_book.Line(m_bd);
    cmd << "\nTEXT Line=" << line << " Active=" << active.size() << " Other="
        << other.size() << '\n';
}

void GoBookCommands::Register(GtpEngine& e)
{
    e.Register("book_add", &GoBookCommands::CmdAdd, this);
    e.Register("book_clear", &GoBookCommands::CmdClear, this);
    e.Register("book_delete", &GoBookCommands::CmdDelete, this);
    e.Register("book_info", &GoBookCommands::CmdInfo, this);
    e.Register("book_load", &GoBookCommands::CmdLoad, this);
    e.Register("book_moves", &GoBookCommands::CmdMoves, this);
    e.Register("book_position", &GoBookCommands::CmdPosition, this);
    e.Register("book_save", &GoBookCommands::CmdSave, this);
    e.Register("book_save_as", &GoBookCommands::CmdSaveAs, this);
}

//----------------------------------------------------------------------------
