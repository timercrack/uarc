#pragma once

#include <stdint.h> 
#include <vector>
#include <map>

using namespace Poco;
using namespace Poco::Dynamic;

using namespace std;

#define MAX_FRAME_LENGTH 1026 * 16
#define FIXED_FN_CREATE_CONNECT	1
#define FIXED_FN_DELETE_CONNECT	2
#define FIXED_FN_KEEP_CONNECT	3

#define VER_AFN_CONFIRM_OR_DENY 0
#define VER_AFN_REALTIME_DATA	13

#pragma pack(1)

struct STBCDType1 {
	union {
		struct {
			uint8_t SecUni: 4;
			uint8_t SecTen: 4;
		};
		uint8_t ByteSec;
	};

	union {
		struct {
			uint8_t MinUni: 4;
			uint8_t MinTen: 4;
		};
		uint8_t ByteMin;
	};

	union {
		struct {
			uint8_t HourUni: 4;
			uint8_t HourTen: 4;
		};
		uint8_t ByteHour;
	};

	union {
		struct {
			uint8_t DayUni: 4;
			uint8_t DayTen: 4;
		};
		uint8_t ByteDay;
	};

	union {
		struct {
			uint8_t WeekMon_Mon_Uni: 4;
			uint8_t WeekMon_Mon_Ten: 1;
			uint8_t WeekMon_Week: 3;
		};
		uint8_t ByteWeekMon;
	};

	union {
		struct {
			uint8_t YearUni: 4;
			uint8_t YearTen: 4;
		};
		uint8_t ByteYear;
	};
};

struct STBCDType2 {
	union {
		struct {
			uint8_t MinUni: 4;
			uint8_t MinTen: 4;
		};
		uint8_t ByteMin;
	};

	union {
		struct {
			uint8_t HourUni: 4;
			uint8_t HourTen: 4;
		};
		uint8_t ByteHour;
	};

	union {
		struct {
			uint8_t DayUni: 4;
			uint8_t DayTen: 4;
		};
		uint8_t ByteDay;
	};

	union {
		struct {
			uint8_t MonUni: 4;
			uint8_t MonTen: 4;
		};
		uint8_t ByteMon;
	};

	union {
		struct {
			uint8_t YearUni: 4;
			uint8_t YearTen: 4;
		};
		uint8_t ByteYear;
	};
};

struct STBCDType3 {
	union {
		struct {
			uint8_t SecUni: 4;
			uint8_t SecTen: 4;
		};
		uint8_t ByteSec;
	};

	union {
		struct {
			uint8_t MinUni: 4;
			uint8_t MinTen: 4;
		};
		uint8_t ByteMin;
	};

	union {
		struct {
			uint8_t HourUni: 4;
			uint8_t HourTen: 4;
		};
		uint8_t ByteHour;
	};

	union {
		struct {
			uint8_t DayUni: 4;
			uint8_t DayTen: 4;
		};
		uint8_t ByteDay;
	};
};

struct STBCDType4 {
	union {
		struct {
			uint8_t B1TenThousandth: 4;
			uint8_t B1Thousanth: 4;
		};
		uint8_t Byte1;
	};

	union {
		struct {
			uint8_t B2Hundredth: 4;
			uint8_t B2Tenth: 4;
		};
		uint8_t Byte2;
	};

	union {
		struct {
			uint8_t B3Units: 4;
			uint8_t B3Tens: 4;
		};
		uint8_t Byte3;
	};

	union {
		struct {
			uint8_t B4Hundreds: 4;
			uint8_t B4Thousands: 4;
		};
		uint8_t Byte4;
	};

	union {
		struct {
			uint8_t B5TenThousands: 4;
			uint8_t B5HundredThousands: 4;
		};
		uint8_t Byte5;
	};
};

struct STBCDType5 {
	union {
		struct {
			uint8_t B1Hundredth: 4;
			uint8_t B1Tenth: 4;
		};
		uint8_t Byte1;
	};

	union {
		struct {
			uint8_t B2Units: 4;
			uint8_t B2Tens: 4;
		};
		uint8_t Byte2;
	};

	union {
		struct {
			uint8_t B3Hundreds: 4;
			uint8_t B3Thousands: 4;
		};
		uint8_t Byte3;
	};

