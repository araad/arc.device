#include "NetworkDiscovery.h"
#include "../utils/LogManager.h"
#include <string>
#include "features\netsocket\WiFiAccessPoint.h"
#include "../core/TaskManager.h"

arc::device::net::NetworkDiscovery::NetworkDiscovery(ESP8266Interface* esp, Callback<void(char*, char*)> discoverComplete)
{
	Logger.Trace("NetworkDiscovery - ctor()");

	this->esp = esp;
	this->discoverComplete = &discoverComplete;

	Logger.Trace("starting up as AP...");
	esp->startup(3);
	Logger.Trace("configuring...");
	esp->configureSoftAP("ARC_Discovery");
	Logger.Trace("starting server...");
	esp->startServer();
	Logger.Trace("Server started");

	espThread = new Thread(osPriorityNormal, 4096);
	espThread->start(callback(this, &NetworkDiscovery::listen));
}

arc::device::net::NetworkDiscovery::~NetworkDiscovery()
{
	Logger.Trace("NetworkDiscovery - dtor()");
	espThread->terminate();
	delete espThread;

	bool closed = esp->close(0);
	Logger.Trace("closed: %d", closed);

	Logger.Trace("stopping server...");
	esp->stopServer();
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
		Logger.Trace("received: %s", req);
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
	Logger.Trace("Getting AP List...");

	WiFiAccessPoint *ap;
	int count = 30;

	ap = new WiFiAccessPoint[count];
	count = esp->scan(ap, count);

	string data("{\"result\":[");
	for (int i = 0; i < count; i++) {
		data.append("\"");
		data.append(ap[i].get_ssid());
		data.append("\"");
		if (i == (count - 1))
		{
			data.append("]}");
		}
		else
		{
			data.append(",");
		}
	}

	Logger.Trace("Sending AP List...");

	string response("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nContent-Type: application/json\r\nContent-Length: ");
	char size_buf[5];
	itoa(data.length(), size_buf, 10);
	response.append(size_buf);
	response.append("\r\n\r\n");
	response.append(data.c_str());
	esp->send(0, response.c_str(), response.length());
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
	string response("HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n");
	esp->send(0, response.c_str(), response.length());

	Logger.Trace("Adding one time task...");
	discoverComplete->call(ssid, pswd);
	//Tasks->AddOneTimeTask(*discoverComplete, ssid, pswd);
}
