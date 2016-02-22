#include <iostream>
#include "uarc_rmemd.h"
#include "CalcSubsystem.h"
#include <string>
#include <unistd.h>
#include <ctime>
#include <sys/time.h>

using namespace std;

CalcSubsystem *_CalcSub = NULL;
CalcSubsystem::CalcSubsystem() :
	_stopped(true), _thread()
{
	// TODO Auto-generated constructor stub
	_CalcSub = this;
}

CalcSubsystem::~CalcSubsystem()
{
	// TODO Auto-generated destructor stub
	delete _rdbmsCalc;
	Stop();
	_CalcSub = NULL;
}

void CalcSubsystem::initialize(Poco::Util::Application & self)
{

	printf("CalcSubsystem 初始化开始执行\n");
	_rtdbms_ip = self.config().getString("RedisDB.Base.IPAddr", "127.0.0.1");
	_rtdbms_port = self.config().getInt("RedisDB.Base.Port", 6379);
	_rdbmsCalc = new CRtDbMs(_rtdbms_ip, _rtdbms_port);
	_CalcFrquent	= self.config().getInt("CalculateSubsystem.sleepTime", 30000);

	printf("计算频率为：%d毫秒\n", _CalcFrquent);
}

void CalcSubsystem::uninitialize()
{
	Stop();
}
void CalcSubsystem::Start()
{
	poco_assert (_stopped)
		;
	_stopped = false;
	_thread.start(*this);
}
const char* CalcSubsystem::name() const
{
	return "CalcSubsystem";
}
void CalcSubsystem::run()
{
	UarcRmemdServer::GetLogger().information("CalcSubsystem is running!");

	while (!_stopped) {

		if (g_pevtWakeup->tryWait(_CalcFrquent))
			break;
		//1.获取所有计算任务档案
		std::vector<UarcCalTaskArchive> AllCalcTaskVec, tempCalcTaskVec;
		AllCalcTaskVec.clear();
		tempCalcTaskVec.clear();
		TaskCounter tCounter;
		bool bget = false;
		bget = _rdbmsCalc->getAllCalcTask(&tempCalcTaskVec);
		tCounter.ReceivedTaskNum = (int)tempCalcTaskVec.size();//统计本次收到的计算任务总数
		//未接收或者集合为空则结束本次
		if ((!bget) || (tempCalcTaskVec.empty())) {
			//UarcRmemdServer::GetLogger().information("本次未能获得计算任务!\n");
			continue;
		}

		else {
			while (tempCalcTaskVec.size() > 1)
			{
				std::vector<UarcCalTaskArchive>::iterator iter, minPosition;
				minPosition = tempCalcTaskVec.begin();

				for (iter = (tempCalcTaskVec.begin() + 1); iter	!= tempCalcTaskVec.end();)
				{
					if (minPosition->priority > iter->priority)
					{
						minPosition = iter;
					}
					else
					{
						iter++;
					}

				}
				AllCalcTaskVec.push_back(*minPosition);
				tempCalcTaskVec.erase(minPosition);
			}
			AllCalcTaskVec.push_back(tempCalcTaskVec.front());

            tempCalcTaskVec.clear();
			//2.遍历获取的计算档案
			time_t detaTime = 0;
			std::vector<UarcCalTaskArchive>::iterator iter;
			for (iter = AllCalcTaskVec.begin(); iter != AllCalcTaskVec.end(); )
			{
				//剔除：  频率=0、无效的档案
				if ((iter->freq <= 0) || (iter->status == CALC_T_UNAVAILABLE))
				{
					tCounter.InvalidTaskNum++; //统计无效计算指标个数
					iter = AllCalcTaskVec.erase(iter);
					continue;
				}
				//3.执行计算
				//进行计算的条件：没计算过 或 达到甚至超过了计算时间
				detaTime = iter->lastUpdate + iter->freq - time(NULL);
				if ((iter->status == CALC_T_NO_EXEC) || (detaTime <= 0))
				{

					bool bCacled = false;
					UarcCalcResult CalcResult;
					bCacled = _CalcSub->CalcExe(*iter, CalcResult);

					if (bCacled)
					{
						tCounter.CalculatedNum++;//统计成功计算的任务总数
						//4.保存结果
						bool bSaved = false;
						bSaved = _rdbmsCalc->saveCalcResult(&CalcResult);

						if (!bSaved)
						{
							tCounter.FailedSavedNum++;//统计本轮保存失败的计算数据个数
							UarcRmemdServer::GetLogger().information("计算结果保存失败!unitId:%s ItemId%d\n",iter->unitId, iter->itemId);
						}
					}
					//5.更新计算任务档案
					UarcCalcTaskModified UpdataTask;
					UpdataTask.calcId = iter->calcId;
					UpdataTask.lastUpdate = time(NULL);
					UpdataTask.status = CALC_T_EXEC;
					UpdataTask.unitId = iter->unitId;
					bool bsavedup = false;
					bsavedup = _rdbmsCalc->saveCalcTaskPara(UpdataTask);
					if (!bsavedup)
					{
						tCounter.FailedUpdateNum++;//统计更新计算指标失败的总数
						//UarcRmemdServer::GetLogger().information("计算档案更新失败!unitId:%s ItemId%d\n", iter->unitId,iter->itemId);
					}
				}
				AllCalcTaskVec.erase(iter);
			}

		}
		UarcRmemdServer::GetLogger().information("本轮收到 %d 计算任务，其中无效任务%d 个，完成计算 %d 个 ，保存计算结果失败 %d 更新任务档案失败 %d ",
				tCounter.ReceivedTaskNum,
				tCounter.InvalidTaskNum,
				tCounter.CalculatedNum,
				tCounter.FailedSavedNum,
				tCounter.FailedUpdateNum);
	}
	UarcRmemdServer::GetLogger().information("CalcSubsystem thread quit!",__FILE__, __LINE__);
}
void CalcSubsystem::Stop()
{
	if (!_stopped)
	{
		_rdbmsCalc->stop();
		_stopped = true;
		_thread.join();
		_CalcSub = NULL;
	}
}

