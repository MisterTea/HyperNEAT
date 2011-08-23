//----------------------------------------------------------------------------
/** @file SgEvaluatedMoves.h
    Data structure for keeping move values.
*/
//----------------------------------------------------------------------------

#ifndef SG_EVALUATEDMOVES_H
#define SG_EVALUATEDMOVES_H

#include "SgPointSet.h"
#include "SgVector.h"

//----------------------------------------------------------------------------

/** Simple data structure keeps a best move value and a list of all moves
    with that value.
    @todo  Originally worked with any SgMove, but introduction of SgPointSet
    relevant now requires SgMove=SgPoint. Should be made independent of that
    again, otherwise, why not merge with SgEvaluatedMovesArray?
*/
class SgEvaluatedMoves
{
public:
    explicit SgEvaluatedMoves(const SgPointSet& relevant)
        : m_bestValue(s_minValue),
          m_relevant(relevant)
    { }

    SgEvaluatedMoves(const SgEvaluatedMoves& original)
        : m_bestValue(original.m_bestValue),
          m_moveList(original.m_moveList),
          m_relevant(original.m_relevant)
    { }
    
    virtual ~SgEvaluatedMoves()
    { }

    virtual void AddMove(SgPoint move, int value);

    virtual void AddMoves(const SgPointSet& moves, int value);

    virtual void AddMoves(const SgVector<SgPoint>& moves, int value);

    virtual void Clear()
    {
        m_bestValue = s_minValue;
        m_moveList.Clear();
    }

    SgPoint BestMove();

    int BestValue()
    {
        return m_bestValue;
    }

    const SgPointSet& Relevant() const
    {
        return m_relevant;
    }

    bool IsRelevant(SgPoint p) const
    {
        return m_relevant[p];
    }

    void Disable(SgPoint p)
    {
        m_relevant.Exclude(p);
    }

    void Enable(SgPoint p)
    {
        m_relevant.Include(p);
    }

    virtual SgEvaluatedMoves* Duplicate() const
    {
        return new SgEvaluatedMoves(*this);
    }

    virtual int GetEvaluation(SgPoint p) const
    {
        if (m_moveList.Contains(p))
            return m_bestValue;
        else
            return 0;
    }

	int moveListSize() const
	{
		return (int)m_moveList.Length();
	}

    /** Compute list of the n best moves. */
    virtual void BestMoves(SgVector<SgPoint>& best, int nuMoves) const;

protected:
    int m_bestValue;

    SgVector<SgPoint> m_moveList;

    SgPointSet m_relevant;

    // DS: INT_MIN is sometimes used to mark illegal moves
    static const int s_minValue = INT_MIN + 1;
};

//----------------------------------------------------------------------------

/** Simple data structure keeps an integer value for each point on a board.
    @todo better name: SgEvaluatedPoints?
*/
class SgEvaluatedMovesArray
    : public SgEvaluatedMoves
{
public:
    explicit SgEvaluatedMovesArray(const SgPointSet& relevant,
                                   int boardSize = SG_MAX_SIZE);

    virtual ~SgEvaluatedMovesArray()
    { }

    SgEvaluatedMovesArray(const SgEvaluatedMovesArray& original)
        : SgEvaluatedMoves(original),
          m_boardSize(original.m_boardSize)
    {
        for (int i = 0; i < SG_MAXPOINT; ++i)
            m_value[i] = original.m_value[i];
    }

    virtual void AddMove(SgPoint move, int value);

    virtual void ReduceMove(SgPoint move, int value);

    virtual void Clear();

    void Write() const;

    virtual SgEvaluatedMoves* Duplicate() const
    {
        return new SgEvaluatedMovesArray(*this);
    }

    virtual int GetEvaluation(SgPoint p) const
    {
        return m_value[p];
    }

    virtual void BestMoves(SgVector<SgPoint>& best, int nuMoves) const;

private:
    int m_value[SG_MAXPOINT];

    int m_boardSize;

    SgPoint SelectNextBest(SgVector<SgPoint>& bestSoFar) const;
};

//----------------------------------------------------------------------------

#endif // SG_EVALUATEDMOVES_H
