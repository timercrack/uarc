/*
 * Control.cpp
 *
 *  Created on: 2014-10-15
 *      Author: root
 */
#include <Poco/Util/Application.h>
#include <Poco/Random.h>
#include "Control.h"
#include "Frame.h"
#include "Device.h"
#include "TableFile.h"

namespace Uarc {

namespace Protocol {

namespace VIEC104 {

using Poco::Util::Application;
using Poco::Random;

Control::Control(Device *device) :
		_pLogger(&dynamic_cast<Application&>( Application::instance() ).logger()),
		_stop(true), _pDevice(device),
		_sendICount(0), _recvICount(0),_sendICountReq(0), _recvICountReq(0),
		_w(8), _k(12),
		_t0_value(30000), _t1_value(15000), _t2_value(10000), _t3_value(20000),
		_t_close_value(1),
		_callback_t0(new TimerCallback<Control>(*this, &Control::onTimer0)),
		_callback_t1(new TimerCallback<Control>(*this, &Control::onTimer1)),
		_callback_t2(new TimerCallback<Control>(*this, &Control::onTimer2)),
		_callback_t3(new TimerCallback<Control>(*this, &Control::onTimer3)),
		_callback_close(new TimerCallback<Control>(*this, &Control::onTimerClose)),
		_callback_vdata(new TimerCallback<Control>(*this, &Control::onTimerVdata)){

}

Control::~Control() {
	// TODO Auto-generated destructor stub
	stop();
	delete _callback_t0;
	delete _callback_t1;
	delete _callback_t2;
	delete _callback_t3;
	delete _callback_close;
	delete _callback_vdata;
}

void Control::show(const string &tip) const
{
	_pLogger->information("=========================================");
	_pLogger->information("%s"                                "", tip);
	_pLogger->information("=========================================");
	_pLogger->information("=    Send Frame I Count-----:%-6d     ""=", _sendICount);
	_pLogger->information("=    Receive Frame I Count--:%-6d     ""=", _recvICount);
	_pLogger->information("=    Send Frame I Requst----:%-6d     ""=", _sendICountReq);
	_pLogger->information("=    Receive Frame I Requst-:%-6d     ""=", _recvICountReq);
	_pLogger->information("=    K----------------------:%-6d     ""=", _k);
	_pLogger->information("=    W----------------------:%-6d     ""=", _w);
	_pLogger->information("=========================================");
}

void Control::showData(const char *data, int length)
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

void Control::start()
{
	_stop = false;
	clearSendFrame();
	_sendICount = 0;
	_recvICount = 0;
	_sendICountReq = 0;
	_recvICountReq = 0;
//	_k = 8;
//	_w = 12;
//	_t0_value = 30000;
//	_t1_value = 15000;
//	_t2_value = 10000;
//	_t3_value = 20000;

	timerStart(_t3, _t3_value);
	sendActivation();
}

void Control::stop()
{
	_stop = true;
	timerStop(_t0);
	timerStop(_t1);
	timerStop(_t2);
	timerStop(_t3);

	//防止关闭后有新的帧被加入
	clearSendFrame(1);//参数1，代表只关闭定时器不销毁帧结构
	clearSendFrame(1);//参数1，代表只关闭定时器不销毁帧结构
	clearSendFrame(1);//参数1，代表只关闭定时器不销毁帧结构
}

void Control::clearSendFrame(int type)//清空发送的帧
{
	_mutex.lock();
	for (int i = 0; i < (int)_sendFrameList.size(); ++i)
	{
		if (type == 0)
		{
			delete _sendFrameList[i];
		}else {
			_sendFrameList[i]->timerStop(_sendFrameList[i]->_t0);
			_sendFrameList[i]->timerStop(_sendFrameList[i]->_t1);
			_sendFrameList[i]->timerStop(_sendFrameList[i]->_t2);
			_sendFrameList[i]->timerStop(_sendFrameList[i]->_t3);
		}
	}
	if (type == 0)
	{
		_sendFrameList.clear();
	}
	_mutex.unlock();
}

void Control::addSendFrame(Frame *pFrame)//添加发送帧
{

	_mutex.lock();
	_sendFrameList.push_back(pFrame);
	_mutex.unlock();
}

void Control::removeSendFrame(Frame *pFrame)//删除发送的帧
{
	FastMutex::ScopedLock lock(_mutex_send_receive);

	_mutex.lock();
	for (int i = 0; i < (int)_sendFrameList.size(); ++i)
	{
		if (pFrame == _sendFrameList[i])
		{
			delete pFrame;
			_sendFrameList.erase(_sendFrameList.begin()+i);
			break;
		}
	}
	_mutex.unlock();
}

Frame *Control::findSendFrame(int format, int type, int reason)//查找发送的帧
{
	FastMutex::ScopedLock lock(_mutex);

	for (int i = 0; i < (int)_sendFrameList.size(); ++i)
	{
		Frame *pFrame = _sendFrameList[i];
		int format_ = pFrame->getFormat((char *)&(pFrame->_frame), pFrame->_length);
		int type_ = pFrame->getType((char *)&(pFrame->_frame), pFrame->_length);
		int reason_ = pFrame->getReason((char *)&(pFrame->_frame), pFrame->_length);
		if (format_ == format && type_ == type && reason_ == reason)
		{
			return pFrame;
		}
	}
	return NULL;
}

vector<Frame*> &Control::findSendFrame(int number, vector<Frame*> &result)//序列号小于number的帧
{
	FastMutex::ScopedLock lock(_mutex);

	for (int i = 0; i < (int)_sendFrameList.size(); ++i)
	{
		Frame *pFrame = _sendFrameList[i];
		int format_ = pFrame->getFormat((char *)&(pFrame->_frame), pFrame->_length);
		unsigned short control1 = pFrame->_frame.head.control1;
		unsigned short control2 = pFrame->_frame.head.control2;
		int number_ = (control1>>1) | (control2<<7);
		if (format_ == FRAME_I && number_ <= number)
		{
			result.push_back(pFrame);
		}
	}
	return result;
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
	if (&timer == &_t1)
	{
		timer.start(*_callback_t1);
	}
	if (&timer == &_t2)
	{
		timer.start(*_callback_t2);
	}
	if (&timer == &_t3)
	{
		timer.start(*_callback_t3);
	}
	if (&timer == &_t_close)
	{
		timer.start(*_callback_close);
	}
	if (&timer == &_vdata)
	{
		timer.start(*_callback_vdata);
	}
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
	//建立链接超时
	_pLogger->information("建立链接超时");
	_pDevice->close();
}

void Control::onTimer1(Timer &timer)
{
	//等待确认超时
	_pLogger->information("等待确认超时");
	_pDevice->close();
}

void Control::onTimer2(Timer &timer)
{
	//等待I格式报文超时，发送S帧确认
	_pLogger->information("等待I格式报文超时，发送S帧确认");
	sendSFrame();
}

void Control::onTimer3(Timer &timer)
{
	//链路空闲超时
	_pLogger->information("链路空闲超时");
	sendLinkTest();
}

void Control::onTimerClose(Timer &timer)
{
	_pDevice->close();
	timerStop(timer);
}

void Control::execVdata()
{
	int t_value = 0;

	time_t rawtime;
	time(&rawtime);

	Random rnd;
	rnd.seed((int)rawtime);
	t_value = rnd.next()%10+1;
	t_value *= 1000;
	t_value = 10;

	timerStart(_vdata, t_value);
}

void Control::onTimerVdata(Timer &timer)
{
	std::vector<DataItem> dataItems;
	int rowCount = _pDevice->_pIEC104Items->getRowCount();
	int count = 0;
	int addrRow = 0;

	time_t rawtime;
	time(&rawtime);

	Random rnd;
	rnd.seed((int)rawtime);
	count = rnd.next()%rowCount+1;
	count = rowCount;

	for (int i = 0; i < count; ++i)
	{
		DataItem item;
		addrRow = i;
		sscanf(_pDevice->_pIEC104Items->value(addrRow, 1).c_str(), "%X", &item.termId);
		sscanf(_pDevice->_pIEC104Items->value(addrRow, 2).c_str(), "%X", &item.itemId);
		item.time = rawtime;
		//int val_i = rnd.next();
		double val_f = rnd.nextDouble();
		val_f -= (int)val_f;
		
		//bool stat = false;
		char tmp[30] = {0};
		sprintf(tmp, "update.sh %06X %06X", item.termId, item.itemId);
		system(tmp);

		TableFile *source = new TableFile(string("/var/lib/uarc/value"));
		item.value = atof(source->value(0, 0).c_str());
		delete source;
		source = NULL;

		dataItems.push_back(item);
		printf("[地址:%s], [termID:%06X], [itemID:%06X], [值:%lf]\n", _pDevice->_pIEC104Items->value(addrRow, 0).c_str(), item.termId, item.itemId, item.value);
	}
	
	_pDevice->report(REASON_SPONT, dataItems);

	timerStop(timer);
}

void Control::splitFrame(char *data, int length)
{
	Frame::crypt(data, length, _pDevice->getCrypt().c_str(), _pDevice->getCryptLength());

	int pos = 0;
	int len = 0;
	while (true)
	{
		len = Frame::getLength(data + pos, length - pos);
		if (len < LENGTH_APCI)
		{
			return ;
		}
		handleData(data + pos, len);
		pos += len;
	}
}

void Control::handleData(char *data, int length)
{
	printf("device[%s] receive data:", _pDevice->identity().c_str());
	_pLogger->information("device[%s] receive data:", _pDevice->identity());
	showData(data, length);

	timerRestart(_t3);
	int format = Frame::getFormat(data, length);
	switch (format)
	{
	case FRAME_I:
		handleFrameI(data, length);
		break;
	case FRAME_S:
		handleFrameS(data, length);
		break;
	case FRAME_U:
		handleFrameU(data, length);
		break;
	default:
		break;
	}
}

void Control::handleFrameI(const char *data, int length)
{
	if (FRAME_I != Frame::getFormat(data, length))
		return ;

	receiveI(data, length);
	int reason = Frame::getReason(data, length);
	switch (reason)
	{
	case REASON_ACT_CON://激活确认
		receiveActCon(data, length);
		break;
	case REASON_INRO_EN://响应站召唤
		receiveCallAllData(data, length);
		break;
	case REASON_ACT_TERM://激活停止
		receiveActStop(data, length);
		break;
	case REASON_SPONT://突发
		receiveSpont(data, length);
		break;
	case REASON_REQ://请求或被请求
		receiveSpont(data, length);
		break;
	default:
		break;
	}
}

void Control::receiveActCon(const char *data, int length)
{
	int type = Frame::getType(data, length);
	switch (type)
	{
	case 0x67://对时确认
		receiveTimeSyncReq();
		break;
	case 0x64://总召确认
		receiveCallAllReq();
		break;
	case C_SC_NA_1://单点命令
	{
		Frame *pFrame = findSendFrame(FRAME_I, C_SC_NA_1, REASON_ACT);
		removeSendFrame(pFrame);
		receiveCmdReq(data, length);
		break;
	}
	case C_DC_NA_1://双点命令
	{
		Frame *pFrame = findSendFrame(FRAME_I, C_DC_NA_1, REASON_ACT);
		removeSendFrame(pFrame);
		receiveCmdReq(data, length);
		break;
	}
	case C_SE_NA_1://归一化设点命令
	{
		Frame *pFrame = findSendFrame(FRAME_I, C_SE_NA_1, REASON_ACT);
		removeSendFrame(pFrame);
		receiveCmdReq(data, length);
		break;
	}
	case C_SE_NB_1://标度化设点命令
	{
		Frame *pFrame = findSendFrame(FRAME_I, C_SE_NB_1, REASON_ACT);
		removeSendFrame(pFrame);
		receiveCmdReq(data, length);
		break;
	}
	case C_SE_NC_1://短浮点数设点命令
	{
		Frame *pFrame = findSendFrame(FRAME_I, C_SE_NC_1, REASON_ACT);
		removeSendFrame(pFrame);
		receiveCmdReq(data, length);
		break;
	}
	case C_SC_TA_1://带长时标，单点命令
	{
		Frame *pFrame = findSendFrame(FRAME_I, C_SC_TA_1, REASON_ACT);
		removeSendFrame(pFrame);
		receiveCmdReq(data, length);
		break;
	}
	case C_DC_TA_1://带长时标，双点命令
	{
		Frame *pFrame = findSendFrame(FRAME_I, C_DC_TA_1, REASON_ACT);
		removeSendFrame(pFrame);
		receiveCmdReq(data, length);
		break;
	}
	case C_SE_TA_1://带长时标，归一化设点命令
	{
		Frame *pFrame = findSendFrame(FRAME_I, C_SE_TA_1, REASON_ACT);
		removeSendFrame(pFrame);
		receiveCmdReq(data, length);
		break;
	}
	case C_SE_TB_1://带长时标，标度化设点命令
	{
		Frame *pFrame = findSendFrame(FRAME_I, C_SE_TB_1, REASON_ACT);
		removeSendFrame(pFrame);
		receiveCmdReq(data, length);
		break;
	}
	case C_SE_TC_1://带长时标，短浮点数设点命令
	{
		Frame *pFrame = findSendFrame(FRAME_I, C_SE_TC_1, REASON_ACT);
		removeSendFrame(pFrame);
		receiveCmdReq(data, length);
		break;
	}
	default:
		break;
	}
}

void Control::receiveCmdReq(const char *data, int length)
{
	int status = 0;
	DataItem dataItem;
	Frame::DecodeCmd(dataItem, status, data, length, _pDevice->_pIEC104Items);
	if (status == SELECT_CMD)
	{
		sendExecCmd(dataItem);
	}else if (status == EXEC_CMD)
	{

	}else if (status == UNDO_CMD)
	{

	}
}

void Control::receiveActStop(const char *data, int length)
{
	int type = Frame::getType(data, length);
	switch (type)
	{
	case C_SC_NA_1://单点命令
	{
		Frame *pFrame = findSendFrame(FRAME_I, C_SC_NA_1, REASON_ACT);
		removeSendFrame(pFrame);
		break;
	}
	case C_DC_NA_1://双点命令
	{
		Frame *pFrame = findSendFrame(FRAME_I, C_DC_NA_1, REASON_ACT);
		removeSendFrame(pFrame);
		break;
	}
	case C_SE_NA_1://归一化设点命令
	{
		Frame *pFrame = findSendFrame(FRAME_I, C_SE_NA_1, REASON_ACT);
		removeSendFrame(pFrame);
		break;
	}
	case C_SE_NB_1://标度化设点命令
	{
		Frame *pFrame = findSendFrame(FRAME_I, C_SE_NB_1, REASON_ACT);
		removeSendFrame(pFrame);
		break;
	}
	case C_SE_NC_1://短浮点数设点命令
	{
		Frame *pFrame = findSendFrame(FRAME_I, C_SE_NC_1, REASON_ACT);
		removeSendFrame(pFrame);
		break;
	}
	case C_RD_NA_1://单点召唤
	{
		Frame *pFrame = findSendFrame(FRAME_I, C_RD_NA_1, REASON_REQ);
		removeSendFrame(pFrame);
		break;
	}
	default:
		break;
	}
}

void Control::sendCmd(const vector<DataItem> &dataItems)
{
	DataItem dataItem = dataItems[0];
	sendSelectCmd(dataItem);
}

void Control::sendSelectCmd(DataItem &dataItem)
{
	FastMutex::ScopedLock lock(_mutex_send_receive);

	stFrame data;
	memset(&data, 0, sizeof(stFrame));
	int length = Frame::buildSelectCmd(data, _sendICount, _recvICount,
			dataItem, _pDevice->_pIEC104Items);
	if (length > 0)
	{
		if (sendFrameI((char *)&data, length))
		{
			Frame *pFrame = new Frame((char *)&data, length, this);
			addSendFrame(pFrame);
			pFrame->timerStart(pFrame->_t1, pFrame->_t1_value);
		}
	}else {
		_pLogger->information("build select cmd failed.");
	}
}

void Control::sendExecCmd(DataItem &dataItem)
{
	FastMutex::ScopedLock lock(_mutex_send_receive);

	stFrame data;
	memset(&data, 0, sizeof(stFrame));
	int length = Frame::buildExecCmd(data, _sendICount, _recvICount,
			dataItem, _pDevice->_pIEC104Items);
	if (length > 0)
	{
		if (sendFrameI((char *)&data, length))
		{
			Frame *pFrame = new Frame((char *)&data, length, this);
			addSendFrame(pFrame);
			pFrame->timerStart(pFrame->_t1, pFrame->_t1_value);
		}
	}else {
		_pLogger->information("build exec cmd failed.");
	}
}

void Control::sendUndoCmd(DataItem &dataItem)
{
	FastMutex::ScopedLock lock(_mutex_send_receive);

	stFrame data;
	memset(&data, 0, sizeof(stFrame));
	int length = Frame::buildUndoCmd(data, _sendICount, _recvICount,
			dataItem, _pDevice->_pIEC104Items);
	if (length > 0)
	{
		if (sendFrameI((char *)&data, length))
		{
			Frame *pFrame = new Frame((char *)&data, length, this);
			addSendFrame(pFrame);
			pFrame->timerStart(pFrame->_t1, pFrame->_t1_value);
		}
	}else {
		_pLogger->information("build undo cmd failed.");
	}
}

void Control::sendCallAll()
{
	FastMutex::ScopedLock lock(_mutex_send_receive);

	stFrame data;
	memset(&data, 0, sizeof(stFrame));
	int length = Frame::buildCallAll(data, _sendICount, _recvICount);
	if (length == sizeof(stHead) + 1)
	{
		if (sendFrameI((char *)&data, length))
		{
			Frame *pFrame = new Frame((char *)&data, length, this);
			addSendFrame(pFrame);
			pFrame->timerStart(pFrame->_t1, pFrame->_t1_value);
		}
	}else {
		_pLogger->information("build call all failed.");
	}
}

void Control::sendCallSingle(const DataItem &dataItem)
{
	FastMutex::ScopedLock lock(_mutex_send_receive);

	DataItem di = dataItem;
	stFrame data;
	memset(&data, 0, sizeof(stFrame));
	int length = Frame::buildCallSingle(data,
			_sendICount, _recvICount, di, _pDevice->_pIEC104Items);
	if (length == sizeof(stHead))
	{
		if (sendFrameI((char *)&data, length))
		{
			Frame *pFrame = new Frame((char *)&data, length, this);
			addSendFrame(pFrame);
			pFrame->timerStart(pFrame->_t1, pFrame->_t1_value);
		}
	}else {
		_pLogger->information("build call single failed.");
	}
}

void Control::receiveCallAllReq()
{
	Frame *pFrame = findSendFrame(FRAME_I, 0x64, REASON_ACT);
	removeSendFrame(pFrame);
}

void Control::receiveCallAllData(const char *data, int length)
{
	std::vector<DataItem> dataItems;
	Frame::Decode(dataItems, data, length, _pDevice->_pIEC104Items);
	//_pDevice->report(REASON_INRO_EN, dataItems);
}

void Control::sendTimeSync()
{
	FastMutex::ScopedLock lock(_mutex_send_receive);

	stFrame data;
	memset(&data, 0, sizeof(stFrame));
	int length = Frame::buildTimeSync(data, _sendICount, _recvICount);
	if (length == sizeof(stHead) + sizeof(stTimeStamp))
	{
		if (sendFrameI((char *)&data, length))
		{
			Frame *pFrame = new Frame((char *)&data, length, this);
			addSendFrame(pFrame);
			pFrame->timerStart(pFrame->_t1, pFrame->_t1_value);
		}
	}else {
		_pLogger->information("build time sync failed.");
	}
}

void Control::receiveTimeSyncReq()
{
	Frame *pFrame = findSendFrame(FRAME_I, 0x67, REASON_ACT);
	removeSendFrame(pFrame);
}

void Control::receiveSpont(const char *data, int length)
{
	std::vector<DataItem> dataItems;
	Frame::Decode(dataItems, data, length, _pDevice->_pIEC104Items);
	//_pDevice->report(REASON_SPONT, dataItems);
}

void Control::handleFrameS(const char *data, int length)
{
	if (FRAME_S != Frame::getFormat(data, length))
		return ;

	receiveS(data, length);
	vector<Frame*> result;
	int num = Frame::getICountReq(data, length);
	findSendFrame(num, result);

	for (int i = 0; i < (int)result.size(); ++i)
	{
		removeSendFrame(result[i]);
	}
}

void Control::handleFrameU(const char *data, int length)
{
	if (FRAME_U != Frame::getFormat(data, length))
		return ;
	int reason = Frame::getReason(data, length);
	switch (reason)
	{
	case REASON_CUS_TEST_LINK://测试链路请求
		sendLinkTestReq();
		break;
	case REASON_CUS_REQ_TEST_LINK://测试链路回应
		receiveLinkTestReq();
		break;
	case REASON_CUS_ACTIVATION://激活请求
		_pLogger->information("error:接收到激活请求");
		sendActivationReq();
		break;
	case REASON_CUS_REQ_ACTIVATION://激活请求回应
		receiveActivationReq();
		break;
	case REASON_CUS_STOP_LINK://断开链接
		sendStopReq();
		break;
	case REASON_CUS_REQ_STOP_LINK://断开链接回应
		receiveStopReq();
		break;
	default:
		break;
	}
}

void Control::sendActivation()
{
	FastMutex::ScopedLock lock(_mutex_send_receive);

	stFrame data;
	memset(&data, 0, sizeof(stFrame));
	int length = Frame::buildActivation(data);
	if (length == LENGTH_APCI)
	{
		if (sendFrameU((char *)&data, length))
		{
			Frame *pFrame = new Frame((char *)&data, length, this);
			addSendFrame(pFrame);
			pFrame->timerStart(pFrame->_t0, pFrame->_t0_value);
		}
	}else {
		_pLogger->information("build activation failed.");
	}
}

void Control::sendActivationReq()
{
	stFrame data;
	memset(&data, 0, sizeof(stFrame));
	int length = Frame::buildActivationReq(data);
	if (length == LENGTH_APCI)
	{
		sendFrameU((char *)&data, length);
	}else {
		_pLogger->information("build activation req failed.");
	}
}

void Control::receiveActivationReq()
{
	Frame *pFrame = findSendFrame(FRAME_U, 0, REASON_CUS_ACTIVATION);
	removeSendFrame(pFrame);

	sendCallAll();
	sendTimeSync();
}

void Control::sendLinkTest()
{
	FastMutex::ScopedLock lock(_mutex_send_receive);

	stFrame data;
	memset(&data, 0, sizeof(stFrame));
	int length = Frame::buildLinkTest(data);
	if (length == LENGTH_APCI)
	{
		if (sendFrameU((char *)&data, length))
		{
			Frame *pFrame = new Frame((char *)&data, length, this);
			addSendFrame(pFrame);
			pFrame->timerStart(pFrame->_t1, pFrame->_t1_value);
		}
	}else {
		_pLogger->information("build link test failed.");
	}
}

void Control::sendLinkTestReq()
{
	stFrame data;
	memset(&data, 0, sizeof(stFrame));
	int length = Frame::buildLinkTestReq(data);
	if (length == LENGTH_APCI)
	{
		sendFrameU((char *)&data, length);
	}else {
		_pLogger->information("build link test req failed.");
	}
}

void Control::receiveLinkTestReq()
{
	Frame *pFrame = findSendFrame(FRAME_U, 0, REASON_CUS_TEST_LINK);
	removeSendFrame(pFrame);
}

void Control::sendStop()
{
	FastMutex::ScopedLock lock(_mutex_send_receive);

	stFrame data;
	memset(&data, 0, sizeof(stFrame));
	int length = Frame::buildStop(data);
	if (length == LENGTH_APCI)
	{
		if (sendFrameU((char *)&data, length))
		{
			Frame *pFrame = new Frame((char *)&data, length, this);
			addSendFrame(pFrame);
			pFrame->timerStart(pFrame->_t1, pFrame->_t1_value);
		}
	}else {
		_pLogger->information("build stop failed.");
	}
}

void Control::sendStopReq()
{
	stFrame data;
	memset(&data, 0, sizeof(stFrame));
	int length = Frame::buildStopReq(data);
	if (length == LENGTH_APCI)
	{
		sendFrameU((char *)&data, length);
	}else {
		_pLogger->information("build stop req failed.");
	}
}

void Control::receiveStopReq()
{
	_pDevice->close();
}

void Control::sendSFrame()
{
	stFrame data;
	memset(&data, 0, sizeof(stFrame));
	int length = Frame::buildFrameS(data, _recvICount);
	if (length == LENGTH_APCI)
	{
		sendFrameS((char *)&data, length);
	}else {
		_pLogger->information("build S frame failed.");
	}
}

bool Control::sendFrameI(const char *pFrame, int length)
{
	sendI(pFrame, length);
	return sendFrame(pFrame, length);
}

bool Control::sendFrameS(const char *pFrame, int length)
{
	sendS(pFrame, length);
	return sendFrame(pFrame, length);
}

bool Control::sendFrameU(const char *pFrame, int length)
{
	return sendFrame(pFrame, length);
}

bool Control::sendFrame(const char *pFrame, int length)
{
	stFrame frame;
	memset(&frame, 0, sizeof(stFrame));
	memcpy(&frame, pFrame, length);
	Frame::crypt((char *)&frame, length, _pDevice->getCrypt().c_str(), _pDevice->getCryptLength());
	if (_pDevice->send((char *)&frame, length) < 0)
	{
		timerStart(_t_close, _t_close_value);
		return false;
	}
	printf("device[%s] send data---:", _pDevice->identity().c_str());
	_pLogger->information("device[%s] send data---:", _pDevice->identity());
	showData(pFrame, length);
	return true;
}

bool Control::reqSendICount(const char *pFrame, int length)
{
	bool req = true;
	int recvICountReq = Frame::getICountReq(pFrame, length);
	(recvICountReq > _recvICountReq) ? (_recvICountReq = recvICountReq) : (req = false);

	return req;
}

bool Control::reqRecvICount(const char *pFrame, int length)
{
	bool req = true;
	int sendICountReq = Frame::getICountReq(pFrame, length);
	(sendICountReq > _sendICountReq) ? (_sendICountReq = sendICountReq) : (req = false);

	return req;
}

void Control::receiveI(const char *pFrame, int length)
{
	++_recvICount;
	if (_recvICount - _sendICountReq >= _w)//判断未确认的I帧是否达到w个
	{
		sendSFrame();
	}

	reqSendICount(pFrame, length);//确认发送的I帧
	if (_recvICount > _ICountMax)//接收序列号过大
	{
		_pLogger->information("接收序列号过大\n");
		sendStop();
	}
}

void Control::receiveS(const char *pFrame, int length)
{
	reqSendICount(pFrame, length);//确认发送的I帧
}

void Control::sendS(const char *pFrame, int length)
{
	reqRecvICount(pFrame, length);//确认接收的I帧
}

void Control::sendI(const char *pFrame, int length)
{
	++_sendICount;
	if (_sendICount - _recvICountReq >= _k)//有k个I帧未被确认
	{
		sendStop();
		return ;
	}
	reqRecvICount(pFrame, length);//确认接收的I帧

	if (_sendICount > _ICountMax)//发送序列号过大
	{
		_pLogger->information("发送序列号过大\n");
		sendStop();
		return ;
	}
}

}//namespace VIEC104

}//namespace Protocol

}//namespace Uarc
