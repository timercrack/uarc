/*
 * StoreExeSubsystem.h
 *
 *  Created on: 2015-1-24
 *      Author: root
 */

#ifndef STOREEXESUBSYSTEM_H_
#define STOREEXESUBSYSTEM_H_
#include "rdbms.h"
#include "WaitAllPerArchSubsystem.h"
#include "Store_To_RunTime.h"
#include "StoreNotification.h"

#include "uarc_rmemd.h"
#include <Poco/Util/Subsystem.h>
#include <Poco/Runnable.h>
#include <Poco/Thread.h>
#include <Poco/ThreadPool.h>
#include "Poco/Notification.h"
#include "Poco/NotificationQueue.h"
#include <iostream>
#include <Poco/Mutex.h>
#include "Poco/SharedPtr.h"
#include "Poco/Data/MySQL/Connector.h"
#include "Poco/Data/SessionPool.h"

#include <Poco/Stopwatch.h>
#include <Poco/Data/Statement.h>
#include <sys/time.h>
using Poco::Stopwatch;
using Poco::Timestamp;
using namespace std;
using namespace Poco::Data;
using Poco::Data::Session;
using namespace Poco::Data::Keywords;
using Poco::NotificationQueue;



using Poco::FastMutex;

using namespace std;



class StoreExeSubsystem: public Poco::Util::Subsystem, public Poco::Runnable {

public:

	StoreExeSubsystem();
	const char* name() const;
	virtual ~StoreExeSubsystem();

	void initialize(Poco::Util::Application& self);
	void uninitialize();
	void Start();
	void run();
	void Stop();


	bool StoreExecute();
	bool DOBLSAMPStore(std::string UnitId,int ItemId, int StoreTimeStamp);
	bool RealTimeDBStore(std::vector<RealTimeDataStruct> RealTimeStoreVector, ConntDBSetting  ConnectInfo);
	static std::vector<UarcStorePeriodInfo> PeriodVecor; //归集任务集合


	static FastMutex _mutex_StoreExe;
//	static std::vector<UarcStorePeriodInfo> TimeForCollVecto; //归集周期集合，维护所有归集时间，指导触发归集任务
	std::vector<CollDataStr> FinalStoreValue;
	std::vector<RealTimeDataStruct> RealTimeStoreVector;

	std::string AssembledSql(std::string unitId, int itemId, double val,std::string Storetime, std::string secCollSql);
	std::string AssembledOtherSql(std::string unitId, int itemId, double val,std::string Storetime, std::string secCollOtherSql);

	std::string IntTimeTostring(int InputTime);
	ConntDBSetting        _ConnectInfo;
	GoldenWorks* gld;

private:
	bool DoubleSampleStoreToDB(std::vector<CollDataStr>& FinalStoreValue);
	bool FixedLatestTime(const time_t CurrentTime, int Frequence, int &fixedTimeInt);
	bool Enquene(int Timelabel,int Priod);
	Poco::Notification::Ptr pNf;

	bool _stopped;
	Poco::Thread _thread;

	int _nextStoreTime;
	std::string _rtdbms_ip;
	int _rtdbms_port;
	std::string _connect_ralatinDB_str_gath;
	CRtDbMs* _rdbmsStoreExe;//数据归集任务


	Poco::SharedPtr<Poco::Data::SessionPool> _pStoreDBSessionPool;
	int _StoreminSessions;//sessionPool的最小session数量
	int _StoremaxSessions;//sessionPool的最大session数量
	int _StoreidleTime;//sessionPool的最大session空闲时间

};

#endif /* STOREEXESUBSYSTEM_H_ */