	union {
		struct {
			uint8_t B4TenThousands: 4;
			uint8_t B4HundredThousands: 4;
		};
		uint8_t Byte4;
	};
};

struct STBCDType6 {
	union {
		struct {
			uint8_t B1TenThousandth: 4;
			uint8_t B1Thousandth: 4;
		};
		uint8_t Byte1;
	};

	union {
		struct {
			uint8_t B2Hundredth: 4;
			uint8_t B2Tenth: 4;
		};
		uint8_t Byte2;
	};

	union {
		struct {
			uint8_t B3Unit: 4;
			uint8_t B3Tens: 3;
			uint8_t B3Sign: 1;
		};
		uint8_t Byte3;
	};
};

struct STBCDType7 {
	union {
		struct {
			uint8_t B1Tenth: 4;
			uint8_t B1Unit: 4;
		};
		uint8_t Byte1;
	};

	union {
		struct {
			uint8_t B2Tens: 4;
			uint8_t B2Hundreds: 3;
			uint8_t B2Sign: 1;
		};
		uint8_t Byte2;
	};
};

struct STBCDType8 {
	union {
		struct {
			uint8_t B1Tenth: 4;
			uint8_t B1Unit: 4;
		};
		uint8_t Byte1;
	};

	union {
		struct {
			uint8_t B2Tens: 4;
			uint8_t B2Hundreds: 4;
		};
		uint8_t Byte2;
	};
};

struct STBCDType9 {
	union {
		struct {
			uint8_t B1Thousandth: 4;
			uint8_t B1Hundredth: 4;
		};
		uint8_t Byte1;
	};

	union {
		struct {
			uint8_t B2Tenth: 4;
			uint8_t B2Unit: 4;
		};
		uint8_t Byte2;
	};

	union {
		struct {
			uint8_t B3Tens: 4;
			uint8_t B3Hundreds: 3;
			uint8_t B3Sign: 1;
		};
		uint8_t Byte3;
	};
};

struct STBCDType10 {
	union {
		struct {
			uint8_t B1Hundredth: 4;
			uint8_t B1Tenth: 4;
		};
		uint8_t Byte1;
	};

	union {
		struct {
			uint8_t B2Unit: 4;
			uint8_t B2Tens: 4;
		};
		uint8_t Byte2;
	};
};

struct STBCDType11 {
	union {
		struct {
			uint8_t B1Unit: 4;
			uint8_t B1Tens: 4;
		};
		uint8_t Byte1;
	};

	union {
		struct {
			uint8_t B2Hundreds: 4;
			uint8_t B2Thousands: 4;
		};
		uint8_t Byte2;
	};

	union {
		struct {
			uint8_t B3TenThousands: 4;
			uint8_t B3HundredThousands: 4;
		};
		uint8_t Byte3;
	};

	union {
		struct {
			uint8_t B4Millions: 4;
			uint8_t B4Sign: 1;
			uint8_t B4Power: 3;
		};
		uint8_t Byte4;
	};
};

struct STBCD2 {
	union {
		struct {
			uint8_t B1Unit: 4;
			uint8_t B1Tens: 4;
		};
		uint8_t Byte1;
	};

	union {
		struct {
			uint8_t B2Unit: 4;
			uint8_t B2Tens: 4;
		};
		uint8_t Byte2;
	};
};

struct STBCD3 {
	union {
		struct {
			uint8_t B1Unit: 4;
			uint8_t B1Tens: 4;
		};
		uint8_t Byte1;
	};

	union {
		struct {
			uint8_t B2Unit: 4;
			uint8_t B2Tens: 4;
		};
		uint8_t Byte2;
	};

	union {
		struct {
			uint8_t B3Unit: 4;
			uint8_t B3Tens: 4;
		};
		uint8_t Byte3;
	};
};

struct STBCD6 {
	union {
		struct {
			uint8_t B1Unit: 4;
			uint8_t B1Tens: 4;
		};
		uint8_t Byte1;
	};

	union {
		struct {
			uint8_t B2Unit: 4;
			uint8_t B2Tens: 4;
		};
		uint8_t Byte2;
	};
	
	union {
		struct {
			uint8_t B3Unit: 4;
			uint8_t B3Tens: 4;
		};
		uint8_t Byte3;
	};

