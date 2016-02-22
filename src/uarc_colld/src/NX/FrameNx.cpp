#include <iostream>

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>

#include "Poco/LocalDateTime.h"
#include "Poco/Timestamp.h"
#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/NodeIterator.h"
#include "Poco/DOM/NodeFilter.h"
#include "Poco/DOM/NodeList.h"
#include "Poco/DOM/AutoPtr.h"
#include "Poco/Format.h"
#include "Poco/DOM/NamedNodeMap.h"
#include "Poco/SAX/InputSource.h"
#include "Poco/Exception.h"
#include "Poco/DynamicAny.h"
#include "FrameNx.h"

using namespace Poco::XML;
using Poco::XML::DOMParser;
using Poco::XML::InputSource;
using Poco::XML::Document;
using Poco::XML::NodeIterator;
using Poco::XML::NodeFilter;
using Poco::XML::NodeList;
using Poco::XML::Node;
using Poco::XML::AutoPtr;
using Poco::Exception;


using namespace std;
using std::hex;

const string path_xml = "/etc/uarc/protocol-cfg/Nx/";
const string down_xml = path_xml + "AfnFns-X.xml";
const string up_xml = path_xml + "AfnFns.xml";

bool Control::encode(uint8_t *pos)
{
	STControl *control = (STControl *)pos;

	int funCode = 0, Rseq = 0, Pseq = 0;
	if (get<int>("funCode", funCode))
	{
		control->Control_D0_D3_func = funCode;
	}else {
		return false;
	}

	if (get<int>("Rseq", Rseq))
	{
		control->Control_D4_D7_resq = Rseq;
	}else {
		return false;
	}

	if (get<int>("Pseq", Pseq))
	{
		control->Control_D8_D12_pesq = Pseq;
	}else {
		return false;
	}

	bool Reserve = false, Prm = false, Dir = false;

	if (get<bool>("Reserve", Reserve))
	{
		control->Control_D13_reserve = (bool)Reserve;
	}else {
		return false;
	}

	if (get<bool>("Prm", Prm))
	{
		control->Control_D14_prm = (bool)Prm;
	}else {
		return false;
	}

	if (get<bool>("Dir", Dir))
	{
		control->Control_D15_dir = (bool)Dir;
	}else {
		return false;
	}

	return true;
}

bool Control::decode(const uint8_t *pos)
{
	STControl *control = (STControl *)pos;

	set<int>("funCode", control->Control_D0_D3_func);
	set<int>("Rseq", control->Control_D4_D7_resq);
	set<int>("Pseq", control->Control_D8_D12_pesq);
	set<bool>("Reserve", control->Control_D13_reserve);
	set<bool>("Prm", control->Control_D14_prm);
	set<bool>("Dir", control->Control_D15_dir);

	return true;
}

bool Address::encode(uint8_t *pos)
{
	STAddress *adata = (STAddress *)pos;

	int areaCode = 0;
	if (get<int>("areaCode", areaCode))
	{
		adata->A1B1Uni = (areaCode%10)/1;
		adata->A1B1Ten = (areaCode%100)/10;
		adata->A1B2Uni = (areaCode%1000)/100;
		adata->A1B2Ten = (areaCode%10000)/1000;
		adata->A1B3Uni = (areaCode%100000)/10000;
		adata->A1B3Ten = (areaCode%1000000)/100000;
	}else {
		return false;
	}

	int termAddr = 0;
	if (get<int>("termAddr", termAddr))
	{
		adata->A2Byte1 = (termAddr >> 0) & 0xff;
		adata->A2Byte2 = (termAddr >> 8) & 0xff;
		adata->A2Byte3 = (termAddr >> 16) & 0xff;
		adata->A2Byte4 = (termAddr >> 24) & 0xff;
	}else {
		return false;
	}

	int masterAddr = 0;
	if (get<int>("masterAddr", masterAddr))
	{
		adata->Addr3 = masterAddr << 1;
	}else {
		return false;
	}

	bool Group = false;
	if (get<bool>("Group", Group))
	{
		if (Group)
		{
			adata->Addr3 |= 0x1;
		}else {
			adata->Addr3 &= ~0x1;
		}
	}else {
		return false;
	}

	return true;
}

bool Address::decode(const uint8_t *pos)
{
	STAddress *adata = (STAddress *)pos;
	set<int>("areaCode", adata->A1B1Uni  + adata->A1B1Ten * 10 + adata->A1B2Uni * 100 + adata->A1B2Ten * 1000 +
			adata->A1B3Uni * 10000 + adata->A1B3Ten * 100000);
	set<int>("termAddr",
			((adata->A2Byte1 & 0xff) +
			 ((adata->A2Byte2 <<  8) & 0xff00) +
			 ((adata->A2Byte3 << 16) & 0xff0000) +
			 ((adata->A2Byte4 << 24) & 0xff000000)));
	set<int>("masterAddr", adata->Addr3 >> 1);
	set<bool>("Group", adata->Addr3 & 0x1);

	return true;
}

bool AppControl::encode(uint8_t *pos)
{
	uint8_t *ac = (uint8_t *)pos;

	int acAfn = 0;
	bool acAcd = false, acTpv = false;
	if (get<int>("acAfn", acAfn))
	{
		*ac = acAfn & 0x1f;
	}else {
		return false;
	}

	*ac &= ~0x20;

	if (get<bool>("acAcd", acAcd))
	{
		if (acAcd)
		{
			*ac |= 0x40;
		}else {
			*ac &= ~0x40;
		}
	}else {
		return false;
	}
	if (get<bool>("acTpv", acTpv))
	{
		if (acTpv)
		{
			*ac |= 0x80;
		}else {
			*ac &= ~0x80;
		}
	}else {
		return false;
	}

	return true;
}

bool AppControl::decode(const uint8_t *pos)
{
	uint8_t *ac = (uint8_t *)pos;

	set<int>("acAfn", (*ac & 0x1f));
	set<bool>("acAcd", ((*ac & 0x40) != 0));
	set<bool>("acTpv", ((*ac & 0x80) != 0));

	return true;
}

bool FEvent::encode(uint8_t *pos)
{
	uint16_t *ec1 = (uint16_t *)pos;
	int ec1_ = 0;
	if (get<int>("ec1", ec1_))
	{
		*ec1 = ((ec1_ >> 8) & 0xff) + ((ec1_ << 8) & 0xff00);
	}else {
		return false;
	}

	uint16_t *ec2 = (uint16_t *)(pos + 2);
	int ec2_ = 0;
	if (get<int>("ec2", ec2_))
	{
		*ec2 = ((ec2_ >> 8) & 0xff) + ((ec2_ << 8) & 0xff00);
	}else {
		return false;
	}

	return true;
}

bool FEvent::decode(const uint8_t *pos)
{
	uint16_t *ec1 = (uint16_t *)pos;
	set<int>("ec1", ((*ec1 >> 8) & 0xff) + ((*ec1 << 8) & 0xff00));
	uint16_t *ec2 = (uint16_t *)(pos + 2);
	set<int>("ec2", ((*ec2 >> 8) & 0xff) + ((*ec2 << 8) & 0xff00));

	return true;
}

bool Password::encode(uint8_t *pos)
{
	string password;
	if (get<string>("password", password))
	{
		strncpy((char *)pos, password.c_str(), 16);
	}else {
		return false;
	}

	return true;
}

bool Password::decode(const uint8_t *pos)
{
	string password((const char*)pos, 16);
	set<string>("password", password);

	return true;
}

bool TimeStamp::encode(uint8_t *pos)
{
	STBCDType3 *tm = (STBCDType3 *)pos;

	time_t tmp;
	if (get<time_t>("timeStamp", tmp))
	{
		struct tm *tm_t = localtime(&tmp);
		tm->DayTen = tm_t->tm_mday/10;
		tm->DayUni = tm_t->tm_mday%10;

		tm->HourTen = tm_t->tm_hour/10;
		tm->HourUni = tm_t->tm_hour%10;

		tm->MinTen = tm_t->tm_min/10;
		tm->MinUni = tm_t->tm_min%10;

		tm->SecTen = tm_t->tm_sec/10;
		tm->SecUni = tm_t->tm_sec%10;
	}else {
		return false;
	}

	uint8_t *timeout = pos + 4;
	int timeout_ = 0;
	if (get<int>("timeout", timeout_))
	{
		*timeout = timeout_;
	}else {
		return false;
	}

	return true;
}

bool TimeStamp::decode(const uint8_t *pos)
{
	STBCDType3 *tm = (STBCDType3 *)pos;
	// struct tm tm_t;
	// time_t cur, tmp;

	// memset(&tm_t, 0, sizeof(struct tm));
	// cur = time(NULL);
	// localtime_r(&cur, &tm_t);
	// tm_t.tm_mday = tm->DayUni + tm->DayTen * 10;
	// tm_t.tm_hour = tm->HourUni + tm->HourTen * 10;
	// tm_t.tm_min = tm->MinUni + tm->MinTen * 10;
	// tm_t.tm_sec = tm->SecUni + tm->SecTen * 10;
	// tmp = mktime(&tm_t);
	Poco::LocalDateTime cur;
	Poco::LocalDateTime recur(cur.year(),
						      cur.month(),
						      tm->DayUni + tm->DayTen * 10,
						      tm->HourUni + tm->HourTen * 10,
						      tm->MinUni + tm->MinTen * 10,
						      tm->SecUni + tm->SecTen * 10,
						      cur.millisecond(),
						      cur.microsecond()
						      );
	Poco::Timestamp stamp = recur.timestamp();
	std::time_t tmp = stamp.epochTime();
	
	set<time_t>("timeStamp", tmp);

	const uint8_t *timeout = pos + 4;
	set<int>("timeout", *timeout);

	return true;
}

bool FixedFrame::encode(uint8_t *pos)
{
	STFixedFrameData *frameData = (STFixedFrameData *)pos;

	frameData->FrameBegin = FIXED_FRAME_HEAD;

	Control control;
	if (get<Control>("control", control))
	{
		if (!control.encode((uint8_t *)(&frameData->cdata)))
		{
			return false;
		}
	}else {
		return false;
	}

	Address address;
	if (get<Address>("address", address))
	{
		if (!address.encode((uint8_t *)(&frameData->adata)))
		{
			return false;
		}
	}else {
		return false;
	}

	int protocolVersion = 0;
	if (get<int>("protocolVersion", protocolVersion))
	{
		frameData->protocolType_D0_D5_version = protocolVersion;
	}else {
		return false;
	}

	int protocolFlag = 0;
	if (get<int>("protocolFlag", protocolFlag))
	{
		frameData->protocolType_D6_flag = protocolFlag;
	}else {
		return false;
	}

	int protocolReserve = 0;
	if (get<int>("protocolReserve", protocolReserve))
	{
		frameData->protocolType_D7_reserve = protocolReserve;
	}else {
		return false;
	}

	const uint8_t *checkBegin = pos + FIXED_CS_BEGIN_POS;
	uint8_t *checkEnd = pos + FIXED_FRAME_CS_CHECK_LENGTH;
	int checksum = 0;
	for (const uint8_t *index = checkBegin; index < checkEnd; index++)
		checksum += *index;

	*checkEnd = checksum & 0xff;

	frameData->tail = FRAME_TAIL;

	return true;
}

bool FixedFrame::decode(const uint8_t *pos)
{
	STFixedFrameData *frameData = (STFixedFrameData *)pos;
	if (frameData->FrameBegin != FIXED_FRAME_HEAD)
	{
		cout << "Frame head error." << endl;
		return false;
	}
	set<int>("frameHead", frameData->FrameBegin);

	const uint8_t *checkBegin = pos + FIXED_CS_BEGIN_POS;
	const uint8_t *checkEnd = pos + FIXED_FRAME_CS_CHECK_LENGTH;
	int checksum = 0;
	for (const uint8_t *index = checkBegin; index < checkEnd; index++)
		checksum += *index;
	if ((checksum & 0xff) != (*checkEnd & 0xff))
	{
		cout << "Frame check sum failed." << endl;
		return false;
	}

	Control control;
	if (control.decode((uint8_t *)(&frameData->cdata)))
		set<Control>("control", control);
	else
		return false;

	Address address;
	if (address.decode((uint8_t *)(&frameData->adata)))
		set<Address>("address", address);
	else
		return false;

	if (frameData->protocolType_D6_flag != FRAMENX_FRAME_FLAG)
	{
		cout << "Frame protocol type error." << endl;
		return false;
	}
	set<int>("protocolVersion", frameData->protocolType_D0_D5_version);
	set<int>("protocolFlag", frameData->protocolType_D6_flag);
	set<int>("protocolReserve", frameData->protocolType_D7_reserve);

	if (frameData->tail != FRAME_TAIL)
	{
		cout << "Frame tail error." << endl;
		return false;
	}
	set<int>("frameTail", frameData->tail);

	return true;
}

