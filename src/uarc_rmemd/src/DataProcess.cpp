/*
 * DataProcess.cpp
 *
 *  Created on: 2014-10-25
 *      Author: root
 */

#include "DataProcess.h"
#include "Store_To_RunTime.h"
#include <string>
#include <iostream>
#include <stdio.h>
#include <sys/time.h>
#include "Poco/Data/MySQL/Connector.h"


using namespace Poco::Data;

DataProcess *g_pDataProcess = NULL;

DataProcess::DataProcess() :
		_stopped(true), _thread()
{
	// TODO Auto-generated constructor stub
	g_pDataProcess = this;

}

DataProcess::~DataProcess()
{
	// TODO Auto-generated destructor stub
	_rdbmsDataPro->stop();
	delete _rdbmsDataPro;
	Stop();

	g_pDataProcess = NULL;

}

void DataProcess::initialize(Poco::Util::Application & self)
{
	// TODO subscribe the data process from the rdbms

	//redis连接设置
	_rtdbms_redis_ip = self.config().getString("RedisDB.Base.IPAddr",
			"127.0.0.1");
	_rtdbms_redis_port = self.config().getInt("RedisDB.Base.Port", 6379);
	_rdbmsDataPro = new CRtDbMs(_rtdbms_redis_ip, _rtdbms_redis_port);

	//关系数据库连接信息
	std::string relaDB_IPAddr = self.config().getString("RelationDB.Base.IPAddr", "192.168.25.88");
	std::string relaDB_Port = self.config().getString("RelationDB.Base.Port","3306");
	std::string relaDB_DataBaseName = self.config().getString("RelationDB.Base.DataBaseName", "dev_test");
	std::string relaDB_UserName = self.config().getString("RelationDB.Base.UserName", "enocu");
	std::string relaDB_Password = self.config().getString("RelationDB.Base.Password", "enocu123");
	std::string relaDB_CompressState = self.config().getString("RelationDB.Base.CompressState", "true");
	std::string relaDB_AutoReconnect = self.config().getString("RelationDB.Base.AutoReconnect", "true");
	_connect_ralatinDB_str += "host=" + relaDB_IPAddr;
	_connect_ralatinDB_str += ";port=" + relaDB_Port;
	_connect_ralatinDB_str += ";db=" + relaDB_DataBaseName;
	_connect_ralatinDB_str += ";user=" + relaDB_UserName;
	_connect_ralatinDB_str += ";password=" + relaDB_Password;
	_connect_ralatinDB_str += ";compress=" + relaDB_CompressState;
	_connect_ralatinDB_str += ";auto-reconnect=" + relaDB_AutoReconnect;

	//读取会话连接池参数
	_DataminSessions = self.config().getInt("SessionPool.minSessions", 1);
	_DatamaxSessions = self.config().getInt("SessionPool.maxSessions", 32);
	_DataidleTime = self.config().getInt("SessionPool.idleTime", 60);
	_pDataDBSessionPool = new SessionPool(MySQL::Connector::KEY,_connect_ralatinDB_str, _DataminSessions, _DatamaxSessions, _DataidleTime);

}

void DataProcess::uninitialize()
{

}

void DataProcess::Start()
{
	poco_assert (_stopped);
	_stopped = false;
	_thread.start(*this);

}

const char *DataProcess::name() const
{
	return "DataProcess";
}



void DataProcess::run()
{
	UarcRmemdServer::GetLogger().information("DataProcess is running!");
	// 注册数据处理任务，返回指向数据处理任务

	bool IsRegised = false;
	IsRegised = _rdbmsDataPro->setDataProcCallback( DataProcessCallback );
	if (true == IsRegised)
	{
		UarcRmemdServer::GetLogger().information("注册数据处理任务成功!\n");
	} else
	{
		UarcRmemdServer::GetLogger().information("注册数据处理任务失败!\n");
		std::cout << __FILE__ << __LINE__ << "bool value" << IsRegised << std::cout;
	}

	_rdbmsDataPro->start();


}
void DataProcess::Stop()
{


	if (!_stopped) {

		_rdbmsDataPro->stop();

		_stopped = true;

		_thread.join();

		g_pDataProcess = NULL;
	}
}

