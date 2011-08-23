/** @page generalportability Portability

Fuego is largely written using standard C++.

The only external library it depends on is
<a href="http://www.boost.org/">Boost</a>. Boost is available on a variety
of platforms. Currently, Fuego uses only features existing in Boost version
1.33.1, but should also compile with newer versions (although the included
autoconf scripts currently support only versions up to 1.36).

The implementation of functionality that is not available in the standard
C++ libraries or Boost is encapsulated in classes in the SmartGame library
(e.g. SgTime, SgProcess). The current implementation using POSIX functions.
To port these classes to other platforms, an alternative implementation needs
to be added to the source code.

*/
