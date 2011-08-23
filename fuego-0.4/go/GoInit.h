//----------------------------------------------------------------------------
/** @file GoInit.h
    Initialization of module Go.
*/
//----------------------------------------------------------------------------

#ifndef GO_INIT_H
#define GO_INIT_H

//----------------------------------------------------------------------------

void GoFini();

/** Initialization of module Go.
    Must be called after SgInit.
    @throws SgException on failure.
*/
void GoInit();

void GoInitCheck();

//----------------------------------------------------------------------------

#endif // GO_INIT_H

