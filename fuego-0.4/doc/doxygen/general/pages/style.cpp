/** @page generalstyle Coding Style

    @section generalstyleformatting Formatting

    - Indentation level is 4 spaces
    - Don't use tabs
    - Maximum line length is 78 characters
    - No multiple statements per line if possible
    - One Space around operators
    - No spaces around parenthesis
    - One Space between keyword and open parenthesis
    - Block braces are on their own line, same indentation level as the line
      above

    @section generalstylenaming Naming Conventions

    - Use meaningful names
    - Underscore is only used for member prefix and in all-uppercase names
    - Macros and constants are all-uppercase
    - Types (classes, structs, typedefs) and functions are capitalized;
      no all-uppercase abbreviations (e.g. GtpEngine, not GTPEngine)
    - Variables begin with a lower case, member variables use the prefix m_,
      static variables s_, global variables g_
    - Names that are in the global namespace use a short prefix unique for
      each module

    @section generalstylecomments Comments

    - Functions, classes, etc. are documented using Javadoc-style
      <a href="http://www.doxygen.org">Doxygen</a> syntax.
    - The documentation is a single multi-line comment block, starting with
      <tt>/**</tt>, followed by the (auto-)brief description in the first
      line, followed by the detailed description.
    - Public functions are documented at the declaration in the header file,
      private functions at the definition in the cpp-file (exception: GTP
      commend callbacks are documented at the definition in the cpp-file
      to increase the probability that the documentation kept up-to-date
      if the GTP command arguments change)

    @section generalstylefiles Files

    - No <tt>using namespace::std</tt> in header files
    - Don't include SgSystem.h in header files
    - All cpp-files include SgSystem.h first (to ensure that macros are
      defined consistently everywhere), then the header file with the same
      name (to ensure self-consistency of the header file), then system
      includes, then other includes, alphabetically sorted (in order to not
      pretend that the header files are not self-consistent and depend on
      an inclusion order)
    - Classes should be in their own file, apart from helper and friend
      classes

    @section generalstyleother Other
    - Don't write long classes or long and deeply nested functions
    - Don't copy and paste code; avoid redundancy
    - Don't create cyclic dependencies between classes, files or modules
    - Don't use inheritance to tweak the implementation of classes
    - <b>In general: Use best C++ practices</b>
*/

