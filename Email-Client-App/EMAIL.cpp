#include "EMAIL.h"

EMAIL::EMAIL() {
	isRead = 0;
	keyMap = 0;
	key = "";
}

EMAIL::EMAIL(const std::vector<std::string>& buffer) {
	std::string temp = buffer[0].substr(buffer[0].find(':') + 2);
	recvTO = splitEmails(temp);
	temp = buffer[1].substr(buffer[1].find(':') + 2);
	recvCC = splitEmails(temp);
	sender = buffer[2].substr(buffer[2].find(':') + 2);
	subject = buffer[3].substr(buffer[3].find(':') + 2);
	int numLines = atoi(buffer[6].substr(buffer[6].find(':') + 2).c_str());
	for (int i = 8; i <= 8 + numLines - 1; i++) {
		body.push_back(buffer[i]);
	}
	isRead = false;
	attachFiles.resize(0);
	recvBCC.resize(0);
	keyMap = 0;
	//remove last null character
	sender.pop_back();
	if (recvTO.size()) recvTO[recvTO.size() - 1].pop_back();
	if (recvCC.size()) recvCC[recvCC.size() - 1].pop_back();
	if (recvBCC.size()) recvBCC[recvBCC.size() - 1].pop_back();
	key = "";
}

bool EMAIL::show() {
	std::cout << "------------------------------\n";
	std::cout << "To: ";
	if (recvTO.size() == 1) {
		std::cout << '<' << recvTO[0] << "> ";
	}
	else {
		for (const auto& x : recvTO) {
			std::cout << '<' << x << ">, ";
		}
	}
	std::cout << "\b\n";

	std::cout << "CC: ";
	if (recvCC.size() == 1) {
		std::cout << '<' << recvCC[0] << "> ";
	}
	else {
		for (const auto& x : recvCC) {
			std::cout << '<' << x << ">, ";
		}
	}
	std::cout << "\b\n";

	std::cout << "From: " << '<' << sender << '>' << '\n';
	std::cout << "Subject: " << subject << '\n';
	std::cout << "Content: " << '\n';
	std::cout << '\n';
	for (const auto& x : body) {
		std::cout << x << '\n';
	}
	std::cout << '\n';

	if (attachFiles.size() == 0) {
		std::cout << "No file attached.\n";
		std::cout << "------------------------------\n";
		return false;
	}
	else {
		int n = attachFiles.size();
		std::cout << n << " files attached.\n";
		for (int i = 0; i < n; i++) {
			std::cout<<'[' << i + 1 <<']'<< ". " << attachFiles[i].fileName << '\n';
		}
		return true;
	}
}

void EMAIL::subShow(int i) {
	std::cout << '[' << i << ']' << ". ";
	if (isRead == false) std::cout << "<UNREAD> ";
	else std::cout << "< READ > ";
	std::cout << "<" << sender << "> <" << subject << ">\n";
}

void EMAIL::delEmailinLocal(const std::string& user) {
	std::string _path = "Mail_Client\\" + user + "\\Inbox" + "\\mail_" + std::to_string(keyMap);
	std::string _path_content = "Mail_Client\\" + user + "\\Inbox" + "\\mail_" + std::to_string(keyMap) + "\\" + "content.txt";
	std::remove(_path_content.c_str());
	RemoveDirectoryA(_path.c_str());
}

std::vector<std::string> splitEmails(const std::string& emailString) {
	std::istringstream iss(emailString);
	std::vector<std::string> emailAddresses;

	std::string email;
	while (std::getline(iss, email, ',')) {
		size_t start = email.find_first_not_of(" \t");
		size_t end = email.find_last_not_of(" \t");

		if (start != std::string::npos && end != std::string::npos) {
			emailAddresses.push_back(email.substr(start, end - start + 1));
		}
	}

	return emailAddresses;
}

