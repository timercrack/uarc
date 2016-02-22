/*
 * Control.cpp
 *
 *  Created on: 2015-1-9
 *      Author: root
 */
#include <Poco/Util/Application.h>
#include "Control.h"
#include "Device.h"
#include "Frame.h"

namespace Uarc {

namespace Protocol {

namespace NX {

using Poco::Util::Application;

Control::Control(Device *device) :
		_pLogger(&dynamic_cast<Application&>( Application::instance() ).logger()),
		_stop(true), _pDevice(device), _recvPseq(0), _recvRseq(0), _sendPseq(0), _sendRseq(0),
		_t0_value(120000), _t_close_value(10),/*_t1_value(15000), _t2_value(10000), _t3_value(20000),*/
		_callback_t0(new TimerCallback<Control>(*this, &Control::onTimer0)),
		_callback_close(new TimerCallback<Control>(*this, &Control::onTimerClose))/*,
		_callback_t2(new TimerCallback<Control>(*this, &Control::onTimer2)),
		_callback_t3(new TimerCallback<Control>(*this, &Control::onTimer3))*/{
	// TODO 自动生成的构造函数存根

}

Control::~Control() {
	// TODO 自动生成的析构函数存根
	stop();
	delete _callback_t0;
	_callback_t0 = NULL;
	delete _callback_close;
	_callback_close = NULL;
}

void Control::show(const string &tip) const
{
	_pLogger->information("=========================================");
	_pLogger->information("%s"                                "", tip);
	_pLogger->information("=========================================");

}

void Control::showData(const char *data, int length)
{
	for (int i = 0; i < length; ++i)
	{
		printf("%02x ", (unsigned char)data[i]);
	}
	printf("\n");
}

void Control::timerStart(Timer &timer, int time)
{
	timer.stop();
	timer.setStartInterval(time);
	timer.setPeriodicInterval(time);
	if (&timer == &_t0)
	{
		timer.start(*_callback_t0);
	}
	if (&timer == &_t_close)
	{
		timer.start(*_callback_close);
	}
//	if (&timer == &_t1)
//	{
//		timer.start(*_callback_t1);
//	}
//	if (&timer == &_t2)
//	{
//		timer.start(*_callback_t2);
//	}
//	if (&timer == &_t3)
//	{
//		timer.start(*_callback_t3);
//	}
}

void Control::timerStop(Timer &timer)
{
	timer.restart(0);
}

void Control::timerRestart(Timer &timer)
{
	timer.restart();
}

void Control::onTimer0(Timer &timer)
{
	//链路空闲超时
	timerStart(_t_close, _t_close_value);
}

void Control::onTimerClose(Timer &timer)
{
	_pDevice->close();
	timer.restart(0);
}

void Control::start()
{
	_stop = false;
	//timerStart(_t0, _t0_value);

	//发送连接确认
	sendConnectReq();

//	vector<DataItem> dataItems;
//	DataItem dataItem;
//	dataItem.termId = 2;
//	dataItem.itemId = 0xC651;
//	dataItem.value = 6;
//	dataItem.time = 0;
//	dataItems.push_back(dataItem);
//
//	dataItem.termId = 3;
//	dataItem.itemId = 0xF161;
//	dataItem.value = 4.2;
//	dataItem.time = 0;
//	dataItems.push_back(dataItem);
//
//	dataItem.termId = 3;
//	dataItem.itemId = 0xF162;
//	dataItem.value = 4.3;
//	dataItem.time = 0;
//	dataItems.push_back(dataItem);

//	dataItem.termId = 4;
//	dataItem.itemId = 0xF061;
//	dataItem.value = 4.2;
//	dataItem.time = 0;
//	dataItems.push_back(dataItem);
//
//	dataItem.termId = 4;
//	dataItem.itemId = 0xF062;
//	dataItem.value = 4.3;
//	dataItem.time = 0;
//	dataItems.push_back(dataItem);

//	sendGetRealTimeData(dataItems);
}

void Control::stop()
{
	timerStop(_t0);
	_stop = true;
}

void Control::splitFrame(char *data, int length)
{
	char buffer[length];
	memcpy(buffer, data, length);
//	Frame::crypt(data, length, _pDevice->getCrypt().c_str(), _pDevice->getCryptLength());
	printf("before split device[%s] receive data:", _pDevice->identity().c_str());
	showData(data, length);

	int pos = 0;
	while(!_stop || _pDevice->_tmp)
	{
		if (length < FIXED_FRAME_LENGTH)
			break;
		int len = Frame::length(buffer + pos, length - pos);
		if (len == -2)
		{
			_pDevice->_recvLen = 0;
			timerStart(_t_close, _t_close_value);
			return;
		}
		if (len < FIXED_FRAME_LENGTH)
			break;
		handleData(buffer + pos, len);
		pos += len;
	}

	if (pos != length)
	{
		memcpy(data, buffer+pos, length - pos);
	}
	_pDevice->_recvLen = length - pos;
}

void Control::handleData(char *data, int length)
{
	printf("device[%s] receive data:", _pDevice->identity().c_str());
	showData(data, length);
	Frame frame(data, length, this);

	if (_pDevice->isOpen() && frame.identity() == _pDevice->identity())
	{
		//保存PSEQ，RSEQ,以供回复时使用
		_recvPseq = frame.pseq();
		_recvRseq = frame.rseq();

		if (frame.isFixedFrame())//固定帧
		{
			handleFixedFrame(frame);
		}else {//可变帧
			handleVerFrame(frame);
		}
	}else if (!_pDevice->isOpen())
	{
		if (frame.isFixedFrame() && frame.funCode() == FIXED_FN_CREATE_CONNECT)//建立链路
		{
			//保存PSEQ，RSEQ,以供回复时使用
			_recvPseq = frame.pseq();
			_recvRseq = frame.rseq();

			_areaCode = frame.areaCode();
			_termAddr = frame.termAddr();
			_masterAddr = frame.masterAddr();
			_pDevice->_identity = frame.identity();
		}
	}else {
		_pLogger->information("device[%s] is receive indentity[%s]!", _pDevice->identity(), frame.identity());
		printf("device[%s] is receive indentity[%s]!\n", _pDevice->identity().c_str(), frame.identity().c_str());
	}
}

void Control::handleFixedFrame(Frame &frame)
{
	if (!frame.isFixedFrame())
	{
		printf("error:handleFixedFrame get verFrame!\n");
		return ;
	}

	switch (frame.funCode())
	{
	case FIXED_FN_CREATE_CONNECT:
	{
		//建立链路单独处理，不在此处操作
		printf("建立链路单独处理，不在此处操作\n");
		break;
	}
	case FIXED_FN_DELETE_CONNECT:
	{
		//拆除链路
		break;
	}
	case FIXED_FN_KEEP_CONNECT:
	{
		//链路保持
		sendHeartbeatReq();
		break;
	}
	default:
	{
		printf("unknow fixed frame funCode [%d]\n", frame.funCode());
		break;
	}
	}
}

void Control::handleVerFrame(Frame &frame)
{
	if (frame.isFixedFrame())
	{
		printf("error:hanleVerFrame get fixedFrame!\n");
		return ;
	}

	switch (frame.afn())
	{
	case VER_AFN_CONFIRM_OR_DENY:
	{
		bool value;
		if (!frame.getConfirmOrDeny(value))
		{
			printf("getConfirmOrDeny error!\n");
			return ;
		}
		if (value)
		{
			printf("received Confirm!\n");
		}else {
			printf("received Deny!\n");
		}
		break;
	}
	case VER_AFN_REALTIME_DATA:
	{
		vector<DataItem> dataItems;
		if (!frame.getValues(dataItems))
		{
			printf("getValues error!\n");
			return ;
		}

		_pDevice->report(dataItems);
		break;
	}
	default:
	{
		printf("unknow ver frame afn[%d]\n", frame.afn());
		break;
	}
	}

}

void Control::sendConnectReq()
{
	Frame frame(NULL, 0, this);
	int length = 0;

	_sendRseq = 0;
	length = frame.buildConnectReq(_recvPseq, _sendRseq);
	if (length < 0)
	{
		_pLogger->information("构建连接确认帧失败!");
		printf("构建连接确认帧失败!\n");
		return ;
	}

	if (!sendFrame(frame.data(), length))
	{
		_pLogger->information("发送连接确认失败!");
		printf("发送连接确认失败!\n");
	}
}

void Control::sendHeartbeatReq()
{
	Frame frame(NULL, 0, this);
	int length = 0;

	_sendRseq = 0;
	length = frame.buildConfirmReq(_recvPseq, _sendRseq);
	if (length < 0)
	{
		_pLogger->information("构建心跳确认帧失败!");
		printf("构建心跳确认帧失败!\n");
		return ;
	}

	if (!sendFrame(frame.data(), length))
	{
		_pLogger->information("发送心跳确认失败!");
		printf("发送心跳确认失败!\n");
	}
}

void Control::sendGetRealTimeData(vector<DataItem> &dataItems)
{
	Frame frame(NULL, 0, this);
	int length = 0;

	_sendPseq = (_sendPseq+1)%255;
	_sendRseq = 0;

	length = frame.buildRequestRealTimeData(_sendPseq, _sendRseq, dataItems);
	if (length < 0)
	{
		_pLogger->information("构建请求实时数据帧失败!");
		printf("构建请求实时数据帧失败!\n");
		return ;
	}

	if (!sendFrame(frame.data(), length))
	{
		_pLogger->information("发送请求实时数据帧失败!");
		printf("发送请求实时数据帧失败!\n");
	}
}

void Control::sendMeterConnectConfig(vector<DataItem> &dataItems)
{
	Frame frame(NULL, 0, this);
	int length = 0;

	_sendPseq = (_sendPseq+1)%255;
	_sendRseq = 0;

	length = frame.buildMeterConnectConfig(_sendPseq, _sendRseq, dataItems);
	if (length < 0)
	{
		_pLogger->information("构建表的连接配置帧失败!");
		printf("构建表的连接配置帧失败!\n");
		return ;
	}
	printf("构建表的连接配置帧：");
	showData(frame.data(), length);

	if (!sendFrame(frame.data(), length))
	{
		_pLogger->information("发送表的连接配置帧失败!");
		printf("发送表的连接配置帧失败!\n");
	}
}

bool Control::sendFrame(const char *data, int length)
{
	if (length >= FIXED_FRAME_LENGTH)
	{
		if (_pDevice->send(data, length) < 0)
		{
			timerStart(_t_close, _t_close_value);
			return false;
		}

		printf("device[%s] send data:", _pDevice->identity().c_str());
		showData(data, length);
		return true;
	}
	return false;
}

}//namespace Uarc

}//namespace Protocol

}//namespace Uarc
