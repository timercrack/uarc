/*
 * Device.h
 *
 *  Created on: 2014-10-14
 *      Author: root
 */

#ifndef UARC_PROTOCOL_VIEC104_DEVICE_H_
#define UARC_PROTOCOL_VIEC104_DEVICE_H_

#include <Poco/Logger.h>
#include <iostream>
#include <string>
#include <vector>
#include "VIEC104Subsystem.h"

namespace Uarc {

namespace Protocol {

namespace VIEC104 {

using Poco::FastMutex;
using std::string;
using std::vector;

class Control;
class VIEC104Subsystem;
class TableFile;
class Device {
public:

	Device(string identity, int id = -1, string ip = "", int port = -1, string crypt = string(""), VIEC104Subsystem *parent = NULL);
	virtual ~Device();

//	Device& operator = (const Device&);

	bool isVaild(){
		bool vaild_ = false;
		_mutex_vaild.lock();
		vaild_ = _vaild;
		_mutex_vaild.unlock();
		return vaild_;
	}
	void setVaild(bool vaild){
		_mutex_vaild.lock();
		_vaild = vaild;
		_mutex_vaild.unlock();
	}
	bool isListening(){
		bool listening_ = false;
		_mutex_listen.lock();
		listening_ = _listening;
		_mutex_listen.unlock();
		return listening_;
	}
	void setListening(bool listening){
		_mutex_listen.lock();
		_listening = listening;
		_mutex_listen.unlock();
	}
	bool isOpen(){
		bool open_ = false;
		open_ = _open;
		return open_;
	}
	std::string identity(){return _identity;}
	int id() const {return _id;}
	void setId(int id){_id = id;}
	std::string ip() const {return _ip;}
	void setIp(std::string ip){_ip = ip;}
	int port() const {return _port;}
	void setPort(int port){_port = port;}
	int fd() const {return _fd;}
	void setFd(int fd){_fd = fd;}
	VIEC104Subsystem* parent(){return _parent;}
	void setParent(VIEC104Subsystem *parent){_parent = parent;}
	void setCrypt(const string &crypt){_crypt = crypt;}
	const string &getCrypt(){return _crypt;}
	int getCryptLength(){return _crypt.length();}

	void show(const string &tip = "Device") const;

	void handleData();
	int send(const char *data, int length);
	int receive();

	bool send(int sendType, const vector<DataItem> &dataItems);//发送命令
	void report(int reason, vector<DataItem> &dataItems);//上报接收数据

	void open();
	void close(int flag = true);

private:
	void showData(const char *data, int length);
	bool connect();
	int setNonBlocking(int fd);

	void saveItems(vector<DataItem> &dest, const vector<DataItem> &src);
	DataItem *findItem(vector<DataItem> &src, DataItem &di);
	void removeItem(vector<DataItem> &src, DataItem *pDi);

public:
	TableFile *_pIEC104Items;//点表映射文件

private:
	static const int BufLen = 1024 * 16;	// 能接收最大单帧16Kb的报文

	Poco::Logger 	*_pLogger;

	char 			_recvBuf[BufLen];
	int				_recvLen;

	bool _vaild;
	bool _listening;
	bool _open;

	string _identity;//设备唯一标识
	int _id;//设备id
	std::string _ip;//设备ip
	int _port;

	int _fd;//文件描述符

	Control *_control;
	VIEC104Subsystem *_parent;

	FastMutex *_mutex;//设备关闭和打开锁
	FastMutex _mutex_listen;
	FastMutex _mutex_vaild;

	FastMutex _mutex_dataitem;//设备关闭和打开锁
	vector<DataItem> _collectItems;
	vector<DataItem> _zhaoceItems;

	string _crypt;

};

}//namespace VIEC104

}//namespace Protocol

}//namespace Uarc

#endif /* UARC_PROTOCOL_VIEC104_DEVICE_H_ */
