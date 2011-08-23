/** @page sgnotes Notes for the SmartGame Library

@section sgnotessystem System Header

SgSystem.h includes platform-dependent macros and should be included
as the first in every cpp-file, but not in any header file.

@section sgnotesinit Initialization

SgInit() / SgFini() must be called before / after using any classes to
initialize global variables (however it is deprecated to use global or
class static variables in newly written code).

*/

