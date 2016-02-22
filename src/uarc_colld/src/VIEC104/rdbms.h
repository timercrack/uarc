//#pragma once
#ifndef RDBMS_H_
#define RDBMS_H_

#include <vector>
#include <map>
#include<string>
#include<string.h>

#define PROTOCOL_NEWENERGYEFFI 0   //DeviceProtocol
#define PROTOCOL_376 1
#define PROTOCOL_MODBUS 2
#define PROTOCOL_104 3

#define DEVICE_LINK_T_TCPIP 0   //linkType
#define DEVICE_LINK_T_RS485 1
#define DEVICE_LINK_T_USB 2
#define DEVICE_LINK_T_DBLINK 3

#define DEVICE_ST_UNAVAILABLE 0		//DeviceStatus
#define DEVICE_ST_AVAILABLE 1
#define DEVICE_ST_DISCONN 2
#define DEVICE_ST_CONNING 3
#define DEVICE_ST_STOP 4

//#define ITEM_TM_INVALID 0  //ItemCodeType 指标档案：无效数据
//#define ITEM_TM_DISP 1     //指标档案：示值指标
//#define ITEM_TM_WARN 2     //指标档案：告警指标
//#define ITEM_TM_CALC 4     //指标档案：计算指标

#define ITEM_TY_ANALOG 0	//--itemType 指标类型：模拟量
#define	ITEM_TY_DIGITAL 1	//指标类型：数字量
#define ITEM_TY_DISPLAY 2	//指标类型：示值

#define ITEM_SRTY_DEVICE 0		//来源类型：采集设备
#define ITEM_SRTY_CALCULATE 1	//来源类型：公式计算

#define ITEM_WARNING_NO 0      //--warnType 无警告
#define ITEM_WARNING_LOG 1		//记录日志
#define ITEM_WARNING_SQL 2		//记录平台关系库

#define ITEM_STORE_T_NO 0  	//--dayStoreType 不归集
#define ITEM_STORE_T_DAYSTORE 1	//普通归集
#define ITEM_SOTRE_T_DOBLSAMP 2	//二次采样归集
#define ITEM_STORE_T_STAT 3		//统计归集

#define ITEM_CODE_UNAVAILABLE 0     //--ItemCodeStatus 无效
#define ITEM_CODE_AVAILABLE 1		//有效

#define FORMULAR_P_LEVEL1 1   //FormularPriority 1>2>3>4>5
#define FORMULAR_P_LEVEL2 2
#define FORMULAR_P_LEVEL3 3
#define FORMULAR_P_LEVEL4 4
#define FORMULAR_P_LEVEL5 5  //

#define CALC_T_UNAVAILABLE 0	//calcTask status
#define CALC_T_AVAILABLE 1
#define CALC_T_EXEC 2
#define CALC_T_NO_EXEC 3

#define COLLTASK_ST_INVALID 0   //采集任务状态：无效 (平台下发过来的状态)
#define COLLTASK_ST_VALID 1     //采集任务状态：有效  （平台下发过来的状态）
#define COLLTASK_ST_NO_EXEC 2   //采集任务状态：不执行 （在内存库的状态）
#define COLLTASK_ST_EXECING 3   //采集任务状态：正在执行 （在内存库中的状态，由子系统维护）

#define ITEM_ST_INVALID 0   //指标档案：无效
#define ITEM_ST_VALID	1   //指标档案:有效

#define FORMULA_PARAM_NUM 32    //计算公式参数最大个数
#define CALC_FIRST_TIME 0		//第一次计算时间（计算任务的第一次计算的标示）

/*****平台数据服务接口结构*****/
struct CollTask {   		   //--采集任务结构体

	CollTask(std::string _unitId = std::string(""),int _taskStatus = 0,long _startDate = 0,
			int _freq = 0,int _collRound = 0, int _onFail = 0,int _lastCollTime = 0):unitId(_unitId),
			taskStatus(_taskStatus),startDate(_startDate),freq(_freq),collRound(_collRound),
			onFail(_onFail),lastCollTime(_lastCollTime)
	{}

