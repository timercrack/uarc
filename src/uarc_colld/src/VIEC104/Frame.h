/*
 * Frame.h
 *
 *  Created on: 2014-10-15
 *      Author: root
 */

#ifndef UARC_PROTOCOL_VIEC104_FRAME_H_
#define UARC_PROTOCOL_VIEC104_FRAME_H_

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <Poco/DateTime.h>
#include <Poco/LocalDateTime.h>
#include <Poco/Timer.h>
#include "global_104.h"
#include "VIEC104Subsystem.h"

namespace Uarc {

namespace Protocol {

namespace VIEC104 {

using Poco::DateTime;
using Poco::LocalDateTime;
using Poco::Timer;
using Poco::TimerCallback;
using std::string;
using std::vector;
using std::map;

class Control;
class TableFile;
class Frame {
public:
	Frame(const char *pFrame, int length, Control *parent);
	virtual ~Frame();

	void timerStart(Timer &timer, int time);
	void timerStop(Timer &timer);
	void timerRestart(Timer &timer);

	static void crypt(char *data, int length, const char *key, int keyLen);
	//获得帧长度
	static int getLength(const char *pFrame, int length);
	//获得帧格式I S U
	static int getFormat(const char *pFrame, int length);
	//获得帧类型
	static int getType(const char *pFrame, int length);
	//获得传输原因
	static int getReason(const char *pFrame, int length);

	static int getICount(const char *pFrame, int length);
	static int getICountReq(const char *pFrame, int length);

	/////////I帧
	static int buildCallAll(stFrame &frame, int sendICount, int recvICount);
	static int buildCallSingle(stFrame &frame,
			int sendICount, int recvICount, DataItem &dataItem, TableFile *pIEC104Items);
	static int buildTimeSync(stFrame &frame, int sendICount, int recvICount);
	static int buildSelectCmd(stFrame &frame,
			int sendICount, int recvICount, DataItem &dataItem, TableFile *pIEC104Items);
	static int buildExecCmd(stFrame &frame,
			int sendICount, int recvICount, DataItem &dataItem, TableFile *pIEC104Items);
	static int buildUndoCmd(stFrame &frame,
			int sendICount, int recvICount, DataItem &dataItem, TableFile *pIEC104Items);

	/////////S帧
	static int buildFrameS(stFrame &frame, int recvICount);

	//////////U帧
	static int buildLinkTest(stFrame &frame);
	static int buildLinkTestReq(stFrame &frame);
	static int buildActivation(stFrame &frame);
	static int buildActivationReq(stFrame &frame);
	static int buildStop(stFrame &frame);
	static int buildStopReq(stFrame &frame);

	static void Decode(vector<DataItem> &dataItems,
			const char *pFrame, int length, TableFile *pIEC104Items);
	static void DecodeCmd(DataItem &dataItem, int &status,
			const char *pFrame, int length, TableFile *pIEC104Items);

	static bool getTypeByAddr(int &type, int addr, TableFile *pIEC104Items);
	static bool getAddrByTermItem(int &addr, int termId, int itemId, TableFile *pIEC104Items);
	static bool getTermItemByAddr(int &termId, int &itemId, int addr, TableFile *pIEC104Items);

	static bool getResultByValue(map<int, int> &value, map<int, int> &result, TableFile *pIEC104Items);

private:

	static int getReasonI(const char *pFrame, int length);
	static int getReasonS(const char *pFrame, int length);
	static int getReasonU(const char *pFrame, int length);

	static int getTypeI(const char *pFrame, int length);
	static int getTypeS(const char *pFrame, int length);
	static int getTypeU(const char *pFrame, int length);

	static int buildCSCNA1(int buildType, char *data, DataItem &dataItem);//单点命令
	static int buildCSCTA1(int buildType, char *data, DataItem &dataItem);//带长时标，单点命令
	static int buildCDCNA1(int buildType, char *data, DataItem &dataItem);//双点命令
	static int buildCDCTA1(int buildType, char *data, DataItem &dataItem);//带长时标，双点命令
	static int buildCSENA1(int buildType, char *data, DataItem &dataItem);//归一化设点命令
	static int buildCSETA1(int buildType, char *data, DataItem &dataItem);//带长时标，归一化设点命令
	static int buildCSENB1(int buildType, char *data, DataItem &dataItem);//标度化设点命令
	static int buildCSETB1(int buildType, char *data, DataItem &dataItem);//带长时标，标度化设点命令
	static int buildCSENC1(int buildType, char *data, DataItem &dataItem);//短浮点数设点命令
	static int buildCSETC1(int buildType, char *data, DataItem &dataItem);//带长时标，短浮点数设点命令