static bool FindPositionNode(Document *pDoc, Node **pNode, string strpath);
static bool encodeTraverseNodeTree(UserDataUnit *UserFrame, Document *pDoc, Node *pNode, int afn, int fn, int pn, int index);
static bool decodeTraverseNodeTree(UserDataUnit *UserFrame, Document *pDoc, Node *pNode, int afn, int fn, int pn, int index);
static bool encodeEachNode(UserDataUnit *UserFrame, Document *pDoc, NamedNodeMap *pMap, int afn, int pn, int fn, int index);
static bool decodeEachNode(UserDataUnit *UserFrame, Document *pDoc, NamedNodeMap *pMap, int afn, int pn, int fn, int index);
static bool encodeRepeatNodes(UserDataUnit *UserFrame, Node *nextNode, Document *pDoc, NamedNodeMap *pMap, int *realDecodeNum, int afn, int pn, int fn);
static bool decodeRepeatNodes(UserDataUnit *UserFrame, Node *nextNode, Document *pDoc, NamedNodeMap *pMap, int *realDecodeNum, int afn, int pn, int fn);
static bool encodeAccordingXML(UserDataUnit *UserFrame, Document *pDoc, int afn, int pn, int fn);
static bool decodeAccordingXML(UserDataUnit *UserFrame, Document *pDoc, int afn, int pn, int fn);

bool FindPositionNode(Document *pDoc, Node **pNode, string strpath)
{
	try {
		*pNode = pDoc->getNodeByPath(strpath);
		if (*pNode == NULL)
		{
			cout << "Path:" << strpath << endl;
			cout << "Can't find node path." << endl;
			return false;
		}
	} catch (Exception &e) {
		cout << e.displayText() << endl;
		return false;
	}

	return true;
}

bool encodeEachNode(UserDataUnit *UserFrame, Document *pDoc, NamedNodeMap *pMap, int afn, int pn, int fn, int index)
{
	Node *Itemid = NULL;
	Node *Itemename = NULL;
	Node *Itemformat = NULL;
	Node *Itemlen = NULL;
	Node *Itemisrepeatcount = NULL;
	Node *Itemisunitid = NULL;
	Node *ItemisERC = NULL;
	Node *ItemisUnitLength = NULL;
	Node *ItemisReference = NULL;
	Node *ItemisAccess = NULL;
	Node *ItemAccess = NULL;

	try {
		Itemid = pMap->getNamedItem("id");
		if (!Itemid)
		{
			cout << "XML file: Id attribute missing." << endl;
			return false;
		}
		Itemename = pMap->getNamedItem("ename");
		if (!Itemename)
		{
			cout << "XML file: Ename attribute missing." << endl;
			return false;
		}
		Itemformat = pMap->getNamedItem("format");
		if (!Itemformat)
		{
			cout << "XML file: Format attribute missing." << endl;
			return false;
		}
		Itemlen = pMap->getNamedItem("len");
		if (!Itemlen)
		{
			cout << "XML file: Len attribute missing." << endl;
			return false;
		}

		string ename = Itemename->nodeValue();

		ename = Poco::format("AF%dP%dF%d:%s", afn, pn, fn, ename);
		int userAfn = 0;
		UserFrame->get<int>("afn", userAfn);

		//非电气量的ename处理
		int NECount = 0, curNE = 1;
		if (Itemename->nodeValue() == "NonElectricType" ||
				Itemename->nodeValue() == "NonElectric")
		{
			string key = Poco::format("AF%dP%dF%d:", afn, pn, fn);
			if (!UserFrame->get<int>(key+"NonElectricCount", NECount))
			{
				cout << "get " << key << "NonElectricCount failed!" << endl;
				return false;
			}

			if (!UserFrame->get<int>(key+"curNonElectricCount", curNE))
			{
				cout << "get " << key << "curNonElectricCount failed!" << endl;
				return false;
			}
			if (curNE > NECount)
			{
				cout << key << "curNE > NECount!" << endl;
				return false;
			}
			if (Itemename->nodeValue() == "NonElectricType")
			{
				if (!UserFrame->set<int>(key+"curNonElectricCount", ++curNE))
				{
					cout << "set " << key << "curNonElectricCount failed!" << endl;
					return false;
				}
			}
			ename += Poco::format("%d", curNE);
		}

		if (Itemformat->nodeValue() == "CSXB")
		{
			double value = 0;
			if (!UserFrame->get<double>(ename, value))
			{
				if (!UserFrame->set<double>(ename, value))
				{
					return false;
				}
			}
		}

		ItemAccess = pMap->getNamedItem("access");
		if (ItemAccess)//存在access
		{
			string key = Poco::format("AF%dP%dF%d:", afn, pn, fn);
			int accessCode;
			double access = 0;
			if (!UserFrame->get<double>(key + "AccessCode", access))
			{
				cout << "Get access code failed." << endl;
				return false;
			}
			accessCode = access;
			if (atoi(ItemAccess->nodeValue().c_str()) != accessCode)//不匹配跳过
			{
				 return true;
			}
		}

		if (index && userAfn == afn)
		{
			Node *subTreeNode;
			ostringstream oss;
			oss << "/FRAMENX/AFN[@id='" << afn << "']/FN[@id='" << fn << "']/elem[@code='" << index << "']";
			string strpath = oss.str();
			if (!FindPositionNode(pDoc, &subTreeNode, strpath))
				return false;
			NamedNodeMap *subNodeMap = subTreeNode->attributes();
			if (!subNodeMap)
			{
				return false;
			}
			Node *Ename = subNodeMap->getNamedItem("ename");
			subNodeMap->release();
			if (!subNodeMap)
			{
				return false;
			}
			ename = Poco::format("AF%dP%dF%d:%s", afn, pn, fn, Ename->nodeValue());
		}
//		int length;
//		if (Itemlen->nodeValue() == "reference")
//			length = UserFrame->reference;
//		else
//			length = atoi(Itemlen->nodeValue().c_str());

		int length = atoi(Itemlen->nodeValue().c_str());

		if (Itemformat->nodeValue() == "DataUnit")
		{
			if (UserFrame->__PnS.empty() || UserFrame->__FnS.empty())
			{
				cout << "__PnS Or __FnS empty, exception occured." << endl;
				return false;
			}
			int afn = 13;
			int i = 1;
			vector<int>::iterator ptmp, ftmp;
			for (ptmp = UserFrame->__PnS.begin(); ptmp != UserFrame->__PnS.end(); ptmp++)
			{
				for (ftmp = UserFrame->__FnS.begin(); ftmp != UserFrame->__FnS.end(); ftmp++)
				{
					Node *subTreeNode;
					ostringstream oss;
					oss << "/FRAMENX/AFN[@id='" << afn << "']/FN[@id='" << *ftmp << "']";
					string strpath = oss.str();
					if (!FindPositionNode(pDoc, &subTreeNode, strpath))
						return false;
					NamedNodeMap *subNodeMap = subTreeNode->attributes();
					Node *hasDataUnit = subNodeMap->getNamedItem("hasDataUnit");
					subNodeMap->release();
					if (hasDataUnit->nodeValue() == "true")
					{
						if (!encodeTraverseNodeTree(UserFrame, pDoc, subTreeNode, 13, *ptmp, *ftmp, i++))
							return false;
					}
				}
			}
		}
		else if (Itemformat->nodeValue() == "EventUnit")
		{
			if (UserFrame->curERC <= 0 || UserFrame->curERC > 20)
			{
				cout << "Unknown Event ERC." << endl;
				return false;
			}
			int afn, pn, fn;
			UserFrame->get<int>("afn", afn);
			UserFrame->get<int>("pn", pn);
			UserFrame->get<int>("fn", fn);
			Node *subEventNode;
			ostringstream oss;
			oss << "/FRAMENX/AFN[@id='" << afn << "']/EV[@id='" << UserFrame->curERC << "']";
			string strpath = oss.str();
			if (!FindPositionNode(pDoc, &subEventNode, strpath))
				return false;
			NamedNodeMap *subNodeMap = subEventNode->attributes();
			Node *hasDataUnit = subNodeMap->getNamedItem("hasDataUnit");
			subNodeMap->release();
			if (hasDataUnit->nodeValue() == "true")
			{
				if (!encodeTraverseNodeTree(UserFrame, pDoc, subEventNode, afn, pn, fn, 0))
					return false;
			}
		}
		else if (Itemformat->nodeValue() == "CSBINBIT")
		{
			Node *startBitNode = pMap->getNamedItem("startBit");
			Node *endBitNode = pMap->getNamedItem("endBit");
			Node *isfinishedNode = pMap->getNamedItem("isfinished");
			if (!startBitNode || !endBitNode)
			{
				cout << "XML attributes error." << endl;
				return false;
			}

			int startbit = atoi(startBitNode->nodeValue().c_str());
			int endbit = atoi(endBitNode->nodeValue().c_str());
			int finishFlag = isfinishedNode->nodeValue() == "true" ? 1 : 0;
			CSBINBits *stData = new CSBINBits;
			if (!stData->encode(UserFrame, ename, UserFrame->encodePos, length, startbit, endbit, finishFlag))
			{
				delete stData;
				return false;
			}
			UserFrame->encodePos += length;
			delete stData;
		}
		else
		{
			auto *stData = CSBaseFactory::getTypePointer(Itemformat->nodeValue());
			if (!stData->encode(UserFrame, ename, UserFrame->encodePos, length))
			{
				cout << Itemformat->nodeValue() <<" encode failed!" << endl;
				delete stData;
				return false;
			}
			UserFrame->encodePos += length;
			delete stData;
		}

		Itemisrepeatcount = pMap->getNamedItem("isrepeatcount");
		if(Itemisrepeatcount && Itemisrepeatcount->nodeValue() == "true")
		{
			int id = atoi(Itemid->nodeValue().c_str());
			int STvalue = 0;
			double v = 0;
			if (UserFrame->get<double>(ename, v))
			{
				STvalue = v;
				UserFrame->repeatcountMap.insert(pair<int, int>(id, STvalue));
			}else
				return false;

			int userAfn;
			UserFrame->get<int>("afn", userAfn);
			if (userAfn == 14 && id == 4)
			{
				int repeatcount1, repeatcount2;
				auto cp = UserFrame->repeatcountMap.find(3);
				if (cp == UserFrame->repeatcountMap.end())
					return false;
				repeatcount1 = cp->second;

				cp = UserFrame->repeatcountMap.find(4);
				if (cp == UserFrame->repeatcountMap.end())
					return false;
				repeatcount2 = cp->second;

				UserFrame->repeatcountMap.insert(pair<int, int>(5, repeatcount1 < repeatcount2 ? repeatcount2 - repeatcount1 : 256 + repeatcount2 - repeatcount1));
			}
		}
		Itemisunitid = pMap->getNamedItem("isDataUnitId");
		if (Itemisunitid)
		{
			vector<int> tmp;
			UserFrame->__PnS.clear();
			UserFrame->__FnS.clear();
			if (!UserFrame->get<vector<int>>(ename + "Pns", tmp))
			{
				cout << "get __PnS failed." << endl;
				return false;
			}
			UserFrame->__PnS = tmp;
			if (!UserFrame->get<vector<int>>(ename + "Fns", tmp))
			{
				cout << "get __FnS failed." << endl;
				return false;
			}
			UserFrame->__FnS = tmp;
		}
		ItemisERC = pMap->getNamedItem("isERC");
		if (ItemisERC)
		{
			int curERC;
			if (!UserFrame->get<int>(ename, curERC))
			{
				cout << "Get current ERC failed." << endl;
				return false;
			}
			UserFrame->curERC = curERC;
		}
		ItemisUnitLength = pMap->getNamedItem("isunitlength");
		if (ItemisUnitLength)
		{
			int unitLength;
			if (!UserFrame->get<int>(ename, unitLength))
			{
				cout << "Get unitlength failed." << endl;
				return false;
			}
			UserFrame->unitLength = unitLength;
		}
		ItemisReference = pMap->getNamedItem("isreference");
		if (ItemisReference)
		{
			int reference;
			if (!UserFrame->get<int>(ename, reference))
			{
				cout << "Get reference value failed." << endl;
				return false;
			}
			UserFrame->reference = reference;
		}

		ItemisAccess = pMap->getNamedItem("isAccess");
		if (ItemisAccess && ItemisAccess->nodeValue() == "true")
		{
			double access;
			int accessCode;
			if (!UserFrame->get<double>(ename, access))
			{
				cout << "Get access code failed." << endl;
				return false;
			}
			accessCode = access;

			string key = Poco::format("AF%dP%dF%d:", afn, pn, fn);
			if (!UserFrame->set<double>(key + "AccessCode", access))
			{
				cout << "Set access code failed." << endl;
				return false;
			}

			ostringstream oss;
			oss << "len" << accessCode;
			string lenName = oss.str();
			Itemlen = pMap->getNamedItem(lenName);
			if (!Itemlen)
			{
				cout << "XML file: Len"<<accessCode<<" attribute missing." << endl;
				return false;
			}
			int length = atoi(Itemlen->nodeValue().c_str());
			double len = length;
			if (!UserFrame->set<double>(key + "LP", len))
			{
				cout << "Set LP value failed." << endl;
				return false;
			}
		}

	} catch (Exception &e) {
		cout << e.displayText() << endl;
		return false;
	}

	return true;
}

