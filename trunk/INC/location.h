//Copyright (c) 2010, Information Security Institute of Wuhan Universtiy(ISIWhu)
//Project Homepage:http://code.google.com/p/whutnetsim/
//corresponding author's email: guochi@mail.whu.edu.cn


//All rights reserved
//
//PLEASE READ THIS DOCUMENT CAREFULLY BEFORE UTILIZING THE PROGRAM
//BY UTILIZING THIS PROGRAM, YOU AGREE TO BECOME BOUND BY THE TERMS OF
//THIS LICENSE.  IF YOU DO NOT AGREE TO THE TERMS OF THIS LICENSE, DO
//NOT USE THIS PROGRAM OR ANY PORTION THEREOF IN ANY FORM OR MANNER.
//
//This License allows you to:
//1. Make copies and distribute copies of the Program's source code provide that any such copy 
//   clearly displays any and all appropriate copyright notices and disclaimer of warranty as set 
//   forth in this License.
//2. Modify the original copy or copies of the Program or any portion thereof ("Modification(s)"). 
//   Modifications may be copied and distributed under the terms and conditions as set forth above. 
//   Any and all modified files must be affixed with prominent notices that you have changed the 
//   files and the date that the changes occurred.

//Termination:
//   If at anytime you are unable to comply with any portion of this License you must immediately 
//   cease use of the Program and all distribution activities involving the Program or any portion 
//   thereof.

//Statement:
//   In this program, part of the code is from the GTNetS project, The Georgia Tech Network 
//   Simulator (GTNetS) is a full-featured network simulation environment that allows researchers in 
//   computer networks to study the behavior of moderate to large scale networks, under a variety of 
//   conditions. Our work have great advance due to this project, Thanks to Dr. George F. Riley from 
//   Georgia Tech Research Corporation. Anyone who wants to study the GTNetS can come to its homepage:
//   http://www.ece.gatech.edu/research/labs/MANIACS/GTNetS/
//


//File Information:
//
//
//File Name:
//File Purpose:
//Original Author:
//Author Organization:
//Construct Date:
//Modify Author:
//Author Organization:
//Modify Date:



// Define locations and regions.
// George F. Riley, Georgia Tech, Summer 2003

#ifndef __location_h__
#define __location_h__

#include <vector>

#include "G_common-defs.h"

//Doc:ClassXRef
class Location {
  //Doc:Class Contains location information in the X--Y--Z coordinate for nodes.
public:
  //Doc:Method
  Location() : xPos(0), yPos(0), zPos(0) {}
    //Doc:Desc Default constructor.  Sets both X, Y and Z position to zero.

  //Doc:Method
  Location(Meters_t x, Meters_t y, Meters_t z=0) : xPos(x), yPos(y), zPos(z) {}
    //Doc:Desc Specifies X, Y and Z locations.
    //Doc:Arg1 X location (meters)
    //Doc:Arg2 Y location (meters)
    //Doc:Arg3 Z location (meters)

  Location(const Location& l) : xPos(l.X()), yPos(l.Y()), zPos(l.Z()) {}

  //Doc:Method
  Meters_t X() const { return xPos; }
    //Doc:Desc Get the X location.
    //Doc:Return The X location (meters)

  Meters_t Y() const { return yPos; }
    //Doc:Desc Get the Y location.
    //Doc:Return The Y location (meters)

  Meters_t Z() const { return zPos; }
  //Doc:Desc Get the Z location.
  //Doc:Return The Z location (meters)

  void     X(Meters_t x) { xPos = x; }
    //Doc:Desc Set the X location.
    //Doc:Arg1 The X location (meters)

  void     Y(Meters_t y) { yPos = y; }
    //Doc:Desc Set the Y location.
    //Doc:Arg1 The Y location (meters)

  void     Z(Meters_t z) { zPos = z; }
    //Doc:Desc Set the Z location.
    //Doc:Arg1 The Z location (meters)

private:
  Meters_t xPos;
  Meters_t yPos;
  Meters_t zPos;
};

bool operator==(const Location& l, const Location& r);

//Doc:ClassXRef
class RectRegion {
  //Doc:Class Contains information about a rectangular  region on the grid.
public:
  //Doc:Method
  RectRegion() : ll(), ur() {}
    //Doc:Desc Default constructor for an empty region.

  //Doc:Method
  RectRegion(const Location& l, const Location& u) : ll(l), ur(u) {}
    //Doc:Desc Construct a region from lower left and upper right locations.
    //Doc:Arg1 Lower left location.
    //Doc:Arg2 Upper right location.

  //Doc:Method
  Location& LowerLeft()  { return ll;}
    //Doc:Desc Query the lower left corner of the region.
    //Doc:Return Lower left corner.

  //Doc:Method
  Location& UpperRight() { return ur;}
    //Doc:Desc Query the upper right corner of the region.
    //Doc:Return Upper right corner.

  bool InRegion(const Location& p) const
  {
    return p.X() >= ll.X() && p.X() <= ur.X() &&
           p.Y() >= ll.Y() && p.Y() <= ur.Y();
  }

  bool Empty() const { return ll.X() == ur.X() && ll.Y() == ur.Y(); }

private:
  Location ll;
  Location ur;
};


typedef std::vector<Location>      LocationVec_t;    // List of x/y/z points
typedef std::vector<LocationVec_t> LocationVecVec_t; // List of LocationVec

#endif
