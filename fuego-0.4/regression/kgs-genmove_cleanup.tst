#-----------------------------------------------------------------------------
# kgs-genmove_cleanup tests for GoUctGlobalSearchPlayer
#-----------------------------------------------------------------------------

uct_param_player max_games 10000

loadsgf sgf/games/2008/KGS/9/16/Fuego-masaician.sgf 296

10 kgs-genmove_cleanup w
#? [!PASS]*
# Fuego played a pass here, because the implementation of kgs-genmove_cleanup
# at the time was done independent of the player in GoGtpEngine by temporarily
# switching on GoRules::CaptureDead() and calling GoPlayer::GenMove(). This
# does not work here because then the quick check if a pass wins according to
# Tromp-Taylor counting triggered in Fuego (GoUctPlayer) and the computer
# should not pass before all dead stones are captured
