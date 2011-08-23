//----------------------------------------------------------------------------
/** @file SgUctTreeUtilTest.cpp
    Unit tests for classes in SgUctTreeUtil.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include "SgUctSearch.h"
#include "SgUctTreeUtil.h"

using namespace std;

//----------------------------------------------------------------------------

namespace {

BOOST_AUTO_TEST_CASE(SgUctTreeUtilTest_ApplyFilter)
{
    SgUctTree tree;
    tree.CreateAllocators(1);
    tree.SetMaxNodes(10);
    vector<SgMoveInfo> moves;
    moves.push_back(SgMoveInfo(10));
    moves.push_back(SgMoveInfo(20));
    moves.push_back(SgMoveInfo(30));
    const SgUctNode& root = tree.Root();
    tree.CreateChildren(0, root, moves);

    vector<SgMove> rootFilter;
    rootFilter.push_back(20);

    tree.ApplyFilter(0, root, rootFilter);
    BOOST_CHECK_EQUAL(root.NuChildren(), 2);
    SgUctChildIterator it(tree, root);
    BOOST_REQUIRE(it);
    BOOST_CHECK_EQUAL((*it).Move(), 10);
    ++it;
    BOOST_REQUIRE(it);
    BOOST_CHECK_EQUAL((*it).Move(), 30);
}

BOOST_AUTO_TEST_CASE(SgUctTreeUtilTest_ExtractSubtree)
{
    /* Test tree
       numbers in nodes: position indices
       (SgMove integers are node index times 10)

           (0)
          / | \
         /  |  \
       (1) (2) (3)
           / \
          /   \
         (4)  (5) <-- Root for ExtractSubtree()
              / \
             /   \
            (6)  (7)
    */
    SgUctTree tree;
    tree.CreateAllocators(1);
    tree.SetMaxNodes(10);
    vector<SgMoveInfo> moves;
    moves.push_back(SgMoveInfo(10));
    moves.push_back(SgMoveInfo(20));
    moves.push_back(SgMoveInfo(30));
    const SgUctNode* node;
    node = &tree.Root();
    tree.CreateChildren(0, *node, moves);

    node = SgUctTreeUtil::FindChildWithMove(tree, *node, 20);
    moves.clear();
    moves.push_back(SgMoveInfo(40));
    moves.push_back(SgMoveInfo(50));
    tree.CreateChildren(0, *node, moves);

    node = SgUctTreeUtil::FindChildWithMove(tree, *node, 50);
    moves.clear();
    moves.push_back(60);
    moves.push_back(70);
    tree.CreateChildren(0, *node, moves);

    BOOST_REQUIRE_NO_THROW(tree.CheckConsistency());

    SgUctTree target;
    target.CreateAllocators(1);
    target.SetMaxNodes(10);
    vector<SgMove> sequence;
    sequence.push_back(20);
    sequence.push_back(50);
    SgUctTreeUtil::ExtractSubtree(tree, target, sequence, false);
    BOOST_REQUIRE_NO_THROW(target.CheckConsistency());
    BOOST_CHECK_EQUAL(3u, target.NuNodes());

    node = SgUctTreeUtil::FindChildWithMove(target, target.Root(), 60);
    BOOST_CHECK(node != 0);
    node = SgUctTreeUtil::FindChildWithMove(target, target.Root(), 70);
    BOOST_CHECK(node != 0);
}

/** Test that extracting subtree does not overflow target tree allocator.
    Tests for a bug that could lead to the overflow of a target allocator
    array, if multiple allocators are used, even if the trees have the same
    maximum size. The reason was that the target allocator index was cycled
    through, but the source nodes could came from arbitrary allocators
    of the source tree. Therefore it can happen that one of the target
    allocators overflows, even if all the source allocators are below their
    maximum limits.
*/
BOOST_AUTO_TEST_CASE(SgUctTreeUtilTest_ExtractSubtree_Overflow)
{
    SgUctTree tree;
    tree.CreateAllocators(2);
    tree.SetMaxNodes(10); // 5 nodes per allocator
    vector<SgMoveInfo> moves;
    moves.push_back(SgMoveInfo(10));
    moves.push_back(SgMoveInfo(20));
    moves.push_back(SgMoveInfo(30));
    const SgUctNode* node;
    node = &tree.Root();
    tree.CreateChildren(0, *node, moves); // 3 nodes into allocator 0

    node = SgUctTreeUtil::FindChildWithMove(tree, *node, 10);
    moves.clear();
    moves.push_back(SgMoveInfo(40));
    moves.push_back(SgMoveInfo(50));
    tree.CreateChildren(0, *node, moves); // 2 nodes into allocator 0

    node = SgUctTreeUtil::FindChildWithMove(tree, *node, 40);
    moves.clear();
    moves.push_back(SgMoveInfo(60));
    moves.push_back(SgMoveInfo(70));
    moves.push_back(SgMoveInfo(80));
    tree.CreateChildren(1, *node, moves); // 3 nodes into allocator 1

    // In the buggy implementation, the target allocator was cycling
    // while traversing to the source tree, therefore target allocator 0
    // had 6 nodes.

    SgUctTree target;
    target.CreateAllocators(2);
    target.SetMaxNodes(10);
    vector<SgMove> sequence;
    SgUctTreeUtil::ExtractSubtree(tree, target, sequence, false);

    // We don't care, if ExtractSubtree cuts the target tree or ensures
    // that the target nodes are distributed evenly, as long as none of the
    // target allocators overflows.
    // Note that in the old implementation this triggered an assertion in
    // debug mode, but in release mode the allocator capacity was exceeded,
    // which triggered a reallocation in the vector and invalidated
    // all node pointer members in the SgUctNode elements.

    BOOST_CHECK(target.NuNodes(0) <= 5);
    BOOST_CHECK(target.NuNodes(1) <= 5);
}

} // namespace

//----------------------------------------------------------------------------