bool decodeEachNode(UserDataUnit *UserFrame, Document *pDoc, NamedNodeMap *pMap, int afn, int pn, int fn, int index)
{
	Node *Itemid = NULL;
	Node *Itemename = NULL;
	Node *Itemformat = NULL;
	Node *Itemlen = NULL;

	Node *Itemisrepeatcount = NULL;
	Node *Itemisunitid = NULL;
	Node *ItemisERC = NULL;
	Node *ItemisUnitLength = NULL;
	Node *ItemisReference = NULL;

	try {
		Itemid = pMap->getNamedItem("id");
		if (!Itemid)
		{
			cout << "XML file: Id attribute missing." << endl;
			return false;
		}
		Itemename = pMap->getNamedItem("ename");
		if (!Itemename)
		{
			cout << "XML file: Ename attribute missing." << endl;
			return false;
		}
		Itemformat = pMap->getNamedItem("format");
		if (!Itemformat)
		{
			cout << "XML file: Format attribute missing." << endl;
			return false;
		}
		Itemlen = pMap->getNamedItem("len");
		if (!Itemlen)
		{
			cout << "XML file: Len attribute missing." << endl;
			return false;
		}

		string ename = Itemename->nodeValue();
		ename = Poco::format("AF%dP%dF%d:%s", afn, pn, fn, ename);
		int userAfn = 0;
		UserFrame->get<int>("afn", userAfn);
		if (index && userAfn == afn)
			ename = Poco::format("%s[%d]", ename, index);
		int length;		
		if (Itemlen->nodeValue() == "reference")
			length = UserFrame->reference;
		else
			length = atoi(Itemlen->nodeValue().c_str());

		//获得非电气量ename
		if (Itemename->nodeValue() == "NonElectricType")
		{
			ename = Poco::format("AF%dP%dF%d:%s", afn, pn, fn, Itemename->nodeValue());
		}
		if (Itemename->nodeValue() == "NonElectric")
		{
			int code = 0;
			string key = Poco::format("AF%dP%dF%d:", afn, pn, fn);
			if (!UserFrame->get<int>(key+"NonElectricType", code))
			{
				cout << "get code failed!" << endl;
				return false;
			}
			Node *subTreeNode;
			ostringstream oss;
			oss << "/FRAMENX/AFN[@id='" << afn << "']/FN[@id='" << fn << "']/elem[@code='" << code << "']";
			string strpath = oss.str();
			if (!FindPositionNode(pDoc, &subTreeNode, strpath))
				return false;
			NamedNodeMap *subNodeMap = subTreeNode->attributes();
			if (!subNodeMap)
			{
				return false;
			}
			Node *Ename = subNodeMap->getNamedItem("ename");
			subNodeMap->release();
			if (!subNodeMap)
			{
				return false;
			}
			ename = Poco::format("AF%dP%dF%d:%s", afn, pn, fn, Ename->nodeValue());
		}

		Node *Itemtype = NULL;
		Itemtype = pMap->getNamedItem("type");
		if (Itemtype && Itemtype->nodeValue() == "repeat")
		{
			Node *subTreeNode;
			ostringstream oss;
			oss << "/FRAMENX/AFN[@id='" << afn << "']/FN[@id='" << fn << "']/elem[@code='" << index << "']";
			string strpath = oss.str();
			if (!FindPositionNode(pDoc, &subTreeNode, strpath))
				return false;
			NamedNodeMap *subNodeMap = subTreeNode->attributes();
			if (!subNodeMap)
			{
				return false;
			}
			Node *Ename = subNodeMap->getNamedItem("ename");
			subNodeMap->release();
			if (!subNodeMap)
			{
				return false;
			}
			ename = Poco::format("AF%dP%dF%d:%s", afn, pn, fn, Ename->nodeValue());
		}

		if (Itemformat->nodeValue() == "DataUnit")
		{
			if (UserFrame->__PnS.empty() || UserFrame->__FnS.empty())
			{
				cout << "__PnS Or __FnS empty, exception occured." << endl;
				return false;
			}
			int afn = 13;
			int i = 1;
			vector<int>::iterator ptmp, ftmp;
			for (ptmp = UserFrame->__PnS.begin(); ptmp != UserFrame->__PnS.end(); ptmp++)
			{
				for (ftmp = UserFrame->__FnS.begin(); ftmp != UserFrame->__FnS.end(); ftmp++)
				{
					Node *subTreeNode;
					ostringstream oss;
					oss << "/FRAMENX/AFN[@id='" << afn << "']/FN[@id='" << *ftmp << "']";
					string strpath = oss.str();
					if (!FindPositionNode(pDoc, &subTreeNode, strpath))
						return false;
					NamedNodeMap *subNodeMap = subTreeNode->attributes();
					Node *hasDataUnit = subNodeMap->getNamedItem("hasDataUnit");
					subNodeMap->release();
					if (hasDataUnit->nodeValue() == "true")
					{
						if (!decodeTraverseNodeTree(UserFrame, pDoc, subTreeNode, 13, *ptmp, *ftmp, i++))
							return false;
					}
				}
			}
		}
		else if (Itemformat->nodeValue() == "EventUnit")
		{
			if (UserFrame->curERC <= 0 || UserFrame->curERC > 20)
			{
				cout << "Unknown Event ERC." << endl;
				return false;
			}
			int afn, pn, fn;
			UserFrame->get<int>("afn", afn);
			UserFrame->get<int>("pn", pn);
			UserFrame->get<int>("fn", fn);
			Node *subEventNode;
			ostringstream oss;
			oss << "/FRAMENX/AFN[@id='" << afn << "']/EV[@id='" << UserFrame->curERC << "']";
			string strpath = oss.str();
			if (!FindPositionNode(pDoc, &subEventNode, strpath))
				return false;
			NamedNodeMap *subNodeMap = subEventNode->attributes();
			Node *hasDataUnit = subNodeMap->getNamedItem("hasDataUnit");
			subNodeMap->release();
			if (hasDataUnit->nodeValue() == "true")
			{
				if (!decodeTraverseNodeTree(UserFrame, pDoc, subEventNode, afn, pn, fn, 0))
					return false;
			}
		}
		else if (Itemformat->nodeValue() == "CSBINBIT")
		{
			Node *startBitNode = pMap->getNamedItem("startBit");
			Node *endBitNode = pMap->getNamedItem("endBit");
			Node *isfinishedNode = pMap->getNamedItem("isfinished");
			if (!startBitNode || !endBitNode)
			{
				cout << "XML attributes error." << endl;
				return false;
			}

			int startbit = atoi(startBitNode->nodeValue().c_str());
			int endbit = atoi(endBitNode->nodeValue().c_str());
			int finishFlag = isfinishedNode->nodeValue() == "true" ? 1 : 0;
			CSBINBits *stData = new CSBINBits;
			if (!stData->decode(UserFrame, ename, UserFrame->decodePos, length, startbit, endbit, finishFlag))
			{
				delete stData;
				return false;
			}
			UserFrame->decodePos += length;
			delete stData;
		}
		else
		{
			auto *stData = CSBaseFactory::getTypePointer(Itemformat->nodeValue());
			if (!stData->decode(UserFrame, ename, UserFrame->decodePos, length))
			{
				delete stData;
				return false;
			}

			UserFrame->decodePos += length;
			delete stData;
		}

		Itemisrepeatcount = pMap->getNamedItem("isrepeatcount");
		if(Itemisrepeatcount)
		{
			int id = atoi(Itemid->nodeValue().c_str());
			int STvalue = 0;
			if (UserFrame->get<int>(ename, STvalue))
				UserFrame->repeatcountMap.insert(pair<int, int>(id, STvalue));
			else
				return false;

			int userAfn;
			UserFrame->get<int>("afn", userAfn);
			if (userAfn == 14 && id == 4)
			{
				int repeatcount1, repeatcount2;
				auto cp = UserFrame->repeatcountMap.find(3);
				if (cp == UserFrame->repeatcountMap.end())
					return false;
				repeatcount1 = cp->second;

				cp = UserFrame->repeatcountMap.find(4);
				if (cp == UserFrame->repeatcountMap.end())
					return false;
				repeatcount2 = cp->second;

				UserFrame->repeatcountMap.insert(pair<int, int>(5, repeatcount1 < repeatcount2 ? repeatcount2 - repeatcount1 : 256 + repeatcount2 - repeatcount1));
			}
		}
		Itemisunitid = pMap->getNamedItem("isDataUnitId");
		if (Itemisunitid)
		{
			vector<int> tmp;
			UserFrame->__PnS.clear();
			UserFrame->__FnS.clear();
			if (!UserFrame->get<vector<int>>(ename + "Pns", tmp))
			{
				cout << "get __PnS failed." << endl;
				return false;
			}
			UserFrame->__PnS = tmp;
			if (!UserFrame->get<vector<int>>(ename + "Fns", tmp))
			{
				cout << "get __FnS failed." << endl;
				return false;
			}
			UserFrame->__FnS = tmp;
		}
		ItemisERC = pMap->getNamedItem("isERC");
		if (ItemisERC)
		{
			int curERC;
			if (!UserFrame->get<int>(ename, curERC))
			{
				cout << "Get current ERC failed." << endl;
				return false;
			}
			UserFrame->curERC = curERC;
		}
		ItemisUnitLength = pMap->getNamedItem("isunitlength");
		if (ItemisUnitLength)
		{
			int unitLength;
			if (!UserFrame->get<int>(ename, unitLength))
			{
				cout << "Get unitlength failed." << endl;
				return false;
			}
			UserFrame->unitLength = unitLength;
		}
		ItemisReference = pMap->getNamedItem("isreference");
		if (ItemisReference)
		{
			int reference;
			if (!UserFrame->get<int>(ename, reference))
			{
				cout << "Get reference value failed." << endl;
				return false;
			}
			UserFrame->reference = reference;
		}
	} catch (Exception &e) {
		cout << e.displayText() << endl;
		return false;
	}

	return true;
}

bool encodeRepeatNodes(UserDataUnit *UserFrame, Node *nextNode, Document *pDoc, NamedNodeMap *pMap, int *realDecodeNum, int afn, int pn, int fn)
{
	Node *Itemrepeatcount = NULL;
	Node *Itemeachlength = NULL;
	Node *Itemfixedlength = NULL;
	int repeatTimes = 0;

	try {
		Itemrepeatcount = pMap->getNamedItem("countid");
		if (!Itemrepeatcount)
		{
			cout << "Can't find countid item." << endl;
			return false;
		}
		if (Itemrepeatcount->nodeValue() == "NULL")
		{
			Itemeachlength = pMap->getNamedItem("eachLength");
			Itemfixedlength = pMap->getNamedItem("fixedLength");
			if (Itemeachlength && Itemfixedlength)
			{
				int eachlength = atoi(Itemeachlength->nodeValue().c_str());
				int fixedlength = atoi(Itemfixedlength->nodeValue().c_str());
				repeatTimes = (UserFrame->unitLength  - fixedlength ) / eachlength;
			}
		}else{
			auto cp = UserFrame->repeatcountMap.find(atoi(Itemrepeatcount->nodeValue().c_str()));
			if (cp == UserFrame->repeatcountMap.end())
			{
				cout << "Can't find repeat count in map." << endl;
				return false;
			}
			repeatTimes = cp->second;
		}

		NodeList *repeatNodes = nextNode->childNodes();
		unsigned long  nodesNum = repeatNodes->length();
		int elemNum = 0;
		Node *eachNode = NULL, *pParentNode = NULL;
		for (int i = 0; i < repeatTimes; i++)
		{
			for (unsigned long j = 0; j < nodesNum; j++)
			{
				eachNode = repeatNodes->item(j);
				if ((eachNode->nodeType() == Node::ELEMENT_NODE) && (eachNode->parentNode() != pParentNode))
				{
					NamedNodeMap *eachNodeMap = eachNode->attributes();
					Node *Itemrepeat = eachNodeMap->getNamedItem("repeat");
					if (!Itemrepeat)
					{
						if (!encodeEachNode(UserFrame, pDoc, eachNodeMap, afn, pn, fn, i+1))
						{
							eachNodeMap->release();
							return false;
						}
						if (i == 0)
							elemNum += 1;
					}
					else if (Itemrepeat->nodeValue() == "true")
					{
						pParentNode = eachNode;
						int Num = 0;
						if (!encodeRepeatNodes(UserFrame, eachNode, pDoc, eachNodeMap, &Num, afn, pn, fn))
						{
							eachNodeMap->release();
							return false;
						}
						if (i == 0)
							elemNum += Num + 1;
					}
					eachNodeMap->release();
				}
			}
		}
		*realDecodeNum = elemNum;
		repeatNodes->release();
	} catch (Exception &e) {
		cout << e.displayText() << endl;
		return false;
	}

	return true;
}

