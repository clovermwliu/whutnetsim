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



// Georgia Tech Network Simulator - Layer 5 Application base class
// George F. Riley.  Georgia Tech, Spring 2002

// Define the layer 5 application interface

//2010.01.04 modified by 张笑盈

#ifndef __application_h__
#define __application_h__

#include "G_common_defs.h"
#include "handler.h"
#include "l4protocol.h"
#include "AppModel.h"

// Define the start/stop events
class BasicAppEvent : public Event {
/*
描述：定义应用对应的事件类，实现启动或暂停应用
*/
public:
  typedef enum { START, STOP } BasicAppEvent_t;
  BasicAppEvent() { }
  //构造函数
  BasicAppEvent(Event_t ev) : Event(ev) { }
  //构造函数
};

//Doc:ClassXRef
class Application : public Handler, public Object  {
/*
Application类是所有应用类的基类。定义各种应用类和与此相关的第4层协议的接口。
Application类可以利用多种第4层协议来实现某种应用。
*/
  //Doc:Class Class {\tt Application} is the base class for all
  //Doc:Class \GTNS\ applications.
  //Doc:Class It defines the interface between the application class and
  //Doc:Class the associated layer 4 protocols.  Applications can have one
  //Doc:Class or more layer 4 protocols assigned, to allow (for example)
  //Doc:Class a web browser model with multiple simulataneous connections.
public:
  //Doc:Method
  Application() : deleteOnComplete(false), copyOnConnect(true) { };
  /*
  描述：默认构造函数
  */
    //Doc:Desc The default constructor for the {\tt Application} class.

  //Doc:Method
  Application(const Application&);
    //Doc:Desc Copy constructor, used bt the {\tt Copy()} method for all
    //Doc:Desc applications.
    //Doc:Arg1 Application object to copy.

  virtual ~Application();
  // Starting, Stoping
  //Doc:Method
  void Start(Time_t);
    //Doc:Desc Schedule a {\tt Start} event for this application at the
    //Doc:Desc specified time.
    //Doc:Arg1 The simulation time to start the application.
    //Doc:Arg1 The {\t StartApp} method of the application will be called
    //Doc:Arg1 at the specified time.

  //Doc:Method
  void Stop(Time_t);
    //Doc:Desc Schedule a {\tt Stop} event for this application at the
    //Doc:Desc specified time.
    //Doc:Arg1 The simulation time to stop the application.
    //Doc:Arg1 The {\t StopApp} method of the application will be called
    //Doc:Arg1 at the specified time.

  // Handler functions
  void Handle(Event*, Time_t);

  // Upcalls from L4 protocol
  //Doc:Method
  virtual void Receive(Packet*,L4Protocol*, Seq_t = 0);   // Data received
    //Doc:Desc Called by an associated layer 4 protocol when data 
    //Doc:Desc is received.
    //Doc:Arg1 The packet received, with the layer 4 PDU removed.
    //Doc:Arg2 A pointer to the layer 4 protocol object that recieved the data.
    //Doc:Arg3 Optional sequence number for this data (TCP protocols only)

  //Doc:Method
  virtual void Sent(Count_t, L4Protocol*);     // Data has been sent
    //Doc:Desc Called by an associated layer 4 protocol when all some part 
    //Doc:Desc of the outstanding data has been sent.  For TCP protocols,
    //Doc:Desc this occurs when the acknowledgement is received from the peer.
    //Doc:Arg1 Count of number of bytes successfully sent.
    //Doc:Arg2 A pointer to the layer 4 protocol object that sent the data.

  //Doc:Method
  virtual void CloseRequest(L4Protocol*);      // Close request from peer
    //Doc:Desc Called by an associated layer 4 protocol when a connection
    //Doc:Desc close request has been receivd from a peer.  Applications should
    //Doc:Desc respond by calling the corresponding layer 4 ::Close() routine.
    //Doc:Arg1 A pointer to the layer 4 protocol object that closed.

  //Doc:Method
  virtual void Closed(L4Protocol*);            // Connection has closed
    //Doc:Desc Called by an associated layer 4 protocol when a connection
    //Doc:Desc has completely closed.
    //Doc:Arg1 A pointer to the layer 4 protocol object that closed.

