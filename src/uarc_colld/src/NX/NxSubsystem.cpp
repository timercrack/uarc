/*
 * NxSubsystem.cpp
 *
 *  Created on: 2014-11-22
 *      Author: root
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <Poco/ThreadPool.h>

#include "NxSubsystem.h"
#include "Device.h"

#include "Control.h"
#include "Frame.h"

namespace Uarc {

namespace Protocol {

namespace NX {

void str_to_hex(string &str, char *fdata, int &length)
{
        int i = 0;

        for (string::size_type index = 0; index < str.size(); index += 3)
        {
                fdata[i++] = strtoul(str.substr(index, 2).c_str(), NULL, 16);
        }

        length = i;
}

//使用需先把handleVerFrame改为公有函数
void test()
{
	Control control(NULL);
	string str = "68 1e 00 1e 00 68 01 81 01 30 00 01 00 00 00 00 21 0d 10 01 01 1d 0a 10 00 00 c0 10 01 01 1d 0b 70 00 00 c0 55 16";
	char data[1024] = {0};
	int length;
	str_to_hex(str, data, length);
	Frame frame(data, length, &control);
	frame.identity();

//	control.handleVerFrame(frame);

	exit(0);
}

}

}

}

namespace Uarc {

namespace Protocol {

namespace NX {

void listenMeterConfig(void *param);
void listenDeviceInfo(void *param);

static string config = "HS:LOADCONFIG:CONFIG";
NxSubsystem *g_pNxSubsystem = NULL;

NxSubsystem::NxSubsystem():
		_pRtdbms(NULL), /*_receive(NULL),*/ _pLogger(&dynamic_cast<Application&>( Application::instance() ).logger()),
		_collect_tmp_device_value(30000),
		_callback_collect_tmp_device(new TimerCallback<NxSubsystem>(*this, &NxSubsystem::collectTmpDevice)),
		_stopped(true), _thread(), _maxEvent(4096), _timeout(250), _efd(-1){
	// TODO 自动生成的构造函数存根
	if (!g_pNxSubsystem)
	{
		g_pNxSubsystem = this;
	}else {
		_pLogger->information("double call NxSubsystem.");
		::abort();
	}
}

NxSubsystem::~NxSubsystem() {
	// TODO 自动生成的析构函数存根
}

const char* NxSubsystem::name() const {
	return "NxSubsystem";
}

void NxSubsystem::initialize(Application& self) {
	/// Initializes the subsystem.
	Poco::ThreadPool::defaultPool().addCapacity(200);   //增加线程池的大小
	string ip = self.config().getString("RedisDB.Base.IPAddr", "127.0.0.1");
	int port = self.config().getInt("RedisDB.Base.Port", 6379);
	_port = self.config().getString("nx.port", "6000");
	_pRtdbms = new CRtDbMs(ip, port);
	_pRtdbms1 = new CRtDbMs(ip, port);
}

void NxSubsystem::uninitialize() {
	/// Uninitializes the subsystem.
	delete _pRtdbms;
	_pRtdbms = NULL;
	delete _pRtdbms1;
	_pRtdbms1 = NULL;
}

