//----------------------------------------------------------------------------
/** @file SgUctSearchTest.cpp
    Unit tests for classes in SgUctSearch.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <sstream>
#include <vector>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include "SgDebug.h"
#include "SgUctSearch.h"
#include "SgUctTreeUtil.h"

using namespace std;

//----------------------------------------------------------------------------

namespace {

/** Write debug information. */
const bool WRITE = false;

/** Constant indicating no node for node indices. */
static const size_t NO_NODE = numeric_limits<size_t>::max();

/** Get node corresponding to a sequence of moves of length 1 */
const SgUctNode* GetNode(const SgUctTree& tree, SgMove move1)
{
    return SgUctTreeUtil::FindChildWithMove(tree, tree.Root(), move1);
}

/** Get node corresponding to a sequence of moves of length 2 */
const SgUctNode* GetNode(const SgUctTree& tree, SgMove move1, SgMove move2)
{
    const SgUctNode* node = GetNode(tree, move1);
    return SgUctTreeUtil::FindChildWithMove(tree, *node, move2);
}

/** Get node corresponding to a sequence of moves of length 3 */
const SgUctNode* GetNode(const SgUctTree& tree, SgMove move1, SgMove move2,
                         SgMove move3)
{
    const SgUctNode* node = GetNode(tree, move1, move2);
    return SgUctTreeUtil::FindChildWithMove(tree, *node, move3);
}

//----------------------------------------------------------------------------

/** Node in the artificial game tree of class Search. */
struct TestNode
{
    size_t m_father;

    size_t m_child;

    size_t m_sibling;

    SgMove m_move;

    float m_eval;

    bool m_isLeaf;
};

//----------------------------------------------------------------------------

/** Thread state for TestUctSearch.
    @note This class is not thread-safe, it can be used only, if the search
    uses only one thread.
*/
class TestThreadState
    : public SgUctThreadState
{
public:
    TestThreadState(size_t threadId, const vector<TestNode>& nodes);


    /** @name Virtual functions of SgUctThreadState */
    // @{

    float Evaluate();

    void Execute(SgMove move);

    void ExecutePlayout(SgMove move);

    bool GenerateAllMoves(std::size_t count, vector<SgMoveInfo>& moves,
                          SgProvenNodeType& provenType);

    SgMove GeneratePlayoutMove(bool& skipRaveUpdate);

    void StartSearch();

    void TakeBackInTree(size_t nuMoves);

    void TakeBackPlayout(size_t nuMoves);

    SgBlackWhite ToPlay() const;

    // @} // @name

private:
    size_t m_currentNode;

    SgBlackWhite m_toPlay;

    const vector<TestNode>& m_nodes;

    const TestNode& CurrentNode() const;

    const TestNode& Node(size_t index) const;

    void TakeBack(size_t nuMoves);
};

TestThreadState::TestThreadState(size_t threadId,
                                 const vector<TestNode>& nodes)
    : SgUctThreadState(threadId),
      m_currentNode(0),
      m_toPlay(SG_BLACK),
      m_nodes(nodes)
{
}

inline const TestNode& TestThreadState::CurrentNode() const
{
    return Node(m_currentNode);
}

void TestThreadState::Execute(SgMove move)
{
    if (WRITE)
        SgDebug() << "TestUctSearch::Execute: " << move << '\n';
    m_toPlay = SgOppBW(m_toPlay);
    size_t child = CurrentNode().m_child;
    while (child != NO_NODE)
    {
        if (Node(child).m_move == move)
        {
            m_currentNode = child;
            return;
        }
        child = Node(child).m_sibling;
    }
    SG_ASSERT(false);
}

void TestThreadState::ExecutePlayout(SgMove move)
{
    Execute(move);
}

float TestThreadState::Evaluate()
{
    if (WRITE)
        SgDebug() << "TestUctSearch::Evaluate " << m_currentNode << ": "
                  << CurrentNode().m_eval << '\n';
    SG_ASSERT(CurrentNode().m_isLeaf);
    return CurrentNode().m_eval;
}

