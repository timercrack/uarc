#include <iostream>
#include <string.h>
#include <string>

#include "ParseFrame.h"

using namespace std;

void str_to_hex(string &str, uint8_t *fdata, int &length)
{
	int i = 0;

	for (string::size_type index = 0; index < str.size(); index += 2)
	{
		fdata[i++] = strtoul(str.substr(index, 2).c_str(), NULL, 16);
	}

	length = i;
}

void hex_to_str(string &str, uint8_t *fdata, int &length)
{
	str.clear();
	for (int i = 0; i < length; ++i)
	{
		char tmp[3] = {0};
		sprintf(tmp, "%02X", fdata[i]);
		str += tmp;
	}
}

bool find(TableFile &file, const string &key, string &value)
{
	map<int, int> rc;
	if (!file.find(key, rc, -1, 0))
	{
		return false;
	}else if ((int)rc.size() != 1)
	{
		return false;
	}else {
		value = file.value(rc.begin()->first, 1);
	}

	return true;
}

bool isBool(const string &value, bool &boolean)
{
	if (value == string("false")
			|| value == string("False")
			|| value == string("FALSE"))
	{
		boolean = false;
	}else if (value == string("true")
			|| value == string("True")
			|| value == string("TRUE"))
	{
		boolean = true;
	}else {
		return false;
	}

	return true;
}

bool loadControl(FrameBase &frame, TableFile &file)
{
	string tmp;
	::Control control;

	if (!find(file, "funCode", tmp))
	{
		cout<<"funCode not found"<<endl;
		return false;
	}
	if (!control.set<int>("funCode", atoi(tmp.c_str())))
	{
		cout<<"set funCode failed"<<endl;
		return false;
	}

	if (!find(file, "Rseq", tmp))
	{
		cout<<"Rseq not found"<<endl;
		return false;
	}
	if (!control.set<int>("Rseq", atoi(tmp.c_str())))
	{
		cout<<"set Rseq failed"<<endl;
		return false;
	}

	if (!find(file, "Pseq", tmp))
	{
		cout<<"Pseq not found"<<endl;
		return false;
	}
	if (!control.set<int>("Pseq", atoi(tmp.c_str())))
	{
		cout<<"set Pseq failed"<<endl;
		return false;
	}

	if (!find(file, "Reserve", tmp))
	{
		cout<<"Reserve not found"<<endl;
		return false;
	}
	bool Reserve;
	if (!isBool(tmp, Reserve))
	{
		cout<<"Reserve is not bool"<<endl;
		return false;
	}
	if (!control.set<bool>("Reserve", Reserve))
	{
		cout<<"set Reserve failed"<<endl;
		return false;
	}

	if (!find(file, "Prm", tmp))
	{
		cout<<"Prm not found"<<endl;
		return false;
	}
	bool Prm;
	if (!isBool(tmp, Prm))
	{
		cout<<"Prm is not bool"<<endl;
		return false;
	}
	if (!control.set<bool>("Prm", Prm))
	{
		cout<<"set Prm failed"<<endl;
		return false;
	}

	if (!find(file, "Dir", tmp))
	{
		cout<<"Dir not found"<<endl;
		return false;
	}
	bool Dir;
	if (!isBool(tmp, Dir))
	{
		cout<<"Dir is not bool"<<endl;
		return false;
	}
	if (!control.set<bool>("Dir", Dir))
	{
		cout<<"set Dir failed"<<endl;
		return false;
	}

//	cdata->Control_D0_D3_func = 10;
//	cdata->Control_D4_D7_resq = 2;
//	cdata->Control_D8_D12_pesq = 5;
//	cdata->Control_D13_reserve = false;
//	cdata->Control_D14_prm = false;
//	cdata->Control_D15_dir = true;

	if (!frame.set< ::Control >("control", control))
	{
		cout<<"set control failed"<<endl;
		return false;
	}

	return true;
}

bool loadAddress(FrameBase &frame, TableFile &file)
{
	string tmp;
	Address address;

	if (!find(file, "areaCode", tmp))
	{
		cout<<"areaCode not found"<<endl;
		return false;
	}
	if (!address.set<int>("areaCode", atoi(tmp.c_str())))
	{
		cout<<"set areaCode failed"<<endl;
		return false;
	}

	if (!find(file, "termAddr", tmp))
	{
		cout<<"termAddr not found"<<endl;
		return false;
	}
	if (!address.set<int>("termAddr", atoi(tmp.c_str())))
	{
		cout<<"set termAddr failed"<<endl;
		return false;
	}

	if (!find(file, "masterAddr", tmp))
	{
		cout<<"masterAddr not found"<<endl;
		return false;
	}
	if (!address.set<int>("masterAddr", atoi(tmp.c_str())))
	{
		cout<<"set masterAddr failed"<<endl;
		return false;
	}

	bool Group;
	if (!find(file, "Group", tmp))
	{
		cout<<"Group not found"<<endl;
		return false;
	}
	if (!isBool(tmp, Group))
	{
		cout<<"Group is not bool"<<endl;
		return false;
	}
	if (!address.set<bool>("Group", Group))
	{
		cout<<"set Group failed"<<endl;
		return false;
	}

//	address.set<int>("areaCode", adata->A1B1Uni  + adata->A1B1Ten * 10 + adata->A1B2Uni * 100 + adata->A1B2Ten * 1000 +
//			adata->A1B3Uni * 10000 + adata->A1B3Ten * 100000);
//	address.set<int>("termAddr",
//			((adata->A2Byte1 & 0xff) +
//					((adata->A2Byte2 <<  8) & 0xff00) +
//					((adata->A2Byte3 << 16) & 0xff0000) +
//					((adata->A2Byte4 << 24) & 0xff000000)));
//	address.set<int>("masterAddr", adata->Addr3 >> 1);
//	address.set<bool>("Group", adata->Addr3 & 0x1);

	if (!frame.set<Address>("address", address))
	{
		cout<<"set address failed"<<endl;
		return false;
	}

	return true;
}

bool loadAppControl(FrameBase &frame, TableFile &file)
{
	AppControl appControl;
	string tmp;
	if (!find(file, "acAfn", tmp))
	{
		cout<<"acAfn not found"<<endl;
		return false;
	}
	if (!appControl.set<int>("acAfn", atoi(tmp.c_str())))
	{
		cout<<"set acAfn failed"<<endl;
		return false;
	}

	bool acAcd;
	if (!find(file, "acAcd", tmp))
	{
		cout<<"acAcd not found"<<endl;
		return false;
	}
	if (!isBool(tmp, acAcd))
	{
		cout<<"acAcd is not bool"<<endl;
		return false;
	}
	if (!appControl.set<bool>("acAcd", acAcd))
	{
		cout<<"set acAcd failed"<<endl;
		return false;
	}

	bool acTpv;
	if (!find(file, "acTpv", tmp))
	{
		cout<<"acTpv not found"<<endl;
		return false;
	}
	if (!isBool(tmp, acTpv))
	{
		cout<<"acTpv is not bool"<<endl;
		return false;
	}
	if (!appControl.set<bool>("acTpv", acTpv))
	{
		cout<<"set acTpv failed"<<endl;
		return false;
	}

	if (!frame.set<AppControl>("appControl", appControl))
	{
		cout<<"set appControl failed"<<endl;
		return false;
	}

	return true;
}