bool decodeRepeatNodes(UserDataUnit *UserFrame, Node *nextNode, Document *pDoc, NamedNodeMap *pMap, int *realDecodeNum, int afn, int pn, int fn)
{
	Node *Itemrepeatcount = NULL;
	Node *Itemeachlength = NULL;
	Node *Itemfixedlength = NULL;
	int repeatTimes = 0;
	try {
		Itemrepeatcount = pMap->getNamedItem("countid");
		if (!Itemrepeatcount)
		{
			cout << "Can't find countid item." << endl;
			return false;
		}
		if (Itemrepeatcount->nodeValue() == "NULL")
		{
			Itemeachlength = pMap->getNamedItem("eachLength");
			Itemfixedlength = pMap->getNamedItem("fixedLength");
			if (Itemeachlength && Itemfixedlength)
			{
				int eachlength = atoi(Itemeachlength->nodeValue().c_str());
				int fixedlength = atoi(Itemfixedlength->nodeValue().c_str()); 
				repeatTimes = (UserFrame->unitLength  - fixedlength ) / eachlength;
			}
		}
		else{
			auto cp = UserFrame->repeatcountMap.find(atoi(Itemrepeatcount->nodeValue().c_str()));
			if (cp == UserFrame->repeatcountMap.end())
			{
				cout << "Can't find repeat count in map." << endl;
				return false;
			}
			repeatTimes = cp->second;
		}

		NodeList *repeatNodes = nextNode->childNodes();
		unsigned long  nodesNum = repeatNodes->length();
		int elemNum = 0;
		Node *eachNode = NULL, *pParentNode = NULL;
		for (int i = 0; i < repeatTimes; i++)
		{
			for (unsigned long j = 0; j <  nodesNum; j++)
			{
				eachNode = repeatNodes->item(j);
				if ((eachNode->nodeType() == Node::ELEMENT_NODE) && (eachNode->parentNode() != pParentNode))
				{
					NamedNodeMap *eachNodeMap = eachNode->attributes();
					Node *Itemrepeat = eachNodeMap->getNamedItem("repeat");
					if (!Itemrepeat)
					{
						if (!decodeEachNode(UserFrame, pDoc, eachNodeMap, afn, pn, fn, i+1))
						{
							eachNodeMap->release();
							return false;
						}
						if (i == 0)
							elemNum += 1;
					}
					else if (Itemrepeat->nodeValue() == "true")
					{
						pParentNode = eachNode;
						int Num = 0;
						if (!decodeRepeatNodes(UserFrame, eachNode, pDoc, eachNodeMap, &Num, afn, pn, fn))
						{
							eachNodeMap->release();
							return false;
						}
						if (i == 0)
							elemNum += Num + 1;
					}
					eachNodeMap->release();
				}
			}
		}
		*realDecodeNum = elemNum;
		repeatNodes->release();
	} catch (Exception &e) {
		cout << e.displayText() << endl;
		return false;
	}

	return true;
}

bool encodeTraverseNodeTree(UserDataUnit *UserFrame, Document *pDoc, Node *pNode, int afn, int pn, int fn, int index)
{
	Node *Itemrepeat = NULL;
	Node *Itemrepeatcount = NULL;
	NamedNodeMap *pMap = NULL;

	try {
		NodeIterator it(pNode, NodeFilter::SHOW_ELEMENT);
		Node *nextNode = it.nextNode();
		nextNode = it.nextNode();
		while(nextNode)
		{
			Itemrepeat = NULL;
			Itemrepeatcount = NULL;
			pMap = nextNode->attributes();
			Itemrepeat = pMap->getNamedItem("repeat");
			if (!Itemrepeat || Itemrepeat->nodeValue() == "false")
			{
				if (!encodeEachNode(UserFrame, pDoc, pMap, afn, pn, fn, index))
				{
					cout<<"encodeEachNode failed!"<<endl;
					pMap->release();
					return false;
				}
			}
			else if (Itemrepeat->nodeValue() == "true")
			{
				int elemNum;
				if (!encodeRepeatNodes(UserFrame, nextNode, pDoc, pMap, &elemNum, afn, pn, fn))
				{
					cout<<"encodeRepeatNodes failed!"<<endl;
					pMap->release();
					return false;
				}
				for (int i = 0; i < elemNum; i++)
					nextNode = it.nextNode();
			}
			pMap->release();
			nextNode = it.nextNode();
		}
	} catch (Exception &e) {
		cout << e.displayText() << endl;
		return false;
	}

	return true;
}

bool decodeTraverseNodeTree(UserDataUnit *UserFrame, Document *pDoc, Node *pNode, int afn, int pn, int fn, int index)
{
	Node *Itemrepeat = NULL;
	Node *Itemrepeatcount = NULL;
	NamedNodeMap *pMap = NULL;

	try {
		NodeIterator it(pNode, NodeFilter::SHOW_ELEMENT);
		Node *nextNode = it.nextNode();
		nextNode = it.nextNode();

		while(nextNode)
		{
			Itemrepeat = NULL;
			Itemrepeatcount = NULL;
			pMap = nextNode->attributes();
			Itemrepeat = pMap->getNamedItem("repeat");
			if (!Itemrepeat || Itemrepeat->nodeValue() == "false")
			{
				if (!decodeEachNode(UserFrame, pDoc, pMap, afn, pn, fn, index))
				{
					pMap->release();
					return false;
				}
			}
			else if (Itemrepeat->nodeValue() == "true")
			{
				int elemNum;
				if (!decodeRepeatNodes(UserFrame, nextNode, pDoc, pMap, &elemNum, afn, pn, fn))
				{
					pMap->release();
					return false;
				}
				for (int i = 0; i < elemNum; i++)
					nextNode = it.nextNode();
			}
			pMap->release();
			nextNode = it.nextNode();
		}
	} catch (Exception &e) {
		cout << e.displayText() << endl;
		return false;
	}

	return true;
}

bool encodeAccordingXML(UserDataUnit *UserFrame, Document *pDoc, int afn, int pn, int fn)
{
	try
	{
		Node *pNode = NULL;
		ostringstream oss;
		oss << "/FRAMENX/AFN[@id='" << afn << "']/FN[@id='" << fn << "']";
		string strpath = oss.str();
		if (!FindPositionNode(pDoc, &pNode, strpath))
			return false;
		NamedNodeMap *pMap = pNode->attributes();
		Node* hasDataUnit = pMap->getNamedItem("hasDataUnit");
		pMap->release();
		if (hasDataUnit->nodeValue() == "false")
		{
			return true;
		}else
			return encodeTraverseNodeTree(UserFrame, pDoc, pNode, afn, pn, fn, 0);
	} catch (Exception& e) {
		cerr << e.displayText() << endl;
		return false;
	}

	return true;
}

bool decodeAccordingXML(UserDataUnit *UserFrame, Document *pDoc, int afn, int pn, int fn)
{
	try 
	{
		Node *pNode = NULL;
		ostringstream oss;
		oss << "/FRAMENX/AFN[@id='" << afn << "']/FN[@id='" << fn << "']";
		string strpath = oss.str();
		if (!FindPositionNode(pDoc, &pNode, strpath))
			return false;
		NamedNodeMap *pMap = pNode->attributes();
		Node* hasDataUnit = pMap->getNamedItem("hasDataUnit");
		pMap->release();
		if (hasDataUnit->nodeValue() == "false")
			return true;
		else
			return decodeTraverseNodeTree(UserFrame, pDoc, pNode, afn, pn, fn, 0);
	} catch (Exception& e) {
		cerr << e.displayText() << endl;
		return false;
	}

	return true;
}

int UserDataUnit::findItems(int pn, int fn, string ename)
{
	int index = -1;

	for (int i = 0; i < (int)_items.size(); i++)
	{
		if (_items[i].pn == pn &&
			_items[i].fn == fn &&
			_items[i].ename == ename)
		{
			return i;
		}
	}

	return index;
}

bool UserDataUnit::encode(uint8_t *pos)
{
	int afn = 0, fn = 0, pn = 0;
	if (!get<int>("afn", afn))
	{
		return false;
	}
	if (!get<int>("fn", fn))
	{
		return false;
	}
	if (!get<int>("pn", pn))
	{
		return false;
	}
	encodePos = pos;

	DOMParser parser;
	AutoPtr<Document> pDoc = parser.parse(down_xml);

	return encodeAccordingXML(this, pDoc, afn, pn, fn);
}

bool UserDataUnit::decode(const uint8_t *pos)
{
	int afn = 0, fn = 0, pn = 0;
	get<int>("afn", afn);
	get<int>("fn", fn);
	get<int>("pn", pn);
	decodePos = pos;

	DOMParser parser;
	AutoPtr<Document> pDoc = parser.parse(up_xml);

	return decodeAccordingXML(this, pDoc, afn, pn, fn);
}

bool VerFrame::encode(uint8_t *pos)
{
	STVerFrameData *frameData = (STVerFrameData *)pos;

	frameData->FirstBegin = VERIABLE_FRAME_HEAD;
	frameData->SecondBegin = VERIABLE_FRAME_HEAD;

	int dataSize = 0;
	if (!get<int>("dataSize", dataSize))
	{
		return false;
	}
	uint8_t *aux = pos + VERIABLE_LENGTH_BEFORE_PNFNS + dataSize;

	Control control;
	if (get<Control>("control", control))
	{
		if (!control.encode((uint8_t *)(&frameData->cdata)))
		{
			return false;
		}
	}else {
		return false;
	}

	Address address;
	if (get<Address>("address", address))
	{
		if (!address.encode((uint8_t *)(&frameData->adata)))
		{
			return false;
		}
	}else {
		return false;
	}

	int protocolVersion = 0;
	if (get<int>("protocolVersion", protocolVersion))
	{
		frameData->protocolType_D0_D5_version = protocolVersion;
	}else {
		return false;
	}

	int protocolFlag = 0;
	if (get<int>("protocolFlag", protocolFlag))
	{
		frameData->protocolType_D6_flag = protocolFlag;
	}else {
		return false;
	}

	int protocolReserve = 0;
	if (get<int>("protocolReserve", protocolReserve))
	{
		frameData->protocolType_D7_reserve = protocolReserve;
	}else {
		return false;
	}

	AppControl appControl;
	if (get<AppControl>("appControl", appControl))
	{
		if (!appControl.encode((uint8_t *)(&frameData->appControl)))
		{
			return false;
		}
	}else {
		return false;
	}

	bool hasEventCount = false, hasPassword = false, hasTpv = false;
	if (!get<bool>("hasEventCount", hasEventCount))
	{
		return false;
	}
	if (!get<bool>("hasPassword", hasPassword))
	{
		return false;
	}
	if (!get<bool>("hasTpv", hasTpv))
	{
		return false;
	}

	if (hasEventCount)
	{
		dataSize += 4;
	}
	if (hasPassword)
	{
		dataSize += 16;
	}else if (hasTpv)
	{
		dataSize += 5;
	}

	if (hasEventCount)
	{
		FEvent ec;
		if (get<FEvent>("ec", ec))
		{
			ec.encode(aux);
		}else {
			return false;
		}
		aux += 4;
	}

	if (hasPassword)
	{
		Password pw;
		if (get<Password>("pw", pw))
		{
			pw.encode(aux);
		}else {
			return false;
		}
		aux += 16;
	}

	if (hasTpv)
	{
		TimeStamp tp;
		if (get<TimeStamp>("tp", tp))
		{
			tp.encode(aux);
		}else {
			return false;
		}
		aux += 5;
	}

	int length = dataSize + VERIABLE_FRAME_FIXED_DATA_SIZE;
	frameData->FirstLengthByte1 = length & 0xff;
	frameData->FirstLengthByte2 = (length >> 8) & 0xff;
	frameData->SecondLengthByte1 = length & 0xff;
	frameData->SecondLengthByte2 = (length >> 8) & 0xff;

	const uint8_t *checkBegin = pos + VERIABLE_CS_BEGIN_POS;
	uint8_t *checkEnd = pos + VERIABLE_CS_BEGIN_POS + length;
	int checksum = 0;
	for (const uint8_t *index = checkBegin; index < checkEnd; index++)
		checksum += *index;

	*checkEnd = checksum & 0xff;

	uint8_t *tail = aux + 1;
	*tail = FRAME_TAIL;

	set<int>("length", tail+1-pos);

	return true;
}

