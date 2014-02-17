/******* COPYRIGHT ************************************************
*                                                                 *
*                             FlowVR                              *
*                     Daemon and Base Plugins                     *
*                                                                 *
*-----------------------------------------------------------------*
* COPYRIGHT (C) 2003-2011                by                       *
* INRIA and                                                       *
* Laboratoire d'Informatique Fondamentale d'Orleans               *
* (FRE 2490) ALL RIGHTS RESERVED.                                 *
*                                                                 *
* This source is covered by the GNU GPL, please refer to the      *
* COPYING file for further information.                           *
*                                                                 *
*-----------------------------------------------------------------*
*                                                                 *
*  Original Contributors:                                         *
*    Jeremie Allard,                                              *
*    Ronan Gaugne,                                                *
*    Valerie Gouranton,                                           *
*    Loick Lecointre,                                             *
*    Sebastien Limet,                                             *
*    Bruno Raffin,                                                *
*    Sophie Robert,                                               *
*    Emmanuel Melin.                                              *
*                                                                 * 
*******************************************************************
*                                                                 *
* File: src/plugins/flowvr.plugins.FilterStamp.cpp                *
*                                                                 *
* Contacts:                                                       *
*  12/10/2003 Jeremie Allard <Jeremie.Allard@imag.fr>             *
*                                                                 *
******************************************************************/
#include "flowvr/daemon.h"
#include "flowvr/plugins/filter.h"
#include "flowvr/plugd/dispatcher.h"
#include "flowvr/plugd/messagequeue.h"
#include "flowvr/mem/sharedmemorymanager.h"
#include <iostream>
#include <sstream>
#include <unistd.h>

namespace flowvr
{

namespace plugins
{

using namespace flowvr::plugd;

/// \brief Transmits messages selected by a stamp in the order
/// messages (seems the same as FilterIt).
///
/// <b>Init parameters:</b> none.
///
/// <b>Input ports:</b>
/// -  <b>in</b>: Messages to be filtered.
/// -  <b>order</b>: Filtering orders (from a synchronizer such as flowvr::plugins::GreedySynchronizor).
///
/// <b>Output Ports:</b>
/// - <b>out</b>: Filtered messages.

class FilterStamp : public Filter
{
 public:

  FilterStamp(const std::string &objID);

  virtual ~FilterStamp();

  virtual Class* getClass() const;

  virtual flowvr::plugd::Result init(flowvr::xml::DOMElement* xmlRoot, flowvr::plugd::Dispatcher* dispatcher);

  virtual void newMessageNotification(int mqid, int msgnum, const Message& msg, Dispatcher* dispatcher);
  virtual void newStampListSpecification(int mqid, const Message& msg, Dispatcher* dispatcher);

  enum {
    IDPORT_IN=0,
    IDPORT_ORDER,
    NBPORTS
  };

  int numout;

protected:
  virtual void sendPendingOrders(plugd::Dispatcher* dispatcher);

  TypedTrace<int> traceout;

