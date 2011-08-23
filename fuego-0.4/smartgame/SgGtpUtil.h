//----------------------------------------------------------------------------
/** @file SgGtpUtil.h
    Utility functions
*/
//----------------------------------------------------------------------------

#ifndef SG_GTPUTIL_H
#define SG_GTPUTIL_H

class GtpCommand;
class SgPointSet;

//----------------------------------------------------------------------------

namespace SgGtpUtil
{
    /** Append a point set to the response of a command.
        The response will be formatted as a single line with the points
        ordered (A1, B1, ..., A2, ...) to facilitate writing GTP regression
        tests using this command.
    */
    void RespondPointSet(GtpCommand& cmd, const SgPointSet& pointSet);

} // namespace SgGtpUtil

//----------------------------------------------------------------------------

#endif // SG_GTPUTIL_H
