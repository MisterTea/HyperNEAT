//----------------------------------------------------------------------------
/** @file SgInit.h
    Initialization of the SmartGo module.
*/
//----------------------------------------------------------------------------

#ifndef SG_INIT_H
#define SG_INIT_H

//----------------------------------------------------------------------------

/** Used in SgInit(); public only as a side effect of the implementation. */
void SgInitImpl(bool compiledInDebugMode);

/** Call all lower-level Fini functions.
    This function must be called after using the SmartGo module.
    Also calls SgMemCheck.
    @note Will become obsolete in the future
    @see SgInit
*/
void SgFini();

/** Call all lower-level initialization functions.
    This function must be called before using the SmartGo module.
    Returns false if one of them returns false.
    @note Don't add any more global variables that need explicit
    initialization; this function will become unnecessary in the future.
    Currently still needed for:
    - Property
    @throws SgException on error
*/
inline void SgInit()
{
    // This function must be inline, it needs to use the setting of NDEBUG
    // of the user code including this header
#ifndef NDEBUG
    SgInitImpl(true);
#else
    SgInitImpl(false);
#endif
}

/** Check that SgInit was called.
    @throws SgException if not
*/
void SgInitCheck();

//----------------------------------------------------------------------------

#endif // SG_INIT_H

