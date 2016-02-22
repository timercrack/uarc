/*
 * ByteDataBuffer.cpp
 *
 *  Created on: 2014-10-14
 *      Author: root
 */

#include <Poco/Util/Application.h>
#include "ByteDataBuffer.h"
#include <stdlib.h>
#include <math.h>

namespace Uarc {
namespace Protocol {

ByteDataBuffer::ByteDataBuffer(char *buffer, int bufferSize) {
	this->isAlloc=0;
	this->start=(uint8_t *)buffer;
	this->tail=(uint8_t *)(buffer);
	this->pos=0;
	this->size=0;
	this->nullSign=false;
	this->length=bufferSize;
}
ByteDataBuffer::ByteDataBuffer(int bufferSize) {
	this->isAlloc=1;
	this->start=(uint8_t *)malloc(bufferSize);
	if (start==NULL) {
		Poco::Util::Application::instance().logger().error("\n内存益处,请求内存大小失败 %d",bufferSize);
//		FES_LOGGER(ERROR,"\n内存益处,请求内存大小失败 %d",bufferSize);
		exit(1);
	};
	this->tail=start;
	this->pos=0;
	this->size=0;
	this->nullSign=false;
	this->length=bufferSize;
}

ByteDataBuffer::~ByteDataBuffer() {
	if (this->isAlloc) {
		free(this->start);
	}
}

int ByteDataBuffer::writeInt8(uint8_t data) {
	*(this->tail)=data;
	this->tail++;
	this->pos++;
	return 1;
}
int ByteDataBuffer::writeInt16(uint16_t data) {
	*this->tail=(data&0xff);
	this->tail++;
	*this->tail=((data>>8)&0xff);
	this->tail++;
	this->pos+=2;
	return 2;
}
int ByteDataBuffer::writeInt32(uint32_t data) {
	*this->tail=(data&0xff);
	this->tail++;
	*this->tail=((data>>8)&0xff);
	this->tail++;
	*this->tail=((data>>16)&0xff);
	this->tail++;
	*this->tail=((data>>24)&0xff);
	this->tail++;
	this->pos +=4;
	return 4;
}
uint8_t ByteDataBuffer::readInt8() {
	uint8_t data=*(this->tail);
	this->tail++;
	this->pos++;
	return data;
}
uint16_t ByteDataBuffer::readInt16() {

	uint16_t data= *this->tail;
	this->tail++;
	data+= *(this->tail) <<8;
	this->tail++;
	this->pos+=2;
	return data;
}
uint32_t ByteDataBuffer::readInt32() {
	uint32_t data= *this->tail;
	this->tail++;
	data+= *(this->tail) <<8;

	this->tail++;
	data+= *(this->tail) <<16;
	this->tail++;
	data+= *(this->tail) <<24;
	this->tail++;

	this->pos+=4;
	return data;
}
int ByteDataBuffer::seek(int pos) {
	this->tail= this->start+pos;
	this->pos=pos;
	return pos;
}

const uint8_t EMPTY_SIGN=0xEE;

double ByteDataBuffer::readBCD(const char *format) {
	int low=1, sign=0, multply=0;
	double pow=1, spow=1, data=0, dp=1;
	const char *p=format;
	this->nullSign=false;

	uint8_t ub, *pdata=this->tail;

	if ((*pdata)==EMPTY_SIGN) {
		int formatSize=strlen(format);
		if(strchr(format,'.')!= NULL)--formatSize;

		int size=formatSize/2;
		this->pos+=size;
		this->tail+=size;
		this->nullSign=true;
		return 0;
	};

	while (*p) {
		switch (*p) {
		case 'S':
			sign=1;
			break;
		case 'G':
			sign=1;
			multply=1;
		case '#':
		case '9':
			if (low) {
				ub=((*pdata) &0xf);
				if (ub>9)
					ub = ub %10;
				data= data + ub*dp;
				low= !low;
			} else {
				ub=((*pdata)>>4)&0xf;

				if ((sign)&&(*(p+1)=='\0')) {
					if (multply) {
						if ((ub&1) !=0) {
							data= -1*data;
						}
						ub = ub>>1 &0x7;
						switch (ub) {
						case 0:
							pow=10000;
							break;
						case 1:
							pow = 1000;
							break;
						case 2:
							pow = 100;
							break;
						case 3:
							pow =10;
							break;
						case 4:
							pow =1;
							break;
						case 5:
							pow =0.1;
							break;
						case 6:
							pow = 0.01;
							break;
						case 7:
							pow = 0.001;
							break;
						}
					} else {
						data= data + (ub&0x7)*dp;
						if ((ub &0x8)!=0) {
							data= -1*data;
						}
					}

				} else {
					if (ub>9)
						ub = ub %10;
					data= data + ub *dp;
				}
				pdata++;
				this->pos++;
				low= !low;
			}
			pow=pow*spow;
			dp=dp*10;
			break;
		case '.':
			spow=0.1;
			break;
		}
		p++;
	}
	this->tail=pdata;
	return data*pow;
}
void ByteDataBuffer::writeBCD(double d,const char *format) {
	int length=0, pow=0, sign=0, m, i;
	uint8_t b;
	const char *p=format;
	double data;
	while (*p) {
		switch (*p) {
		case 'S':
			sign=1;
			break;
		case '.':
			pow=1;
			break;
		case '#':
		case '9':
			length++;
			pow *=10;
			break;

		}
		p++;
	}
	if (pow==0)
		pow =1;
	if (d>0) {
		data=floor(d*pow);
	} else {
		data=floor(-d*pow);
	};

	for (i=0; i<length; i+=2) {
		m= (int)(data-floor(data/100)*100);
		data=floor(data/100);
		b= ((m/10)<<4)+(m%10);
		if ((sign)&&(i+3>length)) {

			if (d>0) {
				b=b&0x7f;
			} else {
				b=b|0x80;
			}
		}
		writeInt8(b);
	}

}
void ByteDataBuffer::writeBCDG(double d,const char *format) {
	//TODO
	int length=0,multply = 0,pows=0, sign=0, m, i;
	int a = 1;
	uint8_t b;
	const char *p=format;
	long data;
	while (*p) {
		switch (*p) {
		case 'G':
			sign = 1;
			multply = 1;
//			pows = 1;
		case '#':
		case '9':
			length++;
			break;

		}
		p++;
	}
	a = (int)((d - (int)d)*10);
	if(d > 9999999){
		a = (int)(d/10000000);
		while(a){
			a /= 10;
			pows--;
		}
		d = d*(10*pow(10,pows));
	}else{
		while(a){
			d *= 10;
			a = (int)((d - (int)d)*10);
			pows ++;
			if(pows == 3){
				break;
			}
		}
	}
	data = abs((long)floor(d));
	for (i=0; i<length; i+=2) {
		m = data - (int)floor(data/100)*100;
		data =  data/100 ;
		if ((sign)&&(i+3>length)) {
			b = ((4+pows)<<5)+(m%10);
			if (d>0) {
				b=b&0xEF;
			}else{
				b=b|0x10;
			}
		}else{
			b= ((m/10)<<4)+(m%10);
		}
		writeInt8(b);
	}

}
int ByteDataBuffer::readDate(struct tm *tm,const char *format) {

	char rformat[20];
	int m, pos=0;
	const char *p=format;
	memset(tm,0,sizeof(*tm));
	while (*p) {
		rformat[pos++]='#';
		rformat[pos++]='#';
		p++;
	};
	rformat[pos]='\0';

	double data =readBCD(rformat);
	p--;
	while (p>=format)
	{
		m= (int)(data- floor(data/100)*100);
		data=floor(data/100);
		switch (*p) {
		case 'S':
			tm->tm_sec= m;
			break;
		case 'm':
			tm->tm_min=m;
			break;
		case 'H':
			tm->tm_hour=m;
			break;
		case 'D':
			tm->tm_mday=m;
			break;
		case 'M':
			tm->tm_mon= m-1;
			break;
		case 'W':
			if (m)
				tm->tm_mon= (m)%20-1;
			break;
		case 'Y':
			tm->tm_year= m+ 100;
			break;
		};
		p--;
	}
	return 0;
}
int ByteDataBuffer::writeBytes(uint8_t *buf, int bufsize) {
	memcpy(this->tail, buf, bufsize);
	this->pos+=bufsize;
	this->tail+=bufsize;
	return bufsize;
}
int ByteDataBuffer::readBytes(uint8_t *buf, int bufsize) {
	if (bufsize>0) {
		memcpy(buf, this->tail, bufsize);
		this->pos+=bufsize;
		this->tail+=bufsize;
	}
	return bufsize;
}
int ByteDataBuffer::readString(char *buf, int bufsize) {
	if (bufsize >0) {
		int i, isEnd=0;
		*buf = '\0';
		for (i=0; i<bufsize; i++) {
			if (!isEnd && (*(this->tail) != '\0')) {
				*(buf+i) = *(this->tail);
			} else {
				isEnd = 1;
			}
			this->tail++;
		}
	}
	this->pos += bufsize;
	return bufsize;
}
int ByteDataBuffer::writeString(char *buf, int bufsize) {
	int i;
	int isEnd = 0;
	for (i = 0; i < bufsize; i++) {
		if (!isEnd && *(buf+i)) {
			*(this->tail) = (char)*(buf+i);
		} else {
			*(this->tail) = '\0';
			isEnd = 1;
		}
		this->tail++;
	}
	this->pos += bufsize;
	return bufsize;
}
void ByteDataBuffer::writeDate(struct tm *tm,const char *format) {
	const char *p=format;
	char wformat[20];
	int pos=0;
	double data=0;

	while (*p != '\0') {
		switch (*p) {
		case 'S':
			data=data*100+tm->tm_sec;
			break;
		case 'm':
			data=data*100+tm->tm_min;
			break;
		case 'H':
			data=data*100+tm->tm_hour;
			break;
		case 'D':
			data=data*100+tm->tm_mday;
			break;
		case 'M':
			data=data*100+tm->tm_mon+1;
			break;
		case 'W':
			data=data*100+tm->tm_mon+1;
			break;
		case 'Y':
			data=data*100+((tm->tm_year)%100);
			break;
		};
		++p;
		wformat[pos++]='#';
		wformat[pos++]='#';
	}
	wformat[pos]='\0';
	writeBCD(data, wformat);
}
int ByteDataBuffer::readDateFrm1(struct tm *tm) {
	const char *p ="SmHDWY";
	while (*p) {
		int data = 0;
		if (*p == 'W') {
			data = readInt8();
		} else {
			data = (int)readBCD("##");
		}
		switch (*p) {
		case 'S':
			tm->tm_sec= data;
			break;
		case 'm':
			tm->tm_min=data;
			break;
		case 'H':
			tm->tm_hour=data;
			break;
		case 'D':
			tm->tm_mday=data;
			break;
		case 'W': {
			tm->tm_wday = (data>>5)%7;
			tm->tm_mon = ((data>>4) & 1)*10 + (data & 0x0f) - 1;
		}
			break;
		case 'Y':
			tm->tm_year= data+ 100;
			break;
		};
		p++;
	}
	return 0;
}
int ByteDataBuffer::writeDateFrm1(struct tm *tm) {
	const char *p ="SmHDWY";
	while (*p) {
		switch (*p) {
		case 'S':
			writeBCD(tm->tm_sec, "##");
			break;
		case 'm':
			writeBCD(tm->tm_min, "##");
			break;
		case 'H':
			writeBCD(tm->tm_hour, "##");
			break;
		case 'D':
			writeBCD(tm->tm_mday, "##");
			break;
		case 'W': {
			int week = (tm->tm_wday==0 ? 7 : tm->tm_wday);//1~7表示周一到周日
			writeInt8((uint8_t)(week * 32 + floor(tm->tm_mon/10) * 16
					+ tm->tm_mon%10 + 1));
		}
			break;
		case 'Y':
			writeBCD(tm->tm_year%100, "##");
			break;
		};
		p++;
	}
	return 0;
}

}//namespace Protocol
}//namespace Uarc
