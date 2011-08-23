#-----------------------------------------------------------------------------
# Static safety test for fullboard positions.
# 1~31 (q1-q31 from set1)
# 32~58 (CZD professional games from set2)
# 59~94   (korean professional players' games from set3)
# All results are based on the improved static safety recognition
# that recursively finds
# two disjoint connections for all interior points.
#-----------------------------------------------------------------------------

loadsgf sgf/safetytest-whole-board/q1.sgf

10 go_safe static
#? [124 .*]

11 go_safe static black
#? [53 .*]

12 go_safe static white
#? [71 .*]

13 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/q2.sgf

20 go_safe static
#? [15 .*]

21 go_safe static black
#? [0]

22 go_safe static white
#? [15 .*]

23 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/q3.sgf

30 go_safe static
#? [69 .*]

31 go_safe static black
#? [19 .*]

32 go_safe static white
#? [50 .*]

33 go_safe_dame_static
#? [J15]

loadsgf sgf/safetytest-whole-board/q4.sgf

40 go_safe static
#? [336 .*]

41 go_safe static black
#? [168 .*]

42 go_safe static white
#? [168 .*]

43 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/q5.sgf

50 go_safe static
#? [80 .*]

51 go_safe static black
#? [52 .*]

52 go_safe static white
#? [28 .*]

53 go_safe_dame_static
#? [E12]

loadsgf sgf/safetytest-whole-board/q6.sgf

60 go_safe static
#? [19 .*]

61 go_safe static black
#? [0]

62 go_safe static white
#? [19 .*]

63 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/q7.sgf

70 go_safe static
#? [17 .*]

71 go_safe static black
#? [17 .*]

72 go_safe static white
#? [0]

73 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/q8.sgf

80 go_safe static
#? [62 .*]

81 go_safe static black
#? [28 .*]

82 go_safe static white
#? [34 .*]

83 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/q9.sgf

90 go_safe static
#? [0]

91 go_safe static black
#? [0]

92 go_safe static white
#? [0]

93 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/q10.sgf

100 go_safe static
#? [84 .*]

101 go_safe static black
#? [0]

102 go_safe static white
#? [84 .*]

103 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/q11.sgf

110 go_safe static
#? [154 .*]

111 go_safe static black
#? [94 .*]

112 go_safe static white
#? [60 .*]

113 go_safe_dame_static
#? [L17]

loadsgf sgf/safetytest-whole-board/q12.sgf

120 go_safe static
#? [0]

121 go_safe static black
#? [0]

122 go_safe static white
#? [0]

123 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/q13.sgf

130 go_safe static
#? [136 .*]

131 go_safe static black
#? [82 .*]

132 go_safe static white
#? [54 .*]

133 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/q14.sgf

140 go_safe static
#? [210 .*]

141 go_safe static black
#? [87 .*]

142 go_safe static white
#? [123 .*]

143 go_safe_dame_static
#? [P7 P9 P11 K14]

loadsgf sgf/safetytest-whole-board/q15.sgf

150 go_safe static
#? [101 .*]

151 go_safe static black
#? [59 .*]

152 go_safe static white
#? [42 .*]

153 go_safe_dame_static
#? [H13 F15]

loadsgf sgf/safetytest-whole-board/q16.sgf

160 go_safe static
#? [170 .*]

161 go_safe static black
#? [124 .*]

162 go_safe static white
#? [46 .*]

163 go_safe_dame_static
#? [P11 P12 P14]

loadsgf sgf/safetytest-whole-board/q17.sgf

170 go_safe static
#? [69 .*]

171 go_safe static black
#? [48 .*]

172 go_safe static white
#? [21 .*]

173 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/q18.sgf

180 go_safe static
#? [0]

181 go_safe static black
#? [0]

182 go_safe static white
#? [0]

183 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/q19.sgf

190 go_safe static
#? [47 .*]

191 go_safe static black
#? [0]

192 go_safe static white
#? [47 .*]

193 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/q20.sgf

200 go_safe static
#? [62 .*]

201 go_safe static black
#? [0]

202 go_safe static white
#? [62 .*]

203 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/q21.sgf

210 go_safe static
#? [147 .*]

211 go_safe static black
#? [54 .*]

212 go_safe static white
#? [93 .*]

213 go_safe_dame_static
#? [K9 J10 G12]

loadsgf sgf/safetytest-whole-board/q22.sgf

220 go_safe static
#? [176 .*]

221 go_safe static black
#? [66 .*]

