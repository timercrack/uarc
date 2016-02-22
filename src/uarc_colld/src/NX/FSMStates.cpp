#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "FSM.hpp"
#include "FrameNx.h"
#include "FSMEncapsulate.h"
#include "FSMStates.h"

CSMap *CSBaseFactory::__csMap = new CSMap;

void TransitionImpl<ProcessFrameNx, Begin>::encodeBegin(FrameNx &frame)
{
	bool isFixedFrame = true;
	if (!frame.get<bool>("isFixedFrame", isFixedFrame))
	{
		std::cout<<"encodeBegin:get isFixedFrame error!"<<std::endl;
		state = BadFrame;
		return ;
	}

	data = frame;
	if (isFixedFrame)
	{
		state = EncodeFixedPart;
	}else {
		VerFrame verFrame;
		if (!data.get<VerFrame>("verFrame", verFrame))
		{
			state = BadFrame;
			return ;
		}
		AppControl appControl;
		if (!verFrame.get<AppControl>("appControl", appControl))
		{
			state = BadFrame;
			return ;
		}
		if (!appControl.get<int>("acAfn", data.afn))
		{
			state = BadFrame;
			return ;
		}

		if (!data.dataItemsToItems())//发送数据格式转换_dataItems -> _items
		{
			state = BadFrame;
			return ;
		}

		data.fillPnSet();//根据_items填充_pnSet
		data.fillFnSet();//根据_items填充_fnSet
		data._pSetIter = data._pnSet.begin();//初始化pn集合指针
		data._fSetIter = data._fnSet.begin();//初始化fn集合指针

		data.encodePos = data.frameBuf + VERIABLE_LENGTH_BEFORE_PNFNS;//初始化编码指针位置
		state = EncodeVerFramePnfns;
	}

	REGISTER_DATA_TYPE(CSNONE);
	REGISTER_DATA_TYPE(CSXB);
	REGISTER_DATA_TYPE(CSNDQ);
	REGISTER_DATA_TYPE(CSBIN);
	REGISTER_DATA_TYPE(CSCOMM);
//	REGISTER_DATA_TYPE(CSBINBits);//单独处理，无须注册
	REGISTER_DATA_TYPE(CSASCII);
	REGISTER_DATA_TYPE(CSPnFnS);
	REGISTER_DATA_TYPE(CSBS8);
	REGISTER_DATA_TYPE(CSBS64);
	REGISTER_DATA_TYPE(CSBS248);
	REGISTER_DATA_TYPE(CSIP);
	REGISTER_DATA_TYPE(CSBCDType1);
	REGISTER_DATA_TYPE(CSBCDType2);
	REGISTER_DATA_TYPE(CSBCDType3);
	REGISTER_DATA_TYPE(CSBCDType4);
	REGISTER_DATA_TYPE(CSBCDType5);
	REGISTER_DATA_TYPE(CSBCDType6);
	REGISTER_DATA_TYPE(CSBCDType7);
	REGISTER_DATA_TYPE(CSBCDType8);
	REGISTER_DATA_TYPE(CSBCDType9);
	REGISTER_DATA_TYPE(CSBCDType10);
	REGISTER_DATA_TYPE(CSBCDType11);
	REGISTER_DATA_TYPE(CSBCD2);
	REGISTER_DATA_TYPE(CSBCD3);
	REGISTER_DATA_TYPE(CSBCD6);
	REGISTER_DATA_TYPE(CSBCD8);
}

void TransitionImpl<ProcessFrameNx, Begin>::decodeBegin(uint8_t *fdata, int &length)
{
	if (length <= (int)sizeof(data.frameBuf))
	{
		memcpy(data.frameBuf, fdata, length);
		data.frameLength = length;
	}else {
		cout << "frame is too long." << endl;
		state = BadFrame;
		return ;
	}
	state = DecodeFixedPart;

	REGISTER_DATA_TYPE(CSNONE);
	REGISTER_DATA_TYPE(CSXB);
	REGISTER_DATA_TYPE(CSNDQ);
	REGISTER_DATA_TYPE(CSCOMM);
	REGISTER_DATA_TYPE(CSBIN);
//	REGISTER_DATA_TYPE(CSBINBits);//单独处理，无须注册
	REGISTER_DATA_TYPE(CSASCII);
	REGISTER_DATA_TYPE(CSPnFnS);
	REGISTER_DATA_TYPE(CSBS8);
	REGISTER_DATA_TYPE(CSBS64);
	REGISTER_DATA_TYPE(CSBS248);
	REGISTER_DATA_TYPE(CSIP);
	REGISTER_DATA_TYPE(CSBCDType1);
	REGISTER_DATA_TYPE(CSBCDType2);
	REGISTER_DATA_TYPE(CSBCDType3);
	REGISTER_DATA_TYPE(CSBCDType4);
	REGISTER_DATA_TYPE(CSBCDType5);
	REGISTER_DATA_TYPE(CSBCDType6);
	REGISTER_DATA_TYPE(CSBCDType7);
	REGISTER_DATA_TYPE(CSBCDType8);
	REGISTER_DATA_TYPE(CSBCDType9);
	REGISTER_DATA_TYPE(CSBCDType10);
	REGISTER_DATA_TYPE(CSBCDType11);
	REGISTER_DATA_TYPE(CSBCD2);
	REGISTER_DATA_TYPE(CSBCD3);
	REGISTER_DATA_TYPE(CSBCD6);
	REGISTER_DATA_TYPE(CSBCD8);
}