	std::string   unitId;        		   		//用能单元Id
	std::vector<int>    itemIds;        //指标Id
	int     taskStatus;            		//任务状态，0：无效；1：未执行；2：执行中
	int     startDate;      	  		//开始时间
	int     freq;           	  		//采集频率
	int     collRound;     		  		//采集次数
	int     onFail;        		  		//失败处理方式
	int     lastCollTime;   	  		//最后采集时间
};

struct ItemInfo {   //--指标配置结构体

	ItemInfo(int _itemId = -1, std::string _unitId = std::string(""),
			int _deviceId = -1,int _termId = -1, int _itemType = 0,int _srcType =0,
			double _upLimitVal = 0.0, double _downLimitVal = 0.0,
			int _warnTypeMask = -1, double _baseVal=0.0, int _coefficient=0.0,
			std::string _dayStoreTime = "",std::string _dayStoreSql = "",
			std::string _warnStoreSql = "",int _secCollPeriod=0,std::string _secCollSql = "",
			std::string _secCollOtherSql = "",int _storeType = 0,int _status = ITEM_ST_INVALID) :
			itemId(_itemId),unitId(_unitId),deviceId(_deviceId),termId(_termId),
			itemType(_itemType),srcType(_srcType),upLimitVal(_upLimitVal),
			downLimitVal(_downLimitVal),warnTypeMask(_downLimitVal),baseVal(_baseVal),
			coefficient(_coefficient),dayStoreTime(_dayStoreTime),dayStoreSql(_dayStoreSql),
			warnStoreSql(_warnStoreSql),secCollPeriod(_secCollPeriod),secCollSql(_secCollSql),
			secCollOtherSql(_secCollOtherSql),storeType(_storeType),status(_status)
	{}

	int     itemId;         		//指标Id
	std::string     unitId;         //用能单元Id
	int     deviceId;       		//设备Id
	int		termId;					//终端表号
	int     itemType;       		//数据类型（指标类型）
	int		srcType;				//来源类型
	double  upLimitVal;     		//上限值
	double  downLimitVal;   		//下限值
	int     warnTypeMask;   		//报警类型掩码
	double  baseVal;        		//基值
	int     coefficient;    		//系数
	std::string   dayStoreTime;   	//日归集时间
	std::string   dayStoreSql;    	//日归集SQL
	std::string   warnStoreSql;   	//告警归集SQL
	int     secCollPeriod;  		//二次采样周期
	std::string	  secCollSql;		//二次采样归集SQL
	std::string	  secCollOtherSql;   //二次采样另一个SQL
	int     storeType;   		    //归集类型
	int		status;					//0:无效指标;1:有效指标
};

struct DataKey{            //--计算公式参数结构体
	DataKey(std::string _unitId = std::string(""),int _itemId = -1):unitId(_unitId),itemId(_itemId)
	{}
	std::string   unitId;           //用能单元Id
	int   itemId;           //数据项Id
};

struct CalculateTask {        //---计算任务结构体
	CalculateTask(int _calcId = -1,std::string _unitId = std::string(""),int _itemId = -1,
			int _priority = -1,int _freq = -1,int _lastUpdate = CALC_FIRST_TIME,std::string _formula = "",
			int _paraMask = 0,int _status=CALC_T_UNAVAILABLE):calcId(_calcId),unitId(_unitId),
			itemId(_itemId),priority(_priority),freq(_freq),lastUpdate(_lastUpdate),
			formula(_formula),paramMask(_paraMask),status(_status)
	{}

	int			calcId;           //计算任务Id
	std::string			unitId;           //用能单元Id
	int			itemId;           //数据项Id
	int         priority;	      //计算等级
	int		    freq;			  //时间片，计算频率，单位：秒
	int         lastUpdate;		  //上次计算时间
	std::string  formula;	  //公式
	int   		paramMask;	  //整数2进制存储格式的BIT0-BIT8表示参数1-参数8。1表示对应参数项有效，0表示对应参数项无效
	DataKey     params[FORMULA_PARAM_NUM];	 //参数集合
	int			status;		 //0:无效任务；1：有效任务
};

