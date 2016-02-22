/*
 * CollDataSubsystem.h
 *
 *  Created on: 2014-10-10
 *      Author: root
 */

#ifndef COLLDATASUBSYSTEM_H_
#define COLLDATASUBSYSTEM_H_

#include "rdbms.h"
#include "CommStruct.h"
#include "VIEC104/VIEC104Subsystem.h"
#include "IEC104/IEC104Subsystem.h"
#include "CollTaskSubsystem.h"
#include "uarc_colld.h"
#include <iostream>

#include <Poco/Util/Subsystem.h>
#include <Poco/Runnable.h>
#include <Poco/Thread.h>

using Uarc::Protocol::VIEC104::VIEC104Subsystem;
using Uarc::Protocol::IEC104::IEC104Subsystem;

using namespace std;

class CollDataSubsystem  : public Poco::Util::Subsystem, public Poco::Runnable {

public:

	CollDataSubsystem();
	const char* name() const;
	virtual ~CollDataSubsystem();

	void initialize(Poco::Util::Application& self);
	void uninitialize();
	void Start();
	void run();
	void Stop();

private:

	bool 		_stopped;
	Poco::Thread 	_thread;
	CRtDbMs*        _rtdbms;

};

bool freshCollectTask(const CollTaskData& collTask);

#endif /* COLLDATASUBSYSTEM_H_ */
