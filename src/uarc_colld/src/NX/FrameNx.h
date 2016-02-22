#pragma once

#include <time.h>
#include <math.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <type_traits>

#include "NxSubsystem.h"

#include "Poco/Dynamic/Struct.h"
#include "Poco/Dynamic/Var.h"
#include "STData.h"

using namespace std;
using namespace Poco;
using namespace Poco::Dynamic;

#define FIXED_FRAME_HEAD 				0x10
#define VERIABLE_FRAME_HEAD 			0x68
#define FRAME_TAIL 						0x16

#define FIXED_FRAME_LENGTH				14
#define VERIABLE_FRAME_ADDITIONAL_LEN	8

#define VERIABLE_LENGTH_BEFORE_PNFNS 	18
#define VERIABLE_FRAME_FIXED_DATA_SIZE 	12
#define FIXED_FRAME_CS_CHECK_LENGTH 	12

#define FRAMENX_FRAME_FLAG 				0

#define VERIABLE_CS_BEGIN_POS 			6
#define FIXED_CS_BEGIN_POS 				1

class FrameBase : public DynamicStruct {

public:

	virtual bool decode(const uint8_t * pos = 0) { return true; }
	virtual bool encode(uint8_t *pos = 0) { return true; }
	virtual bool execute() { return true; }

	template<typename T, typename enable_if<!is_base_of<FrameBase, T>::value, int>::type = 0>
	bool set(const string& key, const T& value) {

		auto it = this->find(key);
		if ( it == this->end() ) {

			insert(key, value);
			string value_str = key + " = " + Var::toString(value) + "\n";
			_strMap.insert(pair<string, string>(key, value_str)); /* "\r\n"; */
			_strVector.push_back(key);
		}
		else {
			it->second = value;
			_strMap[key] = key + " = " + Var::toString(value) + "\n";
		}

		return true;
	}

	template<typename T, typename enable_if<is_base_of<FrameBase, T>::value, int>::type = 0>
	bool set(const string& key, const T& value) {

		try {

			auto it = this->find(key);
			if ( it == this->end() ) {

				insert(key, value);
				string value_str = key + ":\n" + value.ToString();
				_strMap.insert(pair<string, string>(key, value_str));
				_strVector.push_back(key);
			}
			else {
				it->second = value;
				_strMap[key] = key + ":\n" + value.ToString();
			}

		} catch (...) {

			return false;
		}

		return true;
	}

	template<typename T>
	bool get(const string& key, T& value) {
	
		try {
		
			auto it = find(key);
			if ( it != end() ) 
			
				value = it->second.extract<T>();
				
			else return false;
			
		} catch (Poco::BadCastException&) {
		
			return false;
		}

		return true;
	}

	const string ToString() const
	{
		string str;

		for (int i = 0; i < (int)_strVector.size(); i++)
		{
			string key = _strVector[i];
			str += _strMap.find(key)->second;
		}

		return str;
	}
	
private:
	vector<string> _strVector;
	map<string, string> _strMap;
};

/*
 * Map keys: funCode(int), Rseq(int), Pseq(int), Reserve(bool), Prm(bool), Dir(bool);
 */
class Control : public FrameBase{

public:

	bool decode(const uint8_t * pos);
	bool encode(uint8_t *pos);
};

/*
 * Map keys: areaCode(int), termAddr(int), masterAddr(int), Group(bool);
 */
class Address : public FrameBase {

public:

	bool decode(const uint8_t *pos);
	bool encode(uint8_t *pos);
};

/*
 * Map keys: Afn(int), Tpv(bool), Acd(bool);
 */
class AppControl : public FrameBase {

public:

	bool decode(const uint8_t *pos);
	bool encode(uint8_t *pos);
};

/*
 * Map keys: ec1count(int), ec2count(int);
 */
class FEvent : public FrameBase {

public:

	bool decode(const uint8_t *pos);
	bool encode(uint8_t *pos);
};

class Password : public FrameBase {

public:

	bool decode(const uint8_t *pos);
	bool encode(uint8_t *pos);
};

/*
 * Map keys: timestamp(time_t), timeout(uint8_t)
 */
class TimeStamp : public FrameBase {

public:

	bool decode(const uint8_t *pos);
	bool encode(uint8_t *pos);
};

class FixedFrame: public FrameBase {

public:

	bool decode(const uint8_t *pos);
	bool encode(uint8_t *pos);
};

class VerFrame: public FrameBase {

public:

	bool decode(const uint8_t *pos);
	bool encode(uint8_t *pos);
};

class Item {
public:

	Item():exist(true), pn(-1), fn(-1), repeatCount(""), ename(""), code(0), format(""), value(0), time(0){}

	bool exist;
	int pn;
	int fn;
	string repeatCount;
	string ename;
	int code;
	string format;
	double value;
	long int time;
};

class UserDataUnit : public FrameBase {

public:

	bool decode(const uint8_t *pos);
	bool encode(uint8_t *pos);

	int findItems(int pn, int fn, string ename);
public:
	vector<Item> _items;

