/** @mainpage

    @section fuegotestoverview Overview

    FuegoTest is a GTP interface with commands from the Fuego libraries for
    testing purposes. It avoids that it is necessary to include GTP commands
    into the main Fuego GTP interface for functionality that is not used by
    the main Fuego player used in the FuegoMain module. This engine also
    provides a switchable simple player from the SimplePlayers module, with
    can be set using the @c -player option or the @c fuegotest_player GTP
    command.

    @section fuegotestdependencies Dependencies

    - %GtpEngine
    - SmartGame
    - Go
    - SimplePlayers
    - GoUct

    @section fuegotestdocumentation Documentation

    - GTP commands
      - @ref gtpenginecommands "GtpEngine"
      - @ref sggtpcommandscommands "SgGtpCommands"
      - @ref gogtpenginecommands "GoGtpEngine"
      - @ref gogtpextracommands "GoGtpExtraCommands"
      - @ref gosafetycommands "GoSafetyCommands"
      - @ref fuegotestenginecommands "FuegoTestEngine"
*/
