/*
 * Store_To_RunTime.cpp
 *
 *  Created on: 2014-11-18
 *      Author: root
 */

#include "Store_To_RunTime.h"
#include "uarc_rmemd.h"
#include <ctime>
#include <Poco/Stopwatch.h>
using namespace std;
using Poco::Stopwatch;
static Stopwatch sw_Store_To_RunTime;


GoldenWorks::GoldenWorks( ConntDBSetting Info )
{

	_Info = Info;
}

GoldenWorks::~GoldenWorks()
{
	Disconnect_DB( this->getHandle());

}
bool GoldenWorks::connect_DB( ConnectStr CnctInfo )
{
	int nRet = 0;

	nRet = go_connect(CnctInfo.hostname, CnctInfo.port, &CnctInfo.handle);
	if (nRet != 0)
	{

		UarcRmemdServer::GetLogger().error("%d时刻连接实时库出错! [ %d  ]\n",(int)time(NULL),(int)nRet);
		//cout << "连接实时库出错! [" << nRet << "]" << endl;
	} else {
				this->setHandle(CnctInfo.handle);
		//cout << "连接实时库成功，准备登录...!" << endl;

	}

	return true;
}

bool GoldenWorks::Login_DB( LoginStr LoginInfo )
{
	int nRet = 0;

	nRet = go_login(LoginInfo.handle, LoginInfo.user, LoginInfo.password, &LoginInfo.priv);
	if (nRet != 0)
	{
		//cout << "登录实时库出错! [" << nRet << "]" << endl;
		UarcRmemdServer::GetLogger().error("%d时刻登录实时库出错! [ %d  ]\n",(int)time(
						NULL),(int)nRet);
	} else {
		//cout << "登录实时数据库成功!" << endl;
	}

	return true;
}

bool GoldenWorks::Disconnect_DB( golden_int32 handle )
{
	//断开与数据库的链接
	int nRet = 0;
	nRet = go_disconnect( handle );
	if (nRet != 0) {
		//cout << "断开实时库出错! [" << nRet << "]" << endl;
		UarcRmemdServer::GetLogger().error("%d时刻断开实时库出错! [ %d  ]\n",(int)time(
								NULL),(int)nRet);
	} else {
		//cout << "断开实时库成功!" << endl;
	}

	return true;
}

bool GoldenWorks::Find_points( FpStr &pointerInfo)
{

	int nRet = 0;
	nRet = gob_find_points(pointerInfo.handle, &pointerInfo.count,
			&pointerInfo.q, &pointerInfo.ids, &pointerInfo.types,
			&pointerInfo.classof, &pointerInfo.use_ms);
	if ( nRet != 0)
	{
		//cout << "无法找到标点(本身没有缓存表)! [" << nRet<<"]"<<endl;
		UarcRmemdServer::GetLogger().error("%d时刻无法找到标点(本身没有缓存表)! [ %d  ]\n",(int)time(
										NULL),(int)nRet);
	}
	return true;
}

bool GoldenWorks::Insert_base_point( InStPStr InstPoint )
{
	int nRet = 0;

	InstPoint.type = 10;//float
	nRet = gob_insert_base_point(InstPoint.handle, InstPoint.tag,
			InstPoint.type, InstPoint.table_id, InstPoint.use_ms,
			&InstPoint.point_id);
	if (nRet != 0)
		//cout << "无法插入基本点 ! [" << nRet << "]" << endl;
		UarcRmemdServer::GetLogger().error("%d时刻无法插入基本点 ! [ %d  ]\n",(int)time(
												NULL),(int)nRet);
	return true;
}

bool GoldenWorks::Insert_point(InStComPStr &InstComPoint)
{
	int nRet = 0;
	nRet = gob_insert_point(InstComPoint.handle, &InstComPoint.base, InstComPoint.scan,InstComPoint.calc);
	if (nRet != 0)
		UarcRmemdServer::GetLogger().error("%d时刻无法插入基本点 ! [ %d  ]\n",(int)time(NULL),(int)nRet);
		return true;
}

