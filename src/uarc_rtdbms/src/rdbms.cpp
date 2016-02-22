/*
 * CRtDbMs.cpp
 *
 *  Created on: 2014-10-13
 *      Author: root
 */
#include "rdbms.h"

#include <iostream>
#include <vector>
#include <map>
#include <cstdlib>
#include <time.h>
#include <hiredis/hiredis.h>
#include <hiredis/async.h>
#include <hiredis/adapters/libevent.h>

#include<string>
#include<sstream>

#include "Poco/Mutex.h"
#include"Poco/DateTime.h"
#include"Poco/LocalDateTime.h"
#include"Poco/DateTimeParser.h"
#include"Poco/Timespan.h"
#include"Poco/NumberParser.h"
#include"Poco/StringTokenizer.h"

using namespace std;
using Poco::DateTime;
using Poco::LocalDateTime;
using Poco::DateTimeParser;
using Poco::Timespan;
using Poco::StringTokenizer;
using Poco::FastMutex;

enum CallBackTye {
	CallBackTye_ZhaoceData,
	CallBackTye_DeviceCntl,
	CallBackTye_DevSchedule,
	CallBackTye_ZhaoceInfo,
	CallBackTye_NewCollTask,
	CallBackTye_SecCollPeriod,
	CallBackTye_DataProcess,
	CallBackTye_DeviceInfo,
	CallBackTye_CallBack
};
struct CRtDbMs::impl {

	struct event_base* _base;
	redisAsyncContext* _acontext;
	redisContext* _context;
	FastMutex *_mutex;

	CallBackTye _callbackType;
	string _callbackName;

	RtDbMsCallback_ZhaoceData* _pZhaoceDataCallback;
	RtDbMsCallback_DeviceCntl* _pDeviceCntlCallback;
	RtDbMsCallback_DevSchedule* _pDevScheduleCallback;
	RtDbMsCallback_ZhaoceInfo* _pZhaoceInfoCallback;
	RtDbMsCallback_NewCollTask* _pNewCollTaskCallback;
	RtDbMsCallback_SecCollPeriod* _pSecCollPeriodCallback;
	RtDbMsCallback_DataProcess* _pDataProcessCallback;
	RtDbMsCallback_DeviceInfo* _pDeviceInfoCallback;
	map<string, RtDbMsCallBack *> _pCallBack;
};

void _redisCallBack(redisAsyncContext *c, void *reply,void *privdata)
{
	CRtDbMs* pCtrDbMs = (CRtDbMs *)privdata;
	redisReply *callBReply = (redisReply *)reply;
	if ( pCtrDbMs->_impl->_callbackType == CallBackTye_DeviceCntl){
//		printf("===CallBackTye_DeviceCntl===\n");
		ControlExecInfoData ct;
		if(callBReply!=NULL && callBReply->elements>=4 && string(callBReply->element[3]->str)==string("hset"))
		{
			StringTokenizer stringCtrlKeyToken(callBReply->element[2]->str,":");
			if(stringCtrlKeyToken.count() >= 3)
			{
				redisReply *ctrlCmdReply =(redisReply *)redisCommand(pCtrDbMs->_impl->_context,"hmget "
						"%s:%s deviceId termId itemId ctrlValue",stringCtrlKeyToken[1].data(),
						stringCtrlKeyToken[2].data());
				if(ctrlCmdReply!=NULL && ctrlCmdReply->elements >= 4)
				{
					ct.deviceId = atoi(ctrlCmdReply->element[0]->str!=NULL? ctrlCmdReply->element[0]->str:"0");
					ct.termId = atoi(ctrlCmdReply->element[1]->str!=NULL? ctrlCmdReply->element[1]->str:"0");
					sscanf(ctrlCmdReply->element[2]->str!=NULL? ctrlCmdReply->element[2]->str:"0","%X",&ct.itemId);
					Poco::NumberParser::tryParseFloat(ctrlCmdReply->element[3]->str!=NULL?
							ctrlCmdReply->element[3]->str:"0",ct.ctrlValue);

					freeReplyObject(ctrlCmdReply);
				}
				else
				{
					freeReplyObject(ctrlCmdReply);
					return;
				}

				redisCommand(pCtrDbMs->_impl->_context,"del %s:%s",stringCtrlKeyToken[1].data(),
						stringCtrlKeyToken[2].data());

			}
			else return;

			pCtrDbMs->_impl->_pDeviceCntlCallback(ct);
		}
		else
			return;

	}
	else if(pCtrDbMs->_impl->_callbackType == CallBackTye_DevSchedule){
//		printf("===CallBackTye_DevSchedule===\n");
		ScheduleInfoData ct;
		if(callBReply!=NULL && callBReply->elements>=4 && string(callBReply->element[3]->str)==string("hset"))
		{
			StringTokenizer stringSchedKeyToken(callBReply->element[2]->str,":");
			if(stringSchedKeyToken.count() >= 3)
			{
				redisReply *scheduleReply =(redisReply *)redisCommand(pCtrDbMs->_impl->_context,"hmget "
						"%s:%s deviceId termId itemId scheduleVal scheduleTime",stringSchedKeyToken[1].data(),
						stringSchedKeyToken[2].data());
				if(scheduleReply!=NULL && scheduleReply->elements >= 5)
				{
					ct.deviceId = atoi(scheduleReply->element[0]->str!=NULL?
							           scheduleReply->element[0]->str:"0");
					ct.termId = atoi(scheduleReply->element[1]->str!=NULL?
							           scheduleReply->element[1]->str:"0");
					sscanf(scheduleReply->element[2]->str!=NULL?
							scheduleReply->element[2]->str:"0","%X",&ct.itemId);
					Poco::NumberParser::tryParseFloat(scheduleReply->element[3]->str!=NULL?
											scheduleReply->element[3]->str:"0",ct.scheduleVal);
					ct.scheduleTime = atoi(scheduleReply->element[4]->str!=NULL?
											scheduleReply->element[4]->str:"0");

					freeReplyObject(scheduleReply);
				}
				else
				{
					freeReplyObject(scheduleReply);
					return;
				}

				redisCommand(pCtrDbMs->_impl->_context,"del %s:%s",stringSchedKeyToken[1].data(),
						stringSchedKeyToken[2].data());

			}
			else return;

			pCtrDbMs->_impl->_pDevScheduleCallback(ct);
		}
		else
			return;
	}
	else if(pCtrDbMs->_impl->_callbackType == CallBackTye_ZhaoceInfo){
//		printf("===CallBackTye_ZhaoceInfo===\n");
		ZhaoCeExecInfoData zhaoceData;

		if(callBReply!=NULL && callBReply->elements>=4 && string(callBReply->element[3]->str)==string("sadd"))
		{
			StringTokenizer stringZhaoCeToken(callBReply->element[2]->str,":");
			if(stringZhaoCeToken.count() >= 3)
			{
				redisReply *zhaoceTaskReply =(redisReply *)redisCommand(pCtrDbMs->_impl->_context,
						"smembers %s:%s",stringZhaoCeToken[1].data(),stringZhaoCeToken[2].data());
				if(zhaoceTaskReply!= NULL)
				{
					for(unsigned int i=0; i<zhaoceTaskReply->elements; i++)
					{
						StringTokenizer strZhaoCeTask(zhaoceTaskReply->element[i]->str,":");
						if(strZhaoCeTask.count() >= 6)
						{
							zhaoceData.unitId = strZhaoCeTask[2];
							zhaoceData.deviceId = atoi(strZhaoCeTask[3].data());
							zhaoceData.termId = atoi(strZhaoCeTask[4].data());
							sscanf(strZhaoCeTask[5].data(),"%X",&zhaoceData.itemId);

							pCtrDbMs->_impl->_pZhaoceInfoCallback(zhaoceData);
						}
						redisCommand(pCtrDbMs->_impl->_context,"srem %s:%s %s",stringZhaoCeToken[1].data(),
								stringZhaoCeToken[2].data(),zhaoceTaskReply->element[i]->str);
					}

					freeReplyObject(zhaoceTaskReply);
				}
				else
				{
					freeReplyObject(zhaoceTaskReply);
					return;
				}
			}
		}
	}
	else if ( pCtrDbMs->_impl->_callbackType == CallBackTye_NewCollTask ) {
//		printf("===CallBackTye_NewCollTask===\n");
		CollTaskData collTdata;
		if(callBReply!=NULL && callBReply->elements >= 4)
		{
			if(string(callBReply->element[3]->str)==string("sadd"))
			{
				StringTokenizer strNewCollToken(callBReply->element[2]->str,":");
				if(strNewCollToken.count()>=5)
				{
					collTdata.unitId = strNewCollToken[3];
					collTdata.taskId = atoi(strNewCollToken[4].data());

					collTdata.status = COLLTASK_ST_VALID;

					redisReply *collTReply = (redisReply *)redisCommand(pCtrDbMs->_impl->_context,
							"hmget HS:TASK:%s:%s deviceId startDate freq round onfail "
							"lastCollTime",strNewCollToken[3].data(),strNewCollToken[4].data());
					if(collTReply != NULL && collTReply->elements >=6)
					{
						collTdata.deviceId = atoi(collTReply->element[0]->str!=NULL?
								collTReply->element[0]->str:"0");
						collTdata.startDate = atoi(collTReply->element[1]->str!=NULL?
								collTReply->element[1]->str:"0");
						collTdata.freq = atoi(collTReply->element[2]->str!=NULL?
								collTReply->element[2]->str:"0");
						collTdata.exeRound = atoi(collTReply->element[3]->str!=NULL?
								collTReply->element[3]->str:"0");
						collTdata.onfail = atoi(collTReply->element[4]->str!=NULL?
								collTReply->element[4]->str:"0");
						collTdata.lastCollTime = atoi(collTReply->element[5]->str!=NULL?
								collTReply->element[5]->str:"0");

						freeReplyObject(collTReply);
					}
					else
					{
						freeReplyObject(collTReply);
						return;
					}
				}
				else
					return;

				pCtrDbMs->_impl->_pNewCollTaskCallback(collTdata);
			}
			else if(string(callBReply->element[3]->str)==string("del"))
			{
				StringTokenizer strNewCollToken(callBReply->element[2]->str,":");
				if(strNewCollToken.count()>=5)
				{
					collTdata.unitId = strNewCollToken[3];
					collTdata.taskId = atoi(strNewCollToken[4].data());

					collTdata.status = COLLTASK_ST_INVALID;
				}

				pCtrDbMs->_impl->_pNewCollTaskCallback(collTdata);
			}

		}
		else
			return;

	}
	else if( pCtrDbMs->_impl->_callbackType == CallBackTye_SecCollPeriod )
	{
//		printf("===CallBackTye_SecCollPeriod===\n");
		UarcPeriodStateOfArch periodStateInfo;

		if(callBReply!=NULL && callBReply->elements>=4)
		{
			if(string(callBReply->element[3]->str)==string("sadd"))
			{
				StringTokenizer strHsArchToken(callBReply->element[2]->str,":");
				if(strHsArchToken.count() >= 4)
				{
					periodStateInfo.secCollPeriod = atoi(strHsArchToken[3].data()!=NULL?
							strHsArchToken[3].data():"0");
					periodStateInfo.status = ITEM_CODE_AVAILABLE;

					pCtrDbMs->_impl->_pSecCollPeriodCallback(periodStateInfo);
				}
				else
					return;
			}
			else if(string(callBReply->element[3]->str)==string("srem"))
			{
				StringTokenizer strHsArchToken(callBReply->element[2]->str,":");
				if(strHsArchToken.count()>=4)
				{
					periodStateInfo.secCollPeriod = atoi(strHsArchToken[3].data()!=NULL?
							strHsArchToken[3].data():"0");
					periodStateInfo.status = ITEM_CODE_UNAVAILABLE;

					pCtrDbMs->_impl->_pSecCollPeriodCallback(periodStateInfo);
				}
			}
		}
		else
			return;

	}
	else if( pCtrDbMs->_impl->_callbackType == CallBackTye_DataProcess )
	{
//		printf("===CallBackTye_DataProcess===\n");
		UarcProcDataItemArchive uarcProcDataIArc;
		UarcProcData uarcProcData;

		if(callBReply!=NULL && callBReply->elements >= 4
				&& string(callBReply->element[3]->str)==string("rpush"))
		{
			StringTokenizer strDataProcToken(callBReply->element[2]->str,":");
			if(strDataProcToken.count() >= 5)
			{
				uarcProcDataIArc.unitId = strDataProcToken[3];
				uarcProcData.unitId = strDataProcToken[3];
				sscanf(strDataProcToken[4].data(),"%X",&uarcProcDataIArc.itemId);
				sscanf(strDataProcToken[4].data(),"%X",&uarcProcData.itemId);

				redisReply *procDItemReply = (redisReply *)redisCommand(pCtrDbMs->_impl->_context,
						"hmget HS:ARCH:%s:%X termId itemType srcType upLimit downLimit warnTypeMask baseval "
						"coefficient warnStoreSql",uarcProcDataIArc.unitId.data(),uarcProcDataIArc.itemId);

				if(procDItemReply != NULL && procDItemReply->elements >= 9)
				{
					uarcProcDataIArc.termId = atoi(procDItemReply->element[0]->str!=NULL?
							procDItemReply->element[0]->str:"0");
					uarcProcDataIArc.itemType = atoi(procDItemReply->element[1]->str!=NULL?
							procDItemReply->element[1]->str:"0");
					uarcProcDataIArc.srcType = atoi(procDItemReply->element[2]->str!=NULL?
										procDItemReply->element[2]->str:"0");
					Poco::NumberParser::tryParseFloat(procDItemReply->element[3]->str!=NULL?
							procDItemReply->element[3]->str:"0",uarcProcDataIArc.upLimitVal);

					Poco::NumberParser::tryParseFloat(procDItemReply->element[4]->str!=NULL?
							procDItemReply->element[4]->str:"0",uarcProcDataIArc.downLimitVal);

					uarcProcDataIArc.warnTypeMask = atoi(procDItemReply->element[5]->str!=NULL?
							procDItemReply->element[5]->str:"0");
					Poco::NumberParser::tryParseFloat(procDItemReply->element[6]->str!=NULL?
							procDItemReply->element[6]->str:"0",uarcProcDataIArc.baseVal);
					Poco::NumberParser::tryParseFloat(procDItemReply->element[7]->str!=NULL?
							procDItemReply->element[7]->str:"0",uarcProcDataIArc.coefficient);
					uarcProcDataIArc.warnStoreSql = string(procDItemReply->element[8]->str!=NULL?
							procDItemReply->element[8]->str:"0");

				}
				freeReplyObject(procDItemReply);

				redisReply *listLenReply = (redisReply *)redisCommand(pCtrDbMs->_impl->_context,
						"llen LST:TIME:%s:%X",uarcProcData.unitId.data(),uarcProcData.itemId);

				if(listLenReply!=NULL && REDIS_REPLY_INTEGER==listLenReply->type)
				{
					redisReply *lastTimeReply = (redisReply *)redisCommand(pCtrDbMs->_impl->_context,
							"lindex LST:TIME:%s:%X %d",uarcProcData.unitId.data(),uarcProcData.itemId,
							listLenReply->integer-1);

					if(lastTimeReply!=NULL && REDIS_REPLY_STRING==lastTimeReply->type)
					{
						uarcProcData.timeVal = atoi(lastTimeReply->str);
						redisReply *procDataReply = (redisReply *)redisCommand(pCtrDbMs->_impl->_context,
								"get STR:DATA:%s:%X:%d",uarcProcData.unitId.data(),uarcProcData.itemId,
								uarcProcData.timeVal);
						if(procDataReply != NULL && procDataReply->type == REDIS_REPLY_STRING)
						{
							Poco::NumberParser::tryParseFloat(procDataReply->str,uarcProcData.dataVal);

							freeReplyObject(procDataReply);
						}
						else
						{
							freeReplyObject(procDataReply);
							freeReplyObject(lastTimeReply);
							freeReplyObject(listLenReply);

							return;
						}

						freeReplyObject(lastTimeReply);
					}
					else
					{
						freeReplyObject(lastTimeReply);
						freeReplyObject(listLenReply);
						return;
					}

					freeReplyObject(listLenReply);
				}
				else
				{
					freeReplyObject(listLenReply);
					return;
				}
			}
			else
				return;

			pCtrDbMs->_impl->_pDataProcessCallback(uarcProcDataIArc,uarcProcData);
		}
		else
			return;
	}
	else if( pCtrDbMs->_impl->_callbackType == CallBackTye_ZhaoceData )
	{
//		printf("===CallBackTye_ZhaoceData===\n");
		ZhaoCeData zhaoceDa;
		if(callBReply!=NULL && callBReply->elements >= 4 &&
				string(callBReply->element[3]->str)==string("set"))
		{
			StringTokenizer strZhaoceKeyToken(callBReply->element[2]->str,":");

			if(strZhaoceKeyToken.count() >= 7)
			{
				zhaoceDa.unitId = strZhaoceKeyToken[3];
				zhaoceDa.deviceId = atoi(strZhaoceKeyToken[4].data());
				zhaoceDa.termId = atoi(strZhaoceKeyToken[5].data());
				sscanf(strZhaoceKeyToken[6].data(),"%X",&zhaoceDa.itemId);

				redisReply* moniDataReply = (redisReply*)redisCommand(pCtrDbMs->_impl->_context,
						"get %s:%s:%s:%s:%s:%s",strZhaoceKeyToken[1].data(),
						strZhaoceKeyToken[2].data(),strZhaoceKeyToken[3].data(),
						strZhaoceKeyToken[4].data(),strZhaoceKeyToken[5].data(),
						strZhaoceKeyToken[6].data());
				if(moniDataReply != NULL && moniDataReply->type == REDIS_REPLY_STRING)
				{
					StringTokenizer strZhaoceDataToken(moniDataReply->str,":");
					if(strZhaoceDataToken.count() >= 2)
					{
						zhaoceDa.zhaoceTime = atoi(strZhaoceDataToken[0].data());
						Poco::NumberParser::tryParseFloat(strZhaoceDataToken[1].data(),
								zhaoceDa.zhaoCeValue);
					}

				}
				freeReplyObject(moniDataReply);
			}

			pCtrDbMs->_impl->_pZhaoceDataCallback(zhaoceDa,pCtrDbMs);
		}
		else
			return;
	}
	else if( pCtrDbMs->_impl->_callbackType == CallBackTye_DeviceInfo )
	{
//		printf("===CallBackTye_DeviceInfo===\n");
		stDeviceInfo stDevInfo;

		if(callBReply!=NULL && callBReply->elements >= 4)
		{
			if(string(callBReply->element[3]->str)==string("sadd"))
			{
				StringTokenizer strDevInfoKeyToken(callBReply->element[2]->str,":");

				if(strDevInfoKeyToken.count() >= 4)
				{
					stDevInfo.id = atoi(strDevInfoKeyToken[3].data());
					stDevInfo.status = DEVICE_ST_AVAILABLE;

					redisReply* devInfoReply = (redisReply*)redisCommand(pCtrDbMs->_impl->_context,
							"hmget HS:DEVICE:%d address port identity linkType protocol "
							"pulsePeriod lastConnTime",stDevInfo.id);
					if(devInfoReply != NULL && devInfoReply->elements >= 7)
					{
						sprintf(stDevInfo.ip,devInfoReply->element[0]->str != NULL?
								devInfoReply->element[0]->str:"0");
						stDevInfo.port = atoi(devInfoReply->element[1]->str != NULL?
								devInfoReply->element[1]->str:"0");
						stDevInfo.identity = std::string(devInfoReply->element[2]->str != NULL?
								devInfoReply->element[2]->str:"");
						stDevInfo.link_type = atoi(devInfoReply->element[3]->str != NULL?
								devInfoReply->element[3]->str:"0");
						stDevInfo.protocol_type = atoi(devInfoReply->element[4]->str != NULL?
								devInfoReply->element[4]->str:"0");
						stDevInfo.heart_period = atoi(devInfoReply->element[5]->str != NULL?
								devInfoReply->element[5]->str:"0");
						stDevInfo.last_connect_time = atoi(devInfoReply->element[6]->str != NULL?
								devInfoReply->element[6]->str:"0");

						freeReplyObject(devInfoReply);

						pCtrDbMs->_impl->_pDeviceInfoCallback(stDevInfo);
					}
					else
					{
						freeReplyObject(devInfoReply);
						return;
					}
				}
			}
			else if(string(callBReply->element[3]->str)==string("del"))
			{
				StringTokenizer strDevInfoKeyToken(callBReply->element[2]->str,":");

				if(strDevInfoKeyToken.count() >= 4)
				{
					stDevInfo.id = atoi(strDevInfoKeyToken[3].data());
					stDevInfo.status = DEVICE_ST_UNAVAILABLE;

					pCtrDbMs->_impl->_pDeviceInfoCallback(stDevInfo);
				}
			}
		}
	}else if (pCtrDbMs->_impl->_callbackType == CallBackTye_CallBack)
	{
//		printf("===CallBackTye_CallBack===\n");
		if (callBReply!=NULL && callBReply->elements >= 4)
		{
			RtDbMsCallBack *callback = NULL;
			map<string, RtDbMsCallBack *>::iterator iter =
					pCtrDbMs->_impl->_pCallBack.find(pCtrDbMs->_impl->_callbackName);
			if (iter != pCtrDbMs->_impl->_pCallBack.end())
			{
				callback = iter->second;
				callback(pCtrDbMs);
			}else {
				printf("%s not find.\n", pCtrDbMs->_impl->_callbackName.c_str());
			}
		}
	}else {
		printf("===CallBackTye_None===\n");
	}
}

