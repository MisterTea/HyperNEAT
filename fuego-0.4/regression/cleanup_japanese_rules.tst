#-----------------------------------------------------------------------------
# cleanup_japanese_rules: tests for finishing the game under Japanese rules.
# Test cases where territory (Japanese) rules demand different play than 
# area (e.g. Chinese) rules.
# Examples: play dame point or pass, do not play in own territory.
# Also see go/GoRules.h
# Also see http://en.wikipedia.org/wiki/Rules_of_Go#Scoring_systems
#-----------------------------------------------------------------------------

go_rules japanese
go_param_rules japanese_scoring 1

loadsgf sgf/games/2010/LittleGolem/1173624.sgf 48
10 reg_genmove w
#? [A1]*
# Fuego was winning, but cannot afford to play in its own territory.
# good test case for implementing Japanese rules.

loadsgf sgf/games/2010/LittleGolem/1177582.sgf 39
20 reg_genmove b
#? [PASS|J6|H6|E8|C9]*
# Fuego was winning, but cannot afford to play in its own territory.
# Playing a threat in the opponent's area is silly but OK.
# good test case for implementing Japanese rules.