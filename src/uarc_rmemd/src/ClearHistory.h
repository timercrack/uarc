/*
 * ClearHistory.h
 *
 *  Created on: 2015-3-2
 *      Author: root
 */

#ifndef CLEARHISTORY_H_
#define CLEARHISTORY_H_
#include "uarc_rmemd.h"
#include "rdbms.h"
#include "ClearNotofication.h"
#include <Poco/Util/Subsystem.h>
#include <Poco/Runnable.h>
#include "Poco/Notification.h"
#include "Poco/NotificationQueue.h"
#include <Poco/Data/Statement.h>

using Poco::NotificationQueue;
using namespace std;


class ClearHistory: public Poco::Util::Subsystem, public Poco::Runnable
{

public:
	ClearHistory();
	virtual ~ClearHistory();

	void initialize(Poco::Util::Application& self);
	void uninitialize();
	void Start();
	void run();
	void Stop();
	const char* name() const;
private:
	bool TimeToChar(int Currenttime, std::string &TimeChar);
	int nextClearTime( time_t current);
	Poco::Notification::Ptr pNf;
	bool _stopped;
	Poco::Thread _thread;
	std::string _rtdbms_redis_ip;
	int _rtdbms_redis_port;
	CRtDbMs* _rdbmsClearHis;
};
#endif /* CLEARHISTORY_H_ */
