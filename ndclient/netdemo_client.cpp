
#include <conio.h>

#include <openssl/err.h>
#include <openssl/md5.h>
#include <openssl/ssl.h>
#include <openssl/x509v3.h>
#include <mutex>

#include "netdemo_client.h"

// #define ENABLE_SSL


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
	mSock = NET_NOT_CONNECTED; 
}

void NDClient::Reconnect ()
{
	// reconnect to server
	std::string serverName = "192.168.1.78";  //"localhost";
	int serverPort = 16101;

	dbgprintf ( "Connecting..\n" );	
	mSock = netClientConnectToServer ( serverName, serverPort, false );	

	netPrint (true);

	// secure socket with SSL
	#ifdef ENABLE_SSL
		if (mSock != NET_NOT_CONNECTED) {
			AttachSSLtoSocket ( getSock(mSock).socket );
		}
	#endif
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
	if ( elapsed_sec >= 0.01 ) {
		m_lasttime = m_currtime;

		if ( netIsConnected( mSock) ) {			
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
	int srv_sock = netGetServerSocket ( mSock );
	if ( srv_sock != -1) {
		Event e = new_event ( 120, 'app ', 'cRqs', 0, mEventPool );	
		e.attachInt ( srv_sock );  // must always tell server which socket 
		e.attachInt ( num );		   // payload

		netSend ( e );		// send to server
	}
}

void main (int argc, char* argv[])
{
	//----- network performance profiling	
	// - libmin must be built with PROFILE_NET and USE_NVTX
	// - for CPU netRecv will show >1/ms due to perf printfs, use nvtx markers for better analysis
	// - for NVTX the nvToolsExt64_1.dll must be present
	// - enable this line to see profiling:
	// PERF_INIT ( 64, true, true, true, 0, "" );	

	NDClient cli;

	cli.Start ();

	while ( !_kbhit() ) {		

		cli.Run ();
	}

	cli.Close ();  
}


//-----------------------------------------------
//--------- TESTING HERE
//--------- This code will be moved into network_system.cpp
//
bool NDClient::AttachSSLtoSocket ( SOCKET sock)
{

	//---- this is a hello world SSL test	

	int ret;
	std::string client_cert_path;
	std::string client_key_path;
	X509_STORE *ca_cert_store_ = nullptr;

	// create an SLL context
	SSL_CTX* ctx = SSL_CTX_new ( SSLv23_client_method() );
	
	// read certificate file or private key
	if (!client_cert_path.empty() && !client_key_path.empty()) {
    if (SSL_CTX_use_certificate_file( ctx, client_cert_path.c_str(), SSL_FILETYPE_PEM) != 1 ||
        SSL_CTX_use_PrivateKey_file( ctx, client_key_path.c_str(), SSL_FILETYPE_PEM) != 1) {
			dbgprintf ( "SSL Error. Problem with certificate file or private key.\n\n" );
      SSL_CTX_free( ctx );
      ctx = nullptr;
			return false;
    }
  } else {
		dbgprintf ( "SSL Error. Certificate file or private key not specified.\n\n" );
		return false;
	}

	// create a new SSL socket
  std::mutex ctx_mutex;
	SSL* ssl_sock = nullptr;
  {
    std::lock_guard<std::mutex> guard(ctx_mutex);
    ssl_sock = SSL_new(ctx);
  }

  if (!ssl_sock) {    
    return false;
  }

	// create a new BIO on an existing TCP/IP socket
  BIO* bio = BIO_new_socket(static_cast<int>( sock ), BIO_NOCLOSE);
	if (bio==0x0) {		
		dbgprintf( "BIO_new_socket. Error: %s\n", GetSSLError ( 0, 0 ).c_str() );		
	}

	// attach the BIO to the SSL socket
  SSL_set_bio( ssl_sock, bio, bio);

	// connect to the server
	ret = SSL_connect ( ssl_sock );				// use SSL_accept for the server
	if ( ret <= 0 ) {
		dbgprintf( "SSL_connect. Error: %s\n", GetSSLError ( ssl_sock, ret ).c_str() );
	}

	// write encrpyted data to server
	char buf[256];
	sprintf ( buf, "hello world" );
	ret = SSL_write ( ssl_sock, buf, 256 );

	if ( ret <= 0 ) {
		dbgprintf( "SSL_write. Error: %s\n", GetSSLError ( ssl_sock, ret ).c_str() );
	}

	return true;
	
	//SSL_set_tlsext_host_name(ssl, host_.c_str());   //-- client only

	//-- when ready to close an SSL socket..
  /*  SSL_shutdown(ssl);
    {
      std::lock_guard<std::mutex> guard(ctx_mutex);
      SSL_free(ssl);
    }

    close_socket(sock);
    return false;
  }*/
}


std::string NDClient::GetSSLError (SSL* sslsock, int ret)
{	
	 std::string msg = "";

	 // error code on socket
	 if (sslsock != 0x0) { 
		 int code = SSL_get_error (sslsock, ret );
		 switch (code)
		 {
		 case SSL_ERROR_NONE:					msg = "The TLS/SSL I/O operation completed."; break;
		 case SSL_ERROR_ZERO_RETURN:  msg = "The TLS/SSL connection has been closed."; break;
		 case SSL_ERROR_WANT_READ:    msg = "The read operation did not complete; the same TLS/SSL I/O function should be called again later.";    break;
		 case SSL_ERROR_WANT_WRITE:   msg = "The write operation did not complete; the same TLS/SSL I/O function should be called again later.";     break;
		 case SSL_ERROR_WANT_CONNECT: msg = "The connect operation did not complete; the same TLS/SSL I/O function should be called again later.";      break;
		 case SSL_ERROR_WANT_ACCEPT:  msg = "The accept operation did not complete; the same TLS/SSL I/O function should be called again later.";      break;
		 case SSL_ERROR_WANT_X509_LOOKUP:  msg = "The operation did not complete because an application callback set"
					" by SSL_CTX_set_client_cert_cb() has asked to be called again. "
					"The TLS/SSL I/O function should be called again later.";
					break;
		 case SSL_ERROR_SYSCALL: msg = "Some I/O error occurred. The OpenSSL error queue is here:";     break;
		 case SSL_ERROR_SSL:     msg = "A failure in the SSL library occurred, usually a protocol error. The OpenSSL error queue is here:"; break;
		 default: msg = "Unknown error"; break;
		 };
		 msg += "\n";
	 }

	 // retrieve error queue 
	 char buf[512];
	 unsigned long err = ERR_get_error();
	 while ( err != 0 ) {
		 ERR_error_string ( err, buf );
		 msg += std::string(buf) + "\n";
		 err = ERR_get_error();
	 }	 

	 return msg;
}