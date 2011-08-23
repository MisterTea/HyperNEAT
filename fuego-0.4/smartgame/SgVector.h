//----------------------------------------------------------------------------
/** @file SgVector.h
    std::vector-based replacement for previous SgList class.
*/
//----------------------------------------------------------------------------
#ifndef SG_VECTOR_H
#define SG_VECTOR_H

#include <algorithm>
#include <functional>
#include <iterator>
#include <vector>

using std::vector;

template<typename T>
class SgVector
{
public:
    /** Construct empty vector. */
    SgVector()
        : m_vec()
    { }
    
    /** Return reference to element.
        @param index Position of element in range <code>0..length-1</code>.
    */
    T& operator[](int index)
    {
        return m_vec[index];
    }

    /** Return const reference to element.
        @param index Position of element in range <code>0..length-1</code>.
    */
    const T& operator[](int index) const
    {
        return m_vec[index];
    }

    /** Assignment operator.
        Copy content of other vector.
    */
    SgVector<T>& operator=(const SgVector<T>& v);

    /** Compare whether the contents of two vectors are identical.
        Same length, and the same elements in the same sequence.
    */
    bool operator==(const SgVector<T>& rhs) const
    {
        return m_vec == rhs.m_vec;
    }

    /** Compare whether the contents of two vectors are not identical. */
    bool operator!=(const SgVector& rhs) const
    {
        return ! (*this == rhs);
    }
            
    /** Returns the last element of the vector.
        Asserts if the vector is empty.
    */
    const T& Back() const
    {
        SG_ASSERT(NonEmpty());
        return m_vec.back();
    }

    T BackAndPop()
    {
        SG_ASSERT(NonEmpty());
        T back = m_vec.back();
        PopBack();
        return back;
    }

    /** Remove all elements in this vector. */
    void Clear()
    {
        m_vec.clear();
    }

    /** Push all of <code>*tail</code> to the back of this vector,
        and clear the <code>*tail</code> vector.
        Remark: this operation is efficient for lists but not so
        meaningful for vectors.
        PushBackList may be more appropriate in most cases.
    */
    void Concat(SgVector<T>* tail);

    /** Check if element is in vector.
        @return <code>true</code> if <code>elt</code> is included in this
        vector, <code>false</code> if not.
    */
    bool Contains(const T& elt) const;
    
    /** delete element at specified index */
    void DeleteAt(int index);

    /** Remove the first occurence of element.
        If <code>elt</code> is in the vector, remove the first
        occurence of <code>elt</code> from the vector, and return
        <code>true</code>. Otherwise do nothing and return <code>false</code>.
    */
    bool Exclude(const T& elt);

    /** Exclude each element of <code>vector</code> from this vector. */
    void Exclude(const SgVector<T>& vector);

    /** Returns the first element of the vector.
        Asserts if the vector is empty.
    */
    const T& Front() const
    {
        SG_ASSERT(NonEmpty());
        return m_vec.front();
    }

    /** Find position of element.
        @returns The position of <code>elt</code> in the vector,
        in range <code>0..length-1</code>. Returns -1 if <code>elt</code>
        is not in the vector.
    */
    int Index(const T& elt) const;

    /** Push <code>elt</code> to the back of the vector only
        if it is not included yet.
    */
    void Include(const T& elt)
    {
        if (! Contains(elt))
            PushBack(elt);
    }

    /** Includes the element in a ascending sorted vector at the right place.
        Does nothing and returns <code>false</code> if the element is
        already in the vector;
        returns <code>true</code> if the element is inserted.
        @todo made two separate functions for efficiency, should be
        a template taking a compare template arg.
        Same for <code>Merge()</code> below.
    */
    bool Insert(const T& elt);

    /** Return whether this vector contains zero elements. */
    bool IsEmpty() const
    {
        return m_vec.empty();
    }

    /** Test whether a vector is as long as a given length. */
    bool IsLength (int length) const
    {
        return Length() == length;
    }

    /** Returns whether the vector is sorted in given order. */
    bool IsSorted(bool ascending = true) const;

    /** Returns whether the vector is sorted and has no duplicates. */
    bool IsSortedAndUnique(bool ascending = true) const;

    /** Return the number of elements in this vector. */
    int Length() const
    {
        return m_vec.size();
    }
    
    /** Cut off vector after at most <code>length</code> elements. */
    void LimitListLength (int limit);

