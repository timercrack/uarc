/*
 * RealTimeHeader.h
 *
 *  Created on: 2015-2-6
 *      Author: root
 */

#ifndef REALTIMEHEADER_H_
#define REALTIMEHEADER_H_
#include <iostream>
#include "rdbms.h"
using namespace std;

struct RealTimeDataStruct
{
	RealTimeDataStruct(std::string _Tagname = std::string(""), int _timeStamp=0, double _dataVal=0 ,int _tagId=0):
		Tagname(_Tagname),timeStamp(_timeStamp),dataVal(_dataVal),tagId(_tagId){}
	std::string Tagname;
	int timeStamp;
	double dataVal;
	int tagId;
};

struct ConntDBSetting
{
	std::string IPAddr;
	int         port;
	std::string UserName;
	std::string Password;
	std::string table;
	bool NeedStore;
};

struct CollDataStr
{
	CollDataStr(std::string _unitId = std::string(""),int _itemId=0, int _lastStoreTime=0,std::string _timeStamp=std::string(""), double _lastStoreValue=0,std::string _Sql = std::string(""),std::string _OtherSql = std::string("")):
		unitId(_unitId),itemId(_itemId),lastStoreTime(_lastStoreTime), timeStamp(_timeStamp),lastStoreValue(lastStoreValue),Sql(_Sql),OtherSql(_OtherSql){}
	std::string unitId;
	int itemId;
	int lastStoreTime;
	std::string timeStamp;
	double lastStoreValue;
	std::string Sql;
	std::string OtherSql;
};


#endif /* REALTIMEHEADER_H_ */
