/** @mainpage

    @section sgoverview Overview

    SmartGame provides general utility classes and code that can be
    shared between different two-player board games.
    Some important classes are:

    - Classes for representing square boards and the player color of
      two-player games (Black and White):
      SgBoardConst, SgBWIterator, SgBWArray, SgPoint, SgPointSet
    - Container classes:
      SgArray, SgList, SgSList, SgStack
    - Utility classes and platform dependent functionality:
      SgStatistics, SgTime
    - Search algorithms:
      SgSearch, SgUctSearch
    - Classes for using the SGF file format:
      SgNode, SgProp
    - Utility classes for Go Text Protocol engines extending the functionality
      of the GtpEngine library (GTP can also be used for other games than Go):
      SgGtpCommands, SgGtpUtil

    @section sgdependencies Dependencies

    - %GtpEngine

    @section sgdocumentation Documentation

    - @ref sgnotes
    - @ref sgboardrepresentation
    - @ref sguctgroup
*/