bool CalcSubsystem::CalcExe(UarcCalTaskArchive  CalcArch,	UarcCalcResult &Result)
{
	UarcCalcParaItem CalcPaItem;
	double tempVal[FORMULA_PARAM_NUM]={0};

	CFomular f;
	f.AddConstants();
	//获取参数
	for (int i = 0; i < FORMULA_PARAM_NUM; i++)
	{
		char buf[8] = { 0 };
		sprintf(buf, "P%02d", i + 1);
		if (CalcArch.paramMask & (1 << i))
		{
           bool bgetCalc = false;
           bgetCalc = _rdbmsCalc->getCalcParaValue(CalcArch.params[i].unitId,CalcArch.params[i].itemId, tempVal[i], &CalcPaItem);
			if(!bgetCalc)
			{
				UarcRmemdServer::GetLogger().information("获取计算参数失败! 指标%s:%d[params:%02d = %s:%d]\n",CalcArch.unitId,CalcArch.itemId,(i+1),CalcArch.params[i].unitId,CalcArch.params[i].itemId);
				tempVal[i] = 0;

			}
           //计算需要换算示值
			tempVal[i] = CalcPaItem.baseVal + (tempVal[i]	* (CalcPaItem.coefficient));

			f.AddVariable(buf, tempVal[i]);
		}
	}
	//计算
	const char* ExprBuf = CalcArch.expr.c_str();
	if (f.Compile(ExprBuf))
	{
		double Calcresult = f.Value();
		Result.unitId = CalcArch.unitId;
		Result.itemId = CalcArch.itemId;
		Result.resultVal = Calcresult;
		Result.calcId = CalcArch.calcId;
		Result.timeVal = time(NULL);

	}
	else
	{
		UarcRmemdServer::GetLogger().information("计算公式编译出错%s,[ unitId=%s ,itemid=%d ]",string(f.CompileError()),CalcArch.unitId,CalcArch.itemId);
		return false;
	}
    printf("公式%s的计算结果为：%2f",CalcArch.expr.c_str(),Result.resultVal);
	return true;
}