void TransitionImpl<ProcessFrameNx, EncodeFinish>::encodeEnd(uint8_t *fdata, int &length)
{
	if (data.frameLength <= length)
	{
		memcpy(fdata, data.frameBuf, data.frameLength);
		length = data.frameLength;
	}else {
		length = 0;
	}
}

void TransitionImpl<ProcessFrameNx, DecodeFinish>::decodeEnd(FrameNx &frame)
{
	frame = data;
}

bool TransitionImpl<ProcessFrameNx, EncodeFinish>::process()
{
	return true;
}

bool TransitionImpl<ProcessFrameNx, DecodeFinish>::process()
{
	return true;
}

bool TransitionImpl<ProcessFrameNx, BadFrame>::process()
{
	cout << "frame failed." << endl;
	return false;
}

void TransitionImpl<ProcessFrameNx, EncodeFixedPart>::encode()
{
	bool isFixedFrame = true;

	if (data.encode(data.frameBuf))
	{
		data.get<bool>("isFixedFrame", isFixedFrame);
		if (isFixedFrame)
		{
			state = EncodeFinish;
			return;
		}
		else
		{
			state = EncodeFinish;
			return;
		}
	}
	else
	{
		// Decode failed;
		// Log error;
		cout<<"encode EncodeFixedPart failed"<<endl;
		state = BadFrame;
		return;
	}
}

void TransitionImpl<ProcessFrameNx, DecodeFixedPart>::decode()
{
	bool isFixedFrame = true;

	if (data.decode(data.frameBuf))
	{
		data.get<bool>("isFixedFrame", isFixedFrame);
		if (isFixedFrame)
		{
			state = DecodeFinish;
			return;
		}
		else
		{
			state = DecodeVerFramePnfns;
			return;
		}
	}
	else
	{
		// Decode failed;
		// Log error;
		state = BadFrame;
		return;
	}
}

void TransitionImpl<ProcessFrameNx, EncodeVerFramePnfns>::encode()
{
	if (data.encodePos >= data.frameBuf + MAX_FRAME_LENGTH)
	{
		cout<<"frame is too long!"<<endl;
		state = BadFrame;
		return ;
	}

	data.pns.clear();
	data.fns.clear();

	int result = data.getPnFns();
	if (result == 1)//获得pns，fns
	{
		STPnFnS *pnfns = (STPnFnS *)data.encodePos;
		//编码pn，fn
		pnfns->Pn.encode(data.pns, 1);
		pnfns->Fn.encode(data.fns, 0);
		data.encodePos += 4;

		//初始化pn，fn索引
		data.piter = data.pns.begin();
		data.fiter = data.fns.begin();
		state = EncodeUserDataBlock;
	}else if (result == 0)//未获得pns，fns
	{
		data.dataSize = data.encodePos - data.frameBuf - VERIABLE_LENGTH_BEFORE_PNFNS;
		state = EncodeFixedPart;
		return ;
	}else if (result == -1)//执行出错
	{
		cout<<"getPnFns error!"<<endl;
		state = BadFrame;
		return ;
	}


}

void TransitionImpl<ProcessFrameNx, DecodeVerFramePnfns>::decode()
{
	if (data.dataSize <= 0)
	{
		state = DecodeFinish;
		return;
	}

	if (data.decodePos < data.frameBuf + VERIABLE_LENGTH_BEFORE_PNFNS + data.dataSize)
	{
		data.pns.clear();
		data.fns.clear();
		STPnFnS *pnfns = (STPnFnS *)data.decodePos;
		pnfns->Pn.decode(data.pns, 1);
		pnfns->Fn.decode(data.fns, 0);
		data.piter = data.pns.begin();
		data.fiter = data.fns.begin();
		data.decodePos += 4;
		state = DecodeUserDataBlock;
	}else
		state = DecodeFinish;
}

