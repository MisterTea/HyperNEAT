#-----------------------------------------------------------------------------
# Quick tests for the static safety solver
#-----------------------------------------------------------------------------

loadsgf sgf/safetytest/static-safety.1.sgf

10 go_safe benson
#? [76 .*]

20 go_safe static
#? [76 .*]

loadsgf sgf/safetytest/benson_safe1.sgf

30 go_safe benson
#? [180 .*]

40 go_safe static
#? [191 .*]

50 go_safe_dame_static
#? [E8]

loadsgf sgf/safetytest/benson_safe2.sgf

60 go_safe benson
#? [177 .*]

70 go_safe static
#? [186 .*]
# could be 8 more if bottom left resolved by static-dead analysis.
# can we solve the whole middle, too? maybe split it into 3-4 parts.

80 go_safe_dame_static
#? []

loadsgf sgf/safetytest/benson_safe3.sgf

90 go_safe benson
#? [225 .*]

100 go_safe static
#? [225 .*]

110 go_safe_dame_static
#? []

loadsgf sgf/safetytest/benson_safe4.sgf

120 go_safe benson
#? [54 .*]

130 go_safe static
#? [54 .*]

140 go_safe_dame_static
#? []

loadsgf sgf/safetytest/benson_safe5.sgf

150 go_safe benson
#? [21 .*]

160 go_safe static
#? [42 .*]

170 go_safe_dame_static
#? []

loadsgf sgf/safetytest/benson_unsafe.sgf

180 go_safe benson
#? [0]

190 go_safe static
#? [155 .*]
# could be a bit more if top left corner solved

200 go_safe_dame_static
#? [T16]

loadsgf sgf/safetytest/static-safety.2.sgf 90

210 go_safe benson
#? [57 .*]

220 go_safe static
#? [81 .*]

221 go_safe_dame_static
#? []

loadsgf sgf/safetytest/static-safety.2.sgf 91

230 go_safe benson
#? [57 .*]

240 go_safe static
#? [81 .*]

241 go_safe_dame_static
#? []

loadsgf sgf/safetytest/static-safety.2.sgf 92

250 go_safe benson
#? [57 .*]

260 go_safe static
#? [81 .*]

261 go_safe_dame_static
#? []

#------------------------------------------------------------------------------
# test cases from gnugo seki tests. Make sure some seki are not "statically safe".

loadsgf sgf/gnugo-games/seki01.sgf

500 go_safe static
#? [0]

501 go_safe_dame_static
#? []

loadsgf sgf/gnugo-games/seki02.sgf

510 go_safe static
#? [32 .*]

511 go_safe_dame_static
#? []

loadsgf sgf/gnugo-games/seki03.sgf

520 go_safe static
#? [63 .*]

521 go_safe_dame_static
#? []

loadsgf sgf/gnugo-games/seki04.sgf

530 go_safe static
#? [56 .*]

531 go_safe_dame_static
#? []

loadsgf sgf/gnugo-games/seki05.sgf

540 go_safe static
#? [53 .*]

541 go_safe_dame_static
#? []

loadsgf sgf/gnugo-games/seki06.sgf

550 go_safe static
#? [52 .*]

551 go_safe_dame_static
#? []

loadsgf sgf/gnugo-games/seki07.sgf

560 go_safe static
#? [52 .*]

561 go_safe_dame_static
#? []

loadsgf sgf/gnugo-games/seki08.sgf

570 go_safe static
#? [52 .*]

571 go_safe_dame_static
#? []

loadsgf sgf/gnugo-games/seki09.sgf

580 go_safe static
#? [64 .*]

581 go_safe_dame_static
#? []

loadsgf sgf/gnugo-games/seki10.sgf

590 go_safe static
#? [65 .*]

591 go_safe_dame_static
#? []

loadsgf sgf/gnugo-games/seki11.sgf

600 go_safe static
#? [53 .*]

601 go_safe_dame_static
#? []