bool TestThreadState::GenerateAllMoves(std::size_t count, 
                                       vector<SgMoveInfo>& moves,
                                       SgProvenNodeType& provenType)
{
    SG_UNUSED(provenType);
    if (WRITE)
        SgDebug() << "TestUctSearch::Generate " << m_currentNode << ": ";
    size_t child = CurrentNode().m_child;
    while (child != NO_NODE)
    {
        if (WRITE)
            SgDebug() << Node(child).m_move << ' ';
        moves.push_back(SgMoveInfo(Node(child).m_move));
        child = Node(child).m_sibling;
    }

    // Test knowledge expansion: give all children boost of one win,
    // remove last child, and add new move with move 100 and (value, count)
    // of (1.0, 10)
    if (count)
    {
        moves.pop_back();
        for (size_t i = 0; i < moves.size(); ++i) 
        {
            moves[i].m_value = 1.0;
            moves[i].m_count = 1;
        }
        moves.push_back(SgMoveInfo(100, 1.0, 10, 0, 0));
    }

    if (WRITE)
        SgDebug() << '\n';

    return false;
}

SgMove TestThreadState::GeneratePlayoutMove(bool& skipRaveUpdate)
{
    SG_UNUSED(skipRaveUpdate);
    // Search does not use randomness
    vector<SgMoveInfo> moves;
    SgProvenNodeType provenType = SG_NOT_PROVEN;
    GenerateAllMoves(0, moves, provenType);
    if (moves.empty())
        return SG_NULLMOVE;
    else
        return moves.begin()->m_move;
}

inline const TestNode& TestThreadState::Node(size_t index) const
{
    SG_ASSERT(index < m_nodes.size());
    return m_nodes[index];
}

void TestThreadState::StartSearch()
{
}

void TestThreadState::TakeBack(size_t nuMoves)
{
    if (WRITE)
        SgDebug() << "TestUctSearch::TakeBack\n";
    for (size_t i = 1; i <= nuMoves; ++i)
    {
        m_toPlay = SgOppBW(m_toPlay);
        m_currentNode = CurrentNode().m_father;
        SG_ASSERT(m_currentNode != NO_NODE);
    }
}

void TestThreadState::TakeBackInTree(size_t nuMoves)
{
    TakeBack(nuMoves);
}

void TestThreadState::TakeBackPlayout(size_t nuMoves)
{
    TakeBack(nuMoves);
}

SgBlackWhite TestThreadState::ToPlay() const
{
    return m_toPlay;
}

//----------------------------------------------------------------------------

class TestThreadStateFactory
    : public SgUctThreadStateFactory
{
public:
    TestThreadStateFactory(const vector<TestNode>& nodes);

    SgUctThreadState* Create(size_t threadId, const SgUctSearch& search);

private:
    const vector<TestNode>& m_nodes;
};

TestThreadStateFactory::TestThreadStateFactory(const vector<TestNode>& nodes)
    : m_nodes(nodes)
{
}

SgUctThreadState* TestThreadStateFactory::Create(size_t threadId,
                                                 const SgUctSearch& search)
{
    SG_UNUSED(search);
    return new TestThreadState(threadId, m_nodes);
}

//----------------------------------------------------------------------------

/** Test class that allows to define artificial game trees for testing
    class SgUctSearch.
*/
class TestUctSearch
    : public SgUctSearch
{
public:
    TestUctSearch();

    ~TestUctSearch();

    /** @name Functions for constructing the domain tree */
    // @{

    /** Add leaf node to test tree.
        @param father Index of father node, NO_NODE if root node.
        @param eval Game result
    */
    void AddLeafNode(size_t father, SgMove move, float eval);

    /** Add internal node to test tree.
        @param father Index of father node, NO_NODE if root node.
    */
    void AddNode(size_t father, SgMove move);

    // @} // @name

    /** @name Virtual functions of SgUctSearch */
    // @{

    string MoveString(SgMove move) const;

    float UnknownEval() const;

    // @} // @name

private:
    vector<TestNode> m_nodes;

    void AddNode(size_t father, SgMove move, bool isLeaf, float eval);
};