bool loadEventCount(FrameBase &frame, TableFile &file)
{
	::Control control;
	if (!frame.get< ::Control >("control", control))
	{
		cout<<"get control failed"<<endl;
		return false;
	}
	bool Dir = false;
	if (!control.get<bool>("Dir", Dir))
	{
		cout<<"get Dir failed"<<endl;
		return false;
	}

	AppControl appControl;
	if (!frame.get<AppControl>("appControl", appControl))
	{
		cout<<"get appControl failed"<<endl;
		return false;
	}
	bool acAcd = false;
	if (!appControl.get<bool>("acAcd", acAcd))
	{
		cout<<"get acAcd failed"<<endl;
		return false;
	}

	bool acTpv = false;
	if (!appControl.get<bool>("acTpv", acTpv))
	{
		cout<<"get acTpv failed"<<endl;
		return false;
	}

	int acAfn;
	if (!appControl.get<int>("acAfn", acAfn))
	{
		cout<<"get acAfn failed"<<endl;
		return false;
	}

	bool hasEventCount;
	if (Dir && acAcd)
	{
		hasEventCount = true;
	}else {
		hasEventCount = false;
	}
	if (!frame.set<bool>("hasEventCount", hasEventCount))
	{
		cout<<"set hasEventCount failed"<<endl;
		return false;
	}

//	if (hasEventCount)
	{
		FEvent ec;
		uint16_t ec1 = 0;
		uint16_t ec2 = 0;

		string tmp;
		if (!find(file, "ec1", tmp))
		{
			cout<<"ec1 not found"<<endl;
			return false;
		}
		ec1 = atoi(tmp.c_str());

		if (!find(file, "ec2", tmp))
		{
			cout<<"ec2 not found"<<endl;
			return false;
		}
		ec2 = atoi(tmp.c_str());

		if (!ec.set<int>("ec1", ((ec1 >> 8) & 0xff) + ((ec1 << 8) & 0xff00)))
		{
			cout<<"set ec1 failed"<<endl;
			return false;
		}

		if (!ec.set<int>("ec2", ((ec2 >> 8) & 0xff) + ((ec2 << 8) & 0xff00)))
		{
			cout<<"set ec2 failed"<<endl;
			return false;
		}

		if (!frame.set<FEvent>("ec", ec))
		{
			cout<<"set ec failed"<<endl;
			return false;
		}
	}

	return true;
}

bool loadPassword(FrameBase &frame, TableFile &file)
{
	::Control control;
	if (!frame.get< ::Control >("control", control))
	{
		cout<<"get control failed"<<endl;
		return false;
	}
	bool Dir = false;
	if (!control.get<bool>("Dir", Dir))
	{
		cout<<"get Dir failed"<<endl;
		return false;
	}

	AppControl appControl;
	if (!frame.get<AppControl>("appControl", appControl))
	{
		cout<<"get appControl failed"<<endl;
		return false;
	}
	bool acAcd;
	if (!appControl.get<bool>("acAcd", acAcd))
	{
		cout<<"get acAcd failed"<<endl;
		return false;
	}

	bool acTpv;
	if (!appControl.get<bool>("acTpv", acTpv))
	{
		cout<<"get acTpv failed"<<endl;
		return false;
	}

	int acAfn;
	if (!appControl.get<int>("acAfn", acAfn))
	{
		cout<<"get acAfn failed"<<endl;
		return false;
	}

	bool hasPassword;
	if ((!Dir)
		&& ((acAfn == 1) || (acAfn == 4) || (acAfn == 5) || (acAfn == 6) || (acAfn == 15) || (acAfn == 16))
		&& (!acTpv))
	{
		hasPassword = true;
	}else {
		hasPassword = false;
	}
	if (!frame.set<bool>("hasPassword", hasPassword))
	{
		cout<<"set hasPassword failed"<<endl;
		return false;
	}

//	if (hasPassword)
	{
		Password password;
		string tmp;
		if (!find(file, "password", tmp))
		{
			cout<<"password not found"<<endl;
			return false;
		}
		if (!password.set<string>("password", tmp))
		{
			cout<<"set password failed"<<endl;
			return false;
		}

		if (!frame.set<Password>("password", password))
		{
			cout<<"set password failed"<<endl;
			return false;
		}
	}

	return true;
}

bool loadTpv(FrameBase &frame, TableFile &file)
{
	::Control control;
	if (!frame.get< ::Control >("control", control))
	{
		cout<<"get control failed"<<endl;
		return false;
	}
	bool Dir;
	if (!control.get<bool>("Dir", Dir))
	{
		cout<<"get Dir failed"<<endl;
		return false;
	}

	AppControl appControl;
	if (!frame.get<AppControl>("appControl", appControl))
	{
		cout<<"get appControl failed"<<endl;
		return false;
	}
	bool acAcd;
	if (!appControl.get<bool>("acAcd", acAcd))
	{
		cout<<"get acAcd failed"<<endl;
		return false;
	}

	bool acTpv;
	if (!appControl.get<bool>("acTpv", acTpv))
	{
		cout<<"get acTpv failed"<<endl;
		return false;
	}

	int acAfn;
	if (!appControl.get<int>("acAfn", acAfn))
	{
		cout<<"get acAfn failed"<<endl;
		return false;
	}

	bool hasPassword;
	if (!frame.get<bool>("hasPassword", hasPassword))
	{
		cout<<"get hasPassword failed"<<endl;
		return false;
	}

	bool hasTpv;
	if ((!hasPassword) && acTpv)
	{
		hasTpv = true;
	}else {
		hasTpv = false;
	}
	if (!frame.set<bool>("hasTpv", hasTpv))
	{
		cout<<"set hasPassword failed"<<endl;
		return false;
	}

//	if (hasTpv)
	{
		string tmp;
		TimeStamp timeStamp;

		//写入时间戳
		time_t rawtime;
		time(&rawtime);
		if (!timeStamp.set<time_t>("timeStamp", rawtime))
		{
			cout<<"set timeStamp failed"<<endl;
			return false;
		}

		//写入超时时间
		if (!find(file, "timeout", tmp))
		{
			cout<<"timeout not found"<<endl;
			return false;
		}
		if (!timeStamp.set<int>("timeout", atoi(tmp.c_str())))
		{
			cout<<"set timeout failed"<<endl;
			return false;
		}

		//写入Tpv
		if (!frame.set<TimeStamp>("timeStamp", timeStamp))
		{
			cout<<"set timeStamp failed"<<endl;
			return false;
		}
	}

	return true;
}

