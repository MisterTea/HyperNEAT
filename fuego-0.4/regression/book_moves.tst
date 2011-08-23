#-----------------------------------------------------------------------------
# Test the opening book using the book_moves command
#-----------------------------------------------------------------------------

book_load ../book/book.dat

#-----------------------------------------------------------------------------
loadsgf sgf/games/2007/CGOS/200368.sgf 9
10 book_moves
#? [D2]
# E2 seems OK too.

loadsgf sgf/games/2007/CGOS/193991.sgf 2
20 book_moves
#? [E4]

loadsgf sgf/games/2008/CGOS/595283.sgf 5
30 book_moves
#? [E3]
# book move was C4, but this seems refuted by this game.
# See followup tests below.
# then it was D3.
# Now back to E3.

loadsgf sgf/games/2008/CGOS/595283.sgf 6
40 book_moves
#? [D4]

loadsgf sgf/games/2008/CGOS/595283.sgf 8
50 book_moves
#? [C3]

loadsgf sgf/games/2008/CGOS/595283.sgf 10
60 book_moves
#? [C6]

loadsgf sgf/games/2008/CGOS/595283.sgf 12
70 book_moves
#? [E4]

loadsgf sgf/games/2008/CGOS/595283.sgf 14
80 book_moves
#? [E3]

loadsgf sgf/games/2008/CGOS/595283.sgf 16
90 book_moves
#? [F5]

loadsgf sgf/games/2008/CGOS/595283.sgf 18
100 book_moves
#? [G6]

loadsgf sgf/games/2008/CGOS/595042.sgf 10
110 book_moves
#? [C4]
# This whole line is a big headache. This game seems to refute D8.
# The bottom group is too weak after G3.
# not convinced that C4 is enough, either. Maybe W needs to deviate earlier.

loadsgf sgf/book_test/book.0.sgf
120 book_moves
#? [E5]
# 9 | E5  (empty board)

loadsgf sgf/book_test/book.1.sgf
130 book_moves
#? [D3]
# 9 E5 | D3

loadsgf sgf/book_test/book.2.sgf
140 book_moves
#? [F3]
# 9 E5 D3 | F3

loadsgf sgf/book_test/book.3.sgf
150 book_moves
#? [C4]
loadsgf sgf/book_test/book.4.sgf
160 book_moves
#? [D3]
loadsgf sgf/book_test/book.5.sgf
170 book_moves
#? [G5]
loadsgf sgf/book_test/book.6.sgf
180 book_moves
#? [G5]
loadsgf sgf/book_test/book.7.sgf
190 book_moves
#? [F4]
loadsgf sgf/book_test/book.8.sgf
200 book_moves
#? [G4]
loadsgf sgf/book_test/book.9.sgf
210 book_moves
#? [F4]
loadsgf sgf/book_test/book.10.sgf
220 book_moves
#? [E4]
loadsgf sgf/book_test/book.11.sgf
230 book_moves
#? [F6]
loadsgf sgf/book_test/book.12.sgf
240 book_moves
#? [F5]
# recommended by Yu Ping.
