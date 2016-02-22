//============================================================================
// Name        : CalcSubsystem.h
// Author      :
// Version     :
// Copyright   :
// Description : 声明class UarcCalculateSubsystem, Ansi-style
//============================================================================


#include "rdbms.h"
#include <Poco/Util/Subsystem.h>
#include <vector>
#include <stdio.h>
#include "fomular.h"
class CalcSubsystem : public Poco::Util::Subsystem, public Poco::Runnable {

struct TaskCounter
{
	TaskCounter(int _ReceivedTaskNum = 0,int _InvalidTaskNum = 0,int _CalculatedNum = 0,int _FailedSavedNum = 0,int _FailedUpdateNum = 0):
		ReceivedTaskNum(_ReceivedTaskNum),InvalidTaskNum(_InvalidTaskNum),CalculatedNum(_CalculatedNum),FailedSavedNum(_FailedSavedNum),
		FailedUpdateNum(_FailedUpdateNum)
	{}
		int ReceivedTaskNum; //收到的计算任务总数
		int InvalidTaskNum; //收到的无效计算任务总数
		int CalculatedNum; //完成计算的个数
		int FailedSavedNum; //保存失败的计算数据总数
		int FailedUpdateNum; //最终更新的计算档案失败个数
};

public:

	CalcSubsystem();
	const char* name() const;
	virtual ~CalcSubsystem();
	void initialize(Poco::Util::Application& self);
	void uninitialize();
	void run();
	void Start();
	void Stop();

	bool CalcExe(UarcCalTaskArchive  CalcArch, UarcCalcResult &Result);
private:

	std::string     _rtdbms_ip;
	int             _rtdbms_port;

	bool 			_stopped;
	Poco::Thread 	_thread;
	CRtDbMs*         _rdbmsCalc;
	int            _CalcFrquent;



};
void showLocalTime();
