#pragma once

#define MAX_AFNS 17
#define MAX_FNS 248

enum States {
	Begin = 0,

	EncodeFixedPart,
	EncodeVerFramePnfns,
	EncodeUserDataBlock,
	EncodeAFnFn,
	EncodeFinish,

	DecodeFixedPart,
	DecodeVerFramePnfns,
	DecodeUserDataBlock,
	DecodeAFnFn,
	DecodeFinish,

	BadFrame,

	Finish
};
