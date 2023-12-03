#include "MAILCLIENT.h"

std::string MAILCLIENT::getUser() {
	return localUser;
}

MAILCLIENT::MAILCLIENT(std::string IP, int smtp, int pop3) {
	hostIP = IP;
	SMTPp = smtp;
	POP3p = pop3;
	localUser = "hcmus.edu.vn";
	smtpSock = socket(AF_INET, SOCK_STREAM, 0);
	pop3Sock = socket(AF_INET, SOCK_STREAM, 0);
	makeSpace();
	std::string txtFolder = "Mail_client\\" + localUser + "\\folders.txt";
	std::fstream fFolder(txtFolder.c_str(), std::ios::in);

	int nFolder = 0;
	fFolder >> nFolder;
	fFolder.ignore();

	for (int i = 0; i < nFolder; i++) {
		std::string tempFolder;
		std::getline(fFolder, tempFolder);
		folders.push_back(tempFolder);
	}

	fFolder.close();

	for (auto& a : folders) {
		if (!a.makeSpace(localUser)) {
			std::string _path = "Mail_client\\" + localUser + "\\" + a.name + "\\count.txt";
			std::fstream _nMailf(_path.c_str(), std::ios::in);
			int nMail = 0; _nMailf >> nMail; _nMailf.close();
			for (int i = 1; i <= nMail; i++) {
				std::string _path_mail = "Mail_client\\" + localUser + "\\" + a.name + "\\mail_" + std::to_string(i) + "\\content.txt";
				EMAIL mail; mail.inputF(_path_mail);
				a.addMail(mail);
			}
		}
	}
}

