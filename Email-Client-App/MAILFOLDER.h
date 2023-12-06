#pragma once
#include "EMAIL.h"
#include <fstream>

class MAILFOLDER{
private:
public:
	std::string name;
	std::vector<EMAIL> mails;
	bool makeSpace(const std::string& user);
	MAILFOLDER();
	MAILFOLDER(const std::string& _name);
	void addMail(const EMAIL& mail);
	void saveLocal(const std::string& user);
	
	void updateMails(const std::string& user);
};

