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

/************************************************************************/
/* 2010.1.4  ����Ƽ                                                     */
/***********************************************************************/

#ifndef __interface_basic_h__
#define __interface_basic_h__

#include "G_common_defs.h"
#include "handler.h"
#include "node.h"

class EventCQ;
class Queue;
class Link;

class InterfaceBasic : public Handler
//����interface�Ļ���
{
public:
//��������
  InterfaceBasic();
  InterfaceBasic(Node*);
  virtual ~InterfaceBasic();

  Node* GetNode() const { return pNode;}//�����뱾�ӿڰ󶨵Ľ��
  virtual void SetNode(Node* n) { pNode = n;}//�����뱾�ӿڰ󶨵Ľ��
  virtual Link* GetLink() const { return pLink;} //�����뱾�ӿڰ󶨵���·
  virtual void SetLink(Link* l) { pLink = l;}//�����ڱ��ӿڰ󶨵���·
  virtual Queue* GetQueue() const = 0;//�����뱾��������Ķ��ж���
  virtual void SetQueue(const Queue& q) = 0;//����һ���µĶ��ж���

  virtual void Neighbors(NodeWeightVec_t&, bool = false) = 0;//ͨ����ѯ�뱾�ӿ���������·����ȡ·�����ڽӽ��ĸ���

  virtual void ScheduleRx(LinkEvent*, Time_t, bool = true);//�ڱ����Ƚ��ȳ������м�¼�������ݰ��¼�

  EventCQ* GetEventCQ() { return evList; }//���ر��ؽӿڵ��¼��б�

//�ӿ�״̬����
  bool IsDown() { return down; } //���ؽӿڵ�״̬
  void Down(bool noReInit = false);//�ӿ�״̬��Ϊ�ر�
  void Up();//�ӿ�״̬��Ϊ����
  
protected:
  Node* pNode;//�뱾�ӿڰ󶨵Ľ��
  Link* pLink;//�뱾�ӿڰ󶨵���·
  EventCQ* evList;//�¼��б�
  bool down;//�ӿ�״̬��True��ʾ�ӿڹر�
  bool used; //�¼��б��Ƿ�ʹ��
};

#endif