222 go_safe static white
#? [110 .*]

223 go_safe_dame_static
#? [P6 N8 H9 H10 N10]

loadsgf sgf/safetytest-whole-board/q23.sgf

230 go_safe static
#? [209 .*]

231 go_safe static black
#? [103 .*]

232 go_safe static white
#? [106 .*]

233 go_safe_dame_static
#? [F6]

loadsgf sgf/safetytest-whole-board/q24.sgf

240 go_safe static
#? [155 .*]

241 go_safe static black
#? [113 .*]

242 go_safe static white
#? [42 .*]

243 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/q25.sgf

250 go_safe static
#? [0]

251 go_safe static black
#? [0]

252 go_safe static white
#? [0]

253 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/q26.sgf

260 go_safe static
#? [125 .*]

261 go_safe static black
#? [39 .*]

262 go_safe static white
#? [86 .*]

263 go_safe_dame_static
#? [E6 F6 E8 A10]

loadsgf sgf/safetytest-whole-board/q27.sgf

270 go_safe static
#? [38 .*]

271 go_safe static black
#? [38 .*]

272 go_safe static white
#? [0]

273 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/q28.sgf

280 go_safe static
#? [0]

281 go_safe static black
#? [0]

282 go_safe static white
#? [0]

283 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/q29.sgf

290 go_safe static
#? [158 .*]

291 go_safe static black
#? [97 .*]

292 go_safe static white
#? [61 .*]

293 go_safe_dame_static
#? [D15 L17]

loadsgf sgf/safetytest-whole-board/q30.sgf

300 go_safe static
#? [61 .*]

301 go_safe static black
#? [61 .*]

302 go_safe static white
#? [0]

303 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/q31.sgf

310 go_safe static
#? [68 .*]

311 go_safe static black
#? [25 .*]

312 go_safe static white
#? [43 .*]

313 go_safe_dame_static
#? [N19]

loadsgf sgf/safetytest-whole-board/CZD_05.sgf

320 go_safe static
#? [32 .*]

321 go_safe static black
#? [32 .*]

322 go_safe static white
#? [0]

323 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/CZD_16.sgf

330 go_safe static
#? [179 .*]

331 go_safe static black
#? [84 .*]

332 go_safe static white
#? [95 .*]

333 go_safe_dame_static
#? [D5 O5 E8 N11 K13]

loadsgf sgf/safetytest-whole-board/CZD_17.sgf

340 go_safe static
#? [31 .*]

341 go_safe static black
#? [31 .*]

342 go_safe static white
#? [0]

343 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/CZD_20.sgf

350 go_safe static
#? [116 .*]

351 go_safe static black
#? [34 .*]

352 go_safe static white
#? [82 .*]

353 go_safe_dame_static
#? [G8]

loadsgf sgf/safetytest-whole-board/CZD_21.sgf

360 go_safe static
#? [227 .*]

361 go_safe static black
#? [97 .*]

362 go_safe static white
#? [130 .*]

363 go_safe_dame_static
#? [J12 F15]

loadsgf sgf/safetytest-whole-board/CZD_24.sgf

370 go_safe static
#? [0]

371 go_safe static black
#? [0]

372 go_safe static white
#? [0]

373 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/CZD_26.sgf

380 go_safe static
#? [115 .*]

381 go_safe static black
#? [98 .*]

382 go_safe static white
#? [17 .*]

383 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/CZD_28.sgf

390 go_safe static
#? [34 .*]

391 go_safe static black
#? [0]

392 go_safe static white
#? [34 .*]

393 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/CZD_29.sgf

400 go_safe static
#? [56 .*]

401 go_safe static black
#? [0]

402 go_safe static white
#? [56 .*]

403 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/CZD_31.sgf

410 go_safe static
#? [88 .*]

411 go_safe static black
#? [0]

412 go_safe static white
#? [88 .*]

413 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/CZD_33.sgf

420 go_safe static
#? [67 .*]

421 go_safe static black
#? [0]

422 go_safe static white
#? [67 .*]

423 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/CZD_34.sgf

430 go_safe static
#? [129 .*]

431 go_safe static black
#? [69 .*]

432 go_safe static white
#? [60 .*]

433 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/CZD_35.sgf

440 go_safe static
#? [128 .*]

441 go_safe static black
#? [0]

442 go_safe static white
#? [128 .*]

443 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/CZD_36.sgf

450 go_safe static
#? [86 .*]

451 go_safe static black
#? [24 .*]