void TransitionImpl<ProcessFrameNx, EncodeUserDataBlock>::encode()
{
	if (data.fiter == data.fns.end())
	{
		data.fiter = data.fns.begin();
		data.piter++;
	}

	if (data.piter == data.pns.end())
	{
		state = EncodeVerFramePnfns;
		return;
	}

	state = EncodeAFnFn;
	return;
}

void TransitionImpl<ProcessFrameNx, DecodeUserDataBlock>::decode()
{
	if (data.fiter == data.fns.end())
	{   
		data.fiter = data.fns.begin();
		data.piter++;
	}

	if (data.piter == data.pns.end())
	{
		state = DecodeVerFramePnfns;
		return;
	}

	state = DecodeAFnFn;
	return;
}

void TransitionImpl<ProcessFrameNx, EncodeAFnFn>::encode()
{
	int afn = data.afn, pn = *data.piter, fn = *data.fiter;
	string afnpnfn;
	stringstream afnpnfn_stream;
	afnpnfn_stream<<"AF"<<afn<<"P"<<pn<<"F"<<fn;
	afnpnfn_stream>>afnpnfn;
	cout << "Begin to encode data unit of " << afnpnfn << endl;

	UserDataUnit afnfn;
	afnfn.set<int>("afn", afn);
	afnfn.set<int>("pn", pn);
	afnfn.set<int>("fn", fn);

	std::vector<int> index;
	if (!data.findItems(index, pn, fn) || index.empty())
	{
		cout<<"EncodeAFnFn encode error!"<<endl;
		state = BadFrame;
		return;
	}

	//设置待编码用户数据
	string key = Poco::format("AF%dP%dF%d:", afn, pn, fn);
	int NonElectricTypeId = 1;
	for (int i = 0; i < (int)index.size(); i++)
	{
		if (data._items[index[i]].format == "CSXB")
		{
			//写入谐波次数
			afnfn.set<double>(key+data._items[index[i]].repeatCount, double(data._items[index[i]].code));
			//写入谐波值
			afnfn.set<double>(key+data._items[index[i]].ename, data._items[index[i]].value);
		}else if (data._items[index[i]].format == "CSNDQ")
		{
			//非电气量需写入编码值
			afnfn.set<double>(key+"NonElectricType"+Poco::format("%d", NonElectricTypeId), double(data._items[index[i]].code));
			//写入值
			afnfn.set<double>(key+"NonElectric"+Poco::format("%d", NonElectricTypeId++), data._items[index[i]].value);
		}else {
			afnfn.set<double>(key+data._items[index[i]].ename, data._items[index[i]].value);
		}
	}
	afnfn.set<int>(key+"curNonElectricCount", 0);//当前非电气量数
	afnfn.set<int>(key+"NonElectricCount", NonElectricTypeId-1);//总非电气量数

	if (afnfn.encode(data.encodePos))
	{
//		ostringstream oss;
//		oss << "Af" << afn << "P" << pn << "F" << fn;
//		data.set<UserDataUnit>(oss.str(), afnfn);
		data.encodePos = afnfn.encodePos;
	}
	else
	{
		cout<<"afnfn encode failed!"<<endl;
		state = BadFrame;
		return;
	}

	data.fiter++;
	state = EncodeUserDataBlock;
	return;
}

void TransitionImpl<ProcessFrameNx, DecodeAFnFn>::decode()
{
	int afn = data.afn, pn = *data.piter, fn = *data.fiter;
	cout << "Begin to decode data unit of AF" << afn << "F" << fn << endl;

	ostringstream oss;
	oss << "AF" << afn << "P" << pn << "F" << fn;

	UserDataUnit afnfn;
	data.get<UserDataUnit>(oss.str(), afnfn);

	afnfn.set<int>("afn", afn);
	afnfn.set<int>("pn", pn);
	afnfn.set<int>("fn", fn);
	afnfn.unitLength = data.dataSize;
	if (afnfn.decode(data.decodePos))
	{
		data.set<UserDataUnit>(oss.str(), afnfn);
		data.decodePos = afnfn.decodePos;
	}
	else
	{
		state = BadFrame;
		return;
	}

	data.fiter++;
	state = DecodeUserDataBlock;
	return;
}
