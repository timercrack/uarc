/*
 * RecvDataSubsystem.cpp
 *
 *  Created on: 2014-10-10
 *      Author: root
 */

#include "RecvDataSubsystem.h"

CRtDbMs*  RecvDataSubsystem::_rtdbms;

RecvDataSubsystem::RecvDataSubsystem()  :
_stopped(true), _thread() {
	// TODO Auto-generated constructor stub

}

RecvDataSubsystem::~RecvDataSubsystem() {
	// TODO Auto-generated destructor stub

}

void RecvDataSubsystem::initialize(Poco::Util::Application & self)
{
	// TODO 初始化rtdbms对象
	std::string   rtdbms_ip   = self.config().getString("RedisDB.Base.IPAddr", "127.0.0.1");
	int           rtdbms_port = self.config().getInt("RedisDB.Base.Port", 6379);
	_rtdbms = new  CRtDbMs(rtdbms_ip,rtdbms_port);

}

void RecvDataSubsystem::uninitialize()
{
	delete _rtdbms;
	_rtdbms = NULL;
}

void RecvDataSubsystem::Start()
{
	poco_assert (_stopped);
	_stopped = false;
	UARCCollServer::GetLogger().information("启动接收数据子系统[%s]!", std::string(name()));

	//注册回调
	bool bRegistCallback = false;
	bRegistCallback = setReceiveCallBack(receive);
	if( !bRegistCallback)
	{
		UARCCollServer::GetLogger().error("注册接收终端数据回调失败!");
	}else {
		UARCCollServer::GetLogger().information("注册接收终端数据回调成功!");
	}
}

const char *RecvDataSubsystem::name() const
{
	return "RecvDataSubsystem";
}

void receive(int deviceId, int reason, const std::vector<DataItem> &dataItems)
{
	// TODO 将终端返回的数据存储至内存数据库
	std::vector<DataItem>::const_iterator it_itemId;
    if(reason == RECV_COLLECT_DATA_TYPE)
    {
		bool  bSaveCollData = false;
		CollectedData saveCollData;
		UARCCollServer::GetLogger().information("保存采集数据至内存数据库!");
		for(it_itemId = dataItems.begin();it_itemId!= dataItems.end();it_itemId++)
		{
			saveCollData.deviceId = deviceId;
			saveCollData.termId   = it_itemId->termId;
			saveCollData.itemId   = it_itemId->itemId;
			saveCollData.value    = it_itemId->value;
			saveCollData.time     = it_itemId->time;
			//保存采集数据到内存数据库
			bSaveCollData = RecvDataSubsystem::_rtdbms->saveCollectedData(&saveCollData);
			if(!bSaveCollData)
			{
				UARCCollServer::GetLogger().error("保存采集数据(time:%d value:%f)至内存数据库失败!",saveCollData.time,saveCollData.value);
			}
		}
    }
    else if(reason == RECV_ZHAOCE_DATA_TYPE)
    {
		bool           bSaveZhaoCeData;
		ZhaoceBackData saveZhaoCeData;
		UARCCollServer::GetLogger().information("保存召测数据至内存数据库!");
		for(it_itemId = dataItems.begin();it_itemId!= dataItems.end();it_itemId++)
		{
			saveZhaoCeData.deviceId = deviceId;
			saveZhaoCeData.termId   = it_itemId->termId;
			saveZhaoCeData.itemId   = it_itemId->itemId;
			saveZhaoCeData.value    = it_itemId->value;
			saveZhaoCeData.time     = it_itemId->time;
			//保存召测数据到内存数据库
			std::vector<std::string>  unitIds;
			std::vector<std::string>  ::iterator it_unitId;
			UARCCollServer::instance().getSubsystem<SendZhaoceSubsystem>().getUnitid(deviceId, it_itemId->termId, it_itemId->itemId, unitIds);
			for(it_unitId = unitIds.begin();it_unitId!= unitIds.end();it_unitId++)
			{
				saveZhaoCeData.unitId = (*it_unitId);
				bSaveZhaoCeData = RecvDataSubsystem::_rtdbms->saveZhaoceData(&saveZhaoCeData);
				UARCCollServer::instance().getSubsystem<SendZhaoceSubsystem>().deleteZhaoceTask(deviceId, it_itemId->termId, it_itemId->itemId,(*it_unitId));
				if(!bSaveZhaoCeData)
				{
					UARCCollServer::GetLogger().error("保存召测数据(time:%d value:%f)至内存数据库失败!",saveZhaoCeData.time,saveZhaoCeData.value);
				}
			}
		}
    }else
    {
    	UARCCollServer::GetLogger().error("接收数据原因不详!");
    }
}

void RecvDataSubsystem::Stop()
{
	if (!_stopped) {

		_stopped = true;
		_thread.join();
	}
	UARCCollServer::GetLogger().information("停止接收数据子系统[%s]!", std::string(name()));
}

