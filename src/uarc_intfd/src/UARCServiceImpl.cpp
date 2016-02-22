/*
 * UARCServiceImpl.cpp
 *
 *  Created on: 2014-10-16
 *      Author: jeffchen
 */

#include "uarc_intfd.h"
#include "UARCServiceImpl.h"
#include <google/protobuf/text_format.h>
#include"Poco/StringTokenizer.h"
#include<unistd.h>
#include<vector>

using namespace google::protobuf;
using namespace RCF;
using Poco::StringTokenizer;

UARCServiceImpl::UARCServiceImpl()
{
	_rtdbms	= new CRtDbMs(UARCIntfServer::rtdbms_ip,UARCIntfServer::rtdbms_port);
}
UARCServiceImpl::~UARCServiceImpl()
{
	delete _rtdbms;
}
void UARCServiceImpl::printRequest(RpcController* controller, const google::protobuf::Message * request)
{
	UARCIntfServer::GetLogger().information("************************");
	UARCIntfServer::GetLogger().information("*** Received request ***");

	RcfProtoController * rcfController = static_cast<RcfProtoController *>(controller);
	RcfProtoSession * pSession = rcfController->getSession();

	UARCIntfServer::GetLogger().information("Client username: %s",RCF::toAstring(pSession->getClientUsername()));
	UARCIntfServer::GetLogger().information("Compression enabled: %s",pSession->getEnableCompression()==true? std::string("true"):std::string("false"));
	UARCIntfServer::GetLogger().information("Transport type:  %s",RCF::getTransportTypeName(pSession->getTransportType()));
	UARCIntfServer::GetLogger().information("Transport protocol: %s",
			RCF::getTransportProtocolName(pSession->getTransportProtocol()));

	std::string strRequest;
	TextFormat::PrintToString(*request, &strRequest);
	UARCIntfServer::GetLogger().information("Request:\n");
	UARCIntfServer::GetLogger().information(" %s",strRequest);
}

void UARCServiceImpl::printResponse(google::protobuf::Message * response)
{
	UARCIntfServer::GetLogger().information("*** Sending response ***\n");
	std::string strResponse;
	TextFormat::PrintToString(*response, &strResponse);
	UARCIntfServer::GetLogger().information("%s\n",strResponse);
	UARCIntfServer::GetLogger().information("************************");
}

void UARCServiceImpl::UarcSendSchedule(::google::protobuf::RpcController* controller,
                       const ::MsgSchedule* request,
                       ::MsgResult* response,
                       ::google::protobuf::Closure* done)
{
	printRequest(controller,request);

	std::cout<<"schedule information is:"<<std::endl;
	std::cout<<"schedule date is:"<<request->date()<<std::endl;
	std::cout<<"schedule startTime is:"<<request->starttime()<<std::endl;
	std::cout<<"schedule controlTime is:"<<request->controltime()<<std::endl;
	int scheduleDate = request->date();
	ScheduleTask scheduleT;

	for(int i=0; i<request->schedulevalues_size();i++)
	{
		scheduleT.deviceId = request->schedulevalues(i).deviceid();
		scheduleT.termId = request->schedulevalues(i).termid();
		scheduleT.itemId = request->schedulevalues(i).itemid();
		std::cout<<"schedule deviceId is:"<<scheduleT.deviceId<<std::endl;
		std::cout<<"schedule termId is:"<<scheduleT.termId<<std::endl;
		std::cout<<"schedule itemId is:"<<scheduleT.itemId<<std::endl;
		
		for(int j = 0; j<request->schedulevalues(i).schedulevalue_size()&&
			j<request->schedulevalues(i).scheduletime_size();j++)
		{
			scheduleT.scheduleVal = request->schedulevalues(i).schedulevalue(j);
			//scheduleT.scheduleTime = scheduleDate + j*3600;
			scheduleT.scheduleTime = scheduleDate + request->schedulevalues(i).scheduletime(j)*3600;
			std::cout<<"schedule value is:"<<scheduleT.scheduleVal<<std::endl;
			std::cout<<"schedule time is:"<<scheduleT.scheduleTime<<std::endl;
			_rtdbms->saveScheduleTask(&scheduleT);
			usleep(1000000);
		}
	}
	response->set_result(MsgResult_ResultCode_Success);

	printResponse(response);

	done->Run();
}