bool GoldenWorks::GetTableId(int handle,GOLDEN_TABLE &GlTable)
{

	int nRet = 0;
	nRet =  gob_get_table_property_by_name(handle, &GlTable);
	if ( nRet != 0)
			//cout << "获得表时出错! [" << nRet<<"]"<<endl;
		UarcRmemdServer::GetLogger().error("%d时刻获得表时出错! [ %d  ]\n",(int)time(NULL),(int)nRet);
	return true;
}
bool GoldenWorks::Put_snapshots( PutDataStr CreatPtStrt)
{

	int nRet = 0;
		nRet = gos_put_snapshots(CreatPtStrt.handle,
									&CreatPtStrt.count,
									&CreatPtStrt.ids,
									&CreatPtStrt.datetimes,
									&CreatPtStrt.ms,
									&CreatPtStrt.values,
									&CreatPtStrt.states,
									&CreatPtStrt.qualities,
									&CreatPtStrt.errors);
		if ( nRet != 0)
					//cout << "存储数据出错! [" << nRet<<"]"<<endl;
			UarcRmemdServer::GetLogger().error("%d时刻存储数据出错! [ %d  ]\n",(int)time(NULL),(int)nRet);
    return true;
}
golden_int32 GoldenWorks::getHandle() const
{

    return _handle;
}

void GoldenWorks::setHandle(golden_int32 handle)
{
    this->_handle = handle;
}


//存储数据
bool GoldenWorks::StoreData(RealTimeDataStruct  StoreDataStru)
{
	sw_Store_To_RunTime.restart();//..........................................////////////////////////////////////////////
	//建立连接
	ConnectStr Cndb;
	Cndb.hostname = _Info.IPAddr.c_str();
	Cndb.port = _Info.port;
//	UarcRmemdServer::GetLogger().information("与实时库建立连接\n");
	connect_DB(Cndb);

	//登录
	LoginStr lgdb;
	lgdb.handle = this->getHandle();
	lgdb.password = _Info.Password.c_str();
	lgdb.user = _Info.UserName.c_str();
	//UarcRmemdServer::GetLogger().information("登录实时库\n");
	Login_DB(lgdb);

	//获取表ID
	GOLDEN_TABLE table;
	//table.name = TableName;
	sprintf(table.name, "%s", _Info.table.c_str());

	GetTableId(this->getHandle(), table);

	//查找标识点，如果没有就创建标识点
	FpStr fdpt;

	fdpt.handle = this->getHandle();
	fdpt.count = 1;

	sprintf(fdpt.p, "%s.%s", table.name, StoreDataStru.Tagname.c_str());
	fdpt.q = fdpt.p;

	Find_points(fdpt);
	if (fdpt.ids == 0)
	{


		InStComPStr CreatePoint;
		memset(&CreatePoint.base,0,sizeof(CreatePoint.base));
		strcpy(CreatePoint.base.tag, StoreDataStru.Tagname.c_str());
		strcpy(CreatePoint.base.table_dot_tag , fdpt.p);
		CreatePoint.handle = this->getHandle();
		CreatePoint.base.compress = 0;
		CreatePoint.base.table = table.id;
		CreatePoint.base.type = 10;
		CreatePoint.base.archive = 4;
		CreatePoint.calc = NULL;
		CreatePoint.scan = NULL;
		Insert_point(CreatePoint);
		//存储数据
		PutDataStr storeinfo;
		storeinfo.handle = this->getHandle();
		storeinfo.datetimes = StoreDataStru.timeStamp;
		storeinfo.count = 1;//单个数据
		storeinfo.ids = CreatePoint.base.id;

		storeinfo.values = StoreDataStru.dataVal;
	//	UarcRmemdServer::GetLogger().information(str);
	//	UarcRmemdServer::GetLogger().information("存储时刻time:%d存储到实时库\n",StoreDataStru.timeStamp);
		Put_snapshots(storeinfo);

	}
	else
	{
		//存储数据
		PutDataStr storeinfo;
		storeinfo.handle = this->getHandle();
		storeinfo.datetimes = StoreDataStru.timeStamp;
		storeinfo.count = 1;//单个数据
		storeinfo.ids = fdpt.ids;
		storeinfo.ms = fdpt.use_ms;
		storeinfo.values = StoreDataStru.dataVal;
	//	UarcRmemdServer::GetLogger().information(fdpt.p);
	//	UarcRmemdServer::GetLogger().information("存储时刻time:%d存储到实时库\n",StoreDataStru.timeStamp);
		Put_snapshots(storeinfo);
	}


	//断开连接

	Disconnect_DB(this->getHandle());

	sw_Store_To_RunTime.stop();//
	if(sw_Store_To_RunTime.elapsed()>=1000)
	{
	UarcRmemdServer::GetLogger().information("============3、实时库用时%d ms............\n",(int)sw_Store_To_RunTime.elapsed()/1000);
	}

	return true;
}



