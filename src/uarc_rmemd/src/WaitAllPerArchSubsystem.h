/*
 * WaitAllPerArchSubsystem.h
 *
 *  Created on: 2015-2-12
 *      Author: root
 */

#ifndef WAITALLPERARCHSUBSYSTEM_H_
#define WAITALLPERARCHSUBSYSTEM_H_
#include "StoreExeSubsystem.h"
#include "uarc_rmemd.h"
#include "rdbms.h"
#include "StoreNotification.h"
#include "RealTimeHeader.h"
#include "Poco/Notification.h"
#include "Poco/NotificationQueue.h"
using namespace std;


class WaitAllPerArchSubsystem: public Poco::Util::Subsystem, public Poco::Runnable
{
public:
	WaitAllPerArchSubsystem();
	virtual ~WaitAllPerArchSubsystem();

	void initialize(Poco::Util::Application& self);
	void uninitialize();
	void Start();
	void run();
	void Stop();
	const char* name() const;
	bool FixedLatestTime(const time_t CurrentTime, const int Frequence,int &fixedTimeInt);
	std::string IntTimeTostring(int InputTime);

private:

	bool _stopped;
	Poco::Thread _thread;
	std::string _rtdbms_redis_ip;
	int _rtdbms_redis_port;
	CRtDbMs* _rdbmsGetPerArch;


};
extern TimedNotificationQueue Timedqueue;
#endif /* WAITALLPERARCHSUBSYSTEM_H_ */
