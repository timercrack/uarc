/*
 * RecvDataSubsystem.h
 *
 *  Created on: 2014-10-10
 *      Author: root
 */

#ifndef RECVDATASUBSYSTEM_H_
#define RECVDATASUBSYSTEM_H_

#include "uarc_colld.h"
#include "rdbms.h"
#include "CommStruct.h"
#include "VIEC104/VIEC104Subsystem.h"
#include "IEC104/IEC104Subsystem.h"
#include "SendZhaoceSubsystem.h"

#include <Poco/Util/Subsystem.h>
#include <Poco/Thread.h>

using Uarc::Protocol::VIEC104::VIEC104Subsystem;
using Uarc::Protocol::IEC104::IEC104Subsystem;

class RecvDataSubsystem  : public Poco::Util::Subsystem{

public:

	RecvDataSubsystem();
	const char* name() const;
	virtual ~RecvDataSubsystem();

	void initialize(Poco::Util::Application& self);
	void uninitialize();
	void Start();
	void Stop();

	static CRtDbMs*        _rtdbms;

private:

	bool 			       _stopped;
	Poco::Thread 	       _thread;

};

void receive(int deviceId,int reason,const std::vector<DataItem> &dataItems);

#endif /* RECVDATASUBSYSTEM_H_ */
