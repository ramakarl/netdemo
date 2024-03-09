
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

		// demo app protocol
		void RequestWords ( int num );		

	private:
		int			mSrvSock;				// this is my client socket on the server

		TimeX		m_currtime;
		TimeX		m_lasttime;

	};

#endif