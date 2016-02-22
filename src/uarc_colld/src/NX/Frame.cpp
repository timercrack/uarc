/*
 * Frame.cpp
 *
 *  Created on: 2014-12-26
 *      Author: root
 */

#include "Frame.h"
#include "Control.h"
#include <Poco/RegularExpression.h>

namespace Uarc {

namespace Protocol {

namespace NX {

const string Frame::_protocol_path = "/etc/uarc/protocol-cfg/Nx/";
//const string Frame::_protocol_path = "./";
const string Frame::_fixedFrame = "fixedFrame";
const string Frame::_verFrame = "verFrame";
Frame::Frame(const char *pFrame, int length, Control *parent) :
	_parent(parent) {
	// TODO 自动生成的构造函数存根
	if (pFrame && length < MAX_FRAME_LENGTH && length > 0)
	{
		memcpy(_data, pFrame, length);
		_length = length;
		this->length();
	}else {
		memset(_data, 0, MAX_FRAME_LENGTH);
		_length = 0;
	}
}

Frame::~Frame() {
	// TODO 自动生成的析构函数存根
}

char *Frame::data()
{
	return (char *)_data;
}

void Frame::showData(const char *data, int length)
{
	for (int i = 0; i < length; ++i)
	{
		printf("%02x ", (unsigned char)data[i]);
	}
	printf("\n");
}

bool Frame::isFixedFrame()
{
	bool isFixedFrame = true;

	if (!_frame.get<bool>("isFixedFrame", isFixedFrame))
	{
		printf("error:get isFixedFrame\n");
		return isFixedFrame;
	}

	return isFixedFrame;
}

int Frame::areaCode()
{
	int areaCode = -1;

	stringstream strstream;
	Address address;
	bool isFixedFrame = true;

	if (!_frame.get<bool>("isFixedFrame", isFixedFrame))
	{
		return areaCode;
	}

	if (isFixedFrame)
	{
		FixedFrame fixedFrame;
		if (!_frame.get<FixedFrame>("fixedFrame", fixedFrame))
		{
			return areaCode;
		}
		if (!fixedFrame.get<Address>("address", address))
		{
			return areaCode;
		}
	}else {
		VerFrame verFrame;
		if (!_frame.get<VerFrame>("verFrame", verFrame))
		{
			return areaCode;
		}
		if (!verFrame.get<Address>("address", address))
		{
			return areaCode;
		}
	}

	if (!address.get<int>("areaCode", areaCode))
	{
		return areaCode;
	}

	return areaCode;
}

int Frame::termAddr()
{
	int termAddr = -1;

	stringstream strstream;
	Address address;
	bool isFixedFrame = true;

	if (!_frame.get<bool>("isFixedFrame", isFixedFrame))
	{
		return termAddr;
	}

	if (isFixedFrame)
	{
		FixedFrame fixedFrame;
		if (!_frame.get<FixedFrame>("fixedFrame", fixedFrame))
		{
			return termAddr;
		}
		if (!fixedFrame.get<Address>("address", address))
		{
			return termAddr;
		}
	}else {
		VerFrame verFrame;
		if (!_frame.get<VerFrame>("verFrame", verFrame))
		{
			return termAddr;
		}
		if (!verFrame.get<Address>("address", address))
		{
			return termAddr;
		}
	}

	if (!address.get<int>("termAddr", termAddr))
	{
		return termAddr;
	}

	return termAddr;
}

int Frame::masterAddr()
{
	int masterAddr = -1;

	stringstream strstream;
	Address address;
	bool isFixedFrame = true;

	if (!_frame.get<bool>("isFixedFrame", isFixedFrame))
	{
		return masterAddr;
	}

	if (isFixedFrame)
	{
		FixedFrame fixedFrame;
		if (!_frame.get<FixedFrame>("fixedFrame", fixedFrame))
		{
			return masterAddr;
		}
		if (!fixedFrame.get<Address>("address", address))
		{
			return masterAddr;
		}
	}else {
		VerFrame verFrame;
		if (!_frame.get<VerFrame>("verFrame", verFrame))
		{
			return masterAddr;
		}
		if (!verFrame.get<Address>("address", address))
		{
			return masterAddr;
		}
	}

	if (!address.get<int>("masterAddr", masterAddr))
	{
		return masterAddr;
	}

	return masterAddr;
}

int Frame::pseq()
{
	int Pseq = -1;

	//类Control冲突，需要加上域限定符，使用的是无命名空间的Control
	::Control control;
	bool isFixedFrame = true;

	if (!_frame.get<bool>("isFixedFrame", isFixedFrame))
	{
		return Pseq;
	}

	if (isFixedFrame)
	{
		FixedFrame fixedFrame;
		if (!_frame.get<FixedFrame>("fixedFrame", fixedFrame))
		{
			return Pseq;
		}

		if (!fixedFrame.get< ::Control >("control", control))
		{
			return Pseq;
		}
	}else {
		VerFrame verFrame;
		if (!_frame.get<VerFrame>("verFrame", verFrame))
		{
			return Pseq;
		}
		if (!verFrame.get< ::Control >("control", control))
		{
			return Pseq;
		}
	}

	if (!control.get<int>("Pseq", Pseq))
	{
		return Pseq;
	}

	return Pseq;
}

int Frame::rseq()
{
	int Rseq = -1;

	//类Control冲突，需要加上域限定符，使用的是无命名空间的Control
	::Control control;
	bool isFixedFrame = true;

	if (!_frame.get<bool>("isFixedFrame", isFixedFrame))
	{
		return Rseq;
	}

	if (isFixedFrame)
	{
		FixedFrame fixedFrame;
		if (!_frame.get<FixedFrame>("fixedFrame", fixedFrame))
		{
			return Rseq;
		}

		if (!fixedFrame.get< ::Control >("control", control))
		{
			return Rseq;
		}
	}else {
		VerFrame verFrame;
		if (!_frame.get<VerFrame>("verFrame", verFrame))
		{
			return Rseq;
		}
		if (!verFrame.get< ::Control >("control", control))
		{
			return Rseq;
		}
	}

	if (!control.get<int>("Rseq", Rseq))
	{
		return Rseq;
	}

	return Rseq;
}

string Frame::identity()
{
	string identity;

	{
		stringstream strstream;
		Address address;
		bool isFixedFrame = true;

		if (!_frame.get<bool>("isFixedFrame", isFixedFrame))
		{
			return identity;
		}

		if (isFixedFrame)
		{
			FixedFrame fixedFrame;
			if (!_frame.get<FixedFrame>("fixedFrame", fixedFrame))
			{
				return identity;
			}
			if (!fixedFrame.get<Address>("address", address))
			{
				return identity;
			}
		}else {
			VerFrame verFrame;
			if (!_frame.get<VerFrame>("verFrame", verFrame))
			{
				return identity;
			}
			if (!verFrame.get<Address>("address", address))
			{
				return identity;
			}
		}

		int areaCode = -1;
		if (!address.get<int>("areaCode", areaCode))
		{
			return identity;
		}
		int termAddr = -1;
		if (!address.get<int>("termAddr", termAddr))
		{
			return identity;
		}
		int masterAddr = -1;
		if (!address.get<int>("masterAddr", masterAddr))
		{
			return identity;
		}

		//唯一标识 = 行政区划码A1+信息集中与交互终端地址A2;//+主(子)站地址和组地址标志A3
		strstream<<areaCode<<termAddr;//<<masterAddr;
		strstream>>identity;
	}

	return identity;
}

string Frame::identity(const char *pFrame, int length)
{
	FrameNx frame;
	string identity;

	if (!pFrame || (length == 0) || length >= MAX_FRAME_LENGTH)
	{
		return identity;
	}

	if (decode(pFrame, length, frame))
	{
		stringstream strstream;
		Address address;
		bool isFixedFrame = true;

		if (!frame.get<bool>("isFixedFrame", isFixedFrame))
		{
			return identity;
		}

		if (isFixedFrame)
		{
			FixedFrame fixedFrame;
			if (!frame.get<FixedFrame>("fixedFrame", fixedFrame))
			{
				return identity;
			}
			if (!fixedFrame.get<Address>("address", address))
			{
				return identity;
			}
		}else {
			VerFrame verFrame;
			if (!frame.get<VerFrame>("verFrame", verFrame))
			{
				return identity;
			}
			if (!verFrame.get<Address>("address", address))
			{
				return identity;
			}
		}

		int areaCode = -1;
		if (!address.get<int>("areaCode", areaCode))
		{
			return identity;
		}
		int termAddr = -1;
		if (!address.get<int>("termAddr", termAddr))
		{
			return identity;
		}
		int masterAddr = -1;
		if (!address.get<int>("masterAddr", masterAddr))
		{
			return identity;
		}

		//唯一标识 = 行政区划码A1+信息集中与交互终端地址A2;//+主(子)站地址和组地址标志A3
		strstream<<areaCode<<termAddr;//<<masterAddr;
		strstream>>identity;
	}

	return identity;
}

int Frame::length()
{
	int len = -1;

	if (_length >= MAX_FRAME_LENGTH || _length < FIXED_FRAME_LENGTH)
	{
		return len;
	}

	if (decode())
	{
		bool isFixedFrame = true;

		if (!_frame.get<bool>("isFixedFrame", isFixedFrame))
		{
			return len;
		}

		if (isFixedFrame)
		{
			len = FIXED_FRAME_LENGTH;
			return len;
		}else {
			VerFrame verFrame;
			if (!_frame.get<VerFrame>("verFrame", verFrame))
			{
				return len;
			}
			int firstLength = -1, secondLength = -1;
			if (!verFrame.get<int>("firstLength", firstLength))
			{
				return len;
			}
			if (!verFrame.get<int>("secondLength", secondLength))
			{
				return len;
			}
			if (firstLength == secondLength && firstLength != -1)
			{
				len = firstLength + VERIABLE_FRAME_ADDITIONAL_LEN;
				return len;
			}
		}
	}

	return len;
}

int Frame::length(const char *pFrame, int length)
{
	FrameNx frame;
	int len = -1;

	if (!pFrame)return len;
	if (length < FIXED_FRAME_LENGTH)
	{
		return len;
	}

	if (decode(pFrame, length, frame))
	{
		bool isFixedFrame = true;

		if (!frame.get<bool>("isFixedFrame", isFixedFrame))
		{
			return -2;
		}

		if (isFixedFrame)
		{
			len = FIXED_FRAME_LENGTH;
			return len;
		}else {
			VerFrame verFrame;
			if (!frame.get<VerFrame>("verFrame", verFrame))
			{
				return -2;
			}
			int firstLength = -1, secondLength = -1;
			if (!verFrame.get<int>("firstLength", firstLength))
			{
				return -2;
			}
			if (!verFrame.get<int>("secondLength", secondLength))
			{
				return -2;
			}
			if (firstLength == secondLength && firstLength != -1)
			{
				len = firstLength + VERIABLE_FRAME_ADDITIONAL_LEN;
				return len;
			}else {
				return -2;
			}
		}
	}

	return -2;
}

int Frame::funCode()
{
	int funCode = -1;

	//类Control冲突，需要加上域限定符，使用的是无命名空间的Control
	::Control control;
	bool isFixedFrame = true;

	if (!_frame.get<bool>("isFixedFrame", isFixedFrame))
	{
		return funCode;
	}

	if (isFixedFrame)
	{
		FixedFrame fixedFrame;
		if (!_frame.get<FixedFrame>("fixedFrame", fixedFrame))
		{
			return funCode;
		}

		if (!fixedFrame.get< ::Control >("control", control))
		{
			return funCode;
		}
	}else {
		VerFrame verFrame;
		if (!_frame.get<VerFrame>("verFrame", verFrame))
		{
			return funCode;
		}
		if (!verFrame.get< ::Control >("control", control))
		{
			return funCode;
		}
	}

	if (!control.get<int>("funCode", funCode))
	{
		return funCode;
	}

	return funCode;
}

int Frame::afn()
{
	int afn = -1;

	bool isFixedFrame = true;

	if (!_frame.get<bool>("isFixedFrame", isFixedFrame))
	{
		return afn;
	}

	if (isFixedFrame)
	{
		return afn;
	}else {
		VerFrame verFrame;
		if (!_frame.get<VerFrame>("verFrame", verFrame))
		{
			return afn;
		}

		AppControl appControl;
		if (!verFrame.get<AppControl>("appControl", appControl))
		{
			return afn;
		}

		if (!appControl.get<int>("acAfn", afn))
		{
			return afn;
		}
	}

	return afn;
}

bool Frame::getConfirmOrDeny(bool &value)
{
	for (FrameBase::Iterator iter = _frame.begin(); iter != _frame.end(); iter++)
	{
		if (RegularExpression::match(iter->first, string("^AF[0-9]{1,}P[0-9]{1,}F[0-9]{1,}$")))
		{
			UserDataUnit userData;
			_frame.get<UserDataUnit>(iter->first, userData);

			for (FrameBase::Iterator it = userData.begin(); it != userData.end(); it++)
			{
				if (RegularExpression::match(it->first, string("^AF[0-9]{1,}P[0-9]{1,}F[0-9]{1,}:ConfirmOrDeny$")))
				{
					string key = it->first;

					int v = it->second.convert<int>();

					value = (v == 1);

					return true;
				}
			}
		}
	}

	return false;
}

bool Frame::getValues(vector<DataItem> &dataItems)
{
	bool result = false;
	DateTime dateTime;
	RegularExpression regularExpression(string("[0-9]{1,}"));

	for (FrameBase::Iterator iter = _frame.begin(); iter != _frame.end(); iter++)
	{
		if (RegularExpression::match(iter->first, string("^AF[0-9]{1,}P[0-9]{1,}F[0-9]{1,}$")))
		{
			UserDataUnit userData;
			_frame.get<UserDataUnit>(iter->first, userData);

			for (FrameBase::Iterator it = userData.begin(); it != userData.end(); it++)
			{
				printf("all:%s = %lf\n", it->first.c_str(), it->second.convert<double>());
				if (RegularExpression::match(it->first, string("^AF[0-9]{1,}P[0-9]{1,}F[0-9]{1,}:[A-F0-9]{1,}$")))
				{
					printf("match:%s = %lf\n", it->first.c_str(), it->second.convert<double>());
					string key = it->first;

					DataItem item;
					string afn;
					string pn;
					string fn;
					string ename;

					int termId = -1;
					int itemId = -1;
					double value = 0;

					int offset = 0;

					//get AFN
					regularExpression.extract(key, offset, afn);

					//get Pn
					offset = key.find_first_of('P');
					if (offset < 0)
					{
						return false;
					}
					regularExpression.extract(key, offset, pn);

					//get Fn
					//get offset==================
//					if (offset < 0)
//					{
//						return false;
//					}
//					regularExpression.extract(key, offset, fn);

					sscanf(pn.c_str(), "%d", &termId);

					offset = key.find_first_of(':');
					if (offset < 0)
					{
						return false;
					}
					ename = key.substr(offset+1, key.length()-offset + 1);
					sscanf(ename.c_str(), "%X", &itemId);

					value = it->second.convert<double>();

					item.termId = termId;
					item.itemId = itemId;
					item.value = value;
					item.time = dateTime.timestamp().epochTime();
					dataItems.push_back(item);
					result = true;
				}
			}
		}
	}

	return result;
}

int Frame::buildConnectReq(int pseq, int rseq)
{
	int funCode = 0;
	int Rseq = rseq;
	int Pseq = pseq;
	int protocolVersion = 33;
	int protocolFlag = 0;

	//加载固定帧模板
	if (!loadFrame(_frame, (_protocol_path + _fixedFrame).c_str()))
	{
		cout<<"loadFrame failed"<<endl;
		return -2;
	}

	if (!setAddress(_parent->_areaCode, _parent->_termAddr, _parent->_masterAddr))
	{
		cout<<"setAddress failed"<<endl;
		return -3;
	}

//	bool Frame::setControl(int funCode = 0, int Rseq = 0, int Pseq = 0, bool Reserve = false, bool Prm = false, bool Dir = false)
	if (!setControl(funCode, Rseq, Pseq))
	{
		cout<<"setControl failed"<<endl;
		return -4;
	}

//	bool Frame::setProtocol(int protocolVersion = 0, int protocolFlag = 0, int protocolReserve = 0)
	if (!setProtocol(protocolVersion, protocolFlag))
	{
		cout<<"setProtocol failed"<<endl;
		return -5;
	}

	if (!encode())
	{
		cout<<"encode failed"<<endl;
		return -6;
	}

	return _length;
}

int Frame::buildHeartbeat(int pseq, int rseq)
{
	int funCode = 3;
	int Rseq = rseq;
	int Pseq = pseq;
	bool Reserve = false;
	bool Prm = true;
	int protocolVersion = 33;
	int protocolFlag = 0;

	//加载固定帧模板
	if (!loadFrame(_frame, (_protocol_path + _fixedFrame).c_str()))
	{
		cout<<"loadFrame failed"<<endl;
		return -2;
	}

	if (!setAddress(_parent->_areaCode, _parent->_termAddr, _parent->_masterAddr))
	{
		cout<<"setAddress failed"<<endl;
		return -3;
	}

	if (!setControl(funCode, Rseq, Pseq, Reserve, Prm))
	{
		cout<<"setControl failed"<<endl;
		return -4;
	}

	if (!setProtocol(protocolVersion, protocolFlag))
	{
		cout<<"setProtocol failed"<<endl;
		return -5;
	}

	if (!encode())
	{
		cout<<"encode failed"<<endl;
		return -6;
	}

	return _length;
}

int Frame::buildConfirmReq(int pseq, int rseq)
{
	int funCode = 0;
	int Rseq = rseq;
	int Pseq = pseq;
	bool Reserve = false;
	bool Prm = false;
	int protocolVersion = 33;
	int protocolFlag = 0;

	//加载固定帧模板
	if (!loadFrame(_frame, (_protocol_path + _fixedFrame).c_str()))
	{
		cout<<"loadFrame failed"<<endl;
		return -2;
	}
	if (!setAddress(_parent->_areaCode, _parent->_termAddr, _parent->_masterAddr))
	{
		cout<<"setAddress failed"<<endl;
		return -3;
	}

	if (!setControl(funCode, Rseq, Pseq, Reserve, Prm))
	{
		cout<<"setControl failed"<<endl;
		return -4;
	}

	if (!setProtocol(protocolVersion, protocolFlag))
	{
		cout<<"setProtocol failed"<<endl;
		return -5;
	}

	if (!encode())
	{
		cout<<"encode failed"<<endl;
		return -6;
	}

	return _length;
}

int Frame::buildRequestRealTimeData(int pseq, int rseq,
		vector<DataItem> &dataItems)//请求实时数据
{
	int funCode = 2;
	int Rseq = rseq;
	int Pseq = pseq;
	int protocolVersion = 33;
	int protocolFlag = 0;
	int afn = 13;
	bool acd = false;
	bool tpv = false;

	//加载可变帧模板
	if (!loadFrame(_frame, (_protocol_path + _verFrame).c_str()))
	{
		cout<<"loadFrame failed"<<endl;
		return -2;
	}

	if (!setAddress(_parent->_areaCode, _parent->_termAddr, _parent->_masterAddr))
	{
		cout<<"setAddress failed"<<endl;
		return -3;
	}

	//	bool Frame::setControl(int funCode = 0, int Rseq = 0, int Pseq = 0, bool Reserve = false, bool Prm = false, bool Dir = false)
	if (!setControl(funCode, Rseq, Pseq))
	{
		cout<<"setControl failed"<<endl;
		return -4;
	}

	//	bool Frame::setProtocol(int protocolVersion = 0, int protocolFlag = 0, int protocolReserve = 0)
	if (!setProtocol(protocolVersion, protocolFlag))
	{
		cout<<"setProtocol failed"<<endl;
		return -5;
	}

	//	bool Frame::setAppControl(int afn = 0, bool acd = false, bool tpv = false)
	if (!setAppControl(afn, acd, tpv))
	{
		cout<<"setAppControl failed"<<endl;
		return -6;
	}

	//设置数据
	_frame._dataItems = dataItems;

	if (!encode())
	{
		cout<<"encode failed"<<endl;
		return -7;
	}

	return _length;
}

int Frame::buildMeterConnectConfig(int pseq, int rseq,
		vector<DataItem> &dataItems)//表的连接配置信息
{
	int funCode = 25;
	int Rseq = rseq;
	int Pseq = pseq;
	int protocolVersion = 33;
	int protocolFlag = 0;
	int afn = 4;
	bool acd = false;
	bool tpv = false;

	//加载可变帧模板
	if (!loadFrame(_frame, (_protocol_path + _verFrame).c_str()))
	{
		cout<<"loadFrame failed"<<endl;
		return -2;
	}

	if (!setAddress(_parent->_areaCode, _parent->_termAddr, _parent->_masterAddr))
	{
		cout<<"setAddress failed"<<endl;
		return -3;
	}

	//	bool Frame::setControl(int funCode = 0, int Rseq = 0, int Pseq = 0, bool Reserve = false, bool Prm = false, bool Dir = false)
	if (!setControl(funCode, Rseq, Pseq))
	{
		cout<<"setControl failed"<<endl;
		return -4;
	}

	//	bool Frame::setProtocol(int protocolVersion = 0, int protocolFlag = 0, int protocolReserve = 0)
	if (!setProtocol(protocolVersion, protocolFlag))
	{
		cout<<"setProtocol failed"<<endl;
		return -5;
	}

	//	bool Frame::setAppControl(int afn = 0, bool acd = false, bool tpv = false)
	if (!setAppControl(afn, acd, tpv))
	{
		cout<<"setAppControl failed"<<endl;
		return -6;
	}

	//设置数据
	_frame._dataItems = dataItems;

	if (!encode())
	{
		cout<<"encode failed"<<endl;
		return -7;
	}

	return _length;
}

bool Frame::setAddress(int areaCode, int termAddr, int masterAddr, bool Group)
{
//	bool modifyAreaCode(FrameBase &frame, int areaCode);
	if (areaCode != 0)
	{
		if (!modifyAreaCode(_frame, areaCode))
		{
			return false;
		}
	}
//	bool modifyTermAddr(FrameBase &frame, int termAddr);
	if (termAddr != 0)
	{
		if (!modifyTermAddr(_frame, termAddr))
		{
			return false;
		}
	}
//	bool modifyMasterAddr(FrameBase &frame, int masterAddr);
	if (masterAddr != 0)
	{
		if (!modifyMasterAddr(_frame, masterAddr))
		{
			return false;
		}
	}
//	bool modifyGroup(FrameBase &frame, bool Group);
	if (Group != false)
	{
		if (!modifyGroup(_frame, Group))
		{
			return false;
		}
	}
	return true;
}

bool Frame::setControl(int funCode, int Rseq, int Pseq, bool Reserve, bool Prm, bool Dir)
{
	if (funCode != 0)
	{
		if (!modifyFunCode(_frame, funCode))
		{
			return false;
		}
	}
	if (Rseq != 0)
	{
		if (!modifyRseq(_frame, Rseq))
		{
			return false;
		}
	}
	if (Pseq != 0)
	{
		if (!modifyPseq(_frame, Pseq))
		{
			return false;
		}
	}
	if (Reserve != false)
	{
		if (!modifyReserve(_frame, Reserve))
		{
			return false;
		}
	}
	if (Prm != false)
	{
		if (!modifyPrm(_frame, Prm))
		{
			return false;
		}
	}
	if (Dir != false)
	{
		if (!modifyDir(_frame, Dir))
		{
			return false;
		}
	}

	return true;
}

bool Frame::setProtocol(int protocolVersion, int protocolFlag, int protocolReserve)
{
	if (protocolVersion != 0)
	{
		if (!modifyProtocolVersion(_frame, protocolVersion))
		{
			return false;
		}
	}
	if (protocolFlag != 0)
	{
		if (!modifyProtocolFlag(_frame, protocolFlag))
		{
			return false;
		}
	}
	if (protocolReserve != 0)
	{
		if (!modifyProtocolReserve(_frame, protocolReserve))
		{
			return false;
		}
	}

	return true;
}

//verFrame
bool Frame::setAppControl(int afn, bool acd, bool tpv)
{
	if (afn != 0)
	{
		if (!modifyAcAfn(_frame, afn))
		{
			return false;
		}
	}

	if (acd != false)
	{
		if (!modifyAcAcd(_frame, acd))
		{
			return false;
		}
	}

	if (tpv != false)
	{
		if (!modifyAcTpv(_frame, tpv))
		{
			return false;
		}
	}

	return true;
}

bool Frame::setEventCount(int ec1, int ec2)
{
	if (ec1 != 0)
	{
		if (!modifyEc1(_frame, ec1))
		{
			return false;
		}
	}
	if (ec2 != 0)
	{
		if (!modifyEc2(_frame, ec2))
		{
			return false;
		}
	}

	return true;
}

bool Frame::setPassword(string password)
{
	if (password != "none")
	{
		if (!modifyPassword(_frame, password))
		{
			return false;
		}
	}

	return true;
}

bool Frame::setTpv(int timeout)
{
	if (timeout != 0)
	{
		if (!modifyTpv(_frame, timeout))
		{
			return false;
		}
	}

	return true;
}

bool Frame::encode()
{
	int state = Begin;

	_fsm->encodeBegin(_frame);
	while (state != EncodeFinish)
	{
		if (state == BadFrame)
		{
			break;
		}
		_fsm->encode();
		state = _fsm.GetState();
	}
	if (!_fsm->process())
	{
		return false;
	}

	_length = MAX_FRAME_LENGTH;
	_fsm->encodeEnd(_data, _length);
	if (_length < FIXED_FRAME_LENGTH)
	{
		return false;
	}

	return true;
}

bool Frame::decode()
{
	int state = Begin;

	_fsm->decodeBegin(_data, _length);
	while(state != DecodeFinish)
	{
		if (state == BadFrame)
		{
			break;
		}
		_fsm->decode();
		state = _fsm.GetState();
	}
	if (!_fsm->process())
	{
		return false;
	}

	_fsm->decodeEnd(_frame);

	return true;

}

bool Frame::decode(const char *pFrame, int &length, FrameNx &frame)
{
	FrameNxFSM fsm;
	int state = Begin;

	fsm->decodeBegin((uint8_t*)pFrame, length);
	while(state != DecodeFinish)
	{
		if (state == BadFrame)
		{
			break;
		}
		fsm->decode();
		state = fsm.GetState();
	}
	if (!fsm->process())
	{
		return false;
	}

	fsm->decodeEnd(frame);

	return true;
}

}//namespace NX

}//namespace Protocol

}//namespace Uarc