bool loadFixedFrame(FrameBase &frame, TableFile &file)
{
	FixedFrame fixedFrame;

	if (!loadControl(fixedFrame, file))
	{
		cout<<"loadControl failed"<<endl;
		return false;
	}

	if (!loadAddress(fixedFrame, file))
	{
		cout<<"loadAddress failed"<<endl;
		return false;
	}

	string tmp;
	if (!find(file, "protocolVersion", tmp))
	{
		cout<<"protocolVersion not found"<<endl;
		return false;
	}
	if (!fixedFrame.set<int>("protocolVersion", atoi(tmp.c_str())))
	{
		cout<<"set protocolVersion failed"<<endl;
		return false;
	}

	if (!find(file, "protocolFlag", tmp))
	{
		cout<<"protocolFlag not found"<<endl;
		return false;
	}
	if (!fixedFrame.set<int>("protocolFlag", atoi(tmp.c_str())))
	{
		cout<<"set protocolFlag failed"<<endl;
		return false;
	}

	if (!find(file, "protocolReserve", tmp))
	{
		cout<<"protocolReserve not found"<<endl;
		return false;
	}
	if (!fixedFrame.set<int>("protocolReserve", atoi(tmp.c_str())))
	{
		cout<<"set protocolReserve failed"<<endl;
		return false;
	}

	if (!frame.set<FixedFrame>("fixedFrame", fixedFrame))
	{
		cout<<"set fixedFrame failed"<<endl;
		return false;
	}

	return true;
}

bool loadVerFrame(FrameBase &frame, TableFile &file)
{
	VerFrame verFrame;
	if (!loadControl(verFrame, file))
	{
		cout<<"loadControl failed"<<endl;
		return false;
	}

	if (!loadAddress(verFrame, file))
	{
		cout<<"loadAddress failed"<<endl;
		return false;
	}

	string tmp;
	if (!find(file, "protocolVersion", tmp))
	{
		cout<<"protocolVersion not found"<<endl;
		return false;
	}
	if (!verFrame.set<int>("protocolVersion", atoi(tmp.c_str())))
	{
		cout<<"set protocolVersion failed"<<endl;
		return false;
	}

	if (!find(file, "protocolFlag", tmp))
	{
		cout<<"protocolFlag not found"<<endl;
		return false;
	}
	if (!verFrame.set<int>("protocolFlag", atoi(tmp.c_str())))
	{
		cout<<"set protocolFlag failed"<<endl;
		return false;
	}

	if (!find(file, "protocolReserve", tmp))
	{
		cout<<"protocolReserve not found"<<endl;
		return false;
	}
	if (!verFrame.set<int>("protocolReserve", atoi(tmp.c_str())))
	{
		cout<<"set protocolReserve failed"<<endl;
		return false;
	}

	if (!loadAppControl(verFrame, file))
	{
		cout<<"loadAppControl failed"<<endl;
		return false;
	}

	if (!loadEventCount(verFrame, file))
	{
		cout<<"loadEventCount failed"<<endl;
		return false;
	}

	if (!loadPassword(verFrame, file))
	{
		cout<<"loadPassword failed"<<endl;
		return false;
	}

	if (!loadTpv(verFrame, file))
	{
		cout<<"loadTpv failed"<<endl;
		return false;
	}

	if (!frame.set<VerFrame>("verFrame", verFrame))
	{
		cout<<"set verFrame failed"<<endl;
		return false;
	}

//	if (!loadUserData(frame, file))
//	{
//		cout<<"loadUserData failed"<<endl;
//		return false;
//	}

	return true;
}

bool loadFrame(FrameBase &frame, const char *file)
{
	string fileName(file);
	TableFile tf(fileName);

	tf.creatRef();

	string tmp;
	bool isFixedFrame;
	if (!find(tf, "isFixedFrame", tmp))
	{
		cout<<"isFixedFrame not found"<<endl;
		return false;
	}
	if (!isBool(tmp, isFixedFrame))
	{
		cout<<"isFixedFrame is not bool"<<endl;
		return false;
	}
	if (!frame.set<bool>("isFixedFrame", isFixedFrame))
	{
		cout<<"set isFixedFrame failed"<<endl;
		return false;
	}

	if (isFixedFrame)
	{
		return loadFixedFrame(frame, tf);
	}else {
		return loadVerFrame(frame, tf);
	}
}

///////////////////////////modify
//Address
bool modifyAreaCode(FrameBase &frame, int areaCode)
{
	bool isFixedFrame = false;

	if (!frame.get<bool>("isFixedFrame", isFixedFrame))
	{
		cout<<"modifyAreaCode:get isFixedFrame failed"<<endl;
		return false;
	}

	if (isFixedFrame)
	{
		FixedFrame fixedFrame;
		if (!frame.get<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyAreaCode:get fixedFrame failed"<<endl;
			return false;
		}
		Address address;
		if (!fixedFrame.get<Address>("address", address))
		{
			cout<<"modifyAreaCode:get address failed"<<endl;
			return false;
		}
		if (!address.set<int>("areaCode", areaCode))
		{
			cout<<"modifyAreaCode:set areaCode failed"<<endl;
			return false;
		}
		if (!fixedFrame.set<Address>("address", address))
		{
			cout<<"modifyAreaCode:set address failed"<<endl;
			return false;
		}
		if (!frame.set<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyAreaCode:set fixedFrame failed"<<endl;
			return false;
		}

	}else {
		VerFrame verFrame;
		if (!frame.get<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyAreaCode:get verFrame failed"<<endl;
			return false;
		}
		Address address;
		if (!verFrame.get<Address>("address", address))
		{
			cout<<"modifyAreaCode:get address failed"<<endl;
			return false;
		}
		if (!address.set<int>("areaCode", areaCode))
		{
			cout<<"modifyAreaCode:set areaCode failed"<<endl;
			return false;
		}
		if (!verFrame.set<Address>("address", address))
		{
			cout<<"modifyAreaCode:set address failed"<<endl;
			return false;
		}
		if (!frame.set<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyAreaCode:set verFrame failed"<<endl;
			return false;
		}
	}

	return true;
}

