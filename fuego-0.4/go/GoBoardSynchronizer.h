//----------------------------------------------------------------------------
/** @file GoBoardSynchronizer.h */
//----------------------------------------------------------------------------

#ifndef GO_BOARDSYNCHRONIZER_H
#define GO_BOARDSYNCHRONIZER_H

#include "GoPlayerMove.h"

class GoBoard;

//----------------------------------------------------------------------------

/** Synchronize two boards. */
class GoBoardSynchronizer
{
public:
    /** Create a synchronizer.
        The publisher board can be at any position; the subscriber match that
        position the first time BoardSynchronizer::UpdateSubscriber is called.
    */
    GoBoardSynchronizer(const GoBoard& publisher);

    virtual ~GoBoardSynchronizer();

    /** Set the board that will subscribe to all the changes of the publisher.
        Can only be called once.
    */
    void SetSubscriber(GoBoard& subscriber);

    /** Update the subscriber board.
        Calls Init, Play, Undo and/or SetToPlay to update the
        subscriber to the current state of the publisher.
        If no subscriber was set with SetSubscriber, this function does
        nothing.
    */
    void UpdateSubscriber();

protected:
    /** @name Hook functions for incremental update events */
    // @{

    /** Board was initialized with new size.
        Default implementation does nothing.
    */
    virtual void OnBoardChange();

    /** Move about to be executed.
        Default implementation does nothing.
    */
    virtual void PrePlay(GoPlayerMove move);

    /** Move was executed.
        Default implementation does nothing.
    */
    virtual void OnPlay(GoPlayerMove move);

    /** Move about to be undone.
        Default implementation does nothing.
    */
    virtual void PreUndo();

    /** Move was undone.
        Default implementation does nothing.
    */
    virtual void OnUndo();

    // @}

private:
    const GoBoard& m_publisher;

    GoBoard* m_subscriber;

    /** Not implemented */
    GoBoardSynchronizer(const GoBoardSynchronizer&);

    /** Not implemented */
    GoBoardSynchronizer& operator=(const GoBoardSynchronizer&);

    void ExecuteSubscriber(const GoPlayerMove& move);

    int FindNuCommon() const;

    void UpdateFromInit();

    void UpdateIncremental();

    void UpdateToPlay();
};

//----------------------------------------------------------------------------

#endif // GO_BOARDSYNCHRONIZER_H