	union {
		struct {
			uint8_t B4Unit: 4;
			uint8_t B4Tens: 4;
		};
		uint8_t Byte4;
	};

	union {
		struct {
			uint8_t B5Unit: 4;
			uint8_t B5Tens: 4;
		};
		uint8_t Byte5;
	};

	union {
		struct {
			uint8_t B6Unit: 4;
			uint8_t B6Tens: 4;
		};
		uint8_t Byte6;
	};
};

struct STBCD8 {
	union {
		struct {
			uint8_t B1Unit: 4;
			uint8_t B1Tens: 4;
		};
		uint8_t Byte1;
	};

	union {
		struct {
			uint8_t B2Unit: 4;
			uint8_t B2Tens: 4;
		};
		uint8_t Byte2;
	};
	
	union {
		struct {
			uint8_t B3Unit: 4;
			uint8_t B3Tens: 4;
		};
		uint8_t Byte3;
	};

	union {
		struct {
			uint8_t B4Unit: 4;
			uint8_t B4Tens: 4;
		};
		uint8_t Byte4;
	};

	union {
		struct {
			uint8_t B5Unit: 4;
			uint8_t B5Tens: 4;
		};
		uint8_t Byte5;
	};

	union {
		struct {
			uint8_t B6Unit: 4;
			uint8_t B6Tens: 4;
		};
		uint8_t Byte6;
	};

	union {
		struct {
			uint8_t B7Unit: 4;
			uint8_t B7Tens: 4;
		};
		uint8_t Byte7;
	};

	union {
		struct {
			uint8_t B8Unit: 4;
			uint8_t B8Tens: 4;
		};
		uint8_t Byte8;
	};
};

struct STBS8 {
	union {
		struct {
			uint8_t BSBit1: 1;
			uint8_t BSBit2: 1;
			uint8_t BSBit3: 1;
			uint8_t BSBit4: 1;
			uint8_t BSBit5: 1;
			uint8_t BSBit6: 1;
			uint8_t BSBit7: 1;
			uint8_t BSBit8: 1;
		};
		uint8_t Byte;
	};
};

