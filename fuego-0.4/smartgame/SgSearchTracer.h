//----------------------------------------------------------------------------
/** @file SgSearchTracer.h
    Trace search for SgSearch.

    SgSearchTracer stores the whole search in a game tree using SgNode's.
*/
//----------------------------------------------------------------------------

#ifndef SG_SEARCHTRACER_H
#define SG_SEARCHTRACER_H

#include "SgBlackWhite.h"
#include "SgMove.h"
#include "SgNode.h"

//----------------------------------------------------------------------------

/** Traces a search. */
class SgSearchTracer
{
public:
    SgSearchTracer(SgNode* root);
    
    virtual ~SgSearchTracer();

    /** Adds the given move as a new node to the trace tree and goes
        to that node. Doesn't do anything if m_traceNode is 0. */
    void AddTraceNode(SgMove move, SgBlackWhite player);

    /** Current node */
    SgNode* TraceNode() const;

    /** Add comment to current tracenode */
    void TraceComment(const char* comment) const;
    
    void StartOfDepth(int depth);

     /** Adds move property to node (game-dependent). 
         The default implementation stores the move in a SgMoveProp.
         Override this method for other games.
     */
    virtual void AddMoveProp(SgNode* node, SgMove move,
                             SgBlackWhite player);
    
    /** Add value as a comment to current tracenode */
    void TraceValue(int value, SgBlackWhite toPlay) const;

    /** Add value and text as a comment to current tracenode */
    void TraceValue(int value, SgBlackWhite toPlay,
                    const char* comment, bool isExact) const;

    /** Go one move up in the trace tree.
        Don't do anything if m_traceNode is null.
        To be called from the client's TakeBack method.
    */
    void TakeBackTraceNode();

    /** Is tracing on? Default implementation always returns true. */
    virtual bool TraceIsOn() const;

    /** Creates a new root node for tracing. Override to add information */
    virtual void InitTracing(const std::string& type);

    /** Move trace tree to a subtree of toNode, and set m_traceNode = 0 */
    void AppendTrace(SgNode* toNode);

protected:
    /** Current node in tracing. */
    SgNode* m_traceNode;

private:
    /** Not implemented */
    SgSearchTracer(const SgSearchTracer&);

    /** Not implemented */
    SgSearchTracer& operator=(const SgSearchTracer&);
};

inline SgNode* SgSearchTracer::TraceNode() const
{
    return m_traceNode;
}

inline bool SgSearchTracer::TraceIsOn() const
{
    return true;
}

//----------------------------------------------------------------------------

#endif // SG_SEARCH_H
