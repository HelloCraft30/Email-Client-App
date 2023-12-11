#pragma once
#include "MAILFOLDER.h"
#include<winsock.h>
#include "base64.h"
#include "pugixml/src/pugixml.hpp"
#pragma comment(lib, "ws2_32.lib")

class MAILCLIENT {
private:
	void readConfigFromXML();
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

	void sendMail( EMAIL& mail);
	void updateInboxMail();
	int readMail();

	void mail_install_attachment(EMAIL& mail, std::string path, std::vector<int> imails);

	void filterMail(EMAIL& email, const std::string& user);
	void filterMail(std::vector<EMAIL>& emails, const std::string& user);
	void filterMail();

	int viewFunction();
};

