//----------------------------------------------------------------------------
/** @file SgIncrementalStack.h
    Incremental Update Stack for fast undo during search.
*/
//----------------------------------------------------------------------------

#ifndef SG_INCREMENTALSTACK_H
#define SG_INCREMENTALSTACK_H

#include "SgBoardColor.h"
#include "SgVector.h"
#include "SgPoint.h"

class SgPointSet;
class SgBWSet;

//----------------------------------------------------------------------------

/** constants for types of stack entries */
enum SgIncrementalStackEvent
{
    SG_NEW_POINTS          = 1000,
    SG_ADD_EMPTY           = 1001,
    SG_NEW_SAFE            = 1002,
    SG_NEW_UNSAFE          = 1003,
    SG_UNSAFE_TO_SAFE      = 1004,
    SG_CAPTURES            = 1005,
    SG_INCREMENTAL_MOVE    = 1006,
    SG_NEXTMOVE            = 1007,
    SG_UNSAFE_TO_HALF_SAFE = 1008,
    SG_CAPTURE_HALF_SAFE   = 1009
};

/** Incremental Update Stack for fast undo during search.
    Used by GoRegionBoard.
*/
class SgIncrementalStack
{
public:
    SgIncrementalStack(){}

    void Clear();

    bool IsEmpty() const {return m_stack.IsEmpty();}

    void PushPts(int type, SgEmptyBlackWhite col, const SgPointSet& pts);

    void PushPt(int type, SgEmptyBlackWhite col, SgPoint pt);
    
    void PushPtr(void* ptr)
    {
        m_stack.PushBack(IntOrPtr(ptr));
    }
    
    void PushPtrEvent(int type, void* ptr);

    void PushInt(int i)
    {
        m_stack.PushBack(IntOrPtr(i));
    }

    /** relies on SgPoint == int; add to union if that changes */
    void PushPoint(SgPoint p)
    {
        m_stack.PushBack(IntOrPtr(p));
    }

    void StartMoveInfo();

    SgIncrementalStackEvent PopEvent()
    {
        return static_cast<SgIncrementalStackEvent>(PopInt());
    }

    void* PopPtr()
    {
        void* p = m_stack.Back().m_ptr;
        m_stack.PopBack();
        return p;
    }

    int PopInt()
    {
        int i = m_stack.Back().m_int;
        m_stack.PopBack();
        return i;
    }
    
    /** relies on SgPoint == int; add to union if that changes */
    SgPoint PopPoint()
    {
        return PopInt();
    }
    
    void SubtractPoints(SgPointSet* set);

    void AddPoints(SgPointSet* set);

    void SubtractPoints(SgBWSet* set);

    void AddPoints(SgBWSet* set);

    void SubtractAndAddPoints(SgBWSet* subtractset, SgBWSet* addset);

    void SubtractAndAddPoints(SgPointSet* subtractset, SgBWSet* addset);

    void SubtractAndAddPoints(SgBWSet* subtractset, SgPointSet* addset);

private:
    /** Entry for SgIncrementalStack */
    union IntOrPtr
    {
        IntOrPtr()
        {
        }

        IntOrPtr(int i)
        {
            m_int = i;
        }

        IntOrPtr(void* ptr)
        {
            m_ptr = ptr;
        }

        int m_int;

        void* m_ptr;
    };

    /** Stores incremental state changes for execute/undo moves */
    SgVector<IntOrPtr> m_stack;
};

//----------------------------------------------------------------------------

#endif // SG_INCREMENTALSTACK_H
