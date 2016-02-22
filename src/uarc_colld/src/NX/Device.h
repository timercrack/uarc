/*
 * Device.h
 *
 *  Created on: 2014-11-22
 *      Author: root
 */

#ifndef UARC_PROTOCOL_NX_DEVICE_H_
#define UARC_PROTOCOL_NX_DEVICE_H_

#include <iostream>
#include <sstream>
#include "STData.h"
#include "NxSubsystem.h"
#include "TableFile.h"

namespace Uarc {

namespace Protocol {

namespace NX {

using std::string;
using std::stringstream;
using Poco::FastMutex;

class NxSubsystem;
class Control;
class TableFile;

class Device {
public:
	Device(string identity, int id = -1,
			string ip = "", int port = -1,
			string crypt = string(""), NxSubsystem *parent = NULL);
	virtual ~Device();

	void setTmp() {
		_tmp = true;
	}
	void startConnectTimer();
	void stopConnectTimer();

	bool isOpen(){return _open;}
	string identity(){return _identity;}
	int id() const {return _id;}
	void setId(int id){_id = id;}
	std::string ip() const {return _ip;}
	void setIp(std::string ip){_ip = ip;}
	int port() const {return _port;}
	void setPort(int port){_port = port;}
	int fd() const {return _fd;}
	void setFd(int fd){_fd = fd;}
	NxSubsystem* parent(){return _parent;}
	void setParent(NxSubsystem *parent){_parent = parent;}
	void setCrypt(const string &crypt){_crypt = crypt;}
	const string &getCrypt(){return _crypt;}
	int getCryptLength(){return _crypt.length();}
	int bufLen(){return _recvLen;}

	void setAreaCode(int areaCode);
	int getAreaCode();
	void setTermAddr(int termAddr);
	int getTermAddr();
	void setMasterAddr(int masterAddr);
	int getMasterAddr();

	void setPseq(int pseq);
	int getPseq();
	void setRseq(int rseq);
	int getRseq();

	void show(const string &tip = "Device") const;
	void showData(const char *data, int length);

	void open();
	void close();

	void startDataHandleThread();
	void handleData();
	int send(const char *data, int length);
	int receive();

	bool send(int sendType, const vector<DataItem> &dataItems);
	void report(vector<DataItem> &dataItems);

	void sendMeterConfig(CRtDbMs *rd);

	friend class Control;
	friend void *funForDataHandleThread(void *param);

private:
	void saveItems(vector<DataItem> &dest, const vector<DataItem> &src);
	DataItem *findItem(vector<DataItem> &src, DataItem &di);
	void removeItem(vector<DataItem> &src, DataItem *pDi);


private:
	static const int BufLen = MAX_FRAME_LENGTH;	// 能接收最大单帧16Kb的报文

	TableFile *_pNxItems;//点表映射文件

	Poco::Logger 	*_pLogger;

	char 			_recvBuf[BufLen];
	int				_recvLen;

	bool _tmp;
	bool _open;
	FastMutex _mutex_open;

	string _identity;//设备唯一标识
	int _id;//设备id

	string _ip;//设备ip
	int _port;

	int _fd;//文件描述符

	Control *_control;

	NxSubsystem *_parent;

	FastMutex _mutex_data_handle;//数据处理锁
	pthread_t tid;
	//Poco::Thread _thread_data_handle;//数据处理线程
	int _thread_count;

	FastMutex _mutex_dataitem;
	vector<DataItem> _collectItems;
	vector<DataItem> _zhaoceItems;

	string _crypt;
};

}//namespace NX

}//namespace Protocol

}//namespace Uarc

#endif /* UARC_PROTOCOL_NX_DEVICE_H_ */
