//----------------------------------------------------------------------------
/** @file SgVectorTest.cpp
    Unit tests for SgVector.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "SgVector.h"

//----------------------------------------------------------------------------

namespace {

BOOST_AUTO_TEST_CASE(SgVectorTestConstructor)
{
    SgVector<int> a;
    BOOST_CHECK(a.IsEmpty());
    BOOST_CHECK(! a.NonEmpty());
    BOOST_CHECK_EQUAL(a.Length(), 0);
}

BOOST_AUTO_TEST_CASE(SgVectorTestOperator_Equal)
{
    SgVector<int> a;
    a.PushBack(123);
    a.PushBack(444);
    a.PushBack(789);
    SgVector<int> b;
    b = a;
    BOOST_CHECK_EQUAL(b.Length(), 3);
    BOOST_CHECK(a == b);
    a = a;
    BOOST_CHECK_EQUAL(a.Length(), 3);
}

BOOST_AUTO_TEST_CASE(SgVectorTestAssign)
{
    SgVector<int> a;
    a.PushBack(123);
    a.PushBack(444);
    a.PushBack(789);
    SgVector<int> b;
    b = a;
    BOOST_CHECK_EQUAL(b[0], 123);
    BOOST_CHECK_EQUAL(b[1], 444);
    BOOST_CHECK_EQUAL(b[2], 789);
    BOOST_CHECK(a == b);
    b.PushBack(0);
    BOOST_CHECK(! (a == b));
    BOOST_CHECK(a != b);
}

BOOST_AUTO_TEST_CASE(SgVectorTestAssignElement)
{
    SgVector<int> a;
    a.PushBack(0);
    a.PushBack(789);
    BOOST_CHECK_EQUAL(a[1], 789);
    a[1] = 444;
    BOOST_CHECK_EQUAL(a[1], 444);
    BOOST_CHECK_EQUAL(a.Length(), 2);
}

BOOST_AUTO_TEST_CASE(SgVectorTestClear)
{
    SgVector<int> a;
    a.Clear();
    BOOST_CHECK(a.IsEmpty());
    BOOST_CHECK(! a.NonEmpty());
    BOOST_CHECK_EQUAL(a.Length(), 0);
    a.PushBack(123);
    BOOST_CHECK(! a.IsEmpty());
    BOOST_CHECK(a.NonEmpty());
    BOOST_CHECK_EQUAL(a.Length(), 1);
    a.Clear();
    BOOST_CHECK(a.IsEmpty());
    BOOST_CHECK(! a.NonEmpty());
    BOOST_CHECK_EQUAL(a.Length(), 0);
    a.PushBack(0);
    a.PushBack(789);
    a.PushBack(123);
    BOOST_CHECK_EQUAL(a.Length(), 3);
    a.Clear();
    SG_ASSERT(a.IsEmpty());
}

BOOST_AUTO_TEST_CASE(SgVectorTestPushBackList)
{
    SgVector<int> a;
    a.PushBack(1);
    a.PushBack(2);
    a.PushBack(3);
    SgVector<int> b;
    b.PushBack(30);
    b.PushBack(20);
    b.PushBack(10);
    a.PushBackList(b);
    BOOST_CHECK(b.IsLength(3));
    BOOST_CHECK(a.IsLength(6));
    BOOST_CHECK_EQUAL(a[0], 1);
    BOOST_CHECK_EQUAL(a[1], 2);
    BOOST_CHECK_EQUAL(a[2], 3);
    BOOST_CHECK_EQUAL(a[3], 30);
    BOOST_CHECK_EQUAL(a[4], 20);
    BOOST_CHECK_EQUAL(a[5], 10);
    BOOST_CHECK_EQUAL(b[0], 30);
    BOOST_CHECK_EQUAL(b[1], 20);
    BOOST_CHECK_EQUAL(b[2], 10);
}

BOOST_AUTO_TEST_CASE(SgVectorTestConcat)
{
    SgVector<int> a;
    a.PushBack(1);
    a.PushBack(2);
    a.PushBack(3);
    SgVector<int> b;
    b.PushBack(3);
    b.PushBack(2);
    b.PushBack(1);
    a.Concat(&b);
    BOOST_CHECK(b.IsEmpty());
    BOOST_CHECK(a.IsLength(6));
}

BOOST_AUTO_TEST_CASE(SgVectorTestContains)
{
    SgVector<int> a;
    BOOST_CHECK(! a.Contains(0));
    BOOST_CHECK(! a.Contains(1));
    for (int i = 0; i < 10; ++i)
        a.PushBack(i);
    for (int i = 0; i < 10; ++i)
        BOOST_CHECK(a.Contains(i));
    BOOST_CHECK(! a.Contains(10));
    BOOST_CHECK(! a.Contains(11));
    BOOST_CHECK(! a.Contains(-1));
}

BOOST_AUTO_TEST_CASE(SgVectorTestExclude)
{
    SgVector<int> a;
    a.PushBack(789);
    a.PushBack(666);
    a.PushBack(123);
    BOOST_CHECK(! a.Exclude(555));
    BOOST_CHECK(a.Exclude(666));
    BOOST_CHECK(! a.Contains(666));
}

BOOST_AUTO_TEST_CASE(SgVectorTestExclude_2)
{
    SgVector<int> a;
    for (int i = 0; i < 10; ++i)
        a.PushBack(i);
    SgVector<int> b;
    b.PushBack(7);
    b.PushBack(0);
    b.PushBack(3);
    b.PushBack(12);
    a.Exclude(b);
    BOOST_CHECK_EQUAL(a.Length(), 7);
    BOOST_CHECK_EQUAL(a[0], 1);
    BOOST_CHECK_EQUAL(a[1], 2);
    BOOST_CHECK_EQUAL(a[2], 4);
    BOOST_CHECK_EQUAL(a[3], 5);
}

BOOST_AUTO_TEST_CASE(SgVectorTestInclude)
{
    SgVector<int> a;
    a.Include(789);
    a.Include(666);
    a.Include(123);
    BOOST_CHECK_EQUAL(a.Length(), 3);
    a.Include(123);
    BOOST_CHECK_EQUAL(a.Length(), 3);
    a.Include(789);
    a.Include(666);
    a.Include(123);
    BOOST_CHECK_EQUAL(a.Length(), 3);
    a.Include(-123);
    BOOST_CHECK_EQUAL(a.Length(), 4);
    a.Exclude(666);
    BOOST_CHECK_EQUAL(a.Length(), 3);
    a.Include(666);
    BOOST_CHECK_EQUAL(a.Length(), 4);
}


BOOST_AUTO_TEST_CASE(SgVectorTestInsert)
{
    SgVector<int> a;                 
    a.PushBack(123);
    a.PushBack(666);
    a.PushBack(789);
    BOOST_CHECK(a.IsSorted());
    bool result = a.Insert(555);
    BOOST_CHECK(result);
    BOOST_CHECK(a.IsSorted());
    BOOST_CHECK_EQUAL(a.Length(), 4);    
    BOOST_CHECK_EQUAL(a[1], 555);    
    result = a.Insert(555); // same number - do not insert.
    BOOST_CHECK(! result);
    BOOST_CHECK_EQUAL(a[1], 555);    
    BOOST_CHECK_EQUAL(a.Length(), 4);    
    result = a.Insert(5555);
    BOOST_CHECK(result);
    BOOST_CHECK(a.IsSorted());
    BOOST_CHECK_EQUAL(a[4], 5555);    
    BOOST_CHECK_EQUAL(a.Length(), 5);    
    result = a.Insert(-3);
    BOOST_CHECK(result);
    BOOST_CHECK(a.IsSorted());
    BOOST_CHECK_EQUAL(a[0], -3);    
    BOOST_CHECK_EQUAL(a.Length(), 6);    
    result = a.Insert(100000);
    BOOST_CHECK(result);
    BOOST_CHECK(a.IsSorted());
    BOOST_CHECK_EQUAL(a[6], 100000);    
    BOOST_CHECK_EQUAL(a.Length(), 7);    
}

BOOST_AUTO_TEST_CASE(SgVectorTestIsSorted)
{
    // ascending
    SgVector<int> a;                 
    BOOST_CHECK(a.IsSorted());
    for (int i = 0; i < 10; ++i)
        a.PushBack(i);
    BOOST_CHECK(a.IsSorted());
    BOOST_CHECK(a.IsSortedAndUnique());
    a.PushBack(9);
    BOOST_CHECK(a.IsSorted());
    BOOST_CHECK(! a.IsSortedAndUnique());
    a.PushBack(5);    
    BOOST_CHECK(! a.IsSorted()); 
    BOOST_CHECK(! a.IsSortedAndUnique());
    
    // descending
    SgVector<int> b;                 
    for (int i = 10; i > 0; --i)
        b.PushBack(i);
    BOOST_CHECK(b.IsSorted(false));
    b.PushBack(5);    
    BOOST_CHECK(! b.IsSorted(false));    
}

BOOST_AUTO_TEST_CASE(SgVectorTestIterator)
{
    SgVector<int> a;                 
    for (int i = 0; i < 10; ++i)
        a.PushBack(i);
    int count = 0;
    for (SgVectorIterator<int> it(a); it; ++it)
    {
        BOOST_CHECK_EQUAL(*it, count);
        ++count;
    }
    BOOST_CHECK_EQUAL(count, 10);    
}

BOOST_AUTO_TEST_CASE(SgVectorTestLargeList)
{
    SgVector<int> a;                 
    for (int i = 1; i <= 1000; ++i)
        a.PushBack(i);
    BOOST_CHECK(a.MinLength(1000));
    BOOST_CHECK(a.IsLength(1000));
    BOOST_CHECK(a.Contains(233));
    BOOST_CHECK(a.Contains(234));
    for (int i = 1; i <= 1000; i += 2)
        a.Exclude(i);
    BOOST_CHECK(a.MaxLength(1000));
    BOOST_CHECK(a.IsLength(500));
    BOOST_CHECK(! a.Contains(233));
    BOOST_CHECK(a.Contains(234));
    int y = a.Index(2);
    BOOST_CHECK_EQUAL(y, 0);
    y = a.Index(456);
    BOOST_CHECK_EQUAL(y, 227);
    BOOST_CHECK(a.Contains(456));
    a.DeleteAt(y);
    BOOST_CHECK(! a.Contains(456));
    BOOST_CHECK(a.Contains(2));
    a.DeleteAt(0);
    BOOST_CHECK(! a.Contains(2));
    BOOST_CHECK(a.Contains(1000));
    a.DeleteAt(a.Length() - 1);
    BOOST_CHECK(! a.Contains(1000));
}


BOOST_AUTO_TEST_CASE(SgVectorTestLimitListLength)
{
    SgVector<int> a;                 
    for (int i = 1; i <= 10; ++i)
        a.PushBack(i);
    BOOST_CHECK_EQUAL(a.Length(), 10);
    a.LimitListLength(5);
    BOOST_CHECK_EQUAL(a.Length(), 5);
    a.LimitListLength(10);
    BOOST_CHECK_EQUAL(a.Length(), 5);
}

BOOST_AUTO_TEST_CASE(SgVectorTestPopFront)
{
    SgVector<int> a;
    a.PushBack(123);
    a.PushBack(456);
    int x = a.PopFront();
    BOOST_CHECK_EQUAL(x, 123);
    BOOST_CHECK_EQUAL(a.Length(), 1);
    BOOST_CHECK_EQUAL(a.Front(), 456);
    BOOST_CHECK_EQUAL(a.Back(), 456);
    a.PushBack(x);
    BOOST_CHECK_EQUAL(a.Length(), 2);
    BOOST_CHECK_EQUAL(a[0], 456);
    BOOST_CHECK_EQUAL(a[1], 123);
}

BOOST_AUTO_TEST_CASE(SgVectorTestMerge)
{
    SgVector<int> a;
    for (int i = 0; i < 10; i+=2) // 0,2,4,6,8
        a.PushBack(i);
    SgVector<int> b;
    b.PushBack(1);
    b.PushBack(5);
    b.PushBack(7);
    b.PushBack(11);
    b.PushBack(12);
    a.Merge(b);
    BOOST_CHECK_EQUAL(b.Length(), 5);
    BOOST_CHECK_EQUAL(a.Length(), 10);
    BOOST_CHECK_EQUAL(a[0], 0);
    BOOST_CHECK_EQUAL(a[1], 1);
    BOOST_CHECK_EQUAL(a[2], 2);
    BOOST_CHECK_EQUAL(a[7], 8);
    BOOST_CHECK_EQUAL(a[9], 12);
}

BOOST_AUTO_TEST_CASE(SgVectorTestMerge_2)
{
    SgVector<int> a;
    for (int i = 0; i < 10; i+=2) // 0,2,4,6,8
        a.PushBack(i);
    SgVector<int> b;
    b.PushBack(-10);
    b.PushBack(-5);
    b.PushBack(-2);
    b.PushBack(-1);
    a.Merge(b);
    BOOST_CHECK_EQUAL(b.Length(), 4);
    BOOST_CHECK_EQUAL(a.Length(), 9);
    BOOST_CHECK_EQUAL(a[0], -10);
    BOOST_CHECK_EQUAL(a[1], -5);
    BOOST_CHECK_EQUAL(a[4], 0);
    BOOST_CHECK_EQUAL(a[8], 8);
}

BOOST_AUTO_TEST_CASE(SgVectorTestMerge_3)
{
    SgVector<int> a;
    SgVector<int> b;
    for (int i = 0; i < 10; i+=2) // 0,2,4,6,8
        b.PushBack(i);
    a.Merge(b);
    BOOST_CHECK_EQUAL(a.Length(), 5);
    BOOST_CHECK_EQUAL(a[0], 0);
    BOOST_CHECK_EQUAL(a[1], 2);
    BOOST_CHECK_EQUAL(a[4], 8);
}

BOOST_AUTO_TEST_CASE(SgVectorTestMerge_4)
{
    SgVector<int> a;
    a.PushBack(3);
    a.PushBack(4);
    SgVector<int> b;
    for (int i = 0; i < 10; i+=2) // 0,2,4,6,8
        b.PushBack(i);
    a.Merge(b);
    BOOST_CHECK_EQUAL(a.Length(), 6);
    BOOST_CHECK_EQUAL(a[0], 0);
    BOOST_CHECK_EQUAL(a[1], 2);
    BOOST_CHECK_EQUAL(a[2], 3);
    BOOST_CHECK_EQUAL(a[3], 4);
    BOOST_CHECK_EQUAL(a[4], 6);
    BOOST_CHECK_EQUAL(a[5], 8);
}

BOOST_AUTO_TEST_CASE(SgVectorTestPopBack)
{
    SgVector<int> a;
    for (int i = 0; i < 10; ++i)
        a.PushBack(i);
    BOOST_CHECK_EQUAL(a.Length(), 10);
    for (int i = 9; i >= 0; --i)
    {
        a.PopBack();
        BOOST_CHECK_EQUAL(a.Length(), i);
    }
    BOOST_CHECK(a.IsEmpty());
}

BOOST_AUTO_TEST_CASE(SgVectorTestPushFront)
{
    SgVector<int> a;
    for (int i = 10; i < 20; ++i)
        a.PushBack(i);
    a.PushFront(0);
    a.PushFront(1);
    BOOST_CHECK_EQUAL(a[0], 1);
    BOOST_CHECK_EQUAL(a[1], 0);
    BOOST_CHECK_EQUAL(a.Length(), 12);
}

BOOST_AUTO_TEST_CASE(SgVectorTestPushBack)
{
    SgVector<int> a;
    a.PushBack(123);
    BOOST_CHECK_EQUAL(a.Length(), 1);
    BOOST_CHECK_EQUAL(a.Front(), 123);
    BOOST_CHECK_EQUAL(a.Back(), 123);
    a.PushBack(456);
    BOOST_CHECK_EQUAL(a.Length(), 2);
    BOOST_CHECK_EQUAL(a.Front(), 123);
    BOOST_CHECK_EQUAL(a.Back(), 456);
    BOOST_CHECK_EQUAL(a[0], 123);
    BOOST_CHECK_EQUAL(a[1], 456);
}

BOOST_AUTO_TEST_CASE(SgVectorTestRemoveDuplicates)
{
    SgVector<int> a;                 
    BOOST_CHECK(a.IsSorted());
    for (int i = 0; i < 10; ++i)
        a.PushBack(i);
    BOOST_CHECK(a.UniqueElements());
    a.PushBack(9);
    BOOST_CHECK_EQUAL(a.Length(), 11);
    BOOST_CHECK(! a.UniqueElements());
    a.RemoveDuplicates();
    BOOST_CHECK_EQUAL(a.Length(), 10);
    BOOST_CHECK(a.UniqueElements());
    for (int i = 9; i >= 0; --i)
        a.PushBack(i);
    BOOST_CHECK_EQUAL(a.Length(), 20);
    BOOST_CHECK(! a.UniqueElements());
    a.RemoveDuplicates();
    BOOST_CHECK_EQUAL(a.Length(), 10);
    BOOST_CHECK(a.UniqueElements());
}

BOOST_AUTO_TEST_CASE(SgVectorTestReverse)
{
    SgVector<int> a;                 
    for (int i = 0; i < 10; ++i)
        a.PushBack(i);
    a.Reverse();
    for (int i = 0; i < 10; ++i)
        BOOST_CHECK_EQUAL(a[i], 9 - i);
}

BOOST_AUTO_TEST_CASE(SgVectorTestSetTo_Element)
{
    SgVector<int> a;
    a.PushBack(0);
    a.PushBack(1);
    BOOST_CHECK_EQUAL(a.Length(), 2);
    a.SetTo(5);
    BOOST_CHECK_EQUAL(a.Length(), 1);
    BOOST_CHECK_EQUAL(a[0], 5);
}

BOOST_AUTO_TEST_CASE(SgVectorTestSetTo_Array)
{
    SgVector<int> a;
    a.PushBack(0);
    a.PushBack(1);
    BOOST_CHECK_EQUAL(a.Length(), 2);
    int b[5] = {5, 4, 3, 2, 1};
    a.SetTo(b, 5);
    BOOST_CHECK_EQUAL(a.Length(), 5);
    BOOST_CHECK_EQUAL(a[0], 5);
    BOOST_CHECK_EQUAL(a[1], 4);
    BOOST_CHECK_EQUAL(a[2], 3);
    BOOST_CHECK_EQUAL(a[3], 2);
    BOOST_CHECK_EQUAL(a[4], 1);
    a.SetTo(b, 2);
    BOOST_CHECK_EQUAL(a.Length(), 2);
    BOOST_CHECK_EQUAL(a[0], 5);
    BOOST_CHECK_EQUAL(a[1], 4);
    a.SetTo(b, 0);
    BOOST_CHECK_EQUAL(a.Length(), 0);
}

BOOST_AUTO_TEST_CASE(SgVectorTestSort)
{
    SgVector<int> a;
    a.PushBack(10);
    a.PushBack(32);
    a.PushBack(13);
    a.PushBack(100);
    a.PushBack(13);
    a.PushBack(2);
    a.PushBack(-3);
    BOOST_CHECK(! a.IsSorted());
    a.Sort();
    BOOST_CHECK(a.IsSorted());
    BOOST_CHECK_EQUAL(a.Length(), 7);
}

BOOST_AUTO_TEST_CASE(SgVectorTestSortedRemoveDuplicates)
{
    SgVector<int> a;
    for (int i = 0; i < 10; i+=2) // 0,2,4,6,8
    {
        a.PushBack(i);
        a.PushBack(i);
    }
    a.SortedRemoveDuplicates();
    BOOST_CHECK_EQUAL(a.Length(), 5);
    BOOST_CHECK_EQUAL(a[0], 0);
    BOOST_CHECK_EQUAL(a[1], 2);
    BOOST_CHECK_EQUAL(a[4], 8);
    a.SortedRemoveDuplicates();
    BOOST_CHECK_EQUAL(a.Length(), 5);
    a.PushBack(8);
    a.SortedRemoveDuplicates();
    BOOST_CHECK_EQUAL(a.Length(), 5);
}

BOOST_AUTO_TEST_CASE(SgVectorTestSwapWith)
{
    SgVector<int> a;
    a.PushBack(1);
    a.PushBack(2);
    a.PushBack(3);
    SgVector<int> b;
    b.PushBack(4);
    b.PushBack(5);
    BOOST_CHECK_EQUAL(a.Length(), 3);
    BOOST_CHECK_EQUAL(b.Length(), 2);
    a.SwapWith(&b);
    BOOST_CHECK_EQUAL(a.Length(), 2);
    BOOST_CHECK_EQUAL(b.Length(), 3);
    BOOST_CHECK_EQUAL(a[0], 4);
    BOOST_CHECK_EQUAL(a[1], 5);
    BOOST_CHECK_EQUAL(b[0], 1);
    BOOST_CHECK_EQUAL(b[1], 2);
    BOOST_CHECK_EQUAL(b[2], 3);
    b.Clear();
    b.SwapWith(&a);
    BOOST_CHECK_EQUAL(a.Length(), 0);
    BOOST_CHECK_EQUAL(b.Length(), 2);
    b.SwapWith(&a);
    BOOST_CHECK_EQUAL(a.Length(), 2);
    BOOST_CHECK_EQUAL(b.Length(), 0);
}

BOOST_AUTO_TEST_CASE(SgVectorTestTopNth)
{
    SgVector<int> a;
    for (int i = 0; i < 10; ++i)
        a.PushBack(i);
    for (int i = 1; i <= 10; ++i)
        BOOST_CHECK_EQUAL(a.TopNth(i), 10 - i);
}

void MakeTestVector(SgVector<int>& a, SgVectorOf<int>& pa)
{
    for (int i = 0; i < 10; ++i)
        a.PushBack(i);
    for (int i = 0; i < 10; ++i)
        pa.PushBack(&a[i]);
}

BOOST_AUTO_TEST_CASE(SgVectorOfTestPushBack)
{
    SgVector<int> a;
    SgVectorOf<int> pa;
    MakeTestVector(a, pa);
    for (int i = 0; i < 10; ++i)
    {
        BOOST_CHECK_EQUAL(pa[i], &a[i]);
        BOOST_CHECK_EQUAL(*pa[i], i);
    }
}

BOOST_AUTO_TEST_CASE(SgVectorOfTestContains)
{
    SgVector<int> a;
    SgVectorOf<int> pa;
    MakeTestVector(a, pa);
    for (int i = 0; i < 10; ++i)
    {
        BOOST_CHECK(pa.Contains(&a[i]));
    }
}

BOOST_AUTO_TEST_CASE(SgVectorIteratorOfTest)
{
    SgVector<int> a;
    SgVectorOf<int> pa;
    MakeTestVector(a, pa);
    
    int i = 0;
    for (SgVectorIteratorOf<int> it(pa); it; ++it,++i)
    {
        BOOST_CHECK_EQUAL(*it, &a[i]);
        BOOST_CHECK_EQUAL(**it, i);
    }
    BOOST_CHECK_EQUAL(i, 10);
}

BOOST_AUTO_TEST_CASE(SgVectorTestPairIterator_EmptyVector)
{
    SgVector<int> a;                 
    SgVectorPairIterator<int> it(a);
    int e1, e2;
    BOOST_CHECK(! it.NextPair(e1, e2));
}

BOOST_AUTO_TEST_CASE(SgVectorTestPairIterator_OneElement)
{
    SgVector<int> a;                 
    a.PushBack(1);
    SgVectorPairIterator<int> it(a);
    int e1, e2;
    BOOST_CHECK(! it.NextPair(e1, e2));
}

BOOST_AUTO_TEST_CASE(SgVectorTestPairIterator_TwoElements)
{
    SgVector<int> a;                 
    a.PushBack(1);
    a.PushBack(2);
    SgVectorPairIterator<int> it(a);
    int e1, e2;
    BOOST_CHECK(it.NextPair(e1, e2));
    BOOST_CHECK_EQUAL(e1, 1);
    BOOST_CHECK_EQUAL(e2, 2);
    BOOST_CHECK(! it.NextPair(e1, e2));
}

BOOST_AUTO_TEST_CASE(SgVectorTestPairIterator_ThreeElements)
{
    SgVector<int> a;                 
    a.PushBack(1);
    a.PushBack(2);
    a.PushBack(3);
    SgVectorPairIterator<int> it(a);
    int e1, e2;
    BOOST_CHECK(it.NextPair(e1, e2));
    BOOST_CHECK_EQUAL(e1, 1);
    BOOST_CHECK_EQUAL(e2, 2);
    BOOST_CHECK(it.NextPair(e1, e2));
    BOOST_CHECK_EQUAL(e1, 1);
    BOOST_CHECK_EQUAL(e2, 3);
    BOOST_CHECK(it.NextPair(e1, e2));
    BOOST_CHECK_EQUAL(e1, 2);
    BOOST_CHECK_EQUAL(e2, 3);
    BOOST_CHECK(! it.NextPair(e1, e2));
    BOOST_CHECK_EQUAL(e1, 2);
    BOOST_CHECK_EQUAL(e2, 3);
}

} // namespace

//----------------------------------------------------------------------------

