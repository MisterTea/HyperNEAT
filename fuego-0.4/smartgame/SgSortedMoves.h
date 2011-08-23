//----------------------------------------------------------------------------
/** @file SgSortedMoves.h
    Sorted table of moves.

    Move tables are used to store a small number of best moves. They
    have the usual operations Insert, Delete, etc.
*/
//----------------------------------------------------------------------------

#ifndef SG_SORTEDMOVES_H
#define SG_SORTEDMOVES_H

#include <iostream>
#include "SgRandom.h"
#include "SgVector.h"

//----------------------------------------------------------------------------

/** Small array with moves sorted by their value */
template<typename MOVE, typename VALUE, int SIZE>
class SgSortedMoves
{
public:
    /** check move table before and after each operation */
    static const bool CHECKMOVES = SG_CHECK;

    explicit SgSortedMoves(int maxNuMoves);

    /**  */
    void CheckOverflow() {m_checkOverflow = true;}

    /**  */
    void Clear();

    /** delete all but one of equal-valued moves */
    void DeleteEqual();

    /** Delete move at index.
        Shift remaining moves forward and adjust lower bound if necessary.
    */
    void Delete(int index);

    /** Insert move with given value in table.
        Update m_nuMoves, m_lowerBound of table if necessary.
    */
    void Insert(const MOVE& move, VALUE value);

    /**  */
    bool GetMove(const MOVE& move, VALUE &value, int &index) const;

    /**  */
    void GetMoves(SgVector<MOVE>* moves) const;

    /** If move in table: increase its value. otherwise insert (move,value) */
    void SetMinValue(const MOVE& move, VALUE value);

    /**  */
    void SetMove(int i, const MOVE& move)
    {
        AssertIndexRange(i);
        m_move[i] = move;
    }

    /**  */
    void SetValue(int i, VALUE value)
    {
        AssertIndexRange(i);
        m_value[i] = value;
    }

    /**  Adjust number of moves: throw out all moves whose value has become
        less than the value of m_move[m_maxNuMoves - 1]
    */
    void SetMaxMoves(int nu); // adjust all other values too

    /** Limit number of moves */
    void SetMaxNuMoves(int max);

    /** Lower bound for accepting moves */
    void SetLowerBound(VALUE bound) {m_lowerBound = bound;};

    /** See m_initLowerBound */
    void SetInitLowerBound(VALUE bound) {m_initLowerBound = bound;};

    /**  */
    int LowerBound() const {return m_lowerBound;}

    /**  */
    int NuMoves() const {return m_nuMoves;}

    /**  */
    int MaxNuMoves() const {return m_maxNuMoves;}

    /** Get move at given index i */
    const MOVE& Move(int i) const {AssertIndexRange(i); return m_move[i];}

    /** The best move is sorted first in the table */
    const MOVE& BestMove() const {return Move(0);}

    /**  */
    VALUE Value(int i) const {AssertIndexRange(i); return m_value[i];}

    /**  */
    VALUE BestValue() const
    {
        SG_ASSERT(m_nuMoves > 0);
        return m_value[0];
    }

    /**  */
    void DecNuMoves()
    {
        SG_ASSERT(m_nuMoves > 0);
        --m_nuMoves;
    }

    /** Assert i is an index within the table */
    void AssertIndexRange(int i) const
    {
        SG_DEBUG_ONLY(i);
        SG_ASSERTRANGE(i, 0, m_nuMoves - 1);
    }

    /** randomly shuffle all the moves of equal value. */
    void Shuffle();

private:
    /**  */
    int m_maxNuMoves;

    /**  */
    int m_nuMoves;

    /** Lower bound for accepting moves */
    VALUE m_lowerBound;

    /** Initial value of m_lowerBound after a Clear() */
    VALUE m_initLowerBound;

    /**  */
    bool m_checkOverflow;

    /**  */
    bool m_considerEqual;

    /**  */
    MOVE m_move[SIZE];

    /**  */
    VALUE m_value[SIZE];

    /**  */
    void CheckMoves() const;

    /** Randomly shuffle all moves in interval. They must be of equal value */
    void ShuffleInterval(int from, int to);
};