struct STBS248 {
	union {
		struct {
			uint8_t BSBit1: 1;
			uint8_t BSBit2: 1;
			uint8_t BSBit3: 1;
			uint8_t BSBit4: 1;
			uint8_t BSBit5: 1;
			uint8_t BSBit6: 1;
			uint8_t BSBit7: 1;
			uint8_t BSBit8: 1;
			uint8_t BSBit9: 1;
			uint8_t BSBit10: 1;
			uint8_t BSBit11: 1;
			uint8_t BSBit12: 1;
			uint8_t BSBit13: 1;
			uint8_t BSBit14: 1;
			uint8_t BSBit15: 1;
			uint8_t BSBit16: 1;
			uint8_t BSBit17: 1;
			uint8_t BSBit18: 1;
			uint8_t BSBit19: 1;
			uint8_t BSBit20: 1;
			uint8_t BSBit21: 1;
			uint8_t BSBit22: 1;
			uint8_t BSBit23: 1;
			uint8_t BSBit24: 1;
			uint8_t BSBit25: 1;
			uint8_t BSBit26: 1;
			uint8_t BSBit27: 1;
			uint8_t BSBit28: 1;
			uint8_t BSBit29: 1;
			uint8_t BSBit30: 1;
			uint8_t BSBit31: 1;
			uint8_t BSBit32: 1;
			uint8_t BSBit33: 1;
			uint8_t BSBit34: 1;
			uint8_t BSBit35: 1;
			uint8_t BSBit36: 1;
			uint8_t BSBit37: 1;
			uint8_t BSBit38: 1;
			uint8_t BSBit39: 1;
			uint8_t BSBit40: 1;
			uint8_t BSBit41: 1;
			uint8_t BSBit42: 1;
			uint8_t BSBit43: 1;
			uint8_t BSBit44: 1;
			uint8_t BSBit45: 1;
			uint8_t BSBit46: 1;
			uint8_t BSBit47: 1;
			uint8_t BSBit48: 1;
			uint8_t BSBit49: 1;
			uint8_t BSBit50: 1;
			uint8_t BSBit51: 1;
			uint8_t BSBit52: 1;
			uint8_t BSBit53: 1;
			uint8_t BSBit54: 1;
			uint8_t BSBit55: 1;
			uint8_t BSBit56: 1;
			uint8_t BSBit57: 1;
			uint8_t BSBit58: 1;
			uint8_t BSBit59: 1;
			uint8_t BSBit60: 1;
			uint8_t BSBit61: 1;
			uint8_t BSBit62: 1;
			uint8_t BSBit63: 1;
			uint8_t BSBit64: 1;
			uint8_t BSBit65: 1;
			uint8_t BSBit66: 1;
			uint8_t BSBit67: 1;
			uint8_t BSBit68: 1;
			uint8_t BSBit69: 1;
			uint8_t BSBit70: 1;
			uint8_t BSBit71: 1;
			uint8_t BSBit72: 1;
			uint8_t BSBit73: 1;
			uint8_t BSBit74: 1;
			uint8_t BSBit75: 1;
			uint8_t BSBit76: 1;
			uint8_t BSBit77: 1;
			uint8_t BSBit78: 1;
			uint8_t BSBit79: 1;
			uint8_t BSBit80: 1;
			uint8_t BSBit81: 1;
			uint8_t BSBit82: 1;
			uint8_t BSBit83: 1;
			uint8_t BSBit84: 1;
			uint8_t BSBit85: 1;
			uint8_t BSBit86: 1;
			uint8_t BSBit87: 1;
			uint8_t BSBit88: 1;
			uint8_t BSBit89: 1;
			uint8_t BSBit90: 1;
			uint8_t BSBit91: 1;
			uint8_t BSBit92: 1;
			uint8_t BSBit93: 1;
			uint8_t BSBit94: 1;
			uint8_t BSBit95: 1;
			uint8_t BSBit96: 1;
			uint8_t BSBit97: 1;
			uint8_t BSBit98: 1;
			uint8_t BSBit99: 1;
			uint8_t BSBit100: 1;
			uint8_t BSBit101: 1;
			uint8_t BSBit102: 1;
			uint8_t BSBit103: 1;
			uint8_t BSBit104: 1;
			uint8_t BSBit105: 1;
			uint8_t BSBit106: 1;
			uint8_t BSBit107: 1;
			uint8_t BSBit108: 1;
			uint8_t BSBit109: 1;
			uint8_t BSBit110: 1;
			uint8_t BSBit111: 1;
			uint8_t BSBit112: 1;
			uint8_t BSBit113: 1;
			uint8_t BSBit114: 1;
			uint8_t BSBit115: 1;
			uint8_t BSBit116: 1;
			uint8_t BSBit117: 1;
			uint8_t BSBit118: 1;
			uint8_t BSBit119: 1;
			uint8_t BSBit120: 1;
			uint8_t BSBit121: 1;
			uint8_t BSBit122: 1;
			uint8_t BSBit123: 1;
			uint8_t BSBit124: 1;
			uint8_t BSBit125: 1;
			uint8_t BSBit126: 1;
			uint8_t BSBit127: 1;
			uint8_t BSBit128: 1;
			uint8_t BSBit129: 1;
			uint8_t BSBit130: 1;
			uint8_t BSBit131: 1;
			uint8_t BSBit132: 1;
			uint8_t BSBit133: 1;
			uint8_t BSBit134: 1;
			uint8_t BSBit135: 1;
			uint8_t BSBit136: 1;
			uint8_t BSBit137: 1;
			uint8_t BSBit138: 1;
			uint8_t BSBit139: 1;
			uint8_t BSBit140: 1;
			uint8_t BSBit141: 1;
			uint8_t BSBit142: 1;
			uint8_t BSBit143: 1;
			uint8_t BSBit144: 1;
			uint8_t BSBit145: 1;
			uint8_t BSBit146: 1;
			uint8_t BSBit147: 1;
			uint8_t BSBit148: 1;
			uint8_t BSBit149: 1;
			uint8_t BSBit150: 1;
			uint8_t BSBit151: 1;
			uint8_t BSBit152: 1;
			uint8_t BSBit153: 1;
			uint8_t BSBit154: 1;
			uint8_t BSBit155: 1;
			uint8_t BSBit156: 1;
			uint8_t BSBit157: 1;
			uint8_t BSBit158: 1;
			uint8_t BSBit159: 1;
			uint8_t BSBit160: 1;
			uint8_t BSBit161: 1;
			uint8_t BSBit162: 1;
			uint8_t BSBit163: 1;
			uint8_t BSBit164: 1;
			uint8_t BSBit165: 1;
			uint8_t BSBit166: 1;
			uint8_t BSBit167: 1;
			uint8_t BSBit168: 1;
			uint8_t BSBit169: 1;
			uint8_t BSBit170: 1;
			uint8_t BSBit171: 1;
			uint8_t BSBit172: 1;
			uint8_t BSBit173: 1;
			uint8_t BSBit174: 1;
			uint8_t BSBit175: 1;
			uint8_t BSBit176: 1;
			uint8_t BSBit177: 1;
			uint8_t BSBit178: 1;
			uint8_t BSBit179: 1;
			uint8_t BSBit180: 1;
			uint8_t BSBit181: 1;
			uint8_t BSBit182: 1;
			uint8_t BSBit183: 1;
			uint8_t BSBit184: 1;
			uint8_t BSBit185: 1;
			uint8_t BSBit186: 1;
			uint8_t BSBit187: 1;
			uint8_t BSBit188: 1;
			uint8_t BSBit189: 1;
			uint8_t BSBit190: 1;
			uint8_t BSBit191: 1;
			uint8_t BSBit192: 1;
			uint8_t BSBit193: 1;
			uint8_t BSBit194: 1;
			uint8_t BSBit195: 1;
			uint8_t BSBit196: 1;
			uint8_t BSBit197: 1;
			uint8_t BSBit198: 1;
			uint8_t BSBit199: 1;
			uint8_t BSBit200: 1;
			uint8_t BSBit201: 1;
			uint8_t BSBit202: 1;
			uint8_t BSBit203: 1;
			uint8_t BSBit204: 1;
			uint8_t BSBit205: 1;
			uint8_t BSBit206: 1;
			uint8_t BSBit207: 1;
			uint8_t BSBit208: 1;
			uint8_t BSBit209: 1;
			uint8_t BSBit210: 1;
			uint8_t BSBit211: 1;
			uint8_t BSBit212: 1;
			uint8_t BSBit213: 1;
			uint8_t BSBit214: 1;
			uint8_t BSBit215: 1;
			uint8_t BSBit216: 1;
			uint8_t BSBit217: 1;
			uint8_t BSBit218: 1;
			uint8_t BSBit219: 1;
			uint8_t BSBit220: 1;
			uint8_t BSBit221: 1;
			uint8_t BSBit222: 1;
			uint8_t BSBit223: 1;
			uint8_t BSBit224: 1;
			uint8_t BSBit225: 1;
			uint8_t BSBit226: 1;
			uint8_t BSBit227: 1;
			uint8_t BSBit228: 1;
			uint8_t BSBit229: 1;
			uint8_t BSBit230: 1;
			uint8_t BSBit231: 1;
			uint8_t BSBit232: 1;
			uint8_t BSBit233: 1;
			uint8_t BSBit234: 1;
			uint8_t BSBit235: 1;
			uint8_t BSBit236: 1;
			uint8_t BSBit237: 1;
			uint8_t BSBit238: 1;
			uint8_t BSBit239: 1;
			uint8_t BSBit240: 1;
			uint8_t BSBit241: 1;
			uint8_t BSBit242: 1;
			uint8_t BSBit243: 1;
			uint8_t BSBit244: 1;
			uint8_t BSBit245: 1;
			uint8_t BSBit246: 1;
			uint8_t BSBit247: 1;
			uint8_t BSBit248: 1;
		};
		uint8_t BS[31];
	};
};

