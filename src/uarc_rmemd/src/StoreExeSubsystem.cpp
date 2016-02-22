/*
 * StoreExeSubsystem.cpp
 *
 *  Created on: 2015-1-24
 *      Author: root
 */


#include "GatherSubsystem.h"
#include "StoreExeSubsystem.h"
#include "StoreNotification.h"
#include <unistd.h>
#include <Poco/Stopwatch.h>
#include <ctime>
using Poco::Stopwatch;
static Stopwatch sw_Store_To_RelationDB;




StoreExeSubsystem *p_StoreExe = NULL;

std::vector<UarcStorePeriodInfo> StoreExeSubsystem::PeriodVecor; //归集任务集合
FastMutex StoreExeSubsystem::_mutex_StoreExe;

StoreExeSubsystem::StoreExeSubsystem() :_stopped(true), _thread()
{
	// TODO Auto-generated constructor stub
	p_StoreExe = this;

}

StoreExeSubsystem::~StoreExeSubsystem()
{
	// TODO Auto-generated destructor stubstd::



	//_rdbmsStoreExe->stop();
	Stop();

	delete _rdbmsStoreExe;
	delete p_StoreExe;
}

void StoreExeSubsystem::initialize(Poco::Util::Application & self)
{
	// TODO Subscribe the collect task from the rdbms
	printf("StoreExeSubsystem 初始化开始执行\n");
	_nextStoreTime = self.config().getInt("GatherSystem.DayStoreTime", 86400);
	_rtdbms_ip = self.config().getString("RedisDB.Base.IPAddr", "127.0.0.1");
	_rtdbms_port = self.config().getInt("RedisDB.Base.Port", 6379);
	_rdbmsStoreExe = new CRtDbMs(_rtdbms_ip, _rtdbms_port);

	//实时数据库

	_ConnectInfo.IPAddr = self.config().getString("SmartRealDB.Base.IPAddr","192.168.25.87");
	_ConnectInfo.port = self.config().getInt("SmartRealDB.Base.Port", 6327);
	_ConnectInfo.UserName = self.config().getString("SmartRealDB.Base.UserName", "sa");
	_ConnectInfo.Password = self.config().getString("SmartRealDB.Base.Password",
			"smartreal");
	_ConnectInfo.table = self.config().getString("SmartRealDB.Base.TableName", "test");
	_ConnectInfo.NeedStore = self.config().getBool("BoolValue.NeedStoreToRealDB" ,false);
	gld = new GoldenWorks(_ConnectInfo);



	//关系数据库连接信息
	std::string relaDB_IPAddr = self.config().getString(
			"RelationDB.Base.IPAddr", "192.168.25.88");
	std::string relaDB_Port = self.config().getString("RelationDB.Base.Port",
			"3306");
	std::string relaDB_DataBaseName = self.config().getString(
			"RelationDB.Base.DataBaseName", "dev_test");
	std::string relaDB_UserName = self.config().getString(
			"RelationDB.Base.UserName", "enocu");
	std::string relaDB_Password = self.config().getString(
			"RelationDB.Base.Password", "enocu123");
	std::string relaDB_CompressState = self.config().getString(
			"RelationDB.Base.CompressState", "true");
	std::string relaDB_AutoReconnect = self.config().getString(
			"RelationDB.Base.AutoReconnect", "true");
	_connect_ralatinDB_str_gath += "host=" + relaDB_IPAddr;
	_connect_ralatinDB_str_gath += ";port=" + relaDB_Port;
	_connect_ralatinDB_str_gath += ";db=" + relaDB_DataBaseName;
	_connect_ralatinDB_str_gath += ";user=" + relaDB_UserName;
	_connect_ralatinDB_str_gath += ";password=" + relaDB_Password;
	_connect_ralatinDB_str_gath += ";compress=" + relaDB_CompressState;
	_connect_ralatinDB_str_gath += ";auto-reconnect=" + relaDB_AutoReconnect;

	//读取会话连接池参数
	_StoreminSessions = self.config().getInt("SessionPool.minSessions", 1);
	_StoremaxSessions = self.config().getInt("SessionPool.maxSessions", 32);
	_StoreidleTime = self.config().getInt("SessionPool.idleTime", 60);
	_pStoreDBSessionPool = new SessionPool(MySQL::Connector::KEY,	_connect_ralatinDB_str_gath, _StoreminSessions, _StoremaxSessions,	_StoreidleTime);
	//printf("关系库连接信息：%s\n",_connect_ralatinDB_str_gath.c_str());

}

