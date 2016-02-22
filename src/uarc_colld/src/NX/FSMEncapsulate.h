#pragma once

#include <stdint.h>
#include "FSM.hpp"
#include "FSMStates.h"
#include "FrameNx.h"


struct IEvents {
	virtual void decodeBegin(uint8_t *fdata, int &length) {}
	virtual void encodeBegin(FrameNx &frame) {}
	virtual void decode() {}
	virtual void encode() {}
//	virtual void send() {}
	virtual void decodeEnd(FrameNx &frame) {}
	virtual void encodeEnd(uint8_t *fdata, int &length) {}
	virtual bool process() {return true;}
};

//typedef SMC<States, IEvents, Begin, Finish, FrameData>ProcessFrame_Def;
typedef SMC<States, IEvents, Begin, Finish, FrameNx> ProcessFrameNx;
typedef FSM<ProcessFrameNx> FrameNxFSM;

BEGIN_TRANSITION_IMPL(ProcessFrameNx, Begin)
	virtual void encodeBegin(FrameNx &frame);
	virtual void decodeBegin(uint8_t *fdata, int &length);
END_TRANSITION_IMPL

BEGIN_TRANSITION_IMPL(ProcessFrameNx, BadFrame)
	virtual bool process();
END_TRANSITION_IMPL

BEGIN_TRANSITION_IMPL(ProcessFrameNx, EncodeFinish)
	virtual bool process();
	virtual void encodeEnd(uint8_t *fdata, int &length);
END_TRANSITION_IMPL

BEGIN_TRANSITION_IMPL(ProcessFrameNx, EncodeFixedPart)
	virtual void encode();
END_TRANSITION_IMPL

BEGIN_TRANSITION_IMPL(ProcessFrameNx, EncodeVerFramePnfns)
	virtual void encode();
END_TRANSITION_IMPL

BEGIN_TRANSITION_IMPL(ProcessFrameNx, EncodeUserDataBlock)
	virtual void encode();
END_TRANSITION_IMPL

BEGIN_TRANSITION_IMPL(ProcessFrameNx, EncodeAFnFn)
	virtual void encode();
END_TRANSITION_IMPL



BEGIN_TRANSITION_IMPL(ProcessFrameNx, DecodeFinish)
	virtual bool process();
	virtual void decodeEnd(FrameNx &frame);
END_TRANSITION_IMPL

BEGIN_TRANSITION_IMPL(ProcessFrameNx, DecodeFixedPart)
	virtual void decode();
END_TRANSITION_IMPL

BEGIN_TRANSITION_IMPL(ProcessFrameNx, DecodeVerFramePnfns)
	virtual void decode();
END_TRANSITION_IMPL

BEGIN_TRANSITION_IMPL(ProcessFrameNx, DecodeUserDataBlock)
	virtual void decode();
END_TRANSITION_IMPL

BEGIN_TRANSITION_IMPL(ProcessFrameNx, DecodeAFnFn)
	virtual void decode();
END_TRANSITION_IMPL
