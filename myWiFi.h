// myWiFi.h

// the functions for connecting to wifi and get the ntp time

#pragma once

/* Start WiFi and Connect to Network */
boolean connectWiFi() {

	WiFi.disconnect();
	WiFi.mode(WIFI_STA);
	WiFi.config(ip, gateway, subnet, dns);
	WiFi.begin(ssid, password);

	boolean state = true;
	String hostname(HOSTNAME);
	WiFi.setHostname(hostname.c_str());

	int i = 0;
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
		if (i > 60) {
			state = false;
			break;
		}
		i++;
	}

	// serial output of connection details
	if (state) {
		Serial.print(F("\nConnected to "));
		Serial.println(ssid);
		Serial.print(F("IP address: "));
		Serial.println(WiFi.localIP());
	}
	else {
		Serial.println("Connection failed.");
	}
	return state;
}

bool getTime() {                                                     // Zeitzone einstellen https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
	configTzTime("CET-1CEST,M3.5.0/02,M10.5.0/03", ntpServer[0]);    // deinen NTP Server einstellen (von 0 - 5 aus obiger Liste)
	if (!getLocalTime(&tm)) return false;
	return true;
}

bool setupTime() {
	if (!getTime()) {
		Serial.println("Zeit konnte nicht geholt werden\n");
		return false;
	}
	else {
		Serial.print("Time synced\n");
		return true;
	}
}

bool checkWiFi() {
	if (WiFi.status() != WL_CONNECTED) return false;
	else return true;
}