void StoreExeSubsystem::uninitialize()
{

}

void StoreExeSubsystem::Start()
{
	poco_assert (_stopped);
	_stopped = false;
	_thread.start(*this);

}

const char *StoreExeSubsystem::name() const
{
	return "StoreExeSubsystem";
}

void StoreExeSubsystem::run()
{
	UarcRmemdServer::GetLogger().information("StoreExeSubsystem is running!");

	//TODO 循环执行归集操作
	while (!_stopped)
	{
		// TODO 采集任务循环执行
		bool bStore = false;
		bStore = p_StoreExe->StoreExecute();
        if(!bStore)
        {
        	UarcRmemdServer::GetLogger().information("本次无法执行StoreExecute!");

        }
	}
}
void StoreExeSubsystem::Stop()
{
	if (!_stopped)
	{
		_rdbmsStoreExe->stop();
		_stopped = true;
		//Poco::Timestamp dataTime;
		Poco::DateTime dataTime;//改用这个，当时间已经过去时，会立刻执行，而不必再等
		dataTime +=	10;
		Timedqueue.enqueueNotification(new StoreNotification(0,0),dataTime.timestamp());
		_thread.join();
		Timedqueue.clear();
		p_StoreExe = NULL;

	}

}


bool StoreExeSubsystem::StoreExecute()
{

	//TODO 获取指标档案并做各种条件判断（是否有效/是否存实时库...）

	UarcRmemdServer::GetLogger().information("定时队列元素个数%d",Timedqueue.size());
	//Timedqueue.waitDequeueNotification();//等待触发的到来
	RealTimeStoreVector.clear();
	Poco::Notification::Ptr pNf(Timedqueue.waitDequeueNotification());

	if (_stopped)
	{
		return false;
	}
	printf("触发执行,此时%s\n", IntTimeTostring((int) time(NULL)).c_str());
	if(pNf)
	{

		StoreNotification* pSNf = pNf.cast<StoreNotification> ();

		if(pSNf->SecStoreFrequent() <60)
		{
			UarcRmemdServer::GetLogger().information("归集周期为%d，直接退出，等待定时队列的下一个任务",(int)pSNf->SecStoreFrequent());
			return false;
		}
		//======当周期元素的计数值为0时，说明此时刻已经不许要再定时
		//======循环一边周期集合，如果元素计数为0，则不再设置该值的触发
		if(PeriodVecor.empty() == true)
		{
			UarcRmemdServer::GetLogger().information("周期集合没有元素，直接退出，等待新的任务");
			cout<<"周期集合没有元素，直接退出，等待新的任务"<<endl;
			Timedqueue.dequeueNotification();
			return false;
		}
		else
		{
			std::vector<UarcStorePeriodInfo>::iterator iter;
			for(iter = PeriodVecor.begin(); iter != PeriodVecor.end();iter++)
			{
               if(pSNf->SecStoreFrequent() == iter->secCollPeriod)
               {
				if(iter->archCount <=0)
				{
					iter = StoreExeSubsystem::PeriodVecor.erase(iter);
					UarcRmemdServer::GetLogger().information("该周期已经没有对应元素，无需设置下次归集时间");
					cout<<"该周期已经没有对应元素，无需设置下次归集时间"<<endl;
					return false;
				}

               }

			}
		}




		//首先设置下次归集时间
		cout<<"本次归集时间设置为"<<IntTimeTostring( pSNf->StoreTime())<<endl;
		long int tempdef=0;

		tempdef = pSNf->StoreTime()+pSNf->SecStoreFrequent()-time(NULL);

		//一旦小于0就不要再归集了，说明后面的队列都被延迟了，就设定好下次归集就退出
		if(tempdef <=0)
		{
			std::string delateTime = "";
			delateTime = IntTimeTostring(pSNf->StoreTime());
			UarcRmemdServer::GetLogger().information("归集任务处理不完，丢弃%s刻归集任务\n",delateTime);
			int i = 0;
		for (i = 0; tempdef <= 0; )
		{
			i++;
			tempdef = pSNf->StoreTime()+i*pSNf->SecStoreFrequent()-time(NULL);
			delateTime = IntTimeTostring(pSNf->StoreTime()+i*pSNf->SecStoreFrequent());
			UarcRmemdServer::GetLogger().information("直接下次设置下次归集时刻：%s\n",delateTime);

		}
		cout<<"从消息队列中得到周期是"<<pSNf->SecStoreFrequent()<<"距离下次触发的时间差是"<<tempdef<<endl;
		//Poco::Timestamp dataTime;//microSecond   1second=1000000microseconds
		Poco::DateTime dataTime;//改用这个，当时间已经过去时，会立刻执行，而不必再等
		dataTime += tempdef*1000000;
		Timedqueue.enqueueNotification(new StoreNotification((pSNf->StoreTime()+i*pSNf->SecStoreFrequent()),pSNf->SecStoreFrequent()),dataTime.timestamp());
	    cout<<"再次设定周期"<<tempdef<<"秒"<<endl;

			return false;
		}
		 else
		{
			//次处为没有被耽误的情况
			cout<<"从消息队列中得到周期是"<<pSNf->SecStoreFrequent()<<"距离下次触发的时间差是"<<tempdef<<endl;
			//Poco::Timestamp dataTime;//microSecond   1second=1000000microseconds
			Poco::DateTime dataTime;//改用这个，当时间已经过去时，会立刻执行，而不必再等
			dataTime += tempdef*1000000;
			Timedqueue.enqueueNotification(new StoreNotification((pSNf->StoreTime()+pSNf->SecStoreFrequent()),pSNf->SecStoreFrequent()),dataTime.timestamp());
			cout<<"再次设定周期"<<tempdef<<"秒"<<endl;
		}
		//归集相关的指标档案信息
		std::vector<UarcStoreItemInfo> StoreItemInfo;
		StoreItemInfo.clear();
		std::vector<CollDataStr> FinalStoreValue;
		int Priod;

		Priod = pSNf->SecStoreFrequent();//归集周期

cout << "pSNf->SecStoreFrequent()获取周期：" << pSNf->SecStoreFrequent() << endl;
        bool bGetInfo = false;
		bGetInfo = _rdbmsStoreExe->getAllArchsSuitSecPeriod(&StoreItemInfo,Priod);
        //获得该周期对应的指标失败，则返回
		if (!bGetInfo)
		{
			UarcRmemdServer::GetLogger().information("获取周期为%d的所有指标档案失败，请检查是否有该指标或redis是否启动\n",pSNf->SecStoreFrequent());
			printf("获取周期为%d的所有指标档案失败，请检查是否有该指标或redis是否启动\n",pSNf->SecStoreFrequent());
			return false;
		}
		else
		{
			//获取周期对应的指标成功后，要进一步判断获取的指标是否为空
			//获得该周期对应的指标不为空，则处理获取的指标
	      if(!StoreItemInfo.empty())
	      {
			std::vector<UarcStoreItemInfo>::iterator it_StoreItemInfo;
	        cout<<"指标档案容量："<<(int)StoreItemInfo.size()<<endl;
	        int failDataCouter = 0;//无法获取采集值的计数器
			for (it_StoreItemInfo = StoreItemInfo.begin(); it_StoreItemInfo!= StoreItemInfo.end(); it_StoreItemInfo++)
			{
	            if (_stopped)
		      {
			   return false;
		      }
	            int lastStoerTime = 0;
				double LastCollData = 0;
				bool bGetStoreValue = false;
				CollDataStr tempCollData;
				bGetStoreValue = _rdbmsStoreExe->getLastStoreValue(it_StoreItemInfo->unitId, it_StoreItemInfo->itemId,lastStoerTime, LastCollData);

				if (!bGetStoreValue)
				{
					printf("获取采集值失败unitId:%s, ItemId%X\n",it_StoreItemInfo->unitId.c_str(),	it_StoreItemInfo->itemId);
					//UarcRmemdServer::GetLogger().information("获取采集值失败unitId:%s, ItemId%d \n",it_StoreItemInfo->unitId, (int)it_StoreItemInfo->itemId);
					failDataCouter++;
					lastStoerTime = time(NULL);
					LastCollData = 0;
				}

                // val = 基值+val*系数
				tempCollData.lastStoreValue= it_StoreItemInfo->baseVal + LastCollData* it_StoreItemInfo->coefficient;

				tempCollData.unitId = it_StoreItemInfo->unitId;
				tempCollData.itemId = it_StoreItemInfo->itemId;
				tempCollData.lastStoreTime = lastStoerTime;

				//从通知队列取出的二次归集周期值

				tempCollData.timeStamp = p_StoreExe->IntTimeTostring(pSNf->StoreTime());
				tempCollData.Sql = it_StoreItemInfo->secCollSql;
				tempCollData.OtherSql = it_StoreItemInfo->secCollOtherSql;
				//将存储所需要的信息存入Vector
				FinalStoreValue.push_back(tempCollData);
				//实时库存储数据
				if(_ConnectInfo.NeedStore==true)
				{
				RealTimeDataStruct RlStoreData;
				char TagName[80]={0};
				sprintf(TagName, "%s_%X",it_StoreItemInfo->unitId.c_str(),it_StoreItemInfo->itemId );
				RlStoreData.Tagname = TagName;
				RlStoreData.tagId=0;
				RlStoreData.timeStamp = pSNf->StoreTime();
				RlStoreData.dataVal = tempCollData.lastStoreValue;
				RealTimeStoreVector.push_back(RlStoreData);

				}
			}
			UarcRmemdServer::GetLogger().information("获取采集值失败与总任务占比 [failed/total] = %d/%d",failDataCouter,(int)StoreItemInfo.size());
			//关系库存储
			bool bStored = false;
			bStored = p_StoreExe->DoubleSampleStoreToDB(FinalStoreValue);

			if (!bStored)
			{
				printf("归集失败存储,对应周期%d\n", Priod);
				 UarcRmemdServer::GetLogger().information("归集失败存储,对应周期%d\n", Priod);

			}
			//实时库存储
			if(_ConnectInfo.NeedStore==true)
			{
				bool bStore = false;
				bStore = gld->StoreData(RealTimeStoreVector);
				//p_StoreExe->RealTimeDBStore( RealTimeStoreVector, _ConnectInfo);
	          if(!bStore)
	          {
	        	  UarcRmemdServer::GetLogger().information("实时库存储失败");
	          }

			}
	      }
	      //若获取周期对应的指标为空
	      else
	      {
	    	  UarcRmemdServer::GetLogger().information(" 根据周期获得的指标档案为空,请检查内存库指标是否正确，退出本次定时归集\n");
	    	  return false;
			}


      }

}
	return true;
}

