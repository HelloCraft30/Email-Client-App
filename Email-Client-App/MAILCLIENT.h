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
	std::string password;
	SOCKET smtpSock;
	SOCKET pop3Sock;
	bool connectSmtp();
	bool connectPop3();
	void disconnect(SOCKET& socket);
	bool makeSpace();
	std::vector<MAILFOLDER> folders;
	int autoLoad;
public:
	bool checkConnection();

	//set get functions
	std::string getHostIp();
	int getSMTPp();
	int getPOP3p();
	std::string getLocalUser();
	std::string getPassword();
	int getAutoload();
	
	//config
	void configClient(
		const std::string& hostip,
		const std::string& user,
		const std::string& pass,
		int smtp,
		int pop3,
		int atload
	);

	MAILCLIENT(std::string IP, int smtp, int pop3);

	void sendMail(const EMAIL& mail);
	void updateInboxMail();
	void readMail(); 

	int viewFunction();
};

