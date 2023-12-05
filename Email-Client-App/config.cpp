#include "config.h"
#include <iostream>

void writeConfigToXML(MAILCLIENT& client) {
    pugi::xml_document doc;
    pugi::xml_node userNode = doc.append_child("config_user");

    userNode.append_child("localUser").text().set(client.getLocalUser().c_str());

    userNode.append_child("password").text().set(client.getPassword().c_str());
    userNode.append_child("ip").text().set(client.getHostIp().c_str());
    userNode.append_child("smtp").text().set(client.getSMTPp());
    userNode.append_child("pop3").text().set(client.getPOP3p());
    userNode.append_child("autoload").text().set(client.getAutoload());

    std::string path;
    path = "Mail_Client/" + client.getLocalUser() + ".xml";
    if (doc.save_file(path.c_str()))
        std::cout << "Configuration saved to " << path << "\n";
    else
        std::cerr << "Failed to save XML file.\n";
}

void readConfigFromXML(MAILCLIENT& client) {
    pugi::xml_document doc;

    std::string path;
    path = "Mail_Client/" + client.getLocalUser() + ".xml";
    if (doc.load_file(path.c_str())) {
        pugi::xml_node userNode = doc.child("config_user");
        std::string localUser = userNode.child("localUser").child_value();
        std::string password = userNode.child("password").child_value();
        std::string ip = userNode.child("ip").child_value();
        int smtp = userNode.child("smtp").text().as_int();
        int pop3 = userNode.child("pop3").text().as_int();
        int autoload = userNode.child("autoload").text().as_int();

        std::cout << "Email: " << localUser << std::endl;
        std::cout << "Password: " << password << std::endl;
        std::cout << "IP: " << ip << std::endl;
        std::cout << "SMTP Port: " << smtp << std::endl;
        std::cout << "POP3 Port: " << pop3 << std::endl;
        std::cout << "Autoload Time: " << autoload << std::endl;
        std::cout << "----------------------" << std::endl;

        client.configClient(ip, localUser, password, smtp, pop3, autoload);
    }
    else {
        std::cout << "Failed to load XML file: " << path << std::endl;
    }
}
