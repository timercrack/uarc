/*
 * SendScheduleSubsystem.h
 *
 *  Created on: 2015-5-23
 *      Author: root
 */

#ifndef SENDSCHEDULESUBSYSTEM_H_
#define SENDSCHEDULESUBSYSTEM_H_

#include "uarc_colld.h"
#include "rdbms.h"
#include "CommStruct.h"
#include "VIEC104/VIEC104Subsystem.h"
#include "IEC104/IEC104Subsystem.h"
#include <Poco/Util/Subsystem.h>
#include <Poco/Runnable.h>
#include <Poco/Thread.h>

using Uarc::Protocol::VIEC104::VIEC104Subsystem;
using Uarc::Protocol::IEC104::IEC104Subsystem;

class SendScheduleSubsystem : public Poco::Util::Subsystem, public Poco::Runnable {
public:
	SendScheduleSubsystem();
	const char* name() const;
	virtual ~SendScheduleSubsystem();

	void initialize(Poco::Util::Application& self);
	void uninitialize();
	void Start();
	void run();
	void Stop();

private:

	bool 			_stopped;
	Poco::Thread 	_thread;
	CRtDbMs*        _rtdbms;
};

bool ScheduleCmdTaskRun(const ScheduleInfoData& scheduleTask);

#endif /* SENDSCHEDULESUBSYSTEM_H_ */
