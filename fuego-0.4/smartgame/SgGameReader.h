//----------------------------------------------------------------------------
/** @file SgGameReader.h */
//----------------------------------------------------------------------------

#ifndef SG_GAMEREADER_H
#define SG_GAMEREADER_H

#include <bitset>
#include <map>
#include <vector>
#include "SgProp.h"
#include "SgVector.h"

class SgNode;

//----------------------------------------------------------------------------

/** Read file with SGF data.
    @bug Properties are read sequentially, therefore GM and SZ properties have
    to be before any point value properties, because they are required to
    parse the point values. According to the SGF standard, the order or
    properties is arbitrary and there exist SGF files, which have the GM and
    SZ property after point properties (like AB, AW) in the root node. To fix
    this, all properties of a node would have to be read and GM and SZ handled
    first before parsing any other property values.
*/
class SgGameReader
{
public:
    /** Warning conditions that occurred during reading.
        @todo Some of these warnings should be errors.
    */
    enum WarningFlags {
        /** Invalid board size.
            The file contained a size property with a value not in
            [SG_MIN_SIZE..SG_MAX_SIZE].
        */
        INVALID_BOARDSIZE,

        PROPERTY_WITHOUT_VALUE,

        /** Number of WarningFlags. */
        NU_WARNING_FLAGS
    };

    /** Warnings that occurred during reading. */
    typedef std::bitset<NU_WARNING_FLAGS> Warnings;

    /** Create reader from an input stream.
        @param in The input stream.
        @param defaultSize The (game-dependent) default board size, if file
        contains no SZ property.
        no SZ property.
    */
    SgGameReader(std::istream& in, int defaultSize = 19);

    /** Get warnings of last ReadGame or ReadGames. */
    Warnings GetWarnings() const;

    /** Print warnings of last ReadGame or ReadGames to stream.
        Prints the warnings as human readable text.
    */
    void PrintWarnings(std::ostream& out) const;

    /** Read next game tree from file.
        @return Root node or 0 if there is no next game.
    */
    SgNode* ReadGame();

    /** Read all game trees from this file.
        Return a list with the root of each game tree.
    */
    void ReadGames(SgVectorOf<SgNode>* rootList);

private:
    /** Map label to values (unparsed) */
    typedef std::map<std::string, std::vector<std::string> > RawProperties;

    std::istream& m_in;

    const int m_defaultSize;

    /** The file format read in. */
    int m_fileFormat;

    Warnings m_warnings;

    /** Not implemented. */
    SgGameReader(const SgGameReader&);

    /** Not implemented. */
    SgGameReader& operator=(const SgGameReader&);

    static bool GetIntProp(const RawProperties& properties,
                           const std::string& label, int& value);

    void HandleProperties(SgNode* node, const RawProperties& properties,
                          int& boardSize, SgPropPointFmt& fmt);

    SgNode* ReadGame(bool resetWarnings);

    std::string ReadLabel(int c);

    SgNode* ReadSubtree(SgNode* node, int boardSize, SgPropPointFmt fmt);

    bool ReadValue(std::string& value);
};

inline SgGameReader::Warnings SgGameReader::GetWarnings() const
{
    return m_warnings;
}

inline SgNode* SgGameReader::ReadGame()
{
    return ReadGame(true);
}

//----------------------------------------------------------------------------

#endif // SG_GAMEREADER_H
