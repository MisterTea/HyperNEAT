//----------------------------------------------------------------------------
/** @file SgPoint.h
    Definitions of points on the board.

    Points are defined in a way that they can be used as indices in an array,
    for a one-dimensional representation of a two-dimensional board with
    border (off-board) points between rows and columns.
    They are also compatible with SgMove (i.e. they use no negative integers,
    there is no overlap with the negative values for special moves
    in SgMove), so they can be used as move integers for games where a move
    can be described by a single point on the board.
    @see sgboardrepresentation
*/
//----------------------------------------------------------------------------
/** @page sgboardrepresentation Board Representation

    The board is represented as a one-dimensional array.
    Neighbors of a point can be computed with offsets <tt>WE</tt> and
    <tt>NS</tt>, so that the four neighbors of point <tt>p</tt> are:
    <pre>
               p + SG_NS
    p - SG_WE      p      p + SG_WE
               p - SG_NS
    </pre>
    The board is surrounded by one line of border points (if size is
    SG_MAX_SIZE) in all directions; also diagonally out from the corners.

    @section sgcoordinates Coordinates

    SgPointUtil::Pt, SgPointUtil::Row, and SgPointUtil::Col can be used to
    convert between points and coordinates. The coordinates start with 1 and
    the conversion is independent of the board size. <tt>Pt(1, 1)</tt>, which
    corresponds to A1, is the lower left corner of the board.

    @section sgpointnumbers Point numbers

    The following point numbers are valid, if the code was compiled with
    <code>SG_MAX_SIZE = 19</code> (which is used in the default version of
    SgPoint.h).

<pre>
19|381 382 383 384 385 386 387 388 389 390 391 392 393 394 395 396 397 398 399
18|361 362 363 364 365 366 367 368 369 370 371 372 373 374 375 376 377 378 379
17|341 342 343 344 345 346 347 348 349 350 351 352 353 354 355 356 357 358 359
16|321 322 323 324 325 326 327 328 329 330 331 332 333 334 335 336 337 338 339
15|301 302 303 304 305 306 307 308 309 310 311 312 313 314 315 316 317 318 319
14|281 282 283 284 285 286 287 288 289 290 291 292 293 294 295 296 297 298 299
13|261 262 263 264 265 266 267 268 269 270 271 272 273 274 275 276 277 278 279
12|241 242 243 244 245 246 247 248 249 250 251 252 253 254 255 256 257 258 259
11|221 222 223 224 225 226 227 228 229 230 231 232 233 234 235 236 237 238 239
10|201 202 203 204 205 206 207 208 209 210 211 212 213 214 215 216 217 218 219
 9|181 182 183 184 185 186 187 188 189 190 191 192 193 194 195 196 197 198 199
 8|161 162 163 164 165 166 167 168 169 170 171 172 173 174 175 176 177 178 179
 7|141 142 143 144 145 146 147 148 149 150 151 152 153 154 155 156 157 158 159
 6|121 122 123 124 125 126 127 128 129 130 131 132 133 134 135 136 137 138 139
 5|101 102 103 104 105 106 107 108 109 110 111 112 113 114 115 116 117 118 119
 4| 81  82  83  84  85  86  87  88  89  90  91  92  93  94  95  96  97  98  99
 3| 61  62  63  64  65  66  67  68  69  70  71  72  73  74  75  76  77  78  79
 2| 41  42  43  44  45  46  47  48  49  50  51  52  53  54  55  56  57  58  59
 1| 21  22  23  24  25  26  27  28  29  30  31  32  33  34  35  36  37  38  39
   [A] [B] [C] [D] [E] [F] [G] [H] [J] [K] [L] [M] [N] [O] [P] [Q] [R] [S] [T]
</pre>
*/
//----------------------------------------------------------------------------

#ifndef SG_POINT_H
#define SG_POINT_H

#include <climits>
#include <cstdlib>
#include <iosfwd>
#include <string>
#include "SgArray.h"
#include "SgMove.h"
#include "SgUtil.h"

//----------------------------------------------------------------------------

/** Minimum board size. */
const int SG_MIN_SIZE = 2;

/** Maximum board size. */
const int SG_MAX_SIZE = 19;

/** Point or SG_PASS. */
typedef int SgPoint;

