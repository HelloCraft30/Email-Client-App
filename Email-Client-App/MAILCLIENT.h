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
	bool connectSmtp();
	bool connectPop3();
	void disconnect(SOCKET& socket);
	bool makeSpace();
	std::vector<MAILFOLDER> folders;
public:
	bool checkConnection();
	std::string getUser();
	MAILCLIENT(std::string IP, int smtp, int pop3);

	void sendMail(const EMAIL& mail);
	void updateInboxMail();
	void readMail(); 

	int viewFunction();

	//loc mail
	void filterMails(const std::string& localUser);
};

//removes leading and trailing whitespaces va xoa ki tu NULL
std::string trimSpaces(const std::string& input);

//Ham kiem tra chuoi x co trong chuoi y hay khong
bool isSubstring(const std::string& x, const std::string& y);

//Tao menu chon noi dung muon loc email (From, Subject,....)
int viewFunctionFilter();

