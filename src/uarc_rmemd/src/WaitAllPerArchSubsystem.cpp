/*
 * WaitAllPerArchSubsystem.cpp
 *
 *  Created on: 2015-2-12
 *      Author: root
 */

#include "WaitAllPerArchSubsystem.h"

WaitAllPerArchSubsystem* g_pWaitAllPerArch = NULL;
TimedNotificationQueue Timedqueue;
WaitAllPerArchSubsystem::WaitAllPerArchSubsystem() :_stopped(true), _thread()
{
	// TODO Auto-generated constructor stub
	g_pWaitAllPerArch = this;

}

WaitAllPerArchSubsystem::~WaitAllPerArchSubsystem()
{
	// TODO Auto-generated destructor stub
	_rdbmsGetPerArch->stop();
	delete _rdbmsGetPerArch;
	Stop();

	g_pWaitAllPerArch = NULL;

}

void WaitAllPerArchSubsystem::initialize(Poco::Util::Application & self)
{
	// TODO subscribe the data process from the rdbms

	//redis连接设置
	_rtdbms_redis_ip = self.config().getString("RedisDB.Base.IPAddr",
			"127.0.0.1");
	_rtdbms_redis_port = self.config().getInt("RedisDB.Base.Port", 6379);
	_rdbmsGetPerArch = new CRtDbMs(_rtdbms_redis_ip, _rtdbms_redis_port);

	Timedqueue.clear();//清空通知时间队列
}

void WaitAllPerArchSubsystem::uninitialize()
{

}

void WaitAllPerArchSubsystem::Start()
{
	poco_assert (_stopped);
	_stopped = false;
	_thread.start(*this);

}

const char *WaitAllPerArchSubsystem::name() const
{
	return "WaitAllPerArchSubsystem";
}


void WaitAllPerArchSubsystem::Stop()
{


	if (!_stopped) {

		_rdbmsGetPerArch->stop();

		_stopped = true;

		_thread.join();

		g_pWaitAllPerArch = NULL;
	}
}

void WaitAllPerArchSubsystem::run()
{
	printf("WaitAllPerArchSubsystem 初始化开始执行\n");
	//TODO 获取所有二次采样归集相关的信息档案
	cout<<"开始获取内存中的归集周期任务"<<endl;
			bool bgetGathTask = false;
			StoreExeSubsystem::_mutex_StoreExe.lock();
			bgetGathTask = _rdbmsGetPerArch->getAllSecPeriodOfArch(&StoreExeSubsystem::PeriodVecor);

			if (_stopped)
			{
				StoreExeSubsystem::_mutex_StoreExe.unlock();
				return ;
			}

			if (bgetGathTask)
			{
               if(StoreExeSubsystem::PeriodVecor.empty() == true)
               {
            	   cout<<"内存库中的归集周期集合为空"<<endl;
            	   UarcRmemdServer::GetLogger().information("初始化获取内存库中的归集周期集合为空，解锁周期集合并退出\n");
            	   StoreExeSubsystem::_mutex_StoreExe.unlock();
            	   return;
               }
				time_t CurrentTime;

				int recordCnt = 0;
				std::vector<UarcStorePeriodInfo>::iterator it_PeriodInfo;
				for (it_PeriodInfo = StoreExeSubsystem::PeriodVecor.begin(); it_PeriodInfo	!= StoreExeSubsystem::PeriodVecor.end(); )
				{

					if (_stopped)
					{
						StoreExeSubsystem::_mutex_StoreExe.unlock();
						return ;
					}
					if((*it_PeriodInfo).secCollPeriod < 60 )
                     {

						StoreExeSubsystem::PeriodVecor.erase( it_PeriodInfo );
						UarcRmemdServer::GetLogger().information("获得异常数据，secCollPeriod=%d,已经剔除\n",it_PeriodInfo->secCollPeriod);
						continue;
                      }

					std::cout << "***** 第 " << ++recordCnt << " 条任务记录 *****"	<< std::endl;
					std::cout << "secCollPeriod  :" << (*it_PeriodInfo).secCollPeriod<< std::endl;
					std::cout << "ArchNumber  :"<< (*it_PeriodInfo).archCount << std::endl;
					UarcRmemdServer::GetLogger().information("初始化获得%d周期下 %d条指标档案数",(*it_PeriodInfo).secCollPeriod,(int)it_PeriodInfo->archCount);

					CurrentTime = time(NULL);

					int FixedTime;
					bool bFixed=false;
		            cout<<"首次执行，待修正的时间"<<IntTimeTostring((int)CurrentTime)<<endl;
					bFixed = g_pWaitAllPerArch->FixedLatestTime(CurrentTime,it_PeriodInfo->secCollPeriod,FixedTime);
					if(!bFixed)
					{
						cout<<"获取当前时间失败"<<endl;
						it_PeriodInfo++;
						continue;
					}
					Poco::DateTime dataTime;
					//Poco::Timestamp dataTime;//microSecond   1second=1000000microseconds
					dataTime += (it_PeriodInfo->secCollPeriod-(CurrentTime-FixedTime))*1000000;
					cout<<"首次执行，得到归集时刻"<<IntTimeTostring(FixedTime+it_PeriodInfo->secCollPeriod)<<endl;
					cout<<"首次执行，设定触发时间"<<it_PeriodInfo->secCollPeriod-(CurrentTime-FixedTime)<<"秒"<<endl;

					Timedqueue.enqueueNotification(new StoreNotification((FixedTime+it_PeriodInfo->secCollPeriod),it_PeriodInfo->secCollPeriod),dataTime.timestamp());
					it_PeriodInfo++;
				}
			}
			else
			{
				std::cout<< std::endl;
				std::cout<< "---init get AllSecPeriodOfArch failed! please check redis start!!---"<< std::endl;

			}

			StoreExeSubsystem::_mutex_StoreExe.unlock();
			UarcRmemdServer::GetLogger().information("初始化获取归集周期集合结束");
}

std::string WaitAllPerArchSubsystem::IntTimeTostring(int InputTime)
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


bool WaitAllPerArchSubsystem::FixedLatestTime(const time_t CurrentTime, const int Frequence,int &fixedTimeInt)
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
