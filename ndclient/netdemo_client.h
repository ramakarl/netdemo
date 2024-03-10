
#ifndef NETDEMO_CLIENT
	#define NETDEMO_CLIENT

	#include "network_system.h"

	class NDClient : public NetworkSystem {
	public:		
		// networking 
		void Start ();
		void Reconnect ();
		void Close ();		
		int Run ();				
		int Process (Event& e);
		static int NetEventCallback ( Event& e, void* this_ptr );	

		// secure networking (SSL)
		bool AttachSSLtoSocket ( SOCKET sock );
		std::string GetSSLError ( SSL* sslsock, int code);

		// demo app protocol
		void RequestWords ( int num );		

	private:
		int			mSock;					// this is my socket (local) to access the server

		TimeX		m_currtime;
		TimeX		m_lasttime;

	};

#endif