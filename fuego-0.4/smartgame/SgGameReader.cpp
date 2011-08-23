//----------------------------------------------------------------------------
/** @file SgGameReader.cpp */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgGameReader.h"

#include <cstdio> // Defines EOF
#include <iostream>
#include <map>
#include <vector>
#include "SgDebug.h"
#include "SgException.h"
#include "SgNode.h"

using namespace std;

//----------------------------------------------------------------------------

namespace {

/** Print warning and reset flag in temporary copy of m_warnings. */
void PrintWarning(ostream& out, SgGameReader::Warnings& warnings, int index,
                  const char* text)
{
    if (! warnings.test(index))
        return;
    out << text << '\n';
    warnings.reset(index);
}

} // namespace

//----------------------------------------------------------------------------

SgGameReader::SgGameReader(istream& in, int defaultSize)
    : m_in(in),
      m_defaultSize(defaultSize),
      m_fileFormat(4)
{
}

bool SgGameReader::GetIntProp(const SgGameReader::RawProperties& properties,
                              const string& label, int& value)
{
    RawProperties::const_iterator it = properties.find(label);
    if (it == properties.end() || it->second.size() == 0)
        return false;
    istringstream in(it->second[0]);
    in >> value;
    return in;
}

/** Create SgProp instances and add them to node.
    The only properties that are interpreted by the reader are SZ (board size)
    and GM (point format, because they must be handled before all other root
    node properties to parse points correctly.
*/
void SgGameReader::HandleProperties(SgNode* node,
                                    const RawProperties& properties,
                                    int& boardSize, SgPropPointFmt& fmt)
{
    int value;
    if (GetIntProp(properties, "SZ", value))
    {
       if (value < SG_MIN_SIZE || value > SG_MAX_SIZE)
           m_warnings.set(INVALID_BOARDSIZE);
       else
           boardSize = value;
    }
    if (GetIntProp(properties, "GM", value))
        fmt = SgPropUtil::GetPointFmt(value);
    for (RawProperties::const_iterator it = properties.begin();
         it != properties.end(); ++it)
    {
        const string& label = it->first;
        const vector<string>& values = it->second;
        if (values.size() == 0)
            m_warnings.set(PROPERTY_WITHOUT_VALUE);
        SgProp* prop;
        SgPropID id = SgProp::GetIDOfLabel(label);
        if (id != SG_PROP_NONE)
            prop = SgProp::CreateProperty(id);
        else
            prop =
                new SgPropUnknown(SG_PROP_UNKNOWN, label, vector<string>());
        if (prop->FromString(values, boardSize, fmt))
            node->Add(prop);
    }
}

void SgGameReader::PrintWarnings(ostream& out) const
{
    Warnings warnings = m_warnings;
    // Print more severe warnings first, less severe warnings later
    PrintWarning(out, warnings, INVALID_BOARDSIZE, "Invalid board size");
    PrintWarning(out, warnings, PROPERTY_WITHOUT_VALUE,
                 "Property withour value");
    SG_ASSERT(warnings.none());
}

SgNode* SgGameReader::ReadGame(bool resetWarnings)
{
    if (resetWarnings)
        m_warnings.reset();
    SgNode* root = 0;
    int c;
    while ((c = m_in.get()) != EOF)
    {
        while (c != '(' && c != EOF)
            c = m_in.get();
        if (c == EOF)
            break;
        root = ReadSubtree(0, m_defaultSize, SG_PROPPOINTFMT_GO);
        if (root)
            root = root->Root();
        if (root)
            break;
    }
    return root;
}

void SgGameReader::ReadGames(SgVectorOf<SgNode>* rootList)
{
    m_warnings.reset();
    SG_ASSERT(rootList);
    rootList->Clear();
    while (true)
    {
        SgNode* root = ReadGame(false);
        if (root)
            rootList->PushBack(root);
        else
            break;
    }
}

string SgGameReader::ReadLabel(int c)
{
    // Precondition: Character 'c' is in range 'A'..'Z', to be interpreted
    // as the first letter of a property label. Second letter can be capital
    // letter or digit, lower case letters are ignored.
    string label;
    label += static_cast<char>(c);
    while ((c = m_in.get()) != EOF
           && (('A' <= c && c <= 'Z')
               || ('a' <= c && c <= 'z')
               || ('0' <= c && c <= '9')))
        label += static_cast<char>(c);
    if (c != EOF)
        m_in.unget();
    return label;
}

SgNode* SgGameReader::ReadSubtree(SgNode* node, int boardSize,
                                  SgPropPointFmt fmt)
{
    RawProperties properties;
    int c;
    while ((c = m_in.get()) != EOF && c != ')')
    {
        if ('A' <= c && c <= 'Z')
        {
            string label = ReadLabel(c);
            m_in >> ws;
            string value;
            while (ReadValue(value))
                properties[label].push_back(value);
        }
        else if (c == ';')
        {
            if (node)
            {
                HandleProperties(node, properties, boardSize, fmt);
                properties.clear();
                node = node->NewRightMostSon();
            }
            else
                node = new SgNode(); // first node
        }
        else if (c == '(')
        {
            HandleProperties(node, properties, boardSize, fmt);
            properties.clear();
            ReadSubtree(node, boardSize, fmt);
        }
    }
    HandleProperties(node, properties, boardSize, fmt);
    return node;
}

bool SgGameReader::ReadValue(string& value)
{
    m_in >> ws;
    value = "";
    int c;
    if ((c = m_in.get()) == EOF)
        return false;
    if (c != '[')
    {
        m_in.unget();
        return false;
    }
    bool inEscape = false;
    while ((c = m_in.get()) != EOF && (c != ']' || inEscape))
    {
        if (c != '\n')
            value += static_cast<char>(c);
        if (inEscape)
            inEscape = false;
        else if (c == '\\')
            inEscape = true;
    }
    return true;
}

//----------------------------------------------------------------------------
