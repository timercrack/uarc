/*
 * CollTaskSubsystem.cpp
 *
 *  Created on: 2014-10-10
 *      Author: root
 */

#include "CollTaskSubsystem.h"

TaskItem TaskNotification::_master("", -1);
CollTaskSubsystem *g_pCollTaskSubsystem = NULL;
CollTaskSubsystem::CollTaskSubsystem()  :
_stopped(true), _thread() {
	// TODO Auto-generated constructor stub
	if (g_pCollTaskSubsystem == NULL)
	{
		g_pCollTaskSubsystem = this;
	}else {
		UARCCollServer::GetLogger().information("重复构造采集任务子系统[%s]!", std::string(name()));
	}

}

CollTaskSubsystem::~CollTaskSubsystem() {
	// TODO Auto-generated destructor stub

}

void CollTaskSubsystem::initialize(Poco::Util::Application & self)
{
	// TODO 初始化rtdbms对象
	std::string   rtdbms_ip   = self.config().getString("RedisDB.Base.IPAddr", "127.0.0.1");
	int           rtdbms_port = self.config().getInt("RedisDB.Base.Port", 6379);
    	_rtdbms = new  CRtDbMs(rtdbms_ip,rtdbms_port);

}

void CollTaskSubsystem::uninitialize()
{
	delete _rtdbms;
	_rtdbms = NULL;
}

void CollTaskSubsystem::Start()
{
	poco_assert (_stopped);
	_stopped = false;
	_thread.start(*this);
	UARCCollServer::GetLogger().information("启动采集任务子系统[%s]!", std::string(name()));

}

const char *CollTaskSubsystem::name() const
{
	return "CollTaskSubsystem";
}

bool CollTaskSubsystem::freshTask(CollTaskData &task, bool init)
{
	FastMutex::ScopedLock lock(_mutexCollectTaskQueue);
	TaskItem item(task.unitId, task.taskId);
	if (findTaskNLock(item))
	{
		if (removeTaskNLock(item))
		{
			addTaskNLock(task, init);
		}else {
			return false;
		}
	}else {
		addTaskNLock(task, init);
	}

	return true;
}

void CollTaskSubsystem::setInit(int deviceId, bool init)
{
	FastMutex::ScopedLock lock(_mutexCollectTaskQueue);

	for (int i = 0; i < (int)_collectTaskNotifications.size(); i++)
	{
		if (!_collectTaskNotifications[i]->isRemoved() &&
			_collectTaskNotifications[i]->data()->deviceId == deviceId &&
			_collectTaskNotifications[i]->isInit())
		{
			_collectTaskNotifications[i]->setInit(init);
		}
	}
}

TaskNotification *CollTaskSubsystem::findTask(TaskItem &item)
{
	FastMutex::ScopedLock lock(_mutexCollectTaskQueue);
	return findTaskNLock(item);
}

TaskNotification *CollTaskSubsystem::findTaskNLock(TaskItem &item)
{
	for (int i = 0; i < (int)_collectTaskNotifications.size(); i++)
	{
		if (!_collectTaskNotifications[i]->isRemoved() &&
			_collectTaskNotifications[i]->data()->unitId == item.unitId && 
			_collectTaskNotifications[i]->data()->taskId == item.taskId)
		{
			return _collectTaskNotifications[i];
		}
	}

	return NULL;
}

bool CollTaskSubsystem::addTask(CollTaskData &task, bool init)
{
	FastMutex::ScopedLock lock(_mutexCollectTaskQueue);
	return addTaskNLock(task, init);
}

bool CollTaskSubsystem::addTaskNLock(CollTaskData &task, bool init)
{
	if(task.status != COLLTASK_ST_INVALID)
	{
		long int nextTime = -1;
		if (task.startDate == 0 ||
			task.lastCollTime == 0)
		{
			nextTime = -1;
		}else {
			nextTime = task.startDate + task.exeRound * task.freq;
		}

		DateTime dateTime;
		if (dateTime.timestamp().epochTime() <= nextTime)
		{
			dateTime += (nextTime-dateTime.timestamp().epochTime())*1000000;
		}else {
			dateTime += 1*1000000;
		}

		TaskNotification *notify = new TaskNotification(task, init);
		_collectTaskQueue.enqueueNotification(notify, dateTime.timestamp());
		_collectTaskNotifications.push_back(notify);

		return true;
	}
	
	return false;
}

bool CollTaskSubsystem::removeTask(TaskItem &item)
{
	FastMutex::ScopedLock lock(_mutexCollectTaskQueue);
	return removeTaskNLock(item);
}

bool CollTaskSubsystem::removeTaskNLock(TaskItem &item)
{
	for (int i = 0; i < (int)_collectTaskNotifications.size(); i++)
	{
		if (_collectTaskNotifications[i]->data()->unitId == item.unitId && 
			_collectTaskNotifications[i]->data()->taskId == item.taskId)
		{
			_collectTaskNotifications[i]->remove();
			if (_collectTaskNotifications[i]->isMaster())
			{
				_collectTaskNotifications[i]->removeMaster();
			}
			_collectTaskNotifications.erase(_collectTaskNotifications.begin()+i);
			return true;
		}
	}

	return false;
}

bool CollTaskSubsystem::clearTask()
{
	FastMutex::ScopedLock lock(_mutexCollectTaskQueue);
	return clearTaskNLock();
}

bool CollTaskSubsystem::clearTaskNLock()
{
	if (_stopped)
	{
		_collectTaskQueue.clear();
		_collectTaskNotifications.clear();
		return true;
	}	

	return false;
}

