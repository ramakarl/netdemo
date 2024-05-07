
#ifdef _WIN32
  #include <conio.h>
#endif


#ifdef __linux__
  #include <stdio.h>
  #include <sys/ioctl.h>
  #include <termios.h>
  
  int _kbhit () {
    static const int STDIN = 0;
    static bool kbinit = false;
    if ( !kbinit) {
      termios term;
      tcgetattr(STDIN, &term);
      term.c_lflag &= ~ICANON;
      tcsetattr(STDIN, TCSANOW, &term);
      setbuf(stdin, NULL);
      kbinit=true;
    }
    int bytes;
    ioctl(STDIN, FIONREAD, &bytes);
    return bytes;
  }
#endif

#include "netdemo_client.h"




int NDClient::NetEventCallback (Event& e, void* this_pointer) {
    NDClient* self = static_cast<NDClient*>(this_pointer);
    return self->Process ( e );
}

void NDClient::Start ()
{
	bool bDebug = true;
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
	int cli_port = 10000 + rand() % 9000;
	netSetPathToPublicKey ( "server_pubkey.pem" );
	netClientStart ( cli_port );
	netSetUserCallback ( &NetEventCallback );
	
	dbgprintf ( "Client IP: %s\n", getIPStr ( getHostIP() ).c_str() );	

	// not yet connected (see Run func)
	mSock = NET_NOT_CONNECTED; 
}

void NDClient::Reconnect ()
{
	// reconnect to server
	std::string serverName = "localhost";  // 192.168.1.78,  144.126.214.218
	int serverPort = 16101;

	dbgprintf ( "Connecting..\n" );	
	mSock = netClientConnectToServer ( serverName, serverPort, false );	

	netPrint (true);

}

void NDClient::Close ()
{
	netCloseConnection ( mSock );
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
		//dbgprintf( "  Connected to server: %s, %d\n", mSockets[cli_sock].dest.name.c_str(), srv_sock);

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
	if ( elapsed_sec >= 0.5 ) {
		m_lasttime = m_currtime;

		if ( netIsConnectComplete (mSock) ) {			
			// if connected, make request
			int rnum = rand() % 10000;
			RequestWords ( rnum );
			dbgprintf ( "  Requested words for: %d\n", rnum );
		
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
	int srv_sock = getServerSock ( mSock );
	if ( srv_sock != -1) {
		Event e = new_event ( 120, 'app ', 'cRqs', 0, getNetPool() );	
		e.attachInt ( srv_sock );  // must always tell server which socket 
		e.attachInt ( num );		   // payload

		netSend ( e );		// send to server
	}
}


int main (int argc, char* argv[])
{
	//----- network performance profiling	
	// - libmin must be built with PROFILE_NET and USE_NVTX
	// - for CPU netRecv will show >1/ms due to perf printfs, use nvtx markers for better analysis
	// - for NVTX the nvToolsExt64_1.dll must be present
	// - enable this line to see profiling:
	// PERF_INIT ( 64, true, true, true, 0, "" );	

	NDClient cli;

	addSearchPath ( ASSET_PATH );

	cli.Start ();

	while ( !_kbhit() ) {		

		cli.Run ();
	}

	cli.Close ();  
 
    return 1;
}

