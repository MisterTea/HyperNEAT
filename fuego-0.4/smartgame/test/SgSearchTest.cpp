//----------------------------------------------------------------------------
/** @file SgSearchTest.cpp
    Unit tests for classes in SgSearch.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <sstream>
#include <vector>
#include <boost/test/auto_unit_test.hpp>
#include "SgDebug.h"
#include "SgHashTable.h"
#include "SgSearch.h"
#include "SgSearchControl.h"
#include "SgVector.h"

using namespace std;

//----------------------------------------------------------------------------

namespace {

/** Test class that allows to define artificial game trees for testing
    SgSearch.
    - Does not use hash tables yet
    - Black (max) is always the first player
*/
class TestSearch
    : public SgSearch
{
public:
    /** Write debug information. */
    bool m_write;

    /** Constant indicating no node for node indices. */
    static const size_t NO_NODE;

    TestSearch();

    ~TestSearch();


    /** Add node to test tree.
        @param father Index of father node, NO_NODE if root node.
    */
    void AddNode(size_t father, SgMove move, int eval);

    bool CheckDepthLimitReached() const;

    void Generate(SgVector<SgMove>* moves, int depth);

    int Evaluate(bool* isExact, int depth);

    bool Execute(SgMove move, int* delta, int depth);

    /** Index of last node in which Evaluate was called.
        For testing that a search aborts at the expected node.
    */
    size_t LastEvaluated() const;

    string MoveString(SgMove move) const;

    void TakeBack();

    SgBlackWhite GetToPlay() const;

    void SetToPlay(SgBlackWhite toPlay);

    SgHashCode GetHashCode() const;

    bool EndOfGame() const;

private:
    /** Node in the artificial game tree of class TestSearch. */
    struct TestNode
    {
        size_t m_father;

        size_t m_child;

        size_t m_sibling;

        SgMove m_move;

        int m_eval;
    };

    size_t m_currentNode;

    size_t m_lastEvaluated;

    SgBlackWhite m_toPlay;

    vector<TestNode> m_nodes;

    TestNode& CurrentNode();

    const TestNode& CurrentNode() const;

    TestNode& Node(size_t index);

    const TestNode& Node(size_t index) const;
};

const size_t TestSearch::NO_NODE = numeric_limits<size_t>::max();

TestSearch::TestSearch()
    : SgSearch(0),
      m_write(false),
      m_currentNode(0),
      m_lastEvaluated(NO_NODE),
      m_toPlay(SG_BLACK)
{
}

TestSearch::~TestSearch()
{
}


void TestSearch::AddNode(size_t father, SgMove move, int eval)
{
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
    m_nodes.push_back(node);
}

bool TestSearch::CheckDepthLimitReached() const
{
    return true;
}

inline TestSearch::TestNode& TestSearch::CurrentNode()
{
    return Node(m_currentNode);
}

inline const TestSearch::TestNode& TestSearch::CurrentNode() const
{
    return Node(m_currentNode);
}

void TestSearch::Generate(SgVector<SgMove>* moves, int depth)
{
    SG_UNUSED(depth);
    if (m_write)
        SgDebug() << "TestSearch::Generate " << m_currentNode << ": ";
    size_t child = CurrentNode().m_child;
    while (child != NO_NODE)
    {
        if (m_write)
            SgDebug() << Node(child).m_move << ' ';
        moves->PushBack(Node(child).m_move);
        child = Node(child).m_sibling;
    }
    if (m_write)
        SgDebug() << '\n';
}

int TestSearch::Evaluate(bool* isExact, int depth)
{
    SG_UNUSED(depth);
    *isExact = false;
    if (m_write)
        SgDebug() << "TestSearch::Evaluate " << m_currentNode << ": "
                  << CurrentNode().m_eval << '\n';
    m_lastEvaluated = m_currentNode;
    return CurrentNode().m_eval;
}

bool TestSearch::Execute(SgMove move, int* delta, int depth)
{
    SG_UNUSED(depth);
    if (m_write)
        SgDebug() << "TestSearch::Execute: " << move << '\n';
    SG_ASSERT(*delta == SgSearch::DEPTH_UNIT);
    SG_UNUSED(delta);
    m_toPlay = SgOppBW(m_toPlay);
    size_t child = CurrentNode().m_child;
    while (child != NO_NODE)
    {
        if (Node(child).m_move == move)
        {
            m_currentNode = child;
            return true;
        }
        child = Node(child).m_sibling;
    }
    SG_ASSERT(false);
    return false;
}

inline size_t TestSearch::LastEvaluated() const
{
    return m_lastEvaluated;
}

string TestSearch::MoveString(SgMove move) const
{
    ostringstream buffer;
    buffer << move;
    return buffer.str();
}

inline TestSearch::TestNode& TestSearch::Node(size_t index)
{
    SG_ASSERT(index < m_nodes.size());
    return m_nodes[index];
}

inline const TestSearch::TestNode& TestSearch::Node(size_t index) const
{
    SG_ASSERT(index < m_nodes.size());
    return m_nodes[index];
}

