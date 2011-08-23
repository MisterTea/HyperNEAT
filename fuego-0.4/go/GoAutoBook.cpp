//----------------------------------------------------------------------------
/** @file GoAutoBook.cpp 
 */
//----------------------------------------------------------------------------

#include "GoAutoBook.h"

//----------------------------------------------------------------------------

GoAutoBookState::GoAutoBookState(const GoBoard& brd)
    : m_synchronizer(brd)
{
    m_synchronizer.SetSubscriber(m_brd[0]);
}

GoAutoBookState::~GoAutoBookState()
{
}

SgHashCode GoAutoBookState::GetHashCode() const
{
    return m_hash;
}

void GoAutoBookState::Synchronize()
{
    m_synchronizer.UpdateSubscriber();
    int size = m_brd[0].Size();
    int numMoves = m_brd[0].MoveNumber();
    for (int rot = 1; rot < 8; ++rot)
    {
        m_brd[rot].Init(size, size);
        for (int i = 0; i < numMoves; ++i)
        {
            SgMove move = m_brd[0].Move(i).Point();
            m_brd[rot].Play(SgPointUtil::Rotate(rot, move, size));
        }
    }
    ComputeHashCode();
}

void GoAutoBookState::Play(SgMove move)
{
    m_hash = m_brd[0].GetHashCodeInclToPlay();
    for (int rot = 0; rot < 8; ++rot)
        m_brd[rot].Play(SgPointUtil::Rotate(rot, move, m_brd[0].Size()));
    ComputeHashCode();
}

void GoAutoBookState::Undo()
{
    for (int rot = 0; rot < 8; ++rot)
        m_brd[rot].Undo();
    ComputeHashCode();
}

void GoAutoBookState::ComputeHashCode()
{
    m_hash = m_brd[0].GetHashCodeInclToPlay();
    for (int rot = 1; rot < 8; ++rot)
    {
        SgHashCode curHash = m_brd[rot].GetHashCodeInclToPlay();
        if (curHash < m_hash)
            m_hash = curHash;
    }
}

//----------------------------------------------------------------------------

GoAutoBookParam::GoAutoBookParam()
    : m_usageCountThreshold(0),
      m_selectType(GO_AUTOBOOK_SELECT_VALUE)
{
}

//----------------------------------------------------------------------------

GoAutoBook::GoAutoBook(const std::string& filename,
                       const GoAutoBookParam& param)
    : m_param(param), 
      m_filename(filename)
{
    std::ifstream is(filename.c_str());
    if (!is)
    {
        std::ofstream of(filename.c_str());
        if (!of)
            throw SgException("Invalid file name!");
        of.close();
    }
    else
    {
        while (is)
        {
            std::string line;
            std::getline(is, line);
            if (line.size() < 19)
                continue;
            std::string str;
            std::istringstream iss(line);
            iss >> str;
            SgHashCode hash;
            hash.FromString(str);
            SgBookNode node(line.substr(19));
            m_data[hash] = node;
        }
        SgDebug() << "GoAutoBook: Parsed " << m_data.size() << " lines.\n";
    }
}

GoAutoBook::~GoAutoBook()
{
}

bool GoAutoBook::Get(const GoAutoBookState& state, SgBookNode& node) const
{
    Map::const_iterator it = m_data.find(state.GetHashCode());
    if (it != m_data.end())
    {
        node = it->second;
        return true;
    }
    return false;
}

void GoAutoBook::Put(const GoAutoBookState& state, const SgBookNode& node)
{
    m_data[state.GetHashCode()] = node;
}

void GoAutoBook::Flush()
{
    Save(m_filename);
}

void GoAutoBook::Save(const std::string& filename) const
{
    std::ofstream out(filename.c_str());
    for (Map::const_iterator it = m_data.begin(); it != m_data.end(); ++it)
    {
        out << it->first.ToString() << '\t' 
            << it->second.ToString() << '\n';
    }
    out.close();
}

void GoAutoBook::Merge(const GoAutoBook& other)
{
    SgDebug() << "GoAutoBook::Merge()\n";
    std::size_t newLeafs = 0;
    std::size_t newInternal = 0;
    std::size_t leafsInCommon = 0;
    std::size_t internalInCommon = 0;
    std::size_t leafToInternal = 0;
    for (Map::const_iterator it = other.m_data.begin();
         it != other.m_data.end(); ++it)
    {
        Map::iterator mine = m_data.find(it->first);
        SgBookNode newNode(it->second);
        if (mine == m_data.end())
        {
            m_data[it->first] = it->second;
            if (newNode.IsLeaf())
                newLeafs++;
            else
                newInternal++;
        }
        else
        {
            SgBookNode oldNode(mine->second);
            if (newNode.IsLeaf() && oldNode.IsLeaf())
            {
                newNode.m_heurValue = 0.5 * (newNode.m_heurValue 
                                             + oldNode.m_heurValue);
                m_data[it->first] = newNode;
                leafsInCommon++;
            }
            else if (!newNode.IsLeaf())
            {
                // Take the max of the count; can't just add them
                // together because then merging a book with itself
                // doubles the counts of everything, which doesn't
                // make sense. Need the parent of these books and do a
                // three-way merge if we want the counts to be
                // accurate after the merge.  I don't think it matters
                // that much.
                newNode.m_count = std::max(newNode.m_count, oldNode.m_count);
                m_data[it->first] = newNode;
                if (!oldNode.IsLeaf())
                    internalInCommon++;
                else 
                    leafToInternal++;
            }
        }
    }
    SgDebug() << "Statistics\n"
              << "New Leafs        " << newLeafs << '\n'
              << "New Internal     " << newInternal << '\n'
              << "Common Leafs     " << leafsInCommon << '\n'
              << "Common Internal  " << internalInCommon << '\n'
              << "Leaf to Internal " << leafToInternal << '\n';
}