  std::string stampname;
  StampInfo* stamp;
};

using namespace flowvr::xml;

/// Constructor.
FilterStamp::FilterStamp(const std::string &objID)
  : Filter(objID), numout(0), traceout(TypedTrace<int>("out")), stamp(NULL)
{
}

FilterStamp::~FilterStamp()
{
}

flowvr::plugd::Result FilterStamp::init(flowvr::xml::DOMElement* xmlRoot, flowvr::plugd::Dispatcher* dispatcher)
{
  flowvr::plugd::Result result = Filter::init(xmlRoot, dispatcher);
  if (result.error()) return result;

  xml::DOMNodeList* lstamp = xmlRoot->getElementsByTagName("stamp");
  if (lstamp->getLength()>=1)
    stampname = lstamp->item(0)->getTextContent();
  else
    stampname = "it";
  delete lstamp;

  initInputs(NBPORTS);
  inputs[IDPORT_IN]->setName("in");
  inputs[IDPORT_ORDER]->setName("order");


  //initialization of the OMQ "out"
  initOutputs(1);
  outputs[0]->setName("out");
  outputs[0]->msgtype = Message::FULL;

  outputTraces.push_back(&traceout);
  
  // Jeremie: Why?
  result.setXML(xmlRoot);

  return result;
}

void FilterStamp::newMessageNotification(int mqid, int msgnum, const Message& msg, Dispatcher* dispatcher)
{
#ifdef DEBUG
  if (mqid == IDPORT_IN)
    std::cout << objectID()<<": new input "<<msgnum<<" queue size "<<inputs[mqid]->size()<<std::endl;
  else
    std::cout << objectID()<<": new order "<<msgnum<<" queue size "<<inputs[mqid]->size()<<std::endl;
#endif
  sendPendingOrders(dispatcher);
}

void FilterStamp::newStampListSpecification(int mqid, const Message& msg, Dispatcher* dispatcher)
{
  if (mqid == IDPORT_IN) {
    stamp = inputs[IDPORT_IN]->getStampList()[stampname];
    if (stamp == NULL)
      std::cerr << objectID() << ":in ERROR stamp "<<stampname<<" not found."<<std::endl;
#ifdef DEBUG
    else
      std::cout << objectID() << ":in : stamp "<<stampname<<" @ "<<stamp->getOffset()<<std::endl;
#endif
  }

  if (mqid == IDPORT_IN)
  { // forward specification to out port
#ifdef DEBUG
    std::cout << objectID()<<": forwarding stamps specification"<<std::endl;
#endif
    outputs[0]->stamps = inputs[IDPORT_IN]->getStampList();
    outputs[0]->newStampSpecification(dispatcher);

    sendPendingOrders(dispatcher);
  }
}

void FilterStamp::sendPendingOrders(plugd::Dispatcher* dispatcher)
{ // MAIN FILTER FUNCTION

  if (!inputs[IDPORT_IN]->stampsReceived()) return; // still waiting for stamps specification

  if (stamp == NULL)
  {
    std::cerr << objectID() << " waiting for good stamps"<<std::endl;
    return; // bad stamps
  }
  
  int stampval,stampin;
  for (;;)
  {
    Message msg;
    int num;
    {
      //ipc::ScopedMTLock locker(globalLock,"sendPendingOrders");
      if (!inputs[IDPORT_ORDER]->frontMsg().valid())
      {
#ifdef DEBUG
	std::cout<<objectID()<<": waiting orders"<<std::endl;
#endif
	return;
      }

      msg = inputs[IDPORT_ORDER]->frontMsg();

      // Useless?
      msg.stamps.read(inputs[IDPORT_ORDER]->getStampList().num,num);
      
      msg.stamps.read(*stamp,stampval);
      bool scratch = (stampval<0);
      stampval = (stampval<0?-stampval:stampval)-10;
      stampin = -10;
      while(!inputs[IDPORT_IN]->empty()
	    && inputs[IDPORT_IN]->frontMsg().valid()
	    && inputs[IDPORT_IN]->frontMsg().stamps.read(*stamp,stampin)
	    && stampin<stampval)
        inputs[IDPORT_IN]->eraseFront();

      if (scratch)
      {
	inputs[IDPORT_ORDER]->eraseFront();
	//std::cout << objectID() << "scratched up to "<<it<<std::endl;
	continue; // do not send any message, just clean the queue
      }
      if (stampin>=stampval)
        msg = inputs[IDPORT_IN]->frontMsg();
      else
      {
#ifdef DEBUG
	std::cout<<objectID()<<": waiting message "<<stampval<<std::endl;
#endif
        return; // missing message;
      }
      inputs[IDPORT_ORDER]->eraseFront();
      num = numout++;
    }
#ifdef DEBUG
    std::cout<<objectID()<<": sending message "<<num<<std::endl;
#endif
    MessagePut newmsg;
    newmsg.stamps.clone(msg.stamps,&inputs[IDPORT_IN]->getStampList());
    newmsg.data=msg.data;

    traceout.write(stampval);
    outputs[0]->put(newmsg,dispatcher,num);
  }
}

flowvr::plugd::GenClass<FilterStamp> FilterStampClass("flowvr.plugins.FilterStamp", // name
						      "", // description
						      &flowvr::plugins::FilterClass
						      );

Class* FilterStamp::getClass() const
{
  return &FilterStampClass;
}

} // namespace plugins

} // namespace flowvr