bool modifyTermAddr(FrameBase &frame, int termAddr)
{
	bool isFixedFrame = false;

	if (!frame.get<bool>("isFixedFrame", isFixedFrame))
	{
		cout<<"modifyTermAddr:get isFixedFrame failed"<<endl;
		return false;
	}

	if (isFixedFrame)
	{
		FixedFrame fixedFrame;
		if (!frame.get<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyTermAddr:get fixedFrame failed"<<endl;
			return false;
		}
		Address address;
		if (!fixedFrame.get<Address>("address", address))
		{
			cout<<"modifyTermAddr:get address failed"<<endl;
			return false;
		}
		if (!address.set<int>("termAddr", termAddr))
		{
			cout<<"modifyTermAddr:set termAddr failed"<<endl;
			return false;
		}
		if (!fixedFrame.set<Address>("address", address))
		{
			cout<<"modifyTermAddr:set address failed"<<endl;
			return false;
		}
		if (!frame.set<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyTermAddr:set fixedFrame failed"<<endl;
			return false;
		}

	}else {
		VerFrame verFrame;
		if (!frame.get<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyTermAddr:get verFrame failed"<<endl;
			return false;
		}
		Address address;
		if (!verFrame.get<Address>("address", address))
		{
			cout<<"modifyTermAddr:get address failed"<<endl;
			return false;
		}
		if (!address.set<int>("termAddr", termAddr))
		{
			cout<<"modifyTermAddr:set termAddr failed"<<endl;
			return false;
		}
		if (!verFrame.set<Address>("address", address))
		{
			cout<<"modifyTermAddr:set address failed"<<endl;
			return false;
		}
		if (!frame.set<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyTermAddr:set verFrame failed"<<endl;
			return false;
		}
	}

	return true;
}

bool modifyMasterAddr(FrameBase &frame, int masterAddr)
{
	bool isFixedFrame = false;

	if (!frame.get<bool>("isFixedFrame", isFixedFrame))
	{
		cout<<"modifyMasterAddr:get isFixedFrame failed"<<endl;
		return false;
	}

	if (isFixedFrame)
	{
		FixedFrame fixedFrame;
		if (!frame.get<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyMasterAddr:get fixedFrame failed"<<endl;
			return false;
		}
		Address address;
		if (!fixedFrame.get<Address>("address", address))
		{
			cout<<"modifyMasterAddr:get address failed"<<endl;
			return false;
		}
		if (!address.set<int>("masterAddr", masterAddr))
		{
			cout<<"modifyMasterAddr:set masterAddr failed"<<endl;
			return false;
		}
		if (!fixedFrame.set<Address>("address", address))
		{
			cout<<"modifyMasterAddr:set address failed"<<endl;
			return false;
		}
		if (!frame.set<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyMasterAddr:set fixedFrame failed"<<endl;
			return false;
		}

	}else {
		VerFrame verFrame;
		if (!frame.get<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyMasterAddr:get verFrame failed"<<endl;
			return false;
		}
		Address address;
		if (!verFrame.get<Address>("address", address))
		{
			cout<<"modifyMasterAddr:get address failed"<<endl;
			return false;
		}
		if (!address.set<int>("masterAddr", masterAddr))
		{
			cout<<"modifyMasterAddr:set masterAddr failed"<<endl;
			return false;
		}
		if (!verFrame.set<Address>("address", address))
		{
			cout<<"modifyMasterAddr:set address failed"<<endl;
			return false;
		}
		if (!frame.set<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyMasterAddr:set verFrame failed"<<endl;
			return false;
		}
	}

	return true;
}

bool modifyGroup(FrameBase &frame, bool Group)
{
	bool isFixedFrame = false;

	if (!frame.get<bool>("isFixedFrame", isFixedFrame))
	{
		cout<<"modifyGroup:get isFixedFrame failed"<<endl;
		return false;
	}

	if (isFixedFrame)
	{
		FixedFrame fixedFrame;
		if (!frame.get<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyGroup:get fixedFrame failed"<<endl;
			return false;
		}
		Address address;
		if (!fixedFrame.get<Address>("address", address))
		{
			cout<<"modifyGroup:get address failed"<<endl;
			return false;
		}
		if (!address.set<bool>("Group", Group))
		{
			cout<<"modifyGroup:set Group failed"<<endl;
			return false;
		}
		if (!fixedFrame.set<Address>("address", address))
		{
			cout<<"modifyGroup:set address failed"<<endl;
			return false;
		}
		if (!frame.set<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyGroup:set fixedFrame failed"<<endl;
			return false;
		}

	}else {
		VerFrame verFrame;
		if (!frame.get<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyGroup:get verFrame failed"<<endl;
			return false;
		}
		Address address;
		if (!verFrame.get<Address>("address", address))
		{
			cout<<"modifyGroup:get address failed"<<endl;
			return false;
		}
		if (!address.set<bool>("Group", Group))
		{
			cout<<"modifyGroup:set Group failed"<<endl;
			return false;
		}
		if (!verFrame.set<Address>("address", address))
		{
			cout<<"modifyGroup:set address failed"<<endl;
			return false;
		}
		if (!frame.set<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyGroup:set verFrame failed"<<endl;
			return false;
		}
	}

	return true;
}

//Control
bool modifyFunCode(FrameBase &frame, int funCode)
{
	bool isFixedFrame = false;

	if (!frame.get<bool>("isFixedFrame", isFixedFrame))
	{
		cout<<"modifyFunCode:get isFixedFrame failed"<<endl;
		return false;
	}

	if (isFixedFrame)
	{
		FixedFrame fixedFrame;
		if (!frame.get<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyFunCode:get fixedFrame failed"<<endl;
			return false;
		}
		::Control control;
		if (!fixedFrame.get< ::Control >("control", control))
		{
			cout<<"modifyFunCode:get control failed"<<endl;
			return false;
		}
		if (!control.set<int>("funCode", funCode))
		{
			cout<<"modifyFunCode:set funCode failed"<<endl;
			return false;
		}
		if (!fixedFrame.set< ::Control >("control", control))
		{
			cout<<"modifyFunCode:set control failed"<<endl;
			return false;
		}
		if (!frame.set<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyFunCode:set fixedFrame failed"<<endl;
			return false;
		}

	}else {
		VerFrame verFrame;
		if (!frame.get<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyFunCode:get verFrame failed"<<endl;
			return false;
		}
		::Control control;
		if (!verFrame.get< ::Control >("control", control))
		{
			cout<<"modifyFunCode:get control failed"<<endl;
			return false;
		}
		if (!control.set<int>("funCode", funCode))
		{
			cout<<"modifyFunCode:set funCode failed"<<endl;
			return false;
		}
		if (!verFrame.set< ::Control >("control", control))
		{
			cout<<"modifyFunCode:set control failed"<<endl;
			return false;
		}
		if (!frame.set<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyFunCode:set verFrame failed"<<endl;
			return false;
		}
	}

	return true;
}