template<typename MOVE, typename VALUE, int SIZE>
SgSortedMoves<MOVE, VALUE, SIZE>::SgSortedMoves(int maxNuMoves)
    : m_maxNuMoves(maxNuMoves),
      m_checkOverflow(false),
      m_considerEqual(true)
{
    SG_ASSERT(maxNuMoves <= SIZE);
    Clear();
}

template<typename MOVE, typename VALUE, int SIZE>
void SgSortedMoves<MOVE, VALUE, SIZE>::Clear()
{
    m_initLowerBound = INT_MIN;
    SG_ASSERT(m_maxNuMoves >= 1);
    m_lowerBound = m_initLowerBound;
    m_nuMoves = 0;
}

template<typename MOVE, typename VALUE, int SIZE>
void SgSortedMoves<MOVE, VALUE, SIZE>::Delete(int index)
{
    if (CHECKMOVES)
        CheckMoves();
    SG_ASSERT(index < m_nuMoves);

    --m_nuMoves;
    for (int k = index; k <= m_nuMoves - 1; ++k)
    {
        m_move[k] = m_move[k + 1];
        m_value[k] = m_value[k + 1];
    }

    if  (m_nuMoves >= m_maxNuMoves)
        m_lowerBound = m_value[m_maxNuMoves - 1];
    else
        m_lowerBound = m_initLowerBound;

    if (CHECKMOVES)
        CheckMoves();
}

template<typename MOVE, typename VALUE, int SIZE>
void SgSortedMoves<MOVE, VALUE, SIZE>::DeleteEqual()
{
    for (int j = m_nuMoves - 2; j >= 0; --j)
    {
        for (int i = m_nuMoves - 1; i >= j + 1; --i)
        {
            if (m_value[i] == m_value[j])
            {
                Delete(SgRandom::Global().Int(2) ? j : i);
                /* */ return; /* */
            }
        }
    }
    // must exit through return in the for loop above
    SG_ASSERT(false);
}

template<typename MOVE, typename VALUE, int SIZE>
void SgSortedMoves<MOVE, VALUE, SIZE>::CheckMoves() const
{
    SG_ASSERT(m_maxNuMoves <= SIZE); // Index[0..MAX - 1]
    SG_ASSERT(m_nuMoves <= SIZE);

    for (int i = 0;  i < m_nuMoves - 1; ++i)
        SG_ASSERT(m_value[i] >= m_value[i + 1]);
    for (int i = m_maxNuMoves; i < m_nuMoves; ++i)
        SG_ASSERT(m_value[i] == m_value[m_maxNuMoves - 1]);

    SG_ASSERT((m_nuMoves == 0) || (m_value[m_nuMoves - 1] >= m_lowerBound));
}

template<typename MOVE, typename VALUE, int SIZE>
void SgSortedMoves<MOVE, VALUE, SIZE>::Insert(const MOVE& m, VALUE val)
{
    if (CHECKMOVES)
        CheckMoves();

    if (val < m_lowerBound)
        /* */ return; /* */
    // must leave 1 free element as scratch space
    int i = 0;
    while (i < m_nuMoves && m_value[i] > val)
        ++i;
    int maxMoves = m_nuMoves;
    if (maxMoves >= SIZE)
    {
        if (m_checkOverflow)
        {
            SG_ASSERT(false);
        }
        maxMoves = SIZE - 1;
    }
    else
        ++m_nuMoves;

    for (int j = maxMoves; j >= i + 1; --j)
    {
        m_move[j] = m_move[j - 1];
        m_value[j] = m_value[j - 1];
    }

    m_move[i] = m;
    m_value[i] = val;

    // throw out all m_move's whose minValue has become
    //  less than the minValue of m_move[m_maxNuMoves - 1]
    if (m_nuMoves >= m_maxNuMoves)
        m_lowerBound = std::max(m_lowerBound, m_value[m_maxNuMoves - 1]);

    if (m_nuMoves > m_maxNuMoves)
        // throw out the Max-weakest moves
        while  (  m_value[m_nuMoves - 1] < m_lowerBound
               || (  m_nuMoves > m_maxNuMoves
                  && ! m_considerEqual
                  && m_value[m_nuMoves - 1] == m_lowerBound
                  )
               )
            --m_nuMoves; // same effect as: Delete(m_nuMoves - 1);

    if (CHECKMOVES)
        CheckMoves();
}