TestUctSearch::TestUctSearch()
    : SgUctSearch(new TestThreadStateFactory(m_nodes))
{
}

TestUctSearch::~TestUctSearch()
{
}

inline void TestUctSearch::AddLeafNode(size_t father, SgMove move, float eval)
{
    const bool isLeaf = true;
    AddNode(father, move, isLeaf, eval);
}

inline void TestUctSearch::AddNode(size_t father, SgMove move)
{
    const bool isLeaf = false;
    const float dummyEval = 0;
    AddNode(father, move, isLeaf, dummyEval);
}

void TestUctSearch::AddNode(size_t father, SgMove move, bool isLeaf,
                            float eval)
{
    SG_ASSERT(father == NO_NODE || ! m_nodes[father].m_isLeaf);
    TestNode node;
    int index = m_nodes.size();
    if (father != NO_NODE)
    {
        SG_ASSERT(father < m_nodes.size());
        size_t child = m_nodes[father].m_child;
        if (child == NO_NODE)
            m_nodes[father].m_child = index;
        else
        {
            while (m_nodes[child].m_sibling != NO_NODE)
                child = m_nodes[child].m_sibling;
            m_nodes[child].m_sibling = index;
        }
    }
    node.m_father = father;
    node.m_child = NO_NODE;
    node.m_sibling = NO_NODE;
    node.m_move = move;
    node.m_eval = eval;
    node.m_isLeaf = isLeaf;
    m_nodes.push_back(node);
}

string TestUctSearch::MoveString(SgMove move) const
{
    ostringstream buffer;
    buffer << move;
    return buffer.str();
}

float TestUctSearch::UnknownEval() const
{
    // UnknownEval() is only called by SgUctSearch if maximum game length was
    // exceeded, which should not happen with Search
    SG_ASSERT(false);
    return 0;
}

} // namespace

//----------------------------------------------------------------------------

