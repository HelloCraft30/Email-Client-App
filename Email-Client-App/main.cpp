#include<iostream>
#include <thread>
#include <chrono>
#include "MAILCLIENT.h"

MAILCLIENT client("127.0.0.1", 2225, 3335);

void threadFunction() {
	while (true) {
		client.updateInboxMail();
		std::this_thread::sleep_for(std::chrono::seconds(10));
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

	//mail client [IP, SMTP port, POP3 port]
	client.checkConnection();

	while (true) {
		switch (client.viewFunction()) {
		case 1: {
			EMAIL tmp; tmp.input(client.getUser());
			client.sendMail(tmp);
			system("pause");
		} break;
		case 2: {
			client.readMail();	
		} break;
		case 3: {
			client.updateInboxMail();
		}break;
		case 4: {
			client.filterMails(client.getUser());
		} break;
		}
	}

	updateThread.join();
	WSACleanup();
	return 0;
}
