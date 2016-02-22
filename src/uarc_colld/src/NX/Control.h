/*
 * Control.h
 *
 *  Created on: 2015-1-9
 *      Author: root
 */

#ifndef UARC_PROTOCOL_NX_CONTROL_H_
#define UARC_PROTOCOL_NX_CONTROL_H_

#include <string>
#include <vector>
#include <Poco/Timer.h>
#include <Poco/Logger.h>
#include <Poco/Mutex.h>
#include "NxSubsystem.h"

namespace Uarc {

namespace Protocol {

namespace NX {

using Poco::Logger;
using Poco::FastMutex;
using Poco::Timer;
using Poco::TimerCallback;
using std::string;
using std::vector;

class Device;
class Frame;
class Control {
public:
	Control(Device *device);
	virtual ~Control();

	void show(const string &tip = "Control") const;
	void showData(const char *data, int length);

	void onTimer0(Timer &timer);
	void onTimerClose(Timer &timer);
//	void onTimer1(Timer &timer);
//	void onTimer2(Timer &timer);
//	void onTimer3(Timer &timer);

	void start();
	void stop();

	void splitFrame(char *data, int length);

	friend class Device;
	friend class Frame;

private:
	Control();

	void timerStart(Timer &timer, int time);
	void timerStop(Timer &timer);
	void timerRestart(Timer &timer);

	void handleData(char *data, int length);
	void handleFixedFrame(Frame &frame);
	void handleVerFrame(Frame &frame);

	bool sendFrame(const char *data, int length);
	void sendConnectReq();
	void sendHeartbeatReq();
	void sendGetRealTimeData(vector<DataItem> &dataItems);
	void sendMeterConnectConfig(vector<DataItem> &dataItems);

private:
	int _areaCode;
	int _termAddr;
	int _masterAddr;

	Logger *_pLogger;

	bool _stop;
	Device *_pDevice;

	int _recvPseq;
	int _recvRseq;

	int _sendPseq;
	int _sendRseq;

	int _t0_value;
	int _t_close_value;
//	int _t1_value;
//	int _t2_value;
//	int _t3_value;

	Timer _t0;
	Timer _t_close;
//	Timer _t1;
//	Timer _t2;
//	Timer _t3;

	TimerCallback<Control> *_callback_t0;
	TimerCallback<Control> *_callback_close;
//	TimerCallback<Control> *_callback_t1;
//	TimerCallback<Control> *_callback_t2;
//	TimerCallback<Control> *_callback_t3;

};

}//namespace NX

}//namespace Protocol

}//namespace Uarc

#endif /* UARC_PROTOCOL_NX_CONTROL_H_ */
