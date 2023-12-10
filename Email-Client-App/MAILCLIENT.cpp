#define _CRT_SECURE_NO_WARNINGS
#include "MAILCLIENT.h"

std::string MAILCLIENT::getLocalUser() {
	return localUser;
}

std::string MAILCLIENT::getHostIp() {
	return hostIP;
}

std::string MAILCLIENT::getPassword() {
	return password;
}

int MAILCLIENT::getSMTPp() {
	return SMTPp;
}

int MAILCLIENT::getPOP3p() {
	return POP3p;
}

int MAILCLIENT::getAutoload() {
	return autoLoad;
}

void MAILCLIENT::configClient(
	const std::string& hostip,
	const std::string& user,
	const std::string& pass,
	int smtp,
	int pop3,
	int atload
) {
	hostIP = hostip;
	localUser = user;
	password = pass;
	SMTPp = smtp;
	POP3p = pop3;
	autoLoad = atload;
}


MAILCLIENT::MAILCLIENT(std::string IP, int smtp, int pop3) {
	//default values
	hostIP = IP;
	SMTPp = smtp;
	POP3p = pop3;
	localUser = "hcmus.edu.vn";
	password = "123";
	readConfigFromXML();
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
				EMAIL mail;
				if (mail.inputF(_path_mail)) a.addMail(mail);
			}
		}
	}

	//filter file.txt create
	std::string _path_filter = "Mail_client\\" + localUser + "\\" + "filters.txt";
	std::fstream filterF(_path_filter.c_str(), std::ios::app | std::ios::out);
	filterF.close();
}

bool MAILCLIENT::connectSmtp() {
	closesocket(smtpSock);
	smtpSock = socket(AF_INET, SOCK_STREAM, 0);
	if (smtpSock == INVALID_SOCKET) {
		smtpSock = socket(AF_INET, SOCK_STREAM, 0);
	}
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(SMTPp);
	serverAddress.sin_addr.s_addr = inet_addr(hostIP.c_str());

	if (connect(smtpSock, (struct sockaddr*)&serverAddress, sizeof(serverAddress))) {
		std::cout << "\nSERVER SMTP: Failed to connect to SMTP protocol\n";
		closesocket(smtpSock);
		return false;
	};
	char removeFirstLine[20]{};
	recv(smtpSock, removeFirstLine, 20, 0);
	return true;
}

bool MAILCLIENT::connectPop3() {
	closesocket(pop3Sock);
	pop3Sock = socket(AF_INET, SOCK_STREAM, 0);
	if (pop3Sock == INVALID_SOCKET) {
		pop3Sock = socket(AF_INET, SOCK_STREAM, 0);
	}
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(POP3p);
	serverAddress.sin_addr.s_addr = inet_addr(hostIP.c_str());

	if (connect(pop3Sock, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
		std::cout << "\nSERVER POP3: Failed to connect to POP3 protocol\n";
		closesocket(pop3Sock);
		return false;
	};
	char removeFirstLine[20]{};
	recv(pop3Sock, removeFirstLine, 20, 0);
	return true;
}

void MAILCLIENT::disconnect(SOCKET& socket) {
	send(socket, "QUIT\r\n", sizeof("QUIT\r\n") - 1, 0);
}

bool createFolder(const char* str) {
	return !_mkdir(str);
}

void MAILCLIENT::readConfigFromXML() {
	pugi::xml_document doc;
	std::string path;
	path = "Mail_Client/config.xml";
	if (doc.load_file(path.c_str())) {
		pugi::xml_node userNode = doc.child("config_user");
		std::string localUser = userNode.child("localUser").child_value();
		std::string password = userNode.child("password").child_value();
		std::string ip = userNode.child("ip").child_value();
		int smtp = userNode.child("smtp").text().as_int();
		int pop3 = userNode.child("pop3").text().as_int();
		int autoload = userNode.child("autoload").text().as_int();

		std::cout << "Applied config file:\n";
		std::cout << "----------------------" << std::endl;
		std::cout << "Email: " << localUser << std::endl;
		std::cout << "Password: " << password << std::endl;
		std::cout << "IP: " << ip << std::endl;
		std::cout << "SMTP Port: " << smtp << std::endl;
		std::cout << "POP3 Port: " << pop3 << std::endl;
		std::cout << "Autoload Time: " << autoload << std::endl;
		std::cout << "----------------------" << std::endl;

		configClient(ip, localUser, password, smtp, pop3, autoload);
	}
	else {
		std::cout << "Failed to load XML file: " << path << std::endl;
	}
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
	std::cout << "--------------------------------------------------------------------------------\n";
	std::cout << "                                  MAIL-CLIENT\n";
	std::cout << "--------------------------------------------------------------------------------\n";
	std::cout << "Hello, <" << localUser << ">\n";
	std::cout << "----------------------------\n";
	std::cout << "[MENU]:\n";
	std::cout << "[1]. Send email\n";
	std::cout << "[2]. Read email\n";
	std::cout << "[3]. Filter\n";
	std::cout << "[0]. Quit\n\n";
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
	if (buffer != "CC: ") buffer.pop_back();
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
	//sending
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
		if (chr != '\r') result.push_back(chr);
		recv(socket, &chr, 1, 0);
	}
	result.push_back('\0');
	return result;
}