struct ZhaoceTask{        //--召测任务结构体
	ZhaoceTask(std::string _unitId = std::string(""),int _deviceId = -1,int _termId = -1,int _itemId = -1):
		unitId(_unitId),deviceId(_deviceId),termId(_termId),itemId(_itemId)
	{}
	std::string		unitId;         //用能单元Id
	int		deviceId;       //设备Id
	int		termId;			//终端表号
	int		itemId;         //数据项Id
};

struct ZhaoCeData{   //--召测数据结构体
	ZhaoCeData(std::string _unitId = std::string(""),int _deviceId = -1,int _itemId = -1,int _termId = -1,
				long _zhaoceTime = 0,double _zhaoCeValue = 0.0):unitId(_unitId),
				deviceId(_deviceId),itemId(_itemId),termId(_termId),
				zhaoceTime(_zhaoceTime),zhaoCeValue(_zhaoCeValue)
	{}
	std::string		unitId;	  //用能单元Id
	int		deviceId;		//设备Id
	int 	itemId;			//数据项Id
	int		termId;			//终端表号
	int		zhaoceTime;		//召测数据对应的时间
	double 	zhaoCeValue;  	//召测回来的数据
};

struct CtrlTask{        //--控制任务结构体
	CtrlTask(std::string _unitId = std::string(""),int _deviceId = -1,
			int _termId = -1,int _itemId = -1,double _ctrlVal = 0.0):unitId(_unitId),
			deviceId(_deviceId),termId(_termId),itemId(_itemId),ctrlVal(_ctrlVal)
	{}
	std::string		unitId;		//用能单元Id
	int			deviceId;		//设备Id
	int			termId;			//终端表号
	int			itemId;			//指标Id
	double		ctrlVal;		//控制值
};

struct ScheduleTask{  //--配比计划任务档案结构体
	ScheduleTask(int _deviceId = -1,int _termId = -1,int _itemId = -1,
			double _scheduleVal = 0.0, int _scheduleTime = 0):deviceId(_deviceId),
			termId(_termId),itemId(_itemId),scheduleVal(_scheduleVal),
			scheduleTime(_scheduleTime)
	{}
	int deviceId;    //设备Id
	int termId;      //终端编号
	int itemId;      //指标Id
	double scheduleVal;   //配比值
	int scheduleTime;     //配比时间
};

struct DeviceInfo{   //--设备档案结构体
	DeviceInfo(int _deviceId = -1, int _status = 0,std::string _address = "",
				int _port = -1,std::string _identity = "",int _linkType = -1,
				int _protocol = -1, int _pulsePeriod = -1, int _lastConnTime = -1):
					deviceId(_deviceId),status(_status),address(_address),port(_port),
					identity(_identity),linkType(linkType),protocol(_protocol),
					pulsePeriod(_pulsePeriod),lastConnTime(_lastConnTime)
	{}

	int		deviceId;      //设备Id
	int		status;        //状态
	std::string	address;   //设备地址
	int		port;          //端口号
	std::string	identity;  //唯一标识
	int		linkType;      //链接类型
	int		protocol;      //规约类型
	int		pulsePeriod;   //心跳周期
	int	lastConnTime;  //上次在线时间
};

/*****************采集执行结构体************************/
struct CollTaskData{      //--采集任务结构体
	CollTaskData(std::string _unitId = std::string(""), int _taskId = -1, int _status = -1,
				 int _deviceId = -1, int _exeRound= -1, int _startDate = 0,
				 int _freq = -1,int _onfail = -1, int _lastCollTime = 0):
					 unitId(_unitId),taskId(_taskId),status(_status),deviceId(_deviceId),
				 exeRound(_exeRound),startDate(_startDate),freq(_freq),
				 onfail(_onfail),lastCollTime(_lastCollTime)
	{}

	std::string		unitId;    //用能单元Id
	int		taskId;			   // 任务标识
	int		status;			   // 任务状态
	int		deviceId;          //设备ID
	int		exeRound;		   // 执行次数
	int		startDate;         //开始时间
	int		freq;              //采集频率
	int		onfail;            //失败处理方式
	int		lastCollTime;	   // 上次执行时间
};