void NxSubsystem::run() {

	int res = initDevice();
	if (res == -1)
	{
		_pLogger->critical("init Device failure.", __FILE__, __LINE__);
		::abort();
	}
	//启动线程监听表配置信息
	_meter_config_callback_thread.start(listenMeterConfig, g_pNxSubsystem);
	//启动线程监听设备信息
	_devices_callback_thread.start(listenDeviceInfo, g_pNxSubsystem);
	//启动定时器，回收无效临时设备
	_timer_collect_tmp_device.setStartInterval(_collect_tmp_device_value);
        _timer_collect_tmp_device.setPeriodicInterval(_collect_tmp_device_value);
        _timer_collect_tmp_device.start(*_callback_collect_tmp_device);

	int sfd = initServerSocket();
	if (sfd == -1)
	{
		_pLogger->critical("init server socket failure.", __FILE__, __LINE__);
		abort();
	}
	if ( setNonBlocking(sfd) == -1 )
	{
		_pLogger->critical("setNonBlocking failure.", __FILE__, __LINE__);
		abort();
	}
	if (listen(sfd, SOMAXCONN) == -1) {
		_pLogger->critical("listen failure.", __FILE__, __LINE__);
		abort();
	}

	int efd = epoll_create1(0);
	if (efd == -1) {
		_pLogger->critical("epoll_create failure.", __FILE__, __LINE__);
		::abort();
	}

	_efd = efd;

	struct epoll_event event;
	event.data.fd = sfd;
	event.events = EPOLLIN | EPOLLET;
	if ( epoll_ctl(efd, EPOLL_CTL_ADD, sfd, &event) == -1 ) {
		_pLogger->critical("epoll_ctl add fd failure.", __FILE__, __LINE__);
		abort();
	}

	/* Buffer where events are returned */
	struct epoll_event *events = (epoll_event*) calloc(_maxEvent, sizeof(struct epoll_event));

	/* The event loop */
	while ( ! _stopped ) {
		usleep(1);
		int n = epoll_wait(efd, events, _maxEvent, _timeout);
		for (int i = 0; i < n; i++) {

			if ( ( events[i].events & EPOLLERR )  || ( events[i].events & EPOLLHUP )
					|| ( ! ( events[i].events & EPOLLIN ) ) )
			{
				/* An error has occured on this fd, or the socket is not
					 ready for reading (why were we notified then?) */
				_pLogger->error("client socket error, ignore it.");
//				removeSock(events[i].data.fd);
				continue;

			} else if ( sfd == events[i].data.fd ) {
				/* We have a notification on the listening socket, which
					 means one or more incoming connections. */
				while ( true ) {

					struct sockaddr in_addr;
					char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
					socklen_t in_len = sizeof( in_addr );
					int infd = accept(sfd, &in_addr, &in_len);
					if (infd == -1) {
						if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
							/* We have processed all incoming
								 connections. */
							break;
						} else {
							_pLogger->error("server socket accept failure.");
							break;
						}
					}
					if ( getnameinfo(&in_addr, in_len, hbuf, sizeof hbuf, sbuf,
							sizeof sbuf, NI_NUMERICHOST | NI_NUMERICSERV) == 0 )
					{
						_pLogger->information("accepted on fd:%d "
								"(host=%s, port=%s)", infd, string(hbuf), string(sbuf) );
					}
					/* Make the incoming socket non-blocking and add it to the
						 list of fds to monitor. */
					if ( setNonBlocking(infd) == -1 )
					{
						_pLogger->critical("setNonBlocking failure.", __FILE__, __LINE__);
						abort();
					}

					event.data.fd = infd;
					event.events = EPOLLIN | EPOLLET;
					if ( epoll_ctl(efd, EPOLL_CTL_ADD, infd, &event) == -1 ) {
						_pLogger->critical("epoll_ctl add fd failure.");
						abort();
					}
					string identity;
					stringstream strstream;
					strstream<<string(hbuf)<<":"<<string(sbuf)<<"@"<<infd;
					strstream>>identity;
					Device *device = new Device(identity, -1,
							string(hbuf), atoi(sbuf), _crypt, this);
					device->setFd(infd);
					device->setTmp();
					device->startConnectTimer();

					addTmpDevice(device);

				}
				continue;

			} else {
				/* We have data on the fd waiting to be read. Read and
					 display it. We must read whatever data is available
					 completely, as we are running in edge-triggered mode
					 and won't get a notification again for the same
					 data. */

				Device *device = NULL;

				//已连接设备信息
				device = findDevice("", -1, events[i].data.fd);
				if (device)
				{
					device->startDataHandleThread();
					continue;
				}

				//新设备连接数据处理
				device = findTmpDevice("", -1, events[i].data.fd);
				if (device)
				{
					device->startDataHandleThread();
					continue;
				}
				//未找到的文件描述符
				printf("未找到的文件描述符\n");
				::close(events[i].data.fd);
			}
		}
	}

	_pRtdbms->stop();
	_pRtdbms1->stop();
	_meter_config_callback_thread.join();
	_devices_callback_thread.join();

	::close(sfd);
	while (_devices.size())
	{
		removeDevice(_devices[0]);
	}
	
	_timer_collect_tmp_device.stop();
	Timer timer;
	collectTmpDevice(timer);

	::free(events);
	::close(efd);
}