    /** Test whether a vector is shorter than or equal to a given length. */ 
    bool MaxLength(int length) const
    {
        return Length() <= length;
    }

    /** Merges two sorted vectors into this. Equivalent to, but faster than:
        <pre>
        for (SgVectorIterator<T> it(vector); it; ++it)
            Insert(*it);
        </pre>
    */
    void Merge(const SgVector<T>& vector);

    /** Test whether a vector is as long as or longer than a given length. */ 
    bool MinLength(int length) const
    {
        return Length() >= length;
    }

    /** Return whether this vector contains more than zero elements. */
    bool NonEmpty() const
    {
        return ! IsEmpty();
    }

    /** Remove element[0] of the vector.
        The vector must not be empty.
        @return The element[0] of the original vector before removal.
        @deprecated Don't use this function; it is slow. 
        Only exists for SgList compatibility.
    */
    T PopFront();

    /** Remove the last element of the vector.
        The vector must not be empty.
    */
    void PopBack();

    /** Insert element at the beginning of the vector.
        @deprecated Don't use this function; it is slow.
        Only exists for SgList compatibility.
    */
    void PushFront(const T& elt);

    /** Add a single element at the end of the vector. */
    void PushBack(const T& elt)
    {
        m_vec.push_back(elt);
    }

    /** Push all elements from <code>vector</code> to the back of this. */
    void PushBackList(const SgVector<T>& vector);

    /** Removes all but the first copy of each element from the vector.
        After calling @c RemoveDuplicates(), @c UniqueElements() is @c true.
        @return true, if at least one duplicate was removed
    */
    bool RemoveDuplicates();
    
    void Reverse()
    {
        reverse(m_vec.begin(), m_vec.end());
    }
    
    /** Clear this vector and set it to contain only <code>elt</code>. */
    void SetTo(const T& elt)
    {
        Clear();
        PushBack(elt);
    }

    /** Do vectors contain the same elements, possibly in different order? */
    bool SetsAreEqual(const SgVector<T>& other) const;

    /** Clear this vector and set it to contain the <code>%count</code>
        elements from
        <code>array[0]</code> to <code>array[%count - 1]</code>.
        If <code>%count</code> is zero, the vector is just cleared.
    */
    void SetTo(const T* array, int count);

    void Sort();
    
    /** remove duplicates in a sorted vector */
    void SortedRemoveDuplicates();

    /** Swap the entire contents of this vector with <code>*vector</code>. */
    void SwapWith(SgVector<T>* vector)
    {
        std::swap(m_vec, vector->m_vec);
    }

    /** Returns the Nth-last element of the vector. It must exist.
    */
    const T& TopNth(int index) const
    {
        SG_ASSERT(NonEmpty());
        SG_ASSERT(index >= 1);
        SG_ASSERT(index <= static_cast<int>(m_vec.size()));
        return m_vec[m_vec.size() - index];
    }

    /** Include all elements from <code>set</code> into this vector.
        Appends new elements at the end of this vector.
    */
    void Union(const SgVector<T>& set);

    /** Check for duplicate elements.
        @return <code>true</code> if there are no duplicate elements in
        the vector.
        Useful for debugging.
        @todo speed it up
    */
    bool UniqueElements() const;

    std::vector<T>& Vector()
    {
        return m_vec;
    }
    
    const std::vector<T>& Vector() const
    {
        return m_vec;
    }
    
private:
    std::vector<T> m_vec;
};

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/** Vector iterator.
    More concise way to iterate (from "Large-Scale C++ Software Design" by
    John Lakos):
    <pre>
      for (SgVectorIterator<T> it(vector); it; ++it) { ... it() ... }
    </pre>
    Better performance because every method is inline.
*/
template<typename T>
class SgVectorIterator
{
public:
    /** Create a vector iterator to iterate through vector. */
    SgVectorIterator(const SgVector<T>& vec)
        : m_vec(vec),
          m_it(m_vec.Vector().begin())
    { }

    /** Copy current state of iterator.
        Useful for creating a new iterator that only runs from the current
        position to the vector end. See <code>UniqueElements()</code>
        for an example.
    */
    SgVectorIterator(const SgVectorIterator& it)
        : m_vec(it.m_vec)
    { }

    virtual ~SgVectorIterator() { }

    /** Advance the state of the iteration to the next element. */
    SgVectorIterator& operator++()
    {
        ++m_it;
        return *this;
    }

    /** Return the value of the current element. */
    const T& operator*() const
    {
        SG_ASSERT(*this);
        return *m_it;
    };