loadsgf sgf/gnugo-games/seki12.sgf

610 go_safe static
#? [49 .*]

611 go_safe_dame_static
#? []

loadsgf sgf/gnugo-games/seki13.sgf

620 go_safe static
#? [48 .*]

621 go_safe_dame_static
#? []

loadsgf sgf/gnugo-games/seki14.sgf

630 go_safe static
#? [61 .*]

631 go_safe_dame_static
#? []

loadsgf sgf/gnugo-games/seki15.sgf

640 go_safe static
#? [60 .*]

641 go_safe_dame_static
#? []

loadsgf sgf/gnugo-games/seki_nakade1.sgf

650 go_safe static
#? [0]

651 go_safe_dame_static
#? []

loadsgf sgf/gnugo-games/seki_nakade2.sgf

660 go_safe static
#? [0]

661 go_safe_dame_static
#? []

loadsgf sgf/safetytest/static-safety.3.sgf

670 go_safe benson
#? [0]

680 go_safe static
#? [42 .*]
# should be 16 more for top middle. Or more if B left side solved.

681 go_safe_dame_static
#? []

loadsgf sgf/safetytest/static-safety.4.sgf

690 go_safe benson
#? [25 .*]

700 go_safe static
#? [36 .*]

701 go_safe_dame_static
#? []

loadsgf sgf/safetytest/static-safety.5.sgf

710 go_safe benson
#? [0]

720 go_safe static
#? [20 .*]
# @todo should solve a few more of these

721 go_safe_dame_static
#? []

loadsgf sgf/safetytest/static-safety.6.sgf

730 go_safe benson
#? [37 .*]

740 go_safe static
#? [37 .*]

741 go_safe_dame_static
#? []

loadsgf sgf/safetytest/static-safety.7.sgf

750 go_safe benson
#? [0]

760 go_safe static
#? [0]

761 go_safe_dame_static
#? []

loadsgf sgf/safetytest/static-safety.8.sgf

770 go_safe benson
#? [0]

780 go_safe static
#? [0]*
# @todo can make seki - right now this is misclassified as statically safe 

781 go_safe_dame_static
#? []

loadsgf sgf/safetytest/nosafe.3.sgf
790 go_safe benson black
#? [0]

800 go_safe static black
#? [0]

801 go_safe_dame_static
#? []

loadsgf sgf/safetytest/nosafe.4.sgf
810 go_safe benson black
#? [0]

820 go_safe static black
#? [0]

821 go_safe_dame_static
#? []

loadsgf sgf/safetytest/static-safety.9.sgf

830 go_safe benson
#? [0]

840 go_safe static
#? [0]

841 go_safe_dame_static
#? []

loadsgf sgf/safetytest/eyetest001.sgf
850 go_safe benson
#? [52 .*]

860 go_safe static
#? [52 .*]

861 go_safe_dame_static
#? []

loadsgf sgf/eyes/1.5/make-nakade.2.sgf 1

870 go_safe benson
#? [0]

880 go_safe static
#? [20 .*]

881 go_safe_dame_static
#? []

loadsgf sgf/safetytest/connect-to-safe.1.sgf
890 go_safe benson black
#? [19 .*]

891 go_safe benson white
#? [40 .*]

900 go_safe static black
#? [41 .*]

901 go_safe static white
#? [40 .*]

910 go_safe_dame_static
#? []

loadsgf sgf/safetytest/nakade-2x3.sgf
920 go_safe benson black
#? [0]

930 go_safe benson white
#? [61 .*]

940 go_safe static black
#? [0]

950 go_safe static white
#? [81 .*]

960 go_safe_dame_static
#? []

loadsgf sgf/safetytest/static-safety.10.sgf

970 go_safe benson black
#? [19 .*]

971 go_safe benson white
#? [28 .*]

980 go_safe static black
#? [19 .*]
# @todo the next version should recognize the top as safe.

981 go_safe static white
#? [47 .*]

990 go_safe_dame_static
#? []