struct STBS64 {
	union {
		struct {
			uint8_t BSBit1: 1;
			uint8_t BSBit2: 1;
			uint8_t BSBit3: 1;
			uint8_t BSBit4: 1;
			uint8_t BSBit5: 1;
			uint8_t BSBit6: 1;
			uint8_t BSBit7: 1;
			uint8_t BSBit8: 1;
			uint8_t BSBit9: 1;
			uint8_t BSBit10: 1;
			uint8_t BSBit11: 1;
			uint8_t BSBit12: 1;
			uint8_t BSBit13: 1;
			uint8_t BSBit14: 1;
			uint8_t BSBit15: 1;
			uint8_t BSBit16: 1;
			uint8_t BSBit17: 1;
			uint8_t BSBit18: 1;
			uint8_t BSBit19: 1;
			uint8_t BSBit20: 1;
			uint8_t BSBit21: 1;
			uint8_t BSBit22: 1;
			uint8_t BSBit23: 1;
			uint8_t BSBit24: 1;
			uint8_t BSBit25: 1;
			uint8_t BSBit26: 1;
			uint8_t BSBit27: 1;
			uint8_t BSBit28: 1;
			uint8_t BSBit29: 1;
			uint8_t BSBit30: 1;
			uint8_t BSBit31: 1;
			uint8_t BSBit32: 1;
			uint8_t BSBit33: 1;
			uint8_t BSBit34: 1;
			uint8_t BSBit35: 1;
			uint8_t BSBit36: 1;
			uint8_t BSBit37: 1;
			uint8_t BSBit38: 1;
			uint8_t BSBit39: 1;
			uint8_t BSBit40: 1;
			uint8_t BSBit41: 1;
			uint8_t BSBit42: 1;
			uint8_t BSBit43: 1;
			uint8_t BSBit44: 1;
			uint8_t BSBit45: 1;
			uint8_t BSBit46: 1;
			uint8_t BSBit47: 1;
			uint8_t BSBit48: 1;
			uint8_t BSBit49: 1;
			uint8_t BSBit50: 1;
			uint8_t BSBit51: 1;
			uint8_t BSBit52: 1;
			uint8_t BSBit53: 1;
			uint8_t BSBit54: 1;
			uint8_t BSBit55: 1;
			uint8_t BSBit56: 1;
			uint8_t BSBit57: 1;
			uint8_t BSBit58: 1;
			uint8_t BSBit59: 1;
			uint8_t BSBit60: 1;
			uint8_t BSBit61: 1;
			uint8_t BSBit62: 1;
			uint8_t BSBit63: 1;
			uint8_t BSBit64: 1;
		};
		uint8_t BS[8];
	};
};

