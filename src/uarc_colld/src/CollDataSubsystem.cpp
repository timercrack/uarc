/*
 * CollDataSubsystem.cpp
 *
 *  Created on: 2014-10-10
 *      Author: root
 */

#include "CollDataSubsystem.h"

extern CollTaskSubsystem *g_pCollTaskSubsystem;
CollDataSubsystem::CollDataSubsystem()  :
_stopped(true), _thread() {
	// TODO Auto-generated constructor stub

}

CollDataSubsystem::~CollDataSubsystem() {
	// TODO Auto-generated destructor stub

}

void CollDataSubsystem::initialize(Poco::Util::Application & self)
{
	// TODO 初始化rtdbms对象
	std::string   rtdbms_ip   = self.config().getString("RedisDB.Base.IPAddr", "127.0.0.1");
	int           rtdbms_port = self.config().getInt("RedisDB.Base.Port", 6379);
	_rtdbms      = new  CRtDbMs(rtdbms_ip,rtdbms_port);
}

void CollDataSubsystem::uninitialize()
{
	delete _rtdbms;
	_rtdbms = NULL;
}

void CollDataSubsystem::Start()
{
	poco_assert (_stopped);
	_stopped = false;
	_thread.start(*this);
	UARCCollServer::GetLogger().information("启动采集任务通知子系统[%s]!", std::string(name()));
}

const char *CollDataSubsystem::name() const
{
	return "CollDataSubsystem";
}

bool freshCollectTask(const CollTaskData& collTask)
{
	CollTaskData task = collTask;
	// TODO 把注册通知来的新的有效采集任务添加到原队列中
	if (g_pCollTaskSubsystem->freshTask(task))
	{
		UARCCollServer::GetLogger().information("刷新采集任务[UnitID:%s TaskID:%d]成功！", task.unitId, task.taskId);
		return true;
	}else {
		UARCCollServer::GetLogger().information("刷新采集任务[UnitID:%s TaskID:%d]失败！", task.unitId, task.taskId);
		return false;
	}
}

void CollDataSubsystem::run()
{
	bool  bRegistCallback = false;
	bRegistCallback = _rtdbms->setnewCollTaskCallback(freshCollectTask);
	if( !bRegistCallback)
	{
		UARCCollServer::GetLogger().error("注册采集任务通知回调失败!");
	}
	else
	{
		UARCCollServer::GetLogger().information("注册采集任务通知回调成功!");
	}

	_rtdbms->start();
}

void CollDataSubsystem::Stop()
{
	if (!_stopped) {
		_rtdbms->stop();
		_stopped = true;
		_thread.join();
	}
	UARCCollServer::GetLogger().information("停止采集任务通知子系统[%s]!", std::string(name()));
}

