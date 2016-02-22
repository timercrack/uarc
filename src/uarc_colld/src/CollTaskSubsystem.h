/*
 * CollDataSubsystem.h
 *
 *  Created on: 2014-10-10
 *      Author: root
 */

#ifndef COLLTASKSUBSYSTEM_H_
#define COLLTASKSUBSYSTEM_H_

#include "rdbms.h"
#include "CommStruct.h"
#include "VIEC104/VIEC104Subsystem.h"
#include "IEC104/IEC104Subsystem.h"
#include "CollDataSubsystem.h"
#include "uarc_colld.h"
#include <iostream>

#include <Poco/Util/Subsystem.h>
#include <Poco/Runnable.h>
#include <Poco/Thread.h>

#include <Poco/Mutex.h>
#include <Poco/TimedNotificationQueue.h> 
#include <Poco/Notification.h> 
#include <Poco/DateTime.h>

using Poco::FastMutex;
using Poco::TimedNotificationQueue;
using Poco::Notification;
using Poco::DateTime;

using Uarc::Protocol::VIEC104::VIEC104Subsystem;
using Uarc::Protocol::IEC104::IEC104Subsystem;

using namespace std;

struct TaskItem
{
	TaskItem(string _unitId, int _taskId)
		:unitId(_unitId), taskId(_taskId){}

	string unitId;
	int taskId;
};

class TaskNotification : public Notification
{
public:
	TaskNotification(CollTaskData data, bool init):_init(init), _removed(false), _data(data)
	{
		_mutexMaster.lock();
		if (_master.taskId == -1)
		{
			_master.unitId = data.unitId;
			_master.taskId = data.taskId;
		}
		_mutexMaster.unlock();
	}
	bool isInit(){
		return _init;
	}
	void setInit(bool init){
		_init = init;
	}

	bool isRemoved(){
		FastMutex::ScopedLock lock(_mutexRemoved);
		return _removed;
	}
	void remove(){
		FastMutex::ScopedLock lock(_mutexRemoved);
		_removed = true;
	}
	CollTaskData *data(){return &_data;}
	bool isMaster()
	{
		FastMutex::ScopedLock lock(_mutexMaster);
		if (_master.taskId == -1)
		{
			_master.unitId = _data.unitId;
			_master.taskId = _data.taskId;
		}
		if (_master.unitId == _data.unitId && 
			_master.taskId == _data.taskId)
		{
			return true;
		}else {
			return false;
		}
	}
	void removeMaster()
	{
		FastMutex::ScopedLock lock(_mutexMaster);
		_master.unitId = "";
		_master.taskId = -1;
	}

private:
	bool _init;
	FastMutex _mutexRemoved;
	bool _removed;
	FastMutex _mutexMaster;
	static TaskItem _master;
	CollTaskData _data;
};

class CollTaskSubsystem  : public Poco::Util::Subsystem, public Poco::Runnable {

public:

	CollTaskSubsystem();
	const char* name() const;
	virtual ~CollTaskSubsystem();

	void initialize(Poco::Util::Application& self);
	void uninitialize();
	void Start();
	void run();
	void Stop();

	void setInit(int deviceId, bool init);
	bool freshTask(CollTaskData &task, bool init = true);
	TaskNotification *findTask(TaskItem &item);
	TaskNotification *findTaskNLock(TaskItem &item);
	bool addTask(CollTaskData &task, bool init = true);
	bool addTaskNLock(CollTaskData &task, bool init = true);
	bool removeTask(TaskItem &item);
	bool removeTaskNLock(TaskItem &item);
	bool clearTask();
	bool clearTaskNLock();
	
	bool collectTaskExecute();                       //采集任务执行

	friend bool freshCollectTask(const CollTaskData &collectTask);
private:
	FastMutex _mutexCollectTaskQueue;
	TimedNotificationQueue _collectTaskQueue;
	std::vector<TaskNotification *> _collectTaskNotifications;

	bool 		_stopped;
	Poco::Thread 	_thread;
	CRtDbMs*        _rtdbms;

};

void showTime(time_t t);

#endif /* COLLTASKSUBSYSTEM_H_ */
