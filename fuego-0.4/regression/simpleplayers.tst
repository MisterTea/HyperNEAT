#-----------------------------------------------------------------------------
# Test basic functionality of simple players.
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# test random player

fuegotest_param player random

loadsgf sgf/simpleplayers/simpleplayers.2.sgf 1

# C1 and D2 are not simple eyes, play there
10 reg_genmove black
#? [C1|D2]

# W has no move
20 reg_genmove white
#? [PASS]

loadsgf sgf/simpleplayers/simpleplayers.2.sgf 2

# now B has no move as well
30 reg_genmove black
#? [PASS]

#-----------------------------------------------------------------------------
# test safe territory player

fuegotest_param player safe

loadsgf sgf/simpleplayers/simpleplayers.1.sgf 1

# create safe group
1010 reg_genmove black
#? [A6]

loadsgf sgf/simpleplayers/simpleplayers.1.sgf 2

# create safe group
1020 reg_genmove white
#? [F4]

loadsgf sgf/simpleplayers/simpleplayers.1.sgf 3

# extend safe group
1030 reg_genmove black
#? [C5]

loadsgf sgf/simpleplayers/simpleplayers.1.sgf 4

# extend safe group
1040 reg_genmove white
#? [D2]

#-----------------------------------------------------------------------------
# test average liberty player

fuegotest_param player average

loadsgf sgf/simpleplayers/test-pass.sgf

go_param_rules japanese_scoring 1
go_param_rules capture_dead 0

# all safe by static safety - pass.
2000 reg_genmove white
#? [PASS]

# all safe by static safety - pass.
2001 reg_genmove black
#? [PASS]

go_param_rules capture_dead 1

2002 reg_genmove black
#? [!PASS]

loadsgf sgf/simpleplayers/simpleplayers.1.sgf 1

2020 reg_genmove black
#? [C5]

2030 reg_genmove white
#? [D2|F4]

loadsgf sgf/simpleplayers/simpleplayers.1.sgf 3

2040 reg_genmove black
#? [C5]

2050 reg_genmove white
#? [D2]


loadsgf sgf/simpleplayers/simpleplayers.1.sgf 4

# extend safe group
2060 reg_genmove black
#? [B3]

2070 reg_genmove white
#? [D2]

loadsgf sgf/simpleplayers/simpleplayers.3.sgf 1

2080 reg_genmove black
#? [F1]

2090 reg_genmove white
#? [C7|C5]

loadsgf sgf/simpleplayers/simpleplayers.3.sgf 2

2100 reg_genmove black
#? [F1]

2110 reg_genmove white
#? [C5]

loadsgf sgf/simpleplayers/simpleplayers.3.sgf 3

2120 reg_genmove black
#? [E4]

2130 reg_genmove white
#? [C5]

loadsgf sgf/simpleplayers/simpleplayers.3.sgf 4

2140 reg_genmove black
#? [E4]

2150 reg_genmove white
#? [E3|E4|E5]

loadsgf sgf/simpleplayers/simpleplayers.3.sgf 5

2160 reg_genmove black
#? [E3]

2170 reg_genmove white
#? [E3]



#-----------------------------------------------------------------------------
# test capture player

fuegotest_param player capture

loadsgf sgf/simpleplayers/test-pass.sgf

go_param_rules japanese_scoring 1
go_param_rules capture_dead 0

# all safe by static safety - pass.
3000 reg_genmove white
#? [PASS]

# all safe by static safety - pass.
3001 reg_genmove black
#? [PASS]

go_param_rules capture_dead 1

3002 reg_genmove black
#? [!PASS]

#-----------------------------------------------------------------------------
# test greedy player

fuegotest_param player greedy

loadsgf sgf/simpleplayers/test-pass.sgf

go_param_rules japanese_scoring 1
go_param_rules capture_dead 0

# all safe by static safety - pass.
4000 reg_genmove white
#? [PASS]

# all safe by static safety - pass.
4001 reg_genmove black
#? [PASS]

go_param_rules capture_dead 1

4002 reg_genmove black
#? [!PASS]

#-----------------------------------------------------------------------------
# test influence player

fuegotest_param player influence

loadsgf sgf/simpleplayers/test-pass.sgf

go_param_rules japanese_scoring 1
go_param_rules capture_dead 0

# all safe by static safety - pass.
5000 reg_genmove white
#? [PASS]

# all safe by static safety - pass.
5001 reg_genmove black
#? [PASS]

go_param_rules capture_dead 1

5002 reg_genmove black
#? [!PASS]

#-----------------------------------------------------------------------------
# test ladder player

fuegotest_param player ladder

loadsgf sgf/simpleplayers/test-pass.sgf

go_param_rules japanese_scoring 1
go_param_rules capture_dead 0

# all safe by static safety - pass.
6000 reg_genmove white
#? [PASS]

# all safe by static safety - pass.
6001 reg_genmove black
#? [PASS]

go_param_rules capture_dead 1

6002 reg_genmove black
#? [!PASS]

loadsgf sgf/simpleplayers/simpleplayers.3.sgf 1

6080 reg_genmove black
#? [C7]

6090 reg_genmove white
#? [C7]

loadsgf sgf/simpleplayers/simpleplayers.3.sgf 2

6100 reg_genmove black
#? [F1]

6110 reg_genmove white
#? [F1]

loadsgf sgf/simpleplayers/simpleplayers.3.sgf 3

6120 reg_genmove black
#? [C5]

6130 reg_genmove white
#? [C5]
