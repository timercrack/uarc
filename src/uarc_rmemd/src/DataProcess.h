/*
 * DataProcess.h
 *
 *  Created on: 2014-10-25
 *      Author: root
 */

#ifndef DATAPROCESS_H_
#define DATAPROCESS_H_
#include "rdbms.h"
#include "uarc_rmemd.h"
#include "Store_To_RunTime.h"
#include <map>
#include <sys/types.h>

#include "Poco/SharedPtr.h"
#include "Poco/DateTime.h"
#include "Poco/Data/SessionFactory.h"
#include "Poco/Data/Session.h"
#include "Poco/Data/RecordSet.h"


#include "Poco/Data/SessionPool.h"
#include "Poco/Data/PooledSessionImpl.h"
#include <Poco/Util/Subsystem.h>
#include <Poco/Runnable.h>
#include <Poco/Thread.h>


using namespace std;
using namespace Poco::Data::Keywords;


class DataProcess: public Poco::Util::Subsystem, public Poco::Runnable
{
public:
	DataProcess();
	virtual ~DataProcess();

	void initialize(Poco::Util::Application& self);
	void uninitialize();
	void Start();
	void run();
	void Stop();
	const char* name() const;
	//向平台存入报警日志
	bool StoreWaringLog(std::string UnitId, int ItemId,int termId , int time,
			double Dataval, std::string warnStoreSql);
//	bool StoreToRunTimeDB(UarcProcData Data);
	bool ProcessExe( const UarcProcDataItemArchive procDataItem,const UarcProcData procData );
private:


    bool TimeToChar(int Currenttime, std::string &TimeChar);
	bool _stopped;
	Poco::Thread _thread;
//	CRtDbMs rdbmsProcess;//数据处理任务

	std::string _rtdbms_redis_ip;
	int _rtdbms_redis_port;
	std::string _connect_ralatinDB_str;
	CRtDbMs* _rdbmsDataPro;
	Poco::SharedPtr<Poco::Data::SessionPool> _pDataProDBSessionPool;
	Poco::SharedPtr<Poco::Data::SessionPool> _pDataDBSessionPool;
	int _DataminSessions;//sessionPool的最小session数量
	int _DatamaxSessions;//sessionPool的最大session数量
	int _DataidleTime;//sessionPool的最大session空闲时间

};

//数据处理回调函数
bool DataProcessCallback(const UarcProcDataItemArchive& procDataItem,const UarcProcData& procData);
void showLocalTime();

#endif /* DATAPROCESS_H_ */
