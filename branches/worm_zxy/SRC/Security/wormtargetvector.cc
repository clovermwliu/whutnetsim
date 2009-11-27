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
// $Id: wormtargetvector.cc,v 1.2 2004/09/14 18:18:43 dheeraj Exp $



// Georgia Tech Network Simulator - Worm Target Vector Class
// Monirul I Sharif.  Georgia Tech, Fall 2003

// Define a Worm Application class

//#define DEBUG_MASK 0x1

#include "common-defs.h"
#include "rng.h"
#include "debug.h"
#include "wormtargetvector.h"

#include <math.h>

using namespace std;

WormTargetVector::WormTargetVector(Count_t range) 
  : initialized(false), scanrange(range), localIP(IPADDR_NONE)
{

}

WormTargetVector::WormTargetVector(Mask m)
{
  scanrange = 1<<(32-m.NBits());
}

WormTargetVector* WormTargetVector::Copy() const
{
  return new WormTargetVector(*this);
}

void WormTargetVector::Initialize(IPAddr_t IP)
{
  //cout<<"Initialize called"<<endl;
  localIP = IP;
  initialized = true;
}


IPAddr_t WormTargetVector::Generate()
{
  if (!initialized) 
    cout<<"TargetVector Not Initialized!!"<<endl;
  return IPADDR_NONE;
}

WTVUniform::WTVUniform(Count_t scanrange) : WormTargetVector(scanrange)
{
}


WormTargetVector* WTVUniform::Copy() const
{
  return new WTVUniform(*this);
}

void WTVUniform::Initialize(IPAddr_t IP)
{
  WormTargetVector::Initialize(IP);

  rngT = new Uniform(0, scanrange);
}

IPAddr_t WTVUniform::Generate()
{
  WormTargetVector::Generate();

  return rngT->IntValue();
}


WTVLocalPref::WTVLocalPref(Count_t range, double p,
			   Mask local) : WormTargetVector(range)
{
  localscanrange = 1<<(32-local.NBits());
  prob = p;
}

WTVLocalPref::WTVLocalPref(Mask global, double p,
			   Mask local) : WormTargetVector(global)
{
  localscanrange = 1<<(32-local.NBits());
  prob = p;
}

WormTargetVector* WTVLocalPref::Copy() const
{
  return new WTVLocalPref(*this);
}

void WTVLocalPref::Initialize(IPAddr_t IP)
{
  WormTargetVector::Initialize(IP);

  rngP  = new Uniform(0, 1);
  rngT1 = new Uniform(0, scanrange);
  rngT2 = new Uniform(0, localscanrange);
}


IPAddr_t WTVLocalPref::Generate()
{
  WormTargetVector::Generate();

  IPAddr_t a,b;
  double p = rngP->Value();
  DEBUG0((cout<<"p="<<p<<endl));

  if ( p< prob) {
    a = rngT2->IntValue();
    DEBUG0((cout<<"l="<<(string)IPAddr(a)
	    <<" s="<<scanrange
	    <<" l="<<(string)IPAddr(~(localscanrange-1))<<endl));
    b = localIP&(scanrange-1);
    DEBUG0((cout<<"a="<<(string)IPAddr(b)<<endl));
    b = b&~(localscanrange-1);
    DEBUG0((cout<<"a="<<(string)IPAddr(b)<<endl));
    return a|b;
  } else {
    a = rngT1->IntValue();
    DEBUG0((cout<<"g="<<(string)IPAddr(a)<<endl));
    return a;
  }
}

WTVSequential::WTVSequential(Count_t range) : WormTargetVector(range)
{
}

WTVSequential::WTVSequential(Mask global) : WormTargetVector(global)
{
}

WormTargetVector* WTVSequential::Copy() const
{
  return new WTVSequential(*this);
}

void WTVSequential::Initialize(IPAddr_t IP)
{
  WormTargetVector::Initialize(IP);
  Random *rngT = new Uniform(0, scanrange);

  currentIP = rngT->IntValue();
  delete rngT;
}


IPAddr_t WTVSequential::Generate()
{
  WormTargetVector::Generate();

  IPAddr_t a = currentIP;

  currentIP++;
  if (currentIP>=scanrange)
    currentIP=0;

  return a;
}





WTVSequentialWithLocalPref::WTVSequentialWithLocalPref(Count_t range,
						       double p,
						       Mask local) 
  : WormTargetVector(range)
{
  localscanrange = 1<<(32-local.NBits());
  prob = p;
}

WTVSequentialWithLocalPref::WTVSequentialWithLocalPref(Mask global,
						       double p,
						       Mask local) 
  : WormTargetVector(global)
{
  localscanrange = 1<<(32-local.NBits());
  prob = p;
}


