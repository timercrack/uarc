/*
 * NxSubsystem.h
 *
 *  Created on: 2014-11-22
 *      Author: root
 */

#ifndef UARC_PROTOCOL_NX_NXSUBSYSTEM_H_
#define UARC_PROTOCOL_NX_NXSUBSYSTEM_H_

#include <string>
#include <vector>
#include <Poco/Util/Application.h>
#include <Poco/Util/Subsystem.h>
#include <Poco/Runnable.h>
#include <Poco/Logger.h>
#include <Poco/Thread.h>
#include <Poco/Mutex.h>
#include <Poco/Timer.h>
#include "rdbms.h"
#include "../CommStruct.h"

class CRtDbMs;
namespace Uarc {

namespace Protocol {

namespace NX {

using Poco::Util::Application;
using Poco::FastMutex;
using Poco::Thread;
using Poco::Timer;
using Poco::TimerCallback;
//using Poco::RunnableAdapter;
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

class NxSubsystem: public Poco::Util::Subsystem, public Poco::Runnable {
public:
	NxSubsystem();
	virtual ~NxSubsystem();

	virtual void run();
			/// Do whatever the thread needs to do. Must
			/// be overridden by subclasses.
	void start();//启动子线程
	void stop();//停止子线程

	bool setReceiveCallBack(Receive_F receive){_receive = receive;return true;}
	//sendType:1--采集，2--召测，3--控制；  1.采集不处理
	bool send(int deviceId, int sendType, const vector<DataItem> &dataItems);
	bool send(string identity, int sendType, const vector<DataItem> &dataItems);

	void handleTmpDevice(Device *device);

	friend void listenMeterConfig(void *param);
	friend bool sendMeterConfig(CRtDbMs *rd);
	friend bool freshDeviceInfo(const stDeviceInfo& deviceInfo);
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

	int initDevice();

	void addDevice(Device *device);
	void addDeviceNLock(Device *device);
	void removeDeviceNLock(Device *device);
	Device *findDeviceNLock(Device *device);
	Device *findDeviceNLock(string identity = "", int id = -1, int fd = -1, string ip = "", int port = -1);
	void closeDeviceNLock(Device *device);
	void closeDevice(Device *device);

	void removeDevice(Device *device);
	Device *findDevice(Device *device);
	Device *findDevice(string identity = "", int id = -1, int fd = -1, string ip = "", int port = -1);

	//////////
	void addTmpDevice(Device *device);
	void removeTmpDevice(Device *device);
	void closeTmpDevice(Device *device);
	Device *findTmpDevice(Device *device);
	Device *findTmpDevice(string identity = "", int id = -1, int fd = -1, string ip = "", int port = -1);

	void addTmpDeviceNLock(Device *device);
	void removeTmpDeviceNLock(Device *device);
	void closeTmpDeviceNLock(Device *device);
	Device *findTmpDeviceNLock(Device *device);
	Device *findTmpDeviceNLock(string identity = "", int id = -1, int fd = -1, string ip = "", int port = -1);
	
	void collectTmpDevice(Timer &timer);

public:
	CRtDbMs *_pRtdbms;
	CRtDbMs *_pRtdbms1;
	string	_crypt;	//密钥
	Receive_F _receive;//接收回调函数指针

private:
	static const int BufLen = 1024 * 16;	// 能接收最大单帧16Kb的报文

	char 			_recvBuf[BufLen];
	int				_recvLen;

	Poco::Logger			*_pLogger;

	//回收无效临时设备
	int _collect_tmp_device_value;
	Timer _timer_collect_tmp_device;
	TimerCallback<NxSubsystem> *_callback_collect_tmp_device;

	bool 				_stopped;
	Poco::Thread 			_thread;

	std::string		_port;//
	int				_maxEvent;
	int				_timeout;// 毫秒

	Poco::FastMutex 	_epoll_mutex;
	int 				_efd;

	std::vector<stOpenDevice>	_openDevices;
	Poco::Thread				_meter_config_callback_thread;
	Poco::Thread 				_devices_callback_thread;

	Poco::FastMutex 			_devices_mutex;
	std::vector<Device*>		_devices;

	Poco::FastMutex 			_tmp_devices_mutex;
	std::vector<Device*>		_tmp_devices;
	

};

extern NxSubsystem *g_pNxSubsystem;

}//namespace NX

}//namespace Protocol

}//namespace Uarc

#endif /* UARC_PROTOCOL_NX_NXSUBSYSTEM_H_ */
