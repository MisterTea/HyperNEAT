#-----------------------------------------------------------------------------
# Tests related to final moves in a game such as capturing dead stones,
# approach moves, inside connection moves, filling neutral points etc.
#-----------------------------------------------------------------------------

# CGOS rules (Tromp-Taylor scoring)
go_rules cgos

loadsgf sgf/games/2010/CGOS/1005347.sgf

10 reg_genmove b
#? [A6|B6|H1|J1]*
# Fuego resigned here because the search gets stuck on the sequence PASS PASS
# root filter prunes all black non-pass moves

loadsgf sgf/final_moves/play-in-benson-safe.1.sgf

20 reg_genmove b
#? [B1|E1|J1|D2|F2|F5|F8]*
# Black must fill own eye since passing loses and no other moves exist

loadsgf sgf/final_moves/play-in-benson-safe.2.sgf 2

30 reg_genmove b
#? [J1|F5|F8]*
# Black must fill own eye since passing and other moves lose
