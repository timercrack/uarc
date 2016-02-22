/*
 * IEC104Subsystem.cpp
 *
 *  Created on: 2014-10-14
 *      Author: root
 */
#include <stdio.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/ThreadPool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <arpa/inet.h>

#include "IEC104Subsystem.h"
#include "Device.h"
#include "global_104.h"

namespace Uarc {

namespace Protocol {

namespace IEC104 {

static IEC104Subsystem *g_pIEC104Subsystem = NULL;

IEC104Subsystem::IEC104Subsystem() :
		_pRtdbms(NULL), _receive(NULL), _pLogger(&dynamic_cast<Application&>( Application::instance() ).logger()),
		_stopped(true), _thread(),
		_port("2404"), _maxEvent(512), _timeout(250), _efd(-1), _reconnect_time(100),
		_callback_reconnect(new TimerCallback<IEC104Subsystem>(*this, &IEC104Subsystem::reconnectDevices)),
		_join_time(50),
		_callback_join_reconnect_thread(new TimerCallback<IEC104Subsystem>(*this, &IEC104Subsystem::joinReconnectThread)){
	// TODO Auto-generated constructor stub
	if (!g_pIEC104Subsystem)
	{
		g_pIEC104Subsystem = this;
	}else {
		_pLogger->information("double call IEC104Subsystme.");
		::abort();
	}
}

IEC104Subsystem::~IEC104Subsystem() {
// TODO Auto-generated destructor stub
	delete _callback_reconnect;
	delete _callback_join_reconnect_thread;
	g_pIEC104Subsystem = NULL;
}

const char* IEC104Subsystem::name() const {
	return "IEC104Subsystem";
}

void IEC104Subsystem::initialize(Application& self) {
	/// Initializes the subsystem.
	Poco::ThreadPool::defaultPool().addCapacity(26);   //增加线程池的大小
	string ip = self.config().getString("RedisDB.Base.IPAddr", "127.0.0.1");
	int port = self.config().getInt("RedisDB.Base.Port", 6379);
	_crypt = self.config().getString("iec104.crypt", "00000000");
	unsigned char tmp = 0;
	char str[10] = {0};
	strcpy(str, _crypt.c_str());
	for (int i = 0; i < 8; i++)
	{
		if (str[i] == '1')
		{
			tmp |= 1<<(8-i-1);
		}
	}
	str[0] = tmp;
	str[1] = 0;
	_crypt = string(str);
	_pRtdbms = new CRtDbMs(ip, port);
}

void IEC104Subsystem::uninitialize() {
	/// Uninitializes the subsystem.
	delete _pRtdbms;
	_pRtdbms = NULL;
}

void listenDeviceInfo(void *param);

void IEC104Subsystem::run() {

	int res = initDevice();
	if (res == -1)
	{
		_pLogger->critical("init Device failure.", __FILE__, __LINE__);
		::abort();
	}

	int efd = epoll_create1(0);
	if (efd == -1) {
		_pLogger->critical("epoll_create failure.", __FILE__, __LINE__);
		::abort();
	}

	_efd = efd;

	_devices_callback_thread.start(listenDeviceInfo, this);

	Timer timer;
	reconnectDevices(timer);

	_timer_join_reconnect_thread.setStartInterval(_join_time);
	_timer_join_reconnect_thread.setPeriodicInterval(_join_time);
	_timer_join_reconnect_thread.start(*_callback_join_reconnect_thread);

	_timer_reconnect.setStartInterval(_reconnect_time);
	_timer_reconnect.setPeriodicInterval(_reconnect_time);
	_timer_reconnect.start(*_callback_reconnect);

	/* Buffer where events are returned */
	struct epoll_event *events = (epoll_event*) calloc(_maxEvent, sizeof(struct epoll_event));

	/* The event loop */
	while ( ! _stopped ) {
		int n = ::epoll_wait(efd, events, _maxEvent, _timeout);
		for (int i = 0; i < n; i++) {
			if ( ( events[i].events & EPOLLERR )  || ( events[i].events & EPOLLHUP )
					|| ( ! ( events[i].events & EPOLLIN ) ) )
			{
				/* An error has occured on this fd, or the socket is not
							 ready for reading (why were we notified then?) */
				_pLogger->error("socket error, ignore it.");
				/*Device *device = findDevice("", -1, events[i].data.fd);
				if (device)
				{
					device->close();
				}*/
				continue;
			} else {
				/* We have data on the fd waiting to be read. Read and
							 display it. We must read whatever data is available
							 completely, as we are running in edge-triggered mode
							 and won't get a notification again for the same
							 data. */
				Device *device = findDevice("", -1, events[i].data.fd);

				if (device)
				{
					device->startDataHandleThread();
				} else {
					_pLogger->critical("has not device connection on fd %d\n",
							events[i].data.fd);
					continue;
				}
			}
		}//epoll for
	}//while

	_pRtdbms->stop();
	_devices_callback_thread.join();
	_timer_reconnect.stop();
	_timer_join_reconnect_thread.stop();
	while (_devices.size())
	{
		removeDevice(_devices[0]);
	}
	::free(events);
	::close(efd);
	joinReconnectThread(timer);

	_pLogger->information("Protocl 104 Thread quit!", __FILE__, __LINE__);

}

void IEC104Subsystem::start() {
	poco_assert (_stopped);
	_stopped = false;
	_thread.start(*this);
	_pLogger->information("启动IEC104子系统[%s]", std::string(name()));
}

void IEC104Subsystem::stop() {
	if (!_stopped) {
		_stopped = true;
		_thread.join();
	}
	_pLogger->information("停止IEC104子系统[%s]", std::string(name()));
}

bool IEC104Subsystem::send(string identity, int sendType,
		const vector<DataItem> &dataItems)
{
	Device *device = findDevice(identity);
	if (device && !device->isListening() && device->isOpen())
	{
		return device->send(sendType, dataItems);
	}

	return false;
}

bool IEC104Subsystem::send(int deviceId, int sendType,
		const vector<DataItem> &dataItems)
{
	Device *device = findDevice("", deviceId);
	if (device && !device->isListening() && device->isOpen())
	{
		return device->send(sendType, dataItems);
	}

	return false;
}

///////////
int IEC104Subsystem::initServerSocket() {

	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sfd;

	::memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC; /* Return IPv4 and IPv6 choices */
	hints.ai_socktype = SOCK_STREAM; /* We want a TCP socket */
	hints.ai_flags = AI_PASSIVE; /* All interfaces */

	int rst = ::getaddrinfo(NULL, _port.c_str(), &hints, &result);
	if ( rst != 0 ) {
		_pLogger->critical("getaddrinfo failure."
				+std::string( gai_strerror(rst) ), __FILE__, __LINE__);
		return -1;
	}

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = ::socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if ( sfd == -1 ) continue;
		int on = 1;
		if ( ::setsockopt( sfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) ) == -1 ){
			_pLogger->critical("setsockopt failure:"
					+std::string( strerror(errno) ), __FILE__, __LINE__);
			return -1;
		}
		if (  ::bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0 ) {
			/* We managed to bind successfully! */
			break;
		}
		::close(sfd);
	}
	if ( rp == NULL ) {
		_pLogger->critical("bind failure.", __FILE__, __LINE__);
		return -1;
	}