void UARCServiceImpl::UarcSetDevice(::google::protobuf::RpcController* controller,
		const ::MsgDevice* device,
		::MsgResult* response,
		 ::google::protobuf::Closure* done)
{
	printRequest(controller, device);

	DeviceInfo di;
	di.deviceId = device->deviceid();
	di.status = device->status();
	di.address = device->address();
	di.port = device->port();
	di.identity = device->identity();
	di.pulsePeriod = device->heartbeat();
	di.protocol = device->protocol();
	di.linkType = device->linktype();

	if(di.status == MsgDevice::DeviceAvailable){

		UARCIntfServer::GetLogger().information("saveDeviceInfo() successful\n");
		//cout<<"saveDeviceInfo() successful"<<endl;
		if(di.protocol == MsgDevice::Protocol_104){
			UARCIntfServer::GetLogger().information("The DeviceProtocol is 104\n");
			//cout<<"The DeviceProtocol is 104"<<endl;
		}
		else if(di.protocol == MsgDevice::Protocol_ModBus){
			UARCIntfServer::GetLogger().information("The DeviceProtocol is ModBus\n");
			//cout<<"The DeviceProtocol is ModBus"<<endl;
		}
	}

	bool rst = _rtdbms->saveDeviceInfo(&di);
	if (rst) response->set_result(MsgResult_ResultCode_Success);
	else response->set_result(MsgResult_ResultCode_InternalErr);

	printResponse(response);
	done->Run();
}

bool UARCServiceImpl::saveItemCode(const ::MsgItemCode* code)
{
	ItemInfo item;
	item.itemId = code->itemid();
	item.unitId = code->unitid();
	item.deviceId = code->deviceid();
	item.termId = code->termid();
	item.itemType = code->itemtype();
	item.srcType = code->srctype();
	item.upLimitVal = code->uplimit();
	item.downLimitVal = code->downlimit();
	item.warnTypeMask = code->warntype();
	item.baseVal = code->baseval();
	item.coefficient = code->coefficient();
	item.dayStoreTime = code->daystoretime();
	item.dayStoreSql = code->daystoresql();
	item.warnStoreSql = code->warnstoresql();
	item.secCollPeriod = code->seccollperiod();
	item.secCollSql = code->seccollsql();
	item.secCollOtherSql = code->seccollothersql();
	item.status = code->status();
	item.storeType = code->storetype();

/*
	if(item.typeMask == MsgItemCode::ItemCodeNormal){
		cout<<"The ItemCodeType is ItemCodeNoraml"<<endl;
	}
	else if(item.typeMask == MsgItemCode::ItemCodeWarning){
		cout<<"The ItemCodeType is ItemCodeWarning"<<endl;
	}
	else if(item.typeMask ==MsgItemCode::ItemCodeCollect){
		cout<<"The ItemCodeType is ItemCodeCollect "<<endl;
	}
*/

	return _rtdbms->saveItemInfo(&item);
}

/***********modify UarcSetCollTask 2014-01-04**************/
struct collTItemInfo      //记录发送过来的指标档案相关信息结构体
{
	int itemId;
	std::string unitId;
	int index;
};
void UARCServiceImpl::UarcSetCollTask(::google::protobuf::RpcController* controller,
		const ::MsgCollTask* task,
		::MsgResult* response,
		 ::google::protobuf::Closure* done)
{
	printRequest(controller, task);

	CollTask ct;

	struct collTItemInfo tempCollTItem;
	std::vector<struct collTItemInfo> collTItems;
	UARCIntfServer::GetLogger().information("*****UarcSetCollTask**call****");

	for(int i = 0;i<task->itemids_size();i++)
	{
		tempCollTItem.itemId = task->itemids(i).itemid();
		tempCollTItem.unitId = task->itemids(i).unitid();
		tempCollTItem.index = i;
		collTItems.push_back(tempCollTItem);
	}
	UARCIntfServer::GetLogger().information("*****接收完成******");

	std::string unitIdTemp;
	bool saveItemSuc = false;
	bool saveCollTaskSuc = false;
	bool unitIdSerial = true;   //后续的unitId是否连续,依据此变量来修改迭代器it

	UARCIntfServer::GetLogger().information("*****开始整理采集任务******");
	for(std::vector<struct collTItemInfo>::iterator it = collTItems.begin(); it != collTItems.end();)
	{
		unitIdTemp = it->unitId;
		ct.unitId = unitIdTemp;
		ct.taskStatus = task->status();
		ct.freq = task->freq();
		unitIdSerial = true;

		for(std::vector<struct collTItemInfo>::iterator findIt = it; findIt != collTItems.end(); )
		{
			if(unitIdTemp == findIt->unitId)
			{
				saveItemSuc = saveItemCode( &( task->itemids(findIt->index) ) );
				if ( saveItemSuc ) {
					ct.itemIds.push_back( task->itemids(findIt->index).itemid() );
					findIt = collTItems.erase(findIt);
					if(unitIdSerial)
					{
						it = findIt;
					}
				}
				else{
					UARCIntfServer::GetLogger().information("saveItemCode(itemId:%X) error\n",task->itemids(findIt->index).itemid());
					//cout<<"saveItemCode(&( task->itemids(i))) error"<<endl;
				}
			}
			else
			{
				findIt++;
				unitIdSerial = false;
			}
		}

		saveCollTaskSuc = _rtdbms->saveCollTask(&ct);

		if(false == saveCollTaskSuc)
		{
			UARCIntfServer::GetLogger().information("*****存储采集任务失败******");
			break;
		}
	}
	UARCIntfServer::GetLogger().information("*****整理采集任务完成******");

	if(saveCollTaskSuc) response->set_result(MsgResult_ResultCode_Success);
	else response->set_result(MsgResult_ResultCode_InternalErr);

	printResponse(response);
	done->Run();
}