	const uint8_t *decodePos;
	uint8_t *encodePos;
	map<int, int> repeatcountMap;
	vector<int> __PnS, __FnS;
	int curERC;
	int unitLength;
	int reference;
};

class FrameNx : public FrameBase {

public:

	bool decode(const uint8_t *pos);
	bool encode(uint8_t *pos);

	bool dataItemsToItems();
	bool parseByItemId(int itemId, int &Fn, int &Code, string &Format, string &repeatCount);

	bool fillPnSet();
	bool fillFnSet();
	void sortSet(vector<int> &values);
	bool isExist(int pn, int fn, bool flag = false);
	bool isSamePnGroup(int pn1, int pn2);
	bool isSameFnGroup(int fn1, int fn2);
	bool findItems(vector<int> &index, int pn, int fn);
	int getPnFns();

public:

	vector<DataItem> _dataItems;//原始输入的发送数据
	vector<Item> _items;//经过处理后的需要发送的数据
	vector<int> _pnSet, _fnSet;//编码pn值、fn值的集合
	vector<int>::iterator _pSetIter, _fSetIter;

	uint8_t frameBuf[MAX_FRAME_LENGTH];
	int frameLength;
	vector<int> pns, fns;
	vector<int>::iterator piter, fiter;
	const uint8_t *decodePos;
	uint8_t *encodePos;
	int afn;
	int dataSize;

};

class CSBase {

public:

	virtual bool decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len) = 0;
	virtual bool encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len) = 0;
};

class CSNONE : public CSBase {
	bool decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len);
	bool encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len);
};

class CSXB : public CSBase {

	bool decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len);
	bool encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len);
};

class CSNDQ : public CSBase {

	bool decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len);
	bool encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len);
};

class CSCOMM : public CSBase {

	bool decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len);
	bool encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len);
};

class CSBIN : public CSBase {

	bool decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len);
	bool encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len);
};

class CSBINBits {

public:

	bool decode(FrameBase *frame, const string& ename, const uint8_t *pos, int &len, int StartBit, int EndBit, int finishFlag);
	bool encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len, int StartBit, int EndBit, int finishFlag);
};

class CSASCII : public CSBase {

	bool decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len);
	bool encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len);
};

class CSPnFnS : public CSBase {

	bool decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len);
	bool encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len);
};

class CSBS8 : public CSBase {

	bool decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len);
	bool encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len);
};

class CSBS64 : public CSBase {

	bool decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len);
	bool encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len);
};

class CSBS248 : public CSBase {

	bool decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len);
	bool encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len);
};

class CSIP : public CSBase {

	bool decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len);
	bool encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len);
};

class CSBCDType1 : public CSBase {

	bool decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len);
	bool encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len);
};

class CSBCDType2 : public CSBase {

	bool decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len);
	bool encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len);
};

class CSBCDType3 : public CSBase {

	bool decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len);
	bool encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len);
};

class CSBCDType4 : public CSBase {

	bool decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len);
	bool encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len);
};

class CSBCDType5 : public CSBase {

	bool decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len);
	bool encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len);
};

class CSBCDType6 : public CSBase {

	bool decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len);
	bool encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len);
};

class CSBCDType7 : public CSBase {

	bool decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len);
	bool encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len);
};

class CSBCDType8 : public CSBase {

	bool decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len);
	bool encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len);
};

class CSBCDType9 : public CSBase {

	bool decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len);
	bool encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len);
};

class CSBCDType10 : public CSBase {

	bool decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len);
	bool encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len);
};

class CSBCDType11 : public CSBase {

	bool decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len);
	bool encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len);
};

class CSBCD2 : public CSBase {

	bool decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len);
	bool encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len);
};

class CSBCD3 : public CSBase {

	bool decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len);
	bool encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len);
};

class CSBCD6 : public CSBase {

	bool decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len);
	bool encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len);
};

class CSBCD8 : public CSBase {

	bool decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len);
	bool encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len);
};

template<typename T>
CSBase *createCS()
{
	return new T;
}

typedef map<string, CSBase*(*)()> CSMap; 
class CSBaseFactory {

public:

	static CSBase *getTypePointer(const string& formatName) {

		CSMap::iterator it = getMap()->find(formatName); 
		
		if (it == getMap()->end())
		
			return NULL;
		
		return it->second();
	}

	static CSMap *getMap() {

		if (!__csMap)
		
			__csMap = new CSMap;
		
		return __csMap;
	}

private:
	
	static CSMap *__csMap;
};

template<typename T>
class CSFactory : public CSBaseFactory {

public:

	CSFactory(const string& formatName) {
	
		getMap()->insert(pair<string, CSBase*(*)()>(formatName, &createCS<T>));
	}
};

#define REGISTER_DATA_TYPE(formatName) 	CSFactory<formatName> Reg##formatName(#formatName);