bool modifyRseq(FrameBase &frame, int Rseq)
{
	bool isFixedFrame = false;

	if (!frame.get<bool>("isFixedFrame", isFixedFrame))
	{
		cout<<"modifyRseq:get isFixedFrame failed"<<endl;
		return false;
	}

	if (isFixedFrame)
	{
		FixedFrame fixedFrame;
		if (!frame.get<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyRseq:get fixedFrame failed"<<endl;
			return false;
		}
		::Control control;
		if (!fixedFrame.get< ::Control >("control", control))
		{
			cout<<"modifyRseq:get control failed"<<endl;
			return false;
		}
		if (!control.set<int>("Rseq", Rseq))
		{
			cout<<"modifyRseq:set Rseq failed"<<endl;
			return false;
		}
		if (!fixedFrame.set< ::Control >("control", control))
		{
			cout<<"modifyRseq:set control failed"<<endl;
			return false;
		}
		if (!frame.set<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyRseq:set fixedFrame failed"<<endl;
			return false;
		}

	}else {
		VerFrame verFrame;
		if (!frame.get<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyRseq:get verFrame failed"<<endl;
			return false;
		}
		::Control control;
		if (!verFrame.get< ::Control >("control", control))
		{
			cout<<"modifyRseq:get control failed"<<endl;
			return false;
		}
		if (!control.set<int>("Rseq", Rseq))
		{
			cout<<"modifyRseq:set Rseq failed"<<endl;
			return false;
		}
		if (!verFrame.set< ::Control >("control", control))
		{
			cout<<"modifyRseq:set control failed"<<endl;
			return false;
		}
		if (!frame.set<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyRseq:set verFrame failed"<<endl;
			return false;
		}
	}

	return true;
}

bool modifyPseq(FrameBase &frame, int Pseq)
{
	bool isFixedFrame = false;

	if (!frame.get<bool>("isFixedFrame", isFixedFrame))
	{
		cout<<"modifyPseq:get isFixedFrame failed"<<endl;
		return false;
	}

	if (isFixedFrame)
	{
		FixedFrame fixedFrame;
		if (!frame.get<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyPseq:get fixedFrame failed"<<endl;
			return false;
		}
		::Control control;
		if (!fixedFrame.get< ::Control >("control", control))
		{
			cout<<"modifyPseq:get control failed"<<endl;
			return false;
		}
		if (!control.set<int>("Pseq", Pseq))
		{
			cout<<"modifyPseq:set Pseq failed"<<endl;
			return false;
		}
		if (!fixedFrame.set< ::Control >("control", control))
		{
			cout<<"modifyPseq:set control failed"<<endl;
			return false;
		}
		if (!frame.set<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyPseq:set fixedFrame failed"<<endl;
			return false;
		}

	}else {
		VerFrame verFrame;
		if (!frame.get<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyPseq:get verFrame failed"<<endl;
			return false;
		}
		::Control control;
		if (!verFrame.get< ::Control >("control", control))
		{
			cout<<"modifyPseq:get control failed"<<endl;
			return false;
		}
		if (!control.set<int>("Pseq", Pseq))
		{
			cout<<"modifyPseq:set Pseq failed"<<endl;
			return false;
		}
		if (!verFrame.set< ::Control >("control", control))
		{
			cout<<"modifyPseq:set control failed"<<endl;
			return false;
		}
		if (!frame.set<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyPseq:set verFrame failed"<<endl;
			return false;
		}
	}

	return true;
}

bool modifyReserve(FrameBase &frame, bool Reserve)
{
	bool isFixedFrame = false;

	if (!frame.get<bool>("isFixedFrame", isFixedFrame))
	{
		cout<<"modifyReserve:get isFixedFrame failed"<<endl;
		return false;
	}

	if (isFixedFrame)
	{
		FixedFrame fixedFrame;
		if (!frame.get<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyReserve:get fixedFrame failed"<<endl;
			return false;
		}
		::Control control;
		if (!fixedFrame.get< ::Control >("control", control))
		{
			cout<<"modifyReserve:get control failed"<<endl;
			return false;
		}
		if (!control.set<bool>("Reserve", Reserve))
		{
			cout<<"modifyReserve:set Reserve failed"<<endl;
			return false;
		}
		if (!fixedFrame.set< ::Control >("control", control))
		{
			cout<<"modifyReserve:set control failed"<<endl;
			return false;
		}
		if (!frame.set<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyReserve:set fixedFrame failed"<<endl;
			return false;
		}

	}else {
		VerFrame verFrame;
		if (!frame.get<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyReserve:get verFrame failed"<<endl;
			return false;
		}
		::Control control;
		if (!verFrame.get< ::Control >("control", control))
		{
			cout<<"modifyReserve:get control failed"<<endl;
			return false;
		}
		if (!control.set<bool>("Reserve", Reserve))
		{
			cout<<"modifyReserve:set Reserve failed"<<endl;
			return false;
		}
		if (!verFrame.set< ::Control >("control", control))
		{
			cout<<"modifyReserve:set control failed"<<endl;
			return false;
		}
		if (!frame.set<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyReserve:set verFrame failed"<<endl;
			return false;
		}
	}

	return true;
}

bool modifyPrm(FrameBase &frame, bool Prm)
{
	bool isFixedFrame = false;

	if (!frame.get<bool>("isFixedFrame", isFixedFrame))
	{
		cout<<"modifyPrm:get isFixedFrame failed"<<endl;
		return false;
	}

	if (isFixedFrame)
	{
		FixedFrame fixedFrame;
		if (!frame.get<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyPrm:get fixedFrame failed"<<endl;
			return false;
		}
		::Control control;
		if (!fixedFrame.get< ::Control >("control", control))
		{
			cout<<"modifyPrm:get control failed"<<endl;
			return false;
		}
		if (!control.set<bool>("Prm", Prm))
		{
			cout<<"modifyPrm:set Prm failed"<<endl;
			return false;
		}
		if (!fixedFrame.set< ::Control >("control", control))
		{
			cout<<"modifyPrm:set control failed"<<endl;
			return false;
		}
		if (!frame.set<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyPrm:set fixedFrame failed"<<endl;
			return false;
		}

	}else {
		VerFrame verFrame;
		if (!frame.get<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyPrm:get verFrame failed"<<endl;
			return false;
		}
		::Control control;
		if (!verFrame.get< ::Control >("control", control))
		{
			cout<<"modifyPrm:get control failed"<<endl;
			return false;
		}
		if (!control.set<bool>("Prm", Prm))
		{
			cout<<"modifyPrm:set Prm failed"<<endl;
			return false;
		}
		if (!verFrame.set< ::Control >("control", control))
		{
			cout<<"modifyPrm:set control failed"<<endl;
			return false;
		}
		if (!frame.set<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyPrm:set verFrame failed"<<endl;
			return false;
		}
	}

	return true;
}