struct CollExecInfoData{    //--采集执行信息数据结构体
	CollExecInfoData(std::string _unitId = std::string(""), int _deviceId = -1):
		unitId(_unitId),deviceId(_deviceId)
	{}

	std::string         unitId;		      // 所属用能单元
	int         deviceId;		  // 设备ID
	std::map<int,int> itemAndTerms;  //指标和终端值对<itemId,termId>
};

struct CollectedData {     //--采集到的数据结构体
	CollectedData(int _deviceId = -1, int _termId = -1, int _itemId = -1,
				  long _time = 0,double _value = 0.0):deviceId(_deviceId),
				  termId(_termId),itemId(_itemId),time(_time),value(_value)
	{}

    int		 deviceId;            //设备ID
    int		 termId;			  //终端表号
    int		 itemId;              //指标Id
    int		 time;			      // 采集时间
	double   value;			      // 数据值
};

struct ZhaoCeExecInfoData{    //--召测任务数据结构体
	ZhaoCeExecInfoData(std::string _unitId = std::string(""), int _deviceId = -1,
						int _termId = -1,int _itemId = -1):unitId(_unitId),
						deviceId(_deviceId),termId(_termId),itemId(_itemId)
	{}

	std::string		   unitId;	   //用能单元Id
	int        deviceId;           //设备ID
	int		   termId;				//终端表号
	int        itemId;             //指标ID
};

struct ZhaoceBackData{         //--召测数据结构
	ZhaoceBackData(std::string _unitId = std::string(""), int _deviceId = -1,int _termId = -1,
				  int _itemId = -1,int _time = 0, double _value = 0.0):
					  unitId(_unitId),deviceId(_deviceId),termId(_termId),
					  itemId(_itemId),time(_time),value(_value)
	{}

	std::string		   unitId;  //用能单元Id
	int        deviceId;       //设备ID
	int		   termId;		   //终端表号
	int        itemId;         //指标ID
	int			time;		//召测时间
	double	   value;	   //召测数据值
};

struct ControlExecInfoData{    //--控制命令数据结构体
	ControlExecInfoData(int _deviceId =-1,int _termId = -1,int _itemId = -1,
			double _ctrlValue = 0.0):deviceId(_deviceId),termId(_termId),
			itemId(_itemId),ctrlValue(_ctrlValue)
	{}

	int        deviceId;           //设备Id
	int	 	   termId;			   //终端表号
	int        itemId;             //指标Id
	double     ctrlValue;          //控制值
};

struct ScheduleInfoData{  //--配比计划数据结构体
	ScheduleInfoData(int _deviceId = -1,int _termId = -1,int _itemId = -1,
			double _scheduleVal = 0.0, int _scheduleTime = 0):deviceId(_deviceId),
			termId(_termId),itemId(_itemId),scheduleVal(_scheduleVal),
			scheduleTime(_scheduleTime)
	{}
	int deviceId;    //设备Id
	int termId;      //终端编号
	int itemId;      //指标Id
	double scheduleVal;   //配比值
	int scheduleTime;     //配比时间
};

/*************采集数据处理***************/
struct UarcStorePeriodInfo{     //指标相关的归集周期信息
	UarcStorePeriodInfo(int _secCollPeriod = 0,int _archCount = 0,int _lastSecCollTime = 0):
		                 secCollPeriod(_secCollPeriod),archCount(_archCount),
		                 lastSecCollTime(_lastSecCollTime)

		{}
	int         secCollPeriod;      //二次采样周期
	int			archCount;			//指标个数
	int			lastSecCollTime;		//二次采样上次归集时间
};

struct UarcPeriodStateOfArch{     //某个指标对应二次采样周期的状态，
	UarcPeriodStateOfArch(int _secCollPeriod = 0,int _status = -1):
		secCollPeriod(_secCollPeriod),status(_status)
	{}
	int			secCollPeriod;    //二次采样周期
	int			status;           //对应指标的状态:有效和无效；
};

