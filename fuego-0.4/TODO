This list is a collection ideas to work on to improve Fuego. It does not
include bugs. Bugs in released version should be reported using the bug
tracker at SourceForge; bugs in development versions should be posted to
the fuego-devel mailing list.

=============================================================================
How to make Fuego stronger 
=============================================================================

Time management
---------------

Check if Fuego plays stronger against MoGo with a fixed time per move compared
to a time per game. If yes, this is an indication that our time manager can be
improved. Imrpove the time manager.

RAVE parameters
---------------

Tune the parameters (rave_weight_initial/final) for the RAVE weight function
now that the formula was fixed after Davis Tom's observation. I already run a
few tests on 9x9 and 19x19 to vary rave_weight_initial (keeping
rave_weight_final at 5000) and it seems that a value close to 1.0 is still
optimal, but I haven't tested different values for rave_weight_final. Note
that the RAVE weight function can work well for very short thinking times, but
bad at longer ones, so one should use reasonable time limits (e.g. what is
used on CGOS). Also, I had different behaviour against different opponents, so
one should check at least after tuning, that the performance with the tuned
value is indeed increased vs. MoGo, GNU Go and in self-play.

Weighted RAVE update
--------------------

Try different functions for weighting the RAVE updates (see
SgUctSearch::WeightRaveUpdates()). The function should probably still depend
on the move difference relative to the sequence length, but does not
necessarily have to be linear. The current version seemed better than using no
weighting on 9x9 and had a positive influence on semeai regression tests, but
not enough test games were played on 19x19. Also, there seemed to be a
different effect when playing vs GNU Go and MoGo.

Prior knowledge initialization
------------------------------

Investigate, if it makes a difference to initialize only the RAVE value with
prior knowledge, but not the move value. If it is better, it should be a
configurable option, because there could be subclasses of SgUctSearch that use
prior knowledge, but no RAVE.

Ladder attack moves
-------------------

Initialize successful ladder attack moves with positive prior knowledge. Right
now, Fuego will not see long successful ladders on 19x19. The current root
filter only prunes certain unsuccessful ladder defense moves. This will
require to make GoLadder thread-safe (in particular not using SgList) or
protect the call to it by a global lock. Before doing refactoring work on
GoLadder, there should be more regression tests for ladders (*not* using the
general reg_genmove, but a more specific command that invokes GoLadder)

Early pass
----------

Investigate, how much of a loss in performance is caused by the early pass
algorithm. Run experiments with uct_param_player early_pass 0 and 1. If
necessary, tune the resign_threshold, which is also used in the early pass
algorithm. A problem that was reported by Martin (see thread in fuego-devel,
date: 02/2009, subject:  Loss against Testch2 - premature pass bug), was
caused by not seeing that a pass of the opponent did win the game under
Tromp-Taylor scoring rules, because pass moves are explored late because of a
negative prior knowledge value. I fixed the problem by adding a call to
GoBoardUtil::PassWins() in DoEarlyPassSearch(), but this works only if the
game is played under Tromp-Taylor rules. It may be a better idea to not
initialize pass moves with a negative value, if the last move was already a
pass (only if both passes are in the search, otherwise it is not treated as a
terminal position).

Mercy rule
----------

Check if the mercy rule is still helpful. In tests long time ago, I found that
it does not change the winning rate against GNU Go, if the number of
simulations is fixed, but it cause a ~5% speed improvement. Check if this is
still the case. Maybe tune the threshold or try a threshold that depends on
the distance from the start of the playout phase and use a lower threshold far
away. If the mercy rule does not help, it should be removed, since it distorts
territory statistics.

=============================================================================
Useful functionality
=============================================================================

Native Windows port
-------------------

Port Fuego to Windows. The current Windows binary compiled with Cygwin GCC
runs at only half the speed compared with a Linux version on the same machine.
To compile Fuego with Visual Studio's C++ compiler, create a project, import
all the necessary files and add a Windows implementation to the
platform-specific files. As far as I can think of, only the functions that
measure CPU-time and wall time in SgTime need a Windows specific
implementation (add conditional code, if the macro WIN32 is defined).
SgProcess also uses platform-specific (POSIX) functions, but since it is not
used by FuegoMain, simply don't include SgProcess.cpp into the project for
now. Compiling Fuego with a different compiler could detect unknown bugs or
portability problems.

=============================================================================
Improving the source code
=============================================================================

GoBoardUtil
-----------

Remove functions from GoBoardUtil that are too specific and not used at at
least 2-3 different places. These functions have a high probability of
changing and will break external projects, if they depend on them. Functions
that are used at only one place and are not obviosly generally useful and
stable should become local static functions or private member functions at the
files/classes, where they are used.

SgSearch
--------

Make SgSearch thread-safe (i.e. don't use SgList for variables and parameters
anymore, which is not thread-safe)

Namespaces
----------

Use namespaces in Fuego. Following the Java convention, I would suggest to use
the reversed SourceForge domain to have a universal unique namespace
(net_sf_fuego) and then sub-namespaces for the modules (e.g.
net_sf_fuego::smartgame). If this is done, it will be possible to put a
"using namespace std: in the "namespace net_sf_fuego" block in the header
file without polluting the global namespace of a user of Fuego modules. Then,
the explicit usage of std:: for symbols from standard headers is no longer
needed in Fuego's header files.