// 数据处理
bool DataProcessCallback(const UarcProcDataItemArchive& procDataItem,const UarcProcData& procData)
{
	//TODO begin to process the DATA PROCESS

	g_pDataProcess->ProcessExe(procDataItem , procData);
	//UarcRmemdServer::GetLogger().information("执行数据处理回调函数!\n");


	return true;
}
bool DataProcess::ProcessExe( const UarcProcDataItemArchive procDataItem,const UarcProcData procData )
{
	//报警指标(只要不是模拟量，就算认为是报警信息-看内存表，指的是开关量)
    if(procDataItem.itemType !=ITEM_TY_ANALOG)
    {
	    //存储类型判断
    	switch (procDataItem.warnTypeMask)
		{
		//记录系统日志
		case ITEM_WARNING_LOG:
			UarcRmemdServer::GetLogger().information("有报警指标unitId:%s, ItemId%d\n",procData.unitId,(int)procData.itemId);
			break;

			//记录平台关系数据库
		case ITEM_WARNING_SQL:
		{
				g_pDataProcess->StoreWaringLog(procDataItem.unitId,procData.itemId,procDataItem.termId, procData.timeVal,procData.dataVal, procDataItem.warnStoreSql);
			break;
		}

		default:
			break;
		}

    }
	//不是报警指标(即模拟量只需要判断是否超出上下限)
    else
	{
    	UarcProcData TemprocData;
    	TemprocData.dataVal = procData.dataVal;
    	TemprocData.itemId = procData.itemId;
    	TemprocData.timeVal = procData.timeVal;
    	TemprocData.unitId = procData.unitId;
		//判断数据类型
		switch (procDataItem.itemType)
		{
		//模拟量
		case ITEM_TY_ANALOG:
		{
			//只有上限值大于下限值才有会上下限报警检查(平台缺省设置  上限=下限=0)
			if(procDataItem.downLimitVal < procDataItem.upLimitVal)
			{
				//数据来源于计算
/*
				if (procDataItem.srcType == ITEM_SRTY_CALCULATE)
				{
					printf(	"procDataItem档案显示:unitId:%s,itenId:%d的值dataVal:%f,不需要再示值换算\n",procDataItem.unitId.c_str(), procDataItem.itemId,TemprocData.dataVal);
				}
				//数据来源于非计算
				else
				{
					TemprocData.dataVal = procDataItem.baseVal+ ((TemprocData.dataVal) * (procDataItem.coefficient));

				}
*/
				TemprocData.dataVal = procDataItem.baseVal+ ((TemprocData.dataVal) * (procDataItem.coefficient));
				if ((TemprocData.dataVal > procDataItem.upLimitVal)||(TemprocData.dataVal < procDataItem.downLimitVal))
				{
					g_pDataProcess->StoreWaringLog(procDataItem.unitId,procDataItem.itemId,procDataItem.termId, TemprocData.timeVal,TemprocData.dataVal,procDataItem.warnStoreSql);

				}

				break;

			}

			else if((procDataItem.downLimitVal==0)&&(procDataItem.upLimitVal==0))
			{
				//print("平台默认设置了上限=下限=0\n");
				break;
			}
			//说明指标配置错误
			else if(procDataItem.downLimitVal>procDataItem.upLimitVal)
			{
				printf("平台设置的上限<下限，请重新检查unitId:%s, ItemId%X\n",procDataItem.unitId.c_str(),procDataItem.itemId);
				UarcRmemdServer::GetLogger().information("指标档案中的下限值大于上限值，请检查指标档案是否配置正确unitId:%s, ItemId%d\n",procDataItem.unitId,(int)procDataItem.itemId);
				break;
			}
			else
			{
				UarcRmemdServer::GetLogger().information("指标档案中的上下限值设置了非默认值，请检查指标档案是否配置unitId:%s, ItemId%d\n",procDataItem.unitId,(int)procDataItem.itemId);
				break;
			}
		}

		case ITEM_TY_DISPLAY:
		{
			//TemprocData.dataVal = procDataItem.baseVal	+ ((TemprocData.dataVal)*(procDataItem.coefficient));
			//无业务要求
			break;
		}
		default:
			break;
		}
	}

return true;
}
bool DataProcess::StoreWaringLog(std::string UnitId, int ItemId, int termId , int time,double DataVal, std::string warnStoreSql)
{
	char sql[300] = { 0 };
	std::string CharTime;
	TimeToChar(time, CharTime);
       sprintf(sql,"%s(\"%s\",\"%s\",\"%s\",\"%s\",\"%d\",%s,\"%s\",\"%s\",\"%X\",\'%2f\',\"%s\")",warnStoreSql.c_str(),"","","",UnitId.c_str(),termId,CharTime.c_str(),"","", ItemId,DataVal,"");
	printf("报警日志存储语句:%s\n", sql);


    try
  	    {

    	Session ses = _pDataDBSessionPool->get();
    	ses<< sql,now;
    	ses.commit();
  	    }catch (Poco::Exception& ex)
  	    {

  	        std::cout << "!!! WARNING: 报警日志存储连接关系库出错,无法存储!" <<sql<< std::endl;
  	        std::cout << ex.message() << std::endl;
  	        UarcRmemdServer::GetLogger().error("报警日志存储时，数据库连接异常[%s]\n",ex.message());
  	        return false;
  	    }

	return true;
}


bool DataProcess::TimeToChar(int CurrentTime, std::string &TimeChar)
{
/*
	//time_t是long类型，精确到秒，是当前时间和1970年1月1日零点时间的差
	const time_t t = time(NULL);
	//本地时间：日期，时间 年月日，星期，时分秒
	struct tm* current_time = localtime(&t);
	printf("当前时间：%d-%d-%d %d:%d:%d\t", current_time->tm_year + 1900,
			current_time->tm_mon + 1, current_time->tm_mday,
			current_time->tm_hour, current_time->tm_min, current_time->tm_sec);
*/
	char timeChar[128] = { 0 };
	const time_t t = CurrentTime;
	struct tm* current_time = localtime(&t);
	sprintf(timeChar,"\"%04d-%02d-%02d %02d:%02d:%02d\"",(current_time->tm_year+ 1900),
				(current_time->tm_mon + 1), current_time->tm_mday,
							current_time->tm_hour, current_time->tm_min, current_time->tm_sec);
	TimeChar +=timeChar;
	return true;
}

