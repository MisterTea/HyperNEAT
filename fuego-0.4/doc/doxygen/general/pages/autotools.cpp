/** @page generalautotools Building Fuego using GNU Autotools

@section generalautotoolsdistro Building from a distribution (released version)

@verbatim
./configure
make
@endverbatim

Interesting options to configure are @c --enable-assert for enabling
assertions or @c --enable-optimize=no for switching off optimizations.
<tt>./configure --help</tt> returns a full list of options.

@section generalautotoolssvn Building a development version checked out from SVN

@verbatim
aclocal
autoheader
autoreconf -i
@endverbatim

The above commands need to be run only initially. Then the compilation works
as in the previous section.

After adding or removing files or doing other changes to
<tt>configure.ac</tt> or a <tt>Makefile.am</tt>, you need to run
<tt>autoreconf</tt> again before doing a make.
A better way is to configure your makefiles with
<tt>./configure --enable-maintainer-mode</tt>. Then a make will automatically
check, if <tt>configure.ac</tt> or a <tt>Makefile.am</tt> have changed and
recreate the makefiles before the compilation if necessary.

@section generalautotoolsinstall Installing Fuego

@verbatim
sudo make install
@endverbatim

@section generalautotoolscheck Running tests

The following command compiles and runs the unit tests (and also runs
other tests):

@verbatim
make check
@endverbatim

@section generalautotoolvpath Building debug and release version

Assuming that you want to build a debug and release version in different
directories. This is called a VPATH-build. In this example, we choose
<tt>fuego/build/autotools/debug</tt> and
<tt>fuego/build/autotools/release</tt> as the build directories:
@verbatim
cd fuego
mkdir -p build/autotools/debug
cd build/autotools/debug
env CXXFLAGS=-g ../../../configure --enable-assert --enable-optimize=no

cd ../../../
mkdir -p build/autotools/release
cd build/autotools/release
../../../configure
@endverbatim
Then the command <tt>make</tt> should be run in
<tt>fuego/build/autotools/debug</tt> or
<tt>fuego/build/autotools/release</tt>.

@section generalautotoollinks Further documentation links

- <a href="http://www.freesoftwaremagazine.com/books/autotools_a_guide_to_autoconf_automake_libtool">Autotools: a practitioner's guide to Autoconf, Automake and Libtool</a>
- <a href="http://sources.redhat.com/autobook/autobook/autobook_toc.html">GNU Autoconf, Automake, and Libtool</a>
  (some of the examples are not yet updated to newer versions of Autotools;
  still a useful reference)
- GNU Autotools manuals:
  <a href="http://www.gnu.org/software/automake/manual/html_node/index.html">Automake</a>,
  <a href="http://www.gnu.org/software/autoconf/manual/html_node/index.html">Autoconf</a>,
  <a href="http://www.gnu.org/software/libtool/manual/html_node/index.html">Libtool</a>
*/

