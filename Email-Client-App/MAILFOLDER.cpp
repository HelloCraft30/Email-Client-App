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
	std::string tmp = "Mail_client\\" + user + "\\" + name;
	if (_mkdir(tmp.c_str())) return false;
	tmp += "\\count.TXT";
	std::fstream createFile(tmp.c_str(), std::ios::out | std::ios::trunc);
	createFile << 0 << std::endl;
	createFile.close();
	return true;
}

void MAILFOLDER::saveLocal(const std::string& user) {
	int n = mails.size();
	for (int i = 0; i < n; i++) {
		std::string _path = "Mail_client\\" + user + "\\" + name + "\\mail_" + std::to_string(mails[i].keyMap) + "\\content.txt";
		mails[i].outputF(_path);
	}
}

void MAILFOLDER::updateMails(const std::string& user) {
	mails.resize(0);
	std::string _path = "Mail_client\\" + user + "\\" + name + "\\count.txt";
	std::fstream _nMailf(_path.c_str(), std::ios::in);
	int nMail = 0; _nMailf >> nMail; _nMailf.close();
	for (int i = 1; i <= nMail; i++) {
		std::string _path_mail = "Mail_client\\" + user + "\\" + name + "\\mail_" + std::to_string(i) + "\\content.txt";
		EMAIL mail;
		if (mail.inputF(_path_mail)) addMail(mail);
	}
}