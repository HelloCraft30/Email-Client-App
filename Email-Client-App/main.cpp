#include<iostream>
#include <thread>
#include <chrono>
#include "MAILCLIENT.h"

MAILCLIENT client("127.0.0.1", 2225, 3335);

void threadFunction() {
	static int delay = client.getAutoload();
	while (true) {
		client.updateInboxMail();
		std::this_thread::sleep_for(std::chrono::seconds(delay));
	}
}

int main() {
	std::thread updateThread(threadFunction);
	// Initialize Winsock
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cout << "Failed to initialize Winsock.\n";
		return -1;
	}
	system("pause");
	//mail client [IP, SMTP port, POP3 port]
	//client.checkConnection();

	while (true) {
		switch (client.viewFunction()) {
		case 0: {
			exit(0);
		} break;
		case 1: {
			EMAIL tmp; 
			if(tmp.input(client.getLocalUser())) client.sendMail(tmp);
			system("pause");
		} break;
		case 2: {
			client.readMail();
		} break;
		case 3: {
			client.filterMail();
		} break;
		}
	}

	updateThread.join();
	WSACleanup();
	return 0;
}
