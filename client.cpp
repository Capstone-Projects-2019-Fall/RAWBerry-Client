#define RSTP_ADDRESS "127.0.1.1"
#define RTSP_PORT 8554
#define RTSP_PATH "/mjpeg/1"
#define BUFF_SIZE 4096

//local port for UDP stream
#define LOCAL_PORT 8555

#define DEBUG 1 //comment out to remove debug messages

#include <string>
#include <iostream>
#include <sstream>
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <netdb.h> 

using namespace std;
typedef int SOCKET;

//make a string of the server's url
string build_url(){

	ostringstream url;

	url << "rstp://" << RSTP_ADDRESS << ":" << RTSP_PORT << RTSP_PATH;

	return url.str();

}

//used to send command to rstp server (PLAY, OPTIONS, etc), then gets reply from server
static bool send_cmd(SOCKET rtsp_socket,
	string request, string &reply){


	#ifdef DEBUG
		cout <<"\n---------------------------------\n" << "<request>\n" << request <<"\n---------------------------------\n\n";
	#endif

	//get server url
	string url = build_url();

	#ifdef DEBUG
		cout << "\n---------------------------------\n" << "<server url>\n" << url << "\n---------------------------------\n\n";
	#endif
	
	//send request to server
	if (send(rtsp_socket, request.c_str(),
		static_cast<int>(request.size()), 0) == request.size())
	{
		char buffer[BUFF_SIZE];
		int bytes = recv(rtsp_socket, buffer, BUFF_SIZE, 0);
		switch (bytes)
		{
		case -1: //Socket error
		case 0: // Socket was closed
			
			break;

		default:
			//get reply
			reply = string(buffer, bytes);
			
			#ifdef DEBUG
				cout << "\n---------------------------------\n" << "<reply>\n" << reply << "\n---------------------------------\n\n" ;
			#endif

			//msg sent and reply recieved successfully
			return true;
			break;
		}
	}
	//no reply, something went wrong
	return false;
}


/*
	Warning: The send functions are not compatable with other rtsp servers.
	I was lazy and ignored standards since our server doesn't care
*/

bool send_options(SOCKET rtsp_socket, int &request_count){
	
	ostringstream request;
	request << "OPTIONS " << build_url() << " RTSP/1.0\r\n"
		<< "CSeq: " << request_count << "\r\n"
		<< "Password=passw0rd\r\n"  //<= not actually my password, I swear.
		<< "user=admin\r\n"
		<< "\r\n";
	string reply;
	return send_cmd(rtsp_socket, request.str(), reply);
}
bool send_describe(SOCKET rtsp_socket, int &request_count){
	//build the request
	ostringstream request;
	request << "DESCRIBE " << build_url() << " RTSP/1.0\r\n"
		<< "CSeq: " << request_count << "\r\n"
		<< "blah\r\n"
		<< "Blahhhh\r\n"
		<< "\r\n";

	request_count++;

	string reply;
	return send_cmd(rtsp_socket, request.str(), reply);
}

//ok, I actually followed the standards on this one.
bool send_setup(SOCKET rtsp_socket, int &request_count){
	ostringstream request;
	
	request << "SETUP " << build_url << " RTSP/1.0\r\n"
		<< "CSeq: " << request_count << "\r\n"
		<< "Transport: RTP/AVP;unicast;client_port=" << LOCAL_PORT << "-" << LOCAL_PORT + 1 << "\r\n"
		<< "User-Agent: RAWBerry\r\n"
		<< "\r\n";
	
	request_count++;

	string reply;
	return send_cmd(rtsp_socket, request.str(), reply);
}

bool send_play(SOCKET rtsp_socket, int &request_count){
	ostringstream request;
	
	request << "PLAY " << build_url << " RTSP/1.0\r\n"
		<< "CSeq: " << request_count << "\r\n"
		<< "frknrf\r\n"
		<< "fkrnknf\r\n"
		<< "jmfmwefmwke\r\n";
	
	request_count++;
	string reply;
	return send_cmd(rtsp_socket, request.str(), reply);
}

void session_handler(SOCKET rtsp_socket, int &request_count){
	
	// Send OPTIONS, DESCRIBE, SETUP, and PLAY requests to establish RTSP
	send_options(rtsp_socket, request_count);

	send_describe(rtsp_socket, request_count);

	send_setup(rtsp_socket, request_count);

	send_play(rtsp_socket, request_count);
}

int main(){

	//for rtsp socket
	SOCKET rtsp_sock;
	sockaddr_in rtsp_addr;
	//tracking server requests
	int request_count = 1;

	//create socket for RTSP Connection
	rtsp_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	//SET TO IPV6 SOON!
	if (rtsp_sock < 0) { 
        printf("socket creation failed\n"); 
        exit(0); 
    } 

	rtsp_addr.sin_family      = AF_INET;					//NEED TO SWITCH TO IPV6 BEFORE DEMO
    rtsp_addr.sin_addr.s_addr = INADDR_ANY;					//IP doesn't matter yet, testing locally. FIX BEFORE DEMO!
    rtsp_addr.sin_port        = htons(8554);                // listen on RTSP port 8554

	
	//while (true)
	//{
	
	//connect to socket
    if (connect(rtsp_sock, 
		reinterpret_cast<sockaddr *>(&rtsp_addr), sizeof(rtsp_addr)) < 0){
			//connect failed :(
			puts("Could not connect to server.");
		}
	
	//else socket success
	else {

		#ifdef DEBUG
			cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << "rtsp_sock connected!" << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
		#endif
		
		//handle rtsp session
		session_handler(rtsp_sock, request_count);

		close(rtsp_sock);
		}
	//}
}
    