template<typename MOVE, typename VALUE, int SIZE>
bool SgSortedMoves<MOVE, VALUE, SIZE>::GetMove(const MOVE& move,
                                               VALUE &value, int &index) const
{
    for (int i = 0; i <= m_nuMoves - 1 ; ++i)
        if (m_move[i] == move)
        {
            value = m_value[i];
            index = i;
            return true;
        }
    return false;
}

template<typename MOVE, typename VALUE, int SIZE>
void SgSortedMoves<MOVE, VALUE, SIZE>::ShuffleInterval(int from, int to)
{
    AssertIndexRange(from);
    AssertIndexRange(to);
    SG_ASSERT(m_value[from] == m_value[to]);

    for (; to > from; --to)
    {// put random element in [from..to] into to
        int i = from + SgRandom::Global().Int(to - from + 1);
        SG_ASSERT(m_value[i] == m_value[to]);
        MOVE t = m_move[i];
        m_move[i] = m_move[to];
        m_move[to] = t;
    }
}

template<typename MOVE, typename VALUE, int SIZE>
void SgSortedMoves<MOVE, VALUE, SIZE>::Shuffle()
{
    for (int i = 0; i < m_nuMoves; ++i)
    {
        int val = m_value[i];
        int j;
        for (j = i + 1; j < m_nuMoves && (val == m_value[j]); ++j)
        { }
        if (j > i + 1)
            ShuffleInterval(i, j - 1);
    }
}

template<typename MOVE, typename VALUE, int SIZE>
void SgSortedMoves<MOVE, VALUE, SIZE>::GetMoves(SgVector<MOVE>* moves) const
{
    for (int i = 0; i < m_nuMoves; ++i)
        moves->PushBack(m_move[i]);
}

template<typename MOVE, typename VALUE, int SIZE>
void SgSortedMoves<MOVE, VALUE, SIZE>::SetMinValue(const MOVE& move,
                                                   VALUE val)
{
    VALUE oldValue;
    int index;
    bool insert = true;

    if (GetMove(move, oldValue, index))
    {
        if (val > oldValue)
            Delete(index);
        else
            insert = false;
    }
    if (insert)
        Insert(move, val);
}

template<typename MOVE, typename VALUE, int SIZE>
void SgSortedMoves<MOVE, VALUE, SIZE>::SetMaxNuMoves(int max)
{
    SG_ASSERT(max >= 1);
    SG_ASSERT(max <= SIZE);
    m_maxNuMoves = max;
}

template<typename MOVE, typename VALUE, int SIZE>
void SgSortedMoves<MOVE, VALUE, SIZE>::SetMaxMoves(int nu)
{
    SetMaxNuMoves(nu);

    if (  m_nuMoves >= m_maxNuMoves
       && m_value[m_maxNuMoves - 1] > m_lowerBound
       )
        m_lowerBound = m_value[m_maxNuMoves - 1];
    if (m_nuMoves > m_maxNuMoves)
    {
        // throw out weakest moves
        while (  m_value[m_nuMoves - 1] < m_lowerBound
              || (  m_nuMoves > m_maxNuMoves
                 && ! m_considerEqual
                 && m_value[m_nuMoves - 1] == m_lowerBound
                 )
              )
            --m_nuMoves; // same as: Delete(m_nuMoves - 1);
    }
}

//----------------------------------------------------------------------------

/** write all moves in the table and their values */
template<typename MOVE, typename VALUE, int SIZE>
std::ostream& operator<<(std::ostream& str,
                         const SgSortedMoves<MOVE, VALUE, SIZE>& m)
{
    if (m.NuMoves() == 0)
        str << "none\n";
    else
    {
        str << m.NuMoves() << " (Moves, Values): ";
        for (int i = 0; i < m.NuMoves(); ++i)
        {
            str << '('
                << m.Move(i) << ", "
                << m.Value(i)
                << ')';
        }
        str << '\n';
    }
    return str;
}

//----------------------------------------------------------------------------

#endif // SG_SORTEDMOVES_H