bool EMAIL::input(const std::string& local) {
	sender = local;
	std::cout << "\n . This is the email content: [ ENTER ] to skip if not filled \n . . Type \"To: - \" to cancel writing email\n . . . If there's more than one email address, separate by \',\'";
	std::cout << "\n . . . . In the body, enter continuously and type a separate '.' to end.\n";
	std::cout << "--------------------------------------------------------------------------------";
	std::cout << "\nTo: ";
	std::string temp;
	//std::cin.ignore();
	std::getline(std::cin, temp);
	if (temp == "") {
		moveCursorBackward(-10,1);
		std::cout << " <none>\n";
	}
	if (temp == "-") return false;
	recvTO = splitEmails(temp);
	std::cout << "CC: ";
	std::getline(std::cin, temp);
	if (temp == "") {
		moveCursorBackward(-10,1);
		std::cout << " <none>\n";
	}
	recvCC = splitEmails(temp);
	std::cout << "BCC: ";
	std::getline(std::cin, temp);
	if (temp == "") {
		moveCursorBackward(-10, 1);
		std::cout << " <none>\n";
	}
	recvBCC = splitEmails(temp);

	if (recvTO.size() + recvCC.size() + recvBCC.size() == 0) {
		std::cout << "[ERROR]: No destination found\n";
		std::cout << "[ERROR]: Try again\n";
		return false;
	}

	std::cout << "\nSubject: ";
	std::getline(std::cin, subject);
	if (subject == "") {
		moveCursorBackward(-10,1);
		std::cout << " <no subject>\n";
		subject = "<No subject>";
	}
	std::cout << "\nBody:\n";
	while (std::getline(std::cin, temp)) {
		if (temp == ".") break;
		else body.push_back(temp);
	}
	std::cout << "Attach file?\n[1] YES [0] NO . Input: ";
	bool athF = false;
	std::cin >> athF;
	if (athF) {
		int num = 0;
		std::cout << "Number of files: ";
		std::cin >> num;
		std::cin.ignore();
		for (int i = 1; i <= num; i++) {
			std::cout << "Path of file " << i << ":\n";
			std::string pathAthF;
			std::getline(std::cin, pathAthF);
			if (pathAthF.find('.') == -1) {
				std::cout << "Invalid file path.\n";
				continue;
			}
			std::fstream isExist(pathAthF.c_str(), std::ios::in);
			if (!isExist) {
				std::cout << "Invalid file path.\n";
				isExist.close();
			}
			else {
				//limit size of attach file
				if (getFileSize(pathAthF) > 3145728) {//3MB
					std::cout << "Size limit exceeded.\n";
					isExist.close();
					continue;
				}
				std::string nameFile, extension;
				SplitPath(pathAthF, nameFile, extension);
				Attachment tmpAthF{ pathAthF, nameFile, getFileSize(pathAthF) };
				attachFiles.push_back(tmpAthF);
			}
			isExist.close();
		}
	}
	std::cout << "--------------------------------------------------------------------------------\n";
	std::cout << "SUCCESSFULL !\n";
	std::cout << "--------------------------------------------------------------------------------\n";
	return true;
}

void SplitPath(const std::string& filePath, std::string& fileName, std::string& fileExtension) {
	size_t lastSlash = filePath.find_last_of("/\\");
	size_t lastDot = filePath.find_last_of(".");
	if (lastDot != std::string::npos && lastDot > lastSlash) {
		fileName = filePath.substr((lastSlash != std::string::npos) ? lastSlash + 1 : 0, lastDot - ((lastSlash != std::string::npos) ? lastSlash + 1 : 0));
		fileExtension = filePath.substr(lastDot);
	}
	else {
		fileName = (lastSlash != std::string::npos) ? filePath.substr(lastSlash + 1) : filePath;
		fileExtension = filePath.substr(lastDot);
	}
}

std::streampos getFileSize(const std::string& filePath) {
	std::ifstream file(filePath, std::ios::binary | std::ios::ate);
	if (!file.is_open()) {
		return -1;
	}
	std::streampos fileSize = file.tellg();
	file.close();
	return fileSize;
}

bool EMAIL::inputF(const std::string& file) {
	std::fstream fileOpen(file.c_str(), std::ios::in);
	keyMap = atoi(file.substr(file.find("\\mail_") + 6, file.find("\\content.txt") - 5).c_str());
	if (fileOpen.is_open() == 0) {
		fileOpen.close();
		return false;
	}
	fileOpen >> isRead;
	fileOpen.ignore();
	std::getline(fileOpen, key);
	std::getline(fileOpen, sender);
	std::string temp;
	std::getline(fileOpen, temp);
	std::stringstream ss{ temp };
	recvTO.resize(0);
	while (std::getline(ss, temp, '\t')) recvTO.push_back(temp);

	std::getline(fileOpen, temp);
	std::stringstream ss2{ temp };
	recvCC.resize(0);
	while (std::getline(ss, temp)) recvCC.push_back(temp);

	std::getline(fileOpen, temp);
	std::stringstream ss3{ temp };
	recvBCC.resize(0);
	while (std::getline(ss3, temp)) recvBCC.push_back(temp);

	std::getline(fileOpen, subject);
	int nLines = 0;
	fileOpen >> nLines;
	fileOpen.ignore();
	body.resize(0);
	for (int i = 0; i < nLines; i++) {
		std::getline(fileOpen, temp);
		body.push_back(temp);
	}
	attachFiles.resize(0);
	while (std::getline(fileOpen, temp)) {
		attachFiles.push_back(Attachment{ "N/A", temp });
	}
	fileOpen.close();
	return true;
}

