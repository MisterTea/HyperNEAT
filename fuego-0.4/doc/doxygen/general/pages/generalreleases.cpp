/** @page generalreleases How to make a release

This is a short HOWTO for maintainers how to do a Fuego release.

-# Run <tt>make distcheck</tt> to check that the compilation, the unit tests,
   and a VPATH build work
-# Change the version in the second argument of the AM_INIT_AUTOMAKE macro in
   configure.ac and update the current section header in NEWS
-# Commit the changes
-# Tag the current version.
   Example 1: for release 0.3.1 in the bugfix branch of version 0.3, the
   command would be:
@verbatim
svn copy  \
https://fuego.svn.sourceforge.net/svnroot/fuego/branches/VERSION_0_3_FIXES  \
https://fuego.svn.sourceforge.net/svnroot/fuego/tags/VERSION_0_3_1 \
-m "Tag release 0.3.1"
@verbatim
@endverbatim
   Example 2: for release 0.3 (a new major release), the command would be:
@verbatim
svn copy  \
https://fuego.svn.sourceforge.net/svnroot/fuego/trunk  \
https://fuego.svn.sourceforge.net/svnroot/fuego/tags/VERSION_0_3 \
-m "Tag release 0.3"
@endverbatim
-# Run <tt>autoreconf -i</tt> in the root directory of Fuego and then
   <tt>make dist</tt>. The file release is now in the current directory
-# If the release is a new major release, create a bugfix branch.
   Example: for release 0.3, the command would be:
@verbatim
svn copy  \
https://fuego.svn.sourceforge.net/svnroot/fuego/trunk  \
https://fuego.svn.sourceforge.net/svnroot/fuego/branches/VERSION_0_3_FIXES \
-m "Create bugfix branch for release 0.3"
@endverbatim
-# Change the version in configure.ac to a development version and add a new
   section header in NEWS ("current development version").
   The development version string is by convention the version of the last
   release with ".SVN" appended.
   After a new major release, you need to change the version string in both the
   trunk and the new bugfix branch.
   To distinguish between trunk and bugfix branch, the first development
   version in the bugfix branch has a ".0.SVN" appended.
-# Commit the changes.
-# Upload the file release to SourceForge

*/
