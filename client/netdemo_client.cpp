
#include <conio.h>

#include "netdemo_client.h"

int NDClient::NetEventCallback (Event& e, void* this_pointer) {
    NDClient* self = static_cast<NDClient*>(this_pointer);
    return self->Process ( e );
}

void NDClient::Start ()
{
	bool bDebug = false;
	bool bVerbose = true;

	// start timer
	m_currtime.SetTimeNSec();	
	m_lasttime = m_currtime;
	srand ( m_currtime.GetMSec() );

	// start networking
	netInitialize();
	netVerbose( bVerbose );
	netDebug( bDebug );	

	// start client on random port
	int cli_port = rand()*1000 / RAND_MAX;
	netStartClient ( cli_port );
	netSetUserCallback ( &NetEventCallback );
	
	dbgprintf ( "Client IP: %s\n", getIPStr ( getHostIP() ).c_str() );	

	// not yet connected (see Run func)
	mSrvSock = NET_NOT_CONNECTED; 
}

void NDClient::Reconnect ()
{
	// reconnect to server
	std::string serverName = "localhost";
	int serverPort = 1010;

	dbgprintf ( "Connecting..\n" );	
	mSrvSock = netClientConnectToServer ( serverName, serverPort, false );	
}

void NDClient::Close ()
{
	netCloseConnection ( mSrvSock );
}


int NDClient::Process ( Event& e )
{
	std::string line;
	eventStr_t sys = e.getTarget ();

	// Check for net error events
	if ( sys == 'net ' && e.getName()=='nerr' ) {
		// enable netVerbose(true) for detailed messages.
		// application can gracefully handle specific net error codes here..		
		int code = e.getInt ();

		return 0;
	}
	// Process Network events
	e.startRead ();
	switch (e.getName()) {
	case 'sOkT': {
		// Connection complete. server accepted OK.
		int srv_sock = e.getInt();		// server sock
		int cli_sock = e.getInt();		// local socket 
		dbgprintf( "  Connected to server: %s, %d\n", mSockets[cli_sock].dest.name.c_str(), srv_sock);

		return 1;
	  //case 'sOkT': {
	} break;	
	};

	// Process Application events
	switch (e.getName()) {
	case 'sRst': {
		// server send back the words
		std::string words = e.getStr ();
		dbgprintf ("  Result from server: %s\n", words.c_str() );
		return 1;
		} break;
	case 'sFIN': {
		// server shutdown unexpectedly
		dbgprintf ("  Server disconnected.\n" );
		return 1;
	  } break;
	};

	dbgprintf ( "   Unhandled message: %s\n", e.getNameStr().c_str() );
	return 0;
}

int NDClient::Run ()
{
	m_currtime.SetTimeNSec();	

	// demo app - request the words for a random number every 2 secs
	//
	float elapsed_sec = m_currtime.GetElapsedSec ( m_lasttime );
	if ( elapsed_sec >= 2 ) {
		m_lasttime = m_currtime;

		if ( netIsConnected( mSrvSock) ) {			
			// if connected, make request
			int rnd = rand()*10000/RAND_MAX;
			RequestWords ( rnd );
			dbgprintf ( "  Requested words for: %d\n", rnd );
		
		} else {
			// if disconnected, try and reconnect
			Reconnect ();
		}
	}

	// process event queue
	return netProcessQueue ();
}


void NDClient::RequestWords (int num)
{	
	// demo application protocol:
	//
	// cRqs - request the words for a number (c=msg from client)
	// sRst - here is the result containing the words (s=msg from server)

	// create cRqs app event
	int sock = netGetServerSocket ( mSrvSock );
	if ( sock != -1) {
		Event e = new_event ( 120, 'app ', 'cRqs', 0, mEventPool );	
		e.attachInt ( sock );  // must always tell server our sock
		e.attachInt ( num );		// payload

		netSend ( e );		// send to server
	}
}

void main (int argc, char* argv[])
{
	//-- network performance profiling	
	// libmin must be built with PROFILE_NET and USE_NVTX
	// for CPU netRecv will show >1/ms due to perf printfs (see nvtx markers for better analysis)
	// for NVTX the nvToolsExt64_1.dll must be present
	// enable this line to see profiling:
	// PERF_INIT ( 64, true, true, true, 0, "" );	

	NDClient cli;

	cli.Start ();

	while ( !_kbhit() ) {		

		cli.Run ();
	}

	cli.Close ();  
}