void NxSubsystem::start() {
	poco_assert (_stopped);
	_stopped = false;
	_thread.start(*this);
	_pLogger->information("启动NX子系统[%s]", std::string(name()));
}

void NxSubsystem::stop() {
	if (!_stopped) {
		_stopped = true;
		_thread.join();
	}
	_pLogger->information("停止NX子系统[%s]", std::string(name()));
}

bool NxSubsystem::send(string identity, int sendType,
		const vector<DataItem> &dataItems)
{
	Device *device = findDevice(identity);
	if (device && device->isOpen())
	{
		device->send(sendType, dataItems);
		return true;
	}

	return false;
}

bool NxSubsystem::send(int deviceId, int sendType,
		const vector<DataItem> &dataItems)
{
	Device *device = findDevice("", deviceId);
	if (device && device->isOpen())
	{
		device->send(sendType, dataItems);
		return true;
	}

	return false;
}

void NxSubsystem::handleTmpDevice(Device *device)
{
	Device *device_ = findDevice(device->identity());
	if (device_)
	{
		if (device_->isOpen())
		{
			device_->close();
			device->close();
		}else {
			device_->setFd(device->fd());

			device->stopConnectTimer();
			device->setFd(-2);

			device_->setIp(device->ip());
			device_->setPort(device->port());

			device_->setAreaCode(device->getAreaCode());
			device_->setTermAddr(device->getTermAddr());
			device_->setMasterAddr(device->getMasterAddr());
			device_->setPseq(device->getPseq());
			device_->setRseq(device->getRseq());
			device_->open();

			stDeviceInfo *deviceInfo = new stDeviceInfo(device_->identity());

			deviceInfo->status = DEVICE_ST_CONNING;
			_pRtdbms->updateDeviceInfo(deviceInfo);
			delete deviceInfo;

			device->setFd(-1);
		}

	}else {
		_pLogger->information("refuse connect on fd %d.", device->fd());

		device->close();
	}
}

int NxSubsystem::initServerSocket()
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sfd;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC; /* Return IPv4 and IPv6 choices */
	hints.ai_socktype = SOCK_STREAM; /* We want a TCP socket */
	hints.ai_flags = AI_PASSIVE; /* All interfaces */

	int rst = getaddrinfo(NULL, _port.c_str(), &hints, &result);
	if ( rst != 0 ) {
		_pLogger->critical("getaddrinfo failure."
				+std::string( gai_strerror(rst) ), __FILE__, __LINE__);
		return -1;
	}
	for (rp = result; rp != NULL; rp = rp->ai_next) {

		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if ( sfd == -1 ) continue;
		int on = 1;
		if ( setsockopt( sfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) ) == -1 ){
			_pLogger->critical("setsockopt failure:"
					+std::string( strerror(errno) ), __FILE__, __LINE__);
			return -1;
		}
		if (  bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0 ) {
			/* We managed to bind successfully! */
			break;
		}
		close(sfd);
	}
	if ( rp == NULL ) {
		_pLogger->critical("bind failure.", __FILE__, __LINE__);
		return -1;
	}
	freeaddrinfo(result);

	return sfd;
}

int NxSubsystem::setNonBlocking(int fd) {

	int flags = ::fcntl(fd, F_GETFL, 0);
	if ( flags == -1 ) {
		_pLogger->critical("F_GETFL failure.", __FILE__, __LINE__);
		return -1;
	}

	flags |= O_NONBLOCK;
	if ( ::fcntl(fd, F_SETFL, flags) == -1 ) {
		_pLogger->critical("F_SETFL failure.", __FILE__, __LINE__);
		return -1;
	}

	return 0;
}

