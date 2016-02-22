/*
 * Device.cpp
 *
 *  Created on: 2014-10-14
 *      Author: root
 */
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <arpa/inet.h>

#include <Poco/Util/Application.h>
#include "Device.h"
#include "VIEC104Subsystem.h"
#include "Control.h"
#include "TableFile.h"

namespace Uarc {

namespace Protocol {

namespace VIEC104 {

using Poco::Util::Application;

Device::Device(string identity, int id, string ip, int port, string crypt, VIEC104Subsystem *parent) :
				_pLogger(&dynamic_cast<Application&>( Application::instance() ).logger()),
				_vaild(true), _listening(false), _open(false),
				_identity(identity), _id(id), _ip(ip), _port(port), _fd(-1),
				_control(new Control(this)), _parent(parent) {
	// TODO Auto-generated constructor stub
	_mutex = new FastMutex();
	char file[255] = {0};
	sprintf(file, "/etc/uarc/protocol-cfg/IEC104/IEC104-%s.conf", _identity.c_str());
	_pIEC104Items = new TableFile(string(file));
	_pIEC104Items->creatRef();
//	_pIEC104Items->show();

}

Device::~Device() {
	// TODO Auto-generated destructor stub
	delete _control;
	delete _mutex;
	delete _pIEC104Items;
}

//Device& Device::operator = (const Device &device)
//{
//	if (this == &device)
//		return *this;
//
//	_open = false;
//	_recvLen = 0;
//	memset(_recvBuf, 0, sizeof(_recvBuf));
//	_id = device.id();
//	_ip = device.ip();
//	_port = device.port();
//	_fd = device.fd();
//
//	return *this;
//}

void Device::show(const string &tip) const
{
	_pLogger->information("==============================================");
	_pLogger->information("%s %s"         "", string(_parent->name()), tip);
	_pLogger->information("==============================================");
	_pLogger->information("=    Device IDENTITY--:%-s        \t"       "=", _identity);
	_pLogger->information("=    Device ID--------:%-6d        \t"      "=", _id);
	_pLogger->information("=    Device IP--------:%s    \t"            "=", _ip);
	_pLogger->information("=    Device PORT------:%-6d        \t"      "=", _port);
	_pLogger->information("=    Device FD--------:%-6d        \t"      "=", _fd);
	_pLogger->information("==============================================");
}

bool Device::connect()
{
	struct sockaddr_in serv_addr;
	int fd = -1;

	if ((fd = ::socket(AF_INET, SOCK_STREAM, 0)) == -1)
		return false;

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(_port);
	serv_addr.sin_addr.s_addr = inet_addr(_ip.c_str());
	bzero(&(serv_addr.sin_zero), 8);
	if (::connect(fd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr)) == -1)
	{
//		printf("\n建立TCP连接失败，errno:%d", errno);
		::close(fd);
		return false;
	}
//	printf("\n建立连接成功");

	if (setNonBlocking(fd) == -1)
	{
		::close(fd);
		return false;
	}
	_fd = fd;

