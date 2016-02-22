/*
 * ParseFrame.h
 *
 *  Created on: 2014-12-23
 *      Author: root
 */

#ifndef PARSEFRAME_H_
#define PARSEFRAME_H_

#include "FrameNx.h"
#include "FSMEncapsulate.h"
#include "TableFile.h"

using Uarc::Protocol::NX::TableFile;

bool loadFrame(FrameBase &frame, const char *file);

//modify address
bool modifyAreaCode(FrameBase &frame, int areaCode);
bool modifyTermAddr(FrameBase &frame, int termAddr);
bool modifyMasterAddr(FrameBase &frame, int masterAddr);
bool modifyGroup(FrameBase &frame, bool Group);

//modify control
bool modifyFunCode(FrameBase &frame, int funCode);
bool modifyRseq(FrameBase &frame, int Rseq);
bool modifyPseq(FrameBase &frame, int Pseq);
bool modifyReserve(FrameBase &frame, bool Reserve);
bool modifyPrm(FrameBase &frame, bool Prm);
bool modifyDir(FrameBase &frame, bool Dir);

//protocol
bool modifyProtocolVersion(FrameBase &frame, int protocolVersion);
bool modifyProtocolFlag(FrameBase &frame, int protocolFlag);
bool modifyProtocolReserve(FrameBase &frame, int protocolReserve);

//modify appControl
bool modifyAcAfn(FrameBase &frame, int acAfn);
bool modifyAcAcd(FrameBase &frame, bool acAcd);
bool modifyAcTpv(FrameBase &frame, bool acTpv);

//modify EventCount
bool modifyEc1(FrameBase &frame, int ec1);
bool modifyEc2(FrameBase &frame, int ec2);

//Password
bool modifyPassword(FrameBase &frame, string password);

//Tpv
bool modifyTpv(FrameBase &frame, int timeout);

#endif /* PARSEFRAME_H_ */
