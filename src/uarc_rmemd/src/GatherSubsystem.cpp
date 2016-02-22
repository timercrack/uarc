/*
 * GatherSubsystem.cpp
 *
 *  Created on: 2014-10-21
 *      Author: root
 */

#include "GatherSubsystem.h"
#include "StoreExeSubsystem.h"
#include "StoreNotification.h"
#include <string>
#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <list>
#include <sys/time.h>


GatherSubsystem* g_pGath=NULL;
GatherSubsystem::GatherSubsystem() :
	_stopped(true), _thread() {

	// TODO Auto-generated constructor stub
}
GatherSubsystem::~GatherSubsystem()
{
	// TODO Auto-generated destructor stubstd::
	_rdbmsGath->stop();
	delete _rdbmsGath;

}

void GatherSubsystem::initialize(Poco::Util::Application & self)
{
	// TODO Subscribe the collect task from the rdbms
	printf("GatherSubsystem 初始化开始执行\n");
	_rtdbms_ip = self.config().getString("RedisDB.Base.IPAddr", "127.0.0.1");
	_rtdbms_port = self.config().getInt("RedisDB.Base.Port", 6379);
	_rdbmsGath = new CRtDbMs(_rtdbms_ip, _rtdbms_port);



}

void GatherSubsystem::uninitialize()
{

}

void GatherSubsystem::Start()
{
	poco_assert (_stopped);
	_stopped = false;
	_thread.start(*this);

}

const char *GatherSubsystem::name() const
{
	return "GatherSubsystem";
}

void GatherSubsystem::run()
{
	UarcRmemdServer::GetLogger().information("GatherSubsystem is running!");
	//注册归集任务回调，当有新指标产生时，通知本处理过程
	bool IsRegisCallback = false;
	IsRegisCallback = _rdbmsGath->setSecCollPeriodCallback(SecCollPeriodCallback);

	if (!IsRegisCallback)
	{
		UarcRmemdServer::GetLogger().information("注册规集任务失败!\n");

	}
	else
	{
		cout<<"注册规集指标信息任务成功"<<endl;
		UarcRmemdServer::GetLogger().information("注册规集任务成功!\n");
	}
	//TODO a instance which is used to observation collect task
	_rdbmsGath->start();
}
void GatherSubsystem::Stop()
{
	if (!_stopped)
	{
		_rdbmsGath->stop();
		_stopped = true;
		_thread.join();

	}
}