bool VerFrame::decode(const uint8_t *pos)
{
	STVerFrameData *frameData = (STVerFrameData *)pos;

	if (frameData->FirstBegin != VERIABLE_FRAME_HEAD)
	{
		cout << "The first frame head error." << endl;
		return false;
	}
	set<int>("firstHead", frameData->FirstBegin);

	int firstLength = (frameData->FirstLengthByte1 & 0xff) + ((frameData->FirstLengthByte2 << 8) & 0xffff);
	int secondLength = (frameData->SecondLengthByte1 & 0xff) + ((frameData->SecondLengthByte2 << 8) & 0xffff);
	if (firstLength != secondLength)
	{
		cout << "The two frame length are not equal." << endl;
		return false;
	}
	set<int>("firstLength", firstLength);
	set<int>("secondLength", secondLength);

	if (frameData->SecondBegin != VERIABLE_FRAME_HEAD)
	{
		cout << "The second frame head error." << endl;
		return false;
	}
	set<int>("secondHead", frameData->SecondBegin);

	const uint8_t *checkBegin = pos + VERIABLE_CS_BEGIN_POS;
	const uint8_t *checkEnd = pos + VERIABLE_CS_BEGIN_POS + firstLength;
	int checksum = 0;
	for (const uint8_t *index = checkBegin; index < checkEnd; index++)
		checksum += *index;
	if ((checksum & 0xff) != (*checkEnd & 0xff))
	{
		cout << "Frame check sum failed." << endl;
		return false;
	}

	int dataSize = firstLength - VERIABLE_FRAME_FIXED_DATA_SIZE;
	set<int>("dataSize", dataSize);

	Control control;
	if (control.decode((uint8_t *)(&frameData->cdata)))
		set<Control>("control", control);
	else
		return false;

	Address address;
	if (address.decode((uint8_t *)(&frameData->adata)))
	{
		set<Address>("address", address);
	}else
		return false;

	if (frameData->protocolType_D6_flag != FRAMENX_FRAME_FLAG)
	{
		cout << "Frame protocol type error." << endl;
		return false;
	}
	set<int>("protocolVersion", frameData->protocolType_D0_D5_version);
	set<int>("protocolFlag", frameData->protocolType_D6_flag);
	set<int>("protocolReserve", frameData->protocolType_D7_reserve);

	AppControl ac;
	if (ac.decode((uint8_t *)&frameData->appControl))
	{
		int Afn = 0;
		if (ac.get<int>("acAfn", Afn))
		{
			set<int>("Afn", Afn);
		}else {
			return false;
		}
		set<AppControl>("appControl", ac);
	}else
		return false;

	bool hasEventCount = false, hasPassword = false, hasTpv = false;
	bool dir = false, acAcd = false, acTpv = false;
	int acAfn = 0;

	if (!control.get<bool>("Dir", dir))
		return false;
	if (!ac.get<bool>("acAcd", acAcd))
		return false;
	if (!ac.get<bool>("acTpv", acTpv))
		return false;
	if (!ac.get<int>("acAfn", acAfn))
		return false;

	if (dir && acAcd)
	{
		hasEventCount = true;
		dataSize -= 4;
	}else {
		hasEventCount = false;
	}

	if ((!dir) && (
				(acAfn == 1) ||
				(acAfn == 4) ||
				(acAfn == 5) ||
				(acAfn == 6) ||
				(acAfn == 15) ||
				(acAfn == 16)))
	{
		hasPassword = true;
		dataSize -= 16;
	}else {
		hasPassword = false;
	}

	if (acTpv)
	{
		hasTpv = true;
		dataSize -= 5;
	}else {
		hasTpv = false;
	}

	set<bool>("hasEventCount", hasEventCount);
	set<bool>("hasPassword", hasPassword);
	set<bool>("hasTpv", hasTpv);
	set<int>("dataSize", dataSize);

	const uint8_t *aux = pos + VERIABLE_LENGTH_BEFORE_PNFNS + dataSize;
	if (hasEventCount)
	{
		FEvent ec;
		if (ec.decode(aux))
			set<FEvent>("ec", ec);
		else
			return false;
		aux += 4;
	}else if (hasPassword)
	{
		Password pw;
		if (pw.decode(aux))
			set<Password>("pw", pw);
		else
			return false;
		aux += 16;
	}

	if (hasTpv)
	{
		TimeStamp tp;
		if (tp.decode(aux))
			set<TimeStamp>("tp", tp);
		else
			return false;
		aux += 5;
	}

	const uint8_t *tail = aux + 1;
	if (*tail != FRAME_TAIL)
	{
		cout << "Frame tail error." << endl;
		return false;
	}
	set<int>("Tail", *tail);

	return true;
}

bool FrameNx::dataItemsToItems()
{
	Item item;
	char tmp[10] = {0};
	vector<Item> XBMaxes;

	for (int i = 0; i < (int)_dataItems.size(); i++)
	{
		item.pn = _dataItems[i].termId;

		if (!parseByItemId(_dataItems[i].itemId, item.fn, item.code, item.format, item.repeatCount))
		{
			cout<<"parseByItemId failed!"<<endl;
			cout<<"fn:["<<item.fn<<"]"<<endl;
			cout<<"code:["<<item.code<<"]"<<endl;
			cout<<"format:["<<item.format<<"]"<<endl;
			cout<<"repeatCount:["<<item.repeatCount<<"]"<<endl;
			return false;
		}
		sprintf(tmp, "%X", _dataItems[i].itemId);
		item.ename = string(tmp);
		item.value = _dataItems[i].value;
		item.time = _dataItems[i].time;

		if (item.format == "CSXB")//谐波
		{
			bool flag = false;
			for (int j = 0; j < (int)XBMaxes.size(); j++)
			{
				if (XBMaxes[j].pn == item.pn &&
						XBMaxes[j].fn == item.fn)//找到pn,fn
				{
					flag = true;
					if (XBMaxes[j].code < item.code)
					{
						XBMaxes[j].pn = item.pn;
						XBMaxes[j].fn = item.fn;
						XBMaxes[j].ename = item.ename;
						XBMaxes[j].code = item.code;
						XBMaxes[j].format = item.format;
						XBMaxes[j].value = item.value;
						XBMaxes[j].time = item.time;
					}
					break;
				}
			}
			if (!flag)//未找到
			{
				XBMaxes.push_back(item);
			}
		}else {//非谐波
			_items.push_back(item);
		}
	}

	for (int i = 0; i < (int)XBMaxes.size(); i++)//插入谐波次数最大值
	{
		_items.push_back(XBMaxes[i]);
	}

	return true;
}

bool FrameNx::parseByItemId(int itemId, int &Fn, int &Code, string &Format, string &repeatCount)
{
	char tmp[10] = {0};
	string ename;

	sprintf(tmp, "%X", itemId);
	ename = string(tmp);

	try
	{
		DOMParser parser;
		AutoPtr<Document> pDoc = parser.parse(down_xml);

		Node *pNode = NULL;

		ostringstream oss;
		oss << "//[@ename='" << ename << "']";
		string strpath = oss.str();
		if (!FindPositionNode(pDoc, &pNode, strpath))
			return false;

		Node *Itemformat = NULL;
		NamedNodeMap *pMap = pNode->attributes();

		Node *Itemcode = NULL;
		Itemcode = pMap->getNamedItem("code");
		if (Itemcode)
		{
			sscanf(Itemcode->nodeValue().c_str(), "%d", &Code);
			if (Code < 0)
			{
				pMap->release();
				return false;
			}
		}else {
			Code = 0;
		}

		Itemformat = pMap->getNamedItem("format");
		if (Itemformat)
		{
			if (Itemformat->nodeValue() != "")
			{
				Format = Itemformat->nodeValue();
			}else {
				pMap->release();
				return false;
			}
		}else {
			pMap->release();
			return false;
		}

		for (pNode = pNode->parentNode();
				pNode && pNode->nodeName() != "FN";
				pNode = pNode->parentNode());

		if (!pNode)
		{
			cout << "parent Node is NULL!" << endl;
			pMap->release();
			return false;
		}

		pMap->release();
		pMap = pNode->attributes();
		Node* Itemid = pMap->getNamedItem("id");
		pMap->release();
		if (!Itemid){
			cout << "getNamedItem id failed!" << endl;
			return false;
		}

		sscanf(Itemid->nodeValue().c_str(), "%d", &Fn);
		if (Fn < 0 || Fn > 255)
		{
			return false;
		}

		//查找isrepeatcount Node
		if (Format == "CSXB")
		{
			Node *pNode = NULL;

			ostringstream oss;
			oss << "/FRAMENX/AFN[@id='" << afn << "']/FN[@id='" << Fn << "']/elem[@isrepeatcount='true']";
			string strpath = oss.str();
			if (!FindPositionNode(pDoc, &pNode, strpath))
				return false;

			NamedNodeMap *pMap = pNode->attributes();
			Node *Itemename = pMap->getNamedItem("ename");
			pMap->release();
			if (!Itemename)
			{
				return false;
			}
			repeatCount = Itemename->nodeValue();
		}

	} catch (Exception& e) {
		cerr << e.displayText() << endl;
		return false;
	}

	return true;
}

bool FrameNx::fillPnSet()
{
	for (int i = 0; i < (int)_items.size(); ++i)
	{
		bool flag = false;
		for (int j = 0; j < (int)_pnSet.size(); ++j)
		{
			if (_pnSet[j] == _items[i].pn)
			{
				flag = true;
				break;
			}
		}
		if (!flag)
		{
			_pnSet.push_back(_items[i].pn);
		}
	}

	sortSet(_pnSet);
	_pSetIter = _pnSet.begin();

	return true;
}

bool FrameNx::fillFnSet()
{
	for (int i = 0; i < (int)_items.size(); ++i)
	{
		bool flag = false;
		for (int j = 0; j < (int)_fnSet.size(); ++j)
		{
			if (_fnSet[j] == _items[i].fn)
			{
				flag = true;
				break;
			}
		}
		if (!flag)
		{
			_fnSet.push_back(_items[i].fn);
		}
	}
	sortSet(_fnSet);
	_fSetIter = _fnSet.begin();

	return true;
}

void FrameNx::sortSet(vector<int> &values)
{
	for (int i = 0; i <= (int)values.size(); ++i)
	{
		for (int j = i + 1; j < (int)values.size(); ++j)
		{
			if (values[i] > values[j])
			{
				int value = values[i];
				values[i] = values[j];
				values[j] = value;
			}
		}
	}
}

bool FrameNx::isExist(int pn, int fn, bool flag)
{
	for (int i = 0; i < (int)_items.size(); ++i)
	{
		if (pn == _items[i].pn && fn == _items[i].fn)
		{
			//是否真的存在，【flag = false】:代表是否未加入过
			if (flag)
			{
				return true;
			}else if (_items[i].exist)
			{
				return true;
			}else {
				return false;
			}
		}
	}

	return false;
}

bool FrameNx::isSamePnGroup(int pn1, int pn2)
{
	if (pn1 == 0 && pn2 == 0)
	{
		return true;
	}else if (pn1 == 0 || pn2 == 0)
	{
		return false;
	}

	int group1 = (pn1-1)/8 + 1;
	int group2 = (pn2-1)/8 + 1;

	if (group1 == group2)
	{
		return true;
	}else {
		return false;
	}
}

bool FrameNx::isSameFnGroup(int fn1, int fn2)
{
	int group1 = (fn1-1)/8;
	int group2 = (fn2-1)/8;

	if (group1 == group2)
	{
		return true;
	}else {
		return false;
	}
}

bool FrameNx::findItems(vector<int> &index, int pn, int fn)
{
	bool flag = false;
	for (int i = 0; i < (int)_items.size(); ++i)
	{
		if (pn == _items[i].pn && fn == _items[i].fn)
		{
			index.push_back(i);
			flag = true;
		}
	}

	return flag;
}

int FrameNx::getPnFns()
{
	vector<int>::iterator piter_ = _pSetIter;
	vector<int>::iterator fiter_ = _fSetIter;
	int result = 0;
	bool pnSameGroup = true, fnSameGroup = true;

	for (piter_ = _pSetIter; piter_ != _pnSet.end(); ++piter_)
	{
		if (!pns.empty())
		{
			if (!isSamePnGroup(*pns.begin(), *piter_))
			{
				pnSameGroup = false;
				break;
			}
		}
		for (fiter_ = _fSetIter; fiter_ != _fnSet.end(); ++fiter_)
		{
			if (!fns.empty())
			{
				if (!isSameFnGroup(*fns.begin(), *fiter_))
				{
					fnSameGroup = false;
					break;
				}
			}
			if (isExist(*piter_, *fiter_))
			{
				//判断加入该点后随带加入的点是否都存在，且从未加入过
				bool flag = true;
				for (int i = 0; i < (int)pns.size(); ++i)
				{
					if (!flag || !isExist(pns[i], *fiter_))
					{
						flag = false;
						break;
					}
				}
				for (int i = 0; i < (int)fns.size(); ++i)
				{
					if (!flag || !isExist(*piter_, fns[i]))
					{
						flag = false;
						break;
					}
				}

				if (flag)//可以加入
				{
					//加入该点，并标记相同pn，fn的所有数据
					vector<int> index;
					if (findItems(index, *piter_, *fiter_))
					{
						for (int i = 0; i < (int)index.size(); i++)
						{
							_items[index[i]].exist = false;
						}
						pns.push_back(*piter_);
						fns.push_back(*fiter_);
						result = 1;
					}else {
						cout<<"find error"<<endl;
						return -1;
					}

					//标记随带加入的pn点所有数据
					index.clear();
					for (int i = 0; i < (int)pns.size(); ++i)
					{
						if (findItems(index, pns[i], *fiter_))
						{
							for (int i = 0; i < (int)index.size(); i++)
							{
								_items[index[i]].exist = false;
							}
						}else {
							cout<<"find error"<<endl;
							return -1;
						}
					}

					//标记随带加入的fn点所有数据
					index.clear();
					for (int i = 0; i < (int)fns.size(); ++i)
					{
						if (findItems(index, *piter_, fns[i]))
						{
							for (int i = 0; i < (int)index.size(); i++)
							{
								_items[index[i]].exist = false;
							}
						}else {
							cout<<"find error"<<endl;
							return -1;
						}
					}
				}
			}
		}
	}

	return result;
}