    /** Return true if iteration is valid, otherwise false. */
    operator bool() const
    {
        return m_it != m_vec.Vector().end(); // @todo cache end.
    }

private:
    const SgVector<T>& m_vec;

    typename vector<T>::const_iterator m_it;
    
    /** not implemented */
    SgVectorIterator& operator=(const SgVectorIterator&);
};

/** Typed vector of pointers to T. Pointers cannot be 0. */
template<class T>
class SgVectorOf
    : public SgVector<void*>
{
public:

    T* operator[] (int index) const
    {
        return static_cast<T*>(SgVector<void*>::operator[](index));
    }

    T* Back() const
    {
        return static_cast<T*>(SgVector<void*>::Back());
    }

    bool Contains(const T* element) const
    {
        SG_ASSERT(element);
        return SgVector<void*>::Contains(GetVoidPtr(element));
    }

    /** Push <code>elt</code> to the back of the vector unless it is
        already included. */
    void Include(const T* element)
    {
        SG_ASSERT(element);
        if (! Contains(element))
            PushBack(element);
    }

    bool Exclude(const T* element)
    {
        return SgVector<void*>::Exclude(GetVoidPtr(element));
    }

    void Exclude(const SgVectorOf<T>& vector)
    {
        SgVector<void*>::Exclude(vector);
    }

    T* Front() const
    {
        return static_cast<T*>(SgVector<void*>::Front());
    }

    bool Insert(const T* element)
    {
        return SgVector<void*>::Insert(GetVoidPtr(element));
    }

    void PushFront(const T* element)
    {
        SG_ASSERT(element);
        SgVector<void*>::PushFront(GetVoidPtr(element));
    }

    void PushBack(const T* element)
    {
        SG_ASSERT(element);
        SgVector<void*>::PushBack(GetVoidPtr(element));
    }

    T* PopFront()
    {
        return static_cast<T*>(SgVector<void*>::PopFront());
    }

#if UNUSED

    bool Extract(const T* element)
    {
        return SgVector<void*>::Extract(GetVoidPtr(element));
    }

    // The following are defined below since they use SgVectorIteratorOf

    bool ContainsContent(const T& element) const;

    void RemoveDuplicateContent();
#endif
private:

    /** Conversion of element pointer to non-const void pointer.
        @note Not sure if there is a better way without needing the
        const cast.
    */
    static void* GetVoidPtr(const T* element)
    {
        return const_cast<void*>(static_cast<const void*>(element));
    }
};

//----------------------------------------------------------------------------

/** Iterator for VectorOf<T> typed vector of pointers to T */
template<class T>
class SgVectorIteratorOf
    : private SgVectorIterator<void*>
{
public:
    /** Create a vector iterator to iterate through vector. */
    SgVectorIteratorOf(const SgVectorOf<T>& vector)
        : SgVectorIterator<void*>(static_cast<const SgVector<void*>&>(vector))
    { }

    void operator++()
    {
        SgVectorIterator<void*>::operator++();
    }

    T* operator*() const
    {
        return static_cast<T*>(SgVectorIterator<void*>::operator*());
    }

    operator bool() const
    {
        return SgVectorIterator<void*>::operator bool();
    }
};
//----------------------------------------------------------------------------

template<typename T>
SgVector<T>& SgVector<T>::operator=(const SgVector<T>& v)
{
    if (this != &v)
    {
        Clear();
        PushBackList(v);
    }
    return *this;
}

template<typename T>
void SgVector<T>::PushBackList(const SgVector<T>& v)
{
    copy(v.m_vec.begin(), v.m_vec.end(), back_inserter(m_vec));
}
    
template<typename T>
void SgVector<T>::Concat(SgVector<T>* tail)
{
    PushBackList(*tail);
    tail->Clear();
}

template<typename T>
bool SgVector<T>::Contains(const T& elt) const
{
    typename vector<T>::const_iterator end = m_vec.end();
    typename vector<T>::const_iterator pos = find(m_vec.begin(), end, elt);
    return pos != end;
}

template<typename T>
void SgVector<T>::DeleteAt(int index)
{
    SG_ASSERT(index >= 0);
    SG_ASSERT(index < Length());
    m_vec.erase(m_vec.begin() + index);
}

template<typename T>
bool SgVector<T>::Exclude(const T& elt)
{
    typename vector<T>::iterator end = m_vec.end();
    typename vector<T>::iterator pos = find(m_vec.begin(), end, elt);
    if (pos != end)
    {
        m_vec.erase(pos);
        return true;
    }
    return false;
}

