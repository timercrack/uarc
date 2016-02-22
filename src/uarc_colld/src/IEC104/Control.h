/*
 * Control.h
 *
 *  Created on: 2014-10-15
 *      Author: root
 */

#ifndef UARC_PROTOCOL_IEC104_CONTROL_H_
#define UARC_PROTOCOL_IEC104_CONTROL_H_

#include <Poco/Timer.h>
#include <Poco/Logger.h>
#include <Poco/Mutex.h>
#include <iostream>
#include <string>
#include <vector>
#include "IEC104Subsystem.h"

namespace Uarc {

namespace Protocol {

namespace IEC104 {

using Poco::FastMutex;
using Poco::Timer;
using Poco::TimerCallback;
using std::string;
using std::vector;

class Frame;
class Device;
class Control {
public:

	Control(Device *device);
	virtual ~Control();

	void show(const string &tip = "Control") const;
	void showData(const char *data, int length);

	void start();
	void stop();

	void splitFrame(char *data, int length);

	void sendCmd(const vector<DataItem> &dataItems);
	void sendCallAll();
	void sendCallSingle(const DataItem &dataItem);

	void onTimer0(Timer &timer);
	void onTimer1(Timer &timer);
	void onTimer2(Timer &timer);
	void onTimer3(Timer &timer);

	void onTimerClose(Timer &timer);
	void onTimerCallAll(Timer &timer);
	friend void *funForDataHandleThread(void *param);

private:
	Control();

	void clearSendFrameNLock(int type = 0);//清空发送的帧
	void clearSendFrame(int type = 0);//清空发送的帧
	void addSendFrameNLock(Frame *pFrame);//添加发送帧
	void addSendFrame(Frame *pFrame);//添加发送帧
	void removeSendFrameNLock(Frame *pFrame);//删除发送的帧
	void removeSendFrame(Frame *pFrame);//删除发送的帧
	Frame *findSendFrameNLock(int format, int type, int reason);//查找发送的帧
	Frame *findSendFrame(int format, int type, int reason);//查找发送的帧
	vector<Frame*> &findSendFrameNLock(int number, vector<Frame*> &result);//序列号小于number的帧
	vector<Frame*> &findSendFrame(int number, vector<Frame*> &result);//序列号小于number的帧

	void handleData(char *data, int length);
	void handleFrameI(const char *data, int length);
	void handleFrameS(const char *data, int length);
	void handleFrameU(const char *data, int length);

	void sendSelectCmd(DataItem &dataItem);
	void sendExecCmd(DataItem &dataItem);
	void sendUndoCmd(DataItem &dataItem);
	void receiveCmdReq(const char *data, int length);

	void sendTimeSync();
	void receiveTimeSyncReq();
	void receiveCallAllReq();
	void receiveCallAllData(const char *data, int length);
	void receiveSpont(const char *data, int length);//接收突发信息
	void receiveActCon(const char *data, int length);//接收激活确认
	void receiveActStop(const char *data, int length);//接收激活停止

	void sendSFrame();

	void sendActivation();
	void sendActivationReq();
	void receiveActivationReq();

	void sendLinkTest();
	void sendLinkTestReq();
	void receiveLinkTestReq();

	void sendStop();//not used
	void sendStopReq();//not used
	void receiveStopReq();//not used

	bool sendFrameI(const char *pFrame, int length);
	bool sendFrameS(const char *pFrame, int length);
	bool sendFrameU(const char *pFrame, int length);
	bool sendFrame(const char *pFrame, int length);

	void timerStart(Timer &timer, int time);
	void timerStop(Timer &timer);
	void timerRestart(Timer &timer);

	bool reqSendICount(const char *pFrame, int length);
	bool reqRecvICount(const char *pFrame, int length);

	//相应操作控制处理
	void receiveI(const char *pFrame, int length);
	void sendI(const char *pFrame, int length);
	void receiveS(const char *pFrame, int length);
	void sendS(const char *pFrame, int length);

private:
	static const int BufLen = 1024 * 16;	// 能接收最大单帧16Kb的报文
	
	static const int _ICountMax = 0x7f00;
	Poco::Logger *_pLogger;

	bool _stop;
	Device *_pDevice;

	int _sendICount;
	int _recvICount;

	int _sendICountReq;
	int _recvICountReq;

	int _w;//接收到w个I帧，发送S帧确认
	int _k;//k个I帧未被确认，则断开连接

	FastMutex _mutex_send_receive;
	vector<Frame*> _sendFrameList;

	int _t0_value;
	int _t1_value;
	int _t2_value;
	int _t3_value;
	int _t_close_value;
	int _t_callAll_value;  //总召计时 2015-07-05

	Timer _t0;
	Timer _t1;
	Timer _t2;
	Timer _t3;
	Timer _t_close;
	Timer _t_callAll;   //总召定时器 2015-07-05

	TimerCallback<Control> *_callback_t0;
	TimerCallback<Control> *_callback_t1;
	TimerCallback<Control> *_callback_t2;
	TimerCallback<Control> *_callback_t3;
	TimerCallback<Control> *_callback_close;
	TimerCallback<Control> *_callback_callAll;  //总召回调 2015-07-05
};

}

}

}

#endif /* UARC_PROTOCOL_IEC104_CONTROL_H_ */