struct UarcStoreItemInfo{   //归集相关的指标档案信息
	UarcStoreItemInfo(std::string _unitId = std::string(""), int _itemId = -1,
							int _itemType = 0,double _baseVal = 0,int _coefficient =0,
							std::string _secCollSql = std::string(""),
							std::string _secCollOtherSql = std::string(""),
							int _storeType = 0):unitId(_unitId),itemId(_itemId),
							itemType(_itemType),baseVal(_baseVal),coefficient(_coefficient),
							secCollSql(_secCollSql),secCollOtherSql(_secCollOtherSql),
							storeType(_storeType)
		{}
	std::string         unitId;	      //所属用能单元（33,66,99）的集合
	int         itemId;	      //指标ID（如代表电压值的33）
	int			itemType;     //指标类型
	double		baseVal;	  //基值
	int			coefficient;		//系数
	std::string	 secCollSql;		//二次采样SQL
	std::string	  secCollOtherSql;   //二次采样另一个SQL
	int    		storeType;          //归集类型
};

struct UarcNextStoreTimeInfo{ //--下次归集定时结构体
	UarcNextStoreTimeInfo(std::string _unitId = std::string(""), int _itemId = -1, int _seconds = -1,
			std::string _dayStoreTime = ""):unitId(_unitId),itemId(_itemId),
			seconds(_seconds),dayStoreTime(_dayStoreTime)
	{}

	std::string	 		unitId;     	//用能单元编号
	int 		itemId;    		//数据项编号
	int 		seconds;   		//时间值
	std::string dayStoreTime;	//日归集时间
};
struct  UarcProcDataItemArchive{   //--处理的数据对应指标结构体
	UarcProcDataItemArchive(std::string _unitId = std::string(""), int _itemId = -1,
							int _termId = -1,int _itemType = 0,
							int _srcType = 0,double _upLimitVal = 0.0,
							double _downLimitVal = 0.0,int _warnTypeMask = 0,
							double _baseVal = 0.0,int _coefficient = 0,
							std::string _warnStoreSql = ""):unitId(_unitId),
							itemId(_itemId),termId(_termId),itemType(_itemType),srcType(_srcType),
							upLimitVal(_upLimitVal),downLimitVal(_downLimitVal),
							warnTypeMask(_warnTypeMask),baseVal(_baseVal),
							coefficient(_coefficient),warnStoreSql(_warnStoreSql)
	{}

	std::string         unitId;			// 所属用能单元
	int         itemId;			// 指标ID（如代表电压值的33）
	int			termId;          //终端表号
	int         itemType;       // 数据类型(指标类型)
	int			srcType;		//来源类型
	double      upLimitVal;       //上限值
	double 	    downLimitVal;     //下限值
	int			warnTypeMask;    //报警类型掩码
	double		baseVal;		     //基值
	int			coefficient;		 //系数
	std::string	warnStoreSql;      //告警日志SQL
};

struct UarcProcData{     //--处理数据结构体
	UarcProcData(std::string _unitId = std::string(""), int _itemId = -1, int _timeVal = -1,
				double _dataVal = 0.0):unitId(_unitId),itemId(_itemId),
				timeVal(_timeVal),dataVal(_dataVal)
	{}

	std::string			unitId;			//用能单元Id
	int			itemId;			//数据项Id
	int			timeVal;		//时间
	double		dataVal;		//数据
};
struct UarcDataKey{            //--计算公式参数结构体
	UarcDataKey(std::string _unitId = std::string(""),int _itemId = -1):unitId(_unitId),itemId(_itemId)
	{}
	std::string   unitId;           //用能单元Id
	int   itemId;           //数据项Id
};

struct UarcCalTaskArchive{    //--计算任务档案结构体
	UarcCalTaskArchive(int _calcId =-1,std::string _unitId = std::string(""), int _itemId = -1,
						std::string _expr = "",int _paramMask = 0,int _priority = -1,
						int _freq = -1,int _status = -1,int _lastUpdate = CALC_FIRST_TIME):calcId(_calcId),
						unitId(_unitId),itemId(_itemId),expr(_expr),paramMask(_paramMask),
						priority(_priority),freq(_freq),status(_status),lastUpdate(_lastUpdate)
	{}

