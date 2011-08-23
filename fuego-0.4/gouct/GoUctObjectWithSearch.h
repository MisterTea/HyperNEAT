//----------------------------------------------------------------------------
/** @file GoUctObjectWithSearch.h */
//----------------------------------------------------------------------------

#ifndef GOUCT_OBJECTWITHSEARCH_H
#define GOUCT_OBJECTWITHSEARCH_H

class GoUctSearch;

//----------------------------------------------------------------------------

/** Interface implemented by object that owns a (single) GoUctSearch.
    If a GoPlayer implements this interface, the GoUctSearch related GTP
    commands in GoUctCommands can be used with this player.
*/
class GoUctObjectWithSearch
{
public:
    virtual ~GoUctObjectWithSearch();

    virtual GoUctSearch& Search() = 0;

    virtual const GoUctSearch& Search() const = 0;
};

//----------------------------------------------------------------------------

#endif // GOUCT_OBJECTWITHSEARCH_H