452 go_safe static white
#? [62 .*]

453 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/CZD_39.sgf

460 go_safe static
#? [76 .*]

461 go_safe static black
#? [33 .*]

462 go_safe static white
#? [43 .*]

463 go_safe_dame_static
#? [L4]

loadsgf sgf/safetytest-whole-board/CZD_41.sgf

470 go_safe static
#? [86 .*]

471 go_safe static black
#? [76 .*]

472 go_safe static white
#? [10 .*]

473 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/CZD_46.sgf

480 go_safe static
#? [58 .*]

481 go_safe static black
#? [29 .*]

482 go_safe static white
#? [29 .*]

483 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/CZD_55.sgf

490 go_safe static
#? [105 .*]

491 go_safe static black
#? [88 .*]

492 go_safe static white
#? [17 .*]

493 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/CZD_58.sgf

500 go_safe static
#? [47 .*]

501 go_safe static black
#? [22 .*]

502 go_safe static white
#? [25 .*]

503 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/CZD_64.sgf

510 go_safe static
#? [0]

511 go_safe static black
#? [0]

512 go_safe static white
#? [0]

513 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/CZD_66.sgf

520 go_safe static
#? [14 .*]

521 go_safe static black
#? [0]

522 go_safe static white
#? [14 .*]

523 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/CZD_67.sgf

530 go_safe static
#? [115 .*]

531 go_safe static black
#? [54 .*]

532 go_safe static white
#? [61 .*]

533 go_safe_dame_static
#? [O13]

loadsgf sgf/safetytest-whole-board/CZD_68.sgf

540 go_safe static
#? [132 .*]

541 go_safe static black
#? [28 .*]

542 go_safe static white
#? [104 .*]

543 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/CZD_69.sgf

550 go_safe static
#? [126 .*]

551 go_safe static black
#? [59 .*]

552 go_safe static white
#? [67 .*]

553 go_safe_dame_static
#? [K5 L5 G6 J6 P6]

loadsgf sgf/safetytest-whole-board/CZD_71.sgf

560 go_safe static
#? [50 .*]

561 go_safe static black
#? [50 .*]

562 go_safe static white
#? [0]

563 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/CZD_72.sgf

570 go_safe static
#? [147 .*]

571 go_safe static black
#? [77 .*]

572 go_safe static white
#? [70 .*]

573 go_safe_dame_static
#? [F10 G11 H12]

loadsgf sgf/safetytest-whole-board/CZD_79.sgf

580 go_safe static
#? [44 .*]

581 go_safe static black
#? [0]

582 go_safe static white
#? [44 .*]

583 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/k01.sgf

590 go_safe static
#? [0]

591 go_safe static black
#? [0]

592 go_safe static white
#? [0]

593 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/k02.sgf

600 go_safe static
#? [186 .*]

601 go_safe static black
#? [81 .*]

602 go_safe static white
#? [105 .*]

603 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/k03.sgf

610 go_safe static
#? [171 .*]

611 go_safe static black
#? [72 .*]

612 go_safe static white
#? [99 .*]

613 go_safe_dame_static
#? [E8 N8]

loadsgf sgf/safetytest-whole-board/k04.sgf

620 go_safe static
#? [114 .*]

621 go_safe static black
#? [54 .*]

622 go_safe static white
#? [60 .*]

623 go_safe_dame_static
#? [L13]

loadsgf sgf/safetytest-whole-board/k05.sgf

630 go_safe static
#? [222 .*]

631 go_safe static black
#? [98 .*]

632 go_safe static white
#? [124 .*]

633 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/k06.sgf

640 go_safe static
#? [29 .*]

641 go_safe static black
#? [29 .*]

642 go_safe static white
#? [0]

643 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/k07.sgf

650 go_safe static
#? [156 .*]

651 go_safe static black
#? [102 .*]

652 go_safe static white
#? [54 .*]

653 go_safe_dame_static
#? [P11 M16]

loadsgf sgf/safetytest-whole-board/k08.sgf

660 go_safe static
#? [94 .*]

661 go_safe static black
#? [61 .*]

662 go_safe static white
#? [33 .*]

663 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/k09.sgf

670 go_safe static
#? [54 .*]

671 go_safe static black
#? [0]

672 go_safe static white
#? [54 .*]

673 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/k10.sgf

680 go_safe static
#? [96 .*]

681 go_safe static black
#? [0]

682 go_safe static white
#? [96 .*]

683 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/k11.sgf

690 go_safe static
#? [27 .*]