	int			calcId;			//计算任务编号
	std::string         unitId;	         //用能单元ID
	int			itemId;		   //计算结果
	std::string   expr;		     //公式
	int			paramMask;		//参数掩码
	UarcDataKey     params[FORMULA_PARAM_NUM];	 //参数集合
	int         priority;       //计算优先级  1 > 2 > 3 > 4 > 5
	int         freq;           //计算频率，单位：秒
	int			status;			//计算任务状态
	int         lastUpdate;     //上次计算时间
};

struct UarcCalcParaItem{     //--计算参数对应的指标档案
	UarcCalcParaItem(std::string _unitId = std::string(""),int _itemId = -1,int _itemType = 0,
			double _baseVal = 0,int _coefficent = 0):unitId(_unitId),
			itemId(_itemId),itemType(_itemType),baseVal(_baseVal),
			coefficient(_coefficent)
	{}

	std::string		 unitId;			//用能单元Id
	int		 itemId;			//数据项Id
	int		 itemType;			//数据类型掩码
	double 	 baseVal;			//基值
	int 	coefficient;			//系数
};

struct UarcCalcResult{    //--计算结果结构体
	UarcCalcResult(int _calcId = -1,std::string _unitId = std::string(""),int _itemId = -1,
			int _timeVal =0, double _resultVal = 0.0):calcId(_calcId),
			unitId(_unitId),itemId(_itemId),timeVal(_timeVal),resultVal(_resultVal)
	{}

	int				calcId;		 //计算任务编号
	std::string		unitId;  	 //保存到用能单元号
	int				itemId;		//保存到的指标Id
	int				timeVal;     //时间值
	double			resultVal;   //结果值
};

struct UarcCalcTaskModified{
	UarcCalcTaskModified(int _calcId = -1,std::string _unitId = std::string(""),
			int _status = -1,int _lastUpdate =CALC_FIRST_TIME):calcId(_calcId),
			unitId(_unitId),status(_status),lastUpdate(_lastUpdate)
	{}
	int			calcId;			//计算任务编号
	std::string         unitId;	         //用能单元ID
	int			status;			//计算任务状态
	int         lastUpdate;     //上次计算时间
};

/**********规约适配*************/
struct stDeviceInfo{
	stDeviceInfo(std::string _identity = std::string(""),int _id = -1, const char *_ip = "",
			int _port = -1,int _status = -1, int _link_type = -1, int _protocol_type = -1,
			int _heart_period = -1, int _last_connect_time = -1) :identity(_identity),
			id(_id), port(_port),status(_status), link_type(_link_type),
			protocol_type(_protocol_type),heart_period(_heart_period),
			last_connect_time(_last_connect_time)
	{
		memcpy(ip, _ip, strlen(_ip)>19?19:strlen(_ip));
		ip[19] = 0;
	}

	std::string		identity;			//唯一标识
	int				id;   		//设备Id
	char			ip[20];				//IP地址
	int				port;				//端口号
	int				status;				// 设备状态
	int				link_type;			//链接类型
	int				protocol_type;		//协议类型
	int				heart_period;		//心跳周期
	int				last_connect_time;	//上次连接时间
};

class CRtDbMs;
typedef bool RtDbMsCallback_ZhaoceData(const ZhaoCeData& zhaoceData,CRtDbMs* rtdbmsPt);

typedef bool RtDbMsCallback_DeviceCntl(const ControlExecInfoData& savedCtrlTask);
typedef bool RtDbMsCallback_DevSchedule(const ScheduleInfoData& savedScheduleInfo);
typedef bool RtDbMsCallback_ZhaoceInfo(const ZhaoCeExecInfoData& zhaoceInfo);
typedef bool RtDbMsCallback_NewCollTask(const CollTaskData& collTask);

typedef bool RtDbMsCallback_SecCollPeriod(const UarcPeriodStateOfArch& periodInfo);
typedef bool RtDbMsCallback_DataProcess(const UarcProcDataItemArchive& procDataItem,
										const UarcProcData& procData);

typedef bool RtDbMsCallback_DeviceInfo(const stDeviceInfo& stdDevInfo);

