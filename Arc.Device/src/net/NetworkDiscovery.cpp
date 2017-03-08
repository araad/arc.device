#include "NetworkDiscovery.h"
#include "../utils/LogManager.h"
#include <string>
#include "../core/TaskManager.h"
#include "MbedJSONValue.h"
#include <vector>
#include <algorithm>

#include "platform\mbed_stats.h"
mbed_stats_heap_t heap_stats;

arc::device::net::NetworkDiscovery::NetworkDiscovery(ESP8266Interface* esp, Callback<void(char*, char*)> discoverComplete)
	: espThread(osPriorityNormal, 512 * 5)
{
	Logger.Trace("NetworkDiscovery - ctor()");

	this->esp = esp;
	this->discoverComplete = &discoverComplete;

	Logger.Trace("starting up as AP...");
	esp->startup(3);
	scanNetworks();
	Logger.Trace("configuring...");
	esp->configureSoftAP("ARC_Discovery");
	Logger.Trace("starting server...");
	esp->startServer();
	Logger.Trace("Server started");

	espThread.start(callback(this, &NetworkDiscovery::listen));
}

arc::device::net::NetworkDiscovery::~NetworkDiscovery()
{
	Logger.Trace("NetworkDiscovery - dtor()");
	espThread.terminate();

	bool closed = esp->close(0);
	Logger.Trace("closed: %d", closed);

	Logger.Trace("stopping server...");
	esp->stopServer();
}

bool sortAp(WiFiAccessPoint a, WiFiAccessPoint b)
{
	int a_rssi = a.get_rssi();
	int b_rssi = b.get_rssi();
	return a_rssi > b_rssi;
}

void arc::device::net::NetworkDiscovery::scanNetworks()
{
	Logger.Trace("Getting AP List...");

	apCount = esp->scan(apList, apCount);
	std::sort(apList, apList + apCount, sortAp);

	apCount = min(11, apCount);
	apToSend = apCount;
	apResponseCount = max(1, (int)ceil((float)apCount / (float)maxApPerResponse));
}

void arc::device::net::NetworkDiscovery::listen()
{
	string name("ESP");
	Logger.mapThreadName((char*)name.c_str());
	Logger.Trace("Receiving...");

	while (1) {
		char req[450];
		string request;
		int length = esp->recv(0, req, 450);
		Logger.Trace("received: %d", length);
		request = "";
		if (length > 0)
		{
			request = req;
		}

		if (request.find("GET") == 0)
		{
			sendAPList();
		}
		else if (request.find("OPTIONS") == 0)
		{
			receiveAPCredentials(request);
		}
	}
}

void arc::device::net::NetworkDiscovery::sendAPList()
{
	if (currentResponseIndex < apResponseCount)
	{
		int currentApIndex = (currentResponseIndex)*maxApPerResponse;
		int limit = currentApIndex + maxApPerResponse;
		if (apToSend < maxApPerResponse)
		{
			limit = currentApIndex + apToSend;
		}
		MbedJSONValue data;
		MbedJSONValue jObj;
		data["more"] = limit < apCount;

		for (int i = currentApIndex, j = 0; i < limit; i++, j++) {
			jObj["ssid"] = apList[i].get_ssid();
			jObj["signal"] = apList[i].get_rssi();
			char mac[18];
			const uint8_t* bssid = apList[i].get_bssid();
			sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x", bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
			jObj["mac"] = mac;
			jObj["security"] = apList[i].get_security();
			data["result"][j] = jObj;
		}

		Logger.Trace("Sending AP List...");

		string response("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nContent-Type: application/json\r\nContent-Length: ");
		string dataStr = data.serialize();
		char size_buf[5];
		itoa(dataStr.length(), size_buf, 10);
		response.append(size_buf);
		response.append("\r\n\r\n");
		response.append(dataStr.c_str());
		esp->send(0, response.c_str(), response.length());

		apToSend = apCount - (++currentResponseIndex*maxApPerResponse);
	}
}

void arc::device::net::NetworkDiscovery::receiveAPCredentials(string request)
{
	Logger.Trace("Receiving AP credentials...");
	char ssid[50];
	char pswd[100];

	Logger.Trace("request: %s", request.c_str());
	sscanf((char*)request.c_str(), "OPTIONS /?ssid=%[^&]&pswd=%s HTTP/1.1", ssid, pswd);
	Logger.Trace("NetworkDiscovery - receiveAPCredentials() ssid: %s pswd: %s", ssid, pswd);

	Logger.Trace("sending response...");
	string response("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nContent-Length: 0\r\n\r\n");
	esp->send(0, response.c_str(), response.length());

	Logger.Trace("Adding one time task...");
	discoverComplete->call(ssid, pswd);
	//Tasks->AddOneTimeTask(*discoverComplete, ssid, pswd);
}
