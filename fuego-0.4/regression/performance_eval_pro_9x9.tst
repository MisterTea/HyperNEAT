#-----------------------------------------------------------------------------
# Tests from games played by professionals
#-----------------------------------------------------------------------------

loadsgf sgf/games/pro-9x9/zhou9a.sgf 60
reg_genmove w
10 sg_compare_float 0.4 uct_value
#? [-1]*
# W is dead and completely lost.

