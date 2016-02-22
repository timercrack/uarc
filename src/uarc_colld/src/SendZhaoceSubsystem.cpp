/*
 * SendZhaoceSubsystem.h
 *
 *  Created on: 2014-10-10
 *      Author: root
 */

#include "SendZhaoceSubsystem.h"

std::vector<ZhaoCeExecInfoData> SendZhaoceSubsystem::shareZhaoceData;

SendZhaoceSubsystem::SendZhaoceSubsystem()  :
_stopped(true), _thread()  {
	// TODO Auto-generated constructor stub

}

SendZhaoceSubsystem::~SendZhaoceSubsystem() {
	// TODO Auto-generated destructor stub

}

void SendZhaoceSubsystem::initialize(Poco::Util::Application & self)
{
	// TODO 初始化rtdbms对象
	std::string   rtdbms_ip   = self.config().getString("RedisDB.Base.IPAddr", "127.0.0.1");
	int           rtdbms_port = self.config().getInt("RedisDB.Base.Port", 6379);
    _rtdbms                   = new  CRtDbMs(rtdbms_ip,rtdbms_port);

}

void SendZhaoceSubsystem::uninitialize()
{
	delete _rtdbms;
	_rtdbms = NULL;
}

void SendZhaoceSubsystem::Start()
{
	poco_assert (_stopped);
	_stopped = false;
	_thread.start(*this);
	UARCCollServer::GetLogger().information("启动发送召测任务子系统[%s]!", std::string(name()));
}

const char *SendZhaoceSubsystem::name() const
{
	return "SendZhaoceSubsystem";
}

void SendZhaoceSubsystem::run()
{
	// TODO 注册回调,执行不断监视内存数据库是否有召测任务下发
	bool bRegistCallback = false;
	bRegistCallback = _rtdbms->setZhaoceCallback(DataZhaoceTaskRun);         // 参数召测回调
	if (bRegistCallback)
	{
		UARCCollServer::GetLogger().information("注册发送召测参数回调成功!");
	}
	else
	{
		UARCCollServer::GetLogger().error("注册发送召测参数回调失败!");
	}

	// TODO 阻塞等待
	_rtdbms->start();

}

void SendZhaoceSubsystem::Stop()
{
	if (!_stopped) {
		_rtdbms->stop();
		_stopped = true;
		_thread.join();
	}
	UARCCollServer::GetLogger().information("停止发送召测任务子系统[%s]!", std::string(name()));
}

bool DataZhaoceTaskRun(const ZhaoCeExecInfoData& zhaoceInfo)
{
	//将召测数据下发至采集终端处理
	bool bSend;
	DataItem dataItem;
	std::vector<DataItem>  dataItems;
	SendZhaoceSubsystem::shareZhaoceData.push_back(zhaoceInfo);//保存召测数据项

	dataItem.itemId = zhaoceInfo.itemId;
	dataItem.termId = zhaoceInfo.termId;
	dataItem.time   = 0;
	dataItem.value  = 0;
	// 填充召测数据添加到下发任务集合中
	dataItems.push_back(dataItem);

	bSend = send(zhaoceInfo.deviceId,SEND_ZHAOCEARGC_TYPE,dataItems);
	if(bSend)
	{
		UARCCollServer::GetLogger().information("下发召测数据(itemId:%d termId:%d)至终端设备(deviceId:%d)成功!",zhaoceInfo.itemId,zhaoceInfo.termId,zhaoceInfo.deviceId);
		return true;
	}
	else
	{
		UARCCollServer::GetLogger().error("下发召测数据(itemId:%d termId:%d)至终端设备(deviceId:%d)失败!",zhaoceInfo.itemId,zhaoceInfo.termId,zhaoceInfo.deviceId);
		return false;
	}

	return true;
}

int SendZhaoceSubsystem::getUnitid(int deviceId, int termId, int itemId, std::vector<std::string>& unitIds)
{
    //便于接收召测数据的时候找到下发召测任务时相应的unitId
	std::vector<ZhaoCeExecInfoData>::iterator it_dataItems;
	for(it_dataItems=shareZhaoceData.begin();it_dataItems!=shareZhaoceData.end();it_dataItems++)
    {
        if((it_dataItems->deviceId == deviceId) && (it_dataItems->itemId == itemId) && (it_dataItems->termId == termId))
        {
        	unitIds.push_back(it_dataItems->unitId);
        }
    }
	return 0;
}

int SendZhaoceSubsystem::deleteZhaoceTask(int deviceId, int termId, int itemId, std::string unitId)
{
	std::vector<ZhaoCeExecInfoData>::iterator it_shareItems;
	for(it_shareItems=shareZhaoceData.begin();it_shareItems!=shareZhaoceData.end(); )
    {
        if((it_shareItems->deviceId == deviceId) && (it_shareItems->itemId == itemId) && (it_shareItems->termId == termId) &&(it_shareItems->unitId == unitId))
        {
        	it_shareItems = SendZhaoceSubsystem::shareZhaoceData.erase(it_shareItems);
        }
    	else
    	{
    		it_shareItems++;
    	}
    }
	return 0;
}