bool FrameNx::encode(uint8_t *pos)
{
	bool isFixedFrame = false;

	if (get<bool>("isFixedFrame", isFixedFrame))
	{
		if (isFixedFrame)
		{
			FixedFrame fixedFrame;
			if (get<FixedFrame>("fixedFrame", fixedFrame))
			{
				if (!fixedFrame.encode(pos))
				{
					return false;
				}
				frameLength = sizeof(STFixedFrameData);
			}else {
				return false;
			}
		}else {//isFixedFrame is false
			VerFrame verFrame;
			if (get<VerFrame>("verFrame", verFrame))
			{
				verFrame.set<int>("dataSize", dataSize);
				if (verFrame.encode(pos))
				{
					int length = 0;
					if (verFrame.get<int>("length", length))
					{
						encodePos = pos;
						frameLength = length;
					}else {
						return false;
					}
				}else {
					return false;
				}
			}else {
				return false;
			}
		}
	}else {
		return false;
	}

	return true;
}

bool FrameNx::decode(const uint8_t *pos)
{
	int head = *pos;

	if (head == FIXED_FRAME_HEAD)
	{
		set<bool>("isFixedFrame", true);
		FixedFrame fixedFrame;
		if (fixedFrame.decode(pos))
			set<FixedFrame>("fixedFrame", fixedFrame);
		else
			return false;
	}
	else if (head == VERIABLE_FRAME_HEAD)
	{
		set<bool>("isFixedFrame", false);
		VerFrame verFrame;
		if (verFrame.decode(pos))
		{
			set<VerFrame>("verFrame", verFrame);

			decodePos = pos + VERIABLE_LENGTH_BEFORE_PNFNS;
			int ver_afn = 0;
			if (verFrame.get<int>("Afn", ver_afn))
			{
				afn = ver_afn;
			}else {
				return false;
			}
			int ver_dataSize;
			if (verFrame.get<int>("dataSize", ver_dataSize))
			{
				dataSize = ver_dataSize;
			}else
				return false;
		}
		else
			return false;
	}
	else
	{
		//Log error
		cout << "Unkown frame error." << endl;
		return false;
	}

	return true;
}

//////////数据格式解析、编码
bool CSNONE::encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len)
{
	return true;
}

bool CSNONE::decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len)
{
	return true;
}
bool CSXB::encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len)
{
	return true;
}

bool CSXB::decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len)
{
	return true;
}

bool CSNDQ::encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len)
{
	return true;
}

bool CSNDQ::decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len)
{
	return true;
}

bool CSCOMM::decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len)
{
	return true;
}

bool CSCOMM::encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len)
{
	STCOMM *stData = (STCOMM *)pos;

	string eName = ename;
	double value = 0;

	map<int, int> mapBaudRate;
	mapBaudRate.insert(pair<int, int>(1200, 1));
	mapBaudRate.insert(pair<int, int>(2400, 2));
	mapBaudRate.insert(pair<int, int>(4800, 3));
	mapBaudRate.insert(pair<int, int>(9600, 4));
	mapBaudRate.insert(pair<int, int>(19200, 5));
	mapBaudRate.insert(pair<int, int>(38400, 6));
	mapBaudRate.insert(pair<int, int>(43000, 7));

	int baudRate = 0;
	int stopBit = 0;
	int hasCheckBit = 0;
	int checkBit = 0;
	int dataBit = 0;

	int postion = 0;
	int index = eName.find_first_of(":", postion);
	string key = eName.substr(postion,index-postion+1);
	postion = index+1;
	index = eName.find_first_of("-", postion);

	for (int i = 0; ; i++)
	{
		string keyStr;
		if (index == (int)string::npos)
		{
			keyStr = eName.substr(postion,eName.length()-postion+1);
		}else {
			keyStr = eName.substr(postion,index-postion);
		}

		switch (i)
		{
		case 0://BaudRate
		{
			if (!frame->get<double>(key + keyStr, value))
			{
				cout << "get BaudRate failed. (ename = " << key + keyStr <<")"<< endl;
				return false;
			}
			int baud_rate = value;
			map<int, int>::iterator iter = mapBaudRate.find(baud_rate);
			if (iter == mapBaudRate.end())
			{
				cout << "BaudRate error.(value = "<< baud_rate <<")" << endl;
				return false;
			}
			baudRate = iter->second;
			break;
		}
		case 1://StopBit
		{
			if (!frame->get<double>(key + keyStr, value))
			{
				cout << "get StopBit failed." << endl;
				return false;
			}
			int bit = value;
			if (bit != 1 && bit != 2)
			{
				cout << "StopBit error." << endl;
				return false;
			}
			stopBit = bit - 1;
			break;
		}
		case 2://HasCheckBit
		{
			if (!frame->get<double>(key + keyStr, value))
			{
				cout << "get HasCheckBit failed." << endl;
				return false;
			}
			int bit = value;
			if (bit != 0 && bit != 1)
			{
				cout << "HasCheckBit error." << endl;
				return false;
			}
			hasCheckBit = bit;
			break;
		}
		case 3://CheckBit
		{
			if (!frame->get<double>(key + keyStr, value))
			{
				cout << "get CheckBit failed." << endl;
				return false;
			}
			int bit = value;
			if (bit != 0 && bit != 1)
			{
				cout << "CheckBit error." << endl;
				return false;
			}
			checkBit = bit;
			break;
		}
		case 4://DataBit
		{
			if (!frame->get<double>(key + keyStr, value))
			{
				cout << "get DataBit failed." << endl;
				return false;
			}
			int bit = value;
			if (bit < 5 || bit > 8)
			{
				cout << "DataBit error." << endl;
				return false;
			}
			dataBit = bit - 5;
			break;
		}
		default:
		{
			break;
		}
		}

		if (index == (int)string::npos)
		{
			break;
		}
		postion = index+1;
		index = eName.find_first_of("-", postion);
	}

	stData->STBaudRateBit3 = baudRate;
	stData->STStopBit1 = stopBit;
	stData->STHasCheckBit1 = hasCheckBit;
	stData->STCheckBit1 = checkBit;
	stData->STDataBit2 = dataBit;

	return true;
}

bool CSBIN::encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len)
{
	double v;
	if (!frame->get<double>(ename, v))
	{
		cout<<"get double "<<ename<<" failed."<<endl;
		return false;
	}

	int value = v;
	for (int i = 0; i < len; i++)
	{
		*(pos + i) = (value >> i*8) & 0xff;
	}

	return true;
}

bool CSBIN::decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len)
{
	bool invaild = false;
	int value = 0;
	for (int i = 0; i < len; i++)
	{
		if (*(pos + i) != '\xee')
			invaild = true;
		value += ((*(pos + i)) << (i * 8));
	}

	if (invaild)
		frame->set<int>(ename, value);
	else
		frame->set<int>(ename, 0);
	return true;
}

bool CSBINBits::encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len, int startBit, int endBit, int finishFlag)
{
	cout<<"CSBINBits encode 未实现"<<endl;
	return false;
}

bool CSBINBits::decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len, int startBit, int endBit, int finishFlag)
{
	bool invaild = false;
	int value = 0;
	for (int i = 0; i < len; i++)
	{
		if (*(pos + i) != '\xee')
			invaild = true;
		value += ((*(pos + i)) << (i * 8));
	}

	int And = 0;
	for (int i = startBit; i <= endBit; i++)
		And += (1 << i); 

	(value &= And) >>= startBit;
	
	if (invaild)
		frame->set<int>(ename, value);
	else
		frame->set<int>(ename, 0);

	if (!finishFlag)
		len = 0;

	return true;
}

bool CSASCII::encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len)
{
	cout<<"CSASCII encode 未实现"<<endl;
	return false;
//	double v;
//	if (!frame->get<double>(ename, v))
//	{
//		return false;
//	}

//	len = value.size();
//	strncpy((char *)pos, //value.c_str(), len);

	return true;
}

bool CSASCII::decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len)
{
	string str((const char *)pos, len);
	
	bool invaild = false;
	for (int i = 0; i < len; i++)
	{
		if (str[i] != '\xee')
		{
			invaild = true;
			break;
		}
	}
	
	if (invaild)
		frame->set<string>(ename, str);
	else
		frame->set<string>(ename, "");
	return true;
}

bool CSPnFnS::encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len)
{
	cout<<"CSPnFnS encode 未实现"<<endl;
	return false;
}

bool CSPnFnS::decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len)
{
	STPnFnS * STData = (STPnFnS *)pos;

	string name;
	vector<int> pns;
	STData->Pn.decode(pns, 1);
	name = ename + "Pns";
	frame->set<vector<int>>(name, pns);

	vector<int> fns;
	STData->Fn.decode(fns, 0);
	name = ename + "Fns";
	frame->set<vector<int>>(name, fns);

	return true;
}

bool CSBS8::encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len)
{
	cout<<"CSBS8 encode 未实现"<<endl;
	return false;
//	double v;
//	if (!frame->get<double>(ename, v))
//	{
//		return false;
//	}
//	unsigned char value = v;
//	STBS8 *stData = (STBS8 *)pos;
//	stData->Byte = value;
//
//	return true;
}

bool CSBS8::decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len)
{
	STBS8 invaild;
	memset(&invaild, '\xee', sizeof(invaild));
	
	STBS8 *stData = (STBS8 *)pos;
	if (memcmp(&invaild, stData, sizeof(invaild)) == 0)
	{
		frame->set<int>(ename, 0);
		return true;
	}

	for (int i = 1; i <= 8; i++)
		frame->set<int>(Poco::format("%s%d", ename, i), ((stData->Byte) >> (i-1)) & 0x1);

	return true;
}

bool CSBS64::encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len)
{
	cout<<"CSBS64 encode 未实现"<<endl;
	return false;
}

bool CSBS64::decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len)
{
	STBS64 invaild;
	memset(&invaild, '\xee', sizeof(invaild));
	
	STBS64 *stData = (STBS64 *)pos;
	if (memcmp(&invaild, stData, sizeof(invaild)) == 0)
	{
		frame->set<int>(ename, 0);
		return true;
	}

	int n = 1;
	for (int i = 0; i < 8; i++)
		for (int j = 1; j <= 8; j++)
		frame->set<int>(Poco::format("%s%d", ename, n++), ((stData->BS[i]) >> (j-1)) & 0x1);
	return true;
}

bool CSBS248::encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len)
{
	cout<<"CSBS248 encode 未实现"<<endl;
	return false;
}

bool CSBS248::decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len)
{
	STBS248 invaild;
	memset(&invaild, '\xee', sizeof(invaild));
	
	STBS248 *stData = (STBS248 *)pos;
	if (memcmp(&invaild, stData, sizeof(invaild)) == 0)
	{
		frame->set<int>(ename, 0);
		return true;
	}

	int n = 1;
	for (int i = 0; i < 31; i++)
		for (int j = 1; j <= 8; j++)
		frame->set<int>(Poco::format("%s%d", ename, n++), ((stData->BS[i]) >> (j-1)) & 0x1);
	return true;
}

bool CSIP::encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len)
{
	STIP *stData = (STIP *)pos;

	double v;
	if (!frame->get<double>(ename, v))
	{
		cout << "get double " << ename << " failed." << endl;
		return false;
	}

	if (len == 5)
	{
		uint32_t ip = (uint32_t)v;
		stData->IPVersion = 1;
		stData->IPV4.IPSeg1 = (ip>>(3*8)) & 0xff;
		stData->IPV4.IPSeg2 = (ip>>(2*8)) & 0xff;
		stData->IPV4.IPSeg3 = (ip>>(1*8)) & 0xff;
		stData->IPV4.IPSeg4 = (ip>>(0*8)) & 0xff;
	}else if (len == 17){
		uint64_t ip = (uint64_t)v;
		stData->IPVersion = 2;
		stData->IPV6.IPSeg1 = (ip>>(5*8)) & 0xff;
		stData->IPV6.IPSeg2 = (ip>>(3*8)) & 0xff;
		stData->IPV6.IPSeg4 = (ip>>(2*8)) & 0xff;
		stData->IPV6.IPSeg5 = (ip>>(1*8)) & 0xff;
		stData->IPV6.IPSeg6 = (ip>>(0*8)) & 0xff;
	}else {
		cout<<"地址长度错误！"<<endl;
		return false;
	}
	return true;
}

