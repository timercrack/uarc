/*
 * IEC104Subsystem.h
 *
 *  Created on: 2014-10-14
 *      Author: root
 */

#ifndef UARC_PROTOCCOL_VIEC104_VIEC104Subsystem_H_
#define UARC_PROTOCCOL_VIEC104_VIEC104Subsystem_H_

#include <Poco/Util/Subsystem.h>
#include <Poco/RunnableAdapter.h>
#include <Poco/Runnable.h>
#include <Poco/Thread.h>
#include <Poco/Mutex.h>
#include <Poco/Logger.h>
#include <Poco/Timer.h>
#include <iostream>
#include "rdbms.h"
#include "../CommStruct.h"

class CRtDbMs;
namespace Uarc {

namespace Protocol {

namespace VIEC104 {

using Poco::Util::Application;
using Poco::FastMutex;
using Poco::Thread;
using Poco::RunnableAdapter;
using Poco::Timer;
using Poco::TimerCallback;
using std::string;
using std::vector;
using ::CRtDbMs;

class Device;
struct stOpenDevice
{
	stOpenDevice(Thread *_thread = NULL, Device *_device = NULL,
			FastMutex *_mutex = NULL, int _efd = -1, CRtDbMs *_rtdbms = NULL):
		thread(_thread), device(_device), mutex(_mutex), efd(_efd), rtdbms(_rtdbms){}
	Thread *thread;
	Device *device;
	FastMutex *mutex;
	int efd;
	CRtDbMs *rtdbms;
};

class VIEC104Subsystem: public Poco::Util::Subsystem, public Poco::Runnable {
public:
	VIEC104Subsystem();
	virtual ~VIEC104Subsystem();

	virtual void run();
			/// Do whatever the thread needs to do. Must
			/// be overridden by subclasses.
	void start();//启动子线程
	void stop();//停止子线程

	bool setReceiveCallBack(Receive_F receive){_receive = receive;return true;}
	//sendType:1--采集，2--召测，3--控制；  1.采集不处理
	bool send(int deviceId, int sendType, const vector<DataItem> &dataItems);
	bool send(string identity, int sendType, const vector<DataItem> &dataItems);

	friend bool freshDeviceInfo(const stDeviceInfo& deviceInfo);
	friend void openDevice(void *param);
	friend class Device;

protected:
	virtual const char* name() const;
		/// Returns the name of the subsystem.
		/// Must be implemented by subclasses.

	virtual void initialize(Application& self);
		/// Initializes the subsystem.

	virtual void uninitialize();
		/// Uninitializes the subsystem.

private:

	int initServerSocket();
	int setNonBlocking(int fd);

	void removeSock(int fd);
	void handleData(int fd);
	int sendData(int fd, const char* buf, int len);

	int initDevice();

	void addDevice(Device *device);
	void addDeviceNLock(Device *device);

	void removeDevice(Device *device);
	void removeDeviceNLock(Device *device);

	void closeDevice(Device *device);
	void closeDeviceNLock(Device *device);

	Device *findDeviceNLock(Device *device);
	Device *findDeviceNLock(string identity = "", int id = -1, int fd = -1, string ip = "", int port = -1);

	Device *findDevice(Device *device);
	Device *findDevice(string identity = "", int id = -1, int fd = -1, string ip = "", int port = -1);

	void reconnectDevices(Timer &timer);
	void joinReconnectThread(Timer &timer);

public:
	CRtDbMs *_pRtdbms;
	string	_crypt;	//密钥
	Receive_F _receive;//接收回调函数指针

private:
	static const int BufLen = 1024 * 16;	// 能接收最大单帧16Kb的报文

	char 			_recvBuf[BufLen];
	int				_recvLen;

	Poco::Thread 			_devices_callback_thread;
	Poco::FastMutex 		_devices_mutex;
	std::vector<Device*>	_devices;

	Poco::Logger			*_pLogger;
	bool 					_stopped;
	Poco::Thread 			_thread;

	std::string		_port;//not used
	int				_maxEvent;
	int				_timeout;// 毫秒

	Poco::FastMutex 	_epoll_mutex;
	int 				_efd;

	int				_reconnect_time;
	Timer								_timer_reconnect;					//设备重连定时器
	TimerCallback<VIEC104Subsystem> 		*_callback_reconnect;

	Poco::FastMutex 					_threads_reconnect_mutex;
	std::vector<stOpenDevice*>			_threads_reconnect;

	int				_join_time;
	Timer								_timer_join_reconnect_thread;		//回收重连线程定时器
	TimerCallback<VIEC104Subsystem>		*_callback_join_reconnect_thread;

};

}//namespace VIEC104

}//namespace Protocol

}//namespace Uarc

#endif /* UARC_PROTOCCOL_VIEC104_VIEC104Subsystem_H_ */