int NxSubsystem::initDevice()
{
	std::vector<stDeviceInfo> deviceInfos;
#if 1
	if (!_pRtdbms->getDeviceInfo(deviceInfos))
	{
		_pLogger->critical("get device info failure.");
		return -1;
	}
#else
	stDeviceInfo deviceInfo;

	deviceInfo.identity = "000";
	deviceInfo.id = 1002;
	::memcpy(deviceInfo.ip, "0.0.0.0", sizeof(deviceInfo.ip));
	deviceInfo.port = 0;
	deviceInfo.status = DEVICE_ST_AVAILABLE;
	deviceInfo.protocol_type = PROTOCOL_NEWENERGYEFFI;
	deviceInfo.link_type = DEVICE_LINK_T_TCPIP;
	deviceInfos.push_back(deviceInfo);

//	deviceInfo.identity = "2";
//	deviceInfo.id = 1003;
//	::memcpy(deviceInfo.ip, "192.168.253.7", sizeof(deviceInfo.ip));
//	deviceInfo.port = 2404;
//	deviceInfo.status = DEVICE_ST_AVAILABLE;
//	deviceInfo.protocol_type = PROTOCOL_NEWENERGYEFFI;
//	deviceInfo.link_type = DEVICE_LINK_T_TCPIP;
//	deviceInfos.push_back(deviceInfo);
//
//	deviceInfo.identity = "3";
//	deviceInfo.id = 1004;
//	::memcpy(deviceInfo.ip, "192.168.0.26", sizeof(deviceInfo.ip));
//	deviceInfo.port = 2404;
//	deviceInfo.status = DEVICE_ST_AVAILABLE;
//	deviceInfo.protocol_type = PROTOCOL_NEWENERGYEFFI;
//	deviceInfo.link_type = DEVICE_LINK_T_TCPIP;
//	deviceInfos.push_back(deviceInfo);
#endif

	Device *device = NULL;
	int size = deviceInfos.size();
	int num = 0;
	for (num = 0; num < size; ++num)
	{
		if (deviceInfos[num].status != DEVICE_ST_UNAVAILABLE &&//未停用
				deviceInfos[num].protocol_type == PROTOCOL_NEWENERGYEFFI &&//新能效
				deviceInfos[num].link_type == DEVICE_LINK_T_TCPIP)//tcp/ip
		{
			device = new Device(deviceInfos[num].identity, deviceInfos[num].id,
					deviceInfos[num].ip, deviceInfos[num].port, _crypt, this);

			addDevice(device);
		}
	}

	return num;
}

void NxSubsystem::addDeviceNLock(Device *device)
{
	Device *dev = NULL;
	if (device)
	{
		dev = findDeviceNLock(device);
		if (dev != NULL)
		{
			return ;
		}else if (!device->identity().empty())
		{
			dev = findDeviceNLock(device->identity());
			if (dev != NULL)
			{
				removeDeviceNLock(dev);
				_devices.push_back(device);
				device->show("update Device Info:");
			}else {
				_devices.push_back(device);
				device->show("add Device:");
			}
		}
	}
}

void NxSubsystem::removeDeviceNLock(Device *device)
{
	for (int i = 0; i < (int)_devices.size(); ++i)
	{
		if (_devices[i] == device)
		{
			device->show("remove Device:");
			_devices.erase(_devices.begin()+i);
//			device->setVaild(false);
//			device->close(false);
			delete device;
			return ;
		}
	}
}

Device *NxSubsystem::findDeviceNLock(string identity, int id, int fd, string ip, int port)
{
	if (identity.empty() && id == -1 && fd == -1 && ip.empty() && port == -1)
	{
		return NULL;
	}
	for (int i = 0; i < (int)_devices.size(); ++i)
	{
		if (!identity.empty() && _devices[i]->identity() != identity)
		{
			continue;
		}
		if (id != -1 && _devices[i]->id() != id)
		{
			continue;
		}
		if (fd != -1 && _devices[i]->fd() != fd)
		{
			continue;
		}
		if (!ip.empty() && _devices[i]->ip() != ip)
		{
			continue;
		}
		if (port != -1 && _devices[i]->port() != port)
		{
			continue;
		}

		return _devices[i];
	}

	return NULL;
}

Device *NxSubsystem::findDeviceNLock(Device *device)
{
	for (int i = 0; i < (int)_devices.size(); ++i)
	{
		if (_devices[i] == device)
		{
//			_pLogger->information("find Device:");
//			_devices[i]->show();
			return _devices[i];
		}
	}

	return NULL;
}