class CRtDbMs
{
public:
   CRtDbMs(std::string ip = std::string(""),int port = -1);
   ~CRtDbMs();
   bool setZhaoceDataCallback(RtDbMsCallback_ZhaoceData callback);  //注册召测数据回调

   bool setDeviceCntlCallback(RtDbMsCallback_DeviceCntl callback);	//注册设备控制回调
   bool setDevScheduleCallback(RtDbMsCallback_DevSchedule callback); //注册配比计划下发回调
   bool setZhaoceCallback(RtDbMsCallback_ZhaoceInfo callback);		//注册召测任务回调
   bool setnewCollTaskCallback(RtDbMsCallback_NewCollTask callback);  //注册新采集任务回调

   bool setSecCollPeriodCallback(RtDbMsCallback_SecCollPeriod callback);		//注册归集指标信息回调
   bool setDataProcCallback(RtDbMsCallback_DataProcess callback);		//注册数据处理回调

   bool setDeviceInfoCallback(RtDbMsCallback_DeviceInfo callback);		//注册设备信息回调

   void start();
   void stop();
   /**********平台服务接口*************/
   bool saveCollTask(CollTask *savedCollTask);       //采集任务配置操作接口
   bool saveItemInfo(ItemInfo *savedItemInfo);       //指标配置操作接口
   bool saveCalculateTask(CalculateTask *savedCalculateTask);  //计算任务配置操作接口
   bool saveZhaoceTask(ZhaoceTask *savedZhaoceTask);    //召测任务配置操作接口
   bool saveControlTask(CtrlTask *savedCtrlTask);       //控制任务配置操作接口
   bool saveScheduleTask(ScheduleTask *savedScheduleTask);   //配比计划配置操作接口
   bool saveDeviceInfo(DeviceInfo *savedDevInfo);       //设备档案信息配置操作接口

   /***********采集执行接口**************/
   bool getAllCollTask (std::vector<CollTaskData> *allCollTask);  //获得所有采集任务
   bool saveCollTaskPara(CollTaskData collTaskPara);     		  //存放采集任务相关参数
   bool getCollExecInfo (std::string unitId,int collTaskId,CollExecInfoData* collExecInfo); //获得采集执行信息
   bool saveCollectedData(CollectedData* collData);   				//存储采集到的数据
   bool saveZhaoceData(ZhaoceBackData* zhaoceData);       			//存储召测到的数据

   /***********采集数据处理接口*************/
   bool getAllSecPeriodOfArch(std::vector<UarcStorePeriodInfo> *secPeriodsVector);  //获得所有需要二次采样的指标的二次采样周期
   bool getAllArchsSuitSecPeriod(std::vector<UarcStoreItemInfo> *storeItemInfos,int secCollPeriod); //获得满足二次采样周期的所有指标
   bool getStoreValue(std::string unitId, int itemId, std::map<int,double> &storeValue); //获得指定项的所有时刻值
   bool getLastStoreValue(std::string unitId, int itemId, int &lastStoreTime, double &lastStoreValue);  //获得指定项的最新时刻的值
   bool saveProcessedData(UarcProcData procData);   //存储处理后的数据
   bool getAllCalcTask(std::vector< UarcCalTaskArchive > *allCollTask); //获得所有的计算任务
   bool getCalcParaValue(std::string unitId,int itemId, double& paraValue,UarcCalcParaItem *uarcCalParaItem); //获得指定数据时间键的参数值
   bool saveCalcResult(UarcCalcResult *calcResult);     //向内存库中存放计算结果
   bool saveCalcTaskPara(UarcCalcTaskModified calcTaskModified);    //存放计算任务相关参数
   bool clearHisData();   //清除历史数据；

   /************规约适配处理接口************/
   bool getDeviceInfo(std::vector<stDeviceInfo> &deviceInfos);   //获得所有设备信息
   bool updateDeviceInfo(const stDeviceInfo *deviceInfo);		//更新设备信息

   struct impl;
   impl* _impl;
   void* privateData;
   bool userCancel;

};

#endif /*RDBMS_H_*/
