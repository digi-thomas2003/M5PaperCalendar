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
		if (getLocalTime(&start)) {
			rtc_time_t time_struct;
			time_struct.hour = start.tm_hour;
			time_struct.min = start.tm_min;
			time_struct.sec = start.tm_sec;
			M5.RTC.setTime(&time_struct);                         // feed the RTC with the time
			rtc_date_t date_struct;
			date_struct.week = start.tm_wday;
			date_struct.mon = start.tm_mon + 1;
			date_struct.day = start.tm_mday;
			date_struct.year = start.tm_year + 1900;
			M5.RTC.setDate(&date_struct);                         // feed the RTC with the date
			Serial.print("Time synced\n");
			Serial.println(&start, "Programmstart: %A, %d.%B.%Y %H:%M:%S");
			return true;
		}
		Serial.print("Time Sync failed");
		return false;
	}
}

bool checkWiFi() {
	if (WiFi.status() != WL_CONNECTED) return false;
	else return true;
}