/** Marker used for marking the end of points lists stored in arrays. */
const SgPoint SG_ENDPOINT = 0;

/** For symmetry, and for use in loops etc. */
const int SG_MINPOINT = 0;

/** Board plus borders. */
const int SG_MAXPOINT = SG_MAX_SIZE * SG_MAX_SIZE + 3 * (SG_MAX_SIZE + 1);

/** Maximum number of points on board. */
const int SG_MAX_ONBOARD = SG_MAX_SIZE * SG_MAX_SIZE;

/** The maximum number of moves.
    Limit using the largest possible board size and pass move.
*/
const int SG_MAX_MOVES = SG_MAX_ONBOARD + 1;

/** West-East  : offset of horizontal neighbors */
const int SG_WE = 1;

/** North-South: offset of vertical neighbors. */
const int SG_NS = SG_MAX_SIZE + 1;

/** Special parameter of type Point */
const SgPoint SG_NULLPOINT = SG_NULLMOVE;

/* Pass move.
   For games in which moves can be described by a SgPoint and pass is a
   legal move, such that legal moves can be stored in an array indexed by
   the move.
   @note SG_PASS should not be used in games that cannot use SgPoint to
   describe a move or in which pass is not legal, because of the potential
   conflict with the integer range for describing moves in those games.
*/
const SgMove SG_PASS = SG_MAXPOINT + 1;

/** Test if move is not a point.
    Returns true for special moves with negative values as defined in SgMove.h
    (e.g. SG_NULLMOVE) and for SG_PASS.
*/
inline bool SgIsSpecialMove(SgMove m)
{
    return m < 0 || m == SG_PASS;
}

/** Coordinate in range 0 to SG_MAX_SIZE. */
typedef int SgGrid;

#define SG_ASSERT_GRIDRANGE(c) SG_ASSERTRANGE(c, 1, SG_MAX_SIZE)

#define SG_ASSERT_BOARDRANGE(p) \
    SG_ASSERTRANGE(p, SgPointUtil::Pt(1, 1), \
                   SgPointUtil::Pt(SG_MAX_SIZE, SG_MAX_SIZE))

//----------------------------------------------------------------------------

/** Write point.
    Wrapper class to allow overloading the output stream operator,
    because SgPoint is a typedef int.
*/
struct SgWritePoint
{
    SgPoint m_p;

    explicit SgWritePoint(SgPoint p);
};

/** @relatesalso SgWritePoint */
std::ostream& operator<<(std::ostream& out, const SgWritePoint& writePoint);

inline SgWritePoint::SgWritePoint(SgPoint p)
    : m_p(p)
{
}

//----------------------------------------------------------------------------

/** Read point.
    For overloading input stream operator for SgPoint (which is an integer).
    Usage:
    @code
    istream& in;
    SgPoint point;
    in >> SgReadPoint(point);
    if (! in)
       SgDebug() << "Invalid point\n";
    @endcode
*/
class SgReadPoint
{
public:
    SgReadPoint(SgPoint& point);

    void Read(std::istream& in) const;

private:
    SgPoint& m_point;
};

inline SgReadPoint::SgReadPoint(SgPoint& point)
    : m_point(point)
{
}

/** @relatesalso SgReadPoint */
inline std::istream& operator>>(std::istream& in,
                                const SgReadPoint& readPoint)
{
    readPoint.Read(in);
    return in;
}

//----------------------------------------------------------------------------

namespace SgPointUtil {

inline char Letter(int coord)
{
    return 'A' + static_cast<char>(coord - (coord >= 9 ? 0 : 1));
}

std::string PointToString(SgPoint p);

SgPoint Pt(int col, int row);

/** Lookup table internally used by SgPointUtil::Row(). */
class PointToRow
{
public:
    PointToRow()
    {
        m_row.Fill(-1);
        for (SgGrid row = 1; row <= SG_MAX_SIZE; ++row)
            for (SgGrid col = 1; col <= SG_MAX_SIZE; ++col)
                m_row[Pt(col, row)] = row;
    }

