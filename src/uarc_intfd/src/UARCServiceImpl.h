#pragma once

#include "uarc.pb.h"
#include <RCFProto.hpp>
#include "rdbms.h"
//#include"rdbms.h"

class UARCServiceImpl : public UarcService {

private:

    void printRequest(google::protobuf::RpcController* controller, const google::protobuf::Message * request);

    void printResponse(google::protobuf::Message * response);

    bool saveItemCode(const ::MsgItemCode* code);

public:
    UARCServiceImpl();

    virtual ~UARCServiceImpl();

	void UarcSendSchedule(::google::protobuf::RpcController* controller,
                       const ::MsgSchedule* request,
                       ::MsgResult* response,
                       ::google::protobuf::Closure* done);
	void UarcSetDevice(::google::protobuf::RpcController* controller,
	                         const ::MsgDevice* request,
	                         ::MsgResult* response,
	                         ::google::protobuf::Closure* done);
	    void UarcSetCollTask(::google::protobuf::RpcController* controller,
	                         const ::MsgCollTask* request,
	                         ::MsgResult* response,
	                         ::google::protobuf::Closure* done);
	    void UarcSetItemCode(::google::protobuf::RpcController* controller,
	                         const ::MsgItemCode* request,
	                         ::MsgResult* response,
	                         ::google::protobuf::Closure* done);
	    void UarcSetFormular(::google::protobuf::RpcController* controller,
	                         const ::MsgFormular* request,
	                         ::MsgResult* response,
	                         ::google::protobuf::Closure* done);
	    void UarcCallData(::google::protobuf::RpcController* controller,
	                         const ::MsgCallData* request,
	                         ::MsgResult* response,
	                         ::google::protobuf::Closure* done);
	    void UarcBatchCallData(::google::protobuf::RpcController* controller,
	    						const ::MsgBatchCallData* request,
	    						::MsgBatchResults* response,
	    						 ::google::protobuf::Closure* done);
	    void UarcDeviceControl(::google::protobuf::RpcController* controller,
	                         const ::MsgControl* request,
	                         ::MsgResult* response,
	                         ::google::protobuf::Closure* done);

	    CRtDbMs*	_rtdbms;
};