void MAILCLIENT::connectSmtp() {
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(SMTPp);
	serverAddress.sin_addr.s_addr = inet_addr(hostIP.c_str());

	connect(smtpSock, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
	char removeFirstLine[20]{};
	recv(smtpSock, removeFirstLine, 20, 0);
}

void MAILCLIENT::connectPop3() {
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(POP3p);
	serverAddress.sin_addr.s_addr = inet_addr(hostIP.c_str());

	connect(pop3Sock, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
	char removeFirstLine[20]{};
	recv(pop3Sock, removeFirstLine, 20, 0);
}

void MAILCLIENT::disconnect(SOCKET& socket) {
	send(socket, "QUIT\r\n", sizeof("QUIT\r\n") - 1, 0);
}

bool createFolder(const char* str) {
	return !_mkdir(str);
}

bool MAILCLIENT::makeSpace() {
	std::string tmp = "Mail_client\\" + localUser;
	if (_mkdir(tmp.c_str())) return false;
	std::string txtFolder = tmp + "\\folders.txt";

	std::fstream file(txtFolder.c_str(), std::ios::out | std::ios::trunc);
	file << 5 << '\n';
	file << "Inbox\n";
	file << "Project\n";
	file << "Important\n";
	file << "Work\n";
	file << "Spam\n";
	file.close();

	return true;
}

int MAILCLIENT::viewFunction() {
	system("cls");
	std::cout << "----------------------------------------\n";
	std::cout << "    MAIL-CLIENT\n";
	std::cout << "----------------------------------------\n";
	std::cout << "Hello, <" << localUser << ">\n";
	std::cout << "Menu:\n";
	std::cout << "1. Send email\n";
	std::cout << "2. Read email\n";

	std::cout << "Your choice: ";
	int result = 0;
	std::cin >> result;
	std::cin.ignore();
	return result;
}

void MAILCLIENT::sendMail(const EMAIL& mail) {
	std::string buffer = "EHLO" + hostIP + "\r\n";
	connectSmtp();

	send(smtpSock, buffer.c_str(), buffer.size(), 0);
	buffer = "MAIL FROM: " + mail.sender + "\r\n";
	send(smtpSock, buffer.c_str(), buffer.size(), 0);

	for (const auto& x : mail.recvTO) {
		buffer = "RCPT TO: <" + x + ">\r\n";
		send(smtpSock, buffer.c_str(), buffer.size(), 0);
	}
	for (const auto& x : mail.recvCC) {
		buffer = "RCPT TO: <" + x + ">\r\n";
		send(smtpSock, buffer.c_str(), buffer.size(), 0);
	}
	for (const auto& x : mail.recvBCC) {
		buffer = "RCPT TO: <" + x + ">\r\n";
		send(smtpSock, buffer.c_str(), buffer.size(), 0);
	}

	send(smtpSock, "DATA\r\n", sizeof("DATA\r\n") - 1, 0);

	buffer = "To: ";
	for (const auto& x : mail.recvTO) {
		buffer += x + ", ";
	} buffer.pop_back(); buffer.pop_back();
	buffer += "\n";
	send(smtpSock, buffer.c_str(), buffer.size(), 0);

	buffer = "CC: ";
	for (const auto& x : mail.recvCC) {
		buffer += x + ", ";
	}
	if(buffer!="CC: ") buffer.pop_back();
	buffer.pop_back();
	buffer += "\n";
	send(smtpSock, buffer.c_str(), buffer.size(), 0);

	buffer = "From: " + localUser + "\n";
	send(smtpSock, buffer.c_str(), buffer.size(), 0);

	buffer = "Subject: " + mail.subject + "\n";
	send(smtpSock, buffer.c_str(), buffer.size(), 0);

	send(smtpSock, "--boundary_text\n", sizeof("--boundary_text\n") - 1, 0);
	send(smtpSock, "Content-type: txt\n", sizeof("Content-type: txt\n") - 1, 0);
	buffer = "line: " + std::to_string(mail.body.size()) + "\n";
	send(smtpSock, buffer.c_str(), buffer.size(), 0);

	for (const auto& x : mail.body) {
		buffer = "\n" + x;
		send(smtpSock, buffer.c_str(), buffer.size(), 0);
	}
	send(smtpSock, "\n\n\n", sizeof("\n\n\n") - 1, 0);

	int n = mail.attachFiles.size();
	std::map<std::string, bool> mp;
	for (int i = 0; i < n; i++) {
		std::string fileName, extension;
		SplitPath(mail.attachFiles[i].filePath, fileName, extension);
		if (mp[mail.attachFiles[i].filePath] == 0) mp[mail.attachFiles[i].filePath] = 1;
		else continue;
		buffer = "Name = " + fileName + "," + extension + "," + std::to_string(mail.attachFiles[i].nBytes) + "\n";
		send(smtpSock, buffer.c_str(), buffer.size(), 0);
		send(smtpSock, "\n", 1, 0);

		std::cout << mail.attachFiles[i].filePath << '\n';
		system("pause");

		std::fstream fileOpen(mail.attachFiles[i].filePath.c_str(), std::ios::in | std::ios::binary);

		int bytes = getFileSize(mail.attachFiles[i].filePath);

		std::vector<BYTE> fileData(bytes);
		fileOpen.read(reinterpret_cast<char*>(&fileData[0]), bytes);
		fileOpen.close();
		std::string encodedData = base64_encode(&fileData[0], fileData.size());
		send(smtpSock, encodedData.c_str(), encodedData.size(), 0);

		send(smtpSock, "\n\r\n", sizeof("\n\r\n") - 1, 0);
	}


	send(smtpSock, "--END\n", sizeof("--END\n") - 1, 0);
	send(smtpSock, ".\r\n", sizeof(".\r\n") - 1, 0);

	disconnect(smtpSock);
}

std::string _getline(SOCKET& socket, char sp = '\n') {
	std::string result;
	char chr = ' ';
	recv(socket, &chr, 1, 0);
	while (chr != sp) {
		if(chr!='\r') result.push_back(chr);
		recv(socket, &chr, 1, 0);
	}
	result.push_back('\0');
	return result;
}

void MAILCLIENT::dataUpdate() {
	//connect to pop3
	connectPop3();
	//remove test mail server text
	char __remove[20]{};
	recv(pop3Sock, __remove, 20, 0);
	//materials
	//paths
	std::string _path = "Mail_Client\\" + localUser + "\\Inbox";
	std::string _path_count = _path + "\\count.TXT";
	//nMails
	int nMail = 0;
	std::fstream __getnMail(_path_count.c_str(), std::ios::in);
	__getnMail >> nMail; __getnMail.close();

	//get server info
	std::string SEND = "USER " + localUser + "\r\n";
	send(pop3Sock, SEND.c_str(), SEND.size(), 0);
	//S: +OK
	recv(pop3Sock, __remove, 20, 0);
	SEND = "PASS 123\r\n";
	send(pop3Sock, SEND.c_str(), SEND.size(), 0);
	//S: +OK
	recv(pop3Sock, __remove, 20, 0);
	SEND = "STAT\r\n";
	send(pop3Sock, SEND.c_str(), SEND.size(), 0);
	recv(pop3Sock, __remove, 20, 0);
	//S: +OK size bytes
	int _nMail = 0;
	std::string __num;
	bool isDone = false;
	for (int i = 0; i < 20; i++) {
		if (isdigit(__remove[i])) {
			__num.push_back(__remove[i]);
			isDone = true;
		}
		else if (isDone) break;
	}
	_nMail = atoi(__num.c_str());

	if (_nMail <= nMail) {
		disconnect(pop3Sock);
		return;
	}

	std::fstream __setMail(_path_count.c_str(), std::ios::out | std::ios::trunc);
	__setMail << _nMail; __setMail.close();
	
	for (int im = nMail + 1; im <= _nMail; im++) {
		//get content
		SEND = "RETR " + std::to_string(im) + "\r\n";
		send(pop3Sock, SEND.c_str(), SEND.size(), 0);
		std::vector<std::string> buffer;
		//ignore first line
		_getline(pop3Sock);
		for (int i = 0; i < 7; i++) {
			buffer.push_back(_getline(pop3Sock));
		}
		int nLines = atoi(buffer[6].substr(buffer[6].find(':') + 2).c_str());
		for (int i = 0; i < nLines + 2; i++) {
			buffer.push_back(_getline(pop3Sock));
		}

		EMAIL mail(buffer);

		//path to mail
		std::string _path_mail = _path + "\\mail_" + std::to_string(im);
		_mkdir(_path_mail.c_str());

		//file extensions things
		while (true) {
			std::string _line = _getline(pop3Sock);
			_line.pop_back();
			if (_line.size() == 0) {
				_line = _getline(pop3Sock);
				if (_line.find("--END")!=-1) break;
				else {
					std::string nameF; int number = 0;
					extractNameAndNumber(_line, nameF, number);

					mail.attachFiles.push_back(Attachment{ nameF, nameF, number });

					//remove 1 nothing line
					_line = _getline(pop3Sock);

					//create file extension
					std::string _pathFile = _path_mail + "\\" + nameF;

					std::fstream createFile(_pathFile.c_str(), std::ios::out | std::ios::binary);

					std::string encodeData = _getline(pop3Sock);
					std::vector<BYTE> decodedData = base64_decode(encodeData);
					for (const auto& x : decodedData) createFile << x;

					createFile.close();
				}
			}

		}
		std::string _line = _getline(pop3Sock);

		//content.txt
		std::string _mailContent = _path_mail + "\\content.txt";
		mail.outputF(_mailContent);
		folders[0].addMail(mail);
	}

	system("pause");
	disconnect(pop3Sock);
}

void MAILCLIENT::readMail() {
	std::cout << "This is list of folders in your mailbox:\n";
	for (int i = 0; i < folders.size(); i++) {
		std::cout << i+1 << ". " << folders[i].name << '\n';
	}

	int iFolder = 0;
	std::cout << "Which folder do you want to read: ";
	std::cin >> iFolder;
	if (folders[iFolder-1].mails.size() == 0) {
		std::cout << "Empty.\n";
	}
	else {
		std::cout << "This is list of mails in " << folders[iFolder-1].name << " folder:\n";

		int n = folders[iFolder-1].mails.size();
		for (int i = 1; i <= n; i++) {
			
			folders[iFolder-1].mails[i - 1].subShow(i);
		}
		int iEmail = 0;
		std::cout << "Which email do you want to read: ";
		std::cin >> iEmail;

		folders[iFolder - 1].mails[iEmail - 1].isRead = 1;
		folders[iFolder - 1].mails[iEmail - 1].show();
		folders[iFolder - 1].saveLocal(localUser);
	}
	

	system("pause");
}