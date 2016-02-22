                                                                                                                                                                                                                                  #include "uarc_intfd.h"
#include "UARCServiceImpl.h"
#include <Poco/Thread.h>
#include <Poco/ThreadPool.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/File.h>
#include <Poco/Path.h>
#include <Poco/Process.h>
#include <iostream>
#include <sys/file.h>

using Poco::Process;
using Poco::File;
using Poco::Path;
using Poco::Thread;
using Poco::ThreadPool;
using Poco::Util::Application;
using Poco::Util::ServerApplication;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::OptionCallback;
using Poco::Util::HelpFormatter;
using namespace std;
using namespace google::protobuf;
using namespace RCF;

std::string  UARCIntfServer::rtdbms_ip;
int			 UARCIntfServer::rtdbms_port;

UARCIntfServer::UARCIntfServer() : _helpRequested(false) {

	int pid_file = ::open("/var/lock/uarc_intfd.lock", O_CREAT | O_RDWR, 0666);
	int rc = ::flock(pid_file, LOCK_EX | LOCK_NB);
	if(rc) {
		//std::cout << "fes_dbcntl进程已存在，请不要重复运行！" << std::endl;
		exit(-1);
	}
}

UARCIntfServer::~UARCIntfServer() {
}

void UARCIntfServer::initialize(Application & self)
{
	// load configuration files, if present
	loadConfiguration("/etc/uarc/uarc-cfg/uarc_intfd.properties");
	//loadConfiguration("./uarc_intfd.properties");
	ServerApplication::initialize(self);
	rtdbms_ip	= config().getString("RedisDB.Base.IPAddr", "127.0.0.1");
	rtdbms_port  = config().getInt("RedisDB.Base.Port", 6379);
	//logger().information("starting up",__FILE__,__LINE__);
}

void UARCIntfServer::uninitialize()
{
	//logger().information("shutting down",__FILE__,__LINE__);
	ServerApplication::uninitialize();
}

void UARCIntfServer::handleOption(const string & name, const string & value)
{
	ServerApplication::handleOption(name, value);

	if (name == "help")
		_helpRequested = true;
}

void UARCIntfServer::defineOptions(OptionSet & options)
{
	ServerApplication::defineOptions(options);

	options.addOption(
		Option("help", "h", "display help information on command line arguments")
			.required(false)
			.repeatable(false)
			.callback(OptionCallback<UARCIntfServer>(this, &UARCIntfServer::handleHelp)));
}

void UARCIntfServer::handleHelp(const string& name, const string& value)
{
	_helpRequested = true;
	displayHelp();
	stopOptionsProcessing();
}

void UARCIntfServer::displayHelp()
{
	HelpFormatter helpFormatter(options());
	helpFormatter.setCommand(commandName());
	helpFormatter.setUsage("OPTIONS");
	helpFormatter.setHeader("uarc interface service.");
	helpFormatter.format(cout);
}

int UARCIntfServer::main(const vector<string> & args)
{
	if ( _helpRequested ) {

		displayHelp();

		return Application::EXIT_OK;
	}

	int port  = config().getInt("Tcp.listenPort", 19860);

	// Initialize RCFProto.
	RCF::init();
	// Create server.
	RCF::RcfProtoServer server( RCF::TcpEndpoint("0.0.0.0", port) );
	// Bind Protobuf service.
	UARCServiceImpl serviceImpl;
	server.bindService(serviceImpl);
	// Start the server.
	server.start();

	// wait for CTRL-C or kill
	waitForTerminationRequest();

	//UARCIntfServer::GetLogger().debug("UARCIntfServer quit.",	__FILE__, __LINE__);

	return Application::EXIT_OK;
}

POCO_SERVER_MAIN(UARCIntfServer)
