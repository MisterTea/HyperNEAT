//----------------------------------------------------------------------------
/** @file SgGtpClient.cpp
    See SgGtpClient.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgGtpClient.h"

#include <sstream>
#include "SgDebug.h"

using namespace std;

//----------------------------------------------------------------------------

SgGtpFailure::SgGtpFailure(const std::string& message)
    : SgException(message)
{
}

//----------------------------------------------------------------------------

SgGtpClient::SgGtpClient(istream& in, ostream& out, bool verbose)
    : m_verbose(verbose),
      m_in(in),
      m_out(out)
{
}

SgGtpClient::~SgGtpClient()
{
}

string SgGtpClient::Send(const string& command)
{
    m_out << command << '\n';
    m_out.flush();
    if (m_verbose)
        SgDebug() << "<< " << command << '\n';
    if (! m_out)
        throw SgGtpFailure("GTP write connection is broken");
    ostringstream response;
    bool done = false;
    bool isFirst = true;
    bool success = true;
    while (! done)
    {
        string line;
        getline(m_in, line);
        if (! m_in)
            throw SgGtpFailure("GTP read connection is broken");
        if (m_verbose)
            SgDebug() << ">> " << line << '\n';
        if (isFirst)
        {
            if (line.size() < 2 || (line[0] != '=' && line[0] != '?')
                || line[1] != ' ')
                throw SgGtpFailure("Invalid response: '" + line + "'");
            if (line[0] == '?')
                success = false;
            line = line.substr(2);
            response << line;
            isFirst = false;
        }
        else
        {
            if (line.empty())
                done = true;
            else
                response << '\n' << line;
        }
    }
    if (! success)
        throw SgGtpFailure(response.str());
    return response.str();
}

//----------------------------------------------------------------------------
