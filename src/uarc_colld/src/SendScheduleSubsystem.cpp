/*
 * SendScheduleSubsystem.cpp
 *
 *  Created on: 2015-5-23
 *      Author: root
 */

#include "SendScheduleSubsystem.h"

SendScheduleSubsystem::SendScheduleSubsystem():
_stopped(true), _thread(){
	// TODO Auto-generated constructor stub

}

SendScheduleSubsystem::~SendScheduleSubsystem() {
	// TODO Auto-generated destructor stub
}

void SendScheduleSubsystem::initialize(Poco::Util::Application & self)
{
	// TODO 初始化rtdbms对象
	std::string   rtdbms_ip   = self.config().getString("RedisDB.Base.IPAddr", "127.0.0.1");
	int           rtdbms_port = self.config().getInt("RedisDB.Base.Port", 6379);
	_rtdbms                   = new  CRtDbMs(rtdbms_ip,rtdbms_port);
}

void SendScheduleSubsystem::uninitialize()
{
	delete _rtdbms;
	_rtdbms = NULL;
}

void SendScheduleSubsystem::Start()
{
	poco_assert (_stopped);
	_stopped = false;
	_thread.start(*this);
	UARCCollServer::GetLogger().information("启动下发配比计划子系统[%s]!", std::string(name()));

}

const char *SendScheduleSubsystem::name() const
{
	return "SendScheduleSubsystem";
}

void SendScheduleSubsystem::run()
{
	// TODO 注册回调,监视内存数据库是否有控制命令下发
	bool bRegistCallback = false;

	bRegistCallback = _rtdbms->setDevScheduleCallback(ScheduleCmdTaskRun);  //配比计划回调
	if (bRegistCallback)
	{
		UARCCollServer::GetLogger().information("注册配比计划下发回调成功!");
	}
	else
	{
		UARCCollServer::GetLogger().error("注册配比计划下发回调失败!");
	}

	// TODO 阻塞等待
	_rtdbms->start();

}

void SendScheduleSubsystem::Stop()
{
	if (!_stopped) {
		_rtdbms->stop();
		_stopped = true;
		_thread.join();
	}
	UARCCollServer::GetLogger().information("停止配比计划下发子系统[%s]!", std::string(name()));
}

bool ScheduleCmdTaskRun(const ScheduleInfoData& scheduleTask)
{
	// TODO 将控制命令下发至采集终端处理
	bool bSend;
	DataItem dataItem;
	std::vector<DataItem>  dataItems;

	dataItem.itemId = scheduleTask.itemId;
	dataItem.termId = scheduleTask.termId;
	dataItem.value  = scheduleTask.scheduleVal;
	dataItem.time   = scheduleTask.scheduleTime;

	// TODO 填充数据添加到下发任务集合中
	dataItems.push_back(dataItem);

	bSend = send(scheduleTask.deviceId,SEND_SCHEDULECMD_TYPE,dataItems);
	if(bSend)
	{
		UARCCollServer::GetLogger().information("下发配比计划(termId:%d itemId:%d scheduleVal:%f scheduleTime:%d)至终端设备(deviceId:%d)成功!",
				scheduleTask.termId,scheduleTask.itemId,scheduleTask.scheduleVal,scheduleTask.scheduleTime,scheduleTask.deviceId);
		return true;
	}
	else
	{
		UARCCollServer::GetLogger().information("下发配比计划(termId:%d itemId:%d scheduleVal:%f scheduleTime:%d)至终端设备(deviceId:%d)失败!",
				scheduleTask.termId,scheduleTask.itemId,scheduleTask.scheduleVal,scheduleTask.scheduleTime,scheduleTask.deviceId);
		return false;
	}

	return true;
}
