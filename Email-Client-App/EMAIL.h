#pragma once
#include<string>
#include<vector>
#include<sstream>
#include<iostream>
#include<fstream>
#include<map>
#include <thread>
#include <chrono>
#include <direct.h>

struct Attachment {
	std::string filePath;
	std::string fileName;
	int nBytes;
};

class EMAIL{
public:
	std::string subject;
	std::string sender;
	std::vector<std::string> recvTO;
	std::vector<std::string> recvCC;
	std::vector<std::string> recvBCC;
	std::vector<std::string> body;
	bool isRead;
	std::vector<Attachment> attachFiles;
	EMAIL();
	EMAIL(const std::vector<std::string>& buffer);
	void show();
	void input(const std::string& local);
	bool inputF(const std::string& file);
	void outputF(const std::string& file);
	void subShow(int i);
};

//Tach thanh cac email
//vd: "mail_1.com, mail_2.com" => {mail_1.com, mail_2.com}
std::vector<std::string> splitEmails(const std::string& emailString);

//Tach 1 path thanh fileName va extension
void SplitPath(const std::string& fullPath, std::string& fileName, std::string& extension);

//Tra ve kich thuoc cua file theo duong dan
std::streampos getFileSize(const std::string& filePath);

//tach "Name = test.docx,.docx,12109" thanh test.docx va 12109
void extractNameAndNumber(const std::string& input, std::string& name, int& number);
