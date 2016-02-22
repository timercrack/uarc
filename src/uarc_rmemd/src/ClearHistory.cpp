/*
 * ClearHistory.cpp
 *
 *  Created on: 2015-3-2
 *      Author: root
 */

#include "ClearHistory.h"
#include <string>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include "Poco/Timestamp.h"
#include "Poco/Timespan.h"
#include <Poco/Stopwatch.h>

using namespace Poco::Data;
using namespace Poco::Data::Keywords;


using Poco::Stopwatch;
Stopwatch Clearstwch;
ClearHistory *g_pClearHistory = NULL;
TimedNotificationQueue ClearQueue;
ClearHistory::ClearHistory() :
		_stopped(true), _thread()
{
	// TODO Auto-generated constructor stub
	g_pClearHistory = this;

}

ClearHistory::~ClearHistory()
{
	// TODO Auto-generated destructor stub
	_rdbmsClearHis->stop();
	delete _rdbmsClearHis;
	Stop();
	g_pClearHistory = NULL;

}

void ClearHistory::initialize(Poco::Util::Application & self)
{
	// TODO subscribe the data process from the rdbms
	//self.config().getString("");
	//redis连接设置
	_rtdbms_redis_ip = self.config().getString("RedisDB.Base.IPAddr","127.0.0.1");
	_rtdbms_redis_port = self.config().getInt("RedisDB.Base.Port", 6379);
	_rdbmsClearHis = new CRtDbMs(_rtdbms_redis_ip, _rtdbms_redis_port);
	ClearQueue.clear();

}

void ClearHistory::uninitialize()
{

}

void ClearHistory::Start()
{
	//poco_assert (_stopped);
	_stopped = false;
	_thread.start(*this);

}

const char *ClearHistory::name() const
{
	return "ClearHistory";
}

void ClearHistory::run()
{
	UarcRmemdServer::GetLogger().information("ClearHistory Process is running!");
	//1.获取当前时间
	time_t thistime;
	thistime = time(NULL);
	std::string TimeChar = "";
	g_pClearHistory->TimeToChar(thistime, TimeChar);
	UarcRmemdServer::GetLogger().information("首次执行，当前时间为：%s", TimeChar);


	//2.计算下次清除时间
	int nClearTime = g_pClearHistory->nextClearTime(thistime);

	long int timedeff = 0;
	timedeff = nClearTime - (long int) thistime;
	Poco::DateTime dataTime;
	dataTime += timedeff*1000000;

	//加入清除队列
	g_pClearHistory->TimeToChar(nClearTime,TimeChar);
	ClearQueue.enqueueNotification(new ClearNotofication(nClearTime),dataTime.timestamp());
	UarcRmemdServer::GetLogger().information("首次执行，设置下次清除数据时间为：%s", TimeChar);
	printf("首次执行，设置下次清除数据时间为：%s\n", TimeChar.c_str());
	while (!_stopped)
	{

		//1.等待清除任务时刻的到来
		Poco::Notification::Ptr pNf(ClearQueue.waitDequeueNotification());
		if (_stopped)
		{
			return ;
		}
		if(pNf)
		{
			//ClearNotofication* pSNf = pNf.cast<ClearNotofication> ();
			//2先设置下次清除时间
			time_t thistime;
			thistime = time(NULL);
			std::string TimeChar = "";
			g_pClearHistory->TimeToChar(thistime, TimeChar);

			UarcRmemdServer::GetLogger().information("清除%s 时刻的定时任务",TimeChar);


			//3.计算下次清除时间
			int nClearTime = g_pClearHistory->nextClearTime(thistime);
			long int timedeff = 0;
			timedeff = nClearTime - (long int) thistime;
			Poco::DateTime dataTime;
			dataTime += timedeff*1000000;
			//4再加入清除队列
			g_pClearHistory->TimeToChar(nClearTime,TimeChar);
			ClearQueue.enqueueNotification(new ClearNotofication(nClearTime ),dataTime.timestamp());

			UarcRmemdServer::GetLogger().information("设置下次清除数据时间为：%s", TimeChar);
			//5此时执行清除处理
			Clearstwch.restart();
			bool bCleard = false;
			bCleard = _rdbmsClearHis->clearHisData();
			Clearstwch.stop();
			if (bCleard == true)
			{
			 UarcRmemdServer::GetLogger().information("清除历史数据成功,用时%d 秒",(int)Clearstwch.elapsedSeconds());
			}
			else
			{
				UarcRmemdServer::GetLogger().information("清除历史数据失败,用时%d 秒",(int)Clearstwch.elapsedSeconds());
				UarcRmemdServer::GetLogger().information("再次调用清除命令");
				bCleard = _rdbmsClearHis->clearHisData();
				if (bCleard == true)
				{
					UarcRmemdServer::GetLogger().information("再次清除历史数据并且成功被清除");
				}
				else
				{
					UarcRmemdServer::GetLogger().information("连续两次清除历史均失败");
				}
			}

		}
	}
	UarcRmemdServer::GetLogger().information("ClearHistory Process quit!", __FILE__,	__LINE__);
}

void ClearHistory::Stop()
{


	if (!_stopped) {

		_rdbmsClearHis->stop();

		_stopped = true;
		Poco::DateTime dataTime;
		dataTime +=	10;
		ClearQueue.enqueueNotification(new ClearNotofication(0 ),dataTime.timestamp());
		_thread.join();
		ClearQueue.clear();
		g_pClearHistory = NULL;
	}
}

int ClearHistory::nextClearTime( time_t current)
{

    int nextClearTime = 0;
    struct tm* current_timeSt;
    struct tm current_time_TEPM;
    current_timeSt = localtime(&current);

    //把当前时间修改成
    current_time_TEPM = *current_timeSt;
    current_time_TEPM.tm_mday = current_timeSt->tm_mday +1;
    current_time_TEPM.tm_hour = 0;
    current_time_TEPM.tm_min  = 3;
    current_time_TEPM.tm_sec  = 0;

    nextClearTime = (int)mktime(&current_time_TEPM);
    return nextClearTime;
}

bool ClearHistory::TimeToChar(int CurrentTime, std::string &TimeChar)
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
	TimeChar =timeChar;
	return true;
}