template<typename T>
void SgVector<T>::Exclude(const SgVector<T>& vector)
{
    for (SgVectorIterator<T> it(vector); it; ++it)
        Exclude(*it);
}

template<typename T>
int SgVector<T>::Index(const T& elt) const
{
    typename vector<T>::const_iterator end = m_vec.end();
    typename vector<T>::const_iterator pos = find(m_vec.begin(), end, elt);
    if (pos == end)
        return -1;
    else
        return pos - m_vec.begin();
}

template<typename T>
bool SgVector<T>::Insert(const T& elt)
{
    SG_ASSERT(IsSorted());
    typename vector<T>::iterator location = 
    lower_bound(m_vec.begin(), m_vec.end(), elt);

    if (   location != m_vec.end()
        && *location == elt
       )
        return false;
    else
    {
        m_vec.insert(location, elt);
        SG_ASSERT(IsSorted());
    }
    return true;
}

template<typename T>
bool SgVector<T>::IsSorted(bool ascending) const
{
    typename vector<T>::const_iterator result;
    if (ascending)
        result = adjacent_find(m_vec.begin(), m_vec.end(), std::greater<T>());
    else
        result = adjacent_find(m_vec.begin(), m_vec.end(), std::less<T>());
    return result == m_vec.end();
}

template<typename T>
bool SgVector<T>::IsSortedAndUnique(bool ascending) const
{
    typename vector<T>::const_iterator result;
    if (ascending)
        result = adjacent_find(m_vec.begin(), m_vec.end(), 
                               std::greater_equal<T>());
    else
        result = adjacent_find(m_vec.begin(), m_vec.end(),
                               std::less_equal<T>());
    return result == m_vec.end();
}

/** Cut off vector after at most <code>length</code> elements. */
template<typename T>
void SgVector<T>::LimitListLength (int limit)
{
    if (Length() > limit)
        m_vec.resize(limit);
}

template<typename T>
void SgVector<T>::Merge(const SgVector<T>& vector)
{
    SG_ASSERT(IsSortedAndUnique());
    SG_ASSERT(vector.IsSortedAndUnique());
    if ((this == &vector) || vector.IsEmpty())
        return;
    else if (IsEmpty() || vector.Front() > Back())
        // all new elements come after all old elements, just push them back
        PushBackList(vector);
    else
    {
        const int oldSize = Length();
        PushBackList(vector);
        inplace_merge(m_vec.begin(), m_vec.begin() + oldSize, m_vec.end());
        SortedRemoveDuplicates();
    }
    SG_ASSERT(IsSortedAndUnique());
}

template<typename T>
T SgVector<T>::PopFront()
{
    SG_ASSERT(NonEmpty());
    T elt = Front();
    m_vec.erase(m_vec.begin());
    return elt;
}

template<typename T>
void SgVector<T>::PopBack()
{
    SG_ASSERT(NonEmpty());
    m_vec.pop_back();
}

template<typename T>
void SgVector<T>::PushFront(const T& elt)
{
    m_vec.insert(m_vec.begin(), elt);
}

template<typename T>
bool SgVector<T>::SetsAreEqual(const SgVector<T>& other) const
{
    if (! IsLength(other.Length()))
        return false;

    for (SgVectorIterator<T> it1(*this); it1; ++it1)
    {
        if (! other.Contains(*it1))
            return false;
    }
    for (SgVectorIterator<T> it2(other); it2; ++it2)
    {
        if (! Contains(*it2))
            return false;
    }
    return true;
}

template<typename T>
void SgVector<T>::SetTo(const T* array, int count)
{
    m_vec.assign(array, array + count);
    SG_ASSERT(IsLength(count));
}

template<typename T>
void SgVector<T>::Sort()
{
    sort(m_vec.begin(), m_vec.end());
}

template<typename T>
void SgVector<T>::Union(const SgVector<T>& set)
{
    for (SgVectorIterator<T> it(set); it; ++it)
        Include(*it);
}

template<typename T>
bool SgVector<T>::RemoveDuplicates()
{
    // @todo n^2; could be made much faster with tags
    SgVector<T> uniqueVector;
    for (SgVectorIterator<T> it(*this); it; ++it)
        if (! uniqueVector.Contains(*it))
            uniqueVector.PushBack(*it);
    SwapWith(&uniqueVector); // avoid copying
    SG_ASSERT(UniqueElements());
    return uniqueVector.Length() != Length();
}