void UARCServiceImpl::UarcSetItemCode(::google::protobuf::RpcController* controller,
		const ::MsgItemCode* code,
		::MsgResult* response,
		 ::google::protobuf::Closure* done)
{
	printRequest(controller, code);

	bool rst = saveItemCode(code);
	if (rst) response->set_result(MsgResult_ResultCode_Success);
	else response->set_result(MsgResult_ResultCode_InternalErr);

	printResponse(response);
	done->Run();
}

void UARCServiceImpl::UarcSetFormular(::google::protobuf::RpcController* controller,
		const ::MsgFormular* formular,
		::MsgResult* response,
		 ::google::protobuf::Closure* done)
{
	printRequest(controller, formular);

	CalculateTask ct;
	ct.calcId = formular->formularid();
	ct.itemId = formular->itemid();
	ct.unitId = formular->unitid();
	ct.formula = formular->expr();
	ct.paramMask = formular->mask();
	for(int i=0;i<formular->params_size();i++)  //上层平台参数输入格式：unitId.itemId
	{
		/*
		StringTokenizer strParaTaskToken(formular->params(i),",");
		if(strParaTaskToken.count() >= 2)
		{
			ct.params[i].unitId = atoi(strParaTaskToken[0].data());
			sscanf(strParaTaskToken[1].data(),"%X",&ct.params[i].itemId);
		}
		*/

		StringTokenizer strParaTaskToken(formular->params(i),".");
		if(strParaTaskToken.count() >= 2 && i < FORMULA_PARAM_NUM)
		{
			ct.params[i].unitId = strParaTaskToken[0];
			sscanf(strParaTaskToken[1].data(),"%X",&ct.params[i].itemId);
		}
	}

	ct.status = formular->status();
	ct.freq = formular->freq();
	ct.priority = formular->priority();

	/*
	if(ct.priority == MsgFormular::Level1){
		cout<<"The FormularPriority is Level1"<<endl;
	}
	else if(ct.priority == MsgFormular::Level2){
		cout<<"The FormularPriority is Level2"<<endl;
	}
	else if(ct.priority == MsgFormular::Level3){
		cout<<"The FormularPriority is Level3"<<endl;
	}
	else if(ct.priority == MsgFormular::Level4){
		cout<<"The FormularPriority is Level4"<<endl;
	}
	else if(ct.priority == MsgFormular::Level5){
		cout<<"The FormularPriority is Level5"<<endl;
	}
	*/

	bool rst = _rtdbms->saveCalculateTask(&ct);
	if (rst) response->set_result(MsgResult_ResultCode_Success);
	else response->set_result(MsgResult_ResultCode_InternalErr);

	printResponse(response);
	done->Run();
}

bool getZhaoceData(const ZhaoCeData& zhaoceData,CRtDbMs* rtdbmsCallP)  //zhaoce Data callback test
{
	/*
	printf("zhaoce Data info:\n");
	printf("unitId:%s;deviceId:%d;termId:%d;itemId:%X,time:%d,value:%f\n",
			zhaoceData.unitId.data(),zhaoceData.deviceId,zhaoceData.termId,zhaoceData.itemId,
			zhaoceData.zhaoceTime,zhaoceData.zhaoCeValue);
	*/

	rtdbmsCallP->privateData = (void*)&zhaoceData.zhaoCeValue;
	rtdbmsCallP->stop();
	return true;
}