bool CSIP::decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len)
{
	STIP invaild;
	memset(&invaild, '\xee', sizeof(invaild));
	
	STIP * stData = (STIP *)pos;
	if (memcmp(&invaild, stData, sizeof(invaild)) == 0)
	{
		frame->set<string>(ename, "");
		return true;
	}

	if (stData->IPVersion == 2) //IPV6
	{
		string IPV6 = "xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx";
		frame->set<string>(ename, IPV6);
		len = 17;
	}
	else
	{
		struct in_addr ipV4;
		ipV4.s_addr = ((*pos)<<8) + *(pos+1);
		string IPV4 = "xxx.xxx.xxx.xxx";
		IPV4 = inet_ntoa(ipV4);

		frame->set<string>(ename, IPV4);
		len = 5;
	}

	return true;
}

bool CSBCDType1::encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len)
{
	STBCDType1 *stData = (STBCDType1 *)pos;
	double v;
	if (!frame->get<double>(ename, v))
	{
		return false;
	}
	long int value = v;
	memset(stData, 0, sizeof(STBCDType1));

	Timestamp timeStamp = Timestamp::fromEpochTime(value);
	LocalDateTime localDateTime(timeStamp);

	stData->YearTen = localDateTime.year()%100/10;
	stData->YearUni = localDateTime.year()%10;
	stData->WeekMon_Mon_Ten = localDateTime.month()/10;
	stData->WeekMon_Mon_Uni = localDateTime.month()%10;
	stData->WeekMon_Week = localDateTime.dayOfWeek()?localDateTime.dayOfWeek():7;
	stData->DayTen = localDateTime.day()/10;
	stData->DayUni = localDateTime.day()%10;
	stData->HourTen = localDateTime.hour()/10;
	stData->HourUni = localDateTime.hour()%10;
	stData->MinTen = localDateTime.minute()/10;
	stData->MinUni = localDateTime.minute()%10;
	stData->SecTen = localDateTime.second()/10;
	stData->SecUni = localDateTime.second()%10;

	return true;
}

bool CSBCDType1::decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len)
{
	STBCDType1 invaild;
	memset(&invaild, '\xee', sizeof(invaild));
	
	STBCDType1 * stData = (STBCDType1 *)pos;
	if (memcmp(&invaild, stData, sizeof(invaild)) == 0)
	{
		frame->set<time_t>(ename, 0);
		return true;
	}

	Poco::LocalDateTime cur;
	try {
		Poco::LocalDateTime recur(cur.year() - cur.year()%100 + stData->YearUni + stData->YearTen * 10,
				stData->WeekMon_Mon_Uni + stData->WeekMon_Mon_Ten * 10,
				stData->DayUni + stData->DayTen * 10,
				stData->HourUni + stData->HourTen * 10,
				stData->MinUni + stData->MinTen * 10,
				stData->SecUni + stData->SecTen * 10,
				cur.millisecond(),
				cur.microsecond()
		);
		Poco::Timestamp stamp = Timestamp::fromUtcTime(recur.utcTime());
		std::time_t tmp = stamp.epochTime();

		frame->set<time_t>(ename, tmp);
	}catch (Exception &e)
	{
		cout << e.displayText() << endl;
		cout << "decode CSBCDType1 failed!" << endl;
		return false;
	}catch (...)
	{
		cout << "decode CSBCDType1 failed!" << endl;
		return false;
	}

	return true;
}

bool CSBCDType2::encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len)
{
	STBCDType2 *stData = (STBCDType2 *)pos;
	double v;
	if (!frame->get<double>(ename, v))
	{
		return false;
	}
	long int value = v;
	memset(stData, 0, sizeof(STBCDType2));

	Timestamp timeStamp = Timestamp::fromEpochTime(value);
	LocalDateTime localDateTime(timeStamp);

	stData->YearTen = localDateTime.year()%100/10;
	stData->YearUni = localDateTime.year()%10;
	stData->MonTen = localDateTime.month()/10;
	stData->MonUni = localDateTime.month()%10;
	stData->DayTen = localDateTime.day()/10;
	stData->DayUni = localDateTime.day()%10;
	stData->HourTen = localDateTime.hour()/10;
	stData->HourUni = localDateTime.hour()%10;
	stData->MinTen = localDateTime.minute()/10;
	stData->MinUni = localDateTime.minute()%10;

	return true;
}

bool CSBCDType2::decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len)
{
	STBCDType2 invaild;
	memset(&invaild, '\xee', sizeof(invaild));
	
	STBCDType2 * stData = (STBCDType2 *)pos;
	if (memcmp(&invaild, stData, sizeof(invaild)) == 0)
	{
		frame->set<time_t>(ename, 0);
		return true;
	}

	Poco::LocalDateTime cur;
	try {
		Poco::LocalDateTime recur(cur.year() - cur.year()%100 + stData->YearUni + stData->YearTen * 10,
				stData->MonUni + stData->MonTen * 10,
				stData->DayUni + stData->DayTen * 10,
				stData->HourUni + stData->HourTen * 10,
				stData->MinUni + stData->MinTen * 10,
				cur.second(),
				cur.millisecond(),
				cur.microsecond()
		);
		Poco::Timestamp stamp = Timestamp::fromUtcTime(recur.utcTime());
		std::time_t tmp = stamp.epochTime();

		frame->set<time_t>(ename, tmp);
	}catch (Exception &e)
	{
		cout << e.displayText() << endl;
		cout << "decode CSBCDType2 failed!" << endl;
		return false;
	}catch (...)
	{
		cout << "decode CSBCDType2 failed!" << endl;
		return false;
	}

	return true;
}

bool CSBCDType3::encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len)
{
	STBCDType3 *stData = (STBCDType3 *)pos;
	double v;
	if (!frame->get<double>(ename, v))
	{
		return false;
	}
	long int value = v;
	memset(stData, 0, sizeof(STBCDType3));

	Timestamp timeStamp = Timestamp::fromEpochTime(value);
	LocalDateTime localDateTime(timeStamp);

	stData->DayTen = localDateTime.day()/10;
	stData->DayUni = localDateTime.day()%10;
	stData->HourTen = localDateTime.hour()/10;
	stData->HourUni = localDateTime.hour()%10;
	stData->MinTen = localDateTime.minute()/10;
	stData->MinUni = localDateTime.minute()%10;
	stData->SecTen = localDateTime.second()/10;
	stData->SecUni = localDateTime.second()%10;

	return true;
}

bool CSBCDType3::decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len)
{
	STBCDType3 invaild;
	memset(&invaild, '\xee', sizeof(invaild));
	
	STBCDType3 * stData = (STBCDType3 *)pos;
	if (memcmp(&invaild, stData, sizeof(invaild)) == 0)
	{
		frame->set<time_t>(ename, 0);
		return true;
	}

	Poco::LocalDateTime cur;
	try {
		Poco::LocalDateTime recur(cur.year(),
				cur.month(),
				stData->DayUni + stData->DayTen * 10,
				stData->HourUni + stData->HourTen * 10,
				stData->MinUni + stData->MinTen * 10,
				stData->SecUni + stData->SecTen * 10,
				cur.millisecond(),
				cur.microsecond()
		);
		Poco::Timestamp stamp = Timestamp::fromUtcTime(recur.utcTime());
		std::time_t tmp = stamp.epochTime();

		frame->set<time_t>(ename, tmp);
	}catch (Exception &e)
	{
		cout << e.displayText() << endl;
		cout << "decode CSBCDType3 failed!" << endl;
		return false;
	}catch (...)
	{
		cout << "decode CSBCDType3 failed!" << endl;
		return false;
	}

	return true;
}

bool CSBCDType4::encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len)
{
	STBCDType4 *stData = (STBCDType4 *)pos;

	double v;
	if (!frame->get<double>(ename, v))
	{
		return false;
	}
	double value = v;
	memset(stData, 0, sizeof(STBCDType4));

	value = fabs(value);

	stData->B1TenThousandth = (int)(trunc(value*10000))%10;
	stData->B1Thousanth = (int)(trunc(value*1000))%10;
	stData->B2Hundredth = (int)(trunc(value*100))%10;
	stData->B2Tenth = (int)(trunc(value*10))%10;
	stData->B3Units = (int)(trunc(value))%10;
	stData->B3Tens = (int)(trunc(value*0.1))%10;
	stData->B4Hundreds = (int)(trunc(value*0.01))%10;
	stData->B4Thousands = (int)(trunc(value*0.001))%10;
	stData->B5TenThousands = (int)(trunc(value*0.0001))%10;
	stData->B5HundredThousands = (int)(trunc(value*0.00001))%10;

	return true;
}

bool CSBCDType4::decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len)
{
	STBCDType4 invaild;
	memset(&invaild, '\xee', sizeof(invaild));
	
	STBCDType4 * stData = (STBCDType4 *)pos;
	if (memcmp(&invaild, stData, sizeof(invaild)) == 0)
	{
		frame->set<double>(ename, 0);
		return true;
	}
	
	double value =  stData->B1TenThousandth * 0.0001 +
		stData->B1Thousanth * 0.001 +
		stData->B2Hundredth * 0.01 +
		stData->B2Tenth * 0.1 +
		stData->B3Units +
		stData->B3Tens * 10 +
		stData->B4Hundreds * 100 +
		stData->B4Thousands * 1000 +
		stData->B5TenThousands * 10000 +
		stData->B5HundredThousands * 100000;

	frame->set<double>(ename, value);
	return true;
}

bool CSBCDType5::encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len)
{
	STBCDType5 *stData = (STBCDType5 *)pos;

	double v;
	if (!frame->get<double>(ename, v))
	{
		return false;
	}
	double value = v;
	memset(stData, 0, sizeof(STBCDType5));

	value = fabs(value);

	stData->B1Hundredth = (int)(trunc(value*100))%10;
	stData->B1Tenth = (int)(trunc(value*10))%10;
	stData->B2Units = (int)(trunc(value))%10;
	stData->B2Tens = (int)(trunc(value*0.1))%10;
	stData->B3Hundreds = (int)(trunc(value*0.01))%10;
	stData->B3Thousands = (int)(trunc(value*0.001))%10;
	stData->B4TenThousands = (int)(trunc(value*0.0001))%10;
	stData->B4HundredThousands = (int)(trunc(value*0.00001))%10;

	return true;
}

bool CSBCDType5::decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len)
{
	STBCDType5 invaild;
	memset(&invaild, '\xee', sizeof(invaild));
	
	STBCDType5 * stData = (STBCDType5 *)pos;
	if (memcmp(&invaild, stData, sizeof(invaild)) == 0)
	{
		frame->set<double>(ename, 0);
		return true;
	}
	
	double value = stData->B1Hundredth * 0.01 +
		stData->B1Tenth * 0.1 +
		stData->B2Units +
		stData->B2Tens * 10 +
		stData->B3Hundreds * 100 +
		stData->B3Thousands * 1000 +
		stData->B4TenThousands * 10000 +
		stData->B4HundredThousands * 100000;
	frame->set<double>(ename, value);
	return true;
}

bool CSBCDType6::encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len)
{
	STBCDType6 *stData = (STBCDType6 *)pos;

	double v;
	if (!frame->get<double>(ename, v))
	{
		return false;
	}
	double value = v;
	memset(stData, 0, sizeof(STBCDType6));

	stData->B3Sign = (value > 0) ? 0 : 1;
	value = fabs(value);
	stData->B1TenThousandth = (int)(trunc(value*10000))%10;
	stData->B1Thousandth = (int)(trunc(value*1000))%10;
	stData->B2Hundredth = (int)(trunc(value*100))%10;
	stData->B2Tenth = (int)(trunc(value*10))%10;
	stData->B3Unit = (int)(trunc(value))%10;
	stData->B3Tens = (int)(trunc(value*0.1))%10;

	return true;
}

bool CSBCDType6::decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len)
{
	STBCDType6 invaild;
	memset(&invaild, '\xee', sizeof(invaild));
	
	STBCDType6 * stData = (STBCDType6 *)pos;
	if (memcmp(&invaild, stData, sizeof(invaild)) == 0)
	{
		frame->set<double>(ename, 0);
		return true;
	}
	
	double value =  stData->B1TenThousandth * 0.0001 +
		stData->B1Thousandth * 0.001 +
		stData->B2Hundredth * 0.01 +
		stData->B2Tenth * 0.1 +
		stData->B3Unit +
		stData->B3Tens * 10;
	value = (stData->B3Sign == 0) ? value : (value * -1);
	frame->set<double>(ename, value);

	return true;
}

bool CSBCDType7::encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len)
{
	STBCDType7 *stData = (STBCDType7 *)pos;

	double v;
	if (!frame->get<double>(ename, v))
	{
		return false;
	}
	double value = v;
	memset(stData, 0, sizeof(STBCDType7));

	stData->B2Sign = (value > 0) ? 0 : 1;
	value = fabs(value);
	stData->B1Tenth = (int)(trunc(value*10))%10;
	stData->B1Unit = (int)(trunc(value))%10;
	stData->B2Tens = (int)(trunc(value*0.1))%10;
	stData->B2Hundreds = (int)(trunc(value*0.01))%10;

	return true;
}

