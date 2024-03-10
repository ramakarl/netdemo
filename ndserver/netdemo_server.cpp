
#include <conio.h>

#include "netdemo_server.h"

int NDServer::NetEventCallback (Event& e, void* this_pointer) {
    NDServer* self = static_cast<NDServer*>(this_pointer);
    return self->Process ( e );
}

void NDServer::Start ()
{
	bool bDebug = false;
	bool bVerbose = true;

	// start networking
	netInitialize();
	netVerbose( bVerbose );
	netDebug( bDebug );
	
	// start server listening
	int srv_port = 1010;
	netStartServer ( srv_port );
	netSetUserCallback ( &NetEventCallback );

	netPrint ();
	
	dbgprintf ( "Server IP: %s\n", getIPStr ( getHostIP() ).c_str() );	
	dbgprintf ( "Listening on %d..\n", srv_port );
}

void NDServer::Close ()
{
	
}

int NDServer::Run ()
{
	// process event queue
	return netProcessQueue ();
}


void NDServer::InitWords ()
{
	// demo app

	for (int n=0; n<10; n++) wordlist.push_back("");

	wordlist[0] = "zero";
	wordlist[1] = "one";
	wordlist[2] = "two";
	wordlist[3] = "three";
	wordlist[4] = "four";
	wordlist[5] = "five";
	wordlist[6] = "six";
	wordlist[7] = "seven";
	wordlist[8] = "eight";
	wordlist[9] = "nine";	
}

std::string NDServer::ConvertToWords ( int num )
{
	// demo - this is the main task of the server
	
	std::string words;
	int n = num;
	int v;

	while (n != 0 ) {
		v = n % 10;
		words = wordlist[v] + " " + words;
		n /= 10;
	}

	return words;
}

void NDServer::SendWordsToClient ( std::string msg, int sock )
{
	// demo app protocol:
	//
	// cRqs - request the words for a number (c=msg from client)
	// sRst - here is the result containing the words (s=msg from server)

	// create sRst app event
	Event e = new_event ( 120, 'app ', 'sRst', 0, mEventPool );	
	e.attachStr ( msg );

	netSend ( e, NET_CONNECT, sock );		// send to specific client
}

int NDServer::Process ( Event& e )
{
	int sock;
	std::string line;
	eventStr_t sys = e.getTarget ();

	// Check for net error events
	if ( sys == 'net ' && e.getName()=='nerr' ) {
		// enable netVerbose(true) for detailed messages.
		// application can gracefully handle specific net error codes here..		
		int code = e.getInt ();		
		if (code==NET_DISCONNECTED) {
			dbgprintf ( "  Connection to client closed unexpectedly.\n" );
		}		
		return 0;
	}

	// Process Network events
	e.startRead ();
	switch (e.getName()) {
	case 'sOkT': 
		// Connection to client complete. (telling myself)
		sock = e.getInt();		// server sock		
		dbgprintf ( "  Connected to client: #%d\n", sock );
		return 1;
		break;
	case 'cFIN': 
		// Client closed connection
		sock = e.getInt();
		dbgprintf ( "  Disconnected client: #%d\n", sock );
		return 1;
		break;		
	};

	// Process Application events
	switch (e.getName()) {
	case 'cRqs': 
		// client requested words for num
		int sock = e.getInt ();     // which client 
		int num = e.getInt();		

		// convert the num to words 
		std::string words = ConvertToWords ( num );

		// send words back to client
		SendWordsToClient ( words, sock );

		dbgprintf ( "  Sent words to #%d: %d, %s\n", sock, num, words.c_str() );
		return 1;
		break;
	};

	dbgprintf ( "   Unhandled message: %s\n", e.getNameStr().c_str() );
	return 0;
}

void main (int argc, char* argv[])
{
	NDServer srv;

	srv.Start ();
	srv.InitWords ();

	while ( !_kbhit() ) {

		srv.Run ();
	}

	srv.Close ();  
}