bool GoldenWorks::StoreData(std::vector<RealTimeDataStruct> RealTimeStoreVector)
{
	sw_Store_To_RunTime.start();
	printf("得到的连接信息为：Ip:%s\n",_Info.IPAddr.c_str());
	int VecNum=0;

	VecNum = (int)RealTimeStoreVector.size();
	//-------------------
	//1连接
	ConnectStr Cndb;
	Cndb.hostname = _Info.IPAddr.c_str();
	Cndb.port = _Info.port;
	connect_DB(Cndb);

	//2登录
	LoginStr lgdb;
	lgdb.handle = this->getHandle();
	lgdb.password = _Info.Password.c_str();
	lgdb.user = _Info.UserName.c_str();
	Login_DB(lgdb);


	//3获取表的ID
	GOLDEN_TABLE table;
	//table.name = TableName;
	sprintf(table.name, "%s",_Info.table.c_str());
	GetTableId(lgdb.handle, table);

	//查询/插入标签点/存储available
	std::vector<RealTimeDataStruct> ::iterator it_iter;


	int recordCnt=0;
	for(it_iter = RealTimeStoreVector.begin();it_iter !=RealTimeStoreVector.end();it_iter++)
	{
		std::cout << "***** 第 " << ++recordCnt << " 条任务记录 *****"	<< std::endl;
		std::cout << "Tagname  :" << it_iter->Tagname<< std::endl;
		std::cout << "dataVal  :"<< it_iter->dataVal << std::endl;
		std::cout << "tagId  :"<< it_iter->tagId << std::endl;
		std::cout << "timeStamp  :"<< it_iter->timeStamp << std::endl;
	}
	for(it_iter = RealTimeStoreVector.begin();it_iter !=RealTimeStoreVector.end();it_iter++)
	{
		PutDataStr storeinfo;
		storeinfo.handle = this->getHandle();
		storeinfo.datetimes = it_iter->timeStamp;
		storeinfo.count = 1;//单个数据
		storeinfo.values = it_iter->dataVal;

		//查找标识点，如果没有就创建标识点
		FpStr fdpt;
		fdpt.handle = this->getHandle();
		fdpt.count = 1;


		sprintf(fdpt.p, "%s.%s", table.name,it_iter->Tagname.c_str() );
		fdpt.q = fdpt.p;

		Find_points(fdpt);

		if (fdpt.ids == 0)
		{
			//没有找到该标签点就创建
			cout << "没找到标签点，正在创建该点" << endl;
			char table_dot_tag[80] = { 0 };
			sprintf(table_dot_tag, "%s.%s", table.name,it_iter->Tagname.c_str());

			InStComPStr CreatePoint;
			memset(&CreatePoint.base, 0,sizeof(CreatePoint.base));
			strcpy(CreatePoint.base.tag,it_iter->Tagname.c_str());
			strcpy(CreatePoint.base.table_dot_tag,table_dot_tag);
			CreatePoint.handle = this->getHandle();
			CreatePoint.base.compress = 0;
			CreatePoint.base.table = table.id;
			CreatePoint.base.type = 10;
			CreatePoint.base.archive = 4;
			CreatePoint.calc = NULL;
			CreatePoint.scan = NULL;
			Insert_point(CreatePoint);

			storeinfo.ids = CreatePoint.base.id;
			//存储数据
			Put_snapshots(storeinfo);
		}
		else
		{
			storeinfo.ids = fdpt.ids;
			//存储数据
			Put_snapshots(storeinfo);
		}

	}
	//断开连接
	Disconnect_DB(this->getHandle());
	sw_Store_To_RunTime.stop();
	cout<<"实时库存储用时 "<<(int)sw_Store_To_RunTime.elapsed()/1000<<"ms"<<endl;
		UarcRmemdServer::GetLogger().information("实时库存储用时 %d ms\n",(int)sw_Store_To_RunTime.elapsed()/1000);
		sw_Store_To_RunTime.reset();
return true;
		}
