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
// $Id: peachqueue.h,v 1.1 2006/01/20 17:37:47 riley Exp $

// Georgia Tech Network Simulator - PeachQueue class
// George F. Riley.  Georgia Tech, Fall 2005

// Implements the "low-priority" queueing for TCP-Peach dummy packets

#ifndef __peachqueue_h__
#define __peachqueue_h__

#include "droptail.h"

//Doc:ClassXRef
class PeachQueue : public DropTail {
//Doc:Class The class {\tt PeachQueue} implements the low--priority
//Doc:Class queuing for TCP Peach "dummy" packets.  THis simply
//Doc:Class drops any packet being enqued if the "dummy" bit is 
//Doc:Class set (in the TOS IP header field) and if the queue has more than
//Doc:Class one packet presently enqued.
public:
  //Doc:Method
  PeachQueue();

  // Destructor
  virtual ~PeachQueue() { }

  //Doc:Method
  virtual bool Enque(Packet*);    // Return true if enqued, false if not
  //Doc:Desc This method is used to enque a packet in the queue.
  //Doc:Desc Enque will always file if current queue length is 
  //Doc:Desc more than one and the packet is a dummy packet.  See
  //Doc:Desc TCPPeach for info on dummy packets.
  //Doc:Arg1 the pointer to the packet to be added to the queue
  //Doc:Return a boolean to indicate if the enque operation succeeded

};

#endif



