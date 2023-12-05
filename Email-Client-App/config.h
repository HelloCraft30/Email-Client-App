#pragma once
#include <iostream>
#include <string>
#include "pugixml/src/pugixml.hpp"
#include "MAILCLIENT.h"

void writeConfigToXML(MAILCLIENT& client);
void readConfigFromXML(MAILCLIENT& client);