  //Doc:Method
  virtual void ConnectionComplete(L4Protocol*);// Connection request succeeded
    //Doc:Desc Called by an associated layer 4 protocol when a previous
    //Doc:Desc connection request has successfully completed.
    //Doc:Arg1 A pointer to the layer 4 protocol object that connected.

  //Doc:Method
  virtual void ServConnectionComplete(L4Protocol*);// Connection request succeeded
   //Doc:Desc Called by an associated layer 4 protocol when a connection
   //Doc:Desc that the TCP server agent accepted has successfully completed.
   //Doc:Arg1 A pointer to the layer 4 protocol object that connected.



  //Doc:Method
  virtual void ConnectionFailed(L4Protocol*, bool);// Connection request failed
    //Doc:Desc Called by an associated layer 4 protocol when a previous
    //Doc:Desc connection request has failed.
    //Doc:Arg1 A pointer to the layer 4 protocol object that failed.
    //Doc:Arg2 True if aborted, false if timed out

  // Listener has cn rq from peer 
  //Doc:Method
  virtual bool ConnectionFromPeer(L4Protocol*, IPAddr_t, PortId_t);
    //Doc:Desc Called by an associated layer 4 protocol when a listening
    //Doc:Desc TCP protocol has received a connection request.
    //Doc:Arg1 A pointer to the listening TCP protocol object.
    //Doc:Arg2 \IPA\ of remote peer.
    //Doc:Arg3 Port number of report peer.
    //Doc:Return Return true if able to accept a new connection, false if not.

  // Manage the application
  //Doc:Method
  void DeleteOnComplete(bool doc = true) { deleteOnComplete = doc;}
    //Doc:Desc Specifies that this application object should automatically
    //Doc:Desc be deleted when the application has finished.
    //Doc:Arg1 True if delete on complete desired.
  ////指定应用完成后是否需要自动删除该应用对象

  //Doc:Method
  void CopyOnConnect(bool coc)           { copyOnConnect = coc;}
    //Doc:Desc Specifies that this application object should be copied
    //Doc:Desc when connection requests are processed.
    //Doc:Arg1 True if delete on copy on connect desired.
  //指定是否需要拷贝连接

  //Doc:Method
  bool CopyOnConnect() { return copyOnConnect;}
    //Doc:Desc Return current status of copy on connect flag.
    //Doc:Return True if copy on connect, false if not.
  //如果需要拷贝连接，则返回true，否则为false


  //Doc:Method
  virtual void StartApp() { };    // Called at time specified by Start
    //Doc:Desc Called at the specified applcation start time.
  //描述：启动当前应用，由Start方法调用
  

  //Doc:Method
  virtual void StopApp() { };     // Called at time specified by Stop
    //Doc:Desc Called at the specified applcation stop time.
  //描述：停止当前应用，由Stop方法调用

  //Doc:Method
  virtual void AttachNode(Node*); // Note which node attached to
    //Doc:Desc Specify which node to which this application is attached.
    //Doc:Arg1 Node pointer to attached node.

  //Doc:Method
  virtual Application* Copy() const = 0;// Make a copy of the application
  //描述：返回当前应用对象的拷贝
    //Doc:Desc Return a copy of this application. \purev
    //Doc:Return A pointer to a new copy of this application.

  // If the application has a single layer4 protocol, return it
  //Doc:Method
  virtual L4Protocol* GetL4() const { return l4proto;}
  //返回当前应用所使用的第四层协议对象的指针
    //Doc:Desc Returns a pointer to the layer 4 protocol object,
    //Doc:Desc if this application has a single associated l4 protocol.
    //Doc:Return Pointer to the single layer 4 object, or {\tt nil} if 
    //Doc:Return none, or {\tt nil} if more than one.

  void DelL4proto() 
  { 
	 // if ( l4proto!= nil )
		//delete l4proto;
  }

public:
  //Doc:Member
  bool        deleteOnComplete;//应用完成后，如果需要自动删除该连接对象则为true
    //Doc:Desc True if delete on complete requested.

  //Doc:Member
  bool        copyOnConnect;      // True if application copied需要拷贝连接时为true
    //Doc:Desc True if copy on connect requested.


  ////////////////////////////////////////////////////////////////////////////
  //////新增
  ////////////////////////////////////////////////////////////////////////////
public:
  void SetAppModel(const AppModel &);//设置当前应用所用的模型

protected:
  AppModel * pModel;

  Node *node;

  L4Protocol *l4proto;
};

#endif


