/*
 * Store_To_RunTime.h
 *
 *  Created on: 2014-11-18
 *      Author: root
 */

#ifndef STORE_TO_RUNTIME_H_
#define STORE_TO_RUNTIME_H_

#include "RealTimeHeader.h"
#include "goldenapi.h"
#include "golden.h"
#include "golden_error.h"
#include "rdbms.h"
#include <iostream>

//配置文件中内容
struct ConntDBProperties
{
	std::string IPAddr;
	int         port;
	std::string UserName;
	std::string Password;
	std::string table;
	bool NeedStore;
};

//连接数据库信息结构体
struct ConnectStr
{
	ConnectStr(const char* _hostname = NULL,golden_int32 _port = 0,golden_int32 _handle = 0):
		hostname(_hostname),port(_port),handle(_handle)
	{}

           const char* hostname;//字符串，输入，GOLDEN 数据平台服务器的网络地址或机器名
           golden_int32 port;//整型，输入，缺省值 6327，连接端口
           golden_int32 handle;//整型，输出，连接句柄
};



//登录数据库信息结构体
struct LoginStr
{
	LoginStr(golden_int32 _handle = 0, const char* _user = NULL, const char* _password = NULL, golden_int32 _priv = 0):
		handle(_handle),user(_user),password(_password),priv(_priv)
	{}
         golden_int32 handle;//连接句柄
         const char* user;//字符串，输入，登录帐户
         const char* password;//字符串，输入，帐户口令
         golden_int32 priv;//整型，GOLDEN_RO, GOLDEN_DW, GOLDEN_TA, GOLDEN_SA 之一，输出，帐户权限
};
//寻找标识结构体
	struct FpStr
	{
	FpStr(golden_int32 _handle = 0, golden_int32 _count = 0,const char *_p ="",char* _q = NULL,
			golden_int32 _ids = 0,golden_int32 _types = 0, golden_int32 _classof = 0,golden_int16 _use_ms = 0

			):
		handle(_handle),count(_count),q(_q),ids(_ids),types(_types),classof(_classof),use_ms(_use_ms)
	{
		strncpy(p,_p, 80);
	}
		 golden_int32 handle;//连接句柄
         golden_int32 count;//整数，输入/输出，输入时表示标签点个数
              //(即table_dot_tags、ids、types、classof、use_ms 的长度)，
              //输出时表示找到的标签点个数
        //const char* const* table_dot_tags;   //字符串指针数组，输入，"表名.标签点名" 列表
        char p[160];
        char* q;
          //table_dot_tags;
        golden_int32 ids ;   // 整型数组，输出，标签点标识列表, 返回 0 表示未找到
        golden_int32 types;   //整型数组，输出，标签点数据类型
        golden_int32 classof;  //整型数组，输出，标签点类别
        golden_int16 use_ms;    // 短整型数组，输出，时间戳精度，
                   //返回 1 表示时间戳精度为毫秒， 为 0 表示为秒。
	};