void MAILCLIENT::updateInboxMail() {
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
	SEND = "PASS " + password + "\r\n";
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
		//get body of email
		int nLines = atoi(buffer[6].substr(buffer[6].find(':') + 2).c_str());
		for (int i = 0; i < nLines + 2; i++) {
			buffer.push_back(_getline(pop3Sock));
		}

		//create email
		EMAIL mail(buffer);

		//path to mail
		std::string _path_mail = _path + "\\mail_" + std::to_string(im);
		mail.keyMap = im;
		_mkdir(_path_mail.c_str());

		//file extensions things
		while (true) {
			std::string _line = _getline(pop3Sock);
			_line.pop_back();
			if (_line.size() == 0) {
				_line = _getline(pop3Sock);
				if (_line.find("--END") != -1) break;
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

		filterMail(mail, localUser);

		//update folders
		for (auto& a : folders) {
			a.updateMails(localUser);
		}
	}
	disconnect(pop3Sock);
}

int MAILCLIENT::readMail() {
__back_folders:
__back_inputFolders:
	std::cout << "--------------------------------------------------------------------------------\n";
	std::cout << "This is list of folders in your mailbox:\n";
	for (int i = 0; i < folders.size(); i++) {
		std::cout << '[' << i + 1 << ']' << ". " << folders[i].name << '\n';
	}
	//std::cin.ignore();
	std::string strFolder; int iFolder = 0;

	std::cout << "Which folder do you want to read [ENTER to cancel]: ";
	std::getline(std::cin, strFolder);
	if (strFolder == "") return 0;
	else iFolder = atoi(strFolder.c_str());
	while (iFolder <= 0 || iFolder > folders.size()) {
		std::cout << "Invalid input. Again [ENTER to cancel]: ";
		std::getline(std::cin, strFolder);
		if (strFolder == "") return 1;
		else iFolder = atoi(strFolder.c_str());
	}
	if (folders[iFolder - 1].mails.size() == 0) {
		std::cout << "--------------------------------------------------------------------------------\n";
		std::cout << "Empty.\n";
		goto __back_inputFolders;
	}
	else {
		std::cout << "--------------------------------------------------------------------------------\n";
		std::cout << "This is list of mails in " << folders[iFolder - 1].name << " folder:\n";

		int n = folders[iFolder - 1].mails.size();
		for (int i = 1; i <= n; i++) {

			folders[iFolder - 1].mails[i - 1].subShow(i);
		}
		int iEmail = 0;
		std::string strEmail;
		std::cout << "Which email do you want to read [ENTER to cancel]: ";
		std::getline(std::cin, strEmail);
		if (strEmail == "") {
			goto __back_folders;
		}
		else iEmail = atoi(strEmail.c_str());
		while (iEmail <= 0 || iEmail > n) {
			std::cout << "Invalid input. Again [ENTER to cancel]: ";
			std::getline(std::cin, strEmail);
			if (strEmail == "") {
				goto __back_folders;
			}
			else iEmail = atoi(strEmail.c_str());
		}
		folders[iFolder - 1].mails[iEmail - 1].isRead = 1;
		if (folders[iFolder - 1].mails[iEmail - 1].show()) {
			std::cout << "Do you want to save attach files?\n[1] YES [0] NO . Input: ";
			int ans = 0;
			std::cin >> ans;
			if (ans) {
				std::cout << "Which file?\n[\"ALL\" to select all files]\n[\"x y z..\" to select file x, y, z]: ";
				std::string ansStr;
				std::cin.ignore();
				std::getline(std::cin, ansStr);
				std::string __path_to_mail = "Mail_Client\\" + localUser + "\\" + folders[iFolder - 1].name
					+ "\\" + "mail_" + std::to_string(folders[iFolder - 1].mails[iEmail - 1].keyMap)
					+ "\\";
				std::cout << "Where do you want to save: ";
				std::string _path_save;
				std::getline(std::cin, _path_save);
				if (ansStr == "ALL") {
					for (const auto& _x : folders[iFolder - 1].mails[iEmail - 1].attachFiles) {
						std::string source = __path_to_mail + _x.fileName;
						std::string destiny = _path_save + "\\" + _x.fileName;
						if (copyFile(source, destiny)) {
							std::cout << "Saved " << _x.fileName << ".\n";
						}
						else std::cout << "Failed to save " << _x.fileName << ".\n";
					}
				}
				else {
					std::stringstream _ss{ ansStr };
					std::string _tmp;
					while (_ss >> _tmp) {
						int key = atoi(_tmp.c_str()) - 1;
						if (key < 0 || key >= folders[iFolder - 1].mails[iEmail - 1].attachFiles.size()) {
							std::cout << "Can not read [" << _tmp << "] file" << ".\n";
						}
						else {
							std::string source = __path_to_mail + folders[iFolder - 1].mails[iEmail - 1].attachFiles[key].fileName;
							std::string destiny = _path_save + "\\" + folders[iFolder - 1].mails[iEmail - 1].attachFiles[key].fileName;
							if (copyFile(source, destiny)) {
								std::cout << "Saved " << folders[iFolder - 1].mails[iEmail - 1].attachFiles[key].fileName << ".\n";
							}
							else std::cout << "Failed to save " << folders[iFolder - 1].mails[iEmail - 1].attachFiles[key].fileName << ".\n";
						}
					}
				}
			}
		}
		folders[iFolder - 1].saveLocal(localUser);
	}


	system("pause");
}

int viewFuncFilterMail() {
	std::cout << "--------------------------------------------------------------------------------\n";
	std::cout << "[1]. New filter\n";
	std::cout << "[2]. See all filters\n";
	std::string ans;
	int cmdLine = 0;
	std::cout << "Your choice [ENTER to go back]: ";
	std::getline(std::cin, ans);
	if (ans == "") {
		return 0;
	}
	else cmdLine = atoi(ans.c_str());
	while (cmdLine <= 0 || cmdLine > 2) {
		std::cout << "Invalid input. Again [ENTER to go back]: ";
		std::getline(std::cin, ans);
		if (ans == "") {
			return 0;
		}
		else cmdLine = atoi(ans.c_str());
	}
	switch (cmdLine) {
	case 1: {
		std::cout << "--------------------------------------------------------------------------------\n";
		std::cout << "New filter email:\n";
		std::cout << "[1]. By sender's address\n";
		std::cout << "[2]. By subject\n";
		std::cout << "[3]. By content\n";
		std::cout << "[4]. To spam\n";
		std::cout << "Type: ";
		std::cin >> cmdLine;
		while (cmdLine <= 0 || cmdLine > 4) {
			std::cout << "Invalid input. Again: ";
			std::cin >> cmdLine;
		}
		return (100 + cmdLine);
	}break;
	case 2: {
		//print all filters - ask to delete filter?

		return 200;
	}break;
	}

}

bool newFilterMail(int cmdLine, const std::string& user, const std::vector<MAILFOLDER>& folders) {
	if (cmdLine / 100 != 1) return false;
	std::cout << "------------------------------\n";
	std::string _path_filter = "Mail_client\\" + user + "\\" + "filters.txt";
	std::fstream filterFile(_path_filter.c_str(), std::ios::app | std::ios::out);
	cmdLine %= 10;
	switch (cmdLine) {
	case 1: {
		std::cin.ignore();
		std::cout << "From: [Enter all senders | separated by ','\n";
		std::string inputStr;
		std::getline(std::cin, inputStr);
		std::vector<std::string> senders = splitEmails(inputStr);

		std::cout << "To folder: [Please enter an existed folder]\n";
		std::string folderTo;
		std::getline(std::cin, folderTo);
		bool isExisted = false;
		for (const auto& x : folders) {
			if (x.name == folderTo) {
				isExisted = true; break;
			}
		}
		if (!isExisted) {
			std::cout << "[ERROR]: Not an existed folder\n";
			return false;
		}
		//write to filter.txt
		filterFile << "From:\t";
		for (int i = 0; i < senders.size() - 1; i++) {
			filterFile << senders[i] << ", ";
		}
		filterFile << senders[senders.size() - 1] << "\tTo:\t" << folderTo << "\n";
		filterFile.close();
	} break;
	case 2: {
		std::cin.ignore();
		std::cout << "Subject's keyword: [Enter all keywords | separated by 'SPACE'\n";
		std::vector<std::string> keys;
		std::string inputStr;
		std::getline(std::cin, inputStr);
		std::stringstream ss{ inputStr };
		while (ss >> inputStr) keys.push_back(inputStr);
		std::cout << "To folder: [Please enter an existed folder]\n";
		std::string folderTo;
		bool isExisted = false;
		for (const auto& x : folders) {
			if (x.name == folderTo) {
				isExisted = true; break;
			}
		}
		if (!isExisted) {
			std::cout << "[ERROR]: Not an existed folder\n";
			return false;
		}
		filterFile << "Subject:\t";
		for (int i = 0; i < keys.size() - 1; i++) {
			filterFile << keys[i] << ' ';
		}
		filterFile << keys[keys.size() - 1] << "\tTo:\t" << folderTo << "\t\n";
		filterFile.close();
	} break;
	case 3: {
		std::cin.ignore();
		std::cout << "Content's keyword: [Enter all keywords | separated by 'SPACE'\n";
		std::vector<std::string> keys;
		std::string inputStr;
		std::getline(std::cin, inputStr);
		std::stringstream ss{ inputStr };
		while (ss >> inputStr) keys.push_back(inputStr);
		std::cout << "To folder: [Please enter an existed folder]\n";
		std::string folderTo;
		std::getline(std::cin, folderTo);
		bool isExisted = false;
		for (const auto& x : folders) {
			if (x.name == folderTo) {
				isExisted = true; break;
			}
		}
		if (!isExisted) {
			std::cout << "[ERROR]: Not an existed folder\n";
			return false;
		}
		filterFile << "Content:\t";
		for (int i = 0; i < keys.size() - 1; i++) {
			filterFile << keys[i] << ' ';
		}
		filterFile << keys[keys.size() - 1] << "\tTo:\t" << folderTo << "\n";
		filterFile.close();
	} break;
	case 4: {
		std::cin.ignore();
		std::cout << "SPAM's keyword: [Enter all keywords | separated by 'SPACE'\n";
		std::vector<std::string> keys;
		std::string inputStr;
		std::getline(std::cin, inputStr);
		std::stringstream ss{ inputStr };
		while (ss >> inputStr) keys.push_back(inputStr);
		filterFile << "Spam:\t";
		for (int i = 0; i < keys.size() - 1; i++) {
			filterFile << keys[i] << ' ';
		}
		filterFile << keys[keys.size() - 1] << "\tTo:\tSpam\n";
		filterFile.close();
	} break;
	}
	return true;
}

bool printFilters(const std::string& user) {
	int iMap = 1;
	bool isEmpty = 1;
	std::string _path_filter = "Mail_client\\" + user + "\\" + "filters.txt";
	std::fstream filterFile(_path_filter.c_str(), std::ios::in);
	std::string temp;
	while (std::getline(filterFile, temp)) {
		isEmpty = 0;
		std::cout << "--------------------------------------------------------------------------------\n";
		std::cout << "Filter[" << iMap++ << "]\n";
		std::stringstream ss{ temp };
		std::string _temp;
		std::getline(ss, _temp, '\t');
		if (_temp == "From:") {
			std::cout << "From senders's address:\n";
			std::getline(ss, _temp, '\t');
			std::stringstream _ss{ _temp };
			std::string _temp_;
			while (_ss >> _temp_) {
				if (_temp_[_temp_.size() - 1] == ',') _temp_.pop_back();
				std::cout << '<' << _temp_ << '>' << " ";
			}std::cout << "\n";
			std::getline(ss, _temp, '\t');
			std::getline(ss, _temp, '\t');
			std::cout << "To folder: " << _temp << '\n';
		}
		else if (_temp == "Subject:") {
			std::cout << "From subject has keywords:\n";
			std::getline(ss, _temp, '\t');
			std::stringstream _ss{ _temp };
			std::string _temp_;
			while (_ss >> _temp_) {
				if (_temp_[_temp_.size() - 1] == ',') _temp_.pop_back();
				std::cout << '\"' << _temp_ << '\"' << " ";
			}std::cout << "\n";
			std::getline(ss, _temp, '\t');
			std::getline(ss, _temp, '\t');
			std::cout << "To folder: " << _temp << '\n';
		}
		else if (_temp == "Content:") {
			std::cout << "From content has keywords:\n";
			std::getline(ss, _temp, '\t');
			std::stringstream _ss{ _temp };
			std::string _temp_;
			while (_ss >> _temp_) {
				if (_temp_[_temp_.size() - 1] == ',') _temp_.pop_back();
				std::cout << '\"' << _temp_ << '\"' << " ";
			}std::cout << "\n";
			std::getline(ss, _temp, '\t');
			std::getline(ss, _temp, '\t');
			std::cout << "To folder: " << _temp << '\n';
		}
		else if (_temp == "Spam:") {
			std::cout << "From subject, content have keywords To Spam:\n";
			std::getline(ss, _temp, '\t');
			std::stringstream _ss{ _temp };
			std::string _temp_;
			while (_ss >> _temp_) {
				if (_temp_[_temp_.size() - 1] == ',') _temp_.pop_back();
				std::cout << '\"' << _temp_ << '\"' << " ";
			}std::cout << "\n";
		}
	}
	if (isEmpty) std::cout << "There's no filters\n";
	std::cout << "--------------------------------------------------------------------------------\n";
	filterFile.close();
	return !isEmpty;
}

void delFilter(const std::string& user) {
	int _count = 0;
	std::string _path_filter = "Mail_client\\" + user + "\\" + "filters.txt", temp;
	std::fstream filterFileI(_path_filter.c_str(), std::ios::in);
	std::vector<std::string> lines;
	while (std::getline(filterFileI, temp)) {
		lines.push_back(temp);
		_count++;
	}
	filterFileI.close();

	if (_count == 0) return;

	std::cout << "Which filter [ENTER number]: ";
	int imap = 0;
	std::cin >> imap;
	while (imap <= 0 || imap > _count) {
		std::cout << "Not found. Retry: ";
		std::cin >> imap;
	}
	lines.erase(lines.begin() + imap - 1);

	std::fstream filterFileO(_path_filter.c_str(), std::ios::out | std::ios::trunc);
	for (const auto& x : lines) {
		filterFileO << x << '\n';
	}
	filterFileO.close();
}

void MAILCLIENT::filterMail(EMAIL& email, const std::string& user) {
	std::string _path_filter = "Mail_client\\" + user + "\\" + "filters.txt";
	std::fstream filterFile(_path_filter.c_str(), std::ios::in);
	std::string line;
	bool isChecked = false;
	//get to destination
	while (std::getline(filterFile, line)) {
		std::stringstream ss{ line };
		std::getline(ss, line, '\t');
		if (line == "From:") {
			std::getline(ss, line, '\t');
			std::vector<std::string> senders = splitEmails(line);

			for (const auto& x : senders) {
				if (x == email.sender) {
					isChecked = true; break;
				}
			}
			std::getline(ss, line, '\t');
			std::getline(ss, line, '\t');
			if (isChecked) {
				moveEmail(user, line, email.keyMap);
				break;
			}
		}
		else if (line == "Subject:") {
			std::getline(ss, line, '\t');
			std::vector<std::string> keywords;
			std::stringstream keyss{ line };
			while (keyss >> line) keywords.push_back(line);

			for (const auto& x : keywords) {
				if (email.subject.find(x) != -1) {
					isChecked = true; break;
				}
			}
			std::getline(ss, line, '\t');
			std::getline(ss, line, '\t');
			if (isChecked) {
				moveEmail(user, line, email.keyMap);
				break;
			}
		}
		else if (line == "Content:") {
			std::getline(ss, line, '\t');
			std::vector<std::string> keywords;
			std::stringstream keyss{ line };
			while (keyss >> line) keywords.push_back(line);

			for (const auto& x : keywords) {
				for (const auto& y : email.body) {
					if (y.find(x) != -1) {
						isChecked = true; break;
					}
				}
				if (isChecked) break;
			}
			std::getline(ss, line, '\t');
			std::getline(ss, line, '\t');
			if (isChecked) {
				moveEmail(user, line, email.keyMap);
				break;
			}
		}
		else if (line == "Spam:") {
			std::getline(ss, line, '\t');
			std::vector<std::string> keywords;
			std::stringstream keyss{ line };
			while (keyss >> line) keywords.push_back(line);

			for (const auto& x : keywords) {
				if (email.subject.find(x) != -1) {
					isChecked = true; break;
				}
			}

			if (isChecked == 0) {
				for (const auto& x : keywords) {
					for (const auto& y : email.body) {
						if (y.find(x) != -1) {
							isChecked = true; break;
						}
					}
					if (isChecked) break;
				}
			}

			std::getline(ss, line, '\t');
			std::getline(ss, line, '\t');
			if (isChecked) {
				moveEmail(user, line, email.keyMap);
				break;
			}
		}
	}
	//remove email if found
	if (isChecked) email.delEmailinLocal(user);
}

void MAILCLIENT::filterMail(std::vector<EMAIL>& emails, const std::string& user) {
	for (EMAIL& x : emails) {
		filterMail(x, user);
	}
}

void MAILCLIENT::filterMail() {
	int cmdLine = viewFuncFilterMail();
	if (newFilterMail(cmdLine, localUser,folders)) {
		filterMail(folders[0].mails, localUser);

		//update folders
		for (auto& a : folders) {
			a.updateMails(localUser);
		}
	}
	if (cmdLine == 200) {
		if (printFilters(localUser)) {
			bool ans = 0;
			std::cout << "Do you want to delete a filter? Only work for next emails\n[1] YES [0] NO . Input: ";
			std::cin >> ans;
			if (ans) delFilter(localUser);
		}
	}
	system("pause");
}