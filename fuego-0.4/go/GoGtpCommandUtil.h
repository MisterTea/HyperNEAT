//----------------------------------------------------------------------------
/** @file GoGtpCommandUtil.h
    Utils for Go GTP commands
*/
//----------------------------------------------------------------------------

#ifndef GO_GTPCOMMANDUTIL_H
#define GO_GTPCOMMANDUTIL_H

#include <cstddef>
#include "SgBlackWhite.h"
#include "SgBoardColor.h"
#include "SgPoint.h"
#include "SgVector.h"

class GtpCommand;
class GoBoard;
template<typename T> class SgPointArray;

//----------------------------------------------------------------------------

namespace GoGtpCommandUtil
{

    SgEmptyBlackWhite EmptyBlackWhiteArg(const GtpCommand& cmd,
                                         std::size_t number);

    SgBlackWhite BlackWhiteArg(const GtpCommand& cmd, std::size_t number);

    SgPoint EmptyPointArg(const GtpCommand& cmd, std::size_t number,
                          const GoBoard& board);

    /** Return the handicap stone locations as defined by the GTP standard.
        See GTP version 2 specification, section 4.1.1 Fixed Handicap
        Placement.
        @param size Size of the board.
        @param n Number of handicap stones (0, 2, 3, 4, 5, 6, 7, 8, 9).
        @returns Location of the handicap stones.
        @throws GtpFailure If handicap locations are not defined for this
        number and board size.
    */
    SgVector<SgPoint> GetHandicapStones(int size, int n);

    SgMove MoveArg(const GtpCommand& cmd, std::size_t number,
                   const GoBoard& board);

    /** Parse argument list consisting of toPlay followed by list of stones.
        All stones are of the same 'defender' color.
     */
    void ParseMultiStoneArgument(GtpCommand& cmd,
                                 const GoBoard& board,
                                 SgBlackWhite& toPlay,
                                 SgBlackWhite& defender,
                                 SgVector<SgPoint>& crucial);
    
    SgPoint PointArg(const GtpCommand& cmd, const GoBoard& board);
    
    SgPoint PointArg(const GtpCommand& cmd, std::size_t number,
                     const GoBoard& board);

    /** Return point list argument starting a given argument number to the
        end of arguments.
    */
    SgVector<SgPoint> PointListArg(const GtpCommand& cmd, std::size_t number,
                                 const GoBoard& board);

    /** Return point list argument using all arguments. */
    SgVector<SgPoint> PointListArg(const GtpCommand& cmd, 
                                   const GoBoard& board);

    void RespondNumberArray(GtpCommand& cmd, const SgPointArray<int>& array,
                            int scale, const GoBoard& board);

    /** Sort response to gogui-analyze_commands alphabetically by label.
        Useful if the response to gogui-analyze_commands was concatenated
        from different components and should be displayed alphabetically.
        @param response Old response in the format expected by
        gogui-analyze_commands
        @return New response, lines sorted by analyze label
    */
    std::string SortResponseAnalyzeCommands(const std::string& response);

    SgPoint StoneArg(const GtpCommand& cmd, std::size_t number,
                     const GoBoard& board);
}

inline SgVector<SgPoint> GoGtpCommandUtil::PointListArg(const GtpCommand& cmd,
                                                      const GoBoard& board)
{
    return PointListArg(cmd, 0, board);
}

//----------------------------------------------------------------------------

#endif // GO_GTPCOMMANDUTIL_H