namespace {

/** Search simple test tree.
    @verbatim
    Numbers are node indices; L=Loss, W=Win for player at root
    0--1--5  L
    |  \--6  W
    \--2--7  W
    |  \--8  W
    \--3--9  W
    |  \--10 L
    \--4--11 L
       \--12 L
    @endverbatim
*/
BOOST_AUTO_TEST_CASE(SgUctSearchTest_Simple)
{
    TestUctSearch search;
    search.SetExpandThreshold(1);
    //search.m_write = true;

    // Add nodes. Parameters: father, move (=target node), [eval]
    search.AddNode(NO_NODE, SG_NULLMOVE);
    search.AddNode(0, 1);
    search.AddNode(0, 2);
    search.AddNode(0, 3);
    search.AddNode(0, 4);
    search.AddLeafNode(1, 5, 0.f);
    search.AddLeafNode(1, 6, 1.f);
    search.AddLeafNode(2, 7, 1.f);
    search.AddLeafNode(2, 8, 1.f);
    search.AddLeafNode(3, 9, 1.f);
    search.AddLeafNode(3, 10, 0.f);
    search.AddLeafNode(4, 11, 0.f);
    search.AddLeafNode(4, 12, 0.f);

    search.StartSearch();

    /* Game 1
       Sequence: 1, 5 (no expand)
       Tree: [nodeIndex](count,value)
       0[0](0,0)
    */
    search.PlayGame();
    {
        const SgUctGameInfo& info = search.LastGameInfo();
        BOOST_CHECK_CLOSE(0.f, info.m_eval[0], 1e-3f);
        const vector<SgMove>& sequence = info.m_sequence[0];
        BOOST_CHECK_EQUAL(2u, sequence.size());
        BOOST_CHECK_EQUAL(1, sequence[0]);
        BOOST_CHECK_EQUAL(5, sequence[1]);
        const SgUctTree& tree = search.Tree();
        BOOST_CHECK_EQUAL(1u, tree.NuNodes());
        BOOST_CHECK_EQUAL(1u, tree.Root().MoveCount());
        BOOST_CHECK_CLOSE(0.f, tree.Root().Mean(), 1e-3f);
    }

    /* Game 2
       Sequence: 1, 5 (expand 0)
       Tree: [nodeIndex](count,value)
       0[0](2,0)--1[1](1,1)
       \----------2[2]
       \----------3[3]
       \----------4[4]
    */
    search.PlayGame();
    {
        const SgUctGameInfo& info = search.LastGameInfo();
        BOOST_CHECK_CLOSE(0.f, info.m_eval[0], 1e-3f);
        const vector<SgMove>& sequence = info.m_sequence[0];
        BOOST_CHECK_EQUAL(2u, sequence.size());
        BOOST_CHECK_EQUAL(1, sequence[0]);
        BOOST_CHECK_EQUAL(5, sequence[1]);
        const SgUctTree& tree = search.Tree();
        BOOST_CHECK_EQUAL(5u, tree.NuNodes());
        BOOST_CHECK_EQUAL(2u, tree.Root().MoveCount());
        BOOST_CHECK_CLOSE(0.f, tree.Root().Mean(), 1e-3f);
        BOOST_CHECK_EQUAL(1u, GetNode(tree, 1)->MoveCount());
        BOOST_CHECK_CLOSE(1.f, GetNode(tree, 1)->Mean(), 1e-3f);
    }

    /* Game 3
       Sequence: 2, 7 (no expand)
       Tree: [nodeIndex](count,value)
       0[0](3,0.33)--1[1](1,1)
       \-------------2[2](1,0)
       \-------------3[3]
       \-------------4[4]
    */
    search.PlayGame();
    {
        const SgUctGameInfo& info = search.LastGameInfo();
        BOOST_CHECK_CLOSE(1.f, info.m_eval[0], 1e-3f);
        const vector<SgMove>& sequence = info.m_sequence[0];
        BOOST_CHECK_EQUAL(2u, sequence.size());
        BOOST_CHECK_EQUAL(2, sequence[0]);
        BOOST_CHECK_EQUAL(7, sequence[1]);
        const SgUctTree& tree = search.Tree();
        BOOST_CHECK_EQUAL(5u, tree.NuNodes());
        BOOST_CHECK_EQUAL(3u, tree.Root().MoveCount());
        BOOST_CHECK_CLOSE(0.33f, tree.Root().Mean(), 2.f);
        BOOST_CHECK_EQUAL(1u, GetNode(tree, 2)->MoveCount());
        BOOST_CHECK_CLOSE(0.f, GetNode(tree, 2)->Mean(), 1e-3f);
    }

    /* Game 4
       Sequence: 3, 9 (no expand)
       Tree: [nodeIndex](count,value)
       0[0](4,0.5)--1[1](1,1)
       \------------2[2](1,0)
       \------------3[3](1,0)
       \------------4[4]
    */
    search.PlayGame();
    {
        const SgUctGameInfo& info = search.LastGameInfo();
        BOOST_CHECK_CLOSE(1.f, info.m_eval[0], 1e-3f);
        const vector<SgMove>& sequence = info.m_sequence[0];
        BOOST_CHECK_EQUAL(2u, sequence.size());
        BOOST_CHECK_EQUAL(3, sequence[0]);
        BOOST_CHECK_EQUAL(9, sequence[1]);
        const SgUctTree& tree = search.Tree();
        BOOST_CHECK_EQUAL(5u, tree.NuNodes());
        BOOST_CHECK_EQUAL(4u, tree.Root().MoveCount());
        BOOST_CHECK_CLOSE(0.5f, tree.Root().Mean(), 1e-3f);
        BOOST_CHECK_EQUAL(1u, GetNode(tree, 3)->MoveCount());
        BOOST_CHECK_CLOSE(0.f, GetNode(tree, 3)->Mean(), 1e-3f);
    }

    /* Game 5
       Sequence: 4, 11 (no expand)
       Tree: [nodeIndex](count,value)
       0[0](5,0.4)--1[1](1,1)
       \------------2[2](1,0)
       \------------3[3](1,0)
       \------------4[4](1,1)
    */
    search.PlayGame();
    {
        const SgUctGameInfo& info = search.LastGameInfo();
        BOOST_CHECK_CLOSE(0.f, info.m_eval[0], 1e-3f);
        const vector<SgMove>& sequence = info.m_sequence[0];
        BOOST_CHECK_EQUAL(2u, sequence.size());
        BOOST_CHECK_EQUAL(4, sequence[0]);
        BOOST_CHECK_EQUAL(11, sequence[1]);
        const SgUctTree& tree = search.Tree();
        BOOST_CHECK_EQUAL(5u, tree.NuNodes());
        BOOST_CHECK_EQUAL(5u, tree.Root().MoveCount());
        BOOST_CHECK_CLOSE(0.4f, tree.Root().Mean(), 1e-3f);
        BOOST_CHECK_EQUAL(1u, GetNode(tree, 4)->MoveCount());
        BOOST_CHECK_CLOSE(1.f, GetNode(tree, 4)->Mean(), 1e-3f);
    }
}

//----------------------------------------------------------------------------

/** Search simple test tree.
    @verbatim
    Numbers are node indices; L=Loss, W=Win for player at root
    0--1  L
    \--2  W
    \--3  W
    @endverbatim
*/
BOOST_AUTO_TEST_CASE(SgUctSearchTest_Knowledge)
{
    TestUctSearch search;
    search.SetExpandThreshold(1);
    std::vector<std::size_t> thresholds(1, 4);
    search.SetKnowledgeThreshold(thresholds);
    //search.m_write = true;

    // Add nodes. Parameters: father, move (=target node), [eval]
    search.AddNode(NO_NODE, SG_NULLMOVE);
    search.AddLeafNode(0, 1, 1.f);
    search.AddLeafNode(0, 2, 0.f);
    search.AddLeafNode(0, 3, 0.f);

    search.StartSearch();

    /* Game 1
       Sequence: 1 (no expand)
       Tree: [nodeIndex](count,value)
       0[0](1,0)
    */
    search.PlayGame();
    {
        const SgUctGameInfo& info = search.LastGameInfo();
        BOOST_CHECK_CLOSE(0.f, info.m_eval[0], 1e-3f);
        const vector<SgMove>& sequence = info.m_sequence[0];
        BOOST_CHECK_EQUAL(1u, sequence.size());
        BOOST_CHECK_EQUAL(1, sequence[0]);
        const SgUctTree& tree = search.Tree();
        BOOST_CHECK_EQUAL(1u, tree.NuNodes());
        BOOST_CHECK_EQUAL(1u, tree.Root().MoveCount());
        BOOST_CHECK_CLOSE(0.f, tree.Root().Mean(), 1e-3f);
    }

    /* Game 2
       Sequence: 1 (expand 0)
       Tree: [nodeIndex](count,value)
       0[0](2,0)--1[1](1,1)
       \----------2[2]
       \----------3[3]
    */
    search.PlayGame();
    {
        const SgUctGameInfo& info = search.LastGameInfo();
        BOOST_CHECK_CLOSE(0.f, info.m_eval[0], 1e-3f);
        const vector<SgMove>& sequence = info.m_sequence[0];
        BOOST_CHECK_EQUAL(1u, sequence.size());
        BOOST_CHECK_EQUAL(1, sequence[0]);
        const SgUctTree& tree = search.Tree();
        BOOST_CHECK_EQUAL(4u, tree.NuNodes());
        BOOST_CHECK_EQUAL(2u, tree.Root().MoveCount());
        BOOST_CHECK_CLOSE(0.f, tree.Root().Mean(), 1e-3f);
        BOOST_CHECK_EQUAL(1u, GetNode(tree, 1)->MoveCount());
        BOOST_CHECK_CLOSE(1.f, GetNode(tree, 1)->Mean(), 1e-3f);
    }

    /* Game 3
       Sequence: 2 (no expand)
       Tree: [nodeIndex](count,value)
       0[0](3,0.33)--1[1](1,1)
       \-------------2[2](1,0)
       \-------------3[3]
    */
    search.PlayGame();
    {
        const SgUctGameInfo& info = search.LastGameInfo();
        BOOST_CHECK_CLOSE(1.f, info.m_eval[0], 1e-3f);
        const vector<SgMove>& sequence = info.m_sequence[0];
        BOOST_CHECK_EQUAL(1u, sequence.size());
        BOOST_CHECK_EQUAL(2, sequence[0]);
        const SgUctTree& tree = search.Tree();
        BOOST_CHECK_EQUAL(4u, tree.NuNodes());
        BOOST_CHECK_EQUAL(3u, tree.Root().MoveCount());
        BOOST_CHECK_CLOSE(0.33f, tree.Root().Mean(), 2.f);
        BOOST_CHECK_EQUAL(1u, GetNode(tree, 2)->MoveCount());
        BOOST_CHECK_CLOSE(0.f, GetNode(tree, 2)->Mean(), 1e-3f);
    }

    /* Game 4
       Sequence: 3 (no expand)
       Tree: [nodeIndex](count,value)
       0[0](4,0.5)--1[1](1,1)
       \------------2[2](1,0)
       \------------3[3](1,0)
    */
    search.PlayGame();
    {
        const SgUctGameInfo& info = search.LastGameInfo();
        BOOST_CHECK_CLOSE(1.f, info.m_eval[0], 1e-3f);
        const vector<SgMove>& sequence = info.m_sequence[0];
        BOOST_CHECK_EQUAL(1u, sequence.size());
        BOOST_CHECK_EQUAL(3, sequence[0]);
        const SgUctTree& tree = search.Tree();
        BOOST_CHECK_EQUAL(4u, tree.NuNodes());
        BOOST_CHECK_EQUAL(4u, tree.Root().MoveCount());
        BOOST_CHECK_EQUAL(3u, tree.Root().PosCount());
        BOOST_CHECK_CLOSE(0.5f, tree.Root().Mean(), 1e-3f);
        BOOST_CHECK_EQUAL(1u, GetNode(tree, 3)->MoveCount());
        BOOST_CHECK_CLOSE(0.f, GetNode(tree, 3)->Mean(), 1e-3f);
    }

    /* Game 5
       Sequence: 2 (no expand)
       Tree: [nodeIndex](count,value)
       0[0](5,0.6)--1[1](2,1.0)
       \------------2[2](3,0.33)
       \------------100[100](10,1.0)

       Node 1 receives its 1 win bonus. Node 2 is chosen for the
       playout, and is a loss, but it also receives a bonus of a
       single win. Node 100 is a new node and is given a large bonus.
    */
    search.PlayGame();
    {
        const SgUctGameInfo& info = search.LastGameInfo();
        BOOST_CHECK_CLOSE(1.f, info.m_eval[0], 1e-3f);
        const vector<SgMove>& sequence = info.m_sequence[0];
        BOOST_CHECK_EQUAL(1u, sequence.size());
        BOOST_CHECK_EQUAL(2, sequence[0]);
        const SgUctTree& tree = search.Tree();
        // tree size is now 7: root + 3 old children + 3 new children
        BOOST_CHECK_EQUAL(7u, tree.NuNodes());
        BOOST_CHECK_EQUAL(5u, tree.Root().MoveCount());
        BOOST_CHECK_EQUAL(15u, tree.Root().PosCount());
        BOOST_CHECK_CLOSE(0.6f, tree.Root().Mean(), 1e-3f);
        BOOST_CHECK_EQUAL(2u, GetNode(tree, 1)->MoveCount());
        BOOST_CHECK_CLOSE(1.f, GetNode(tree, 1)->Mean(), 1e-3f);
        BOOST_CHECK_EQUAL(3u, GetNode(tree, 2)->MoveCount());
        BOOST_CHECK_CLOSE(0.33333333f, GetNode(tree, 2)->Mean(), 1e-3f);
        BOOST_CHECK(! GetNode(tree, 3));
        BOOST_CHECK_EQUAL(10u, GetNode(tree, 100)->MoveCount());
        BOOST_CHECK_CLOSE(1.f, GetNode(tree, 100)->Mean(), 1e-3f);
    }
}

//----------------------------------------------------------------------------

} // namespace

//----------------------------------------------------------------------------