void NxSubsystem::closeDeviceNLock(Device *device)
{
	for (int i = 0; i < (int)_devices.size(); ++i)
	{
		if (_devices[i] == device)
		{
			device->show("close Device:");
			struct epoll_event event;
			event.data.fd = device->fd();
			event.events = EPOLLIN | EPOLLET;
			if ( ::epoll_ctl(_efd, EPOLL_CTL_DEL, device->fd(), &event) == -1 ) {
				_pLogger->critical("epoll_ctl delete fd failure.", __FILE__, __LINE__);
			}
			::close(device->fd());
			stDeviceInfo *deviceInfo = new stDeviceInfo(device->identity());

			deviceInfo->status = DEVICE_ST_DISCONN;
			_pRtdbms->updateDeviceInfo(deviceInfo);
			delete deviceInfo;
		}
	}
}

void NxSubsystem::addDevice(Device *device)
{
	FastMutex::ScopedLock lock(_devices_mutex);
	addDeviceNLock(device);
}

void NxSubsystem::removeDevice(Device *device)
{
	FastMutex::ScopedLock lock(_devices_mutex);
	removeDeviceNLock(device);
}

Device *NxSubsystem::findDevice(string identity, int id, int fd, string ip, int port)
{
	FastMutex::ScopedLock lock(_devices_mutex);
	return findDeviceNLock(identity, id, fd, ip, port);
}

Device *NxSubsystem::findDevice(Device *device)
{
	FastMutex::ScopedLock lock(_devices_mutex);
	return findDeviceNLock(device);
}

void NxSubsystem::closeDevice(Device *device)
{
	FastMutex::ScopedLock lock(_devices_mutex);
	closeDeviceNLock(device);
}

bool sendMeterConfig(CRtDbMs *rd)
{
	static int sign = -2;
	string value;

	if (!rd->getConfig(config, "sign", value))
	{
		printf("getConfig sign failed.\n");
		sign = -1;
		return false;
	}
	int s = atoi(value.c_str());
	if (sign == -2 || sign == s)
	{
		sign = s;
		return true;
	}
	sign = s;

	if (!rd->getConfig(config, "DEVICE", value))
	{
		printf("getConfig DEVICE failed.\n");
		return false;
	}
	int deviceId = atoi(value.c_str());
	Device *device = g_pNxSubsystem->findDevice(string(""), deviceId);
	if (!device)
	{
		printf("find Device%d failed.\n", deviceId);
		return false;
	}
	device->sendMeterConfig(rd);

	return true;
}

void listenMeterConfig(void *param)
{
	NxSubsystem *nx = (NxSubsystem *)param;
	if (!nx->_pRtdbms1->setCallBack("sendMeterConfig", config, sendMeterConfig))
	{
		printf("setCallBack sendMeterConfig is failed.\n");
		abort();
	}
	//初始化sign
	sendMeterConfig(nx->_pRtdbms1);
	nx->_pLogger->information("注册下发表配置回调函数成功!");

	nx->_pRtdbms1->start();
}

bool freshDeviceInfo(const stDeviceInfo& deviceInfo)
{
	Device *device = NULL;
	if (deviceInfo.status == DEVICE_ST_UNAVAILABLE)	//无效
	{
		device = g_pNxSubsystem->findDevice(string(""), deviceInfo.id);
		if (device)
		{
			g_pNxSubsystem->removeDevice(device);
		}
	}else if (deviceInfo.status == DEVICE_ST_AVAILABLE){	//有效
		if (deviceInfo.protocol_type == PROTOCOL_NEWENERGYEFFI &&//新能效
				deviceInfo.link_type == DEVICE_LINK_T_TCPIP)//tcp/ip
		{
			device = new Device(deviceInfo.identity, deviceInfo.id,
					deviceInfo.ip, deviceInfo.port, g_pNxSubsystem->_crypt, g_pNxSubsystem);
			if (device)
			{

				g_pNxSubsystem->addDevice(device);
			}
		}
	}

	return true;
}

void listenDeviceInfo(void *param)
{
//	bool setDeviceInfoCallback(RtDbMsCallback_DeviceInfo callback);
	NxSubsystem *nx = (NxSubsystem *)param;
	if (!nx->_pRtdbms->setDeviceInfoCallback(freshDeviceInfo))
	{
		printf("setDeviceInfoCallback is failed.\n");
		abort();
	}
	nx->_pRtdbms->start();
}