	static void DecodeCSCNA1(DataItem &dataItem, int &status,
			const char *pFrame, int length, TableFile *pIEC104Items);//单点命令
	static void DecodeCSCTA1(DataItem &dataItem, int &status,
			const char *pFrame, int length, TableFile *pIEC104Items);//带长时标，单点命令
	static void DecodeCDCNA1(DataItem &dataItem, int &status,
			const char *pFrame, int length, TableFile *pIEC104Items);//双点命令
	static void DecodeCDCTA1(DataItem &dataItem, int &status,
			const char *pFrame, int length, TableFile *pIEC104Items);//带长时标，双点命令
	static void DecodeCSENA1(DataItem &dataItem, int &status,
			const char *pFrame, int length, TableFile *pIEC104Items);//归一化设点命令
	static void DecodeCSETA1(DataItem &dataItem, int &status,
			const char *pFrame, int length, TableFile *pIEC104Items);//带长时标，归一化设点命令
	static void DecodeCSENB1(DataItem &dataItem, int &status,
			const char *pFrame, int length, TableFile *pIEC104Items);//标度化设点命令
	static void DecodeCSETB1(DataItem &dataItem, int &status,
			const char *pFrame, int length, TableFile *pIEC104Items);//带长时标，标度化设点命令
	static void DecodeCSENC1(DataItem &dataItem, int &status,
			const char *pFrame, int length, TableFile *pIEC104Items);//短浮点数设点命令
	static void DecodeCSETC1(DataItem &dataItem, int &status,
			const char *pFrame, int length, TableFile *pIEC104Items);//带长时标，短浮点数设点命令

	static void DecodeMSPNA1(vector<DataItem> &dataItems,
			const char* pFrame, int length, TableFile *pIEC104Items);//解析单点信息
	static void DecodeMDPNA1(vector<DataItem> &dataItems,
			const char* pFrame, int length, TableFile *pIEC104Items);//解析双点信息
	static void DecodeMMENA1(vector<DataItem> &dataItems,
			const char* pFrame, int length, TableFile *pIEC104Items);//解析测量值，规一化值，带品质描述
	static void DecodeMMENB1(vector<DataItem> &dataItems,
			const char* pFrame, int length, TableFile *pIEC104Items);//解析测量值，标度化值
	static void DecodeMMENC1(vector<DataItem> &dataItems,
			const char* pFrame, int length, TableFile *pIEC104Items);//解析测量值，短浮点数
	static void DecodeMMEND1(vector<DataItem> &dataItems,
			const char* pFrame, int length, TableFile *pIEC104Items);//解析测量值，规一化值，不带品质描述
	static void DecodeMITNA1(vector<DataItem> &dataItems,
			const char* pFrame, int length, TableFile *pIEC104Items);//解析累计量
	static void DecodeMSPTB1(vector<DataItem> &dataItems,
			const char* pFrame, int length, TableFile *pIEC104Items);//解析单点信息，带长时标
	static void DecodeMDPTB1(vector<DataItem> &dataItems,
			const char* pFrame, int length, TableFile *pIEC104Items);//解析双点信息，带长时标
	static void DecodeMMETF1(vector<DataItem> &dataItems,
			const char* pFrame, int length, TableFile *pIEC104Items);//解析测量值，短浮点数，带长时标
	static void DecodeMEPTD1(vector<DataItem> &dataItems,
				const char* pFrame, int length, TableFile *pIEC104Items);//带长时标的继电保护装置事件

public:
	stFrame _frame;
	int _length;

	int _t0_value;
	int _t1_value;
	int _t2_value;
	int _t3_value;

	Timer _t0;
	Timer _t1;
	Timer _t2;
	Timer _t3;

	TimerCallback<Control> *_callback_t0;
	TimerCallback<Control> *_callback_t1;
	TimerCallback<Control> *_callback_t2;
	TimerCallback<Control> *_callback_t3;

};

}//namespace VIEC104

}//namespace Protocol

}//namespace Uarc

#endif /* UARC_PROTOCOL_VIEC104_FRAME_H_ */