	::freeaddrinfo(result);

	return sfd;
}

int IEC104Subsystem::setNonBlocking(int fd) {
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

inline void IEC104Subsystem::removeSock(int fd)
{
	//	FastMutex::ScopedLock lock(_mutex);
	//	auto it = _mapSock2Conctor.find(fd);
	//	if ( it != _mapSock2Conctor.end() ) {
	//
	//		CommPoint *cp = reinterpret_cast<CommPoint*>(
	//				CRtDbms::GetCpAddByAreaAndAdd( it->second.GetArea(), it->second.GetAddr() ));
	//		if ( cp ) {
	//
	//			// 更新通信节点表，调用SendCollectedData将集中器离线事件发送给fes_datasvr
	//			// 使用SendCollectedData的time,CPID,pointNo字段分别表示发生时间、CPID、状态类型
	//			cp->Socket = 0;
	//			cp->Status = FALG_STATUS_OFFLINE;
	//			cp->LastLeave = time(NULL);
	//			struct CollectedData  data(cp->LastLeave,cp->CPID,cp->Status,FALG_MSG_TYPE_STATUS);
	//			int ret = SendCollectedData(data);
	//			if(!ret){
	//				FesNxServer::GetLogger().error("终端离线状态传给业务处理进程，失败！！");
	//			}else{
	//				FesNxServer::GetLogger().information("终端离线状态传给业务处理进程，成功！！");
	//			}
	//		}
	//		_mapSock2Conctor.erase(it);
	//	}
	::close(fd);
}

// 处理接收到的报文（数据已存入_recvBuf，长度为_recvLen）
void IEC104Subsystem::handleData(int fd) {
	//	for (int i = 0; i < _recvLen; ++i)
	//	{
	//		printf("%x ", _recvBuf[i]);
	//	}
	//	printf("\n");

	//	int areaCode = 0, termAddr = 0;
	//	// 校验帧，得到areaCode和termAddr
	//	ByteDataBuffer recvBuf(_recvBuf, _recvLen);
	//	int sign = recvBuf.readInt8();
	//	fixFrame recv;
	//	if(sign == VARIABLE_FRAME_HEAD) {
	//		printData("收到可变帧",_recvBuf,_recvLen);//打印接收的可变帧
	//		int frameLength = recvBuf.readInt16();
	//		if(frameLength != recvBuf.readInt16()) {
	//			FesNxServer::GetLogger().error("长度域读取错误.");
	//			return;
	//		}
	//		if(frameLength != _recvLen - VAR_FRAME_FIXED_LEN) {
	//			FesNxServer::GetLogger().error("帧长度错误.");
	//			return;
	//		}
	//		sign = recvBuf.readInt8();
	//		if(sign != VARIABLE_FRAME_HEAD) {
	//			FesNxServer::GetLogger().error("帧头读取错误.");
	//			return;
	//		}
	//		uint8_t *checkSumBegin = recvBuf.getTail();
	//		recvBuf.seek(VAR_FRAME_TYPE_POS);
	//		int frameType = recvBuf.readInt8();
	//		if (frameType != NX_PROTOCOL_MASK) {
	//			FesNxServer::GetLogger().error("规约类型读取错误.");
	//			return;
	//		}
	//		recvBuf.seek(frameLength + FIXED_HEAD_LENGTH);
	//		uint8_t *checkSumEnd = recvBuf.getTail();
	//		int checkSum = 0;
	//		for(uint8_t *p = checkSumBegin; p < checkSumEnd; p++){
	//			checkSum += *p;
	//		}
	//		if((checkSum & 0xFF) != (recvBuf.readInt8() & 0xFF)){
	//			FesNxServer::GetLogger().error("帧校验错误.");
	//			return;
	//		}
	//		sign = recvBuf.readInt8();
	//		if(sign != FRAME_TAIL) {
	//			FesNxServer::GetLogger().error("帧结束符错误.");
	//			return;
	//		}
	//
	//		recvBuf.seek(VAR_FRAME_ADDR_POS);
	//		Address addr;
	//		addr.decode(recvBuf);
	//		areaCode = addr.getAreaCode();
	//		termAddr = addr.getTerminalAddress();
	//		recvBuf.seek(0);
	//
	//	} else if (sign == FIX_FRAME_HEAD) {
	//		printData("收到固定帧",_recvBuf,_recvLen);
	//		recvBuf.seek(0);
	//		int rst = recv.decode(recvBuf);
	//		if(rst == 0) {
	//			areaCode = recv.getMeterAddress().getAreaCode();
	//			termAddr = recv.getMeterAddress().getTerminalAddress();
	//			recvBuf.seek(0);
	//		} else {
	//			return;
	//		}
	//
	//	} else {
	//		FesNxServer::GetLogger().error("帧头读取错误.");
	//		return;
	//	}
	//
	//	int sfd = getFD(areaCode, termAddr);
	//	if ( sfd < 0 ) {
	//
	//		// 未在内存表中找到集中器（不属于本前置机服务），从epoll_events中删除
	//		FesNxServer::GetLogger().error("未在内存表中找到集中器.areaCode=%d,termAddr=%d",
	//				areaCode, termAddr);
	//		removeSock(fd);
	//		return;
	//
	//	} else if ( sfd != fd ) {
	//
	//		// 集中器第一次上线，或者重新连接（之前的FD失效），更新内存表
	//		FesNxServer::GetLogger().information("集中器第一次上线.");
	//		setFD(areaCode, termAddr, fd);
	//	}
	//	// 判断帧类型，调用解码函数
	//	if (_recvBuf[0] == FIX_FRAME_HEAD) {
	//
	//		// 固定帧处理
	//		ByteDataBuffer sendBuf(FIX_FRAME_LENGTH);
	//		fixFrame sendFrame(recv);
	//		sendFrame.encode(sendBuf);
	//		sendBuf.seek(0);
	//		FesNxServer::GetLogger().information("发送固定帧.");
	//		sendData(fd, (char*)sendBuf.getTail(), FIX_FRAME_LENGTH);
	//	}
	//	else if (_recvBuf[0] == VARIABLE_FRAME_HEAD) {
	//
	//		// （不需要校验）可变帧处理,通过fd查询g_mapSock2Conctor，取得PSEQ
	//		FrameNx recvFrame;
	//		int rst = recvFrame.decode(recvBuf);
	//		if( rst >= 0) {
	//			CommPoint *cp = reinterpret_cast<CommPoint*>(CRtDbms::GetCpAddByAreaAndAdd(areaCode, termAddr));
	//			recvFrame.setCommPoint(cp);
	//			recvFrame.execute();
	//			if (_recordFrame)
	//			{
	//				int mfns[MAX_FUNCTION_ITEM_SIZE] = {0};
	//				recvFrame.getMfns(mfns);
	//				LogRecvFrame(areaCode, termAddr, _recvBuf, _recvLen,
	//					cp->CPID, recvFrame.getAfn(), recvFrame.getControl().getDir() ? 1 : 0, mfns);
	//			}
	//		} else {
	//
	//			FesNxServer::GetLogger().error("报文解析失败！,返回结果：%d", rst);
	//			return;
	//		}
	//	} else {
	//		return;
	//	}
}

int IEC104Subsystem::sendData(int fd, const char* buf, int len) {
	int nSend = 0, s = 0;
	while (nSend < len) {
		s = ::write(fd, &buf[nSend], len - nSend);
		if (s == -1) {
			_pLogger->error("write failure,fd=%d", fd);
			return -1;
		}
		nSend += s;
	}

	return nSend;
}

int IEC104Subsystem::initDevice()
{
	std::vector<stDeviceInfo> deviceInfos;
#if 1
	static int conCount = 0;
	while (!_pRtdbms->getDeviceInfo(deviceInfos))
	{
		if (conCount++ > 3)
		{
			_pLogger->critical("get device info failure.");
			return -1;
		}
		sleep(2);
	}
#else
	stDeviceInfo deviceInfo;
	deviceInfo.id = 1002;
	::memcpy(deviceInfo.ip, "192.168.25.138", sizeof(deviceInfo.ip));
	deviceInfo.port = 2404;
	deviceInfos.push_back(deviceInfo);

	deviceInfo.id = 1003;
	::memcpy(deviceInfo.ip, "192.168.253.7", sizeof(deviceInfo.ip));
	deviceInfo.port = 2404;
	deviceInfos.push_back(deviceInfo);

	deviceInfo.id = 1004;
	::memcpy(deviceInfo.ip, "192.168.0.26", sizeof(deviceInfo.ip));
	deviceInfo.port = 2404;
	deviceInfos.push_back(deviceInfo);
#endif

	Device *device = NULL;
	int size = deviceInfos.size();
	int num = 0;
	for (num = 0; num < size; ++num)
	{
		if (deviceInfos[num].status != DEVICE_ST_UNAVAILABLE &&//有效
				deviceInfos[num].protocol_type == PROTOCOL_104 &&//104
				deviceInfos[num].link_type == DEVICE_LINK_T_TCPIP)//tcp/ip
		{
			device = new Device(deviceInfos[num].identity, deviceInfos[num].id,
					deviceInfos[num].ip, deviceInfos[num].port, _crypt, this);

			addDevice(device);
		}
	}

	return num;
}

void IEC104Subsystem::addDeviceNLock(Device *device)
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

void IEC104Subsystem::removeDeviceNLock(Device *device)
{
	for (int i = 0; i < (int)_devices.size(); ++i)
	{
		if (_devices[i] == device)
		{
//			_pLogger->information("remove Device:");
//			device->show();
			_devices.erase(_devices.begin()+i);
			device->setVaild(false);
			device->close(false);
			delete device;
			return ;
		}
	}
}

Device *IEC104Subsystem::findDeviceNLock(string identity, int id, int fd, string ip, int port)
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

Device *IEC104Subsystem::findDeviceNLock(Device *device)
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

void IEC104Subsystem::closeDeviceNLock(Device *device)
{
	for (int i = 0; i < (int)_devices.size(); ++i)
	{
		if (_devices[i] == device)
		{
			if (device->fd() != -1)
			{
				struct epoll_event event;
				event.data.fd = device->fd();
				event.events = EPOLLIN | EPOLLET;
				if ( ::epoll_ctl(_efd, EPOLL_CTL_DEL, device->fd(), &event) == -1 ) {
					_pLogger->critical("epoll_ctl delete fd failure.", __FILE__, __LINE__);
				}
				::close(device->fd());
			}
			stDeviceInfo *deviceInfo = new stDeviceInfo(device->identity());

			deviceInfo->id = device->id();
			deviceInfo->status = DEVICE_ST_DISCONN;
			_pRtdbms->updateDeviceInfo(deviceInfo);
			delete deviceInfo;
		}
	}
}

void IEC104Subsystem::addDevice(Device *device)
{
	FastMutex::ScopedLock lock(_devices_mutex);
	addDeviceNLock(device);
}

void IEC104Subsystem::removeDevice(Device *device)
{
	FastMutex::ScopedLock lock(_devices_mutex);
	removeDeviceNLock(device);
}

Device *IEC104Subsystem::findDevice(string identity, int id, int fd, string ip, int port)
{
	FastMutex::ScopedLock lock(_devices_mutex);
	return findDeviceNLock(identity, id, fd, ip, port);
}

Device *IEC104Subsystem::findDevice(Device *device)
{
	FastMutex::ScopedLock lock(_devices_mutex);
	return findDeviceNLock(device);
}

void IEC104Subsystem::closeDevice(Device *device)
{
	FastMutex::ScopedLock lock(_devices_mutex);
	closeDeviceNLock(device);
}

bool freshDeviceInfo(const stDeviceInfo& deviceInfo)
{
	Device *device = NULL;
	if (deviceInfo.status == DEVICE_ST_UNAVAILABLE)	//无效
	{
		device = g_pIEC104Subsystem->findDevice(string(""), deviceInfo.id);
		if (device)
		{
			g_pIEC104Subsystem->removeDevice(device);
		}
	}else if (deviceInfo.status == DEVICE_ST_AVAILABLE)//有效
	{
		if (deviceInfo.protocol_type == PROTOCOL_104 &&//104
				deviceInfo.link_type == DEVICE_LINK_T_TCPIP )//tcp/ip
		{
			device = new Device(deviceInfo.identity, deviceInfo.id,
					deviceInfo.ip, deviceInfo.port, g_pIEC104Subsystem->_crypt, g_pIEC104Subsystem);
			if (device)
			{
				g_pIEC104Subsystem->addDevice(device);
			}
		}
	}

	return true;
}

void listenDeviceInfo(void *param)
{
//	bool setDeviceInfoCallback(RtDbMsCallback_DeviceInfo callback);
	IEC104Subsystem *iec104 = (IEC104Subsystem *)param;
	if (!iec104->_pRtdbms->setDeviceInfoCallback(freshDeviceInfo))
	{
		printf("setDeviceInfoCallback is failed.\n");
		abort();
	}
	iec104->_pRtdbms->start();
}

void addEpollDevice(void *param)
{
	stOpenDevice *p = (stOpenDevice*)param;

	FastMutex::ScopedLock lock(*(p->mutex));

	Device *device = p->device;

	if (device->isOpen())
	{
		struct epoll_event event;
		memset(&event, 0, sizeof(event));
		event.data.fd = device->fd();
		event.events = EPOLLIN | EPOLLET;
		if ( ::epoll_ctl(p->efd, EPOLL_CTL_ADD, device->fd(), &event) == -1 ) {
			g_pIEC104Subsystem->_pLogger->critical("epoll_ctl add fd failure.", __FILE__, __LINE__);
			::abort();
		}

		stDeviceInfo *deviceInfo = new stDeviceInfo(device->identity());

		deviceInfo->id = device->id();
		deviceInfo->status = DEVICE_ST_CONNING;
		p->rtdbms->updateDeviceInfo(deviceInfo);
		delete deviceInfo;
	}
}

void openDevice(void *param)
{
	stOpenDevice *p = (stOpenDevice*)param;
	Device *device = p->device;
	device->show("listening Device:");

	while (device->isVaild() && !device->isOpen())
	{
		device->open();
		usleep(30);
	}
	if (!device->isVaild())
		return ;

	addEpollDevice(param);
	while (device->isVaild())
	{
		if (device->state() == 0)//正在连接
		{
			usleep(100);
			continue;
		}else if (device->state() == 1)//连接成功
		{
			device->setState(0);
			break;
		}else if (device->state() == 2)//连接超时
		{
			device->setState(0);
			break;
		}else if (device->state() == 3){//连接被关闭
			device->setState(0);
			break;
		}else {
			device->setState(0);
			break;
		}
	}
}

void IEC104Subsystem::reconnectDevices(Timer &timer)
{
	_threads_reconnect_mutex.lock();
	_devices_mutex.lock();
	for (int i = 0; i < (int)_devices.size(); ++i)
	{
		if (_devices[i]->isVaild() && !_devices[i]->isOpen() && !_devices[i]->isListening())
		{
			Thread *thread = new Thread();
			stOpenDevice *openDeviceParam = new stOpenDevice(thread, _devices[i], &_epoll_mutex, _efd, _pRtdbms);

			_threads_reconnect.push_back(openDeviceParam);
			_devices[i]->setListening(true);
			thread->start(openDevice, openDeviceParam);
		}
	}
	_devices_mutex.unlock();
	_threads_reconnect_mutex.unlock();
}

void IEC104Subsystem::joinReconnectThread(Timer &timer)
{
	FastMutex::ScopedLock lock(_threads_reconnect_mutex);
	for (int i = 0; i < (int)_threads_reconnect.size(); ++i)
	{
		if (_threads_reconnect[i]->thread->tryJoin(1))
		{
			_pLogger->information("listening %s stop.\n",
					_threads_reconnect[i]->device->identity());
			_threads_reconnect[i]->device->setListening(false);
			delete _threads_reconnect[i]->thread;
			delete _threads_reconnect[i];
			_threads_reconnect.erase(_threads_reconnect.begin()+(i--));
		}
	}
}

}//namespace IEC104

}//namespace Protocol

}//namespace Uarc
