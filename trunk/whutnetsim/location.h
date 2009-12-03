// GENERAL PUBLIC LICENSE AGREEMENT
// 
// PLEASE READ THIS DOCUMENT CAREFULLY BEFORE UTILIZING THE PROGRAM
// 
// BY UTILIZING THIS PROGRAM, YOU AGREE TO BECOME BOUND BY THE TERMS OF
// THIS LICENSE.  IF YOU DO NOT AGREE TO THE TERMS OF THIS LICENSE, DO
// NOT USE THIS PROGRAM OR ANY PORTION THEREOF IN ANY FORM OR MANNER.
// 
// This Program is licensed, not sold to you by GEORGIA TECH RESEARCH
// CORPORATION ("GTRC"), owner of all code and accompanying documentation
// (hereinafter "Program"), for use only under the terms of this License,
// and GTRC reserves any rights not expressly granted to you.
// 
// 1.  This License allows you to:
// 
// (a) make copies and distribute copies of the Program's source code
// provide that any such copy clearly displays any and all appropriate
// copyright notices and disclaimer of warranty as set forth in Article 5
// and 6 of this License.  All notices that refer to this License and to
// the absence of any warranty must be kept intact at all times.  A copy
// of this License must accompany any and all copies of the Program
// distributed to third parties.
// 
// A fee may be charged to cover the cost associated with the physical
// act of transferring a copy to a third party.  At no time shall the
// program be sold for commercial gain either alone or incorporated with
// other program(s) without entering into a separate agreement with GTRC.
//  
// 
// (b) modify the original copy or copies of the Program or any portion
// thereof ("Modification(s)").  Modifications may be copied and
// distributed under the terms and conditions as set forth above,
// provided the following conditions are met:
// 
//     i) any and all modified files must be affixed with prominent
// notices that you have changed the files and the date that the changes
// occurred.
// 		
//     ii) any work that you distribute, publish, or make available, that
// in whole or in part contains portions of the Program or derivative
// work thereof, must be licensed at no charge to all third parties under
// the terms of this License.
// 
//    iii) if the modified program normally reads commands interactively
// when run, you must cause it, when started running for such interactive
// use in the most ordinary way, to display and/or print an announcement
// with all appropriate copyright notices and disclaimer of warranty as
// set forth in Article 5 and 6 of this License to be clearly displayed.
// In addition, you must provide reasonable access to this License to the
// user.
// 
// Any portion of a Modification that can be reasonably considered
// independent of the Program and separate work in and of itself is not
// subject to the terms and conditions set forth in this License as long
// as it is not distributed with the Program or any portion thereof.
// 
// 
// 2. This License further allows you to copy and distribute the Program
//    or a work based on it, as set forth in Article 1 Section b in
//    object code or executable form under the terms of Article 1 above
//    provided that you also either:
// 
//    i) accompany it with complete corresponding machine-readable source
// code, which must be distributed under the terms of Article 1, on a
// medium customarily used for software interchange; or,
// 
//   ii) accompany it with a written offer, valid for no less than three
// (3) years from the time of distribution, to give any third party, for
// no consideration greater than the cost of physical transfer, a
// complete machine-readable copy of the corresponding source code, to be
// distributed under the terms of Article 1 on a medium customarily used
// for software interchange; or,
// 
// 
// 3.  Export Law Assurance.
// 
// You agree that the Software will not be shipped, transferred or
// exported, directly into any country prohibited by the United States
// Export Administration Act and the regulations thereunder nor will be
// used for any purpose prohibited by the Act.
//  
// 4.  Termination.
// 
// If at anytime you are unable to comply with any portion of this
// License you must immediately cease use of the Program and all
// distribution activities involving the Program or any portion thereof.
// 
// 
// 5.  Disclaimer of Warranties and Limitation on Liability.
// 
// YOU ACCEPT THE PROGRAM ON AN "AS IS" BASIS.  GTRC MAKES NO WARRANTY
// THAT ALL ERRORS CAN BE OR HAVE BEEN ELIMINATED FROM PROGRAM.  GTRC
// SHALL NOT BE RESPONSIBLE FOR LOSSES OF ANY KIND RESULTING FROM THE USE
// OF PROGRAM AND ITS ACCOMPANYING DOCUMENT(S), AND CAN IN NO WAY PROVIDE
// COMPENSATION FOR ANY LOSSES SUSTAINED, INCLUDING BUT NOT LIMITED TO
// ANY OBLIGATION, LIABILITY, RIGHT, CLAIM OR REMEDY FOR TORT, OR FOR ANY
// ACTUAL OR ALLEGED INFRINGEMENT OF PATENTS, COPYRIGHTS, TRADE SECRETS,
// OR SIMILAR RIGHTS OF THIRD PARTIES, NOR ANY BUSINESS EXPENSE, MACHINE
// DOWNTIME OR DAMAGES CAUSED TO YOU BY ANY DEFICIENCY, DEFECT OR ERROR
// IN PROGRAM OR MALFUNCTION THEREOF, NOR ANY INCIDENTAL OR CONSEQUENTIAL
// DAMAGES, HOWEVER CAUSED.  GTRC DISCLAIMS ALL WARRANTIES, BOTH EXPRESS
// AND IMPLIED RESPECTING THE USE AND OPERATION OF PROGRAM AND ITS
// ACCOMPANYING DOCUMENTATION, INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR PARTICULAR PURPOSE AND ANY IMPLIED
// WARRANTY ARISING FROM COURSE OF PERFORMANCE, COURSE OF DEALING OR
// USAGE OF TRADE.  GTRC MAKES NO WARRANTY THAT PROGRAM IS ADEQUATELY OR
// COMPLETELY DESCRIBED IN, OR BEHAVES IN ACCORDANCE WITH ANY
// ACCOMPANYING DOCUMENTATION.  THE USER OF PROGRAM IS EXPECTED TO MAKE
// THE FINAL EVALUATION OF PROGRAM'S USEFULNESS IN USER'S OWN
// ENVIRONMENT.
// 
// GTRC represents that, to the best of its knowledge, the software
// furnished hereunder does not infringe any copyright or patent.
// 
// GTRC shall have no obligation for support or maintenance of Program.
// 
// 6.  Copyright Notice.
// 
// THE SOFTWARE AND ACCOMPANYING DOCUMENTATION ARE COPYRIGHTED WITH ALL
// RIGHTS RESERVED BY GTRC.  UNDER UNITED STATES COPYRIGHT LAWS, THE
// SOFTWARE AND ITS ACCOMPANYING DOCUMENTATION MAY NOT BE COPIED EXCEPT
// AS GRANTED HEREIN.
// 
// You acknowledge that GTRC is the sole owner of Program, including all
// copyrights subsisting therein.  Any and all copies or partial copies
// of Program made by you shall bear the copyright notice set forth below
// and affixed to the original version or such other notice as GTRC shall
// designate.  Such notice shall also be affixed to all improvements or
// enhancements of Program made by you or portions thereof in such a
// manner and location as to give reasonable notice of GTRC's copyright
// as set forth in Article 1.
// 
// Said copyright notice shall read as follows:
// 
// Copyright 2004
// Dr. George F. Riley
// Georgia Tech Research Corporation
// Atlanta, Georgia 30332-0415
// All Rights Reserved
//
// $Id: location.h,v 1.4 2004/09/14 18:18:42 dheeraj Exp $



// Define locations and regions.
// George F. Riley, Georgia Tech, Summer 2003

#ifndef __location_h__
#define __location_h__

#include <vector>

#include "common-defs.h"

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
