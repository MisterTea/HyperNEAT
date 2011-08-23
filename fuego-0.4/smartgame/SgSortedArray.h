//----------------------------------------------------------------------------
/** @file SgSortedArray.h
    Sorted array.
*/
//----------------------------------------------------------------------------

#ifndef SG_SORTEDARRAY_H
#define SG_SORTEDARRAY_H

#include "SgVector.h"

//----------------------------------------------------------------------------

/** Sorted array.
    Implements an array of type <code>T</code> elements that can be sorted by
    keys of type <code>K</code>.
    Class <code>K</code> needs to support comparison.
    Class <code>T</code> currently needs to be a simple type.
*/
template <class T, class K, int size>
class SgSortedArray
{
public: 
    SgSortedArray()
    {
        m_numElt = 0;
    }

    void AddItem(T elt, K key)
    {
        SG_ASSERT(m_numElt < size);
        m_elt[m_numElt] = elt;
        m_key[m_numElt] = key;
        ++m_numElt;
    }

    void SetToMin(T elt, K key)
    {
        for (int i = 0; i < m_numElt; ++i)
        {
            if (m_elt[i] == elt)
            {
                if (m_key[i] > key) m_key[i] = key;
                return;
            }
        }
        AddItem(elt, key);
    }

    void SetToMax(T elt, K key)
    {
        for (int i = 0; i < m_numElt; ++i)
        {
            if (m_elt[i] == elt)
            {
                if (m_key[i] < key)
                    m_key[i] = key;
                return;
            }
        }
        AddItem(elt, key);
    }

    void SetTo(T elt, K key)
    {
        for (int i = 0; i < m_numElt; ++i)
        {
            if (m_elt[i] == elt)
            {
                m_key[i] = key;
                return;
            }
        }
        AddItem(elt, key);
    }

    void ReduceSizeTo(int newSize)
    {
        SG_ASSERT(0 <= newSize && newSize <= m_numElt);
        m_numElt = newSize;
    }

    void SortMaximize()
    {
        for (int i = 0; i < m_numElt-1; ++i)
        {
            int maxIndex = i;
            K maxKey = m_key[maxIndex];
            for (int j = i+1; j <= m_numElt-1; ++j)
            {
                if (maxKey < m_key[j])
                {
                    maxIndex = j;
                    maxKey = m_key[maxIndex];
                }
            }
            std::swap(m_key[i], m_key[maxIndex]);
            std::swap(m_elt[i], m_elt[maxIndex]);
        }
    }

    void SortMinimize()
    {
        for (int i = 0; i < m_numElt-1; ++i)
        {
            int minIndex = i;
            K minKey = m_key[minIndex];
            for (int j = i+1; j <= m_numElt-1; ++j)
            {
                if (m_key[j] < minKey)
                {
                    minIndex = j;
                    minKey = m_key[minIndex];
                }
            }
            std::swap(m_key[i], m_key[minIndex]);
            std::swap(m_elt[i], m_elt[minIndex]);
        }
    }

    void GetElements(SgVector<T>* listOfElts) const
    {
        listOfElts->SetTo(m_elt, m_numElt);
    }

    void GetKeys(SgVector<K>* listOfKeys) const
    {
        listOfKeys->SetTo(m_key, m_numElt);
    }

    T operator[](int index) const
    {
        return m_elt[index];
    }

    K GetKey(int index) const
    {
        return m_key[index];
    }

    int Size() const
    {
        return m_numElt;
    }

    bool IsEmpty() const
    {
        return m_numElt == 0;
    }

    bool NonEmpty() const
    {
        return m_numElt != 0;
    }

    bool IsFull() const
    {
        return size <= m_numElt;
    }

private:
    int m_numElt;

    T m_elt[size];

    K m_key[size];

    /** not implemented */
    SgSortedArray(const SgSortedArray&);

    /** not implemented */
    SgSortedArray& operator=(const SgSortedArray&);
};

//----------------------------------------------------------------------------

#endif // SG_SORTEDARRAY_H