bool CSBCDType7::decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len)
{
	STBCDType7 invaild;
	memset(&invaild, '\xee', sizeof(invaild));
	
	STBCDType7 * stData = (STBCDType7 *)pos;
	if (memcmp(&invaild, stData, sizeof(invaild)) == 0)
	{
		frame->set<double>(ename, 0);
		return true;
	}
	
	double value = stData->B1Tenth * 0.1 +
		stData->B1Unit +
		stData->B2Tens * 10 +
		stData->B2Hundreds * 100;
	value = (stData->B2Sign == 0) ? value : (value * -1);
	frame->set<double>(ename, value);

	return true;
}

bool CSBCDType8::encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len)
{
	STBCDType8 *stData = (STBCDType8 *)pos;

	double v;
	if (!frame->get<double>(ename, v))
	{
		return false;
	}
	double value = v;
	memset(stData, 0, sizeof(STBCDType8));

	value = fabs(value);
	stData->B1Tenth = (int)(trunc(value*10))%10;
	stData->B1Unit = (int)(trunc(value))%10;
	stData->B2Tens = (int)(trunc(value*0.1))%10;
	stData->B2Hundreds = (int)(trunc(value*0.01))%10;

	return true;
}

bool CSBCDType8::decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len)
{
	STBCDType8 invaild;
	memset(&invaild, '\xee', sizeof(invaild));
	
	STBCDType8 * stData = (STBCDType8 *)pos;
	if (memcmp(&invaild, stData, sizeof(invaild)) == 0)
	{
		frame->set<double>(ename, 0);
		return true;
	}
	
	double value = stData->B1Tenth * 0.1 +
		stData->B1Unit +
		stData->B2Tens * 10 +
		stData->B2Hundreds * 100;
	frame->set<double>(ename, value);

	return true;
}

bool CSBCDType9::encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len)
{
	STBCDType9 *stData = (STBCDType9 *)pos;

	double v;
	if (!frame->get<double>(ename, v))
	{
		return false;
	}
	double value = v;
	memset(stData, 0, sizeof(STBCDType9));

	stData->B3Sign = (value > 0) ? 0 : 1;
	value = fabs(value);
	stData->B1Thousandth = (int)(trunc(value*1000))%10;
	stData->B1Hundredth = (int)(trunc(value*100))%10;
	stData->B2Tenth = (int)(trunc(value*10))%10;
	stData->B2Unit = (int)(trunc(value))%10;
	stData->B3Tens = (int)(trunc(value*0.1))%10;
	stData->B3Hundreds = (int)(trunc(value*0.01))%10;

	return true;
}

bool CSBCDType9::decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len)
{
	STBCDType9 invaild;
	memset(&invaild, '\xee', sizeof(invaild));
	
	STBCDType9 * stData = (STBCDType9 *)pos;
	if (memcmp(&invaild, stData, sizeof(invaild)) == 0)
	{
		frame->set<double>(ename, 0);
		return true;
	}
	
	double value = stData->B1Thousandth * 0.001 +
		stData->B1Hundredth * 0.01 +
		stData->B2Tenth * 0.1 +
		stData->B2Unit +
		stData->B3Tens * 10 +
		stData->B3Hundreds * 100;
	value = (stData->B3Sign == 0) ? value : (value * -1);
	frame->set<double>(ename, value);

	return true;
}

bool CSBCDType10::encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len)
{
	STBCDType10 *stData = (STBCDType10 *)pos;

	double v;
	if (!frame->get<double>(ename, v))
	{
		return false;
	}
	double value = v;
	memset(stData, 0, sizeof(STBCDType10));

	value = fabs(value);
	stData->B1Hundredth = (int)(trunc(value*100))%10;
	stData->B1Tenth = (int)(trunc(value*10))%10;
	stData->B2Unit = (int)(trunc(value))%10;
	stData->B2Tens = (int)(trunc(value*0.1))%10;

	return true;
}

bool CSBCDType10::decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len)
{
	STBCDType10 invaild;
	memset(&invaild, '\xee', sizeof(invaild));
	
	STBCDType10 * stData = (STBCDType10 *)pos;
	if (memcmp(&invaild, stData, sizeof(invaild)) == 0)
	{
		frame->set<double>(ename, 0);
		return true;
	}
	
	double value = stData->B1Hundredth * 0.01 +
		stData->B1Tenth * 0.1 +
		stData->B2Unit +
		stData->B2Tens * 10;
	frame->set<double>(ename, value);

	return true;
}

bool CSBCDType11::encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len)
{
	STBCDType11 *stData = (STBCDType11 *)pos;

	double v;
	if (!frame->get<double>(ename, v))
	{
		return false;
	}
	double value = v;
	memset(stData, 0, sizeof(STBCDType11));

	stData->B4Sign = (value > 0) ? 0 : 1;

	value = fabs(value);

	int i = 0;
	for (i = 3; i >= -3; i--)
	{
		if (value * pow(10, i) < pow(10, 7))
		{
			stData->B4Power = 4 + i;
			break;
		}
	}

	if (i < -3)
	{
		cout<<"数值超限!"<<endl;
		return false;
	}
	value = value * pow(10, -(4 - stData->B4Power));

	stData->B1Unit = (int)(trunc(value))%10;
	stData->B1Tens = (int)(trunc(value*0.1))%10;
	stData->B2Hundreds = (int)(trunc(value*0.01))%10;
	stData->B2Thousands = (int)(trunc(value*0.001))%10;
	stData->B3TenThousands = (int)(trunc(value*0.0001))%10;
	stData->B3HundredThousands = (int)(trunc(value*0.00001))%10;
	stData->B4Millions = (int)(trunc(value*0.000001))%10;

	return true;
}

bool CSBCDType11::decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len)
{
	STBCDType11 invaild;
	memset(&invaild, '\xee', sizeof(invaild));
	
	STBCDType11 *stData = (STBCDType11 *)pos;
	if (memcmp(&invaild, stData, sizeof(invaild)) == 0)
	{
		frame->set<double>(ename, 0);
		return true;
	}
	
	double value = stData->B1Unit +
		stData->B1Tens * 10 +
		stData->B2Hundreds * 100 +
		stData->B2Thousands * 1000 +
		stData->B3TenThousands * 10000 +
		stData->B3HundredThousands * 100000 +
		stData->B4Millions * 1000000;
	value = (stData->B4Sign == 0) ? value : (value * -1);
	value *= pow(10, (4 - stData->B4Power));
	frame->set<double>(ename, value);

	return true;
}

bool CSBCD2::encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len)
{
	STBCD2 *stData = (STBCD2 *)pos;

	double v;
	if (!frame->get<double>(ename, v))
	{
		return false;
	}
	long long int value = v;
	memset(stData, 0, sizeof(STBCD2));

	stData->B1Unit = value%10;
	stData->B1Tens = (value/10)%10;
	stData->B2Unit = (value/100)%10;
	stData->B2Tens = (value/1000)%10;

	return true;
}

bool CSBCD2::decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len)
{
	STBCD2 invaild;
	memset(&invaild, '\xee', sizeof(invaild));
	
	STBCD2 *stData = (STBCD2 *)pos;
	if (memcmp(&invaild, stData, sizeof(invaild)) == 0)
	{
		frame->set<double>(ename, 0);
		return true;
	}
	
	double value = stData->B1Unit +
		stData->B1Tens * 10 +
		stData->B2Unit * 100 +
		stData->B2Tens * 1000;
	frame->set<double>(ename, value);

	return true;
}

bool CSBCD3::encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len)
{
	STBCD3 *stData = (STBCD3 *)pos;

	double v;
	if (!frame->get<double>(ename, v))
	{
		return false;
	}
	long long int value = v;
	memset(stData, 0, sizeof(STBCD3));

	stData->B1Unit = value%10;
	stData->B1Tens = (value/10)%10;
	stData->B2Unit = (value/100)%10;
	stData->B2Tens = (value/1000)%10;
	stData->B3Unit = (value/10000)%10;
	stData->B3Tens = (value/100000)%10;

	return true;
}

bool CSBCD3::decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len)
{
	STBCD3 invaild;
	memset(&invaild, '\xee', sizeof(invaild));
	
	STBCD3 *stData = (STBCD3 *)pos;
	if (memcmp(&invaild, stData, sizeof(invaild)) == 0)
	{
		frame->set<double>(ename, 0);
		return true;
	}

	double value = stData->B1Unit +
		stData->B1Tens * 10 +
		stData->B2Unit * 100 +
		stData->B2Tens * 1000 +
		stData->B3Unit * 10000 +
		stData->B3Tens * 100000;
	frame->set<double>(ename, value);
	return true;
}

bool CSBCD6::encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len)
{
	STBCD6 *stData = (STBCD6 *)pos;

	double v;
	if (!frame->get<double>(ename, v))
	{
		return false;
	}
	long long int value = v;
	memset(stData, 0, sizeof(STBCD6));

	stData->B1Unit = value%10;
	stData->B1Tens = (value/10)%10;
	stData->B2Unit = (value/100)%10;
	stData->B2Tens = (value/1000)%10;
	stData->B3Unit = (value/10000)%10;
	stData->B3Tens = (value/100000)%10;
	stData->B4Unit = (value/1000000)%10;
	stData->B4Tens = (value/10000000)%10;
	stData->B5Unit = (value/100000000)%10;
	stData->B5Tens = (value/1000000000)%10;
	stData->B6Unit = (value/10000000000)%10;
	stData->B6Tens = (value/100000000000)%10;

	return true;
}

bool CSBCD6::decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len)
{
	STBCD6 invaild;
	memset(&invaild, '\xee', sizeof(invaild));
	
	STBCD6 *stData = (STBCD6 *)pos;
	if (memcmp(&invaild, stData, sizeof(invaild)) == 0)
	{
		frame->set<double>(ename, 0);
		return true;
	}
	
	double value = stData->B1Unit +
		stData->B1Tens * 10 +
		stData->B2Unit * 100 +
		stData->B2Tens * 1000 +
		stData->B3Unit * 10000 +
		stData->B3Tens * 100000 +
		stData->B4Unit * 1000000 +
		stData->B4Tens * 10000000 +
		stData->B5Unit * 100000000 +
		stData->B5Tens * 1000000000 +
		stData->B6Unit * 10000000000 +
		stData->B6Tens * 100000000000;
	frame->set<double>(ename, value);
	return true;
}

bool CSBCD8::encode(FrameBase *frame, const string& ename, uint8_t *pos, int& len)
{
	STBCD8 *stData = (STBCD8 *)pos;

	double v;
	if (!frame->get<double>(ename, v))
	{
		return false;
	}
	long long int value = v;
	memset(stData, 0, sizeof(STBCD8));

	stData->B1Unit = value%10;
	stData->B1Tens = (value/10)%10;
	stData->B2Unit = (value/100)%10;
	stData->B2Tens = (value/1000)%10;
	stData->B3Unit = (value/10000)%10;
	stData->B3Tens = (value/100000)%10;
	stData->B4Unit = (value/1000000)%10;
	stData->B4Tens = (value/10000000)%10;
	stData->B5Unit = (value/100000000)%10;
	stData->B5Tens = (value/1000000000)%10;
	stData->B6Unit = (value/10000000000)%10;
	stData->B6Tens = (value/100000000000)%10;
	stData->B7Unit = (value/1000000000000)%10;
	stData->B7Tens = (value/10000000000000)%10;
	stData->B8Unit = (value/100000000000000)%10;
	stData->B8Tens = (value/1000000000000000)%10;

	return true;
}

bool CSBCD8::decode(FrameBase *frame, const string& ename, const uint8_t *pos, int& len)
{
	STBCD8 invaild;
	memset(&invaild, '\xee', sizeof(invaild));
	
	STBCD8 *stData = (STBCD8 *)pos;
	if (memcmp(&invaild, stData, sizeof(invaild)) == 0)
	{
		frame->set<double>(ename, 0);
		return true;
	}
	
	double value = stData->B1Unit +
			stData->B1Tens * 10 +
			stData->B2Unit * 100 +
			stData->B2Tens * 1000 +
			stData->B3Unit * 10000 +
			stData->B3Tens * 100000 +
			stData->B4Unit * 1000000 +
			stData->B4Tens * 10000000 +
			stData->B5Unit * 100000000 +
			stData->B5Tens * 1000000000 +
			stData->B6Unit * 10000000000 +
			stData->B6Tens * 100000000000 +
			stData->B7Unit * 1000000000000 +
			stData->B7Tens * 10000000000000 +
			stData->B8Unit * 100000000000000 +
			stData->B8Tens * 1000000000000000;
	frame->set<double>(ename, value);
	return true;
}