//----------------------------------------------------------------------------

void GoAutoBook::TruncateByDepth(int depth, GoAutoBookState& state,
                                 GoAutoBook& other) const
{
    std::set<SgHashCode> seen;
    TruncateByDepth(depth, state, other, seen);
}

void GoAutoBook::TruncateByDepth(int depth, GoAutoBookState& state, 
                                 GoAutoBook& other, 
                                 std::set<SgHashCode>& seen) const
{
    if (seen.count(state.GetHashCode()))
        return;
    SgBookNode node;
    if (!Get(state, node))
        return;
    seen.insert(state.GetHashCode());
    if (depth == 0)
    {
        // Set this node to be a leaf: copy its heuristic value into
        // its propagated value and set count to 0.
        node.m_count = 0;
        node.m_priority = LEAF_PRIORITY;
        node.m_value = node.m_heurValue;
        other.Put(state, node);
        return;
    }
    other.Put(state, node);
    if (node.IsLeaf() || node.IsTerminal())
        return;
    for (GoBoard::Iterator it(state.Board()); it; ++it)
    {
        if (state.Board().IsLegal(*it))
        {
            state.Play(*it);
            TruncateByDepth(depth - 1, state, other, seen);
            state.Undo();
        }
    }
}

//----------------------------------------------------------------------------

SgMove GoAutoBook::FindBestChild(GoAutoBookState& state) const
{
    std::size_t bestCount = 0;
    SgMove bestMove = SG_NULLMOVE;
    float bestScore = 100.0f;
    SgBookNode node;
    if (!Get(state, node))
        return SG_NULLMOVE;
    if (node.IsLeaf())
        return SG_NULLMOVE;
    for (GoBoard::Iterator it(state.Board()); it; ++it)
    {
        if (state.Board().IsLegal(*it))
        {
            state.Play(*it);
            if (m_disabled.count(state.GetHashCode()) > 0)
                SgDebug() << "Ignoring disabled move " 
                          << SgWritePoint(*it) << '\n';
            // NOTE: Terminal nodes aren't supported at this time, so 
            // we ignore them here.
            else if (Get(state, node) 
                     && !node.IsTerminal() 
                     && node.m_count >= m_param.m_usageCountThreshold)
            {
                if (m_param.m_selectType == GO_AUTOBOOK_SELECT_COUNT)
                {
                    // Select by count, tiebreak by value.
                    if (node.m_count > bestCount)
                    {
                        bestCount = node.m_count;
                        bestMove = *it;
                        bestScore = node.m_value;
                    }
                    // NOTE: do not have access to inverse function,
                    // so we're minimizing here as a temporary solution. 
                    else if (node.m_count == bestCount
                             && node.m_value < bestScore)
                    {
                        bestMove = *it;
                        bestScore = node.m_value;
                    }
                }
                else if (m_param.m_selectType == GO_AUTOBOOK_SELECT_VALUE)
                {
                    // NOTE: do not have access to inverse function,
                    // so we're minimizing here as a temporary solution. 
                    if (node.m_value < bestScore)
                    {
                        bestMove = *it;
                        bestScore = node.m_value;
                    }
                }
            }
            state.Undo();
        }
    }
    return bestMove;
}

SgMove GoAutoBook::LookupMove(const GoBoard& brd) const
{
    GoAutoBookState state(brd);
    state.Synchronize();
    return FindBestChild(state);
}

//----------------------------------------------------------------------------

std::vector< std::vector<SgMove> > GoAutoBook::ParseWorkList(std::istream& in)
{
    std::vector< std::vector<SgMove> > ret;
    while (in)
    {
        std::string line;
        std::getline(in, line);
        if (line == "")
            continue;
        std::vector<SgMove> var;
        std::istringstream in2(line);
        while (true)
        {
            std::string s;
            in2 >> s;
            if (! in2 || s == "|")
                break;
            std::istringstream in3(s);
            SgPoint p;
            in3 >> SgReadPoint(p);
            if (! in3)
                throw SgException("Invalid point");
            var.push_back(p);
        }
        ret.push_back(var);
    }
    SgDebug() << "GoAutoBook::ParseWorkList: Read " << ret.size() 
              << " variations.\n";
    return ret;
}

//----------------------------------------------------------------------------
