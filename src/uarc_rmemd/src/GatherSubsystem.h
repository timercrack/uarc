/*
 * GatherSubsystem.h
 *
 *  Created on: 2014-10-21
 *      Author: root
 */

#ifndef GATHERSUBSYSTEM_H_
#define GATHERSUBSYSTEM_H_

#include "uarc_rmemd.h"
#include "RealTimeHeader.h"
#include"rdbms.h"
#include <map>
#include <sys/types.h>

#include <Poco/Mutex.h>
#include "Poco/SharedPtr.h"
#include "Poco/DateTime.h"
//#include "Poco/Data/RecordSet.h"
#include "Poco/Data/SessionPool.h"

using namespace std;
using namespace Poco::Data::Keywords;
using Poco::FastMutex;

class GatherSubsystem: public Poco::Util::Subsystem, public Poco::Runnable {
public:
	GatherSubsystem();
	virtual ~GatherSubsystem();

	void initialize(Poco::Util::Application& self);
	void uninitialize();
	void Start();
	void run();
	void Stop();
	const char* name() const;

	bool FixedLatestTime(const time_t CurrentTime, const int Frequence,int &fixedTimeInt);
	std::string IntTimeTostring(int InputTime);



private:

	CRtDbMs*         _rdbmsGath;
	bool             _stopped;
	Poco::Thread     _thread;
	std::string      _rtdbms_ip;
	int             _rtdbms_port;



};

//规集任务回调函数
bool SecCollPeriodCallback(const UarcPeriodStateOfArch& periodInfo);

#endif /* GATHERSUBSYSTEM_H_ */
