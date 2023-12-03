#include "MAILFOLDER.h"

MAILFOLDER::MAILFOLDER() {
	mails.resize(0);
	name = "";
}

MAILFOLDER::MAILFOLDER(const std::string& _name) {
	mails.resize(0);
	name = _name;
}

void MAILFOLDER::addMail(const EMAIL& mail) {
	mails.push_back(mail);
}

bool MAILFOLDER::makeSpace(const std::string& user) {
	std::string tmp = "Mail_client\\" + user+"\\"+name;
	if (_mkdir(tmp.c_str())) return false;
	tmp += "\\count.TXT";
	std::fstream createFile(tmp.c_str(), std::ios::out | std::ios::trunc);
	createFile << 0<<std::endl;
	createFile.close();
	return true;
}