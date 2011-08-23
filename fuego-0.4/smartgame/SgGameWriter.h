//----------------------------------------------------------------------------
/** @file SgGameWriter.h */
//----------------------------------------------------------------------------

#ifndef SG_GAMEWRITER_H
#define SG_GAMEWRITER_H

#include <iosfwd>
#include "SgProp.h"

class SgNode;

//----------------------------------------------------------------------------

/** Write file with SGF data */
class SgGameWriter
{
public:
    SgGameWriter(std::ostream& out);

    /** Write the game tree at 'root' to the given archive.
        Add file format and application properties to the root node.
        @param root Root node of the game.
        @param allProps Write all properties.
        @param fileFormat if zero, write it out with the default file format.
        @param application Application name (will add AP property if not
        empty)
        @param gameNumber Game number (will add GM property)
        @param defaultSize The (game-dependent) default board size, if file
        contains no SZ property.
        @throws SgException If saving fails (presently it does only check if
        the file is ok at the beginning, not if all writes succeed)
    */
    void WriteGame(SgNode& root, bool allProps, int fileFormat,
                   const std::string& application, int gameNumber,
                   int defaultSize);

private:
    std::ostream& m_out;

    int m_fileFormat;

    /** Number of properties on this line. */
    int m_numPropsOnLine;

    void ConvertFormat(SgNode& root);

    void HandleProps(const SgNode* node, int& boardSize) const;

    /** Should property be written to file? May depend on options. */
    bool ShouldWriteProperty(const SgProp& prop);

    void StartNewLine();

    void WriteNode(const SgNode& node, bool allProps, int boardSize,
                   SgPropPointFmt fmt);

    void WriteSubtree(const SgNode& node, bool allProps, int boardSize,
                      SgPropPointFmt fmt);

    /** Not implemented. */
    SgGameWriter(const SgGameWriter&);

    /** Not implemented. */
    SgGameWriter& operator=(const SgGameWriter&);
};

//----------------------------------------------------------------------------

#endif // SG_GAMEWRITER_H
