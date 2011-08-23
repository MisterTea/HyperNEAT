//----------------------------------------------------------------------------
/** @file SgArray.h
    Static array.
*/
//----------------------------------------------------------------------------

#ifndef SG_ARRAY_H
#define SG_ARRAY_H

#include <cstring>

//----------------------------------------------------------------------------

template<typename T, int SIZE> class SgArray;

/** Helper class to allow partial specialization of SgArray::operator=.
    Partial member function specialization is not yet supported by standard
    C++.
*/
template<typename T, int SIZE>
class SgArrayAssign
{
public:
    static void Assign(T* dest, const T* src);
};

template<int SIZE>
class SgArrayAssign<int,SIZE>
{
public:
    static void Assign(int* dest, const int* src);
};

template<int SIZE>
class SgArrayAssign<bool,SIZE>
{
public:
    static void Assign(bool* dest, const bool* src);
};

template<typename T,int SIZE>
class SgArrayAssign<T*,SIZE>
{
public:
    static void Assign(T** dest, T* const * src);
};

template<typename T, int SIZE>
void SgArrayAssign<T,SIZE>::Assign(T* dest, const T* src)
{
    SG_ASSERT(dest != src); // self-assignment not supported for efficiency
    T* p = dest;
    const T* pp = src;
    for (int i = SIZE; i--; ++p, ++pp)
        *p = *pp;
}

template<int SIZE>
void SgArrayAssign<int,SIZE>::Assign(int* dest, const int* src)
{
    SG_ASSERT(dest != src); // self-assignment not supported for efficiency
    std::memcpy(dest, src, SIZE * sizeof(int));
}

template<int SIZE>
void SgArrayAssign<bool,SIZE>::Assign(bool* dest, const bool* src)
{
    SG_ASSERT(dest != src); // self-assignment not supported for efficiency
    std::memcpy(dest, src, SIZE * sizeof(bool));
}

template<typename T, int SIZE>
void SgArrayAssign<T*,SIZE>::Assign(T** dest, T* const * src)
{
    SG_ASSERT(dest != src); // self-assignment not supported for efficiency
    std::memcpy(dest, src, SIZE * sizeof(T*));
}

//----------------------------------------------------------------------------

/** Static array.
    Wrapper class around a C style array.
    Uses assertions for indices in range in debug mode.
    @deprecated Use boost::array instead
*/
template<typename T, int SIZE>
class SgArray
{
public:
    /** Local const iterator */
    class Iterator
    {
    public:
        Iterator(const SgArray& array);

        const T& operator*() const;

        void operator++();

        operator bool() const;

    private:
        const T* m_end;

        const T* m_current;
    };

    /** Local non-const iterator */
    class NonConstIterator
    {
    public:
        NonConstIterator(SgArray& array);

        T& operator*() const;

        void operator++();

        operator bool() const;

    private:
        const T* m_end;

        T* m_current;
    };

    SgArray();

    SgArray(const SgArray& array);

    explicit SgArray(const T& val);

    SgArray& operator=(const SgArray& array);

    T& operator[](int index);

    const T& operator[](int index) const;

    SgArray& operator*=(T val);

    void Fill(const T& val);

private:
    friend class Iterator;
    friend class NonConstIterator;

    T m_array[SIZE];
};

template<typename T, int SIZE>
SgArray<T,SIZE>::Iterator::Iterator(const SgArray& array)
    : m_end(array.m_array + SIZE),
      m_current(array.m_array)
{
}

template<typename T, int SIZE>
const T& SgArray<T,SIZE>::Iterator::operator*() const
{
    SG_ASSERT(*this);
    return *m_current;
}

template<typename T, int SIZE>
void SgArray<T,SIZE>::Iterator::operator++()
{
    ++m_current;
}

template<typename T, int SIZE>
SgArray<T,SIZE>::Iterator::operator bool() const
{
    return m_current < m_end;
}

template<typename T, int SIZE>
SgArray<T,SIZE>::NonConstIterator::NonConstIterator(SgArray& array)
    : m_end(array.m_array + SIZE),
      m_current(array.m_array)
{ }

template<typename T, int SIZE>
T& SgArray<T,SIZE>::NonConstIterator::operator*() const
{
    SG_ASSERT(*this);
    return *m_current;
}

template<typename T, int SIZE>
void SgArray<T,SIZE>::NonConstIterator::operator++()
{
    ++m_current;
}

template<typename T, int SIZE>
SgArray<T,SIZE>::NonConstIterator::operator bool() const
{
    return m_current < m_end;
}

template<typename T, int SIZE>
SgArray<T,SIZE>::SgArray()
{
}

template<typename T, int SIZE>
SgArray<T,SIZE>::SgArray(const SgArray& array)
{
    SG_ASSERT(&array != this); // self-assignment not supported for efficiency
    *this = array;
}

template<typename T, int SIZE>
SgArray<T,SIZE>::SgArray(const T& val)
{
    Fill(val);
}

template<typename T, int SIZE>
SgArray<T,SIZE>& SgArray<T,SIZE>::operator=(const SgArray& array)
{
    SG_ASSERT(&array != this); // self-assignment not supported for efficiency
    SgArrayAssign<T,SIZE>::Assign(m_array, array.m_array);
    return *this;
}

template<typename T, int SIZE>
T& SgArray<T,SIZE>::operator[](int index)
{
    SG_ASSERT(index >= 0);
    SG_ASSERT(index < SIZE);
    return m_array[index];
}

template<typename T, int SIZE>
const T& SgArray<T,SIZE>::operator[](int index) const
{
    SG_ASSERT(index >= 0);
    SG_ASSERT(index < SIZE);
    return m_array[index];
}

template<typename T, int SIZE>
SgArray<T,SIZE>& SgArray<T,SIZE>::operator*=(T val)
{
    T* p = m_array;
    for (int i = SIZE; i--; ++p)
        *p *= val;
    return *this;
}

template<typename T, int SIZE>
void SgArray<T,SIZE>::Fill(const T& val)
{
    T* v = m_array;
    for (int i = SIZE; i--; ++v)
        *v = val;
}

//----------------------------------------------------------------------------

#endif // SG_ARRAY_H