struct STSC8 {
	union {
		struct {
			uint8_t STBit1: 1;
			uint8_t STBit2: 1;
			uint8_t STBit3: 1;
			uint8_t STBit4: 1;
			uint8_t STBit5: 1;
			uint8_t STBit6: 1;
			uint8_t STBit7: 1;
			uint8_t STBit8: 1;
		};
		uint8_t Status;
	};

	union {
		struct {
			uint8_t CDBit1: 1;
			uint8_t CDBit2: 1;
			uint8_t CDBit3: 1;
			uint8_t CDBit4: 1;
			uint8_t CDBit5: 1;
			uint8_t CDBit6: 1;
			uint8_t CDBit7: 1;
			uint8_t CDBit8: 1;
		};
	uint8_t Changed;
	};
};

struct STIPV4 {
	uint8_t IPVersion;
	uint8_t IPSeg1;
	uint8_t IPSeg2;
	uint8_t IPSeg3;
	uint8_t IPSeg4;
	//void decodeIP();
};

struct STIPV6 {
	uint8_t IPVersion;
	uint8_t IPSeg1;
	uint8_t IPSeg2;
	uint8_t IPSeg3;
	uint8_t IPSeg4;
	uint8_t IPSeg5;
	uint8_t IPSeg6;
	//void decodeIP();
};

struct STIP {
	uint8_t IPVersion;
	union {
		STIPV4 IPV4;
		STIPV6 IPV6;
	};
};

struct STCOMM {
	union {
		struct {
			uint8_t STDataBit2 		: 2;
			uint8_t STCheckBit1 	: 1;
			uint8_t STHasCheckBit1 	: 1;
			uint8_t STStopBit1		: 1;
			uint8_t STBaudRateBit3 	: 3;
		};
		uint8_t value;
	};
};

struct STTuple {
	uint8_t DA1;
	uint8_t DA2;