bool modifyDir(FrameBase &frame, bool Dir)
{
	bool isFixedFrame = false;

	if (!frame.get<bool>("isFixedFrame", isFixedFrame))
	{
		cout<<"modifyDir:get isFixedFrame failed"<<endl;
		return false;
	}

	if (isFixedFrame)
	{
		FixedFrame fixedFrame;
		if (!frame.get<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyDir:get fixedFrame failed"<<endl;
			return false;
		}
		::Control control;
		if (!fixedFrame.get< ::Control >("control", control))
		{
			cout<<"modifyDir:get control failed"<<endl;
			return false;
		}
		if (!control.set<bool>("Dir", Dir))
		{
			cout<<"modifyDir:set Dir failed"<<endl;
			return false;
		}
		if (!fixedFrame.set< ::Control >("control", control))
		{
			cout<<"modifyDir:set control failed"<<endl;
			return false;
		}
		if (!frame.set<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyDir:set fixedFrame failed"<<endl;
			return false;
		}

	}else {
		VerFrame verFrame;
		if (!frame.get<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyDir:get verFrame failed"<<endl;
			return false;
		}
		::Control control;
		if (!verFrame.get< ::Control >("control", control))
		{
			cout<<"modifyDir:get control failed"<<endl;
			return false;
		}
		if (!control.set<bool>("Dir", Dir))
		{
			cout<<"modifyDir:set Dir failed"<<endl;
			return false;
		}
		if (!verFrame.set< ::Control >("control", control))
		{
			cout<<"modifyDir:set control failed"<<endl;
			return false;
		}
		if (!frame.set<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyDir:set verFrame failed"<<endl;
			return false;
		}
	}

	return true;
}

//protocol
bool modifyProtocolVersion(FrameBase &frame, int protocolVersion)
{
	bool isFixedFrame = false;

	if (!frame.get<bool>("isFixedFrame", isFixedFrame))
	{
		cout<<"modifyProtocolVersion:get isFixedFrame failed"<<endl;
		return false;
	}

	if (isFixedFrame)
	{
		FixedFrame fixedFrame;
		if (!frame.get<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyProtocolVersion:get fixedFrame failed"<<endl;
			return false;
		}
		if (!fixedFrame.set<int>("protocolVersion", protocolVersion))
		{
			cout<<"modifyProtocolVersion:set protocolVersion failed"<<endl;
			return false;
		}
		if (!frame.set<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyProtocolVersion:set fixedFrame failed"<<endl;
			return false;
		}

	}else {
		VerFrame verFrame;
		if (!frame.get<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyProtocolVersion:get verFrame failed"<<endl;
			return false;
		}
		if (!verFrame.set<int>("protocolVersion", protocolVersion))
		{
			cout<<"modifyProtocolVersion:set protocolVersion failed"<<endl;
			return false;
		}
		if (!frame.set<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyProtocolVersion:set verFrame failed"<<endl;
			return false;
		}
	}

	return true;
}

bool modifyProtocolFlag(FrameBase &frame, int protocolFlag)
{
	bool isFixedFrame = false;

	if (!frame.get<bool>("isFixedFrame", isFixedFrame))
	{
		cout<<"modifyProtocolFlag:get isFixedFrame failed"<<endl;
		return false;
	}

	if (isFixedFrame)
	{
		FixedFrame fixedFrame;
		if (!frame.get<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyProtocolFlag:get fixedFrame failed"<<endl;
			return false;
		}
		if (!fixedFrame.set<int>("protocolFlag", protocolFlag))
		{
			cout<<"modifyProtocolFlag:set protocolFlag failed"<<endl;
			return false;
		}
		if (!frame.set<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyProtocolFlag:set fixedFrame failed"<<endl;
			return false;
		}

	}else {
		VerFrame verFrame;
		if (!frame.get<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyProtocolFlag:get verFrame failed"<<endl;
			return false;
		}
		if (!verFrame.set<int>("protocolFlag", protocolFlag))
		{
			cout<<"modifyProtocolFlag:set protocolFlag failed"<<endl;
			return false;
		}
		if (!frame.set<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyProtocolFlag:set verFrame failed"<<endl;
			return false;
		}
	}

	return true;
}

bool modifyProtocolReserve(FrameBase &frame, int protocolReserve)
{
	bool isFixedFrame = false;

	if (!frame.get<bool>("isFixedFrame", isFixedFrame))
	{
		cout<<"modifyProtocolReserve:get isFixedFrame failed"<<endl;
		return false;
	}

	if (isFixedFrame)
	{
		FixedFrame fixedFrame;
		if (!frame.get<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyProtocolReserve:get fixedFrame failed"<<endl;
			return false;
		}
		if (!fixedFrame.set<int>("protocolReserve", protocolReserve))
		{
			cout<<"modifyProtocolReserve:set protocolReserve failed"<<endl;
			return false;
		}
		if (!frame.set<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyProtocolReserve:set fixedFrame failed"<<endl;
			return false;
		}

	}else {
		VerFrame verFrame;
		if (!frame.get<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyProtocolReserve:get verFrame failed"<<endl;
			return false;
		}
		if (!verFrame.set<int>("protocolReserve", protocolReserve))
		{
			cout<<"modifyProtocolReserve:set protocolReserve failed"<<endl;
			return false;
		}
		if (!frame.set<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyProtocolReserve:set verFrame failed"<<endl;
			return false;
		}
	}

	return true;
}

//AppControl
bool modifyAcAfn(FrameBase &frame, int acAfn)
{
	bool isFixedFrame = false;

	if (!frame.get<bool>("isFixedFrame", isFixedFrame))
	{
		cout<<"modifyAcAfn:get isFixedFrame failed"<<endl;
		return false;
	}

	if (isFixedFrame)
	{
		FixedFrame fixedFrame;
		if (!frame.get<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyAcAfn:get fixedFrame failed"<<endl;
			return false;
		}
		AppControl appControl;
		if (!fixedFrame.get<AppControl>("appControl", appControl))
		{
			cout<<"modifyAcAfn:get appControl failed"<<endl;
			return false;
		}
		if (!appControl.set<int>("acAfn", acAfn))
		{
			cout<<"modifyAcAfn:set acAfn failed"<<endl;
			return false;
		}
		if (!fixedFrame.set<AppControl>("appControl", appControl))
		{
			cout<<"modifyAcAfn:set appControl failed"<<endl;
			return false;
		}
		if (!frame.set<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyAcAfn:set fixedFrame failed"<<endl;
			return false;
		}

	}else {
		VerFrame verFrame;
		if (!frame.get<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyAcAfn:get verFrame failed"<<endl;
			return false;
		}
		AppControl appControl;
		if (!verFrame.get<AppControl>("appControl", appControl))
		{
			cout<<"modifyAcAfn:get appControl failed"<<endl;
			return false;
		}
		if (!appControl.set<int>("acAfn", acAfn))
		{
			cout<<"modifyAcAfn:set acAfn failed"<<endl;
			return false;
		}
		if (!verFrame.set<AppControl>("appControl", appControl))
		{
			cout<<"modifyAcAfn:set appControl failed"<<endl;
			return false;
		}
		if (!frame.set<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyAcAfn:set verFrame failed"<<endl;
			return false;
		}
	}

	return true;
}