template<typename T>
void SgVector<T>::SortedRemoveDuplicates()
{
    SG_ASSERT(IsSorted());
    if (IsEmpty())
        return;
    int prev=0;
    bool shifted = false;
    for (int i=1; i<Length(); ++i)
    {
        if (m_vec[i] != m_vec[prev])
        {
            ++prev;
            if (shifted)
                m_vec[prev] = m_vec[i];
        }
        else shifted = true;
    }
    if (shifted)
        LimitListLength(prev+1);
    SG_ASSERT(IsSortedAndUnique());
}

template<typename T>
bool SgVector<T>::UniqueElements() const
{
    // @todo n^2; could be made much faster with tags
    if (MinLength(2))
    {
        if (IsSorted())
            return IsSortedAndUnique();
        else
            for (int i = 0; i < Length() - 1; ++i)
                for (int j = i + 1; j < Length(); ++j)
                    if (m_vec[i] == m_vec[j])
                        return false;
    }
    return true;
}

//----------------------------------------------------------------------------
/** Iterator to iterate through all possible pairs of vector elements.

    Each pair is returned exactly once, i.e. with
    @verbatim
        vector = e1, e2, ... en
    @endverbatim
    the returned pairs are (in order):
    @verbatim
        (e1, e2), (e1, e3), ... (e1, en), (e2,e3), ... (e(n - 1), en)
    @endverbatim
    @todo use standard iterator format.
*/
template<typename T>
class SgVectorPairIterator
{
public:
    SgVectorPairIterator(const SgVector<T>& vector);

    virtual ~SgVectorPairIterator() { }

    /** Find the next pair of data elements.
        Return <code>true</code> and change <code>elt1</code> and
        <code>elt2</code> if not reached the end of
        the vector; return <code>false</code> and don't touch
        the parameters if at the end of the vector.
    */
    bool NextPair(T& elt1, T& elt2);

private:
    const SgVector<T>& m_vector;
    int m_index1;
    int m_index2;
};

template<typename T>
SgVectorPairIterator<T>::SgVectorPairIterator(const SgVector<T>& vector)
    : m_vector(vector), m_index1(0), m_index2(1)
{

}

template<typename T>
bool SgVectorPairIterator<T>::NextPair(T& elt1, T& elt2)
{
    if (m_index1 >= m_vector.Length() - 1)
        return false;
    elt1 = m_vector[m_index1];
    elt2 = m_vector[m_index2];
    if (++m_index2 == m_vector.Length())
    {
        ++m_index1;
        m_index2 = m_index1 + 1;
    }
    return true;
}

//----------------------------------------------------------------------------
/** Iterator for all possible pairs of ListOf<T> elements
    Each pair is returned exactly once, i.e. with
    <code>list = e1, e2,...en</code>
    the returned pairs are (in order):
    <code>(e1,e2), (e1,e3), ... (e1,en), (e2,e3),...(e(n-1),en)</code>
*/
template<class T>
class SgVectorPairIteratorOf
    : public SgVectorPairIterator<void*>
{
public:
    /** Create an iterator to iterate through all possible
        pairs of list elements.
    */
    SgVectorPairIteratorOf(const SgVectorOf<T>& list)
        : SgVectorPairIterator<void*>
          (static_cast<const SgVector<void*>&>(list))
    { }

    /** Find the next pair of data elements.
        Return @c true and change @c elt1 and @c elt2 if not reached the end
        of the list; return <code>false</code> and don't touch the parameters
        if at the end of the list.
    */
    bool NextPair(T*& elt1, T*& elt2)
    {
        void* voidPtr1;
        void* voidPtr2;
        if (SgVectorPairIterator<void*>::NextPair(voidPtr1, voidPtr2))
        {
            elt1 = static_cast<T*>(voidPtr1);
            elt2 = static_cast<T*>(voidPtr2);
            return true;
        }
        return false;
    }
};

//----------------------------------------------------------------------------
/** This template is a partial replacement for the previous SgOwnerListOf
    class. It must be called 'by hand', but the old SgOwnerListOf was
    never 100% implemented anyway.
*/
template<typename T>
void FreeAll(SgVectorOf<T>& objects)
{
    for (SgVectorIteratorOf<T> it(objects); it; ++it)
        delete *it;
    objects.Clear();
}

#endif // SG_VECTOR_H
