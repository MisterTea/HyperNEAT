//----------------------------------------------------------------------------
/** @file SgNodeTest.cpp
    Unit tests for SgNode.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "SgNode.h"

//----------------------------------------------------------------------------

namespace {

//----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(SgNodeTest)
{
    // Test tree functions to make sure there are no regressions.
    SgNode r;
    BOOST_CHECK(! r.HasFather());
    BOOST_CHECK(! r.HasSon());
    BOOST_CHECK(! r.HasBrother());
    SgNode* s1 = r.NewLeftMostSon();
    BOOST_CHECK(r.HasSon());
    BOOST_CHECK_EQUAL(r.LeftMostSon(), s1);
    BOOST_CHECK_EQUAL(s1->Father(), &r);
    SgNode* s2 = r.NewRightMostSon();
    BOOST_CHECK_EQUAL(r.RightMostSon(), s2);
    BOOST_CHECK_EQUAL(s1->RightBrother(), s2);
    BOOST_CHECK_EQUAL(s2->Father(), &r);
    SgVectorOf<SgNode> path;
    s2->PathToRoot(&path);
    BOOST_CHECK_EQUAL(path[0], s2);
    BOOST_CHECK_EQUAL(path[1], &r);
    int numBack;
    s1->ShortestPathTo(s2, &numBack, &path);
    BOOST_CHECK_EQUAL(numBack, 1);
    BOOST_CHECK(path.IsLength(1));
    BOOST_CHECK_EQUAL(path.Front(), s2);
    s2->PromoteNode();
    BOOST_CHECK_EQUAL(s2->RightBrother(), s1);
    int nuNodes = r.CountNodes(false);
    BOOST_CHECK_EQUAL(nuNodes, 3);
    r.DeleteSubtree();
    BOOST_CHECK(! r.HasSon());
    nuNodes = r.CountNodes(false);
    BOOST_CHECK_EQUAL(nuNodes, 1);
}

BOOST_AUTO_TEST_CASE(SgNodeTreeIndexTest)
{
    // n1-n2-n3
    //     +-n4-n5
    SgNode* n1 = new SgNode();
    SgNode* n2 = new SgNode();
    SgNode* n3 = new SgNode();
    SgNode* n4 = new SgNode();
    SgNode* n5 = new SgNode();
    n5->AppendTo(n4);
    n3->AppendTo(n2);
    n4->AppendTo(n2);
    n2->AppendTo(n1);
    BOOST_CHECK_EQUAL(SgNode::TreeIndex(0), "NIL");
    BOOST_CHECK_EQUAL(SgNode::TreeIndex(n1), "1");
    BOOST_CHECK_EQUAL(SgNode::TreeIndex(n2), "1.1");
    BOOST_CHECK_EQUAL(SgNode::TreeIndex(n3), "1.1.1");
    BOOST_CHECK_EQUAL(SgNode::TreeIndex(n4), "1.1.2");
    BOOST_CHECK_EQUAL(SgNode::TreeIndex(n5), "1.1.2.1");
    n1->DeleteTree();
}

BOOST_AUTO_TEST_CASE(SgNodeIteratorTest)
{
    // Test node iterator.
    SgNode r;
    SgNode* s1 = r.NewLeftMostSon();
    SgNode* s11 = s1->NewLeftMostSon();
    SgNode* s2 = r.NewRightMostSon();
    SgNode* s21 = s2->NewLeftMostSon();
    SgNodeConstIterator it(&r, false);
    BOOST_CHECK(it);
    BOOST_CHECK_EQUAL(*it, &r);
    ++it;
    BOOST_CHECK(it);
    BOOST_CHECK_EQUAL(*it, s1);
    ++it;
    BOOST_CHECK(it);
    BOOST_CHECK_EQUAL(*it, s11);
    ++it;
    BOOST_CHECK(it);
    BOOST_CHECK_EQUAL(*it, s2);
    ++it;
    BOOST_CHECK(it);
    BOOST_CHECK_EQUAL(*it, s21);
    ++it;
    BOOST_CHECK(! it);
    r.DeleteSubtree();
}

//----------------------------------------------------------------------------

} // namespace

