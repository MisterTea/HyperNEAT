/** @mainpage

    @section gooverview Overview

    Go specific classes.

    - GoBoard is a general usable and fast Go board implementation,
      which keeps track of liberty and stone lists
    - GoPlayer is a base class for players
    - GoGameRecord, GoGame represent a game with a tree of game nodes
    - The regions code represents some higher level objects on the board
      (GoRegion, GoBlock, GoChain, GoRegionBoard, GoRegionUtil)
    - The static safety solver code is based on regions (GoBensonSolver,
      GoSafetySolver, GoStaticSafetySolver, GoSafetyUtil)
    - GoGtpEngine is a GtpEngine which contains a GoBoard, GoPlayer and GoGame

    @section godependencies Dependencies

    - %GtpEngine
    - SmartGame

    @section godocumentation Documentation

    - @ref gonotes
    - @ref goboardko
    - @ref goboardhash
    - @ref gogtpenginecommands
    - @ref gobookcommands
*/