bool SecCollPeriodCallback(const UarcPeriodStateOfArch& periodInfo)
{
	//TODO 回调函数中添加归集周期到周期集合
		StoreExeSubsystem::_mutex_StoreExe.lock();
bool NeedInsert = true;
	if(periodInfo.status !=ITEM_ST_INVALID)
	{
		printf("接受指标信息回调前PeriodVecor有%d条指标\n",(int)StoreExeSubsystem::PeriodVecor.size());
		std::vector<UarcStorePeriodInfo>::iterator it_PeriodInfo;
		//int StorePeriod_temp=0;
		std::vector<UarcStorePeriodInfo>::iterator lastPosition;

		for (it_PeriodInfo = StoreExeSubsystem::PeriodVecor.begin(); it_PeriodInfo != StoreExeSubsystem::PeriodVecor.end();it_PeriodInfo++)
		{
			//归集周期错误，直接退出本次回调
			if(periodInfo.secCollPeriod < 60)
			{

				UarcRmemdServer::GetLogger().information("回调任务获得异常数据，请检查此刻下发的指标档案 secCollPeriod=%d\n",periodInfo.secCollPeriod);
				return false;
			}
			//如果该周期已经存在，则不需要再加入定时队列
		   if(periodInfo.secCollPeriod==it_PeriodInfo->secCollPeriod)
		   {
			   NeedInsert = false;
			   it_PeriodInfo->archCount++;
			   UarcRmemdServer::GetLogger().information("本次归集回调增加指标档案，使%d 周期计数器变为%d",(int)periodInfo.secCollPeriod,(int)it_PeriodInfo->archCount);
			   break;
		   }
		   //周期大于60秒
		   else if ((periodInfo.secCollPeriod >= 60))
		   {
			   NeedInsert = true;
			   break;
		   }

		}
		if(NeedInsert==true)
		{
			UarcStorePeriodInfo StorePeriodInfo;
			StorePeriodInfo.archCount = 1;
			StorePeriodInfo.lastSecCollTime = 0;
			StorePeriodInfo.secCollPeriod = periodInfo.secCollPeriod;
			StoreExeSubsystem::PeriodVecor.push_back(StorePeriodInfo);

			//加入定时队列
			time_t CurrentTime;
			CurrentTime = time(NULL);

			int FixedTime;
			bool bFixed = false;
			cout << "在回调中获得周期档案，首次执行，待修正的时间" << g_pGath->IntTimeTostring((int) CurrentTime) << endl;
			bFixed = g_pGath->FixedLatestTime(CurrentTime,periodInfo.secCollPeriod, FixedTime);
			if (!bFixed)
			{
				cout << "获取当前时间失败" << endl;
				UarcRmemdServer::GetLogger().information("归集回调中修改时间出错,下发时刻为:%d,请检查周期为%d s的指标",(int)time(NULL),StorePeriodInfo.secCollPeriod);
				StoreExeSubsystem::_mutex_StoreExe.unlock();
				return false;
			}
			//Poco::Timestamp dataTime;//microSecond   1second=1000000microseconds
			Poco::DateTime dataTime;
			dataTime += (periodInfo.secCollPeriod - (CurrentTime- FixedTime)) * 1000000;
			cout << "在回调中获得周期档案，首次执行，得到归集时刻" << g_pGath->IntTimeTostring(FixedTime+ periodInfo.secCollPeriod) << endl;
			cout << "在回调中获得周期档案，首次执行，设定触发时间" << periodInfo.secCollPeriod	- (CurrentTime - FixedTime) << "秒" << endl;

			Timedqueue.enqueueNotification(new StoreNotification((FixedTime	+ periodInfo.secCollPeriod),periodInfo.secCollPeriod), dataTime.timestamp());
		}
	}
	else
	{
		std::vector<UarcStorePeriodInfo>::iterator it_PeriodInfo;
		for(it_PeriodInfo = StoreExeSubsystem::PeriodVecor.begin(); it_PeriodInfo != StoreExeSubsystem::PeriodVecor.end(); it_PeriodInfo++)
	    {
			if (periodInfo.secCollPeriod==it_PeriodInfo->secCollPeriod)
			{
				it_PeriodInfo->archCount--;
				UarcRmemdServer::GetLogger().information("本次归集回调删除指标档案，使%d 周期计数器变为%d",(int)periodInfo.secCollPeriod,(int)it_PeriodInfo->archCount);

				break;

			}
		}
	}
	UarcRmemdServer::GetLogger().information("本次回调结束后PeriodVecor有%d条指标\n",(int)(StoreExeSubsystem::PeriodVecor.size()));
	printf("本次回调结束后PeriodVecor有%d条指标\n",(int)(StoreExeSubsystem::PeriodVecor.size()));
	StoreExeSubsystem::_mutex_StoreExe.unlock();
//
	return true;
}




std::string GatherSubsystem::IntTimeTostring(int InputTime)
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


bool GatherSubsystem::FixedLatestTime(const time_t CurrentTime, const int Frequence,int &fixedTimeInt)
{
    //TODO 将当前时间按周期修改成本小时开始，离本时刻最近的周期时刻
	int FixedTime = 0;
	int Time_N = 0;
	int tempFREQUENT = 0;
	struct tm current_time_TEPM;
	tempFREQUENT = ((Frequence / 60) != 0) ? (Frequence / 60) : 1;

	struct tm* current_time = localtime(&CurrentTime);

	for (Time_N = 0; Time_N < 60; Time_N++) {
		if (((Time_N * tempFREQUENT) <= current_time->tm_min) && (((Time_N + 1)	* tempFREQUENT) >= current_time->tm_min))
		{
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