void UARCServiceImpl::UarcCallData(::google::protobuf::RpcController* controller,
		const ::MsgCallData* call,
		::MsgResult* response,
		 ::google::protobuf::Closure* done)
{
	printRequest(controller, call);

	ZhaoceTask zt;
	// TODO 剩下的赋值补全
	zt.unitId = call->unitid();
	zt.deviceId = call->deviceid();
	zt.termId = call->termid();
	zt.itemId = call->itemid();

	//bool rst = _rtdbms->saveZhaoceTask(&zt);
	UarcStoreItemInfo storeItemInfo;
	bool getStoreSuc = _rtdbms->getArchInfo(zt.unitId,zt.itemId,&storeItemInfo);
	if(getStoreSuc)
	{
		int lstStoreT;
		double lstStoreVal;
		usleep(10000);   //0.01s
		bool rst = _rtdbms->getLastStoreValue(zt.unitId,zt.itemId,lstStoreT,lstStoreVal);
		if (rst)
		{
			response->set_result(MsgResult_ResultCode_Success);
			double lstCallData = lstStoreVal*storeItemInfo.coefficient + storeItemInfo.baseVal;
			response->set_resultdoubleval(lstCallData);
		}
		else
		{
			response->set_result(MsgResult_ResultCode_InternalErr);
			response->set_resultdoubleval(0.0);
		}
	}
	else
	{
		response->set_result(MsgResult_ResultCode_InternalErr);
		response->set_resultdoubleval(0.0);
	}

	printResponse(response);


	//printf("zhu ce Zhaoce Data!\n");
	/*
	_rtdbms->setZhaoceDataCallback(getZhaoceData);
	_rtdbms->start();
	*/

	//response->set_resultdoubleval(*(double*)(_rtdbms->privateData));

	//printf("done run execute!\n");

	done->Run();
}

void UARCServiceImpl::UarcBatchCallData(::google::protobuf::RpcController* controller,
		const ::MsgBatchCallData* call,
		::MsgBatchResults* response,
		 ::google::protobuf::Closure* done)
{
	printRequest(controller, call);

	bool operateSuc = true;
	if(call->unitids_size()!=call->itemids_size())
	{
		operateSuc = false;
	}
	else
	{
		UarcStoreItemInfo storeItemInfo;
		for(int i=0; i<call->unitids_size(); i++)
		{
			bool getStoreSuc = _rtdbms->getArchInfo(call->unitids(i),call->itemids(i),&storeItemInfo);
			if(getStoreSuc)
			{
				int lstStoreT;
				double lstStoreVal;
				bool rst = _rtdbms->getLastStoreValue(call->unitids(i),call->itemids(i),lstStoreT,lstStoreVal);
				if (rst)
				{
					double lstCallData = lstStoreVal*storeItemInfo.coefficient + storeItemInfo.baseVal;
					response->add_unitids(call->unitids(i));
					response->add_itemids(call->itemids(i));
					response->add_values(lstCallData);
				}
			}
		}
	}

	if(operateSuc == true)
	{
		response->set_result(MsgBatchResults_ResultBatchCode_Success);
	}
	else
	{
		response->set_result(MsgBatchResults_ResultBatchCode_InternalErr);
	}

	printResponse(response);

	done->Run();
}

void UARCServiceImpl::UarcDeviceControl(::google::protobuf::RpcController* controller,
		const ::MsgControl* control,
		::MsgResult* response,
		 ::google::protobuf::Closure* done)
{
	printRequest(controller, control);

	CtrlTask ct;
	// TODO 剩下的赋值补全
	ct.deviceId = control->deviceid();
	ct.termId = control->termid();
	ct.itemId = control->itemid();
	ct.ctrlVal = control->value();

	/*
	cout<<"device Control command:"<<endl;
	cout<<"		deviceId:"<<ct.deviceId<<endl;
	cout<<"		termId:"<<ct.termId<<endl;
	//cout<<"		itemId:"<<hex<<ct.itemId<<endl;
	cout<<"		itemId:"<<ct.itemId<<endl;
	cout<<"		ctrlVal:"<<ct.ctrlVal<<endl;
	*/
	bool rst = _rtdbms->saveControlTask(&ct);
	if (rst) response->set_result(MsgResult_ResultCode_Success);
	else response->set_result(MsgResult_ResultCode_InternalErr);

	printResponse(response);
	done->Run();
}