	//type_flag--------0:fn, 1:pn
	void encode(vector<int>& values, int type_flag) {
		if (values.size() == 1 && values[0] == 0)
		{
			DA1 = 0;
			DA2 = 0;
			return ;
		}else if (values.size() == 1 && values[0] == -1)
		{
			DA1 = 0xff;
			DA2 = 0;
			return ;
		}

		DA1 = 0;
		DA2 = (values[0]-1)/8+type_flag;
		for (int i = 0; i < (int)values.size(); ++i)
		{
			DA1 |= 0x1 << (values[i]-1)%8;
		}
	}
	void decode(vector<int>& values, int type_flag) {
		if ((DA1 == 0) && (DA2 == 0)) 
		{   
			values.push_back(0); //表示信息集中与交互终端信息点，用p0表示;
			return;
		}   
		else if ( (DA1 == 0xff) && (DA2 == 0)) 
		{   
			values.push_back(-1); //表示所有有效信息点pn（不含p0）;
			return;
		}   

		int base = (DA2 - type_flag) * 8;
		int tmp = 1;
		for(int i = 0 ;i < 8; i++){
			if((tmp & DA1) != 0){ 
				values.push_back(base + i + 1); 
			}   
			tmp <<= 1;
		}
	}

};

struct STPnFnS {
	STTuple Pn;
	STTuple Fn;
};

struct STControl {
	union {
		struct {
			union {
				struct{
					uint8_t Control_D0_D3_func :4;
					uint8_t Control_D4_D7_resq :4;
				};
				uint8_t Byte1;
			};

			union {
				struct {
					uint8_t Control_D8_D12_pesq :5;
					uint8_t Control_D13_reserve :1;
					uint8_t Control_D14_prm :1;
					uint8_t Control_D15_dir :1;
				};
				uint8_t Byte2;
			};
		};
		uint16_t ControlData;
	};
};

struct STAddress {
	union { /* BCD format */
		struct {
			union {
				struct {
					uint8_t A1B1Uni: 4;
					uint8_t A1B1Ten: 4;
				};
				uint8_t A1Byte1;
			};
			union {
				struct {
					uint8_t A1B2Uni: 4;
					uint8_t A1B2Ten: 4;
				};
				uint8_t A1Byte2;
			};
			union {
				struct {
					uint8_t A1B3Uni: 4;
					uint8_t A1B3Ten: 4;
				};
				uint8_t A1Byte3;
			};
		};

		uint8_t addr1[3];
	};

	union {
		struct {
			uint8_t A2Byte1;
			uint8_t A2Byte2;
			uint8_t A2Byte3;
			uint8_t A2Byte4;
		};
		uint32_t addr2;
	};

	uint8_t Addr3;
};

struct STFixedFrameData {
	/* The begin area */
	uint8_t FrameBegin;

	/* The control area */
	STControl cdata;

	/* The Address area */
	STAddress adata;

	/*The user data;
	 *The protocal type area;
	 */
	union {
		struct {
			uint8_t protocolType_D0_D5_version :6;
			uint8_t protocolType_D6_flag :1;
			uint8_t protocolType_D7_reserve :1;
		};
		uint8_t protocolType;
	};

	/* The CS */
	uint8_t checksum;

	/* The tail */
	uint8_t tail;
};

struct STVerFrameData {
	/* The first begin area */
	uint8_t FirstBegin;

	/* The first length area */
	union {
		struct {
			uint8_t FirstLengthByte1;
			uint8_t FirstLengthByte2;
		};
		uint16_t FirstLength;
	};

	/* The second length area */
	union{
		struct {
			uint8_t SecondLengthByte1;
			uint8_t SecondLengthByte2;
		};
		uint16_t SecondLength;
	};

	/* The second begin area */
	uint8_t SecondBegin;

	/* The control area */
	STControl cdata;

	/* The Address area */
	STAddress adata;

	/* The protocal type area */
	union {
		struct {
			uint8_t protocolType_D0_D5_version :6;
			uint8_t protocolType_D6_flag :1;
			uint8_t protocolType_D7_reserve :1;
		};
		uint8_t protocolType;
	};

	/* The application control area */
	union {
		struct {
			uint8_t appControl_D0_D4_afn :5;
			uint8_t appControl_D5_reserve :1;
			uint8_t appControl_D6_acd :1;
			uint8_t appControl_D7_tpv :1;
		};
		uint8_t appControl;
	};

	/* The pointer of remaining data in the buffer. */
	uint8_t content[0];
};

struct STFrameData {
	union {
		STFixedFrameData FixedFrame;
		STVerFrameData VerFrame;
		uint8_t buf[MAX_FRAME_LENGTH];
	};
};
#pragma pack()
