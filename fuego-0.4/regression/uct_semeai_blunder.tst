#-----------------------------------------------------------------------------
# Misplayed Semeai with small number of moves/liberties tests.
#-----------------------------------------------------------------------------

uct_param_player max_games 50000

#-----------------------------------------------------------------------------

loadsgf sgf/games/2009/May/Fuego9-Fuego-27.sgf 48
10 reg_genmove w
#? [D9|E9]
# basic approach move problem, but E9 throw-in also wins
# @todo make a related problem where throw-in fails.

11 sg_compare_float 0.7 uct_value
#? [1]
# W wins

loadsgf sgf/semeai/Fuego9-Fuego-27-semeai.5.sgf
20 reg_genmove w
#? [D9]*
# Modified from problem above such that throw-in fails.
# 2009-05 takes about 100K simulations to find D9 over E9.

loadsgf sgf/semeai/Fuego9-Fuego-27-semeai.1.sgf
30 reg_genmove w
#? [D7|D8]*
# H9 loses easily. but W wins the ko with C4 and E9 threats.

loadsgf sgf/semeai/Fuego9-Fuego-27-semeai.2.sgf
reg_genmove w
40 sg_compare_float 0.5 uct_value
#? [0]*
# W loses this race by 1 ko threat - after W C4, B D3, W G9, B has D9.

loadsgf sgf/semeai/Fuego9-Fuego-27-semeai.3.sgf

50 reg_genmove b
#? [G9|J6]
# simple connection wins semeai. J6 is also good.

60 sg_compare_float 0.7 uct_value
#? [1]*
# B wins. Eval 2009-05-08 is only 0.52

loadsgf sgf/semeai/Fuego9-Fuego-27-semeai.4.sgf
reg_genmove w
70 sg_compare_float 0.4 uct_value
#? [0]*
# W is lost. Initially, value over 0.7, then 0.45.