#pragma once
#include "MAILFOLDER.h"
#include<winsock.h>
#include "base64.h"
#pragma comment(lib, "ws2_32.lib")

class MAILCLIENT{
private:
	std::string hostIP;
	int SMTPp;
	int POP3p;
	std::string localUser;
	SOCKET smtpSock;
	SOCKET pop3Sock;
	void connectSmtp();
	void connectPop3();
	void disconnect(SOCKET& socket);
	bool makeSpace();
	std::vector<MAILFOLDER> folders;

public:
	std::string getUser();
	MAILCLIENT(std::string IP, int smtp, int pop3);

	void sendMail(const EMAIL& mail);
	void dataUpdate();
	void readMail(); 

	int viewFunction();
};

