//Copyright (c) 2010, Information Security Institute of Wuhan Universtiy(ISIWhu)
//All rights reserved.
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
//Construct Data:
//Modify Author:
//Author Organization:
//Modify Data:



// Georgia Tech Network Simulator - Base class for mobility models
// George F. Riley.  Georgia Tech, Spring 2002

// Define the base class for all mobility models

#include <math.h>
#include <iostream>

#include "mobility.h"
#include "node.h"

using namespace std;

// Copy constructor
Mobility::Mobility(const Mobility& c)
    : lastTime(c.lastTime), velocityX(c.velocityX), velocityY(c.velocityY),
      velocityZ(c.velocityZ), paused(false), first(true)
{
}

void Mobility::ComputeVelocities(Location& n, Location& o, Meters_t v)
{ // Compute x/y velocities from location o to location n at velocity v
  /* 2-dim
  double theta = atan2(n.Y() - o.Y(), n.X() - o.X()); // Angle
  velocityX = v * cos(theta); // X velocity
  velocityY = v * sin(theta); // Y velocity
  */

  double d_xy = sqrt((n.X() - o.X()) * (n.X() - o.X()) +
                     (n.Y() - o.Y()) * (n.Y() - o.Y()));
  double phi = atan2(n.Z() - o.Z(), d_xy); // Angle
  double theta = atan2(n.Y() - o.Y(), n.X() - o.X()); // Angle
  double v_xy = v * cos(phi);
  velocityX = v_xy * cos(theta); // X velocity
  velocityY = v_xy * sin(theta); // Y velocity
  velocityZ = v * sin(phi);      // Z velocity
}

void Mobility::Move(Node* n, Time_t t)
{ // Move for t seconds at current velocities
  Location l = n->GetLocation();
  n->SetLocation(l.X() + t * velocityX,
                 l.Y() + t * velocityY,
                 l.Z() + t * velocityZ);
}

bool Mobility::IsMoving() const
{
  return !paused;
}

Angle_t Mobility::Direction() const
{
  DEBUG0((cout << "MSWP::Direction, paused " << paused << endl));
  if (paused) return 0; // Nothing if paused
  DEBUG0((cout << "Mobility::Direction, nextWP.y " << nextWP.location.Y()
          << " lastWP.y " << lastWP.location.Y()
          << " nextWP.x " << nextWP.location.X()
          << " lastWP.x " << lastWP.location.X() << endl));
  return atan2(nextWP.location.Y() - lastWP.location.Y(),
               nextWP.location.X() - lastWP.location.X());
}

// Static members
Meters_t Mobility::Distance(Location& n, Location& o)
{
  return sqrt((n.X() - o.X()) * (n.X() - o.X()) +
              (n.Y() - o.Y()) * (n.Y() - o.Y()) +
              (n.Z() - o.Z()) * (n.Z() - o.Z())); // distance
}