void TestSearch::TakeBack()
{
    if (m_write)
        SgDebug() << "TestSearch::TakeBack\n";
    m_toPlay = SgOppBW(m_toPlay);
    m_currentNode = CurrentNode().m_father;
    SG_ASSERT(m_currentNode != NO_NODE);
}

SgBlackWhite TestSearch::GetToPlay() const
{
    return m_toPlay;
}

SgHashCode TestSearch::GetHashCode() const
{
    return m_currentNode;
}

bool TestSearch::EndOfGame() const
{
    return false;
}

void TestSearch::SetToPlay(SgBlackWhite toPlay)
{
    m_toPlay = toPlay;
}

} // namespace

//----------------------------------------------------------------------------

namespace {

/** Search simple test tree.
    @verbatim
    Node: index(static eval), Node move is node index
    0(0)--1(0)--4(3)
       |     \--5(10)
       |
       \--2(0)--6(4)
       |
       \--3(0)--7(5)
             \--8(6)
             \--9(7)
    @endverbatim
*/
BOOST_AUTO_TEST_CASE(SgSearchTest_Simple)
{
    TestSearch search;
    // Add nodes. Parameters: father, move, eval
    search.AddNode(TestSearch::NO_NODE, SG_NULLMOVE, 0);
    search.AddNode(0, 1, 0);
    search.AddNode(0, 2, 0);
    search.AddNode(0, 3, 0);
    search.AddNode(1, 4, 3);
    search.AddNode(1, 5, 10);
    search.AddNode(2, 6, 4);
    search.AddNode(3, 7, 5);
    search.AddNode(3, 8, 6);
    search.AddNode(3, 9, 7);
    SgVector<SgMove> sequence;
    int value = search.IteratedSearch(1, 10, -1000, 1000, &sequence, true, 0);
    BOOST_CHECK_EQUAL(value, 5);
    BOOST_CHECK_EQUAL(sequence.Length(), 2);
    BOOST_CHECK_EQUAL(sequence[0], 3);
    BOOST_CHECK_EQUAL(sequence[1], 7);
}

/** Test that result from last iteration is used, if iteration was aborted
    without any value backed up to root.
    @verbatim
    Node: index(static eval), Node move is node index
    0(0)--1(-2)--4(6)
       |
       \--2(-5)
       |
       \--3(-1)
    @endverbatim
    Node control cuts search at 2-ply after node 1
    (last evaluated should be node 3 from 1-ply search)
*/
BOOST_AUTO_TEST_CASE(SgSearchTest_IncompleteIteration)
{
    TestSearch search;
    //search.m_write = true;
    // Add nodes. Parameters: father, move, eval
    search.AddNode(TestSearch::NO_NODE, SG_NULLMOVE, 0);
    search.AddNode(0, 1, -2);
    search.AddNode(0, 2, -5);
    search.AddNode(0, 3, -1);
    search.AddNode(1, 4, 6);
    SgSearchControl* control = new SgNodeSearchControl(6);
    search.SetSearchControl(control);
    SgVector<SgMove> sequence;
    int value = search.IteratedSearch(1, 10, -1000, 1000, &sequence, true, 0);
    BOOST_CHECK_EQUAL(search.LastEvaluated(), 3u);
    BOOST_CHECK_EQUAL(search.Statistics().NumNodes(), 6);
    BOOST_CHECK_EQUAL(value, 5);
    BOOST_CHECK_EQUAL(sequence.Length(), 1);
    BOOST_CHECK_EQUAL(sequence[0], 2);
    delete control;
}

/** Test that new best move is backed up in incomplete iteration.
    @verbatim
    Node: index(static eval), Node move is node index
    0(0)--1(-2)--4(4)
       |      \--5(3)
       |
       \--2(-5)--6(1)
       |
       \--3(-1)--7(1)
    @endverbatim
    Node control cuts search at 2-ply after node 6
*/
BOOST_AUTO_TEST_CASE(SgSearchTest_NewBestInIncompleteIteration)
{
    TestSearch search;
    //search.m_write = true;
    // Add nodes. Parameters: father, move, eval
    search.AddNode(TestSearch::NO_NODE, SG_NULLMOVE, 0);
    search.AddNode(0, 1, -2);
    search.AddNode(0, 2, -5);
    search.AddNode(0, 3, -1);
    search.AddNode(1, 4, 4);
    search.AddNode(1, 5, 3);
    search.AddNode(2, 6, 1);
    search.AddNode(3, 7, 1);
    SgSearchControl* control = new SgNodeSearchControl(11);
    search.SetSearchControl(control);
    SgVector<SgMove> sequence;
    int value = search.IteratedSearch(1, 10, -1000, 1000, &sequence, true, 0);
    BOOST_CHECK_EQUAL(search.LastEvaluated(), 6u);
    BOOST_CHECK_EQUAL(search.Statistics().NumNodes(), 11);
    BOOST_CHECK_EQUAL(value, 3);
    BOOST_CHECK_EQUAL(sequence.Length(), 2);
    BOOST_CHECK_EQUAL(sequence[0], 1);
    BOOST_CHECK_EQUAL(sequence[1], 5);
    delete control;
}

} // namespace

//----------------------------------------------------------------------------