void CollTaskSubsystem::run()
{
	std::vector<CollTaskData> collectTask;

	// TODO 初始化采集任务集合
	while (!_rtdbms->getAllCollTask(&collectTask))
	{
		UARCCollServer::GetLogger().information("获取所有采集任务失败!");
		sleep(2);
	}

	// TODO 去掉采集任务集合中的无效状态任务
	std::vector<CollTaskData> ::iterator it_collTask;
	int count = 0;
	for(it_collTask=collectTask.begin(); it_collTask != collectTask.end(); it_collTask++ ){
		if (freshTask(*it_collTask,false))
		{
			count++;
		}
	}
	UARCCollServer::GetLogger().information("获取采集任务共%d条", count);

	while (!_stopped) {

		// TODO 采集任务循环执行
		collectTaskExecute();
     }
}

bool CollTaskSubsystem::collectTaskExecute()
{
	
	UARCCollServer::GetLogger().information("采集任务队列中有%d个采集任务！", _collectTaskQueue.size());
	Notification::Ptr pNf(_collectTaskQueue.waitDequeueNotification());

	if (_stopped)
	{
		return false;
	}
	TaskNotification* pSNf = pNf.cast<TaskNotification> ();
	if (pSNf == NULL)
	{
		UARCCollServer::GetLogger().information("内部错误!");
		return false;
	}
	if (pSNf->isRemoved())
	{
		return false;
	}
	CollTaskData *pCollTask = pSNf->data();

	// TODO 获取该任务对应的指标集合
	CollExecInfoData collExecInfo;
	bool bCollItem = _rtdbms->getCollExecInfo(pCollTask->unitId,pCollTask->taskId,&collExecInfo);
	if(!bCollItem)
	{
		UARCCollServer::GetLogger().information("获取采集任务[UnitID:%s, TaskID:%d]的指标失败", pCollTask->unitId, pCollTask->taskId);
		TaskItem item(pCollTask->unitId, pCollTask->taskId);
		removeTask(item);
		return false;
	}

	// TODO 再次入队等待下次触发执行
	DateTime dateTime;
	if (pCollTask->startDate == 0 || 
		pCollTask->lastCollTime == 0)
	{
		pCollTask->startDate = dateTime.timestamp().epochTime();
		pCollTask->exeRound = 0;
	}
	pCollTask->status = COLLTASK_ST_EXECING;
	pCollTask->exeRound++;
	pCollTask->lastCollTime = dateTime.timestamp().epochTime();

	if (!freshTask(*pCollTask, false))
	{
		UARCCollServer::GetLogger().information("重新加入采集任务[UnitID:%s, TaskID:%d]失败", pCollTask->unitId, pCollTask->taskId);
		return false;
	}
	
	// TODO 填充待下发的任务指标参数等
	DataItem dataItem;
	std::vector<DataItem>  dataItems;
	std::map<int,int> ::iterator it_itemID;
	for(it_itemID = collExecInfo.itemAndTerms.begin(); it_itemID!= collExecInfo.itemAndTerms.end(); it_itemID++)
	{
		if (_stopped)
		{
			return false;
		}
		dataItem.itemId = it_itemID->first ;
		dataItem.termId = it_itemID->second;
		dataItem.time   = 0;
		dataItem.value  = 0;
		dataItems.push_back(dataItem);
	}

	//if (pSNf->isMaster())
	//{
	//	send(pCollTask->deviceId, 0, dataItems);
	//	UARCCollServer::GetLogger().information("Master下发采集任务[UnitID:%s, TaskID:%d]成功!", pCollTask->unitId, pCollTask->taskId);
	//}
	if (pSNf->isInit())
	{
		if (send(pCollTask->deviceId, 0, dataItems))
		{
			setInit(pCollTask->deviceId, false);
			UARCCollServer::GetLogger().information("初始采集任务[UnitID:%s, TaskID:%d]下发成功!", pCollTask->unitId, pCollTask->taskId);
		}else {
			UARCCollServer::GetLogger().information("初始采集任务[UnitID:%s, TaskID:%d]下发失败!", pCollTask->unitId, pCollTask->taskId);
		}
	}
			
	// TODO 执行下发任务至设备终端操作
	bool bSend = send(pCollTask->deviceId, SEND_COLLECTDATA_TYPE, dataItems);
	if(bSend)
	{
		;//_rtdbms->saveCollTaskPara(*pCollTask);
	}else {
		UARCCollServer::GetLogger().information("下发采集任务[UnitID:%s, TaskID:%d]失败!", pCollTask->unitId, pCollTask->taskId);
		return false;
	}

	return true;
}

void showTime(time_t t)
{
	//time_t是long类型，精确到秒，是当前时间和1970年1月1日零点时间的差
	if(t>0)
	{
		struct tm* current_time = localtime(&t);
		/*本地时间：日期，时间 年月日，星期，时分秒*/
		printf(" %d-%d-%d %d:%d:%d ",
			current_time->tm_year + 1900,
			current_time->tm_mon + 1,
			current_time->tm_mday,
			current_time->tm_hour,
			current_time->tm_min,
			current_time->tm_sec);
	}
	else
	{
		printf(" 0000-00-00 00:00:00 ");
	}
}

void CollTaskSubsystem::Stop()
{
	if (!_stopped) {

		_stopped = true;

		CollTaskData task;

		task.unitId = "";
		task.taskId = 0;
		task.status = COLLTASK_ST_VALID;
		task.startDate = 0;
		task.lastCollTime = 0;
		task.exeRound = 0;
		task.freq = 0;

		addTask(task);

		_thread.join();
		clearTask();
	}
	UARCCollServer::GetLogger().information("停止采集任务子系统[%s]!", std::string(name()));
}