void EMAIL::outputF(const std::string& file) {
	std::fstream fileOpen(file.c_str(), std::ios::out | std::ios::trunc);
	if (!fileOpen.is_open()) {
		std::cout << file << "\n";
		system("pause");
		fileOpen.close();
		return;
	}
	fileOpen << isRead << '\n';
	fileOpen << key << '\n';
	fileOpen << sender << '\n';
	for (const auto& x : recvTO) fileOpen << x << '\t';
	fileOpen << '\n';
	for (const auto& x : recvCC) fileOpen << x << '\t';
	fileOpen << '\n';
	for (const auto& x : recvBCC) fileOpen << x << '\t';
	fileOpen << '\n';
	fileOpen << subject << '\n';
	fileOpen << body.size() << '\n';
	for (const auto& x : body) fileOpen << x << '\n';
	for (const auto& x : attachFiles) fileOpen << x.fileName << '\n';
	fileOpen.close();
}


void extractNameAndNumber(const std::string& input, std::string& name, int& number) {
	std::istringstream iss(input);
	std::string token;
	std::getline(iss, token, '=');
	std::getline(iss, token, ',');
	size_t start = token.find_first_not_of(" ");
	size_t end = token.find_last_not_of(" ");
	name = token.substr(start, end - start + 1);
	std::getline(iss, token, ',');
	std::getline(iss, token);
	std::istringstream numberStream(token);
	numberStream >> number;
}

bool copyFile(const std::string& sourcePath, const std::string& destinationPath) {
	std::ifstream sourceFile(sourcePath, std::ios::binary);
	std::ofstream destinationFile(destinationPath, std::ios::binary);

	if (sourceFile.is_open() && destinationFile.is_open()) {
		sourceFile.seekg(0, std::ios::end);
		std::streamsize fileSize = sourceFile.tellg();
		sourceFile.seekg(0, std::ios::beg);

		char* buffer = new char[fileSize];

		sourceFile.read(buffer, fileSize);
		destinationFile.write(buffer, fileSize);

		delete[] buffer;

		sourceFile.close();
		destinationFile.close();

		return true;
	}
	else {
		return false;
	}
}

void moveFolder(const char* oldPath, const char* newPath) {
	int result = rename(oldPath, newPath);
	if (result == 1) {
		perror("Error while moving email\n");
	}
}

void tmpCopyEmail(const std::string& user, int iMap) {
	std::string _path = "Mail_Client\\" + user + "\\Inbox\\mail_" + std::to_string(iMap);
	std::string tmpPath = "Mail_Client\\" + user + "\\Inbox\\tmp";
	_mkdir(tmpPath.c_str());
	//copy content.txt
	std::string contentDestinationPath = tmpPath + "\\content.txt";
	std::string contentSourcePath = _path + "\\content.txt";
	copyFile(contentSourcePath, contentDestinationPath);
}

void moveEmail(const std::string& user, const std::string& folder, int keyMap) {
	tmpCopyEmail(user, keyMap);
	std::string _path_source = "Mail_Client\\" + user + "\\Inbox\\tmp";
	std::string _path_destination = "Mail_Client\\" + user + "\\" + folder;
	std::string _path_destination_count = "Mail_Client\\" + user + "\\" + folder + "\\count.txt";
	int nMail = 0;
	std::fstream countGet(_path_destination_count.c_str(), std::ios::in);
	countGet >> nMail;
	countGet.close();
	nMail++;
	std::fstream countSet(_path_destination_count.c_str(), std::ios::out | std::ios::trunc);
	countSet << nMail;
	countSet.close();
	_path_destination += "\\";
	_path_destination += "mail_" + std::to_string(nMail);
	moveFolder(_path_source.c_str(), _path_destination.c_str());
}

void moveCursorBackward(int Xsteps, int Ysteps) {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

	COORD newCoord;
	newCoord.X = csbi.dwCursorPosition.X - Xsteps;
	newCoord.Y = csbi.dwCursorPosition.Y - Ysteps;

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), newCoord);
}