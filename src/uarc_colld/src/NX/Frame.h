/*
 * Frame.h
 *
 *  Created on: 2014-12-26
 *      Author: root
 */

#ifndef UARC_PROTOCOL_NX_FRAME_H_
#define UARC_PROTOCOL_NX_FRAME_H_

#include <iostream>
#include "ParseFrame.h"
#include "NxSubsystem.h"

namespace Uarc {

namespace Protocol {

namespace NX {

using std::string;

class Control;
class Frame {
public:
	Frame(const char *pFrame, int length, Control *parent = NULL);
	virtual ~Frame();

	static void showData(const char *data, int length);

	//获得帧长度
	static int length(const char *pFrame, int length);
	//获得设备唯一标识
	static string identity(const char *pFrame, int length);

	bool isFixedFrame();
	int length();
	string identity();
	int areaCode();
	int termAddr();
	int masterAddr();
	int pseq();
	int rseq();
	int funCode();
	int afn();

	bool getConfirmOrDeny(bool &value);
	bool getValues(vector<DataItem> &dataItems);

	char *data();
	int buildConnectReq(int pseq, int rseq);
	int buildHeartbeat(int pseq, int rseq);
	int buildConfirmReq(int pseq, int rseq);
	int buildRequestRealTimeData(int pseq, int rseq,
			vector<DataItem> &dataItems);//请求实时数据
	int buildMeterConnectConfig(int pseq, int rseq,
			vector<DataItem> &dataItems);//表的连接配置信息

	friend class Control;

private:
	bool setAddress(int areaCode = 0, int termAddr = 0, int masterAddr = 0, bool Group = false);
	bool setControl(int funCode = 0, int Rseq = 0, int Pseq = 0, bool Reserve = false, bool Prm = false, bool Dir = false);
	bool setProtocol(int protocolVersion = 0, int protocolFlag = 0, int protocolReserve = 0);
	bool setAppControl(int afn = 0, bool acd = false, bool tpv = false);
	bool setEventCount(int ec1 = 0, int ec2 = 0);
	bool setPassword(string password = "none");
	bool setTpv(int timeout = 0);

	bool encode();
	bool decode();
	static bool decode(const char *pFrame, int &length, FrameNx &frame);

private:

	static const string _protocol_path;
	static const string _fixedFrame;
	static const string _verFrame;

	Control *_parent;

	FrameNxFSM _fsm;

	uint8_t _data[MAX_FRAME_LENGTH];
	int _length;

	FrameNx _frame;

};

}//namespace NX

}//namespace Protocol

}//namespace Uarc

#endif /* UARC_PROTOCOL_NX_FRAME_H_ */
