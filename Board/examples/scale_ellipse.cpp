/**
 * @file   scale_ellipse.cpp
 * @author Sebastien Fourey <http://www.greyc.ensicaen.fr/~seb>
 * @date   Tue Aug 21
 * 
 * @brief  Sample program to check that arrows are correctly drawn.
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <http://www.greyc.ensicaen.fr/~seb/>
 */
/*
 * @file   ruler.cpp
 * @author Sebastien Fourey <http://www.greyc.ensicaen.fr/~seb>
 * @date   Tue Aug 21
 * 
 * @brief  Sample program to check that ellipses are correctly scaled.
 *
 */
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <cmath>
#include <sstream>
#include <string>
#include "Board.h"
using namespace LibBoard;

int main( int , char *[] )
{
  Board board;
  board.setLineWidth(1);

  board << Board::UCentimeter;

  Group g;
  
  g << Rectangle( -30, 10, 60, 20, Color::Green, Color::White, 1 );
  g << Ellipse( 0, 0, 30, 10, Color::Red, Color::White, 1 );
  g << Ellipse( 0, 0, 2, 10, Color::None, Color::Green, 0.1 );
  //board << g.rotated( 1.5 ).scaled(2);
  board << g;
  board.addDuplicates( g, 5, 0, -15, 0.8, 0.8, 0.2 );
  board.addDuplicates( g, 5, 0, 15, 0.8, 0.8, -0.2 );
  board << g.scaled( 0.2 );
  
  board.saveEPS( "scale_ellipse.eps", Board::A4 );
  board.saveFIG( "scale_ellipse.fig", Board::A4 );
  board.saveSVG( "scale_ellipse.svg", Board::A4 );
  exit(0);
}