691 go_safe static black
#? [27 .*]

692 go_safe static white
#? [0]

693 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/k12.sgf

700 go_safe static
#? [170 .*]

701 go_safe static black
#? [108 .*]

702 go_safe static white
#? [62 .*]

703 go_safe_dame_static
#? [O11 F13]

loadsgf sgf/safetytest-whole-board/k13.sgf

710 go_safe static
#? [110 .*]

711 go_safe static black
#? [40 .*]

712 go_safe static white
#? [70 .*]

713 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/k14.sgf

720 go_safe static
#? [43 .*]

721 go_safe static black
#? [43 .*]

722 go_safe static white
#? [0]

723 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/k15.sgf

730 go_safe static
#? [138 .*]

731 go_safe static black
#? [60 .*]

732 go_safe static white
#? [78 .*]

733 go_safe_dame_static
#? [S8 Q17]

loadsgf sgf/safetytest-whole-board/k16.sgf

740 go_safe static
#? [12 .*]

741 go_safe static black
#? [12 .*]

742 go_safe static white
#? [0]

743 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/k17.sgf

750 go_safe static
#? [18 .*]

751 go_safe static black
#? [18 .*]

752 go_safe static white
#? [0]

753 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/k18.sgf

760 go_safe static
#? [36 .*]

761 go_safe static black
#? [0]

762 go_safe static white
#? [36 .*]

763 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/k19.sgf

770 go_safe static
#? [68 .*]

771 go_safe static black
#? [0]

772 go_safe static white
#? [68 .*]

773 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/k20.sgf

780 go_safe static
#? [15 .*]

781 go_safe static black
#? [15 .*]

782 go_safe static white
#? [0]

783 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/k21.sgf

790 go_safe static
#? [21 .*]

791 go_safe static black
#? [0]

792 go_safe static white
#? [21 .*]

793 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/k22.sgf

800 go_safe static
#? [55 .*]

801 go_safe static black
#? [0]

802 go_safe static white
#? [55 .*]

803 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/k23.sgf

810 go_safe static
#? [51 .*]

811 go_safe static black
#? [51 .*]

812 go_safe static white
#? [0]

813 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/k24.sgf

820 go_safe static
#? [29 .*]

821 go_safe static black
#? [29 .*]

822 go_safe static white
#? [0]

823 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/k25.sgf

830 go_safe static
#? [68 .*]

831 go_safe static black
#? [68 .*]

832 go_safe static white
#? [0]

833 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/k26.sgf

840 go_safe static
#? [32 .*]

841 go_safe static black
#? [32 .*]

842 go_safe static white
#? [0]

843 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/k27.sgf

850 go_safe static
#? [158 .*]

851 go_safe static black
#? [104 .*]

852 go_safe static white
#? [54 .*]

853 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/k28.sgf

860 go_safe static
#? [115 .*]

861 go_safe static black
#? [22 .*]

862 go_safe static white
#? [93 .*]

863 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/k29.sgf

870 go_safe static
#? [54 .*]

871 go_safe static black
#? [0]

872 go_safe static white
#? [54 .*]

873 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/k30.sgf

880 go_safe static
#? [25 .*]

881 go_safe static black
#? [25 .*]

882 go_safe static white
#? [0]

883 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/k31.sgf

890 go_safe static
#? [144 .*]

891 go_safe static black
#? [68 .*]

892 go_safe static white
#? [76 .*]

893 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/k32.sgf

900 go_safe static
#? [69 .*]

901 go_safe static black
#? [12 .*]

902 go_safe static white
#? [57 .*]

903 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/k33.sgf

910 go_safe static
#? [32 .*]

911 go_safe static black
#? [17 .*]

912 go_safe static white
#? [15 .*]

913 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/k34.sgf

920 go_safe static
#? [66 .*]

921 go_safe static black
#? [15 .*]

922 go_safe static white
#? [51 .*]

923 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/k35.sgf

930 go_safe static
#? [0]

931 go_safe static black
#? [0]

932 go_safe static white
#? [0]

933 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/static-safety.1.sgf

940 go_safe static
#? [227 .*]

941 go_safe static black
#? [161 .*]

942 go_safe static white
#? [66 .*]

943 go_safe_dame_static
#? []

loadsgf sgf/safetytest-whole-board/static-safety.2.sgf

950 go_safe static
#? [313 .*]

951 go_safe static black
#? [161 .*]

952 go_safe static white
#? [152 .*]

953 go_safe_dame_static
#? []

