//----------------------------------------------------------------------------
/** @file SgDebug.h
    Logging stream.
 */
//----------------------------------------------------------------------------

#ifndef SG_DEBUG_H
#define SG_DEBUG_H

#include <iosfwd>
#include <sstream>

//----------------------------------------------------------------------------

/** Current logging stream. */
std::ostream& SgDebug();

/** Write warning prefix to logging stream and return it.
    Writes "WARNING: " to the logging stream and returns it for outputting
    the rest of the warning line. Lines beginning with this prefix are
    displayed in a different color than regular stderr output in GoGui.
*/
std::ostream& SgWarning();

//----------------------------------------------------------------------------

/** Set logging stream to file. */
void SgDebugToFile(const char* filename);

/** Set logging stream to null stream.
    Discards everything written to SgDebug().
*/
void SgDebugToNull();

/** Set logging stream to console window.
    @todo: Bad function name, uses std::cerr on Unix and std::cout on MAC
*/
void SgDebugToWindow();

std::ostream* SgSwapDebugStr(std::ostream* newStr);

//----------------------------------------------------------------------------

/** Temporarily redirect IO to file for lifetime of this object */
class SgDebugToNewFile
{
public:
    explicit SgDebugToNewFile(const char* filename);

    explicit SgDebugToNewFile();

    void SetFile(const char* filename);

    ~SgDebugToNewFile();

private:
    std::ostream* m_old;
};

//----------------------------------------------------------------------------

/** Temporarily redirect IO to a string buffer for lifetime of this object */
class SgDebugToString
{
public:
    /** Constructor.
        @param writeToOldDebugStr Also write the content of the string to the
        old stream, after it was reset in the desctructor.
    */
    SgDebugToString(bool writeToOldDebugStr);

    ~SgDebugToString();

    std::string GetString() const { return m_str.str(); }

private:
    bool m_writeToOldDebugStr;

    std::ostringstream m_str;

    std::ostream* m_old;
};

//----------------------------------------------------------------------------

#endif // SG_DEBUG_H
