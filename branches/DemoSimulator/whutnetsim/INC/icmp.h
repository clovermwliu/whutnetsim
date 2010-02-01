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
// $Id: icmp.h,v 1.5 2005/11/15 20:48:03 riley Exp $



// Georgia Tech Network Simulator - ICMP Class
// George F. Riley.  Georgia Tech, Summer 2004

#ifndef __icmp_h__
#define __icmp_h__

#include <vector>
#include <map>

#include "pdu.h"
#include "l4demux.h"
#include "l4protocol.h"
#include "datapdu.h"

class Node;
class IPV4Header;
class ICMPHeader;
class Application;
class Packet;
class Data;
class L3Protocol;
class L3PDU;
class L4PDU;

// Define a data structure to manage ICMP callbacks
typedef std::vector<Application*> ApplVec_t;
typedef std::map<Node*, ApplVec_t> ICMPCallback_t;

//Doc:ClassXRef
class ICMP : public L4Demux {
  //Doc:Class Class {\tt ICMP} defines a model of the 
  //Doc:Class Internet Control Message Protocol.
  //Doc:Class {\tt ICMP} inherits from {\tt L4Demux} even though
  //Doc:Class it's conceptually a Layer 3 protocol; however it does use the
  //Doc:Class services of IPV4.
public:
  typedef enum { ProtoNumber =  1 } ICMPProto_t; // ICMP Layer4 Protocol number
  // Types of ICMP messages
  typedef enum {
      ECHO_REPLY = 0,
      DESTINATION_UNREACHABLE = 3,
      SOURCE_QUENCH = 4,
      REDIRECT = 5,
      ECHO = 8,
      TIME_EXCEEDED = 11,
      PARAMETER_PROBLEM = 12,
      TIMESTAMP = 13,
      TIMESTAMP_REPLY = 14,
      INFORMATION_REQUEST = 15,
      INFORMATION_REPLY = 16 } ICMPMsg_t;

  // Various codes
  typedef enum {
      NET_UNREACHABLE,
      HOST_UNREACHABLE,
      PROTOCOL_UNREACHABLE,
      PORT_UNREACHABLE,
      CANT_FRAGMENT,
      SOURCE_ROUTE_FAILED } DU_t; // Destination Unreachable codes
  typedef enum {
      TTL_EXCEEDED,
      REASSEMBLY_EXCEEDED } TE_t; // Time exceeded codes
  
  // Tracing options
  typedef enum { TYPE, CODE, IDENTIFIER, SEQUENCE,
                 ORIGIN_TIME, RECEIVE_TIME, TRANSMIT_TIME } ICMPTrace_t;

  // Constructors
  //Doc:Method
  ICMP();
    //Doc:Desc Default constructor, no arguments.

  // Destructor
  virtual ~ICMP(); 
public:
  // L4Demux methods
  // Upcall From lower layer
  void DataIndication(Node*, Packet*, IPAddr_t, L3Protocol&, Interface*); 
  // Static Methods
  static void Enable(bool e = true); // Enable/Disable ICMP for this simulation
  static void EnableQuench(bool e = true);  // Enable/Disable Quench  messages
  static ICMP* Instance() {return instance;}// Get a pointer to ICMP instance

  // Methods for each ICMP message generation, used mostly by IPV4
  static void DestinationUnreachable(Node*, Packet*, IPV4Header*, L4PDU*,DU_t);
  static void TimeExceeded(Node*, Packet*, IPV4Header*, TE_t);
  // ParameterProblem not implemented
  static void SourceQuench(Node*, Packet*, IPV4Header*);
  // Redirect not implemented
  static void EchoReply(Node*, Packet*, IPV4Header*, ICMPHeader*);
  static void TimestampReply(Node*, Packet*, IPV4Header*, ICMPHeader*);
  // InformationRequest not implemented
  
  // Static methods used by applications to generate ICMP requests
  static void Echo(Node*, ICMPHeader*, IPAddr_t); // Generate an echo message
  static void Timestamp(Node*, ICMPHeader*, IPAddr_t);//Generate timestamp req

  // Request a callback on ICMP message receipt
  static void AddCallback(Node*, Application*);
private:
  // Private methods
  void KillPendingConnection(Node*, IPV4Header*, L4PDU*);
private:
  // Static members
  static bool enabled;   // Default false;
  static bool quench;    // Default false
  static ICMP* instance; // Points to one and only instance of ICMP
  static ICMPCallback_t callbacks;  // List of Node/Application callbacks
};


//Doc:ClassXRef
class ICMPHeader : public L4PDU {
public :
  ICMPHeader();
  ICMPHeader(ICMP::ICMPMsg_t t);
  ICMPHeader(const ICMPHeader&);
  Size_t  Size() const;
  PDU*    Copy() const;
  Proto_t Proto() const { return 1;}   // ICMP is protocol 1
  // Serialization
  Size_t  SSize();                     // Size needed for serialization
  char*   Serialize(char*, Size_t&);   // Serialize to a buffer
  char*   Construct(char*, Size_t&);   // Construct from buffer
  void    Trace(Tfstream&, Bitmap_t, // Trace the contents of this pdu
                Packet* = nil, const char* = nil);
public:
  ICMP::ICMPMsg_t type;
  unsigned long   code;
  Count_t         identifier;
  Count_t         sequence;
  Time_t          originateTimestamp;
  Time_t          receiveTimestamp;
  Time_t          transmitTimestamp;
  IPV4Header*     ipv4Hdr;
  L4PDU*          l4Hdr;
};


#endif