std::string StoreExeSubsystem::IntTimeTostring(int InputTime)
{

	//TODO 将int行 linux时间 修改为string类型的时间，格式为：“2015-01-27 12:04:18“
	char timeChar[128] = { 0 };
	std::string OutputTimeStr = "";
	const time_t t = InputTime;
	struct tm* current_time = localtime(&t);
	sprintf(timeChar, "\"%04d-%02d-%02d %02d:%02d:%02d\"",
			(current_time->tm_year + 1900), (current_time->tm_mon + 1),
			current_time->tm_mday, current_time->tm_hour, current_time->tm_min,
			current_time->tm_sec);
	OutputTimeStr += timeChar;
	return OutputTimeStr;
}

bool StoreExeSubsystem::DoubleSampleStoreToDB(std::vector<CollDataStr>& FinalStoreValue)
{
	sw_Store_To_RelationDB.start();
	//TODO 二次采样存储
cout<<"进入归集存储阶段"<<"FinalStoreValue"<<(int)FinalStoreValue.size()<<endl;
UarcRmemdServer::GetLogger().information("归集存储收到 %d 条数据\n",(int)FinalStoreValue.size());

	 std::string SQL;
	 std::string OtherSQL;
//	 int  failedNumber= 0;
//	 int VecNumber = (int)FinalStoreValue.size();
	 std::vector<CollDataStr>::iterator iter;
	try {
		Session ses = _pStoreDBSessionPool->get();
		 for(iter = FinalStoreValue.begin(); iter !=FinalStoreValue.end();iter++)
		 {
			if (_stopped)
			{
				return false;
			}
			try{
				if(!ses.isConnected())
				{
					cout<<"没有连接数据库"<<endl;
					return false;
				}
				SQL = AssembledSql(iter->unitId, iter->itemId,iter->lastStoreValue, iter->timeStamp, iter->Sql);
				 OtherSQL =AssembledOtherSql(iter->unitId,iter->itemId,iter->lastStoreValue,iter->timeStamp,iter->OtherSql);

				ses << SQL, now;
				ses << OtherSQL, now;
				ses.commit();
			}catch(Poco::Exception& ex)
			{

				 std::cout << ex.message() << std::endl;
				 UarcRmemdServer::GetLogger().error("二次采样归集关系表存储时，数据库连接异常[%s]\n",ex.message());
				continue;

			 }
		}
	}catch(Poco::Exception& ex)
	{
		std::cout << ex.message() << std::endl;
		UarcRmemdServer::GetLogger().error("二次采样归集关系表存储时，数据库连接异常[%s]\n",ex.message());
		return false;
	}

	 //存储用时
	 sw_Store_To_RelationDB.stop();
	 UarcRmemdServer::GetLogger().information("二次采样存储用时 %d ms\n",(int)sw_Store_To_RelationDB.elapsed()/1000);
         sw_Store_To_RelationDB.reset();
	return true;
}


