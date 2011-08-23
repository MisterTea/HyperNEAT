//----------------------------------------------------------------------------
/** @file SgGameWriter.cpp
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgGameWriter.h"

#include <iostream>
#include "SgDebug.h"
#include "SgException.h"
#include "SgNode.h"

using namespace std;
using SgPropUtil::GetPointFmt;

//----------------------------------------------------------------------------

SgGameWriter::SgGameWriter(ostream& out)
    : m_out(out),
      m_fileFormat(4),
      m_numPropsOnLine(0)
{
}

void SgGameWriter::WriteGame(SgNode& root, bool allProps, int fileFormat,
                             const string& application, int gameNumber,
                             int defaultSize)
{
    if (! m_out)
        throw SgException("SgGameWriter: write error");
    // Add file format property to root. If file format not specified,
    // default to file format read in, or FF[4] for new files.
    if (fileFormat != 0)
        m_fileFormat = fileFormat;
    else if (root.HasProp(SG_PROP_FORMAT))
        m_fileFormat = root.GetIntProp(SG_PROP_FORMAT);
    root.Add(new SgPropInt(SG_PROP_FORMAT, m_fileFormat));
    if (application != "")
        root.Add(new SgPropText(SG_PROP_APPLIC, application));
    root.Add(new SgPropInt(SG_PROP_GAME, gameNumber));
    // Reorder the main root properties in a fixed order to make games more
    // human-readable and to avoid unnecessary diffs. Note that they'll end
    // up in the opposite sequence.
    SgPropList& props = root.Props();
    props.MoveToFront(SG_PROP_PLAYER);
    props.MoveToFront(SG_PROP_ADD_EMPTY);
    props.MoveToFront(SG_PROP_ADD_WHITE);
    props.MoveToFront(SG_PROP_ADD_BLACK);
    props.MoveToFront(SG_PROP_USER);
    props.MoveToFront(SG_PROP_SOURCE);
    props.MoveToFront(SG_PROP_ROUND);
    props.MoveToFront(SG_PROP_EVENT);
    props.MoveToFront(SG_PROP_PLACE);
    props.MoveToFront(SG_PROP_DATE);
    props.MoveToFront(SG_PROP_RESULT);
    props.MoveToFront(SG_PROP_PLAYER_BLACK);
    props.MoveToFront(SG_PROP_RANK_BLACK);
    props.MoveToFront(SG_PROP_TEAM_BLACK);
    props.MoveToFront(SG_PROP_PLAYER_WHITE);
    props.MoveToFront(SG_PROP_RANK_WHITE);
    props.MoveToFront(SG_PROP_TEAM_WHITE);
    props.MoveToFront(SG_PROP_GAME_NAME);
    props.MoveToFront(SG_PROP_APPLIC);
    props.MoveToFront(SG_PROP_SIZE);
    props.MoveToFront(SG_PROP_FORMAT);
    props.MoveToFront(SG_PROP_GAME);

    SgPropPointFmt fmt = GetPointFmt(gameNumber);
    WriteSubtree(root, allProps, defaultSize, fmt);
    m_out.put('\n');
}

void SgGameWriter::WriteSubtree(const SgNode& nodeRef, bool allProps,
                                int boardSize, SgPropPointFmt fmt)
{
    // Start new sequence on a new line.
    StartNewLine();

    // Opening parenthesis.
    m_out.put('(');

    // Write out main sequence first.
    const SgNode* node = &nodeRef;
    do
    {
        HandleProps(node, boardSize);
        m_out.put(';');
        WriteNode(*node, allProps, boardSize, fmt);
        node = node->LeftMostSon();
    } while (node && ! node->HasRightBrother());

    // Now either reached end of branch, or fork in linear sequence. Write out
    // each subtree recursively.
    while (node)
    {
        HandleProps(node, boardSize);
        WriteSubtree(*node, allProps, boardSize, fmt);
        node = node->RightBrother();
    }

    // Closing parenthesis.
    m_out.put(')');
}

void SgGameWriter::HandleProps(const SgNode* node, int& boardSize) const
{
    int value;
    bool hasSizeProp = node->GetIntProp(SG_PROP_SIZE, &value);
    if (hasSizeProp)
    {
        if (value >= SG_MIN_SIZE && value <= SG_MAX_SIZE)
            boardSize = value;
        else
            SgWarning() << "Invalid size " << value;
    }
}

void SgGameWriter::WriteNode(const SgNode& node, bool allProps, int boardSize,
                             SgPropPointFmt fmt)
{
    for (SgPropListIterator it(node.Props()); it; ++it)
    {
        SgProp* prop = *it;
        vector<string> values;
        // Check whether property should be written, and get its value.
        if ((allProps || ShouldWriteProperty(*prop))
            && prop->ToString(values, boardSize, fmt, m_fileFormat))
        {
            // Start specific properties on a new line to make file easier
            // to read.
            if (prop->Flag(SG_PROPCLASS_NEWLINE))
                StartNewLine();
            m_out << prop->Label();
            for (vector<string>::const_iterator it2 = values.begin();
                 it2 != values.end(); ++it2)
                m_out << '[' << (*it2) << ']';
            // Limit number of properties per line.
            if (++m_numPropsOnLine >= 10)
                StartNewLine();
    }   }

    // Start first move node after root node on a new line.
    if (node.HasProp(SG_PROP_GAME))
        StartNewLine();
}

void SgGameWriter::StartNewLine()
{
    // Start a new line unless we're already at the beginning of a line.
    if (m_numPropsOnLine > 0)
    {
        m_numPropsOnLine = 0;
        m_out.put('\n');
    }
}

bool SgGameWriter::ShouldWriteProperty(const SgProp& prop)
{
    // Only write out clean properties with FF[3].
    // FUTURE: Could make standard adherence a separate option.
    if (m_fileFormat == 3 && prop.Flag(SG_PROPCLASS_NOTCLEAN))
        return false;

    if (prop.Label() == "")
        return false;

    // don't write out time left properties (e.g. problem collections).
    if (prop.ID() == SG_PROP_TIME_BLACK || prop.ID() == SG_PROP_TIME_WHITE)
        return false;

    // Default: write out all properties.
    return true;
}

//----------------------------------------------------------------------------

