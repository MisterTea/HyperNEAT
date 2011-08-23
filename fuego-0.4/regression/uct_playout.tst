#-----------------------------------------------------------------------------
# Tests using playout policy directly for move generation.
#-----------------------------------------------------------------------------

uct_param_player search_mode playout_policy

#-----------------------------------------------------------------------------
# Capture heuristic

loadsgf sgf/uct_playout/uct_playout_capture_1.sgf
10 reg_genmove b
#? [F3]

#-----------------------------------------------------------------------------
# Atari heuristic

loadsgf sgf/uct_playout/uct_playout_atari_defense_1.sgf
110 reg_genmove b
#? [B1]

loadsgf sgf/uct_playout/uct_playout_atari_defense_2.sgf
120 reg_genmove w
#? [B3]

loadsgf sgf/uct_playout/uct_playout_atari_defense_3.sgf
130 reg_genmove w
#? [D2]