CRtDbMs::CRtDbMs(std::string ip,int port) :  _impl(new impl)
{
	userCancel = false;
	_impl->_base = event_base_new();
	_impl->_acontext = redisAsyncConnect(ip.data(), port);
	_impl->_context = redisConnect(ip.data(), port);
	_impl->_mutex = new FastMutex();
	redisLibeventAttach(_impl->_acontext,_impl->_base);
}

CRtDbMs::~CRtDbMs()
{
	if(_impl->_context) redisFree( _impl->_context);
	if(_impl->_acontext) redisAsyncFree( _impl->_acontext);
	if(_impl->_base) event_base_free(_impl->_base);
	if(_impl->_mutex) delete _impl->_mutex;
	if ( _impl ) delete _impl;
	_impl=NULL;
}

bool CRtDbMs::selectDB(int db)
{
	if (db < 0 || db >= 16)
	{
		return false;
	}
	redisReply *reply = (redisReply*)redisCommand(_impl->_context,
			"SELECT %d", db);
	if(reply != NULL && REDIS_REPLY_ERROR != reply->type)
	{
		freeReplyObject(reply);
		return true;
	}
	else
	{
		return false;
	}
}

bool CRtDbMs::setZhaoceDataCallback(RtDbMsCallback_ZhaoceData callback)
{
	_impl->_pZhaoceDataCallback = callback;
	_impl->_callbackType = CallBackTye_ZhaoceData;
	int sendCommSuc;
	sendCommSuc = redisAsyncCommand(_impl->_acontext, _redisCallBack, this,
            		"psubscribe __keyspace@*__:STR:MONITOR:*");
	if(REDIS_OK == sendCommSuc)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CRtDbMs::setDeviceCntlCallback(RtDbMsCallback_DeviceCntl callback)
{
	_impl->_pDeviceCntlCallback = callback;
	_impl->_callbackType = CallBackTye_DeviceCntl;
	int sendCommSuc;
	sendCommSuc = redisAsyncCommand(_impl->_acontext, _redisCallBack, this,
			                       "psubscribe __keyspace@*__:HS:CONTROL");
	if(REDIS_OK == sendCommSuc)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CRtDbMs::setDevScheduleCallback(RtDbMsCallback_DevSchedule callback)
{
	_impl->_pDevScheduleCallback = callback;
	_impl->_callbackType = CallBackTye_DevSchedule;
	int sendScheduleSuc;
	sendScheduleSuc = redisAsyncCommand(_impl->_acontext, _redisCallBack, this,
			"psubscribe __keyspace@*__:HS:SCHEDULE");
	if(REDIS_OK == sendScheduleSuc)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CRtDbMs::setZhaoceCallback(RtDbMsCallback_ZhaoceInfo callback)
{
	_impl->_pZhaoceInfoCallback = callback;
	_impl->_callbackType = CallBackTye_ZhaoceInfo;
	int sendCommSuc;
	sendCommSuc = redisAsyncCommand(_impl->_acontext, _redisCallBack, this,
									"psubscribe __keyspace@*__:SET:MONITOR");
	if(REDIS_OK == sendCommSuc)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CRtDbMs::setnewCollTaskCallback(RtDbMsCallback_NewCollTask callback)
{
	_impl->_pNewCollTaskCallback = callback;
	_impl->_callbackType = CallBackTye_NewCollTask;
	int sendCommSuc;
	sendCommSuc = redisAsyncCommand(_impl->_acontext, _redisCallBack, this,
									"psubscribe __keyspace@*__:SET:TASK:*");
	if(REDIS_OK == sendCommSuc)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CRtDbMs::setSecCollPeriodCallback(RtDbMsCallback_SecCollPeriod callback)
{
	_impl->_pSecCollPeriodCallback = callback;
	_impl->_callbackType = CallBackTye_SecCollPeriod;
	int sendCommSuc;
	sendCommSuc = redisAsyncCommand(_impl->_acontext, _redisCallBack, this,
			"psubscribe __keyspace@*__:SET:ALLPERIOD:*");
	if(REDIS_OK == sendCommSuc)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CRtDbMs::setDataProcCallback(RtDbMsCallback_DataProcess callback)
{
	_impl->_pDataProcessCallback = callback;
	_impl->_callbackType = CallBackTye_DataProcess;
	int sendCommSuc;
	sendCommSuc = redisAsyncCommand(_impl->_acontext, _redisCallBack, this,
			"psubscribe __keyspace@*__:LST:TIME:*");
	if(REDIS_OK == sendCommSuc)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CRtDbMs::setDeviceInfoCallback(RtDbMsCallback_DeviceInfo callback)
{
	_impl->_pDeviceInfoCallback = callback;
	_impl->_callbackType = CallBackTye_DeviceInfo;
	int sendCommSuc;
	sendCommSuc = redisAsyncCommand(_impl->_acontext, _redisCallBack, this,
			"psubscribe __keyspace@*__:SET:DEVICE:*");
	if(REDIS_OK == sendCommSuc)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CRtDbMs::setCallBack(string name, string key, RtDbMsCallBack callback)
{
	_impl->_pCallBack.insert(pair<string, RtDbMsCallBack *>(name, callback));
	_impl->_callbackType = CallBackTye_CallBack;
	_impl->_callbackName = name;

	int sendCommSuc;
	sendCommSuc = redisAsyncCommand(_impl->_acontext, _redisCallBack, this,
			"psubscribe __keyspace@*__:%s", key.c_str());
	if(REDIS_OK == sendCommSuc)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CRtDbMs::start()
{
	//event_base_dispatch(_impl->_base);

	while(true)
	{
		if(userCancel)
		{
			userCancel = false;
			break;
		}
		struct timeval one_sec;
		one_sec.tv_sec = 1;
		one_sec.tv_usec = 0;

		event_base_loopexit(_impl->_base,&one_sec);
		event_base_dispatch(_impl->_base);

	}
}
void CRtDbMs::stop()
{
	userCancel = true;

	//event_base_loopbreak(_impl->_base);
}

bool CRtDbMs::saveCollTask(CollTask *savedCollTask)    //存放采集任务
{
	FastMutex::ScopedLock lock(*_impl->_mutex);
	if(_impl->_context != NULL && _impl->_context->err)
		return false;

	redisReply *itemArcReply;      //指标档案返回
	int deviceIdChecked;       //检测的设备Id;因不初始化含有警�?
	int taskId;						   //任务编号
	vector<int> itemsInDevice;  		//记录每个TASK_ID对应的指标项

	/********删除内存库原有的相同unitId的采集任�?****/
	if(COLLTASK_ST_VALID==savedCollTask->taskStatus || COLLTASK_ST_INVALID==savedCollTask->taskStatus)
	{
		//redisReply *collTaskKeyReply =(redisReply*)redisCommand(_impl->_context,
		//		"keys SET:TASK:%s*",savedCollTask->unitId.data());
		redisReply *collTaskKeyReply =(redisReply*)redisCommand(_impl->_context,
						"smembers SET:ALL:SETTASK");
		if(collTaskKeyReply != NULL && REDIS_REPLY_ERROR != collTaskKeyReply->type &&
				collTaskKeyReply->elements >= 1)
		{
			for(unsigned int i=0; i < collTaskKeyReply->elements;i++)
			{
				StringTokenizer strCollTaskKeyToken(collTaskKeyReply->element[i]->str,":");
				if(strCollTaskKeyToken.count()>=4 &&
						strcmp(strCollTaskKeyToken[2].data(),savedCollTask->unitId.data())==0)
				{
					redisReply *setTReply =(redisReply*)redisCommand(_impl->_context,"del SET:TASK:%s:%s",
							strCollTaskKeyToken[2].data(),strCollTaskKeyToken[3].data());
					redisReply *setTAReply =(redisReply*)redisCommand(_impl->_context,
							                     "srem SET:ALL:SETTASK SET:TASK:%s:%s",
							strCollTaskKeyToken[2].data(),strCollTaskKeyToken[3].data());
					redisReply *HSTReply =(redisReply*)redisCommand(_impl->_context,"del HS:TASK:%s:%s",
							strCollTaskKeyToken[2].data(),strCollTaskKeyToken[3].data());
					redisReply *HSTAReply =(redisReply*)redisCommand(_impl->_context,"srem SET:ALL:HSTASK HS:TASK:%s:%s",
												strCollTaskKeyToken[2].data(),strCollTaskKeyToken[3].data());
					redisReply *SETTDReply =(redisReply*)redisCommand(_impl->_context,"del SET:TASKDATA:%s:%s",
							strCollTaskKeyToken[2].data(),strCollTaskKeyToken[3].data());

					freeReplyObject(setTReply);
					freeReplyObject(setTAReply);
					freeReplyObject(HSTReply);
					freeReplyObject(HSTAReply);
					freeReplyObject(SETTDReply);
				}
			}
		}
		freeReplyObject(collTaskKeyReply);
	}

	if(COLLTASK_ST_VALID==savedCollTask->taskStatus)
	{
		redisReply *SetUnitReply =(redisReply*)redisCommand(_impl->_context,"sadd SET:UNIT %s",
				                 savedCollTask->unitId.data());
		freeReplyObject(SetUnitReply);
		for(vector<int>::iterator it=savedCollTask->itemIds.begin();it!=savedCollTask->itemIds.end();it++)
		{
			redisReply *SetItemReply =(redisReply*)redisCommand(_impl->_context,"sadd SET:ITEM:%s %X",
					              savedCollTask->unitId.data(),*it);
			freeReplyObject(SetItemReply);
		}

		vector<int>::iterator itemsIterator = savedCollTask->itemIds.begin();
		while(itemsIterator != savedCollTask->itemIds.end())
		{
			taskId = savedCollTask->itemIds.size();

			int itemId = *itemsIterator;
			itemsInDevice.push_back(itemId);
			itemsIterator = savedCollTask->itemIds.erase(itemsIterator);

			itemArcReply =(redisReply*)redisCommand(_impl->_context,"hget HS:ARCH:%s:%X deviceId",
					savedCollTask->unitId.data(),itemId);  //

			if(itemArcReply != NULL && itemArcReply->type != REDIS_REPLY_NIL)
			{
				deviceIdChecked = atoi(itemArcReply->str);
				freeReplyObject(itemArcReply);

				for(vector<int>::iterator it=itemsIterator;it!=savedCollTask->itemIds.end();)
				{
					itemArcReply =(redisReply*)redisCommand(_impl->_context,
							"hget HS:ARCH:%s:%X deviceId",savedCollTask->unitId.data(),*it);   //

					if(itemArcReply != NULL &&itemArcReply->type != REDIS_REPLY_NIL &&
							atoi(itemArcReply->str) == deviceIdChecked)
					{
						itemsInDevice.push_back(*it);
						it = savedCollTask->itemIds.erase(it);
					}
					else
					{
						it++;
					}
					freeReplyObject(itemArcReply);
				}

				redisReply *hSetTReply = (redisReply*)redisCommand(_impl->_context,"hmset HS:TASK:%s:%d status %d deviceId %d "
						"startDate %d freq %d round %d onfail %d lastCollTime %d",
						savedCollTask->unitId.data(),taskId,COLLTASK_ST_NO_EXEC,deviceIdChecked,
						savedCollTask->startDate,savedCollTask->freq,savedCollTask->collRound,
						savedCollTask->onFail,savedCollTask->lastCollTime);
				redisReply *saddSetAHReply =(redisReply*)redisCommand(_impl->_context,"sadd SET:ALL:HSTASK HS:TASK:%s:%d",
						      savedCollTask->unitId.data(),taskId);

				redisReply *saddSetTReply =(redisReply*)redisCommand(_impl->_context,"sadd SET:TASK:%s:%d HS:TASK:%s:%d",
						savedCollTask->unitId.data(),taskId,savedCollTask->unitId.data(),taskId);
				redisReply *saddSetASReply =(redisReply*)redisCommand(_impl->_context,"sadd SET:ALL:SETTASK SET:TASK:%s:%d",
										      savedCollTask->unitId.data(),taskId);
				freeReplyObject(hSetTReply);
				freeReplyObject(saddSetAHReply);
				freeReplyObject(saddSetTReply);
				freeReplyObject(saddSetASReply);

				for(unsigned int i=0; i<itemsInDevice.size();i++)
				{
					redisReply *saddTDReply =(redisReply*)redisCommand(_impl->_context,"sadd SET:TASKDATA:%s:%d HS:ARCH:%s:%X",
							savedCollTask->unitId.data(),taskId,savedCollTask->unitId.data(),itemsInDevice[i]);
					freeReplyObject(saddTDReply);
				}
			}

			itemsInDevice.clear();
		}
	}

	return true;
}
bool CRtDbMs::saveItemInfo(ItemInfo *savedItemInfo)       //存放指标档案
{
	FastMutex::ScopedLock lock(*_impl->_mutex);
	if(_impl->_context != NULL && _impl->_context->err)
			return false;
	if(ITEM_ST_INVALID==savedItemInfo->status)
	{
		redisReply *delArchReply = (redisReply*)redisCommand(_impl->_context,"del HS:ARCH:%s:%X",
									savedItemInfo->unitId.data(),savedItemInfo->itemId);
		redisReply *sremArchReply = (redisReply*)redisCommand(_impl->_context,"srem SET:ALL:ARCH HS:ARCH:%s:%X",
											savedItemInfo->unitId.data(),savedItemInfo->itemId);

		freeReplyObject(delArchReply);
		freeReplyObject(sremArchReply);

		if( ITEM_SRTY_DEVICE==savedItemInfo->srcType )
		{
			redisReply *delWaitBReply = (redisReply*)redisCommand(_impl->_context,
					"srem SET:WAITBACK:%d:%d:%X HS:ARCH:%s:%X",savedItemInfo->deviceId,
					savedItemInfo->termId,savedItemInfo->itemId,savedItemInfo->unitId.data(),
					savedItemInfo->itemId);
			freeReplyObject(delWaitBReply);
		}
		redisReply *delSecPReply = (redisReply*)redisCommand(_impl->_context,"del STR:SECPERIOD:%s:%X:%d",
				savedItemInfo->unitId.data(),savedItemInfo->itemId,savedItemInfo->secCollPeriod);
		redisReply *sremPReply = (redisReply*)redisCommand(_impl->_context,"srem SET:ALLPERIOD:%d %s:%X",
				savedItemInfo->secCollPeriod,savedItemInfo->unitId.data(),savedItemInfo->itemId);
		freeReplyObject(delSecPReply);
		freeReplyObject(sremPReply);
	}
	else if(ITEM_ST_VALID == savedItemInfo->status)
	{
		redisReply *saddUReply = (redisReply*)redisCommand(_impl->_context,"sadd SET:UNIT %s",
				savedItemInfo->unitId.data());
		redisReply *saddIReply = (redisReply*)redisCommand(_impl->_context,"sadd SET:ITEM:%s %X",
				savedItemInfo->unitId.data(),savedItemInfo->itemId);

		redisReply *hsArchReply = (redisReply*)redisCommand(_impl->_context,"hmset HS:ARCH:%s:%X "
				"itemId %X unitId %s deviceId %d termId %d itemType %d srcType %d upLimit %f "
				"downLimit %f warnTypeMask %d baseval %f coefficient %f dayStoreTime %s "
				"dayStoreSql %s warnStoreSql %s secCollPeriod %d secCollSql %s secCollOtherSql %s "
				"storeType %d status %d",
				savedItemInfo->unitId.data(),savedItemInfo->itemId,savedItemInfo->itemId,
				savedItemInfo->unitId.data(),savedItemInfo->deviceId,savedItemInfo->termId,
				savedItemInfo->itemType,savedItemInfo->srcType,savedItemInfo->upLimitVal,
				savedItemInfo->downLimitVal,savedItemInfo->warnTypeMask,savedItemInfo->baseVal,
				savedItemInfo->coefficient,savedItemInfo->dayStoreTime.data(),savedItemInfo->dayStoreSql.data(),
				savedItemInfo->warnStoreSql.data(),savedItemInfo->secCollPeriod,savedItemInfo->secCollSql.data(),
				savedItemInfo->secCollOtherSql.c_str(),savedItemInfo->storeType,savedItemInfo->status);
		redisReply *saddAArchReply = (redisReply*)redisCommand(_impl->_context,"sadd SET:ALL:ARCH HS:ARCH:%s:%X",
						savedItemInfo->unitId.data(),savedItemInfo->itemId);

		freeReplyObject(saddUReply);
		freeReplyObject(saddIReply);
		freeReplyObject(hsArchReply);
		freeReplyObject(saddAArchReply);

		if( ITEM_SRTY_DEVICE==savedItemInfo->srcType )
		{
			redisReply *saddSWBReply = (redisReply*)redisCommand(_impl->_context,"sadd SET:WAITBACK:%d:%d:%X "
					"HS:ARCH:%s:%X",savedItemInfo->deviceId,savedItemInfo->termId,savedItemInfo->itemId,
					savedItemInfo->unitId.data(),savedItemInfo->itemId);
			freeReplyObject(saddSWBReply);
		}

		//存放归集时间集合，并设置过期时间
		if(ITEM_STORE_T_DAYSTORE==savedItemInfo->storeType)
		{
			/*
			LocalDateTime nowLcDateTime;
			DateTime recvDateTime;

			ostringstream recvDTStream;
			recvDTStream<<nowLcDateTime.year()<<"-"<<nowLcDateTime.month()<<"-"
					<<nowLcDateTime.day()<<" "<<savedItemInfo->dayStoreTime;
			string recvDateTimeStr = recvDTStream.str();

			int tzd;
			bool ok = DateTimeParser::tryParse(recvDateTimeStr,recvDateTime,tzd);
			if(true == ok)
			{
				LocalDateTime recvLcDateTime(recvDateTime);
				LocalDateTime nowLcDateTime;
				Timespan collTimeSpan = recvLcDateTime - nowLcDateTime;

				redisCommand(_impl->_context,"sadd SET:COLLTIME:%s:%X %s",savedItemInfo->unitId.data(),
						savedItemInfo->itemId,savedItemInfo->dayStoreTime.data());
				if(collTimeSpan < Timespan::TimeDiff(0))
				{
					collTimeSpan += Timespan(1,0,0,0,0);
				}
				redisCommand(_impl->_context,"expire SET:COLLTIME:%s:%X %d",savedItemInfo->unitId.data(),
						savedItemInfo->itemId,collTimeSpan.totalSeconds());
			}
			*/
		}
		else if(ITEM_SOTRE_T_DOBLSAMP==savedItemInfo->storeType)
		{
			redisReply *setStrPReply = (redisReply*)redisCommand(_impl->_context,"set STR:SECPERIOD:%s:%X:%d "
					"HS:ARCH:%s:%X",savedItemInfo->unitId.data(),savedItemInfo->itemId,savedItemInfo->secCollPeriod,
					     savedItemInfo->unitId.data(),savedItemInfo->itemId);
			redisReply *saddPReply = (redisReply*)redisCommand(_impl->_context,"sadd SET:ALLPERIOD:%d %s:%X",
					savedItemInfo->secCollPeriod,savedItemInfo->unitId.data(),savedItemInfo->itemId);
			freeReplyObject(setStrPReply);
			freeReplyObject(saddPReply);
		}
	}

	return true;
}

bool CRtDbMs::saveCalculateTask(CalculateTask *savedCalculateTask)    //存放计算任务
{
	FastMutex::ScopedLock lock(*_impl->_mutex);
	if(_impl->_context != NULL && _impl->_context->err)
			return false;

	if(CALC_T_UNAVAILABLE==savedCalculateTask->status)
	{
		redisReply *sremCalcPReply = (redisReply*)redisCommand(_impl->_context,"srem SET:CALC:%s %d",
				savedCalculateTask->unitId.data(),savedCalculateTask->calcId);
		redisReply *delCalcReply = (redisReply*)redisCommand(_impl->_context,"del HS:CALC:%s:%d",
				savedCalculateTask->unitId.data(),savedCalculateTask->calcId);
		redisReply *sremHCReply = (redisReply*)redisCommand(_impl->_context,"srem SET:ALL:HSCALC HS:CALC:%s:%d",
						savedCalculateTask->unitId.data(),savedCalculateTask->calcId);
		freeReplyObject(sremCalcPReply);
		freeReplyObject(delCalcReply);
		freeReplyObject(sremHCReply);
	}
	else if(CALC_T_AVAILABLE == savedCalculateTask->status)
	{
		redisReply *saddUReply = (redisReply*)redisCommand(_impl->_context,"sadd SET:UNIT %s",
				savedCalculateTask->unitId.data());
		redisReply *saddIReply = (redisReply*)redisCommand(_impl->_context,"sadd SET:ITEM:%s %X",
				savedCalculateTask->unitId.data(),savedCalculateTask->itemId);

		redisReply *saddCalcReply = (redisReply*)redisCommand(_impl->_context,"sadd SET:CALC:%s %d",
				savedCalculateTask->unitId.data(),savedCalculateTask->calcId);

		redisReply *hsCalcReply = (redisReply*)redisCommand(_impl->_context,"hmset HS:CALC:%s:%d "
				"unitId %s itemId %X expr %s paramMask %d priority %d freq %d lastUpdate %d status %d",
				savedCalculateTask->unitId.data(),savedCalculateTask->calcId,savedCalculateTask->unitId.data(),
				savedCalculateTask->itemId,savedCalculateTask->formula.data(),savedCalculateTask->paramMask,
				savedCalculateTask->priority,savedCalculateTask->freq,savedCalculateTask->lastUpdate,
				CALC_T_NO_EXEC);
		redisReply *saddACReply = (redisReply*)redisCommand(_impl->_context,"sadd SET:ALL:HSCALC HS:CALC:%s:%d",
						savedCalculateTask->unitId.data(),savedCalculateTask->calcId);

		freeReplyObject(saddUReply);
		freeReplyObject(saddIReply);
		freeReplyObject(saddCalcReply);
		freeReplyObject(hsCalcReply);
		freeReplyObject(saddACReply);

		for(int i=0; i<FORMULA_PARAM_NUM; i++)
		{
			if(savedCalculateTask->paramMask & (1<<i))
			{
				redisReply *hsCReply = (redisReply*)redisCommand(_impl->_context,"hmset HS:CALC:%s:%d U%02d %s I%02d %X",
						savedCalculateTask->unitId.data(),savedCalculateTask->calcId,i,
						savedCalculateTask->params[i].unitId.data(),i,savedCalculateTask->params[i].itemId);

				freeReplyObject(hsCReply);
			}
		}
	}

	return true;
}

bool CRtDbMs::saveZhaoceTask(ZhaoceTask *savedZhaoceTask)    //存放召测任务
{
	FastMutex::ScopedLock lock(*_impl->_mutex);
	if(_impl->_context != NULL && _impl->_context->err)
			return false;

	redisReply *saddUReply = (redisReply*)redisCommand(_impl->_context,"sadd SET:UNIT %s",savedZhaoceTask->unitId.data());
	redisReply *saddIReply = (redisReply*)redisCommand(_impl->_context,"sadd SET:ITEM:%s %X",savedZhaoceTask->unitId.data(),
			                                           savedZhaoceTask->itemId);

	redisReply *saddMReply = (redisReply*)redisCommand(_impl->_context,"sadd SET:MONITOR STR:MONITOR:%s:%d:%d:%X",
					savedZhaoceTask->unitId.data(),savedZhaoceTask->deviceId,
					savedZhaoceTask->termId,savedZhaoceTask->itemId);

	freeReplyObject(saddUReply);
	freeReplyObject(saddIReply);
	freeReplyObject(saddMReply);

	return true;
}

bool CRtDbMs::saveControlTask(CtrlTask *savedCtrlTask)        //存放控制任务
{
	FastMutex::ScopedLock lock(*_impl->_mutex);
	if(_impl->_context != NULL && _impl->_context->err)
			return false;

	redisReply *hsCtrlReply = (redisReply*)redisCommand(_impl->_context,"hmset HS:CONTROL "
			"unitId %s deviceId %d termId %d itemId %X ctrlValue %f",savedCtrlTask->unitId.data(),
			savedCtrlTask->deviceId,savedCtrlTask->termId,savedCtrlTask->itemId,savedCtrlTask->ctrlVal);
	freeReplyObject(hsCtrlReply);

	return true;
}

bool CRtDbMs::saveScheduleTask(ScheduleTask *savedScheduleTask)  //存放配比计划任务
{
	if(_impl->_context != NULL && _impl->_context->err)
		return false;

	redisReply *hsScheduleReply = (redisReply*)redisCommand(_impl->_context,"hmset HS:SCHEDULE "
			"deviceId %d termId %d itemId %X scheduleVal %f scheduleTime %d",savedScheduleTask->deviceId,
			savedScheduleTask->termId,savedScheduleTask->itemId,savedScheduleTask->scheduleVal,
			savedScheduleTask->scheduleTime);
	freeReplyObject(hsScheduleReply);

	return true;
}
bool CRtDbMs::saveDeviceInfo(DeviceInfo *savedDevInfo)   //存放设备档案信息
{
	FastMutex::ScopedLock lock(*_impl->_mutex);
	if(_impl->_context != NULL && _impl->_context->err)
			return false;

	if(DEVICE_ST_UNAVAILABLE==savedDevInfo->status)
	{
		redisReply *delHsDevReply = (redisReply*)redisCommand(_impl->_context,"del HS:DEVICE:%d",
				savedDevInfo->deviceId);
		redisReply *sremHsDevReply = (redisReply*)redisCommand(_impl->_context,
				"srem SET:ALL:DEVICE HS:DEVICE:%d",savedDevInfo->deviceId);
		redisReply *delStrDevReply = (redisReply*)redisCommand(_impl->_context," del STR:DEVICE:%d:%d:%s",
				savedDevInfo->linkType,savedDevInfo->port,savedDevInfo->identity.data());
		redisReply *delSetDevReply = (redisReply*)redisCommand(_impl->_context,"del SET:DEVICE:%d",
				savedDevInfo->deviceId);

		freeReplyObject(delHsDevReply);
		freeReplyObject(sremHsDevReply);
		freeReplyObject(delStrDevReply);
		freeReplyObject(delSetDevReply);
	}
	else if(DEVICE_ST_AVAILABLE == savedDevInfo->status)
	{
		redisReply *saveDeviceInfo = (redisReply*)redisCommand(_impl->_context,"hmset "
				"HS:DEVICE:%d status %d address %s port %d identity %s linkType %d "
				"protocol %d pulsePeriod %d lastConnTime %d",
				savedDevInfo->deviceId,savedDevInfo->status,savedDevInfo->address.data(),
				savedDevInfo->port,savedDevInfo->identity.data(),savedDevInfo->linkType,
				savedDevInfo->protocol,savedDevInfo->pulsePeriod,savedDevInfo->lastConnTime);

		redisReply *saddADReply = (redisReply*)redisCommand(_impl->_context,"sadd SET:ALL:DEVICE "
						"HS:DEVICE:%d",savedDevInfo->deviceId);

		redisReply *setDeviceID =(redisReply*)redisCommand(_impl->_context,"set STR:DEVICE:%d:%d:%s %d",
				savedDevInfo->linkType,savedDevInfo->port,savedDevInfo->identity.data(),
				savedDevInfo->deviceId);

		redisReply *saveDeviceId = (redisReply*)redisCommand(_impl->_context,"sadd "
						"SET:DEVICE:%d HS:DEVICE:%d",savedDevInfo->deviceId,savedDevInfo->deviceId);

		if(saveDeviceInfo == NULL || setDeviceID == NULL || saveDeviceId->type == REDIS_REPLY_ERROR ||
				saveDeviceInfo->type == REDIS_REPLY_ERROR || saveDeviceInfo->type == REDIS_REPLY_ERROR)
		{
			freeReplyObject(saveDeviceId);
			freeReplyObject(saddADReply);
			freeReplyObject(saveDeviceInfo);
			freeReplyObject(setDeviceID);
			return false;
		}
		else
		{
			freeReplyObject(saveDeviceId);
			freeReplyObject(saddADReply);
			freeReplyObject(saveDeviceInfo);
			freeReplyObject(setDeviceID);
		}
	}

	return true;
}

bool CRtDbMs::getArchInfo(std::string unitId,int itemId,UarcStoreItemInfo *storeItemInfo)
{
	redisReply *storeItemReply =(redisReply*)redisCommand(_impl->_context,
			"hmget HS:ARCH:%s:%X itemType baseVal coefficient secCollSql "
			"secCollOtherSql storeType deviceId termId",unitId.c_str(),itemId);
	if(storeItemReply != NULL && storeItemReply->elements >= 6)
	{
		storeItemInfo->unitId = unitId;
		storeItemInfo->itemId = itemId;
		storeItemInfo->itemType = atoi(storeItemReply->element[0]->str!=NULL?
				storeItemReply->element[0]->str:"0");
		Poco::NumberParser::tryParseFloat(storeItemReply->element[1]->str != NULL?
				storeItemReply->element[1]->str:"0",storeItemInfo->baseVal);
		Poco::NumberParser::tryParseFloat(storeItemReply->element[2]->str != NULL?
				storeItemReply->element[2]->str:"0",storeItemInfo->coefficient);
		storeItemInfo->secCollSql = std::string(storeItemReply->element[3]->str!=NULL?
				storeItemReply->element[3]->str:"0");
		storeItemInfo->secCollOtherSql = std::string(storeItemReply->element[4]->str!=NULL?
				storeItemReply->element[4]->str:"0");
		storeItemInfo->storeType = atoi(storeItemReply->element[5]->str!=NULL?
				storeItemReply->element[5]->str:"0");
		storeItemInfo->deviceId = atoi(storeItemReply->element[6]->str!=NULL?
				storeItemReply->element[6]->str:"0");
		storeItemInfo->termId = atoi(storeItemReply->element[7]->str!=NULL?
				storeItemReply->element[7]->str:"0");

	}
	freeReplyObject(storeItemReply);

	return true;
}

bool CRtDbMs::getAllCollTask (std::vector<CollTaskData> *allCollTask)  //获得所有采集任�?
{
	FastMutex::ScopedLock lock(*_impl->_mutex);
	if(_impl->_context != NULL && _impl->_context->err)
		return false;

	CollTaskData tempCollTaskData;
	redisReply *unitReply;
	unitReply = (redisReply*)redisCommand(_impl->_context,"smembers SET:UNIT");
	if(unitReply!=NULL && REDIS_REPLY_NIL != unitReply->type &&
			REDIS_REPLY_ARRAY==unitReply->type)
	{
		unsigned int unitElemIndex = 0;
		while(unitElemIndex < unitReply->elements)
		{
			tempCollTaskData.unitId = std::string(unitReply->element[unitElemIndex]->str!= NULL?
					unitReply->element[unitElemIndex]->str:"0");

			redisReply *taskReply;
			//taskReply = (redisReply*)redisCommand(_impl->_context,"keys SET:TASK:%s:*",
			//		unitReply->element[unitElemIndex]->str);
			taskReply = (redisReply*)redisCommand(_impl->_context,"smembers SET:ALL:SETTASK");

			if(taskReply != NULL && REDIS_REPLY_NIL != taskReply->type &&
					REDIS_REPLY_ARRAY==taskReply->type)
			{
				unsigned int taskElemIndex =0;

				while(taskElemIndex < taskReply->elements)
				{
					StringTokenizer strCollTaskToken(taskReply->element[taskElemIndex]->str,":");
					if(strCollTaskToken.count()>=4 &&
							strcmp(strCollTaskToken[2].data(),unitReply->element[unitElemIndex]->str)==0)
					{
						tempCollTaskData.taskId = atoi(strCollTaskToken[3].data());

						redisReply *collTask = (redisReply*)redisCommand(_impl->_context,
								"hmget HS:TASK:%s:%s status deviceId startDate freq round "
								"onfail lastCollTime",strCollTaskToken[2].data(),strCollTaskToken[3].data());

						if(collTask!=NULL && REDIS_REPLY_ARRAY==collTask->type && collTask->elements>=7)
						{
							tempCollTaskData.status = atoi(collTask->element[0]->str != NULL?
									collTask->element[0]->str:"0");
							tempCollTaskData.deviceId = atoi(collTask->element[1]->str!=NULL?
									collTask->element[1]->str:"0");
							tempCollTaskData.startDate = atoi(collTask->element[2]->str!=NULL?
									collTask->element[2]->str:"0");
							tempCollTaskData.freq = atoi(collTask->element[3]->str!=NULL?
									collTask->element[3]->str:"0");
							tempCollTaskData.exeRound = atoi(collTask->element[4]->str!=NULL?
									collTask->element[4]->str:"0");
							tempCollTaskData.onfail = atoi(collTask->element[5]->str!=NULL?
									collTask->element[5]->str:"0");
							tempCollTaskData.lastCollTime = atoi(collTask->element[6]->str!=NULL?
									collTask->element[6]->str:"0");

							allCollTask->push_back(tempCollTaskData);
						}
						freeReplyObject(collTask);
					}

					taskElemIndex++;
				}  //while
			}
			freeReplyObject(taskReply);

			unitElemIndex++;
		}
	}
	freeReplyObject(unitReply);

	return true;
}

bool CRtDbMs::saveCollTaskPara(CollTaskData collTaskPara)      //存放采集任务参数
{
	FastMutex::ScopedLock lock(*_impl->_mutex);
	if(_impl->_context != NULL && _impl->_context->err)
		return false;

	//redisReply *checkParaReply = (redisReply*)redisCommand(_impl->_context,"keys HS:TASK:*");
	redisReply *checkParaReply = (redisReply*)redisCommand(_impl->_context,"smembers SET:ALL:HSTASK");

	if(checkParaReply != NULL && REDIS_REPLY_NIL != checkParaReply->type &&
			REDIS_REPLY_ARRAY==checkParaReply->type)
	{
		unsigned int taskElemIndex =0;

		while(taskElemIndex < checkParaReply->elements)
		{
			StringTokenizer strCollTaskToken(checkParaReply->element[taskElemIndex]->str,":");
			if(strCollTaskToken.count() >= 4)
			{
				int existTaskId = atoi(strCollTaskToken[3].data()!=NULL?strCollTaskToken[3].data():"0");

				if(0==strcmp(strCollTaskToken[2].data(),collTaskPara.unitId.data()) &&
						existTaskId==collTaskPara.taskId)
				{
					redisReply *saveTaskParaReply = (redisReply*)redisCommand(_impl->_context,"hmset HS:TASK:%s:%d status %d "
							"round %d",collTaskPara.unitId.data(),collTaskPara.taskId,collTaskPara.status,collTaskPara.exeRound);
					if(saveTaskParaReply != NULL && REDIS_REPLY_ERROR != saveTaskParaReply->type)
					{
						freeReplyObject(saveTaskParaReply);
						freeReplyObject(checkParaReply);
						return true;
					}
					else
					{
						freeReplyObject(saveTaskParaReply);
						freeReplyObject(checkParaReply);
						return false;
					}
				}
			}
			taskElemIndex++;
		}
		freeReplyObject(checkParaReply);
		return false;
	}
	else
	{
		freeReplyObject(checkParaReply);
		return false;
	}
}

bool CRtDbMs::getCollExecInfo (std::string unitId,int collTaskId,CollExecInfoData* collExecInfo)//获得采集执行信息
{
	FastMutex::ScopedLock lock(*_impl->_mutex);
	if(_impl->_context != NULL && _impl->_context->err)
			return false;

	collExecInfo->unitId = unitId;
	redisReply *taskItemReply = (redisReply*)redisCommand(_impl->_context,"smembers "
			"SET:TASKDATA:%s:%d",unitId.data(),collTaskId);

	if(taskItemReply != NULL && taskItemReply->type == REDIS_REPLY_ARRAY)
	{
		for(unsigned int i=0; i<taskItemReply->elements;i++)
		{
			redisReply *taskItemInfoReply = (redisReply*)redisCommand(_impl->_context,"hmget %s itemId "
					"deviceId termId",taskItemReply->element[i]->str);
			if(taskItemInfoReply!= NULL && taskItemInfoReply->elements >= 3)
			{
				int itemIdVal;
				int termIdVal;
				sscanf(taskItemInfoReply->element[0]->str!=NULL?
						taskItemInfoReply->element[0]->str:"0","%X",&itemIdVal);
				sscanf(taskItemInfoReply->element[2]->str!=NULL?
						taskItemInfoReply->element[2]->str:"0","%d",&termIdVal);
				collExecInfo->itemAndTerms.insert(pair<int,int>(itemIdVal,termIdVal));

				collExecInfo->deviceId = atoi(taskItemInfoReply->element[1]->str!=NULL?
												taskItemInfoReply->element[1]->str:"0");
			}

			freeReplyObject(taskItemInfoReply);
		}

		freeReplyObject(taskItemReply);
	}
	else
	{
		freeReplyObject(taskItemReply);

		return false;
	}

	return true;
}

bool CRtDbMs::saveCollectedData(CollectedData* collData)   //存储采集到的数据
{
	FastMutex::ScopedLock lock(*_impl->_mutex);
	if(_impl->_context != NULL && _impl->_context->err)
			return false;

	redisReply *collDItemReply = (redisReply*)redisCommand(_impl->_context,"smembers SET:WAITBACK:%d:%d:%X "
			,collData->deviceId,collData->termId,collData->itemId);

	if(collDItemReply != NULL)
	{
		for(unsigned int i=0; i<collDItemReply->elements; i++)
		{
			char tempItemInfo[2] = "0";
			char *collItemInfo = collDItemReply->element[i]->str != NULL?
					collDItemReply->element[i]->str:tempItemInfo;
			redisReply *collUnitIdReply = (redisReply*)redisCommand(_impl->_context,"hmget %s unitId",
					collItemInfo);
			if(collUnitIdReply!= NULL && collUnitIdReply->elements >=1)
			{
				char *collUnitId = collUnitIdReply->element[0]->str != NULL?
						collUnitIdReply->element[0]->str:tempItemInfo;
				redisReply *setStrDReply = (redisReply*)redisCommand(_impl->_context,"set STR:DATA:%s:%X:%d %f",
						collUnitId,collData->itemId,collData->time,collData->value);

				redisReply *rpLstTReply = (redisReply*)redisCommand(_impl->_context,"rpush LST:TIME:%s:%X %d",
						collUnitId,collData->itemId,collData->time);
				redisReply *setALTReply = (redisReply*)redisCommand(_impl->_context,"sadd SET:ALL:LSTTIME "
						"LST:TIME:%s:%X",collUnitId,collData->itemId);

				freeReplyObject(setStrDReply);
				freeReplyObject(rpLstTReply);
				freeReplyObject(setALTReply);
			}

			freeReplyObject(collUnitIdReply);
		}

		freeReplyObject(collDItemReply);
	}
	else
	{
		freeReplyObject(collDItemReply);

		return false;
	}

	return true;
}

bool CRtDbMs::saveZhaoceData(ZhaoceBackData* zhaoceData)       //存储召测到的数据
{
	FastMutex::ScopedLock lock(*_impl->_mutex);
	if(_impl->_context != NULL && _impl->_context->err)
			return false;

	redisReply*saveZhaoceReply =(redisReply*)redisCommand(_impl->_context,
			"set STR:MONITOR:%s:%d:%d:%X %d:%f",zhaoceData->unitId.data(),zhaoceData->deviceId,
			zhaoceData->termId,zhaoceData->itemId,zhaoceData->time,zhaoceData->value);

	if(saveZhaoceReply != NULL && saveZhaoceReply->type != REDIS_REPLY_ERROR)
	{
		freeReplyObject(saveZhaoceReply);

		return true;
	}
	else
	{
		freeReplyObject(saveZhaoceReply);

		return false;
	}
}

bool CRtDbMs::getAllSecPeriodOfArch(std::vector<UarcStorePeriodInfo> *secPeriodsVector)
{
	FastMutex::ScopedLock lock(*_impl->_mutex);
	if(_impl->_context != NULL && _impl->_context->err)
		return false;

	//redisReply *archKeysReply =(redisReply*)redisCommand(_impl->_context,"keys HS:ARCH:*");
	redisReply *archKeysReply =(redisReply*)redisCommand(_impl->_context,"smembers SET:ALL:ARCH");
	if(archKeysReply != NULL && REDIS_REPLY_NIL != archKeysReply->type &&
			REDIS_REPLY_ARRAY==archKeysReply->type)
	{
		unsigned int archElemIndex =0;
		UarcStorePeriodInfo tempPeriodInfo;
		bool tempPeriodInVector = false;
		int tempStoreType;

		while(archElemIndex < archKeysReply->elements)
		{
			tempPeriodInVector = false;
			StringTokenizer strKeysToken(archKeysReply->element[archElemIndex]->str,":");
			if(strKeysToken.count() >= 4)
			{
				std::string arch_unitId = strKeysToken[2].data();
				int arch_itemId;
				sscanf(strKeysToken[3].data(),"%X",&arch_itemId);

				redisReply *secCollPItemReply =(redisReply*)redisCommand(_impl->_context,
						"hmget HS:ARCH:%s:%X secCollPeriod storeType",arch_unitId.c_str(),arch_itemId);
				if(secCollPItemReply != NULL && secCollPItemReply->elements >= 2)
				{
					tempStoreType = atoi(secCollPItemReply->element[1]->str!=NULL?
							secCollPItemReply->element[1]->str:"0");

					if(ITEM_SOTRE_T_DOBLSAMP==tempStoreType)
					{
						tempPeriodInfo.secCollPeriod = atoi(secCollPItemReply->element[0]->str!=NULL?
								secCollPItemReply->element[0]->str:"0");

						for(std::vector<UarcStorePeriodInfo>::iterator it = secPeriodsVector->begin();
								it!=secPeriodsVector->end();it++)
						{
							if(it->secCollPeriod == tempPeriodInfo.secCollPeriod)
							{
								tempPeriodInVector = true;
								it->archCount++;
								break;
							}
						}
						if(false==tempPeriodInVector)
						{
							tempPeriodInfo.archCount = 1;
							secPeriodsVector->push_back(tempPeriodInfo);
						}
					}

					freeReplyObject(secCollPItemReply);
				}
				else
				{
					freeReplyObject(secCollPItemReply);
				}
			}
			archElemIndex++;
		}

		freeReplyObject(archKeysReply);
	}
	else
	{
		freeReplyObject(archKeysReply);
		return false;
	}

	return true;
}

bool CRtDbMs::getAllArchsSuitSecPeriod(std::vector<UarcStoreItemInfo> *storeItemInfos,int secCollPeriod)
{
	FastMutex::ScopedLock lock(*_impl->_mutex);
	if(_impl->_context != NULL && _impl->_context->err)
		return false;

	//redisReply *archsSuitSecPReply = (redisReply *)redisCommand(_impl->_context,
	//		"keys STR:SECPERIOD:*:%d",secCollPeriod);
	redisReply *archsSuitSecPReply = (redisReply *)redisCommand(_impl->_context,
			"smembers SET:ALLPERIOD:%d",secCollPeriod);

	if(archsSuitSecPReply != NULL && REDIS_REPLY_NIL != archsSuitSecPReply->type &&
			REDIS_REPLY_ARRAY==archsSuitSecPReply->type)
	{
		unsigned int suitSecArchIndex =0;
		UarcStoreItemInfo tempStoreItemInfo;

		while(suitSecArchIndex < archsSuitSecPReply->elements)
		{
			StringTokenizer strKeysToken(archsSuitSecPReply->element[suitSecArchIndex]->str,":");
			if(strKeysToken.count() >= 2)
			{
				std::string arch_unitId = strKeysToken[0].data();
				int arch_itemId;
				sscanf(strKeysToken[1].data(),"%X",&arch_itemId);

				redisReply *storeItemReply =(redisReply*)redisCommand(_impl->_context,
						"hmget HS:ARCH:%s:%X itemType baseVal coefficient secCollSql "
						"secCollOtherSql storeType deviceId termId",arch_unitId.data(),arch_itemId);
				if(storeItemReply != NULL && storeItemReply->elements >= 6)
				{
					tempStoreItemInfo.unitId = arch_unitId;
					tempStoreItemInfo.itemId = arch_itemId;
					tempStoreItemInfo.itemType = atoi(storeItemReply->element[0]->str!=NULL?
							storeItemReply->element[0]->str:"0");
					Poco::NumberParser::tryParseFloat(storeItemReply->element[1]->str != NULL?
							storeItemReply->element[1]->str:"0",tempStoreItemInfo.baseVal);
					Poco::NumberParser::tryParseFloat(storeItemReply->element[2]->str != NULL?
							storeItemReply->element[2]->str:"0",tempStoreItemInfo.coefficient);
					tempStoreItemInfo.secCollSql = std::string(storeItemReply->element[3]->str!=NULL?
							storeItemReply->element[3]->str:"0");
					tempStoreItemInfo.secCollOtherSql = std::string(storeItemReply->element[4]->str!=NULL?
							storeItemReply->element[4]->str:"0");
					tempStoreItemInfo.storeType = atoi(storeItemReply->element[5]->str!=NULL?
							storeItemReply->element[5]->str:"0");
					tempStoreItemInfo.deviceId = atoi(storeItemReply->element[6]->str!=NULL?
							storeItemReply->element[6]->str:"0");
					tempStoreItemInfo.termId = atoi(storeItemReply->element[7]->str!=NULL?
												storeItemReply->element[7]->str:"0");

					storeItemInfos->push_back(tempStoreItemInfo);
					freeReplyObject(storeItemReply);
				}
				else
				{
					freeReplyObject(storeItemReply);
				}
			}
			suitSecArchIndex++;
		}

		freeReplyObject(archsSuitSecPReply);
	}
	else
	{
		freeReplyObject(archsSuitSecPReply);
		return false;
	}

	return true;
}

bool CRtDbMs::getStoreValue(std::string unitId, int itemId, map<int,double> &storeValue) //获得指定项的所有时刻�?
{
	FastMutex::ScopedLock lock(*_impl->_mutex);
	if(_impl->_context != NULL && _impl->_context->err)
			return false;

	redisReply *getSTimeReply = (redisReply *)redisCommand(_impl->_context,
			"rpop LST:TIME:%s:%X",unitId.data(),itemId);
	int tempTime =0;
	double tempValue =0;
	while(getSTimeReply != NULL && getSTimeReply->type != REDIS_REPLY_ERROR
			&& getSTimeReply->type != REDIS_REPLY_NIL)
	{
		tempTime = atoi(getSTimeReply->str);
		freeReplyObject(getSTimeReply);

		redisReply *getSValueReply = (redisReply*)redisCommand(_impl->_context,
				"get STR:DATA:%s:%X:%d",unitId.data(),itemId,tempTime);
		if(getSValueReply != NULL && getSValueReply->type != REDIS_REPLY_ERROR
				&&getSValueReply->type != REDIS_REPLY_NIL)
		{
			Poco::NumberParser::tryParseFloat(getSValueReply->str,tempValue);
			storeValue.insert(pair<int,double>(tempTime,tempValue));
		}
		freeReplyObject(getSValueReply);

		redisReply *delSValueReply = (redisReply*)redisCommand(_impl->_context,
				"del STR:DATA:%s:%X:%d",unitId.data(),itemId,tempTime);
		freeReplyObject(delSValueReply);

		getSTimeReply = (redisReply *)redisCommand(_impl->_context,"rpop LST:TIME:%s:%X",
				unitId.data(),itemId);
	}

	freeReplyObject(getSTimeReply);

	return true;
}

bool CRtDbMs::getLastStoreValue(std::string unitId, int itemId, int &lastStoreTime, double &lastStoreValue)
{
	FastMutex::ScopedLock lock(*_impl->_mutex);
	if(_impl->_context != NULL && _impl->_context->err)
		return false;

	redisReply *timeListLenReply = (redisReply*)redisCommand(_impl->_context,
			"llen LST:TIME:%s:%X",unitId.data(),itemId);
	if(timeListLenReply != NULL && timeListLenReply->type == REDIS_REPLY_INTEGER)
	{
		redisReply *lstTimeReply = (redisReply*)redisCommand(_impl->_context,
				"lindex LST:TIME:%s:%X %d",unitId.data(),itemId,timeListLenReply->integer-1);
		if(lstTimeReply !=NULL && lstTimeReply->type==REDIS_REPLY_STRING)
		{
			lastStoreTime = atoi(lstTimeReply->str!=NULL? lstTimeReply->str:"0");

			redisReply *listValReply = (redisReply*)redisCommand(_impl->_context,
					"get STR:DATA:%s:%X:%s",unitId.data(),itemId,lstTimeReply->str);

			if(listValReply !=NULL && listValReply->type == REDIS_REPLY_STRING)
			{
				Poco::NumberParser::tryParseFloat(listValReply->str,lastStoreValue);

				freeReplyObject(listValReply);
			}
			else
			{
				freeReplyObject(listValReply);
				freeReplyObject(lstTimeReply);
				freeReplyObject(timeListLenReply);

				return false;
			}
			freeReplyObject(lstTimeReply);
		}
		else
		{
			freeReplyObject(lstTimeReply);
			freeReplyObject(timeListLenReply);

			return false;
		}

		freeReplyObject(timeListLenReply);
	}
	else
	{
		freeReplyObject(timeListLenReply);

		return false;
	}

	return true;
}

bool CRtDbMs::saveProcessedData(UarcProcData procData)
{
	FastMutex::ScopedLock lock(*_impl->_mutex);
	if(_impl->_context != NULL && _impl->_context->err)
			return false;

	redisReply* saveDataReply = (redisReply*)redisCommand(_impl->_context,"set STR:DATA:%s:%X:%d %f",
			procData.unitId.data(),procData.itemId,procData.timeVal,procData.dataVal);
	if(saveDataReply !=NULL && saveDataReply->type != REDIS_REPLY_ERROR)
	{
		freeReplyObject(saveDataReply);

		return true;
	}
	else
	{
		freeReplyObject(saveDataReply);

		return false;
	}
}

bool CRtDbMs::getAllCalcTask(std::vector< UarcCalTaskArchive > *allCollTask) //获得所有的计算任务
{
	FastMutex::ScopedLock lock(*_impl->_mutex);
	if(_impl->_context != NULL && _impl->_context->err)
			return false;

	allCollTask->clear();

	UarcCalTaskArchive tempCalTask;
	redisReply* calcTaskReply;
	redisReply* calcIdReply;
	redisReply *paraReply;

	redisReply* unitIdReply = (redisReply*)redisCommand(_impl->_context,"smembers SET:UNIT");

	if(unitIdReply != NULL && unitIdReply->type != REDIS_REPLY_NIL &&
			unitIdReply->type != REDIS_REPLY_ERROR)
	{
		for(unsigned int i=0; i < unitIdReply->elements; i++)
		{
			calcIdReply = (redisReply*)redisCommand(_impl->_context,
					"smembers SET:CALC:%s",unitIdReply->element[i]->str);
			if(calcIdReply != NULL && calcIdReply->type != REDIS_REPLY_NIL &&
					calcIdReply->type != REDIS_REPLY_ERROR)
			{
				for(unsigned int j=0; j < calcIdReply->elements; j++)
				{
					tempCalTask.calcId = atoi(calcIdReply->element[j]->str != NULL?
											 calcIdReply->element[j]->str:"0");
					calcTaskReply = (redisReply*)redisCommand(_impl->_context,
							"hmget HS:CALC:%s:%s unitId itemId expr paramMask "
							" priority freq status lastUpdate",unitIdReply->element[i]->str,
							calcIdReply->element[j]->str);
					if(calcTaskReply != NULL && calcTaskReply->type != REDIS_REPLY_NIL &&
							calcTaskReply->elements >= 8)
					{
						tempCalTask.unitId = std::string(calcTaskReply->element[0]->str!=NULL?
								calcTaskReply->element[0]->str:"0");
						sscanf(calcTaskReply->element[1]->str!=NULL?calcTaskReply->element[1]->str:"0",
								"%X",&tempCalTask.itemId);
						tempCalTask.expr = string(calcTaskReply->element[2]->str!=NULL?
								calcTaskReply->element[2]->str:" ");
						tempCalTask.paramMask = atoi(calcTaskReply->element[3]->str!=NULL?
								calcTaskReply->element[3]->str:"0");
						tempCalTask.priority = atoi(calcTaskReply->element[4]->str!=NULL?
								calcTaskReply->element[4]->str:"0");
						tempCalTask.freq = atoi(calcTaskReply->element[5]->str!=NULL?
								calcTaskReply->element[5]->str:"0");
						tempCalTask.status = atoi(calcTaskReply->element[6]->str !=NULL?
								calcTaskReply->element[6]->str:"0");
						tempCalTask.lastUpdate = atoi(calcTaskReply->element[7]->str!=NULL?
								calcTaskReply->element[7]->str:"0");

						for(int k=0; k<FORMULA_PARAM_NUM; k++)
						{
							if(tempCalTask.paramMask & (1<<k))
							{
								paraReply=(redisReply*)redisCommand(_impl->_context,
										"hmget HS:CALC:%s:%s U%02d I%02d",unitIdReply->element[i]->str,
										calcIdReply->element[j]->str,k,k);
								if(paraReply!=NULL && paraReply->type != REDIS_REPLY_NIL &&
										paraReply->elements >=2)
								{
									tempCalTask.params[k].unitId = std::string(paraReply->element[0]->str!=NULL?
											paraReply->element[0]->str:"0");
									sscanf(paraReply->element[1]->str!=NULL?paraReply->element[1]->str:"0",
											"%X",&tempCalTask.params[k].itemId);
								}
								freeReplyObject(paraReply);
							}
							else
							{
								tempCalTask.params[k].unitId = std::string("-1");
								tempCalTask.params[k].itemId = -1;
							}
						}
						vector<UarcCalTaskArchive>::iterator calTaskVecIndex;
						for(calTaskVecIndex = allCollTask->begin();calTaskVecIndex!=allCollTask->end()
						&&calTaskVecIndex->priority < tempCalTask.priority;calTaskVecIndex++)
							;
						allCollTask->insert(calTaskVecIndex,tempCalTask);
					}

					freeReplyObject(calcTaskReply);
				}
			}
			freeReplyObject(calcIdReply);
		}

		freeReplyObject(unitIdReply);
	}
	else
	{
		freeReplyObject(unitIdReply);

		return false;
	}

	return true;
}

bool CRtDbMs::getCalcParaValue(std::string unitId,int itemId, double& paraValue,UarcCalcParaItem *uarcCalParaItem) //获得指定数据时间键的参数�?
{
	FastMutex::ScopedLock lock(*_impl->_mutex);
	if(_impl->_context != NULL && _impl->_context->err)
		return false;

	redisReply *timeListLenReply = (redisReply*)redisCommand(_impl->_context,
			"llen LST:TIME:%s:%X",unitId.data(),itemId);
	if(timeListLenReply != NULL && timeListLenReply->type == REDIS_REPLY_INTEGER)
	{
		redisReply *lstTimeReply = (redisReply*)redisCommand(_impl->_context,
				"lindex LST:TIME:%s:%X %d",unitId.data(),itemId,timeListLenReply->integer-1);
		if(lstTimeReply !=NULL && lstTimeReply->type==REDIS_REPLY_STRING)
		{
			redisReply *listValReply = (redisReply*)redisCommand(_impl->_context,
					"get STR:DATA:%s:%X:%s",unitId.data(),itemId,lstTimeReply->str);

			if(listValReply !=NULL && listValReply->type == REDIS_REPLY_STRING)
			{
				Poco::NumberParser::tryParseFloat(listValReply->str,paraValue);

				freeReplyObject(listValReply);
			}
			else
			{
				freeReplyObject(listValReply);
				freeReplyObject(lstTimeReply);
				freeReplyObject(timeListLenReply);

				return false;
			}
			freeReplyObject(lstTimeReply);
		}
		else
		{
			freeReplyObject(lstTimeReply);
			freeReplyObject(timeListLenReply);

			return false;
		}

		freeReplyObject(timeListLenReply);
	}
	else
	{
		freeReplyObject(timeListLenReply);

		return false;
	}

	redisReply *calcParaItemReply = (redisReply*)redisCommand(_impl->_context,
			"hmget HS:ARCH:%s:%X itemType baseval coefficient",unitId.data(),itemId);
	if(calcParaItemReply != NULL && calcParaItemReply->elements >= 3)
	{
		uarcCalParaItem->itemType = atoi(calcParaItemReply->element[0]->str != NULL?
				calcParaItemReply->element[0]->str:"0");
		Poco::NumberParser::tryParseFloat(calcParaItemReply->element[1]->str != NULL?
				calcParaItemReply->element[1]->str:"0",uarcCalParaItem->baseVal);
		Poco::NumberParser::tryParseFloat(calcParaItemReply->element[2]->str != NULL?
				calcParaItemReply->element[2]->str:"0",uarcCalParaItem->coefficient);

		freeReplyObject(calcParaItemReply);
	}
	else
	{
		freeReplyObject(calcParaItemReply);
		return false;
	}

	uarcCalParaItem->unitId =unitId;
	uarcCalParaItem->itemId = itemId;

	return true;
}

bool CRtDbMs::saveCalcResult(UarcCalcResult *calcResult)    //向内存库中存放计算结�?
{
	FastMutex::ScopedLock lock(*_impl->_mutex);
	if(_impl->_context != NULL && _impl->_context->err)
			return false;

	redisReply* saveValueReply = (redisReply*)redisCommand(_impl->_context,
							"set STR:DATA:%s:%X:%d %f",calcResult->unitId.data(),
			calcResult->itemId,calcResult->timeVal,calcResult->resultVal);
	//redisCommand(_impl->_context,"expire STR:DATA:%s:%X:%d %d",calcResult->unitId.data(),
	//		calcResult->itemId,calcResult->timeVal,86400);  //一天后自动删除该键

	redisReply* saveTimeReply = (redisReply*)redisCommand(_impl->_context,
			"rpush LST:TIME:%s:%X %d",calcResult->unitId.data(),calcResult->itemId,
			calcResult->timeVal);
	redisReply *setALTReply = (redisReply*)redisCommand(_impl->_context,"sadd SET:ALL:LSTTIME "
							"LST:TIME:%s:%X",calcResult->unitId.data(),calcResult->itemId);
	if(NULL!=saveValueReply && saveValueReply->type != REDIS_REPLY_ERROR &&	NULL != saveTimeReply && saveTimeReply->type != REDIS_REPLY_ERROR &&
	NULL!=setALTReply && setALTReply->type != REDIS_REPLY_ERROR)
	{
		freeReplyObject(saveValueReply);
		freeReplyObject(saveTimeReply);
		freeReplyObject(setALTReply);
		return true;
	}
	else
	{
		freeReplyObject(saveValueReply);
		freeReplyObject(saveTimeReply);
		freeReplyObject(setALTReply);
		return false;
	}
}

bool CRtDbMs::saveCalcTaskPara(UarcCalcTaskModified calcTaskModified)
{
	FastMutex::ScopedLock lock(*_impl->_mutex);
	if(_impl->_context != NULL && _impl->_context->err)
							return false;

	//redisReply *checkCalcTaskReply = (redisReply*)redisCommand(_impl->_context,"keys HS:CALC:*");
	redisReply *checkCalcTaskReply = (redisReply*)redisCommand(_impl->_context,"smembers SET:ALL:HSCALC");

	if(checkCalcTaskReply != NULL && REDIS_REPLY_NIL != checkCalcTaskReply->type &&
			REDIS_REPLY_ARRAY==checkCalcTaskReply->type)
	{
		unsigned int calcElemIndex =0;

		while(calcElemIndex < checkCalcTaskReply->elements)
		{
			StringTokenizer strCalcToken(checkCalcTaskReply->element[calcElemIndex]->str,":");
			if(strCalcToken.count() >= 4)
			{
				int existCalcId = atoi(strCalcToken[3].data()!=NULL?strCalcToken[3].data():"0");

				if(0==strcmp(strCalcToken[2].data(),calcTaskModified.unitId.data()) && existCalcId==calcTaskModified.calcId)
				{
					redisReply* saveCalcTReply = (redisReply*)redisCommand(_impl->_context,
							"hmset HS:CALC:%s:%d status %d lastUpdate %d",calcTaskModified.unitId.data(),
							calcTaskModified.calcId,calcTaskModified.status,calcTaskModified.lastUpdate);
					if(NULL!=saveCalcTReply && saveCalcTReply->type != REDIS_REPLY_ERROR)
					{
						freeReplyObject(saveCalcTReply);
						freeReplyObject(checkCalcTaskReply);
						return true;
					}
					else
					{
						freeReplyObject(saveCalcTReply);
						freeReplyObject(checkCalcTaskReply);
						return false;
					}
				}
			}
			calcElemIndex++;
		}

		freeReplyObject(checkCalcTaskReply);
		return false;
	}
	else
	{
		freeReplyObject(checkCalcTaskReply);
		return false;
	}

}

bool CRtDbMs::clearHisData()
{
	FastMutex::ScopedLock lock(*_impl->_mutex);
	if(_impl->_context != NULL && _impl->_context->err)
		return false;

	//redisReply *allListReply = (redisReply*)redisCommand(_impl->_context,"keys LST:TIME:*");
	redisReply *allListReply = (redisReply*)redisCommand(_impl->_context,"smembers SET:ALL:LSTTIME");
	if(allListReply != NULL && REDIS_REPLY_NIL != allListReply->type &&
			REDIS_REPLY_ARRAY==allListReply->type)
	{
		std::string unitIdStr;
		std::string itemIdStr;
		unsigned int lstElemIndex =0;

		while(lstElemIndex < allListReply->elements)
		{
			StringTokenizer strLstToken(allListReply->element[lstElemIndex]->str,":");
			if(strLstToken.count() >= 4)
			{
				unitIdStr = strLstToken[2];
				itemIdStr = strLstToken[3];
				redisReply *lstLenReply = (redisReply*)redisCommand(_impl->_context,"llen LST:TIME:%s:%s",
						unitIdStr.c_str(),itemIdStr.c_str());
				if(lstLenReply != NULL && lstLenReply->type == REDIS_REPLY_INTEGER)
				{
					int lstOptIndex = lstLenReply->integer-7;
					while(lstOptIndex >= 0)
					{
						redisReply *lstValReply = (redisReply*)redisCommand(_impl->_context,"lindex LST:TIME:%s:%s %d",
								unitIdStr.c_str(),itemIdStr.c_str(),lstOptIndex);
						if(lstValReply != NULL && lstValReply->type==REDIS_REPLY_STRING)
						{
							redisReply *rmLstReply = (redisReply*)redisCommand(_impl->_context,
									"lrem LST:TIME:%s:%s 0 %s",unitIdStr.c_str(),itemIdStr.c_str(),
									lstValReply->str);
							redisReply *delStrDataReply = (redisReply*)redisCommand(_impl->_context,
									"del STR:DATA:%s:%s:%s",unitIdStr.c_str(),itemIdStr.c_str(),
									lstValReply->str);

							freeReplyObject(rmLstReply);
							freeReplyObject(delStrDataReply);

							freeReplyObject(lstValReply);
						}
						else
						{
							freeReplyObject(lstValReply);
						}

						lstOptIndex--;
					}
				}
				freeReplyObject(lstLenReply);

			}

			lstElemIndex++;
		}

		freeReplyObject(allListReply);
	}
	else
	{
		freeReplyObject(allListReply);
	}

	return true;
}

bool CRtDbMs::getDeviceInfo(vector<stDeviceInfo> &deviceInfos)
{
	FastMutex::ScopedLock lock(*_impl->_mutex);
	if(_impl->_context != NULL && _impl->_context->err)
			return false;

	deviceInfos.clear();
	stDeviceInfo tempDevInfo;
	//redisReply *getDevInfoKeyReply = (redisReply*)redisCommand(_impl->_context,"keys HS:DEVICE:*");
	redisReply *getDevInfoKeyReply = (redisReply*)redisCommand(_impl->_context,"smembers SET:ALL:DEVICE");
	if(getDevInfoKeyReply != NULL && getDevInfoKeyReply->type != REDIS_REPLY_ERROR)
	{
		redisReply *getDevInfoReply;
		for(unsigned int i=0; i<getDevInfoKeyReply->elements; i++)
		{
			StringTokenizer strDevInfoKeyToken(getDevInfoKeyReply->element[i]->str, ":");
			if(strDevInfoKeyToken.count() >= 3)
			{
				tempDevInfo.id = atoi(strDevInfoKeyToken[2].data()!=NULL? strDevInfoKeyToken[2].data():"0");
			}
			else
			{
				continue;
			}

			getDevInfoReply = (redisReply*)redisCommand(_impl->_context,
					"hmget %s status address port "
					"linkType protocol pulsePeriod lastConnTime identity",
					getDevInfoKeyReply->element[i]->str);

			if(getDevInfoReply != NULL && getDevInfoReply->type != REDIS_REPLY_ERROR &&
					getDevInfoReply->elements >= 8)
			{
				tempDevInfo.status = atoi(getDevInfoReply->element[0]->str!=NULL?
						getDevInfoReply->element[0]->str:"0");
				sprintf(tempDevInfo.ip,getDevInfoReply->element[1]->str!=NULL?
						getDevInfoReply->element[1]->str:"0");
				tempDevInfo.port = atoi(getDevInfoReply->element[2]->str!=NULL?
						getDevInfoReply->element[2]->str:"10000");
				tempDevInfo.link_type = atoi(getDevInfoReply->element[3]->str!=NULL?
						getDevInfoReply->element[3]->str:"0");
				tempDevInfo.protocol_type = atoi(getDevInfoReply->element[4]->str!=NULL?
						getDevInfoReply->element[4]->str:"0");
				tempDevInfo.heart_period = atoi(getDevInfoReply->element[5]->str!=NULL?
						getDevInfoReply->element[5]->str:"0");
				tempDevInfo.last_connect_time = atoi(getDevInfoReply->element[6]->str!=NULL?
						getDevInfoReply->element[6]->str:"0");
				tempDevInfo.identity = std::string(getDevInfoReply->element[7]->str!=NULL?
						getDevInfoReply->element[7]->str:"");
			}

			deviceInfos.push_back(tempDevInfo);

			freeReplyObject(getDevInfoReply);
		}
		freeReplyObject(getDevInfoKeyReply);
	}
	else
	{
		freeReplyObject(getDevInfoKeyReply);
		return false;
	}

	return true;
}

bool CRtDbMs::updateDeviceInfo(const stDeviceInfo *deviceInfo)
{
	FastMutex::ScopedLock lock(*_impl->_mutex);
	if(_impl->_context != NULL && _impl->_context->err)
			return false;

	//redisReply *checkDevInfoReply = (redisReply*)redisCommand(_impl->_context,"keys HS:DEVICE:*");
	redisReply *checkDevInfoReply = (redisReply*)redisCommand(_impl->_context,"smembers SET:ALL:DEVICE");

	if(checkDevInfoReply != NULL && REDIS_REPLY_NIL != checkDevInfoReply->type &&
			REDIS_REPLY_ARRAY==checkDevInfoReply->type)
	{
		unsigned int devElemIndex =0;

		while(devElemIndex < checkDevInfoReply->elements)
		{
			StringTokenizer strDevInfoToken(checkDevInfoReply->element[devElemIndex]->str,":");
			if(strDevInfoToken.count() >= 3)
			{
				int existDevId = atoi(strDevInfoToken[2].data()!=NULL?strDevInfoToken[2].data():"0");

				if(existDevId==deviceInfo->id)
				{
					redisReply* updateDevReply = (redisReply*)redisCommand(_impl->_context,"hmset HS:DEVICE:%d "
							"status %d lastConnTime %d",deviceInfo->id,
							deviceInfo->status,deviceInfo->last_connect_time);

					if(updateDevReply != NULL && updateDevReply->type != REDIS_REPLY_ERROR)
					{
						freeReplyObject(updateDevReply);
						return true;
					}
					else
					{
						freeReplyObject(updateDevReply);
						return false;
					}
				}
			}
			devElemIndex++;
		}
		freeReplyObject(checkDevInfoReply);
		return false;
	}
	else
	{
		freeReplyObject(checkDevInfoReply);
		return false;
	}

}

bool CRtDbMs::saveSrcDataConfig(SrcDataConfig &sdc)
{
	FastMutex::ScopedLock lock(*_impl->_mutex);
//	printf("hmset HS:%s:%X "
//				"type %d floatmin %f floatmax %f expr %s incbase %f "
//				"hour0 %f hour1 %f hour2 %f hour3 %f hour4 %f hour5 %f "
//				"hour6 %f hour7 %f hour8 %f hour9 %f hour10 %f hour11 %f "
//				"hour12 %f hour13 %f hour14 %f hour15 %f hour16 %f hour17 %f "
//				"hour18 %f hour19 %f hour20 %f hour21 %f hour22 %f hour23 %f \n", sdc.unitId.data(), sdc.itemId,
//				sdc.type, sdc.floatMin, sdc.floatMax, sdc.expr.data(), sdc.incbase,
//				sdc.h[0], sdc.h[1], sdc.h[2], sdc.h[3], sdc.h[4], sdc.h[5],
//				sdc.h[6], sdc.h[7], sdc.h[8], sdc.h[9], sdc.h[10], sdc.h[11],
//				sdc.h[12], sdc.h[13], sdc.h[14], sdc.h[15], sdc.h[16], sdc.h[17],
//				sdc.h[18], sdc.h[19], sdc.h[20], sdc.h[21], sdc.h[22], sdc.h[23]);
	redisReply* reply = (redisReply*)redisCommand(_impl->_context, "hmset HS:%s:%X "
			"type %d floatmin %f floatmax %f expr %s incbase %f "
			"hour0 %f hour1 %f hour2 %f hour3 %f hour4 %f hour5 %f "
			"hour6 %f hour7 %f hour8 %f hour9 %f hour10 %f hour11 %f "
			"hour12 %f hour13 %f hour14 %f hour15 %f hour16 %f hour17 %f "
			"hour18 %f hour19 %f hour20 %f hour21 %f hour22 %f hour23 %f ", sdc.unitId.data(), sdc.itemId,
			sdc.type, sdc.floatMin, sdc.floatMax, sdc.expr.data(), sdc.incbase,
			sdc.h[0], sdc.h[1], sdc.h[2], sdc.h[3], sdc.h[4], sdc.h[5],
			sdc.h[6], sdc.h[7], sdc.h[8], sdc.h[9], sdc.h[10], sdc.h[11],
			sdc.h[12], sdc.h[13], sdc.h[14], sdc.h[15], sdc.h[16], sdc.h[17],
			sdc.h[18], sdc.h[19], sdc.h[20], sdc.h[21], sdc.h[22], sdc.h[23]);
	if (reply == NULL || reply->type == REDIS_REPLY_ERROR)
	{
		freeReplyObject(reply);
		return false;
	}

	freeReplyObject(reply);

	reply = (redisReply*)redisCommand(_impl->_context, "SADD SET:SOURCE:DATA:CONFIG HS:%s:%X ", sdc.unitId.data(), sdc.itemId);
	if (reply == NULL || reply->type == REDIS_REPLY_ERROR)
	{
		freeReplyObject(reply);
		return false;
	}

	freeReplyObject(reply);
	return true;
}

bool CRtDbMs::getSDC(vector<SrcDataConfig> &sdcs, string unitId, int itemId, int type)
{
	FastMutex::ScopedLock lock(*_impl->_mutex);
	bool ret = true;
	SrcDataConfig sdc;
	if (unitId != string("") && itemId != -1)//对给定unitid和itemid的优�?
	{
		redisReply* reply = (redisReply*)redisCommand(_impl->_context, "SISMEMBER SET:SOURCE:DATA:CONFIG HS:%s:%X ", unitId.data(), itemId);
		if (reply == NULL || reply->type == REDIS_REPLY_ERROR)
		{
			freeReplyObject(reply);
			return false;
		}
		if (reply->integer == 1)
		{
			redisReply *reply1 = (redisReply*)redisCommand(_impl->_context, "HMGET HS:%s:%X "
					"type floatmin floatmax expr incbase "
					"hour0 hour1 hour2 hour3 hour4 hour5 "
					"hour6 hour7 hour8 hour9 hour10 hour11 "
					"hour12 hour13 hour14 hour15 hour16 hour17 "
					"hour18 hour19 hour20 hour21 hour22 hour23 ",
					sdc.unitId.data(), sdc.itemId);
			if (reply1 == NULL || reply1->elements != 29)
			{
				freeReplyObject(reply1);
				ret = false;
			}else {
				int k = 0, len = 0;

				len = reply1->element[k]->len;
				Poco::NumberParser::tryParse(string(reply1->element[k++]->str, 0, len), sdc.type);

				len = reply1->element[k]->len;
				Poco::NumberParser::tryParseFloat(string(reply1->element[k++]->str, 0, len), sdc.floatMin);

				len = reply1->element[k]->len;
				Poco::NumberParser::tryParseFloat(string(reply1->element[k++]->str, 0, len), sdc.floatMax);

				len = reply1->element[k]->len;
				sdc.expr = string(reply1->element[k++]->str, 0, len);

				len = reply1->element[k]->len;
				Poco::NumberParser::tryParseFloat(string(reply1->element[k++]->str, 0, len), sdc.incbase);

				for (int j = 0; j < 24; j++)
				{
					len = reply1->element[k]->len;
					Poco::NumberParser::tryParseFloat(string(reply1->element[k++]->str, 0, len), sdc.h[j]);
				}

				if (type == -1 || type == sdc.type)
				{
					sdcs.push_back(sdc);
				}
				freeReplyObject(reply1);
			}
		}
		freeReplyObject(reply);

		return ret;
	}

	///////
	redisReply* reply = (redisReply*)redisCommand(_impl->_context, "SMEMBERS SET:SOURCE:DATA:CONFIG");
	if (reply == NULL || reply->type != REDIS_REPLY_ARRAY)
	{
		freeReplyObject(reply);
		return false;
	}

	for (int i = 0; i < (int)reply->elements; i++)
	{
		StringTokenizer token(reply->element[i]->str,":");
		if (token.count() == 3)
		{
			sdc.unitId = token[1];
			sscanf(token[2].data(), "%X", &sdc.itemId);

			redisReply *reply1 = (redisReply*)redisCommand(_impl->_context, "HMGET HS:%s:%X "
					"type floatmin floatmax expr incbase "
					"hour0 hour1 hour2 hour3 hour4 hour5 "
					"hour6 hour7 hour8 hour9 hour10 hour11 "
					"hour12 hour13 hour14 hour15 hour16 hour17 "
					"hour18 hour19 hour20 hour21 hour22 hour23 ",
					sdc.unitId.data(), sdc.itemId);
			if (reply1 == NULL || reply1->elements != 29)
			{
				freeReplyObject(reply1);
				ret = false;
				break;
			}

			int k = 0, len = 0;

			len = reply1->element[k]->len;
			Poco::NumberParser::tryParse(string(reply1->element[k++]->str, 0, len), sdc.type);

			len = reply1->element[k]->len;
			Poco::NumberParser::tryParseFloat(string(reply1->element[k++]->str, 0, len), sdc.floatMin);

			len = reply1->element[k]->len;
			Poco::NumberParser::tryParseFloat(string(reply1->element[k++]->str, 0, len), sdc.floatMax);

			len = reply1->element[k]->len;
			sdc.expr = string(reply1->element[k++]->str, 0, len);

			len = reply1->element[k]->len;
			Poco::NumberParser::tryParseFloat(string(reply1->element[k++]->str, 0, len), sdc.incbase);

			for (int j = 0; j < 24; j++)
			{
				len = reply1->element[k]->len;
				Poco::NumberParser::tryParseFloat(string(reply1->element[k++]->str, 0, len), sdc.h[j]);
			}

			//检查筛选条�?
			if (unitId != string("") && unitId != sdc.unitId)
			{
				continue;
			}
			if (itemId != -1 && itemId != sdc.itemId)
			{
				continue;
			}
			if (type != -1 && type != sdc.type)
			{
				continue;
			}

			sdcs.push_back(sdc);
			freeReplyObject(reply1);
		}
	}
	freeReplyObject(reply);

	return ret;
}

bool CRtDbMs::getSrcDataTimes(std::string unitId, int itemId, std::vector<time_t> &SDTs)
{
	FastMutex::ScopedLock lock(*_impl->_mutex);
	redisReply* reply = (redisReply*)redisCommand(_impl->_context, "ZRANGE ZSET:SOURCE:DATA:TIME:%s:%X 0 -1",
				unitId.data(), itemId);

	if (reply == NULL || reply->type == REDIS_REPLY_ERROR || reply->type != REDIS_REPLY_ARRAY)
	{
		freeReplyObject(reply);
		return false;
	}

	for (int i = 0; i < (int)reply->elements; i++)
	{
		unsigned long t;
		int len = reply->element[i]->len;

		Poco::NumberParser::tryParseUnsigned64(string(reply->element[i]->str, 0, len), t);
		SDTs.push_back((time_t)t);
	}

	freeReplyObject(reply);
	return true;
}

bool CRtDbMs::readSrcData(std::string unitId, int itemId, time_t t, double &value)
{
	FastMutex::ScopedLock lock(*_impl->_mutex);
	redisReply* reply = (redisReply*)redisCommand(_impl->_context, "GET STR:SOURCE:DATA:%s:%X:%ld",
			unitId.data(), itemId, t);
	if (reply == NULL || reply->type == REDIS_REPLY_ERROR)
	{
		freeReplyObject(reply);
		return false;
	}

	if (reply->type == REDIS_REPLY_STRING)
	{
		double val = 0;
		if (!Poco::NumberParser::tryParseFloat(string(reply->str, 0, reply->len), val))
		{
			freeReplyObject(reply);
			return false;
		}
		value = val;

		freeReplyObject(reply);
		return true;
	}

	freeReplyObject(reply);
	return false;
}

bool CRtDbMs::writeSrcData(std::string unitId, int itemId, time_t t, double &value)
{
	FastMutex::ScopedLock lock(*_impl->_mutex);
	redisReply* reply = (redisReply*)redisCommand(_impl->_context, "SET STR:SOURCE:DATA:%s:%X:%d %f",
			unitId.data(), itemId, t, value);
	if (reply == NULL || reply->type == REDIS_REPLY_ERROR)
	{
		freeReplyObject(reply);
		return false;
	}

	freeReplyObject(reply);

	reply = (redisReply*)redisCommand(_impl->_context,"ZADD ZSET:SOURCE:DATA:TIME:%s:%X %d %d",
			unitId.data(), itemId, t, t);
	if (reply == NULL || reply->type == REDIS_REPLY_ERROR)
	{
		freeReplyObject(reply);
		return false;
	}

	freeReplyObject(reply);

	return true;
}

bool CRtDbMs::saveDBConfig(string key, string &value)
{
	FastMutex::ScopedLock lock(*_impl->_mutex);
	redisReply* reply = (redisReply*)redisCommand(_impl->_context, "hmset HS:DB:CONFIG "
			"%s %s ", key.data(), value.data());
	if (reply == NULL || reply->type == REDIS_REPLY_ERROR)
	{
		freeReplyObject(reply);
		return false;
	}

	freeReplyObject(reply);
	return true;
}

bool CRtDbMs::getDBConfig(string key, string &value)
{
	FastMutex::ScopedLock lock(*_impl->_mutex);
	redisReply* reply = (redisReply*)redisCommand(_impl->_context, "hmget HS:DB:CONFIG "
			"%s ", key.data());

	if (reply == NULL || reply->type != REDIS_REPLY_ARRAY || reply->elements != 1)
	{
		freeReplyObject(reply);
		return false;
	}
	if (reply->element[0]->type != REDIS_REPLY_STRING)
	{
		freeReplyObject(reply);
		return false;
	}

	int len = reply->element[0]->len;
	value = string(reply->element[0]->str, 0, len);

	freeReplyObject(reply);
	return true;
}

bool CRtDbMs::saveConfig(string name, string key, string &value)
{
	FastMutex::ScopedLock lock(*_impl->_mutex);
	redisReply* reply = (redisReply*)redisCommand(_impl->_context, "hmset %s "
			"%s %s ", name.data(), key.data(), value.data());
	if (reply == NULL || reply->type == REDIS_REPLY_ERROR)
	{
		freeReplyObject(reply);
		return false;
	}

	freeReplyObject(reply);
	return true;
}

bool CRtDbMs::getConfig(string name, string key, string &value)
{
	FastMutex::ScopedLock lock(*_impl->_mutex);
	redisReply* reply = (redisReply*)redisCommand(_impl->_context, "hmget %s "
			"%s ", name.data(), key.data());

	if (reply == NULL || reply->type != REDIS_REPLY_ARRAY || reply->elements != 1)
	{
		freeReplyObject(reply);
		return false;
	}
	if (reply->element[0]->type != REDIS_REPLY_STRING)
	{
		freeReplyObject(reply);
		return false;
	}

	int len = reply->element[0]->len;
	value = string(reply->element[0]->str, 0, len);

	freeReplyObject(reply);
	return true;
}

bool CRtDbMs::removeSrcData(std::string unitId, int itemId, time_t formTime, time_t toTime)
{
	FastMutex::ScopedLock lock(*_impl->_mutex);

	redisReply* reply = (redisReply*)redisCommand(_impl->_context, "zrangebyscore ZSET:SOURCE:DATA:TIME:%s:%X %d %d ",
			unitId.data(), itemId, formTime, toTime);
	if (reply == NULL || reply->type == REDIS_REPLY_ERROR || reply->type != REDIS_REPLY_ARRAY)
	{
		freeReplyObject(reply);
		return false;
	}

	for (int i = 0; i < (int)reply->elements; i++)
	{
		unsigned long t;
		int len = reply->element[i]->len;

		Poco::NumberParser::tryParseUnsigned64(string(reply->element[i]->str, 0, len), t);
		redisReply* reply1 = (redisReply*)redisCommand(_impl->_context, "del STR:SOURCE:DATA:%s:%X:%d ",
				unitId.data(), itemId, t);
		if (reply1 == NULL || reply1->type == REDIS_REPLY_ERROR)
		{
			freeReplyObject(reply1);
			continue;
		}
		freeReplyObject(reply1);
	}

	freeReplyObject(reply);

	reply = (redisReply*)redisCommand(_impl->_context, "zremrangebyscore ZSET:SOURCE:DATA:TIME:%s:%X %d %d",
			unitId.data(), itemId, formTime, toTime);

	if (reply == NULL || reply->type == REDIS_REPLY_ERROR)
	{
		freeReplyObject(reply);
		return false;
	}

	freeReplyObject(reply);
	return true;
}