bool modifyAcAcd(FrameBase &frame, bool acAcd)
{
	bool isFixedFrame = false;

	if (!frame.get<bool>("isFixedFrame", isFixedFrame))
	{
		cout<<"modifyAcAcd:get isFixedFrame failed"<<endl;
		return false;
	}

	if (isFixedFrame)
	{
		FixedFrame fixedFrame;
		if (!frame.get<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyAcAcd:get fixedFrame failed"<<endl;
			return false;
		}
		AppControl appControl;
		if (!fixedFrame.get<AppControl>("appControl", appControl))
		{
			cout<<"modifyAcAcd:get appControl failed"<<endl;
			return false;
		}
		if (!appControl.set<bool>("acAcd", acAcd))
		{
			cout<<"modifyAcAcd:set acAcd failed"<<endl;
			return false;
		}
		if (!fixedFrame.set<AppControl>("appControl", appControl))
		{
			cout<<"modifyAcAcd:set appControl failed"<<endl;
			return false;
		}
		if (!frame.set<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyAcAcd:set fixedFrame failed"<<endl;
			return false;
		}

	}else {
		VerFrame verFrame;
		if (!frame.get<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyAcAcd:get verFrame failed"<<endl;
			return false;
		}
		AppControl appControl;
		if (!verFrame.get<AppControl>("appControl", appControl))
		{
			cout<<"modifyAcAcd:get appControl failed"<<endl;
			return false;
		}
		if (!appControl.set<bool>("acAcd", acAcd))
		{
			cout<<"modifyAcAcd:set acAcd failed"<<endl;
			return false;
		}
		if (!verFrame.set<AppControl>("appControl", appControl))
		{
			cout<<"modifyAcAcd:set appControl failed"<<endl;
			return false;
		}
		if (!frame.set<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyAcAcd:set verFrame failed"<<endl;
			return false;
		}
	}

	return true;
}

bool modifyAcTpv(FrameBase &frame, bool acTpv)
{
	bool isFixedFrame = false;

	if (!frame.get<bool>("isFixedFrame", isFixedFrame))
	{
		cout<<"modifyAcTpv:get isFixedFrame failed"<<endl;
		return false;
	}

	if (isFixedFrame)
	{
		FixedFrame fixedFrame;
		if (!frame.get<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyAcTpv:get fixedFrame failed"<<endl;
			return false;
		}
		AppControl appControl;
		if (!fixedFrame.get<AppControl>("appControl", appControl))
		{
			cout<<"modifyAcTpv:get appControl failed"<<endl;
			return false;
		}
		if (!appControl.set<bool>("acTpv", acTpv))
		{
			cout<<"modifyAcTpv:set acTpv failed"<<endl;
			return false;
		}
		if (!fixedFrame.set<AppControl>("appControl", appControl))
		{
			cout<<"modifyAcTpv:set appControl failed"<<endl;
			return false;
		}
		if (!frame.set<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyAcTpv:set fixedFrame failed"<<endl;
			return false;
		}

	}else {
		VerFrame verFrame;
		if (!frame.get<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyAcTpv:get verFrame failed"<<endl;
			return false;
		}
		AppControl appControl;
		if (!verFrame.get<AppControl>("appControl", appControl))
		{
			cout<<"modifyAcTpv:get appControl failed"<<endl;
			return false;
		}
		if (!appControl.set<bool>("acTpv", acTpv))
		{
			cout<<"modifyAcTpv:set acTpv failed"<<endl;
			return false;
		}
		if (!verFrame.set<AppControl>("appControl", appControl))
		{
			cout<<"modifyAcTpv:set appControl failed"<<endl;
			return false;
		}
		if (!frame.set<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyAcTpv:set verFrame failed"<<endl;
			return false;
		}
	}

	return true;
}

//EventCount
bool modifyEc1(FrameBase &frame, int ec1)
{
	bool isFixedFrame = false;

	if (!frame.get<bool>("isFixedFrame", isFixedFrame))
	{
		cout<<"modifyEc1:get isFixedFrame failed"<<endl;
		return false;
	}

	if (isFixedFrame)
	{
		FixedFrame fixedFrame;
		if (!frame.get<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyEc1:get fixedFrame failed"<<endl;
			return false;
		}
		FEvent ec;
		if (!fixedFrame.get<FEvent>("ec", ec))
		{
			cout<<"modifyEc1:get ec failed"<<endl;
			return false;
		}
		if (!ec.set<int>("ec1", ((ec1 >> 8) & 0xff) + ((ec1 << 8) & 0xff00)))
		{
			cout<<"modifyEc1:set ec1 failed"<<endl;
			return false;
		}
		if (!fixedFrame.set<FEvent>("ec", ec))
		{
			cout<<"modifyEc1:set ec failed"<<endl;
			return false;
		}
		if (!frame.set<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyEc1:set fixedFrame failed"<<endl;
			return false;
		}

	}else {
		VerFrame verFrame;
		if (!frame.get<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyEc1:get verFrame failed"<<endl;
			return false;
		}
		FEvent ec;
		if (!verFrame.get<FEvent>("ec", ec))
		{
			cout<<"modifyEc1:get ec failed"<<endl;
			return false;
		}
		if (!ec.set<int>("ec1", ((ec1 >> 8) & 0xff) + ((ec1 << 8) & 0xff00)))
		{
			cout<<"modifyEc1:set ec1 failed"<<endl;
			return false;
		}
		if (!verFrame.set<FEvent>("ec", ec))
		{
			cout<<"modifyEc1:set ec failed"<<endl;
			return false;
		}
		if (!frame.set<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyEc1:set verFrame failed"<<endl;
			return false;
		}
	}

	return true;
}

