/*
 * SendCmdSubSystem.cpp
 *
 *  Created on: 2014-10-10
 *      Author: root
 */

#include "SendCmdSubsystem.h"

SendCmdSubsystem::SendCmdSubsystem()  :
_stopped(true), _thread() {
	// TODO Auto-generated constructor stub

}

SendCmdSubsystem::~SendCmdSubsystem() {
	// TODO Auto-generated destructor stub

}

void SendCmdSubsystem::initialize(Poco::Util::Application & self)
{
	// TODO 初始化rtdbms对象
	std::string   rtdbms_ip   = self.config().getString("RedisDB.Base.IPAddr", "127.0.0.1");
	int           rtdbms_port = self.config().getInt("RedisDB.Base.Port", 6379);
    _rtdbms                   = new  CRtDbMs(rtdbms_ip,rtdbms_port);

}

void SendCmdSubsystem::uninitialize()
{
	delete _rtdbms;
	_rtdbms = NULL;
}

void SendCmdSubsystem::Start()
{
	poco_assert (_stopped);
	_stopped = false;
	_thread.start(*this);
	UARCCollServer::GetLogger().information("启动发送控制命令子系统[%s]!", std::string(name()));

}

const char *SendCmdSubsystem::name() const
{
	return "SendCmdSubSystem";
}

void SendCmdSubsystem::run()
{
	// TODO 注册回调,监视内存数据库是否有控制命令下发
	bool bRegistCallback = false;
	bRegistCallback = _rtdbms->setDeviceCntlCallback(ControlCmdTaskRun);     //控制命令回调
	if (bRegistCallback)
	{
		UARCCollServer::GetLogger().information("注册发送控制命令回调成功!");
	}
	else
	{
		UARCCollServer::GetLogger().error("注册发送控制命令回调失败!");
	}

	// TODO 阻塞等待
	_rtdbms->start();

}

void SendCmdSubsystem::Stop()
{
	if (!_stopped) {
		_rtdbms->stop();
		_stopped = true;
		_thread.join();
	}
	UARCCollServer::GetLogger().information("停止发送控制命令子系统[%s]!", std::string(name()));
}

bool ControlCmdTaskRun(const ControlExecInfoData& ctrlTask)
{
	// TODO 将控制命令下发至采集终端处理
	bool bSend;
	DataItem dataItem;
	std::vector<DataItem>  dataItems;

	dataItem.itemId = ctrlTask.itemId;
	dataItem.termId = ctrlTask.termId;
	dataItem.value  = ctrlTask.ctrlValue;
	dataItem.time   = 0;

	// TODO 填充数据添加到下发任务集合中
	dataItems.push_back(dataItem);

	bSend = send(ctrlTask.deviceId,SEND_CTRLCOMMAND_TYPE,dataItems);
	if(bSend)
	{
		UARCCollServer::GetLogger().information("下发控制命令(itemId:%d termId:%d ctrlValue:%f)至终端设备(deviceId:%d)成功!",ctrlTask.itemId,ctrlTask.termId,ctrlTask.ctrlValue,ctrlTask.deviceId);
		return true;
	}
	else
	{
		UARCCollServer::GetLogger().information("下发控制命令(itemId:%d termId:%d ctrlValue:%f)至终端设备(deviceId:%d)失败!",ctrlTask.itemId,ctrlTask.termId,ctrlTask.ctrlValue,ctrlTask.deviceId);
		return false;
	}

	return true;
}