WormTargetVector* WTVSequentialWithLocalPref::Copy() const
{
  return new WTVSequentialWithLocalPref(*this);
}

void WTVSequentialWithLocalPref::Initialize(IPAddr_t IP)
{
  WormTargetVector::Initialize(IP);
  Random *rngP = new Uniform(0,1);
  Random *rngT;

  double p = rngP->Value();
  if (p<prob) {
    rngT = new Uniform(0, localscanrange);
    IPAddr_t a,b;
    a = rngT->IntValue();
    b = localIP&(scanrange-1);
    b = b&~(localscanrange-1);
    currentIP = a|b;
  } else {
    rngT = new Uniform(0, scanrange); 
    currentIP = rngT->IntValue();
  }
  delete rngT;
}


IPAddr_t WTVSequentialWithLocalPref::Generate()
{
  WormTargetVector::Generate();

  IPAddr_t a = currentIP;

  currentIP++;
  if (currentIP>=scanrange)
    currentIP=0;

  return a;
}

////////////////////////////////////////////////////////////////////////
///////新增类
//////////////////////////////////////////////////////////////////////////

WTVVulDistributePref::WTVVulDistributePref(Count_t range,
										   const vector<double> *&InDistribute,
										   bool IsStatic): WormTargetVector(range)
{
	IsStaticPref = IsStatic;
	if (IsStaticPref)//静态扫描
	{	
		ProDistribute = InDistribute;
	}
	else
	{
		VulDistribute = InDistribute;
	}

}
WTVVulDistributePref::WTVVulDistributePref(Mask m,
										   const vector<double> *&InDistribute,
										   bool IsStatic): WormTargetVector(m)
{
	IsStaticPref = IsStatic;
	if (IsStaticPref)//静态扫描
	{	
		ProDistribute = InDistribute;
	}
	else
	{
		VulDistribute = InDistribute;
	}

}


WormTargetVector* WTVVulDistributePref::Copy() const
{
	return new WTVVulDistributePref(*this);
}

void WTVVulDistributePref::Initialize(IPAddr_t IP)
{
	WormTargetVector::Initialize(IP);
	
	//检查数据合法性
	if (IsStaticPref)
	{
		GroupNum = ProDistribute->size();
		int sum = 0;
		for (int i = 0; i < (int)GroupNum; ++i)
		{
			sum += ProDistribute->at(i);
		}
		if (sum != 1)
		{
			DEBUG0((cout << "扫描各组的概率之和不为1" << endl));
		}
	}
	else
	{
		GroupNum = VulDistribute->size();
		int sum = 0;
		for (int i = 0; i < (int)GroupNum; ++i)
		{
			sum += VulDistribute->at(i);
		}
		if (sum != 1)
		{
			DEBUG0((cout << "脆弱主机分布之和不为1" << endl));
		}
	}
	GroupSize = scanrange/GroupNum;

}

IPAddr_t WTVVulDistributePref::Generate()
{
	int GroupIndex;//所选中的组号
	IPAddr SelectIP;//所选择的IP

	if(!IsStaticPref)//动态扫描
	{
		Count_t timeSeq;
		Count_t scanrate;
		////ConfigCache c;
		////timeSeq = (获取当前时间 - 获取开始时间)/时间粒度; //获取已经经过的时间拍
		////sancrate = c.GetScanRate();
		vector<double> tmpProDistribute;
		int PrefFactor;
		////根据timeSeq、scanrate和解析出来的公式，计算PrefFactor
		int sum = 0;
		for (int i = 0; i < (int)GroupNum; ++i)
		{
			sum += pow(VulDistribute->at(i), PrefFactor);
		}
		for (int i = 0; i < (int)GroupNum; ++i)
		{
			tmpProDistribute.push_back(pow(VulDistribute->at(i), PrefFactor)/sum);
		}
		GroupIndex = SpecProRandom(&tmpProDistribute);
	}
	else//静态扫描
	{
		GroupIndex = SpecProRandom(ProDistribute);
	}
	//在被选择的组里随机选择某一IP，作为返回值，即下一拍扫描的主机的IP
	Random * rngT = new Uniform(GroupIndex*GroupSize, (GroupIndex+1)*GroupSize);
	SelectIP = rngT->IntValue();
	delete rngT;

	return SelectIP;
}

int 
WTVVulDistributePref::SpecProRandom(const vector<double> * pro)
//按制定概率随机选择各概率所在的下标，返回下标
{
	Random * rngT = new Uniform();
	int tmp_ran = rngT->IntValue();
	delete rngT;
	double tmp_sum = 0;
	for (int i = 0; i < (int)pro->size(); ++i)
	{
		tmp_sum += pro->at(i);
		if (tmp_ran < tmp_sum)
		{
			return i;
		}
	}

}