bool modifyEc2(FrameBase &frame, int ec2)
{
	bool isFixedFrame = false;

	if (!frame.get<bool>("isFixedFrame", isFixedFrame))
	{
		cout<<"modifyEc2:get isFixedFrame failed"<<endl;
		return false;
	}

	if (isFixedFrame)
	{
		FixedFrame fixedFrame;
		if (!frame.get<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyEc2:get fixedFrame failed"<<endl;
			return false;
		}
		FEvent ec;
		if (!fixedFrame.get<FEvent>("ec", ec))
		{
			cout<<"modifyEc2:get ec failed"<<endl;
			return false;
		}
		if (!ec.set<int>("ec2", ((ec2 >> 8) & 0xff) + ((ec2 << 8) & 0xff00)))
		{
			cout<<"modifyEc2:set ec2 failed"<<endl;
			return false;
		}
		if (!fixedFrame.set<FEvent>("ec", ec))
		{
			cout<<"modifyEc2:set ec failed"<<endl;
			return false;
		}
		if (!frame.set<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyEc2:set fixedFrame failed"<<endl;
			return false;
		}

	}else {
		VerFrame verFrame;
		if (!frame.get<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyEc2:get verFrame failed"<<endl;
			return false;
		}
		FEvent ec;
		if (!verFrame.get<FEvent>("ec", ec))
		{
			cout<<"modifyEc2:get ec failed"<<endl;
			return false;
		}
		if (!ec.set<int>("ec2", ((ec2 >> 8) & 0xff) + ((ec2 << 8) & 0xff00)))
		{
			cout<<"modifyEc2:set ec2 failed"<<endl;
			return false;
		}
		if (!verFrame.set<FEvent>("ec", ec))
		{
			cout<<"modifyEc2:set ec failed"<<endl;
			return false;
		}
		if (!frame.set<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyEc2:set verFrame failed"<<endl;
			return false;
		}
	}

	return true;
}

//Password
bool modifyPassword(FrameBase &frame, string password)
{
	bool isFixedFrame = false;

	if (!frame.get<bool>("isFixedFrame", isFixedFrame))
	{
		cout<<"modifyPassword:get isFixedFrame failed"<<endl;
		return false;
	}

	if (isFixedFrame)
	{
		FixedFrame fixedFrame;
		if (!frame.get<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyPassword:get fixedFrame failed"<<endl;
			return false;
		}
		Password pw;
		if (!fixedFrame.get<Password>("password", pw))
		{
			cout<<"modifyPassword:get pw failed"<<endl;
			return false;
		}
		if (!pw.set<string>("password", password))
		{
			cout<<"modifyPassword:set password failed"<<endl;
			return false;
		}
		if (!fixedFrame.set<Password>("password", pw))
		{
			cout<<"modifyPassword:set pw failed"<<endl;
			return false;
		}
		if (!frame.set<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyPassword:set fixedFrame failed"<<endl;
			return false;
		}

	}else {
		VerFrame verFrame;
		if (!frame.get<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyPassword:get verFrame failed"<<endl;
			return false;
		}
		Password pw;
		if (!verFrame.get<Password>("password", pw))
		{
			cout<<"modifyPassword:get pw failed"<<endl;
			return false;
		}
		if (!pw.set<string>("password", password))
		{
			cout<<"modifyPassword:set password failed"<<endl;
			return false;
		}
		if (!verFrame.set<Password>("password", pw))
		{
			cout<<"modifyPassword:set pw failed"<<endl;
			return false;
		}
		if (!frame.set<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyPassword:set verFrame failed"<<endl;
			return false;
		}
	}

	return true;
}

//Tpv
bool modifyTpv(FrameBase &frame, int timeout)
{
	bool isFixedFrame = false;

	if (!frame.get<bool>("isFixedFrame", isFixedFrame))
	{
		cout<<"modifyTpv:get isFixedFrame failed"<<endl;
		return false;
	}

	if (isFixedFrame)
	{
		FixedFrame fixedFrame;
		if (!frame.get<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyTpv:get fixedFrame failed"<<endl;
			return false;
		}
		TimeStamp timeStamp;
		if (!fixedFrame.get<TimeStamp>("timeStamp", timeStamp))
		{
			cout<<"modifyTpv:get timeStamp failed"<<endl;
			return false;
		}
		if (!timeStamp.set<int>("timeout", timeout))
		{
			cout<<"modifyTpv:set timeout failed"<<endl;
			return false;
		}
		if (!fixedFrame.set<TimeStamp>("timeStamp", timeStamp))
		{
			cout<<"modifyTpv:set timeStamp failed"<<endl;
			return false;
		}
		if (!frame.set<FixedFrame>("fixedFrame", fixedFrame))
		{
			cout<<"modifyTpv:set fixedFrame failed"<<endl;
			return false;
		}

	}else {
		VerFrame verFrame;
		if (!frame.get<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyTpv:get verFrame failed"<<endl;
			return false;
		}
		TimeStamp timeStamp;
		if (!verFrame.get<TimeStamp>("timeStamp", timeStamp))
		{
			cout<<"modifyTpv:get timeStamp failed"<<endl;
			return false;
		}
		if (!timeStamp.set<int>("timeout", timeout))
		{
			cout<<"modifyTpv:set timeout failed"<<endl;
			return false;
		}
		if (!verFrame.set<TimeStamp>("timeStamp", timeStamp))
		{
			cout<<"modifyTpv:set timeStamp failed"<<endl;
			return false;
		}
		if (!frame.set<VerFrame>("verFrame", verFrame))
		{
			cout<<"modifyTpv:set verFrame failed"<<endl;
			return false;
		}
	}

	return true;
}

/*
int main(int argc ,char **argv)
{
	if (argc != 3)
	{
		cout << "Usage:./ParseFrame -c/x encodeFile/parseFrame" << endl;
		return -1;
	}

	uint8_t fdata[MAX_FRAME_LENGTH];
	int length = 0;
	string str;

	FrameNx frame;
	FrameNxFSM pfFSM;

	int state = Begin;

	if (!strcmp(argv[1], "-c"))//编码
	{
		length = MAX_FRAME_LENGTH;

		if (!loadFrame(frame, argv[2]))
		{
			cout<<"loadFrame failed"<<endl;
			return -2;
		}
		pfFSM->encodeBegin(frame);
		while(state != EncodeFinish)
		{
			if (state == BadFrame)
			{
				break;
			}
			pfFSM->encode();
			state = pfFSM.GetState();
		}

		if (pfFSM->process())
		{
			pfFSM->encodeEnd(fdata, length);

			hex_to_str(str, fdata, length);
			cout << str << endl;
		}
	}else if (!strcmp(argv[1], "-x")){//解码
		str = argv[2];
		str_to_hex(str, fdata, length);

		pfFSM->decodeBegin(fdata, length);
		while(state != DecodeFinish)
		{
			if (state == BadFrame)
			{
				break;
			}
			pfFSM->decode();
			state = pfFSM.GetState();
		}
		if (pfFSM->process())
		{
			pfFSM->decodeEnd(frame);

			cout << frame.ToString() << endl;
		}
	}

	return 0;
}*/