std::string StoreExeSubsystem::AssembledSql(std::string unitId, int itemId, double val,
			std::string Storetime, std::string secCollSql)
{
	//TODO 把将要存储数据的数据拼接成SQL存储需要的格式（当前数据表）
	char sql[128] = { 0 };
	char refeshtime[128]={0};
	 const time_t t = time(NULL);
	//本地时间：日期，时间 年月日，星期，时分秒
	struct tm* current_time = localtime(&t);

	sprintf(refeshtime, "\"%04d-%02d-%02d %02d:%02d:%02d\"",
			(current_time->tm_year + 1900), (current_time->tm_mon + 1),
			current_time->tm_mday, current_time->tm_hour,
			current_time->tm_min, current_time->tm_sec);

	//insert into R_DATA_CURR (ms_no,data_date,indi_no,rval) values
	sprintf(sql, "%s(\"%s\",%s,\"%X\",%2f,%s)", secCollSql.c_str(), unitId.data(),
			Storetime.c_str(), itemId, val, refeshtime);
	return sql;
}

std::string StoreExeSubsystem::AssembledOtherSql(std::string unitId, int itemId, double val,
			std::string Storetime, std::string secCollOtherSql)
{
   //TODO 把将要存储数据的数据拼接成SQL存储需要的格式（状态表）
	char OtherSql[128] = { 0 };
	char itemIdChar[80] = { 0 };
	char unitIdChar[80] = { 0 };


	sprintf(itemIdChar, "\"%X\"", itemId);
	sprintf(unitIdChar, "\"%s\"", unitId.c_str());

	//update R_STATUS_CURR set curr_val=%2f ,refresh_time=%s where ms_no=%s and indi_no=%s

	sprintf(OtherSql, secCollOtherSql.c_str(), val,Storetime.c_str() , unitIdChar,itemIdChar);
	return OtherSql;
}

bool StoreExeSubsystem::FixedLatestTime(const time_t CurrentTime, const int Frequence,int &fixedTimeInt)
{
    //TODO 将当前时间按周期修改成本小时开始，离本时刻最近的周期时刻
	int FixedTime = 0;
	int Time_N = 0;
	int tempFREQUENT = 0;
	struct tm current_time_TEPM;
	tempFREQUENT = ((Frequence / 60) != 0) ? (Frequence / 60) : 1;

	struct tm* current_time = localtime(&CurrentTime);

	for (Time_N = 0; Time_N < 60; Time_N++) {
		if (((Time_N * tempFREQUENT) <= current_time->tm_min) && (((Time_N + 1)	* tempFREQUENT) >= current_time->tm_min)) {
			FixedTime = Time_N * tempFREQUENT;
		}
	}

	current_time_TEPM = *current_time;
	current_time_TEPM.tm_min = FixedTime;
	current_time_TEPM.tm_sec = 0;
	fixedTimeInt = (int)mktime(&current_time_TEPM);
	//printf("修改后的时间：%d\n",fixedTimeInt);
	return true;
}