void NxSubsystem::addTmpDevice(Device *device)
{
	FastMutex::ScopedLock lock(_tmp_devices_mutex);
	addTmpDeviceNLock(device);
}

void NxSubsystem::removeTmpDevice(Device *device)
{
	FastMutex::ScopedLock lock(_tmp_devices_mutex);
	removeTmpDeviceNLock(device);
}

void NxSubsystem::closeTmpDevice(Device *device)
{
	FastMutex::ScopedLock lock(_tmp_devices_mutex);
	closeTmpDeviceNLock(device);
}

Device *NxSubsystem::findTmpDevice(Device *device)
{
	FastMutex::ScopedLock lock(_tmp_devices_mutex);
	return findTmpDeviceNLock(device);
}

Device *NxSubsystem::findTmpDevice(string identity, int id, int fd, string ip, int port)
{
	FastMutex::ScopedLock lock(_tmp_devices_mutex);
	return findTmpDeviceNLock(identity, id, fd, ip, port);
}

void NxSubsystem::addTmpDeviceNLock(Device *device)
{
	Device *dev = NULL;
	if (device)
	{
		dev = findTmpDeviceNLock(device);
		if (dev != NULL)
		{
			return ;
		}else if (!device->identity().empty())
		{
			dev = findTmpDeviceNLock(device->identity());
			if (dev != NULL)
			{
				device->show("update tmp Device:");
				removeTmpDeviceNLock(dev);
				_tmp_devices.push_back(device);
			}else {
				device->show("add tmp Device:");
				_tmp_devices.push_back(device);
			}
		}
	}
}

void NxSubsystem::removeTmpDeviceNLock(Device *device)
{
	for (int i = 0; i < (int)_tmp_devices.size(); ++i)
	{
		if (_tmp_devices[i] == device)
		{
			_tmp_devices.erase(_tmp_devices.begin()+i);
			device->show("remove tmp Device:");
			delete device;
			return ;
		}
	}
}

void NxSubsystem::closeTmpDeviceNLock(Device *device)
{
	for (int i = 0; i < (int)_tmp_devices.size(); ++i)
	{
		if (_tmp_devices[i] == device)
		{
			device->show("close tmp Device:");
			struct epoll_event event;
			event.data.fd = device->fd();
			event.events = EPOLLIN | EPOLLET;
			if ( ::epoll_ctl(_efd, EPOLL_CTL_DEL, device->fd(), &event) == -1 ) {
				_pLogger->critical("epoll_ctl delete fd failure.", __FILE__, __LINE__);
			}
			int fd_ = device->fd();
			device->stopConnectTimer();
			::close(fd_);
		}
	}
}

Device *NxSubsystem::findTmpDeviceNLock(Device *device)
{
	for (int i = 0; i < (int)_tmp_devices.size(); ++i)
	{
		if (_tmp_devices[i] == device)
		{
			return _tmp_devices[i];
		}
	}

	return NULL;
}

Device *NxSubsystem::findTmpDeviceNLock(string identity, int id, int fd, string ip, int port)
{
	if (identity.empty() && id == -1 && fd == -1 && ip.empty() && port == -1)
	{
		return NULL;
	}
	for (int i = 0; i < (int)_tmp_devices.size(); ++i)
	{
		if (!identity.empty() && _tmp_devices[i]->identity() != identity)
		{
			continue;
		}
		if (id != -1 && _tmp_devices[i]->id() != id)
		{
			continue;
		}
		if (fd != -1 && _tmp_devices[i]->fd() != fd)
		{
			continue;
		}
		if (!ip.empty() && _tmp_devices[i]->ip() != ip)
		{
			continue;
		}
		if (port != -1 && _tmp_devices[i]->port() != port)
		{
			continue;
		}

		return _tmp_devices[i];
	}

	return NULL;
}

void NxSubsystem::collectTmpDevice(Timer &timer)
{
	FastMutex::ScopedLock lock(_tmp_devices_mutex);
	for (int i = 0; i < (int)_tmp_devices.size(); ++i)
	{
		if (_tmp_devices[i]->fd() == -1)
		{
			removeTmpDeviceNLock(_tmp_devices[i]);
		}
	}
}

}//namespace Uarc

}//namespace Protocol

}//namespace Uarc
