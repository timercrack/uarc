/*
 * Frame.cpp
 *
 *  Created on: 2014-10-15
 *      Author: root
 */

#include "Frame.h"
#include "Control.h"
#include "TableFile.h"

namespace Uarc {

namespace Protocol {

namespace VIEC104 {

Frame::Frame(const char *pFrame, int length, Control *parent) :
		_t0_value(30000), _t1_value(15000), _t2_value(10000), _t3_value(20000),
		_callback_t0(new TimerCallback<Control>(*parent, &Control::onTimer0)),
		_callback_t1(new TimerCallback<Control>(*parent, &Control::onTimer1)),
		_callback_t2(new TimerCallback<Control>(*parent, &Control::onTimer2)),
		_callback_t3(new TimerCallback<Control>(*parent, &Control::onTimer3)){
	// TODO Auto-generated constructor stub
	memcpy(&_frame, pFrame, length);
	_length = length;
}

Frame::~Frame() {
	// TODO Auto-generated destructor stub
	timerStop(_t0);
	timerStop(_t1);
	timerStop(_t2);
	timerStop(_t3);

	delete _callback_t0;
	delete _callback_t1;
	delete _callback_t2;
	delete _callback_t3;
}

void Frame::timerStart(Timer &timer, int time)
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
}

void Frame::timerStop(Timer &timer)
{
	timer.restart(0);
}

void Frame::timerRestart(Timer &timer)
{
	timer.restart();
}

void Frame::crypt(char *data, int dataLen, const char *key, int keyLen)
{
	int data_len = dataLen;
	int key_len = keyLen;
	char *p_data = data;
	const char *p_key = key;

	while (1)
	{
		if (!keyLen || !data_len)
		{
			break;
		}
		*p_data++ ^= *p_key;
		--data_len;
		key_len = (key_len-1) ? (key_len-1) : keyLen;
		p_key = (key_len==keyLen) ? key : p_key+1;
	}
}

int Frame::getLength(const char *pFrame, int length)
{
	if (!pFrame)return -1;
	if (length < LENGTH_APCI)return -2;

	stFrame frame;
	memcpy(&frame, pFrame, LENGTH_APCI);

	if (frame.head.length + 2 > length)
		return -3;

	return frame.head.length + 2;
}

int Frame::getFormat(const char *pFrame, int length)
{
	if (!pFrame)return -1;
	if (length <= 0 || length > (int)sizeof(stFrame))return -2;

	stFrame frame;
	memcpy(&frame, pFrame, length);

	//I帧
	if (frame.head.frameI_D0 == 0x00)
	{
		return FRAME_I;
	}
	//S帧
	else if (frame.head.frameSU_D0_D1 == 0x01)
	{
		return FRAME_S;
	}
	//U帧
	else if (frame.head.frameSU_D0_D1 == 0x03)
	{
		return FRAME_U;
	}

	return -3;
}

int Frame::getType(const char *pFrame, int length)
{
	int type = -1;
	int format = getFormat(pFrame, length);
	switch (format)
	{
	case FRAME_I:
		type = getTypeI(pFrame, length);
		break;
	case FRAME_S:
		type = getTypeS(pFrame, length);
		break;
	case FRAME_U:
		type = getTypeU(pFrame, length);
		break;
	default:
		break;
	}

	return type;
}

int Frame::getTypeI(const char *pFrame, int length)
{
	stFrame &frame = *((stFrame*)pFrame);
	return frame.head.type;
}

int Frame::getTypeS(const char *pFrame, int length)
{
	return 0;
}

int Frame::getTypeU(const char *pFrame, int length)
{
	return 0;
}

int Frame::getReason(const char *pFrame, int length)
{
	int reason = -1;
	int format = getFormat(pFrame, length);
	switch (format)
	{
	case FRAME_I:
//		std::cout<<"FRAME_I"<<std::endl;
		reason = getReasonI(pFrame, length);
		break;
	case FRAME_S:
//		std::cout<<"FRAME_S"<<std::endl;
		reason = getReasonS(pFrame, length);
		break;
	case FRAME_U:
//		std::cout<<"FRAME_U"<<std::endl;
		reason = getReasonU(pFrame, length);
		break;
	default:
		break;
	}

	return reason;
}

int Frame::getReasonI(const char *pFrame, int length)
{
	stFrame &frame = *((stFrame*)pFrame);
	return frame.head.reason;
}

int Frame::getReasonS(const char *pFrame, int length)
{
	return 0;
}

int Frame::getReasonU(const char *pFrame, int length)
{
	stFrame &frame = *((stFrame*)pFrame);
	//测试链路
	if(frame.head.length == 0x04 && frame.head.control1 == 0x43)
		return REASON_CUS_TEST_LINK;
	//链路确认
	if(frame.head.length == 0x04 && frame.head.control1 == 0x83)
		return REASON_CUS_REQ_TEST_LINK;
	//激活请求
	if(frame.head.length == 0x04 && frame.head.control1 == 0x07)
		return REASON_CUS_ACTIVATION;
	//激活确认
	if(frame.head.length == 0x04 && frame.head.control1 == 0x0B)
		return REASON_CUS_REQ_ACTIVATION;
	//断开链路
	if(frame.head.length == 0x04 && frame.head.control1 == 0x13)
		return REASON_CUS_STOP_LINK;
	//断开链路确认
	if(frame.head.length == 0x04 && frame.head.control1 == 0x23)
		return REASON_CUS_REQ_STOP_LINK;

	return -1;
}

int Frame::getICount(const char *pFrame, int length)
{
	stFrame &frame = *((stFrame*)pFrame);
	unsigned short control1 = frame.head.control1;
	unsigned short control2 = frame.head.control2;
	unsigned short result = (control1>>1) | (control2<<7);
	return result;
}

int Frame::getICountReq(const char *pFrame, int length)
{
	stFrame &frame = *((stFrame*)pFrame);
	unsigned short control3 = frame.head.control3;
	unsigned short control4 = frame.head.control4;
	unsigned short result = (control3>>1) | (control4<<7);
	return result;
}

int Frame::buildLinkTest(stFrame &frame)
{
	memset(&frame, 0, sizeof(stFrame));
	frame.head.start = 0x68;
	frame.head.length = 0x04;
	frame.head.control1 = 0x43;
	frame.head.control2 = 0x00;
	frame.head.control3 = 0x00;
	frame.head.control4 = 0x00;

	return LENGTH_APCI;
}

int Frame::buildLinkTestReq(stFrame &frame)
{
	memset(&frame, 0, sizeof(stFrame));
	frame.head.start = 0x68;
	frame.head.length = 0x04;
	frame.head.control1 = 0x83;
	frame.head.control2 = 0x00;
	frame.head.control3 = 0x00;
	frame.head.control4 = 0x00;

	return LENGTH_APCI;
}

int Frame::buildActivation(stFrame &frame)
{
	memset(&frame, 0, sizeof(stFrame));
	frame.head.start = 0x68;
	frame.head.length = 0x04;
	frame.head.control1 = 0x07;
	frame.head.control2 = 0x00;
	frame.head.control3 = 0x00;
	frame.head.control4 = 0x00;

	return LENGTH_APCI;
}

int Frame::buildActivationReq(stFrame &frame)
{
	memset(&frame, 0, sizeof(stFrame));
	frame.head.start = 0x68;
	frame.head.length = 0x04;
	frame.head.control1 = 0x0B;
	frame.head.control2 = 0x00;
	frame.head.control3 = 0x00;
	frame.head.control4 = 0x00;

	return LENGTH_APCI;
}

int Frame::buildStop(stFrame &frame)
{
	memset(&frame, 0, sizeof(stFrame));
	frame.head.start = 0x68;
	frame.head.length = 0x04;
	frame.head.control1 = 0x13;
	frame.head.control2 = 0x00;
	frame.head.control3 = 0x00;
	frame.head.control4 = 0x00;

	return LENGTH_APCI;
}

int Frame::buildStopReq(stFrame &frame)
{
	memset(&frame, 0, sizeof(stFrame));
	frame.head.start = 0x68;
	frame.head.length = 0x04;
	frame.head.control1 = 0x23;
	frame.head.control2 = 0x00;
	frame.head.control3 = 0x00;
	frame.head.control4 = 0x00;

	return LENGTH_APCI;
}

int Frame::buildFrameS(stFrame &frame, int recvICount)
{
	unsigned short u_recvICount = recvICount;
	memset(&frame, 0, sizeof(stFrame));
	frame.head.start = 0x68;
	frame.head.length = 0x04;
	frame.head.control1 = 0x01;
	frame.head.control2 = 0x00;
	frame.head.control3 = (0xff&(u_recvICount<<1));
	frame.head.control4 = (0xff&(u_recvICount>>7));

	return LENGTH_APCI;
}

int Frame::buildCallAll(stFrame &frame, int sendICount, int recvICount)
{
	unsigned short u_sendICount = sendICount;
	unsigned short u_recvICount = recvICount;
	memset(&frame, 0, sizeof(stFrame));
	frame.head.start = 0x68;
	frame.head.length = 0x0E;
	frame.head.control1 = (0xff&(u_sendICount<<1));
	frame.head.control2 = (0xff&(u_sendICount>>7));
	frame.head.control3 = (0xff&(u_recvICount<<1));
	frame.head.control4 = (0xff&(u_recvICount>>7));

	frame.head.type = 0x64;
	frame.head.vsq = 01;
	frame.head.reason = 0x0006;
	frame.head.addr = 0x0001;//单元公共地址
	frame.head.objAddr.objAddrChar1 = 0x00;//信息地址
	frame.head.objAddr.objAddrChar2 = 0x00;//信息地址
	frame.head.objAddr.objAddrChar3 = 0x00;//信息地址

	frame.buf[0] = 0x14;

	return sizeof(stHead) + 1;
}

int Frame::buildCallSingle(stFrame &frame,
		int sendICount, int recvICount, DataItem &dataItem, TableFile *pIEC104Items)
{
	unsigned short u_sendICount = sendICount;
	unsigned short u_recvICount = recvICount;
	memset(&frame, 0, sizeof(stFrame));
	frame.head.start = 0x68;
	frame.head.length = 0x0D;
	frame.head.control1 = (0xff&(u_sendICount<<1));
	frame.head.control2 = (0xff&(u_sendICount>>7));
	frame.head.control3 = (0xff&(u_recvICount<<1));
	frame.head.control4 = (0xff&(u_recvICount>>7));

	frame.head.type = 0x66;
	frame.head.vsq = 01;
	frame.head.reason = 0x0005;
	frame.head.addr = 0x0001;//单元公共地址

	//查询信息体地址
	int objAddr = -1;
	if (getAddrByTermItem(objAddr, dataItem.termId, dataItem.itemId, pIEC104Items))
	{
		frame.head.objAddr.objAddrChar1 = 0xff&((unsigned int)objAddr);
		frame.head.objAddr.objAddrChar2 = 0xff&(((unsigned int)objAddr)>>8);
		frame.head.objAddr.objAddrChar3 = 0xff&(((unsigned int)objAddr)>>16);
	}else {
		return -1;
	}

	return sizeof(stHead);
}

int Frame::buildTimeSync(stFrame &frame, int sendICount, int recvICount)
{
//	DateTime dateTime;
	LocalDateTime dateTime;

	unsigned short u_sendICount = sendICount;
	unsigned short u_recvICount = recvICount;

	unsigned char Y = dateTime.year()%100;
	unsigned char M = dateTime.month();
	unsigned char D = dateTime.day();
	unsigned char h = dateTime.hour();
	unsigned char m = dateTime.minute();
	unsigned char s = dateTime.second();//秒
	unsigned short millisec = dateTime.millisecond();//毫秒
//	unsigned short microsec = dateTime.microsecond();//微秒

	millisec += 1000*s;

	memset(&frame, 0, sizeof(stFrame));
	frame.head.start = 0x68;
	frame.head.length = 0x14;
	frame.head.control1 = (0xff&(u_sendICount<<1));
	frame.head.control2 = (0xff&(u_sendICount>>7));
	frame.head.control3 = (0xff&(u_recvICount<<1));
	frame.head.control4 = (0xff&(u_recvICount>>7));

	frame.head.type = 0x67;
	frame.head.vsq = 01;
	frame.head.reason = 0x0006;
	frame.head.addr = 0x0001;//单元公共地址
	frame.head.objAddr.objAddrChar1 = 0x00;//信息地址
	frame.head.objAddr.objAddrChar2 = 0x00;//信息地址
	frame.head.objAddr.objAddrChar3 = 0x00;//信息地址

	stTimeStamp time_stamp;
	memset(&time_stamp, 0, sizeof(stTimeStamp));
	time_stamp.year_res_d7 = 0;
	time_stamp.year_val_d0_d6 = Y;

	time_stamp.month_res_d4_d7 = 0;
	time_stamp.month_val_d0_d3 = M;

	time_stamp.day_d5_d7 = 0;
	time_stamp.day_val_d0_d4 = D;

	time_stamp.hour_su_d7 = 0;
	time_stamp.hour_res_d5_d6 = 0;
	time_stamp.hour_val_d0_d4 = h;

	time_stamp.min_iv_d7 = 0;
	time_stamp.min_val_d0_d5 = m;

	time_stamp.millisec = millisec;

	memcpy(frame.buf, &time_stamp, sizeof(stTimeStamp));

	return sizeof(stHead) + sizeof(stTimeStamp);
}

int Frame::buildSelectCmd(stFrame &frame,
		int sendICount, int recvICount, DataItem &dataItem, TableFile *pIEC104Items)
{
	unsigned short u_sendICount = sendICount;
	unsigned short u_recvICount = recvICount;
	memset(&frame, 0, sizeof(stFrame));
	frame.head.start = 0x68;

	frame.head.control1 = (0xff&(u_sendICount<<1));
	frame.head.control2 = (0xff&(u_sendICount>>7));
	frame.head.control3 = (0xff&(u_recvICount<<1));
	frame.head.control4 = (0xff&(u_recvICount>>7));

	frame.head.vsq = 01;
	frame.head.reason = 0x0006;
	//单元公共地址------COMADDR低字节 = 公共地址；COMADDR高字节 = 装置地址
	frame.head.addr = 0x0001;

	//查询信息体地址
	int objAddr = -1;
	if (getAddrByTermItem(objAddr, dataItem.termId, dataItem.itemId, pIEC104Items))
	{
		frame.head.objAddr.objAddrChar1 = 0xff&((unsigned int)objAddr);
		frame.head.objAddr.objAddrChar2 = 0xff&(((unsigned int)objAddr)>>8);
		frame.head.objAddr.objAddrChar3 = 0xff&(((unsigned int)objAddr)>>16);
	}else {
		return -1;
	}
	int type = 0;
	int len = 0;
	if (dataItem.time == 0)
	{
		type = 5;
	}else {
		type = 6;
	}
	if (getTypeByAddr(type, objAddr, pIEC104Items))
	{
		switch(type)
		{
		case C_SC_NA_1://单点命令
			frame.head.type = C_SC_NA_1;
			len = buildCSCNA1(SELECT_CMD, (char *)frame.buf, dataItem);
			break;
		case C_DC_NA_1://双点命令
			frame.head.type = C_DC_NA_1;
			len = buildCDCNA1(SELECT_CMD, (char *)frame.buf, dataItem);
			break;
		case C_SE_NA_1://归一化设点命令
			frame.head.type = C_SE_NA_1;
			len = buildCSENA1(SELECT_CMD, (char *)frame.buf, dataItem);
			break;
		case C_SE_NB_1://标度化设点命令
			frame.head.type = C_SE_NB_1;
			len = buildCSENB1(SELECT_CMD, (char *)frame.buf, dataItem);
			break;
		case C_SE_NC_1://短浮点数设点命令
			frame.head.type = C_SE_NC_1;
			len = buildCSENC1(SELECT_CMD, (char *)frame.buf, dataItem);
			break;
		case C_SC_TA_1://带长时标，单点命令
			frame.head.type = C_SC_TA_1;
			len = buildCSCTA1(SELECT_CMD, (char *)frame.buf, dataItem);
			break;
		case C_DC_TA_1://带长时标，双点命令
			frame.head.type = C_DC_TA_1;
			len = buildCDCTA1(SELECT_CMD, (char *)frame.buf, dataItem);
			break;
		case C_SE_TA_1://带长时标，归一化设点命令
			frame.head.type = C_SE_TA_1;
			len = buildCSETA1(SELECT_CMD, (char *)frame.buf, dataItem);
			break;
		case C_SE_TB_1://带长时标，标度化设点命令
			frame.head.type = C_SE_TB_1;
			len = buildCSETB1(SELECT_CMD, (char *)frame.buf, dataItem);
			break;
		case C_SE_TC_1://带长时标，短浮点数设点命令
			frame.head.type = C_SE_TC_1;
			len = buildCSETC1(SELECT_CMD, (char *)frame.buf, dataItem);
			break;
		default:
			return -1;
		}
	}else {
		return -1;
	}
	frame.head.length = sizeof(stHead) + len - 2;

	return sizeof(stHead) + len;
}

int Frame::buildExecCmd(stFrame &frame, int sendICount, int recvICount,
		DataItem &dataItem, TableFile *pIEC104Items)
{
	unsigned short u_sendICount = sendICount;
	unsigned short u_recvICount = recvICount;
	memset(&frame, 0, sizeof(stFrame));
	frame.head.start = 0x68;

	frame.head.control1 = (0xff&(u_sendICount<<1));
	frame.head.control2 = (0xff&(u_sendICount>>7));
	frame.head.control3 = (0xff&(u_recvICount<<1));
	frame.head.control4 = (0xff&(u_recvICount>>7));

	frame.head.vsq = 01;
	frame.head.reason = 0x0006;
	//单元公共地址------COMADDR低字节 = 公共地址；COMADDR高字节 = 装置地址
	frame.head.addr = 0x0001;

	//查询信息体地址
	int objAddr = -1;
	if (getAddrByTermItem(objAddr, dataItem.termId, dataItem.itemId, pIEC104Items))
	{
		frame.head.objAddr.objAddrChar1 = 0xff&((unsigned int)objAddr);
		frame.head.objAddr.objAddrChar2 = 0xff&(((unsigned int)objAddr)>>8);
		frame.head.objAddr.objAddrChar3 = 0xff&(((unsigned int)objAddr)>>16);
	}else {
		return -1;
	}

	int type = 0;
	int len = 0;
	if (dataItem.time == 0)
	{
		type = 5;
	}else {
		type = 6;
	}
	if (getTypeByAddr(type, objAddr, pIEC104Items))
	{
		switch(type)
		{
		case C_SC_NA_1://单点命令
			frame.head.type = C_SC_NA_1;
			len = buildCSCNA1(EXEC_CMD, (char *)frame.buf, dataItem);
			break;
		case C_DC_NA_1://双点命令
			frame.head.type = C_DC_NA_1;
			len = buildCDCNA1(EXEC_CMD, (char *)frame.buf, dataItem);
			break;
		case C_SE_NA_1://归一化设点命令
			frame.head.type = C_SE_NA_1;
			len = buildCSENA1(EXEC_CMD, (char *)frame.buf, dataItem);
			break;
		case C_SE_NB_1://标度化设点命令
			frame.head.type = C_SE_NB_1;
			len = buildCSENB1(EXEC_CMD, (char *)frame.buf, dataItem);
			break;
		case C_SE_NC_1://短浮点数设点命令
			frame.head.type = C_SE_NC_1;
			len = buildCSENC1(EXEC_CMD, (char *)frame.buf, dataItem);
			break;
		case C_SC_TA_1://带长时标，单点命令
			frame.head.type = C_SC_TA_1;
			len = buildCSCTA1(EXEC_CMD, (char *)frame.buf, dataItem);
			break;
		case C_DC_TA_1://带长时标，双点命令
			frame.head.type = C_DC_TA_1;
			len = buildCDCTA1(EXEC_CMD, (char *)frame.buf, dataItem);
			break;
		case C_SE_TA_1://带长时标，归一化设点命令
			frame.head.type = C_SE_TA_1;
			len = buildCSETA1(EXEC_CMD, (char *)frame.buf, dataItem);
			break;
		case C_SE_TB_1://带长时标，标度化设点命令
			frame.head.type = C_SE_TB_1;
			len = buildCSETB1(EXEC_CMD, (char *)frame.buf, dataItem);
			break;
		case C_SE_TC_1://带长时标，短浮点数设点命令
			frame.head.type = C_SE_TC_1;
			len = buildCSETC1(EXEC_CMD, (char *)frame.buf, dataItem);
			break;
		default:
			return -1;
		}
	}else {
		return -1;
	}
	frame.head.length = sizeof(stHead) + len - 2;

	return sizeof(stHead) + len;
}

int Frame::buildUndoCmd(stFrame &frame, int sendICount, int recvICount,
		DataItem &dataItem, TableFile *pIEC104Items)
{
	unsigned short u_sendICount = sendICount;
	unsigned short u_recvICount = recvICount;
	memset(&frame, 0, sizeof(stFrame));
	frame.head.start = 0x68;

	frame.head.control1 = (0xff&(u_sendICount<<1));
	frame.head.control2 = (0xff&(u_sendICount>>7));
	frame.head.control3 = (0xff&(u_recvICount<<1));
	frame.head.control4 = (0xff&(u_recvICount>>7));

	frame.head.vsq = 01;
	frame.head.reason = 0x0006;
	//单元公共地址------COMADDR低字节 = 公共地址；COMADDR高字节 = 装置地址
	frame.head.addr = 0x0001;

	//查询信息体地址
	int objAddr = -1;
	if (getAddrByTermItem(objAddr, dataItem.termId, dataItem.itemId, pIEC104Items))
	{
		frame.head.objAddr.objAddrChar1 = 0xff&((unsigned int)objAddr);
		frame.head.objAddr.objAddrChar2 = 0xff&(((unsigned int)objAddr)>>8);
		frame.head.objAddr.objAddrChar3 = 0xff&(((unsigned int)objAddr)>>16);
	}else {
		return -1;
	}
	int type = 0;
	int len = 0;
	if (dataItem.time == 0)
	{
		type = 5;
	}else {
		type = 6;
	}
	if (getTypeByAddr(type, objAddr, pIEC104Items))
	{
		switch(type)
		{
		case C_SC_NA_1://单点命令
			frame.head.type = C_SC_NA_1;
			len = buildCSCNA1(UNDO_CMD, (char *)frame.buf, dataItem);
			break;
		case C_DC_NA_1://双点命令
			frame.head.type = C_DC_NA_1;
			len = buildCDCNA1(UNDO_CMD, (char *)frame.buf, dataItem);
			break;
		case C_SE_NA_1://归一化设点命令
			frame.head.type = C_SE_NA_1;
			len = buildCSENA1(UNDO_CMD, (char *)frame.buf, dataItem);
			break;
		case C_SE_NB_1://标度化设点命令
			frame.head.type = C_SE_NB_1;
			len = buildCSENB1(UNDO_CMD, (char *)frame.buf, dataItem);
			break;
		case C_SE_NC_1://短浮点数设点命令
			frame.head.type = C_SE_NC_1;
			len = buildCSENC1(UNDO_CMD, (char *)frame.buf, dataItem);
			break;
		case C_SC_TA_1://带长时标，单点命令
			frame.head.type = C_SC_TA_1;
			len = buildCSCTA1(UNDO_CMD, (char *)frame.buf, dataItem);
			break;
		case C_DC_TA_1://带长时标，双点命令
			frame.head.type = C_DC_TA_1;
			len = buildCDCTA1(UNDO_CMD, (char *)frame.buf, dataItem);
			break;
		case C_SE_TA_1://带长时标，归一化设点命令
			frame.head.type = C_SE_TA_1;
			len = buildCSETA1(UNDO_CMD, (char *)frame.buf, dataItem);
			break;
		case C_SE_TB_1://带长时标，标度化设点命令
			frame.head.type = C_SE_TB_1;
			len = buildCSETB1(UNDO_CMD, (char *)frame.buf, dataItem);
			break;
		case C_SE_TC_1://带长时标，短浮点数设点命令
			frame.head.type = C_SE_TC_1;
			len = buildCSETC1(UNDO_CMD, (char *)frame.buf, dataItem);
			break;
		default:
			return -1;
		}
	}else {
		return -1;
	}
	frame.head.length = sizeof(stHead) + len - 2;

	return sizeof(stHead) + len;
}

int Frame::buildCSCNA1(int buildType, char *data, DataItem &dataItem)//单点命令
{
	stC_SC_NA_1 *pst = (stC_SC_NA_1*)data;

	if (buildType == SELECT_CMD)//select cmd
	{
		pst->se_d7 = 1;
	}else if (buildType == EXEC_CMD)//exec cmd
	{
		pst->se_d7 = 0;
	}else if (buildType == UNDO_CMD)//undo cmd
	{
		pst->se_d7 = 1;
	}
	pst->qu_d2_d6 = 0;
	pst->res_d1 = 0;
	pst->scs_d0 = dataItem.value;

	return sizeof(stC_SC_NA_1);
}

int Frame::buildCSCTA1(int buildType, char *data, DataItem &dataItem)//带长时标，单点命令
{
	time_t time = dataItem.time;
	struct tm *pT = localtime(&time);

	unsigned char Y = (pT->tm_year + 1900)%100;
	unsigned char M = pT->tm_mon + 1;
	unsigned char D = pT->tm_mday;
	unsigned char h = pT->tm_hour;
	unsigned char m = pT->tm_min;
	unsigned char s = pT->tm_sec;//秒
	unsigned short millisec = 0;//毫秒

	millisec += 1000*s;

	stC_SC_TA_1 *pst = (stC_SC_TA_1*)data;

	if (buildType == SELECT_CMD)//select cmd
	{
		pst->se_d7 = 1;
	}else if (buildType == EXEC_CMD)//exec cmd
	{
		pst->se_d7 = 0;
	}else if (buildType == UNDO_CMD)//undo cmd
	{
		pst->se_d7 = 1;
	}
	pst->qu_d2_d6 = 0;
	pst->res_d1 = 0;
	pst->scs_d0 = dataItem.value;

	pst->time_stamp.year_res_d7 = 0;
	pst->time_stamp.year_val_d0_d6 = Y;

	pst->time_stamp.month_res_d4_d7 = 0;
	pst->time_stamp.month_val_d0_d3 = M;

	pst->time_stamp.day_d5_d7 = 0;
	pst->time_stamp.day_val_d0_d4 = D;

	pst->time_stamp.hour_su_d7 = 0;
	pst->time_stamp.hour_res_d5_d6 = 0;
	pst->time_stamp.hour_val_d0_d4 = h;

	pst->time_stamp.min_iv_d7 = 0;
	pst->time_stamp.min_val_d0_d5 = m;

	pst->time_stamp.millisec = millisec;

	return sizeof(stC_SC_TA_1);
}

int Frame::buildCDCNA1(int buildType, char *data, DataItem &dataItem)//双点命令
{
	stC_DC_NA_1 *pst = (stC_DC_NA_1*)data;

	if (buildType == SELECT_CMD)//select cmd
	{
		pst->se_d7 = 1;
	}else if (buildType == EXEC_CMD)//exec cmd
	{
		pst->se_d7 = 0;
	}else if (buildType == UNDO_CMD)//undo cmd
	{
		pst->se_d7 = 1;
	}
	pst->qu_d2_d6 = 0;
	pst->dcs_d0_d1 = dataItem.value;

	return sizeof(stC_DC_NA_1);
}

int Frame::buildCDCTA1(int buildType, char *data, DataItem &dataItem)//带长时标，双点命令
{
	time_t time = dataItem.time;
	struct tm *pT = localtime(&time);

	unsigned char Y = (pT->tm_year + 1900)%100;
	unsigned char M = pT->tm_mon + 1;
	unsigned char D = pT->tm_mday;
	unsigned char h = pT->tm_hour;
	unsigned char m = pT->tm_min;
	unsigned char s = pT->tm_sec;//秒
	unsigned short millisec = 0;//毫秒

	millisec += 1000*s;

	stC_DC_TA_1 *pst = (stC_DC_TA_1*)data;

	if (buildType == SELECT_CMD)//select cmd
	{
		pst->se_d7 = 1;
	}else if (buildType == EXEC_CMD)//exec cmd
	{
		pst->se_d7 = 0;
	}else if (buildType == UNDO_CMD)//undo cmd
	{
		pst->se_d7 = 1;
	}
	pst->qu_d2_d6 = 0;
	pst->dcs_d0_d1 = dataItem.value;

	pst->time_stamp.year_res_d7 = 0;
	pst->time_stamp.year_val_d0_d6 = Y;

	pst->time_stamp.month_res_d4_d7 = 0;
	pst->time_stamp.month_val_d0_d3 = M;

	pst->time_stamp.day_d5_d7 = 0;
	pst->time_stamp.day_val_d0_d4 = D;

	pst->time_stamp.hour_su_d7 = 0;
	pst->time_stamp.hour_res_d5_d6 = 0;
	pst->time_stamp.hour_val_d0_d4 = h;

	pst->time_stamp.min_iv_d7 = 0;
	pst->time_stamp.min_val_d0_d5 = m;

	pst->time_stamp.millisec = millisec;

	return sizeof(stC_DC_TA_1);
}

int Frame::buildCSENA1(int buildType, char *data, DataItem &dataItem)//归一化设点命令
{
	stC_SE_NA_1 *pst = (stC_SE_NA_1*)data;

	if (buildType == SELECT_CMD)//select cmd
	{
		pst->qos.se_d7 = 1;
	}else if (buildType == EXEC_CMD)//exec cmd
	{
		pst->qos.se_d7 = 0;
	}else if (buildType == UNDO_CMD)//undo cmd
	{
		pst->qos.se_d7 = 1;
	}
	pst->qos.ql_d0_d6 = 0;
	pst->val = dataItem.value;

	return sizeof(stC_SE_NA_1);
}

int Frame::buildCSETA1(int buildType, char *data, DataItem &dataItem)//带长时标，归一化设点命令
{
	time_t time = dataItem.time;
	struct tm *pT = localtime(&time);

	unsigned char Y = (pT->tm_year + 1900)%100;
	unsigned char M = pT->tm_mon + 1;
	unsigned char D = pT->tm_mday;
	unsigned char h = pT->tm_hour;
	unsigned char m = pT->tm_min;
	unsigned char s = pT->tm_sec;//秒
	unsigned short millisec = 0;//毫秒

	millisec += 1000*s;

	stC_SE_TA_1 *pst = (stC_SE_TA_1*)data;

	if (buildType == SELECT_CMD)//select cmd
	{
		pst->qos.se_d7 = 1;
	}else if (buildType == EXEC_CMD)//exec cmd
	{
		pst->qos.se_d7 = 0;
	}else if (buildType == UNDO_CMD)//undo cmd
	{
		pst->qos.se_d7 = 1;
	}
	pst->qos.ql_d0_d6 = 0;
	pst->val = dataItem.value;

	pst->time_stamp.year_res_d7 = 0;
	pst->time_stamp.year_val_d0_d6 = Y;

	pst->time_stamp.month_res_d4_d7 = 0;
	pst->time_stamp.month_val_d0_d3 = M;

	pst->time_stamp.day_d5_d7 = 0;
	pst->time_stamp.day_val_d0_d4 = D;

	pst->time_stamp.hour_su_d7 = 0;
	pst->time_stamp.hour_res_d5_d6 = 0;
	pst->time_stamp.hour_val_d0_d4 = h;

	pst->time_stamp.min_iv_d7 = 0;
	pst->time_stamp.min_val_d0_d5 = m;

	pst->time_stamp.millisec = millisec;

	return sizeof(stC_SE_TA_1);
}

int Frame::buildCSENB1(int buildType, char *data, DataItem &dataItem)//标度化设点命令
{
	stC_SE_NB_1 *pst = (stC_SE_NB_1*)data;

	if (buildType == SELECT_CMD)//select cmd
	{
		pst->qos.se_d7 = 1;
	}else if (buildType == EXEC_CMD)//exec cmd
	{
		pst->qos.se_d7 = 0;
	}else if (buildType == UNDO_CMD)//undo cmd
	{
		pst->qos.se_d7 = 1;
	}
	pst->qos.ql_d0_d6 = 0;
	pst->val = dataItem.value;

	return sizeof(stC_SE_NB_1);
}

int Frame::buildCSETB1(int buildType, char *data, DataItem &dataItem)//带长时标，标度化设点命令
{
	time_t time = dataItem.time;
	struct tm *pT = localtime(&time);

	unsigned char Y = (pT->tm_year + 1900)%100;
	unsigned char M = pT->tm_mon + 1;
	unsigned char D = pT->tm_mday;
	unsigned char h = pT->tm_hour;
	unsigned char m = pT->tm_min;
	unsigned char s = pT->tm_sec;//秒
	unsigned short millisec = 0;//毫秒

	millisec += 1000*s;

	stC_SE_TB_1 *pst = (stC_SE_TB_1*)data;

	if (buildType == SELECT_CMD)//select cmd
	{
		pst->qos.se_d7 = 1;
	}else if (buildType == EXEC_CMD)//exec cmd
	{
		pst->qos.se_d7 = 0;
	}else if (buildType == UNDO_CMD)//undo cmd
	{
		pst->qos.se_d7 = 1;
	}
	pst->qos.ql_d0_d6 = 0;
	pst->val = dataItem.value;

	pst->time_stamp.year_res_d7 = 0;
	pst->time_stamp.year_val_d0_d6 = Y;

	pst->time_stamp.month_res_d4_d7 = 0;
	pst->time_stamp.month_val_d0_d3 = M;

	pst->time_stamp.day_d5_d7 = 0;
	pst->time_stamp.day_val_d0_d4 = D;

	pst->time_stamp.hour_su_d7 = 0;
	pst->time_stamp.hour_res_d5_d6 = 0;
	pst->time_stamp.hour_val_d0_d4 = h;

	pst->time_stamp.min_iv_d7 = 0;
	pst->time_stamp.min_val_d0_d5 = m;

	pst->time_stamp.millisec = millisec;

	return sizeof(stC_SE_TB_1);
}

int Frame::buildCSENC1(int buildType, char *data, DataItem &dataItem)//短浮点数设点命令
{
	stC_SE_NC_1 *pst = (stC_SE_NC_1*)data;

	if (buildType == SELECT_CMD)//select cmd
	{
		pst->qos.se_d7 = 1;
	}else if (buildType == EXEC_CMD)//exec cmd
	{
		pst->qos.se_d7 = 0;
	}else if (buildType == UNDO_CMD)//undo cmd
	{
		pst->qos.se_d7 = 1;
	}
	pst->qos.ql_d0_d6 = 0;
	pst->val = dataItem.value;

	return sizeof(stC_SE_NC_1);
}

int Frame::buildCSETC1(int buildType, char *data, DataItem &dataItem)//带长时标，短浮点数设点命令
{
	time_t time = dataItem.time;
	struct tm *pT = localtime(&time);

	unsigned char Y = (pT->tm_year + 1900)%100;
	unsigned char M = pT->tm_mon + 1;
	unsigned char D = pT->tm_mday;
	unsigned char h = pT->tm_hour;
	unsigned char m = pT->tm_min;
	unsigned char s = pT->tm_sec;//秒
	unsigned short millisec = 0;//毫秒

	millisec += 1000*s;

	stC_SE_TC_1 *pst = (stC_SE_TC_1*)data;

	if (buildType == SELECT_CMD)//select cmd
	{
		pst->qos.se_d7 = 1;
	}else if (buildType == EXEC_CMD)//exec cmd
	{
		pst->qos.se_d7 = 0;
	}else if (buildType == UNDO_CMD)//undo cmd
	{
		pst->qos.se_d7 = 1;
	}
	pst->qos.ql_d0_d6 = 0;
	pst->val = dataItem.value;

	pst->time_stamp.year_res_d7 = 0;
	pst->time_stamp.year_val_d0_d6 = Y;

	pst->time_stamp.month_res_d4_d7 = 0;
	pst->time_stamp.month_val_d0_d3 = M;

	pst->time_stamp.day_d5_d7 = 0;
	pst->time_stamp.day_val_d0_d4 = D;

	pst->time_stamp.hour_su_d7 = 0;
	pst->time_stamp.hour_res_d5_d6 = 0;
	pst->time_stamp.hour_val_d0_d4 = h;

	pst->time_stamp.min_iv_d7 = 0;
	pst->time_stamp.min_val_d0_d5 = m;

	pst->time_stamp.millisec = millisec;

	return sizeof(stC_SE_TC_1);
}

void Frame::DecodeCmd(DataItem &dataItem, int &status,
		const char *pFrame, int length, TableFile *pIEC104Items)
{
	int type = getType(pFrame, length);

	switch(type)
	{
	case C_SC_NA_1://单点命令
		DecodeCSCNA1(dataItem, status, pFrame, length, pIEC104Items);
		break;
	case C_DC_NA_1://双点命令
		DecodeCDCNA1(dataItem, status, pFrame, length, pIEC104Items);
		break;
	case C_SE_NA_1://归一化设点命令
		DecodeCSENA1(dataItem, status, pFrame, length, pIEC104Items);
		break;
	case C_SE_NB_1://标度化设点命令
		DecodeCSENB1(dataItem, status, pFrame, length, pIEC104Items);
		break;
	case C_SE_NC_1://短浮点数设点命令
		DecodeCSENC1(dataItem, status, pFrame, length, pIEC104Items);
		break;
	case C_SC_TA_1://带长时标，单点命令
		DecodeCSCTA1(dataItem, status, pFrame, length, pIEC104Items);
		break;
	case C_DC_TA_1://带长时标，双点命令
		DecodeCDCTA1(dataItem, status, pFrame, length, pIEC104Items);
		break;
	case C_SE_TA_1://带长时标，归一化设点命令
		DecodeCSETA1(dataItem, status, pFrame, length, pIEC104Items);
		break;
	case C_SE_TB_1://带长时标，标度化设点命令
		DecodeCSETB1(dataItem, status, pFrame, length, pIEC104Items);
		break;
	case C_SE_TC_1://带长时标，短浮点数设点命令
		DecodeCSETC1(dataItem, status, pFrame, length, pIEC104Items);
		break;
	default:
		break;
	}
}

void Frame::DecodeCSCNA1(DataItem &dataItem, int &status,
		const char *pFrame, int length, TableFile *pIEC104Items)//单点命令
{
	stFrame frame;
	memcpy(&frame, pFrame, sizeof(frame));

	int reason = getReason(pFrame, length);
	int objAddr = frame.head.objAddr.objAddrChar3 << 16 | frame.head.objAddr.objAddrChar2 << 8 | frame.head.objAddr.objAddrChar1;
	stC_SC_NA_1* pst = reinterpret_cast<stC_SC_NA_1*>(frame.buf);
	if (reason == REASON_ACT_CON && pst->se_d7 == 1)
	{
		status = SELECT_CMD;
	}else if (reason == REASON_DEACT && pst->se_d7 == 1)
	{
		status = UNDO_CMD;
	}else if (reason == REASON_ACT_CON && pst->se_d7 == 0)
	{
		status = EXEC_CMD;
	}

	getTermItemByAddr(dataItem.termId, dataItem.itemId, objAddr, pIEC104Items);
	dataItem.value = pst->val;
}

void Frame::DecodeCSCTA1(DataItem &dataItem, int &status,
		const char *pFrame, int length, TableFile *pIEC104Items)//带长时标，单点命令
{
	stFrame frame;
	memcpy(&frame, pFrame, sizeof(frame));

	int reason = getReason(pFrame, length);
	int objAddr = frame.head.objAddr.objAddrChar3 << 16 | frame.head.objAddr.objAddrChar2 << 8 | frame.head.objAddr.objAddrChar1;
	stC_SC_TA_1* pst = reinterpret_cast<stC_SC_TA_1*>(frame.buf);
	if (reason == REASON_ACT_CON && pst->se_d7 == 1)
	{
		status = SELECT_CMD;
	}else if (reason == REASON_DEACT && pst->se_d7 == 1)
	{
		status = UNDO_CMD;
	}else if (reason == REASON_ACT_CON && pst->se_d7 == 0)
	{
		status = EXEC_CMD;
	}

	getTermItemByAddr(dataItem.termId, dataItem.itemId, objAddr, pIEC104Items);
	dataItem.value = pst->val;

	if (!pst->time_stamp.min_iv_d7 &&
			!pst->time_stamp.hour_su_d7)
	{
		struct tm t;
		memset(&t, 0, sizeof(t));
		t.tm_year = pst->time_stamp.year_val_d0_d6 - 1900 + 2000;
		t.tm_mon = pst->time_stamp.month_val_d0_d3 - 1;
		t.tm_mday = pst->time_stamp.day_val_d0_d4;
		t.tm_hour = pst->time_stamp.hour_val_d0_d4;
		t.tm_min = pst->time_stamp.min_val_d0_d5;
		t.tm_sec = pst->time_stamp.millisec/1000;
		dataItem.time = timelocal(&t);
	}else {
		dataItem.time = 0;
	}
}

void Frame::DecodeCDCNA1(DataItem &dataItem, int &status,
		const char *pFrame, int length, TableFile *pIEC104Items)//双点命令
{
#if 1
	stFrame frame;
	memcpy(&frame, pFrame, sizeof(frame));

	int reason = getReason(pFrame, length);
	int objAddr = frame.head.objAddr.objAddrChar3 << 16 | frame.head.objAddr.objAddrChar2 << 8 | frame.head.objAddr.objAddrChar1;
	stC_DC_NA_1* pst = reinterpret_cast<stC_DC_NA_1*>(frame.buf);
	if (reason == REASON_ACT_CON && pst->se_d7 == 1)
	{
		status = SELECT_CMD;
	}else if (reason == REASON_DEACT && pst->se_d7 == 1)
	{
		status = UNDO_CMD;
	}else if (reason == REASON_ACT_CON && pst->se_d7 == 0)
	{
		status = EXEC_CMD;
	}

	getTermItemByAddr(dataItem.termId, dataItem.itemId, objAddr, pIEC104Items);
	dataItem.value = pst->val;
#endif
}

void Frame::DecodeCDCTA1(DataItem &dataItem, int &status,
		const char *pFrame, int length, TableFile *pIEC104Items)//带长时标，双点命令
{
#if 1
	stFrame frame;
	memcpy(&frame, pFrame, sizeof(frame));

	int reason = getReason(pFrame, length);
	int objAddr = frame.head.objAddr.objAddrChar3 << 16 | frame.head.objAddr.objAddrChar2 << 8 | frame.head.objAddr.objAddrChar1;
	stC_DC_TA_1* pst = reinterpret_cast<stC_DC_TA_1*>(frame.buf);
	if (reason == REASON_ACT_CON && pst->se_d7 == 1)
	{
		status = SELECT_CMD;
	}else if (reason == REASON_DEACT && pst->se_d7 == 1)
	{
		status = UNDO_CMD;
	}else if (reason == REASON_ACT_CON && pst->se_d7 == 0)
	{
		status = EXEC_CMD;
	}

	getTermItemByAddr(dataItem.termId, dataItem.itemId, objAddr, pIEC104Items);
	dataItem.value = pst->val;

	if (!pst->time_stamp.min_iv_d7 &&
			!pst->time_stamp.hour_su_d7)
	{
		struct tm t;
		memset(&t, 0, sizeof(t));
		t.tm_year = pst->time_stamp.year_val_d0_d6 - 1900 + 2000;
		t.tm_mon = pst->time_stamp.month_val_d0_d3 - 1;
		t.tm_mday = pst->time_stamp.day_val_d0_d4;
		t.tm_hour = pst->time_stamp.hour_val_d0_d4;
		t.tm_min = pst->time_stamp.min_val_d0_d5;
		t.tm_sec = pst->time_stamp.millisec/1000;
		dataItem.time = timelocal(&t);
	}else {
		dataItem.time = 0;
	}
#endif
}

void Frame::DecodeCSENA1(DataItem &dataItem, int &status,
		const char *pFrame, int length, TableFile *pIEC104Items)//归一化设点命令
{
#if 1
	stFrame frame;
	memcpy(&frame, pFrame, sizeof(frame));

	int reason = getReason(pFrame, length);
	int objAddr = frame.head.objAddr.objAddrChar3 << 16 | frame.head.objAddr.objAddrChar2 << 8 | frame.head.objAddr.objAddrChar1;
	stC_SE_NA_1* pst = reinterpret_cast<stC_SE_NA_1*>(frame.buf);
	if (reason == REASON_ACT_CON && pst->qos.se_d7 == 1)
	{
		status = SELECT_CMD;
	}else if (reason == REASON_DEACT && pst->qos.se_d7 == 1)
	{
		status = UNDO_CMD;
	}else if (reason == REASON_ACT_CON && pst->qos.se_d7 == 0)
	{
		status = EXEC_CMD;
	}

	getTermItemByAddr(dataItem.termId, dataItem.itemId, objAddr, pIEC104Items);
	dataItem.value = pst->val;
#endif
}

void Frame::DecodeCSETA1(DataItem &dataItem, int &status,
		const char *pFrame, int length, TableFile *pIEC104Items)//带长时标，归一化设点命令
{
#if 1
	stFrame frame;
	memcpy(&frame, pFrame, sizeof(frame));

	int reason = getReason(pFrame, length);
	int objAddr = frame.head.objAddr.objAddrChar3 << 16 | frame.head.objAddr.objAddrChar2 << 8 | frame.head.objAddr.objAddrChar1;
	stC_SE_TA_1* pst = reinterpret_cast<stC_SE_TA_1*>(frame.buf);
	if (reason == REASON_ACT_CON && pst->qos.se_d7 == 1)
	{
		status = SELECT_CMD;
	}else if (reason == REASON_DEACT && pst->qos.se_d7 == 1)
	{
		status = UNDO_CMD;
	}else if (reason == REASON_ACT_CON && pst->qos.se_d7 == 0)
	{
		status = EXEC_CMD;
	}

	getTermItemByAddr(dataItem.termId, dataItem.itemId, objAddr, pIEC104Items);
	dataItem.value = pst->val;

	if (!pst->time_stamp.min_iv_d7 &&
			!pst->time_stamp.hour_su_d7)
	{
		struct tm t;
		memset(&t, 0, sizeof(t));
		t.tm_year = pst->time_stamp.year_val_d0_d6 - 1900 + 2000;
		t.tm_mon = pst->time_stamp.month_val_d0_d3 - 1;
		t.tm_mday = pst->time_stamp.day_val_d0_d4;
		t.tm_hour = pst->time_stamp.hour_val_d0_d4;
		t.tm_min = pst->time_stamp.min_val_d0_d5;
		t.tm_sec = pst->time_stamp.millisec/1000;
		dataItem.time = timelocal(&t);
	}else {
		dataItem.time = 0;
	}
#endif
}

void Frame::DecodeCSENB1(DataItem &dataItem, int &status,
		const char *pFrame, int length, TableFile *pIEC104Items)//标度化设点命令
{
#if 1
	stFrame frame;
	memcpy(&frame, pFrame, sizeof(frame));

	int reason = getReason(pFrame, length);
	int objAddr = frame.head.objAddr.objAddrChar3 << 16 | frame.head.objAddr.objAddrChar2 << 8 | frame.head.objAddr.objAddrChar1;
	stC_SE_NB_1* pst = reinterpret_cast<stC_SE_NB_1*>(frame.buf);
	if (reason == REASON_ACT_CON && pst->qos.se_d7 == 1)
	{
		status = SELECT_CMD;
	}else if (reason == REASON_DEACT && pst->qos.se_d7 == 1)
	{
		status = UNDO_CMD;
	}else if (reason == REASON_ACT_CON && pst->qos.se_d7 == 0)
	{
		status = EXEC_CMD;
	}

	getTermItemByAddr(dataItem.termId, dataItem.itemId, objAddr, pIEC104Items);
	dataItem.value = pst->val;
#endif
}

void Frame::DecodeCSETB1(DataItem &dataItem, int &status,
		const char *pFrame, int length, TableFile *pIEC104Items)//带长时标，标度化设点命令
{
#if 1
	stFrame frame;
	memcpy(&frame, pFrame, sizeof(frame));

	int reason = getReason(pFrame, length);
	int objAddr = frame.head.objAddr.objAddrChar3 << 16 | frame.head.objAddr.objAddrChar2 << 8 | frame.head.objAddr.objAddrChar1;
	stC_SE_TB_1* pst = reinterpret_cast<stC_SE_TB_1*>(frame.buf);
	if (reason == REASON_ACT_CON && pst->qos.se_d7 == 1)
	{
		status = SELECT_CMD;
	}else if (reason == REASON_DEACT && pst->qos.se_d7 == 1)
	{
		status = UNDO_CMD;
	}else if (reason == REASON_ACT_CON && pst->qos.se_d7 == 0)
	{
		status = EXEC_CMD;
	}

	getTermItemByAddr(dataItem.termId, dataItem.itemId, objAddr, pIEC104Items);
	dataItem.value = pst->val;

	if (!pst->time_stamp.min_iv_d7 &&
			!pst->time_stamp.hour_su_d7)
	{
		struct tm t;
		memset(&t, 0, sizeof(t));
		t.tm_year = pst->time_stamp.year_val_d0_d6 - 1900 + 2000;
		t.tm_mon = pst->time_stamp.month_val_d0_d3 - 1;
		t.tm_mday = pst->time_stamp.day_val_d0_d4;
		t.tm_hour = pst->time_stamp.hour_val_d0_d4;
		t.tm_min = pst->time_stamp.min_val_d0_d5;
		t.tm_sec = pst->time_stamp.millisec/1000;
		dataItem.time = timelocal(&t);
	}else {
		dataItem.time = 0;
	}
#endif
}

void Frame::DecodeCSENC1(DataItem &dataItem, int &status,
		const char *pFrame, int length, TableFile *pIEC104Items)//短浮点数设点命令
{
#if 1
	stFrame frame;
	memcpy(&frame, pFrame, sizeof(frame));

	int reason = getReason(pFrame, length);
	int objAddr = frame.head.objAddr.objAddrChar3 << 16 | frame.head.objAddr.objAddrChar2 << 8 | frame.head.objAddr.objAddrChar1;
	stC_SE_NC_1* pst = reinterpret_cast<stC_SE_NC_1*>(frame.buf);
	if (reason == REASON_ACT_CON && pst->qos.se_d7 == 1)
	{
		status = SELECT_CMD;
	}else if (reason == REASON_DEACT && pst->qos.se_d7 == 1)
	{
		status = UNDO_CMD;
	}else if (reason == REASON_ACT_CON && pst->qos.se_d7 == 0)
	{
		status = EXEC_CMD;
	}

	getTermItemByAddr(dataItem.termId, dataItem.itemId, objAddr, pIEC104Items);
	dataItem.value = pst->val;
#endif
}

void Frame::DecodeCSETC1(DataItem &dataItem, int &status,
		const char *pFrame, int length, TableFile *pIEC104Items)//带长时标，短浮点数设点命令
{
#if 1
	stFrame frame;
	memcpy(&frame, pFrame, sizeof(frame));

	int reason = getReason(pFrame, length);
	int objAddr = frame.head.objAddr.objAddrChar3 << 16 | frame.head.objAddr.objAddrChar2 << 8 | frame.head.objAddr.objAddrChar1;
	stC_SE_TC_1* pst = reinterpret_cast<stC_SE_TC_1*>(frame.buf);
	if (reason == REASON_ACT_CON && pst->qos.se_d7 == 1)
	{
		status = SELECT_CMD;
	}else if (reason == REASON_DEACT && pst->qos.se_d7 == 1)
	{
		status = UNDO_CMD;
	}else if (reason == REASON_ACT_CON && pst->qos.se_d7 == 0)
	{
		status = EXEC_CMD;
	}

	getTermItemByAddr(dataItem.termId, dataItem.itemId, objAddr, pIEC104Items);
	dataItem.value = pst->val;

	if (!pst->time_stamp.min_iv_d7 &&
			!pst->time_stamp.hour_su_d7)
	{
		struct tm t;
		memset(&t, 0, sizeof(t));
		t.tm_year = pst->time_stamp.year_val_d0_d6 - 1900 + 2000;
		t.tm_mon = pst->time_stamp.month_val_d0_d3 - 1;
		t.tm_mday = pst->time_stamp.day_val_d0_d4;
		t.tm_hour = pst->time_stamp.hour_val_d0_d4;
		t.tm_min = pst->time_stamp.min_val_d0_d5;
		t.tm_sec = pst->time_stamp.millisec/1000;
		dataItem.time = timelocal(&t);
	}else {
		dataItem.time = 0;
	}
#endif
}

bool Frame::getTermItemByAddr(int &termId, int &itemId, int addr, TableFile *pIEC104Items)
{
	map<int, int> value;
	map<int, int> result;

	value.insert(pair<int, int>(0, addr));

	result.insert(pair<int, int>(1, 0));
	result.insert(pair<int, int>(2, 0));
	if (getResultByValue(value, result, pIEC104Items))
	{
		termId = result[1];
		itemId = result[2];
		return true;
	}else {
		termId = 0;
		itemId = 0;
		return false;
	}
}

bool Frame::getAddrByTermItem(int &addr, int termId, int itemId, TableFile *pIEC104Items)
{
	map<int, int> value;
	map<int, int> result;

	value.insert(pair<int, int>(3, termId));
	value.insert(pair<int, int>(4, itemId));

	result.insert(pair<int, int>(0, 0));

	if (getResultByValue(value, result, pIEC104Items))
	{
		addr = result[0];
		return true;
	}else {
		addr = 0;
		return false;
	}
}

bool Frame::getTypeByAddr(int &type, int addr, TableFile *pIEC104Items)
{
	int column = type;
	map<int, int> value;
	map<int, int> result;

	value.insert(pair<int, int>(0, addr));

	result.insert(pair<int, int>(column, 0));

	if (getResultByValue(value, result, pIEC104Items))
	{
		type = result[column];
		return true;
	}else {
		type = 0;
		return false;
	}
}

bool Frame::getResultByValue(map<int, int> &value, map<int, int> &result, TableFile *pIEC104Items)
{
	std::map<int, int>::iterator itValue;
	std::map<int, int>::iterator itResult;

	vector<int> rows;
	vector< map<int, int> > mapList;
	int index = 0;
	int sizeMin = 0;
	int indexMin = 0;

	for (itValue = value.begin(); itValue != value.end(); ++itValue)
	{
		std::map<int, int> pos;
		char tmp[10] = {0};

		sprintf(tmp, "%06X", itValue->second);
		string value_ = string(tmp);

		if (pIEC104Items->find(value_, pos, -1, itValue->first))
		{
			if (sizeMin > (int)pos.size() && (int)pos.size() > 0)
			{
				mapList.push_back(pos);
				sizeMin = pos.size();
				indexMin = index;
				++index;
			}else if (pos.size() > 0){
				mapList.push_back(pos);
				if (itValue == value.begin())
				{
					sizeMin = pos.size();
					indexMin = index;
				}
				++index;
			}
		}
	}

	if ((int)mapList.size() <= indexMin)
	{
		return false;
	}
	for (itValue = mapList[indexMin].begin(); itValue != mapList[indexMin].end(); ++itValue)
	{
		bool flag = true;
		for (int i = 0; i < (int)mapList.size(); ++i)
		{
			if (mapList[i].find(itValue->first) == mapList[i].end())
			{
				flag = false;
				break;
			}
		}
		if (flag)
		{
			rows.push_back(itValue->first);
		}
	}

	if ((int)rows.size() == 1)
	{
		for (itResult = result.begin(); itResult != result.end(); ++itResult)
		{
			sscanf(pIEC104Items->value(rows[0], itResult->first).c_str(), "%X", &itResult->second);
		}
		return true;
	}

	return false;
}

void Frame::Decode(vector<DataItem> &dataItems,
		const char *pFrame, int length, TableFile *pIEC104Items)
{
	int type = getType(pFrame, length);

	switch (type)
	{
	case M_SP_NA_1://单点信息
		DecodeMSPNA1(dataItems, pFrame, length, pIEC104Items);
		break;
	case M_DP_NA_1://双点信息
		DecodeMDPNA1(dataItems, pFrame, length, pIEC104Items);
		break;
	case M_ME_NA_1://测量值，规一化值
		DecodeMMENA1(dataItems, pFrame, length, pIEC104Items);
		break;
	case M_ME_NB_1://测量值，标度化值
		DecodeMMENB1(dataItems, pFrame, length, pIEC104Items);
		break;
	case M_ME_NC_1://测量值，短浮点数
		DecodeMMENC1(dataItems, pFrame, length, pIEC104Items);
		break;
	case M_ME_ND_1://测量值，规一化值，不带品质描述
		DecodeMMEND1(dataItems, pFrame, length, pIEC104Items);
		break;
	case M_IT_NA_1://累计量
		DecodeMITNA1(dataItems, pFrame, length, pIEC104Items);
		break;
	case M_SP_TB_1://带长时标，单点信息
		DecodeMSPTB1(dataItems, pFrame, length, pIEC104Items);
		break;
	case M_DP_TB_1://带长时标，双点信息
		DecodeMDPTB1(dataItems, pFrame, length, pIEC104Items);
		break;
	case M_ME_TF_1://带长时标，短浮点数
		DecodeMMETF1(dataItems, pFrame, length, pIEC104Items);
		break;
	case M_EP_TD_1://带长时标的继电保护装置事件
		DecodeMEPTD1(dataItems, pFrame, length, pIEC104Items);
	default:
		break;
	}
}

void Frame::DecodeMSPNA1(vector<DataItem> &dataItems,
		const char* pFrame, int length, TableFile *pIEC104Items)//解析单点信息
{
	std::cout<<"解析单点信息"<<std::endl;
	stFrame frame;
	memcpy(&frame, pFrame, sizeof(frame));

	time_t rawtime;
	time(&rawtime);

	int objAddr = 0;
	int itemCount = frame.head.itemCount_D0_D6;
	int vsq = frame.head.vsq_D7;

//	printf("Head length:%d\n", sizeof(stHead));
	printf("连续:%d,信息体数量:%d\n", vsq, itemCount);

	if (vsq == 0)	//信息体地址非连续
	{
		stM_SP_NA_1* pst = reinterpret_cast<stM_SP_NA_1*>(frame.buf - LENGTH_OBJADDR);
		for(int i = 0; i < itemCount; i++)
		{
			DataItem dataItem;

			objAddr = pst[i].objAddr.objAddrChar3 << 16 | pst[i].objAddr.objAddrChar2 << 8 | pst[i].objAddr.objAddrChar1;

			getTermItemByAddr(dataItem.termId, dataItem.itemId, objAddr, pIEC104Items);
			dataItem.time = rawtime;
			dataItem.value = pst[i].spi_d0;

			dataItems.push_back(dataItem);
			printf("[序号%d], [地址:%06X], [termID:%06X], [itemID:%06X], [值:%lf]"
					":IV:%d,NT:%d,SB:%d,BL:%d,SPI:%d\n",
					i+1, objAddr, dataItem.termId, dataItem.itemId, dataItem.value,
					pst[i].iv_d7, pst[i].nt_d6, pst[i].sb_d5, pst[i].bl_d4, pst[i].spi_d0);
		}
	}
	else if (vsq == 1)	//信息体地址连续
	{
		stM_SP_NA_1_SERIAL* pst = reinterpret_cast<stM_SP_NA_1_SERIAL*>(frame.buf);
		objAddr = frame.head.objAddr.objAddrChar3 << 16 | frame.head.objAddr.objAddrChar2 << 8 | frame.head.objAddr.objAddrChar1;
		for(int i = 0; i < itemCount; i++)
		{
			DataItem dataItem;

			int addr = objAddr+i;
			getTermItemByAddr(dataItem.termId, dataItem.itemId, addr, pIEC104Items);
			dataItem.time = rawtime;
			dataItem.value = pst[i].spi_d0;

			dataItems.push_back(dataItem);

			printf("[序号%d], [地址:%06X], [termID:%06X], [itemID:%06X], [值:%lf]"
					":IV:%d,NT:%d,SB:%d,BL:%d,SPI:%d\n",
					i+1, objAddr+i, dataItem.termId, dataItem.itemId, dataItem.value,
					pst[i].iv_d7, pst[i].nt_d6, pst[i].sb_d5, pst[i].bl_d4, pst[i].spi_d0);
		}
	}
}

void Frame::DecodeMDPNA1(vector<DataItem> &dataItems,
		const char* pFrame, int length, TableFile *pIEC104Items)//解析双点信息
{
	std::cout<<"解析双点信息"<<std::endl;
#if 1
	stFrame frame;
	memcpy(&frame, pFrame, sizeof(frame));

	time_t rawtime;
	time(&rawtime);

	int objAddr = 0;
	int itemCount = frame.head.itemCount_D0_D6;
	int vsq = frame.head.vsq_D7;

//	printf("Head length:%d\n", sizeof(stHead));
	printf("连续:%d,信息体数量:%d\n", vsq, itemCount);

	if (vsq == 0)	//信息体地址非连续
	{
		stM_DP_NA_1* pst = reinterpret_cast<stM_DP_NA_1*>(frame.buf - LENGTH_OBJADDR);
		for(int i = 0; i < itemCount; i++)
		{
			DataItem dataItem;

			objAddr = pst[i].objAddr.objAddrChar3 << 16 | pst[i].objAddr.objAddrChar2 << 8 | pst[i].objAddr.objAddrChar1;

			getTermItemByAddr(dataItem.termId, dataItem.itemId, objAddr, pIEC104Items);
			dataItem.time = rawtime;
			dataItem.value = pst[i].dpi_d0_d1;

			dataItems.push_back(dataItem);
			printf("[序号%d], [地址:%06X], [termID:%06X], [itemID:%06X], [值:%lf]"
					":IV:%d,NT:%d,SB:%d,BL:%d,DPI:%d\n",
					i+1, objAddr, dataItem.termId, dataItem.itemId, dataItem.value,
					pst[i].iv_d7, pst[i].nt_d6, pst[i].sb_d5, pst[i].bl_d4, pst[i].dpi_d0_d1);
		}
	}
	else if (vsq == 1 )	//信息体地址连续
	{
		stM_DP_NA_1_SERIAL* pst = reinterpret_cast<stM_DP_NA_1_SERIAL*>(frame.buf);
		objAddr = frame.head.objAddr.objAddrChar3 << 16 | frame.head.objAddr.objAddrChar2 << 8 | frame.head.objAddr.objAddrChar1;
		for(int i = 0; i < itemCount; i++)
		{
			DataItem dataItem;
			int addr = objAddr+i;

			getTermItemByAddr(dataItem.termId, dataItem.itemId, addr, pIEC104Items);
			dataItem.time = rawtime;
			dataItem.value = pst[i].dpi_d0_d1;

			dataItems.push_back(dataItem);

			printf("[序号%d], [地址:%06X], [termID:%06X], [itemID:%06X], [值:%lf]"
					":IV:%d,NT:%d,SB:%d,BL:%d,DPI:%d\n",
					i+1, objAddr+i, dataItem.termId, dataItem.itemId, dataItem.value,
					pst[i].iv_d7, pst[i].nt_d6, pst[i].sb_d5, pst[i].bl_d4, pst[i].dpi_d0_d1);
		}
	}
#endif
}

void Frame::DecodeMMENA1(vector<DataItem> &dataItems,
		const char* pFrame, int length, TableFile *pIEC104Items)//解析测量值，规一化值，带品质描述
{
	std::cout<<"解析测量值，规一化值，带品质描述"<<std::endl;
	stFrame frame;
	memcpy(&frame, pFrame, sizeof(frame));

	time_t rawtime;
	time(&rawtime);

	int objAddr = 0;
	int itemCount = frame.head.itemCount_D0_D6;
	int vsq = frame.head.vsq_D7;

//	printf("Head length:%d\n", sizeof(stHead));
	printf("连续:%d,信息体数量:%d\n", vsq, itemCount);

	if (vsq == 0)	//信息体地址非连续
	{
		stM_ME_NA_1* pst = reinterpret_cast<stM_ME_NA_1*>(frame.buf - LENGTH_OBJADDR);
		for(int i = 0; i < itemCount; i++)
		{
			DataItem dataItem;

			objAddr = pst[i].objAddr.objAddrChar3 << 16 | pst[i].objAddr.objAddrChar2 << 8 | pst[i].objAddr.objAddrChar1;

			getTermItemByAddr(dataItem.termId, dataItem.itemId, objAddr, pIEC104Items);
			dataItem.time = rawtime;
			dataItem.value = pst[i].val;

			dataItems.push_back(dataItem);
			printf("[序号%d], [地址:%06X], [termID:%06X], [itemID:%06X], [值:%lf]"
					":IV:%d,NT:%d,SB:%d,BL:%d,OV:%d\n",
					i+1, objAddr, dataItem.termId, dataItem.itemId, dataItem.value,
					pst[i].quality_des.iv_d7, pst[i].quality_des.nt_d6, pst[i].quality_des.sb_d5, pst[i].quality_des.bl_d4, pst[i].quality_des.ov_d0);
		}
	}
	else if (vsq == 1 )	//信息体地址连续
	{
		stM_ME_NA_1_SERIAL* pst = reinterpret_cast<stM_ME_NA_1_SERIAL*>(frame.buf);
		objAddr = frame.head.objAddr.objAddrChar3 << 16 | frame.head.objAddr.objAddrChar2 << 8 | frame.head.objAddr.objAddrChar1;
		for(int i = 0; i < itemCount; i++)
		{
			DataItem dataItem;
			int addr = objAddr+i;

			getTermItemByAddr(dataItem.termId, dataItem.itemId, addr, pIEC104Items);
			dataItem.time = rawtime;
			dataItem.value = pst[i].val;

			dataItems.push_back(dataItem);

			printf("[序号%d], [地址:%06X], [termID:%06X], [itemID:%06X], [值:%lf]"
					":IV:%d,NT:%d,SB:%d,BL:%d,OV:%d\n",
					i+1, objAddr+i, dataItem.termId, dataItem.itemId, dataItem.value,
					pst[i].quality_des.iv_d7, pst[i].quality_des.nt_d6, pst[i].quality_des.sb_d5, pst[i].quality_des.bl_d4, pst[i].quality_des.ov_d0);
		}
	}
}

void Frame::DecodeMMENB1(vector<DataItem> &dataItems,
		const char* pFrame, int length, TableFile *pIEC104Items)//解析测量值，标度化值
{
	std::cout<<"解析测量值，标度化值"<<std::endl;
	stFrame frame;
	memcpy(&frame, pFrame, sizeof(frame));

	time_t rawtime;
	time(&rawtime);

	int objAddr = 0;
	int itemCount = frame.head.itemCount_D0_D6;
	int vsq = frame.head.vsq_D7;

//	printf("Head length:%d\n", sizeof(stHead));
	printf("连续:%d,信息体数量:%d\n", vsq, itemCount);

	if (vsq == 0)	//信息体地址非连续
	{
		stM_ME_NB_1* pst = reinterpret_cast<stM_ME_NB_1*>(frame.buf - LENGTH_OBJADDR);
		for(int i = 0; i < itemCount; i++)
		{
			DataItem dataItem;

			objAddr = pst[i].objAddr.objAddrChar3 << 16 | pst[i].objAddr.objAddrChar2 << 8 | pst[i].objAddr.objAddrChar1;

			getTermItemByAddr(dataItem.termId, dataItem.itemId, objAddr, pIEC104Items);
			dataItem.time = rawtime;
			dataItem.value = pst[i].val;

			dataItems.push_back(dataItem);
			printf("[序号%d], [地址:%06X], [termID:%06X], [itemID:%06X], [值:%lf]"
					":IV:%d,NT:%d,SB:%d,BL:%d,OV:%d\n",
					i+1, objAddr, dataItem.termId, dataItem.itemId, dataItem.value,
					pst[i].quality_des.iv_d7, pst[i].quality_des.nt_d6, pst[i].quality_des.sb_d5, pst[i].quality_des.bl_d4, pst[i].quality_des.ov_d0);
		}
	}
	else if (vsq == 1 )	//信息体地址连续
	{
		stM_ME_NB_1_SERIAL* pst = reinterpret_cast<stM_ME_NB_1_SERIAL*>(frame.buf);
		objAddr = frame.head.objAddr.objAddrChar3 << 16 | frame.head.objAddr.objAddrChar2 << 8 | frame.head.objAddr.objAddrChar1;
		for(int i = 0; i < itemCount; i++)
		{
			DataItem dataItem;
			int addr = objAddr+i;

			getTermItemByAddr(dataItem.termId, dataItem.itemId, addr, pIEC104Items);
			dataItem.time = rawtime;
			dataItem.value = pst[i].val;

			dataItems.push_back(dataItem);

			printf("[序号%d], [地址:%06X], [termID:%06X], [itemID:%06X], [值:%lf]"
					":IV:%d,NT:%d,SB:%d,BL:%d,OV:%d\n",
					i+1, objAddr+i, dataItem.termId, dataItem.itemId, dataItem.value,
					pst[i].quality_des.iv_d7, pst[i].quality_des.nt_d6, pst[i].quality_des.sb_d5, pst[i].quality_des.bl_d4, pst[i].quality_des.ov_d0);
		}
	}
}

void Frame::DecodeMMENC1(vector<DataItem> &dataItems,
		const char* pFrame, int length, TableFile *pIEC104Items)//解析测量值，短浮点数
{
	std::cout<<"解析测量值，短浮点数"<<std::endl;
	stFrame frame;
	memcpy(&frame, pFrame, sizeof(frame));

	time_t rawtime;
	time(&rawtime);

	int objAddr = 0;
	int itemCount = frame.head.itemCount_D0_D6;
	int vsq = frame.head.vsq_D7;

//	printf("Head length:%d\n", sizeof(stHead));
	printf("连续:%d,信息体数量:%d\n", vsq, itemCount);

	if (vsq == 0)	//信息体地址非连续
	{
		stM_ME_NC_1* pst = reinterpret_cast<stM_ME_NC_1*>(frame.buf - LENGTH_OBJADDR);
		for(int i = 0; i < itemCount; i++)
		{
			DataItem dataItem;

			objAddr = pst[i].objAddr.objAddrChar3 << 16 | pst[i].objAddr.objAddrChar2 << 8 | pst[i].objAddr.objAddrChar1;

			getTermItemByAddr(dataItem.termId, dataItem.itemId, objAddr, pIEC104Items);
			dataItem.time = rawtime;

//			float value;
//			uint8_t *pValue = (uint8_t *)&value;
//			printf("%X %X %X %X\n", pst[i].d0_d7, pst[i].d8_d15, pst[i].d16_d24, pst[i].d25_d31);
//			pValue[3] = pst[i].d0_d7;
//			pValue[2] = pst[i].d8_d15;
//			pValue[1] = pst[i].d16_d24;
//			pValue[0] = pst[i].d25_d31;

			dataItem.value = pst[i].val;

			dataItems.push_back(dataItem);
			printf("[序号%d], [地址:%06X], [termID:%06X], [itemID:%06X], [值:%lf]"
					":IV:%d,NT:%d,SB:%d,BL:%d,OV:%d\n",
					i+1, objAddr, dataItem.termId, dataItem.itemId, dataItem.value,
					pst[i].quality_des.iv_d7, pst[i].quality_des.nt_d6, pst[i].quality_des.sb_d5, pst[i].quality_des.bl_d4, pst[i].quality_des.ov_d0);
		}
	}
	else if (vsq == 1 )	//信息体地址连续
	{
		stM_ME_NC_1_SERIAL* pst = reinterpret_cast<stM_ME_NC_1_SERIAL*>(frame.buf);
		objAddr = frame.head.objAddr.objAddrChar3 << 16 | frame.head.objAddr.objAddrChar2 << 8 | frame.head.objAddr.objAddrChar1;
		for(int i = 0; i < itemCount; i++)
		{
			DataItem dataItem;
			int addr = objAddr+i;

			getTermItemByAddr(dataItem.termId, dataItem.itemId, addr, pIEC104Items);
			dataItem.time = rawtime;
//			float value;
//			uint8_t *pValue = (uint8_t *)&value;
//			printf("%X %X %X %X\n", pst[i].d0_d7, pst[i].d8_d15, pst[i].d16_d24, pst[i].d25_d31);
//			pValue[3] = pst[i].d0_d7;
//			pValue[2] = pst[i].d8_d15;
//			pValue[1] = pst[i].d16_d24;
//			pValue[0] = pst[i].d25_d31;
			dataItem.value = pst[i].val;

			dataItems.push_back(dataItem);

			printf("[序号%d], [地址:%06X], [termID:%06X], [itemID:%06X], [值:%lf]"
					":IV:%d,NT:%d,SB:%d,BL:%d,OV:%d\n",
					i+1, objAddr+i, dataItem.termId, dataItem.itemId, dataItem.value,
					pst[i].quality_des.iv_d7, pst[i].quality_des.nt_d6, pst[i].quality_des.sb_d5, pst[i].quality_des.bl_d4, pst[i].quality_des.ov_d0);
		}
	}
}

void Frame::DecodeMMEND1(vector<DataItem> &dataItems,
		const char* pFrame, int length, TableFile *pIEC104Items)//解析测量值，规一化值，不带品质描述
{
	std::cout<<"解析测量值，规一化值，不带品质描述"<<std::endl;
	stFrame frame;
	memcpy(&frame, pFrame, sizeof(frame));

	time_t rawtime;
	time(&rawtime);

	int objAddr = 0;
	int itemCount = frame.head.itemCount_D0_D6;
	int vsq = frame.head.vsq_D7;

//	printf("Head length:%d\n", sizeof(stHead));
	printf("连续:%d,信息体数量:%d\n", vsq, itemCount);

	if (vsq == 0)	//信息体地址非连续
	{
		stM_ME_ND_1* pst = reinterpret_cast<stM_ME_ND_1*>(frame.buf - LENGTH_OBJADDR);
		for(int i = 0; i < itemCount; i++)
		{
			DataItem dataItem;

			objAddr = pst[i].objAddr.objAddrChar3 << 16 | pst[i].objAddr.objAddrChar2 << 8 | pst[i].objAddr.objAddrChar1;

			getTermItemByAddr(dataItem.termId, dataItem.itemId, objAddr, pIEC104Items);
			dataItem.time = rawtime;
			dataItem.value = pst[i].val;

			dataItems.push_back(dataItem);
			printf("[序号%d], [地址:%06X], [termID:%06X], [itemID:%06X], [值:%lf]\n",
					i+1, objAddr, dataItem.termId, dataItem.itemId, dataItem.value);
		}
	}
	else if (vsq == 1 )	//信息体地址连续
	{
		stM_ME_ND_1_SERIAL* pst = reinterpret_cast<stM_ME_ND_1_SERIAL*>(frame.buf);
		objAddr = frame.head.objAddr.objAddrChar3 << 16 | frame.head.objAddr.objAddrChar2 << 8 | frame.head.objAddr.objAddrChar1;
		for(int i = 0; i < itemCount; i++)
		{
			DataItem dataItem;
			int addr = objAddr+i;

			getTermItemByAddr(dataItem.termId, dataItem.itemId, addr, pIEC104Items);
			dataItem.time = rawtime;
			dataItem.value = pst[i].val;

			dataItems.push_back(dataItem);

			printf("[序号%d], [地址:%06X], [termID:%06X], [itemID:%06X], [值:%lf]\n",
					i+1, objAddr+i, dataItem.termId, dataItem.itemId, dataItem.value);
		}
	}
}

void Frame::DecodeMITNA1(vector<DataItem> &dataItems,
		const char* pFrame, int length, TableFile *pIEC104Items)//解析累计量
{
	std::cout<<"解析累计量"<<std::endl;
	//	stFrame frame;
	//	memcpy(&frame, pFrame, sizeof(stFrame));
	//
	//	stM_IT_NA_1* pst;
	//
	//	pst = reinterpret_cast<stM_IT_NA_1*>(frame.buf);
	//
	//	time_t rawtime;
	//	time (&rawtime);
	//
	//	int val = 0;
	//	for (int i = 0; i < itemCount; i++)
	//	{
	//		val = pst[i].val & 0x7FFF;
	//		if ((pst[i].val & 0x1000) == 0x1000)
	//			val = val * -1;
	//
	////		pData->cpId = m_cpid;
	////		pData->time = rawtime;
	////		pData->pointNo = itemCount + i;
	////		pData->dataItemCode = -104;
	////		pData->value = val;
	////		SendCollectedData(*pData);
	//
	//		printf("\n序号%d,[%x], 遥测值:%d", i, pst[i].val, val);
	//	}
}

void Frame::DecodeMSPTB1(vector<DataItem> &dataItems,
		const char* pFrame, int length, TableFile *pIEC104Items)//带长时标，单点信息
{
	std::cout<<"解析带长时标，单点信息"<<std::endl;
	stFrame frame;
	memcpy(&frame, pFrame, sizeof(frame));

	time_t rawtime;
	time(&rawtime);

	int objAddr = 0;
	int itemCount = frame.head.itemCount_D0_D6;
	int vsq = frame.head.vsq_D7;

//	printf("Head length:%d\n", sizeof(stHead));
	printf("连续:%d,信息体数量:%d\n", vsq, itemCount);

	if (vsq == 0)	//信息体地址非连续
	{
		stM_SP_TB_1* pst = reinterpret_cast<stM_SP_TB_1*>(frame.buf - LENGTH_OBJADDR);
		for(int i = 0; i < itemCount; i++)
		{
			DataItem dataItem;

			objAddr = pst[i].objAddr.objAddrChar3 << 16 | pst[i].objAddr.objAddrChar2 << 8 | pst[i].objAddr.objAddrChar1;

			getTermItemByAddr(dataItem.termId, dataItem.itemId, objAddr, pIEC104Items);

			if (!pst[i].time_stamp.min_iv_d7 &&
					!pst[i].time_stamp.hour_su_d7)
			{
				struct tm t;
				memset(&t, 0, sizeof(t));
				t.tm_year = pst[i].time_stamp.year_val_d0_d6 - 1900 + 2000;
				t.tm_mon = pst[i].time_stamp.month_val_d0_d3 - 1;
				t.tm_mday = pst[i].time_stamp.day_val_d0_d4;
				t.tm_hour = pst[i].time_stamp.hour_val_d0_d4;
				t.tm_min = pst[i].time_stamp.min_val_d0_d5;
				t.tm_sec = pst[i].time_stamp.millisec/1000;
				dataItem.time = timelocal(&t);
			}else {
				dataItem.time = rawtime;
			}

			dataItem.value = pst[i].spi_d0;

			dataItems.push_back(dataItem);
			printf("[序号%d], [地址:%06X], [termID:%06X], [itemID:%06X], [值:%lf]"
					":IV:%d,NT:%d,SB:%d,BL:%d,SPI:%d\n",
					i+1, objAddr, dataItem.termId, dataItem.itemId, dataItem.value,
					pst[i].iv_d7, pst[i].nt_d6, pst[i].sb_d5, pst[i].bl_d4, pst[i].spi_d0);
			printf("[时标:%02d年%02d月%02d日 %02d:%02d:%02d:%03d]\n",
								pst[i].time_stamp.year_val_d0_d6,
								pst[i].time_stamp.month_val_d0_d3,
								pst[i].time_stamp.day_val_d0_d4,
								pst[i].time_stamp.hour_val_d0_d4,
								pst[i].time_stamp.min_val_d0_d5,
								pst[i].time_stamp.millisec/1000,
								pst[i].time_stamp.millisec%1000);
		}
	}
	else if (vsq == 1 )	//信息体地址连续
	{
		stM_SP_TB_1_SERIAL* pst = reinterpret_cast<stM_SP_TB_1_SERIAL*>(frame.buf);
		objAddr = frame.head.objAddr.objAddrChar3 << 16 | frame.head.objAddr.objAddrChar2 << 8 | frame.head.objAddr.objAddrChar1;
		for(int i = 0; i < itemCount; i++)
		{
			DataItem dataItem;

			int addr = objAddr+i;
			getTermItemByAddr(dataItem.termId, dataItem.itemId, addr, pIEC104Items);

			if (!pst[i].time_stamp.min_iv_d7 &&
					!pst[i].time_stamp.hour_su_d7)
			{
				struct tm t;
				memset(&t, 0, sizeof(t));
				t.tm_year = pst[i].time_stamp.year_val_d0_d6 - 1900 + 2000;
				t.tm_mon = pst[i].time_stamp.month_val_d0_d3 - 1;
				t.tm_mday = pst[i].time_stamp.day_val_d0_d4;
				t.tm_hour = pst[i].time_stamp.hour_val_d0_d4;
				t.tm_min = pst[i].time_stamp.min_val_d0_d5;
				t.tm_sec = pst[i].time_stamp.millisec/1000;
				dataItem.time = timelocal(&t);
			}else {
				dataItem.time = rawtime;
			}

			dataItem.value = pst[i].spi_d0;

			dataItems.push_back(dataItem);

			printf("[序号%d], [地址:%06X], [termID:%06X], [itemID:%06X], [值:%lf]"
					":IV:%d,NT:%d,SB:%d,BL:%d,SPI:%d\n",
					i+1, objAddr+i, dataItem.termId, dataItem.itemId, dataItem.value,
					pst[i].iv_d7, pst[i].nt_d6, pst[i].sb_d5, pst[i].bl_d4, pst[i].spi_d0);
			printf("[时标:%02d年%02d月%02d日 %02d:%02d:%02d:%03d]\n",
								pst[i].time_stamp.year_val_d0_d6,
								pst[i].time_stamp.month_val_d0_d3,
								pst[i].time_stamp.day_val_d0_d4,
								pst[i].time_stamp.hour_val_d0_d4,
								pst[i].time_stamp.min_val_d0_d5,
								pst[i].time_stamp.millisec/1000,
								pst[i].time_stamp.millisec%1000);
		}
	}
}

void Frame::DecodeMDPTB1(vector<DataItem> &dataItems,
		const char* pFrame, int length, TableFile *pIEC104Items)//带长时标，双点信息
{
	std::cout<<"解析带长时标，双点信息"<<std::endl;
#if 1
	stFrame frame;
	memcpy(&frame, pFrame, sizeof(frame));

	time_t rawtime;
	time(&rawtime);

	int objAddr = 0;
	int itemCount = frame.head.itemCount_D0_D6;
	int vsq = frame.head.vsq_D7;

//	printf("Head length:%d\n", sizeof(stHead));
	printf("连续:%d,信息体数量:%d\n", vsq, itemCount);

	if (vsq == 0)	//信息体地址非连续
	{
		stM_DP_TB_1* pst = reinterpret_cast<stM_DP_TB_1*>(frame.buf - LENGTH_OBJADDR);
		for(int i = 0; i < itemCount; i++)
		{
			DataItem dataItem;

			objAddr = pst[i].objAddr.objAddrChar3 << 16 | pst[i].objAddr.objAddrChar2 << 8 | pst[i].objAddr.objAddrChar1;

			getTermItemByAddr(dataItem.termId, dataItem.itemId, objAddr, pIEC104Items);

			if (!pst[i].time_stamp.min_iv_d7 &&
					!pst[i].time_stamp.hour_su_d7)
			{
				struct tm t;
				memset(&t, 0, sizeof(t));
				t.tm_year = pst[i].time_stamp.year_val_d0_d6 - 1900 + 2000;
				t.tm_mon = pst[i].time_stamp.month_val_d0_d3 - 1;
				t.tm_mday = pst[i].time_stamp.day_val_d0_d4;
				t.tm_hour = pst[i].time_stamp.hour_val_d0_d4;
				t.tm_min = pst[i].time_stamp.min_val_d0_d5;
				t.tm_sec = pst[i].time_stamp.millisec/1000;
				dataItem.time = timelocal(&t);
			}else {
				dataItem.time = rawtime;
			}

			dataItem.value = pst[i].dpi_d0_d1;

			dataItems.push_back(dataItem);
			printf("[序号%d], [地址:%06X], [termID:%06X], [itemID:%06X], [值:%lf]"
					":IV:%d,NT:%d,SB:%d,BL:%d,DPI:%d\n",
					i+1, objAddr, dataItem.termId, dataItem.itemId, dataItem.value,
					pst[i].iv_d7, pst[i].nt_d6, pst[i].sb_d5, pst[i].bl_d4, pst[i].dpi_d0_d1);
			printf("[时标:%02d年%02d月%02d日 %02d:%02d:%02d:%03d]\n",
					pst[i].time_stamp.year_val_d0_d6,
					pst[i].time_stamp.month_val_d0_d3,
					pst[i].time_stamp.day_val_d0_d4,
					pst[i].time_stamp.hour_val_d0_d4,
					pst[i].time_stamp.min_val_d0_d5,
					pst[i].time_stamp.millisec/1000,
					pst[i].time_stamp.millisec%1000);
		}
	}
	else if (vsq == 1 )	//信息体地址连续
	{
		stM_DP_TB_1_SERIAL* pst = reinterpret_cast<stM_DP_TB_1_SERIAL*>(frame.buf);
		objAddr = frame.head.objAddr.objAddrChar3 << 16 | frame.head.objAddr.objAddrChar2 << 8 | frame.head.objAddr.objAddrChar1;
		for(int i = 0; i < itemCount; i++)
		{
			DataItem dataItem;
			int addr = objAddr+i;

			getTermItemByAddr(dataItem.termId, dataItem.itemId, addr, pIEC104Items);

			if (!pst[i].time_stamp.min_iv_d7 &&
					!pst[i].time_stamp.hour_su_d7)
			{
				struct tm t;
				memset(&t, 0, sizeof(t));
				t.tm_year = pst[i].time_stamp.year_val_d0_d6 - 1900 + 2000;
				t.tm_mon = pst[i].time_stamp.month_val_d0_d3 - 1;
				t.tm_mday = pst[i].time_stamp.day_val_d0_d4;
				t.tm_hour = pst[i].time_stamp.hour_val_d0_d4;
				t.tm_min = pst[i].time_stamp.min_val_d0_d5;
				t.tm_sec = pst[i].time_stamp.millisec/1000;
				dataItem.time = timelocal(&t);
			}else {
				dataItem.time = rawtime;
			}
			dataItem.value = pst[i].dpi_d0_d1;

			dataItems.push_back(dataItem);

			printf("[序号%d], [地址:%06X], [termID:%06X], [itemID:%06X], [值:%lf]"
					":IV:%d,NT:%d,SB:%d,BL:%d,DPI:%d\n",
					i+1, objAddr+i, dataItem.termId, dataItem.itemId, dataItem.value,
					pst[i].iv_d7, pst[i].nt_d6, pst[i].sb_d5, pst[i].bl_d4, pst[i].dpi_d0_d1);
			printf("[时标:%02d年%02d月%02d日 %02d:%02d:%02d:%03d]\n",
								pst[i].time_stamp.year_val_d0_d6,
								pst[i].time_stamp.month_val_d0_d3,
								pst[i].time_stamp.day_val_d0_d4,
								pst[i].time_stamp.hour_val_d0_d4,
								pst[i].time_stamp.min_val_d0_d5,
								pst[i].time_stamp.millisec/1000,
								pst[i].time_stamp.millisec%1000);
		}
	}
#endif
}

void Frame::DecodeMMETF1(vector<DataItem> &dataItems,
		const char* pFrame, int length, TableFile *pIEC104Items)//带长时标，测量值，短浮点数
{
	std::cout<<"解析带长时标，测量值，短浮点数"<<std::endl;

	stFrame frame;
	memcpy(&frame, pFrame, sizeof(frame));

	time_t rawtime;
	time(&rawtime);

	int objAddr = 0;
	int itemCount = frame.head.itemCount_D0_D6;
	int vsq = frame.head.vsq_D7;

	//	printf("Head length:%d\n", sizeof(stHead));
	printf("连续:%d,信息体数量:%d\n", vsq, itemCount);

	if (vsq == 0)	//信息体地址非连续
	{
		stM_ME_TF_1* pst = reinterpret_cast<stM_ME_TF_1*>(frame.buf - LENGTH_OBJADDR);
		for(int i = 0; i < itemCount; i++)
		{
			DataItem dataItem;

			objAddr = pst[i].objAddr.objAddrChar3 << 16 | pst[i].objAddr.objAddrChar2 << 8 | pst[i].objAddr.objAddrChar1;

			getTermItemByAddr(dataItem.termId, dataItem.itemId, objAddr, pIEC104Items);

			if (!pst[i].time_stamp.min_iv_d7 &&
					!pst[i].time_stamp.hour_su_d7)
			{
				struct tm t;
				memset(&t, 0, sizeof(t));
				t.tm_year = pst[i].time_stamp.year_val_d0_d6 - 1900 + 2000;
				t.tm_mon = pst[i].time_stamp.month_val_d0_d3 - 1;
				t.tm_mday = pst[i].time_stamp.day_val_d0_d4;
				t.tm_hour = pst[i].time_stamp.hour_val_d0_d4;
				t.tm_min = pst[i].time_stamp.min_val_d0_d5;
				t.tm_sec = pst[i].time_stamp.millisec/1000;
				dataItem.time = timelocal(&t);
			}else {
				dataItem.time = rawtime;
			}

			dataItem.value = pst[i].val;

			dataItems.push_back(dataItem);
			printf("[序号%d], [地址:%06X], [termID:%06X], [itemID:%06X], [值:%lf]"
					":IV:%d,NT:%d,SB:%d,BL:%d,OV:%d\n",
					i+1, objAddr, dataItem.termId, dataItem.itemId, dataItem.value,
					pst[i].quality_des.iv_d7, pst[i].quality_des.nt_d6, pst[i].quality_des.sb_d5, pst[i].quality_des.bl_d4, pst[i].quality_des.ov_d0);
			printf("[时标:%02d年%02d月%02d日 %02d:%02d:%02d:%03d]\n",
					pst[i].time_stamp.year_val_d0_d6,
					pst[i].time_stamp.month_val_d0_d3,
					pst[i].time_stamp.day_val_d0_d4,
					pst[i].time_stamp.hour_val_d0_d4,
					pst[i].time_stamp.min_val_d0_d5,
					pst[i].time_stamp.millisec/1000,
					pst[i].time_stamp.millisec%1000);
		}
	}
	else if (vsq == 1 )	//信息体地址连续
	{
		stM_ME_TF_1_SERIAL* pst = reinterpret_cast<stM_ME_TF_1_SERIAL*>(frame.buf);
		objAddr = frame.head.objAddr.objAddrChar3 << 16 | frame.head.objAddr.objAddrChar2 << 8 | frame.head.objAddr.objAddrChar1;
		for(int i = 0; i < itemCount; i++)
		{
			DataItem dataItem;
			int addr = objAddr+i;

			getTermItemByAddr(dataItem.termId, dataItem.itemId, addr, pIEC104Items);

			if (!pst[i].time_stamp.min_iv_d7 &&
					!pst[i].time_stamp.hour_su_d7)
			{
				struct tm t;
				memset(&t, 0, sizeof(t));
				t.tm_year = pst[i].time_stamp.year_val_d0_d6 - 1900 + 2000;
				t.tm_mon = pst[i].time_stamp.month_val_d0_d3 - 1;
				t.tm_mday = pst[i].time_stamp.day_val_d0_d4;
				t.tm_hour = pst[i].time_stamp.hour_val_d0_d4;
				t.tm_min = pst[i].time_stamp.min_val_d0_d5;
				t.tm_sec = pst[i].time_stamp.millisec/1000;
				dataItem.time = timelocal(&t);
			}else {
				dataItem.time = rawtime;
			}
			dataItem.value = pst[i].val;

			dataItems.push_back(dataItem);

			printf("[序号%d], [地址:%06X], [termID:%06X], [itemID:%06X], [值:%lf]"
					":IV:%d,NT:%d,SB:%d,BL:%d,OV:%d\n",
					i+1, objAddr+i, dataItem.termId, dataItem.itemId, dataItem.value,
					pst[i].quality_des.iv_d7, pst[i].quality_des.nt_d6, pst[i].quality_des.sb_d5, pst[i].quality_des.bl_d4, pst[i].quality_des.ov_d0);
			printf("[时标:%02d年%02d月%02d日 %02d:%02d:%02d:%03d]\n",
					pst[i].time_stamp.year_val_d0_d6,
					pst[i].time_stamp.month_val_d0_d3,
					pst[i].time_stamp.day_val_d0_d4,
					pst[i].time_stamp.hour_val_d0_d4,
					pst[i].time_stamp.min_val_d0_d5,
					pst[i].time_stamp.millisec/1000,
					pst[i].time_stamp.millisec%1000);
		}
	}
}

void Frame::DecodeMEPTD1(vector<DataItem> &dataItems,
		const char* pFrame, int length, TableFile *pIEC104Items)//带长时标的继电保护装置事件
{
	std::cout<<"解析带长时标的继电保护装置事件"<<std::endl;

	stFrame frame;
	memcpy(&frame, pFrame, sizeof(frame));

	time_t rawtime;
	time(&rawtime);

	int objAddr = 0;
	int itemCount = frame.head.itemCount_D0_D6;
	int vsq = frame.head.vsq_D7;

	//	printf("Head length:%d\n", sizeof(stHead));
	printf("连续:%d,信息体数量:%d\n", vsq, itemCount);

	if (vsq == 0)	//信息体地址非连续
	{
		stM_EP_TD_1* pst = reinterpret_cast<stM_EP_TD_1*>(frame.buf - LENGTH_OBJADDR);
		for(int i = 0; i < itemCount; i++)
		{
			DataItem dataItem;

			objAddr = pst[i].objAddr.objAddrChar3 << 16 | pst[i].objAddr.objAddrChar2 << 8 | pst[i].objAddr.objAddrChar1;

			getTermItemByAddr(dataItem.termId, dataItem.itemId, objAddr, pIEC104Items);

			if (!pst[i].time_stamp.min_iv_d7 &&
					!pst[i].time_stamp.hour_su_d7)
			{
				struct tm t;
				memset(&t, 0, sizeof(t));
				t.tm_year = pst[i].time_stamp.year_val_d0_d6 - 1900 + 2000;
				t.tm_mon = pst[i].time_stamp.month_val_d0_d3 - 1;
				t.tm_mday = pst[i].time_stamp.day_val_d0_d4;
				t.tm_hour = pst[i].time_stamp.hour_val_d0_d4;
				t.tm_min = pst[i].time_stamp.min_val_d0_d5;
				t.tm_sec = pst[i].time_stamp.millisec/1000;
				dataItem.time = timelocal(&t);
			}else {
				dataItem.time = rawtime;
			}

			dataItem.value = pst[i].val;

			dataItems.push_back(dataItem);
			printf("[序号%d], [地址:%06X], [termID:%06X], [itemID:%06X], [值:%lf]"
					", [动作时间：%d]\n"
//					":IV:%d,NT:%d,SB:%d,BL:%d,OV:%d\n"
					,i+1, objAddr, dataItem.termId, dataItem.itemId, dataItem.value
					,pst[i].action_time
//					,pst[i].quality_des.iv_d7, pst[i].quality_des.nt_d6, pst[i].quality_des.sb_d5, pst[i].quality_des.bl_d4, pst[i].quality_des.ov_d0
					);
			printf("[时标:%02d年%02d月%02d日 %02d:%02d:%02d:%03d]\n",
					pst[i].time_stamp.year_val_d0_d6,
					pst[i].time_stamp.month_val_d0_d3,
					pst[i].time_stamp.day_val_d0_d4,
					pst[i].time_stamp.hour_val_d0_d4,
					pst[i].time_stamp.min_val_d0_d5,
					pst[i].time_stamp.millisec/1000,
					pst[i].time_stamp.millisec%1000);
		}
	}
	else if (vsq == 1 )	//信息体地址连续
	{
		stM_EP_TD_1_SERIAL* pst = reinterpret_cast<stM_EP_TD_1_SERIAL*>(frame.buf);
		objAddr = frame.head.objAddr.objAddrChar3 << 16 | frame.head.objAddr.objAddrChar2 << 8 | frame.head.objAddr.objAddrChar1;
		for(int i = 0; i < itemCount; i++)
		{
			DataItem dataItem;
			int addr = objAddr+i;

			getTermItemByAddr(dataItem.termId, dataItem.itemId, addr, pIEC104Items);

			if (!pst[i].time_stamp.min_iv_d7 &&
					!pst[i].time_stamp.hour_su_d7)
			{
				struct tm t;
				memset(&t, 0, sizeof(t));
				t.tm_year = pst[i].time_stamp.year_val_d0_d6 - 1900 + 2000;
				t.tm_mon = pst[i].time_stamp.month_val_d0_d3 - 1;
				t.tm_mday = pst[i].time_stamp.day_val_d0_d4;
				t.tm_hour = pst[i].time_stamp.hour_val_d0_d4;
				t.tm_min = pst[i].time_stamp.min_val_d0_d5;
				t.tm_sec = pst[i].time_stamp.millisec/1000;
				dataItem.time = timelocal(&t);
			}else {
				dataItem.time = rawtime;
			}
			dataItem.value = pst[i].val;

			dataItems.push_back(dataItem);

			printf("[序号%d], [地址:%06X], [termID:%06X], [itemID:%06X], [值:%lf]"
					", [动作时间：%d]\n"
//					":IV:%d,NT:%d,SB:%d,BL:%d,OV:%d\n"
					,i+1, objAddr, dataItem.termId, dataItem.itemId, dataItem.value
					,pst[i].action_time
//					,pst[i].quality_des.iv_d7, pst[i].quality_des.nt_d6, pst[i].quality_des.sb_d5, pst[i].quality_des.bl_d4, pst[i].quality_des.ov_d0
					);
			printf("[时标:%02d年%02d月%02d日 %02d:%02d:%02d:%03d]\n",
					pst[i].time_stamp.year_val_d0_d6,
					pst[i].time_stamp.month_val_d0_d3,
					pst[i].time_stamp.day_val_d0_d4,
					pst[i].time_stamp.hour_val_d0_d4,
					pst[i].time_stamp.min_val_d0_d5,
					pst[i].time_stamp.millisec/1000,
					pst[i].time_stamp.millisec%1000);
		}
	}
}

}//namespace VIEC104

}//namespace Protocol

}//namespace Uarc