    SgGrid Row(SgPoint p) const
    {
        SG_ASSERT_BOARDRANGE(p);
        SG_ASSERT(m_row[p] >= 0);
        return m_row[p];
    }

private:
    SgArray<SgGrid,SG_MAXPOINT> m_row;
};

/** Lookup table internally used by SgPointUtil::Col(). */
class PointToCol
{
public:
    PointToCol()
    {
        m_col.Fill(-1);
        for (SgGrid row = 1; row <= SG_MAX_SIZE; ++row)
            for (SgGrid col = 1; col <= SG_MAX_SIZE; ++col)
                m_col[Pt(col, row)] = col;
    }

    SgGrid Col(SgPoint p) const
    {
        SG_ASSERT_BOARDRANGE(p);
        SG_ASSERT(m_col[p] >= 0);
        return m_col[p];
    }

private:
    SgArray<SgGrid,SG_MAXPOINT> m_col;
};

/** Return column of point.
    The lower left corner of the coordinate grid is (1, 1).
*/
inline SgGrid Col(SgPoint p)
{
    static PointToCol pointToCol;
    return pointToCol.Col(p);
}

/** Return row of point.
    The lower left corner of the coordinate grid is (1, 1).
*/
inline SgGrid Row(SgPoint p)
{
    static PointToRow pointToRow;
    return pointToRow.Row(p);
}

/** Converts from (col, row) to a one-dimensional point.
    Only for on board points; will trigger assertion for off-board points.
*/
inline SgPoint Pt(int col, int row)
{
    SG_ASSERT_GRIDRANGE(col);
    SG_ASSERT_GRIDRANGE(row);
    return SG_NS * row + col;
}

inline bool InBoardRange(SgPoint p)
{
    return SgUtil::InRange(p, Pt(1, 1), Pt(SG_MAX_SIZE, SG_MAX_SIZE));
}

/** True if the two points are adjacent to each other. */
inline bool AreAdjacent(SgPoint p1, SgPoint p2)
{
    int d = p2 - p1;
    return (d == SG_NS || d == SG_WE || d == -SG_NS || d == -SG_WE);
}

/** True if the two points are diagonally adjacent to each other. */
inline bool AreDiagonal(SgPoint p1, SgPoint p2)
{
    return (p2 == p1 - SG_NS - SG_WE || p2 == p1 - SG_NS + SG_WE
            || p2 == p1 + SG_NS - SG_WE || p2 == p1 + SG_NS + SG_WE);
}

/** Manhattan distance between two points on the board */
inline int Distance(SgPoint p1, SgPoint p2)
{
    return (std::abs(SgPointUtil::Row(p1) - SgPointUtil::Row(p2))
            + std::abs(SgPointUtil::Col(p1) - SgPointUtil::Col(p2)));
}

/** p2 is in 3x3 area around p1. */
inline bool In8Neighborhood(SgPoint p1, SgPoint p2)
{
    int d = p2 - p1;
    return (d == 0 || d == SG_NS || d == SG_WE || d == -SG_NS || d == -SG_WE
            || d == SG_NS - SG_WE || d == SG_NS + SG_WE
            || d == -SG_NS - SG_WE || d == -SG_NS + SG_WE);
}

/** Rotate/mirror point.
    Rotates and/or mirrors a point on a given board according to a given
    rotation mode.
    <table>
    <tr><th>Mode</th><th>col</th><th>row</th></tr>
    <tr><td>0</td><td>col</td><td>row</td></tr>
    <tr><td>1</td><td>size - col + 1</td><td>row</td></tr>
    <tr><td>2</td><td>col</td><td>size - row + 1</td></tr>
    <tr><td>3</td><td>row</td><td>col</td></tr>
    <tr><td>4</td><td>size - row + 1</td><td>col</td></tr>
    <tr><td>5</td><td>row</td><td>size - col + 1</td></tr>
    <tr><td>6</td><td>size - col + 1</td><td>size - row + 1</td></tr>
    <tr><td>7</td><td>size - row + 1</td><td>size - col + 1</td></tr>
    </table>
    @param rotation The rotation mode in [0..7]
    @param p The point to be rotated (SG_PASS is allowed and returned
    unmodified)
    @param size The board size
    @return The rotated mirrored point
*/
SgPoint Rotate(int rotation, SgPoint p, int size);

/** Return the inverse rotation as used in SgPointUtil::Rotate.
    @param rotation The rotation mode in [0..7]
    @return The inverse rotation mode
*/
int InvRotation(int rotation);

} // namespace SgPointUtil

//----------------------------------------------------------------------------

#endif // SG_POINT_H