	return true;
}

int Device::setNonBlocking(int fd) {
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

void Device::open()
{
	_mutex->lock();
	if (_ip != "" && _port != -1)
	{
		if (connect())
		{
			_recvLen = 0;
			memset(_recvBuf, 0, sizeof(_recvBuf));
			_open = true;
			sleep(1);
			_control->start();
			show("open Device:");
		}else {
//			_pLogger->information("connect failed to Device:");
//			show();
		}
	}
	_mutex->unlock();
}

void Device::close(int flag)
{
	_mutex->lock();
	if (!_open)
	{
		_mutex->unlock();
		return ;
	}
	if (flag)
		_parent->closeDevice(this);
	else
		_parent->closeDeviceNLock(this);
	_fd = -1;
	_control->stop();
	_recvLen = 0;
	memset(_recvBuf, 0, sizeof(_recvBuf));
	_open = false;
	_collectItems.clear();
	_zhaoceItems.clear();
	show("close Device:");
	_mutex->unlock();
}

void Device::showData(const char *data, int length)
{
	char tmp[BufLen];
	char *tmp_ = tmp;
	for (int i = 0; i < length; ++i)
	{
		sprintf(tmp_, "%02x ", (unsigned char)data[i]);
		tmp_ += 3;
		printf("%02x ", (unsigned char)data[i]);
	}
	printf("\n");
	_pLogger->information("%s ", std::string(tmp));
}

void Device::handleData()
{
	_control->splitFrame(_recvBuf, _recvLen);
}

DataItem *Device::findItem(vector<DataItem> &src, DataItem &di)
{
	FastMutex::ScopedLock lock(_mutex_dataitem);

	for (int i = 0; i < (int)src.size(); ++i)
	{
		if (src[i].termId == di.termId &&
				src[i].itemId == di.itemId)
		{
			return &src[i];
		}
	}
	return NULL;
}

void Device::saveItems(vector<DataItem> &dest, const vector<DataItem> &src)
{
	FastMutex::ScopedLock lock(_mutex_dataitem);
	bool flag = false;

	for (int i = 0; i < (int)src.size(); ++i)
	{
		flag = false;
		for (int j = 0; j < (int)dest.size(); ++j)
		{
			if (src[i].termId <= 0 || src[i].itemId <= 0)//指标有误，不插入
			{
				flag = true;
			}else if (src[i].termId == dest[j].termId &&
					src[i].itemId == dest[j].itemId)
			{
				flag = true;//已存在该指标，不插入
			}
		}
		if (!flag)
		{
			dest.push_back(src[i]);
		}
	}
}

void Device::removeItem(vector<DataItem> &src, DataItem *pDi)
{
	FastMutex::ScopedLock lock(_mutex_dataitem);

	for (int i = 0; i < (int)src.size(); ++i)
	{
		if (&src[i] == pDi)
		{
			src.erase(src.begin()+i);
			return ;
		}
	}
}

bool Device::send(int sendType, const vector<DataItem> &dataItems)
{
	//sendType:0--首次采集，1--采集，2--召测，3--控制(遥控/遥调)；  1.采集不处理
	switch (sendType)
	{
	case 0:
		_control->execVdata();
//		saveItems(_collectItems, dataItems);
		_control->sendCallAll();
		return true;
	case 1:
		return true;
	case 2:
		saveItems(_zhaoceItems, dataItems);
		_control->sendCallSingle(dataItems[0]);
		return true;
	case 3:
		_control->sendCmd(dataItems);
		return true;
	default:
		return false;
	}
}

int Device::send(const char *data, int length)
{
	int nSend = 0, s = 0;
	while (nSend < length) {

		s = write(_fd, &data[nSend], length - nSend);
		if (s == -1) {
			_pLogger->error("write failure,fd=%d", _fd);
			return -1;
		}
		nSend += s;
	}

//	printf("device[%d] send data---:", _id);
//	showData(data, length);
	return nSend;
}

int Device::receive()
{
	int done = 0;
	_recvLen = 0;
	while ( true ) {
		ssize_t count;
		count = read(_fd, _recvBuf + _recvLen, BufLen - _recvLen);
		if (count == -1) {
			/* If errno == EAGAIN, that means we have read all data. */
			if (errno != EAGAIN) {
//				_logger.error("read client data error.");
				done = -1;
			} else {
				done = 1;
			}
			break;
		} else if (count == 0) {
			/* End of file. The remote has closed the
									 connection. */
			done = -2;
			break;
		}
		_recvLen += count;
	}

	if (done < 0) {
		return done;
	}

//	printf("device[%d] receive data:", _id);
//	showData(_recvBuf, _recvLen);
	return _recvLen;
}

void Device::report(int reason, vector<DataItem> &dataItems)
{
//	vector<DataItem> items;
	vector<DataItem> reportItems;
	if (_parent->_receive != NULL)
	{
//		for (int i = 0; i < (int)dataItems.size(); ++i)//复制指标数据
//		{
//			items.push_back(dataItems[i]);
//		}

		for (int i = 0; i < (int)dataItems.size(); ++i)//获得召测指标
		{
			DataItem *pZhaoceDi = findItem(_zhaoceItems, dataItems[i]);
			if (pZhaoceDi)
			{
				removeItem(_zhaoceItems, pZhaoceDi);
				reportItems.push_back(dataItems[i]);
//				items[i].termId = 0;
			}
		}
		if (reportItems.size())//报告召测指标
		{
			_parent->_receive(_id, 2, reportItems);
		}

//		reportItems.clear();
//		for (int i = 0; i < (int)dataItems.size(); ++i)//获得采集指标
//		{
//			DataItem *pCollectDi = findItem(_collectItems, dataItems[i]);
//			if (pCollectDi)
//			{
//				removeItem(_collectItems, pCollectDi);
//				reportItems.push_back(dataItems[i]);
//				items[i].termId = 0;
//			}
//		}
		if (dataItems.size())//报告采集指标
		{
			_pLogger->information("报告采集指标");
			_parent->_receive(_id, 1, dataItems);
		}

//		for (int i = 0; i < (int)items.size();)//获得未用到指标
//		{
//			if (items[i].termId == 0)
//			{
//				items.erase(items.begin()+i);
//			}else {
//				++i;
//			}
//		}
//
//		if (items.size())//报告未用到指标
//		{
//			_parent->_receive(_id, 0, reportItems);
//		}

	}
}

}//namespace VIEC104

}//namespace Protocol

}//namespace Uarc