//创建标识点结构体
	struct InStPStr
{
		InStPStr(golden_int32 _handle = 0, const char *_tag = NULL,golden_int32 _type = 0,golden_int32 _table_id = 0
				,golden_int16 _use_ms = 0, golden_int32 _point_id = 0):
					handle(_handle),tag(_tag),type(_type),table_id(_table_id),use_ms(_use_ms),point_id(_point_id)
		{}
         golden_int32 handle;     //连接句柄
        const char *tag;         //字符串，输入，标签点名称
        golden_int32 type;       //整型，输入，标签点数据类型，取值 GOLDEN_BOOL、GOLDEN_UINT8、GOLDEN_INT8、
                   //GOLDEN_CHAR、GOLDEN_UINT16、GOLDEN_UINT32、GOLDEN_INT32、GOLDEN_INT64、
                   //GOLDEN_REAL16、GOLDEN_REAL32、GOLDEN_REAL64、GOLDEN_COOR、GOLDEN_STRING、GOLDEN_BLOB 之一。
       golden_int32 table_id;    //整型，输入，标签点所属表 id
       golden_int16 use_ms;      //短整型，输入，标签点时间戳精度，0 为秒；1 为毫秒。
        golden_int32 point_id;   // 整型，输出，标签点 id
};

	//创建带压缩的标识点结构体
	struct InStComPStr
{
		InStComPStr(golden_int32 _handle = 0, GOLDEN_SCAN_POINT *_scan = 0, GOLDEN_CALC_POINT *_calc = 0):
			handle(_handle),scan(_scan),calc(_calc)
		{}
golden_int32 handle;
GOLDEN_POINT base;
GOLDEN_SCAN_POINT *scan;
GOLDEN_CALC_POINT *calc;
};
//存储结构体
struct PutDataStr
{
		PutDataStr(golden_int32 _handle = 0,golden_int32 _count = 0,
				const golden_int32 _ids = 0,const golden_int32 _datetimes = 0,
				const golden_int16 _ms = 0,const golden_float64 _values = 0,
				const golden_int64 _states = 0,golden_int16 _qualities = 0,
				golden_error _errors = 0):
					handle(_handle),ids(_ids),datetimes(_datetimes),ms(_ms),
					values(_values),states(_states),qualities(_qualities),errors(_errors)
		{}
	golden_int32 handle;//连接句柄
	golden_int32 count;//整型，输入/输出，标签点个数，
	                    //输入时表示 ids、datetimes、ms、values、states、qualities、errors 的长度，
	                    //输出时表示成功写入实时值的标签点个数
	  golden_int32 ids;//整型数组，输入，标签点标识列表，同一个标签点标识可以出现多次，
	                        //但它们的时间戳必需是递增的。
	  golden_int32 datetimes;//整型数组，输入，实时数值时间列表,
	                              //表示距离1970年1月1日08:00:00的秒数
	  golden_int16 ms;//短整型数组，输入，实时数值时间列表，
	                       //对于时间精度为毫秒的标签点，表示相应的毫秒值；否则忽略
	  golden_float64 values;//双精度浮点型数组，输入，实时浮点型数值列表，
	                             //对于数据类型为 GOLDEN_REAL16、GOLDEN_REAL32、GOLDEN_REAL64 的标签点，存放相应的快照值；否则忽略
	  golden_int64 states; //64 位整型数组，输入，实时整型数值列表，
	                            //对于数据类型为 GOLDEN_BOOL、GOLDEN_UINT8、GOLDEN_INT8、GOLDEN_CHAR、GOLDEN_UINT16、GOLDEN_INT16、
	                            //GOLDEN_UINT32、GOLDEN_INT32、GOLDEN_INT64 的标签点，存放相应的快照值；否则忽略
	 golden_int16 qualities;//短整型数组，输入，实时数值品质列表，数据库预定义的品质参见枚举 GOLDEN_QUALITY
	golden_error errors;//无符号整型数组，输出，写入实时数据的返回值列表，参考golden_error.h
};

class GoldenWorks {
public:
	GoldenWorks(ConntDBSetting Info);
	~GoldenWorks();
	bool connect_DB(ConnectStr CnctInfo);
	bool Login_DB(LoginStr LoginInfo);
	bool Disconnect_DB(golden_int32 handle);
	bool Put_snapshots( PutDataStr StoreStr);
	bool Get_Snapshots(PutDataStr GetPtStrt);
	bool Find_points( FpStr &pointerInfo);
	//bool insert_point();
	bool Insert_base_point( InStPStr InstPoint );
	bool Insert_point(InStComPStr &InstComPoint);
	bool GetTableId( int handle,GOLDEN_TABLE &GlTable );

	bool StoreData(RealTimeDataStruct  StoreDataStru);
	bool StoreData(std::vector<RealTimeDataStruct> RealTimeStoreVector);
    golden_int32 getHandle() const;
    void setHandle(golden_int32 handle);

private:
	golden_int32 _handle;
	ConntDBSetting _Info;

};
#endif /* STORE_TO_RUNTIME_H_ */
