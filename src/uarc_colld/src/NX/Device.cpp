/*
 * Device.cpp
 *
 *  Created on: 2014-11-22
 *      Author: root
 */
#include <sstream>
#include <Poco/Util/Application.h>
#include "Device.h"
#include "Control.h"
#include "Frame.h"

namespace Uarc {

namespace Protocol {

namespace NX {

Device::Device(string identity, int id, string ip, int port, string crypt, NxSubsystem *parent) :
				_pLogger(&dynamic_cast<Application&>( Application::instance() ).logger()),
				_open(false),
				_identity(identity), _id(id), _ip(ip), _port(port), _fd(-1),
				_control(new Control(this)), _parent(parent), _crypt(crypt) {
	// TODO 自动生成的构造函数存根
	_thread_count = 0;
	_recvLen = 0;

}

Device::~Device() {
	// TODO 自动生成的析构函数存根
	if (_control != NULL)
	{
		delete _control;
		_control = NULL;
	}
}

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

void *funForDataHandleThread(void *param)
{
	pthread_detach(pthread_self());
	Device *device = (Device *)param;
	device->_mutex_data_handle.lock();

	int done = device->receive();
	if (done == -1)
	{
		device->_pLogger->error("read client data error.");
		device->close();
		if (device->_thread_count <= 0)
		{
			device->_mutex_data_handle.unlock();
			return NULL;
		}
	} else if (done == -2) {
		device->_pLogger->error("device[%s] Closed connection\n", device->identity());
		device->close();
		if (device->_thread_count <= 0)
		{
			device->_mutex_data_handle.unlock();
			return NULL;
		}
	} else {
		if (device->_thread_count <= 0)
		{
			device->_mutex_data_handle.unlock();
			return NULL;
		}
		device->handleData();
		if (device->_tmp && device->bufLen() == 0)
		{
			g_pNxSubsystem->handleTmpDevice(device);
		}
	}

	device->_thread_count--;
	device->_mutex_data_handle.unlock();
	return NULL;
}

void Device::startDataHandleThread()
{
	if (_thread_count >= 0)
	{
		_thread_count++;
		pthread_create(&tid, NULL, funForDataHandleThread, this);
	}
}

void Device::open()
{
	FastMutex::ScopedLock lock(_mutex_open);
	if (_fd != -1)
	{
		_recvLen = 0;
		_thread_count = 0;
		memset(_recvBuf, 0, sizeof(_recvBuf));
		_open = true;
		_control->start();
		show("open Device:");
	}
}

void Device::close()
{
	FastMutex::ScopedLock lock(_mutex_open);
	_recvLen = 0;
	memset(_recvBuf, 0, sizeof(_recvBuf));
	_thread_count = -1;
	_control->stop();
	if (_tmp)
	{
		_parent->closeTmpDevice(this);
	}else {
		_parent->closeDevice(this);
	}
	_open = false;
	_fd = -1;
}

void Device::showData(const char *data, int length)
{
	for (int i = 0; i < length; ++i)
	{
//		_pLogger->information("%x ", _recvBuf[i]);
		printf("%02x ", (unsigned char)data[i]);
	}
	printf("\n");
}

void Device::startConnectTimer()
{
	_control->timerStart(_control->_t0, _control->_t0_value);
}
void Device::stopConnectTimer()
{
	_control->timerStop(_control->_t0);
}

void Device::setAreaCode(int areaCode)
{
	_control->_areaCode = areaCode;
}

int Device::getAreaCode()
{
	return _control->_areaCode;
}

void Device::setTermAddr(int termAddr)
{
	_control->_termAddr = termAddr;
}

int Device::getTermAddr()
{
	return _control->_termAddr;
}

void Device::setMasterAddr(int masterAddr)
{
	_control->_masterAddr = masterAddr;
}

int Device::getMasterAddr()
{
	return _control->_masterAddr;
}

void Device::setPseq(int pseq)
{
	_control->_recvPseq = pseq;
}

int Device::getPseq()
{
	return _control->_recvPseq;
}

void Device::setRseq(int rseq)
{
	_control->_recvRseq = rseq;
}

int Device::getRseq()
{
	return _control->_recvRseq;
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

void Device::handleData()
{
	_control->splitFrame(_recvBuf, _recvLen);
}

void Device::sendMeterConfig(CRtDbMs *rd)
{
	string settings_config = "HS:SETTINGS:CONFIG";
	vector<DataItem> dataItems;
	int count = 0;
	string key, value;
	key = "Count";
	if (!rd->getConfig(settings_config, key, value))
	{
		_pLogger->information("getConfig [%s->%s] failed.", settings_config, key);
		return ;
	}
	count = atoi(value.c_str());

	for (int i = 0; i < count; i++)
	{
		ostringstream oss;
		oss << "Name" << i+1;
		key = oss.str();

		if (!rd->getConfig(settings_config, key, value))
		{
			_pLogger->information("getConfig [%s->%s] failed.", settings_config, key);
			return ;
		}

		string name = value;
		key = "Pn";
		if (!rd->getConfig(name, key, value))
		{
			_pLogger->information("getConfig [%s->%s] failed.", name, key);
			return ;
		}
		int pn = atoi(value.c_str());

		key = "all";
		if (!rd->getConfig(name, key, value))
		{
			_pLogger->information("getConfig [%s->%s] failed.", name, key);
			return ;
		}
		string allItems = value;

		int postion = 0;
		int index = allItems.find_first_of("-", postion);
		for (int j = 0; ; j++)
		{
			DataItem dataItem;
			dataItem.termId = pn;
			if (index == (int)string::npos)//没有找到‘-’，获得最后的ItemID
			{
				key = allItems.substr(postion, allItems.length()-postion+1);
			}else {
				key = allItems.substr(postion, index-postion);
			}
			postion = index+1;

			if (!rd->getConfig(name, key, value))
			{
				_pLogger->information("getConfig [%s->%s] failed.", name, key);
				return ;
			}
			sscanf(key.c_str(), "%X", &dataItem.itemId);
			long int val = atol(value.c_str());
			dataItem.value = val;
			dataItems.push_back(dataItem);

			if (index == (int)string::npos)//上次未找到
			{
				break;
			}
			index = allItems.find_first_of("-", postion);
		}


	}

	send(4, dataItems);
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
	//sendType:0--首次采集，1--采集，2--召测，3--控制(遥控/遥调)，4--配置；
	switch (sendType)
	{
	case 0:
//		_control->execVdata();
//		saveItems(_collectItems, dataItems);
//		_control->sendCallAll();
		return true;
	case 1:
//		saveItems(_collectItems, dataItems);
		_control->sendGetRealTimeData(const_cast< vector<DataItem>& >(dataItems));
		return true;
	case 2:
		saveItems(_zhaoceItems, dataItems);
		_control->sendGetRealTimeData(const_cast< vector<DataItem>& >(dataItems));
//		_control->sendCallSingle(dataItems[0]);
		return true;
	case 3:
//		_control->sendCmd(dataItems);
		return true;
	case 4:
		_control->sendMeterConnectConfig(const_cast< vector<DataItem>& >(dataItems));
		return true;
	default:
		return false;
	}
}

void Device::report(vector<DataItem> &dataItems)
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

}//namespace NX

}//namespace Protocol

}//namespace Uarc
