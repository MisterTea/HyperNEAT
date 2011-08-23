//----------------------------------------------------------------------------
/** @file SpSimplePlayer.h
    Base class for simple Go playing algorithms.
*/
//----------------------------------------------------------------------------

#ifndef SP_SIMPLEPLAYER_H
#define SP_SIMPLEPLAYER_H

#include "GoBoard.h"
#include "GoPlayer.h"

class SgTimeRecord;
class SpMoveGenerator;
class SpRandomMoveGenerator;

//----------------------------------------------------------------------------

/** SimplePlayer has one move generator. 
    It generates random moves if no other move is found.
*/
class SpSimplePlayer
    : public GoPlayer
{
public:
    virtual ~SpSimplePlayer();

    SgPoint GenMove(const SgTimeRecord& time, SgBlackWhite toPlay);

    virtual int MoveValue(SgPoint p);

protected:
    SpSimplePlayer(GoBoard& board, SpMoveGenerator* generator);
            
private:
    /** Move generator */
    SpMoveGenerator* m_generator;

    /** Use random generator if no other move found */
    SpRandomMoveGenerator* m_randomGenerator;

    /** Don't play on safe points */
    virtual bool UseFilter() const 
    {
        return true;
    }    
};

//----------------------------------------------------------------------------

#endif // SP_SIMPLEPLAYER_H

