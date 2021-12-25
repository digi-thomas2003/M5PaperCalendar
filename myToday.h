// myToday.h

// functions for the pure clock page
// shows time, date, the next event, the current weather

#pragma once

void printTime() {
	char timeString[16];

	sprintf(timeString, " %02d:%02d ", currentTime.hour, currentTime.min);

	myToday.createCanvas(330, 100);
	myToday.setTextSize(92);
	myToday.setTextDatum(TC_DATUM);
	myToday.drawString(timeString, 165, 5);
	myToday.pushCanvas(5, 60, UPDATE_MODE_GC16);
	myToday.deleteCanvas();

	myToday.createCanvas(200, 80);
	myToday.fillCanvas(MYWHITE);
	myToday.setTextSize(18);
	drawBattery(myToday, 130, 10);
	drawRSSI(myToday, 130, 45);
	myToday.pushCanvas(750, 410, UPDATE_MODE_GC16);
	myToday.deleteCanvas();
}

void printDate() {
	char dateString[64];
	char buf1[64];
	char buf2[30];

	sprintf(dateString, "%s, %02d. %s %04d", dayNames[currentDate.week], currentDate.day, monthNames[currentDate.mon], currentDate.year);
	strftime(buf1, sizeof(buf1), "KW: %V / Tag im Jahr: %j", &tm);
	strftime(buf2, sizeof(buf2), "Zeitzone: %Z", &tm);

	myToday.createCanvas(595, 115);
	myToday.setTextSize(36);
	myToday.setTextDatum(TL_DATUM);
	myToday.drawString(dateString, 5, 0);
	myToday.drawString(buf1, 5, 38);
	myToday.drawString(buf2, 5, 76);
	myToday.pushCanvas(355, 60, UPDATE_MODE_GC16);
	myToday.deleteCanvas();

	myToday.createCanvas(600, 120);
	myToday.setTextSize(24);
	myToday.setTextDatum(TL_DATUM);
	sprintf(buf1, "Netzwerk: %s", WiFi.SSID());
	myToday.drawString(buf1, 5, 0);
	//sprintf(buf1, "IP: %d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
	sprintf(buf1, "IP: %s", WiFi.localIP().toString().c_str());
	myToday.drawString(buf1, 5, 25);
	sprintf(buf1, "Firmware: %s", FIRMWARE);
	myToday.drawString(buf1, 5, 50);
	strftime(buf1, sizeof(buf1), "Letzter Neustart: %d.%m.%Y %H:%M:%S", &start);
	myToday.drawString(buf1, 5, 75);
	myToday.pushCanvas(5, 390, UPDATE_MODE_GC16);
	myToday.deleteCanvas();
}

void printFrame() {
	myToday.createCanvas(960, 540);

	myToday.fillRect(0, 0, 960, 30, MYBLACK);
	myToday.setTextSize(24);
	myToday.setTextDatum(TC_DATUM);
	myToday.setTextColor(MYWHITE, MYBLACK);
	myToday.drawString("--{ Heute }--", 480, 3);

	myToday.setTextColor(MYBLACK, MYWHITE);
	myToday.drawFastHLine(0, 0, 960, 3, MYBLACK);
	myToday.drawFastHLine(0, 540, 960, 3, MYBLACK);
	myToday.drawFastHLine(0, 180, 960, 2, MYBLACK);
	myToday.drawFastHLine(0, 350, 960, 2, MYBLACK);
	myToday.drawFastVLine(0, 0, 540, 3, MYBLACK);
	myToday.drawFastVLine(956, 0, 540, 3, MYBLACK);
	myToday.drawFastVLine(350, 30, 320, 2, MYBLACK);
	myToday.setTextSize(24);
	myToday.setTextDatum(TL_DATUM);
	myToday.drawString("Zeit: ", 5, 35);
	myToday.drawString("Datum: ", 355, 35);
	myToday.drawString("Aktuelles Wetter: ", 5, 185);
	myToday.drawString("N\u00e4chster Termin: ", 355, 185);
	myToday.drawString("System-Infos: ", 5, 355);

	myToday.fillRect(0, 510, 960, 30, MYBLACK);
	myToday.setTextSize(24);
	myToday.setTextDatum(TC_DATUM);
	myToday.setTextColor(MYWHITE, MYBLACK);
	myToday.drawString("\u00a9 TomSoft " + String(currentDate.year), 480, 513);
	myToday.setTextColor(MYBLACK, MYWHITE);

	myToday.pushCanvas(0, 0, UPDATE_MODE_GC16);
	myToday.deleteCanvas();
}

void printWeather() {
	char buf[64];

	rTW = readWeather();

	Serial.println("weather OK? " + String(rTW));
	if (rTW) getLocalTime(&weatherTime);

	myToday.createCanvas(330, 135);

	String icon = weather.hourlyIcon[0];
	int iconX = 10;
	int iconY = 5;

	if (icon == "01d") drawIcon(myToday, iconX, iconY, (uint16_t*)image_data_01d, 64, 64, true);
	else if (icon == "01n") drawIcon(myToday, iconX, iconY, (uint16_t*)image_data_03n, 64, 64, true);
	else if (icon == "02d") drawIcon(myToday, iconX, iconY, (uint16_t*)image_data_02d, 64, 64, true);
	else if (icon == "02n") drawIcon(myToday, iconX, iconY, (uint16_t*)image_data_02n, 64, 64, true);
	else if (icon == "03d") drawIcon(myToday, iconX, iconY, (uint16_t*)image_data_03d, 64, 64, true);
	else if (icon == "03n") drawIcon(myToday, iconX, iconY, (uint16_t*)image_data_03n, 64, 64, true);
	else if (icon == "04d") drawIcon(myToday, iconX, iconY, (uint16_t*)image_data_04d, 64, 64, true);
	else if (icon == "04n") drawIcon(myToday, iconX, iconY, (uint16_t*)image_data_03n, 64, 64, true);
	else if (icon == "09d") drawIcon(myToday, iconX, iconY, (uint16_t*)image_data_09d, 64, 64, true);
	else if (icon == "09n") drawIcon(myToday, iconX, iconY, (uint16_t*)image_data_09n, 64, 64, true);
	else if (icon == "10d") drawIcon(myToday, iconX, iconY, (uint16_t*)image_data_10d, 64, 64, true);
	else if (icon == "10n") drawIcon(myToday, iconX, iconY, (uint16_t*)image_data_03n, 64, 64, true);
	else if (icon == "11d") drawIcon(myToday, iconX, iconY, (uint16_t*)image_data_11d, 64, 64, true);
	else if (icon == "11n") drawIcon(myToday, iconX, iconY, (uint16_t*)image_data_11n, 64, 64, true);
	else if (icon == "13d") drawIcon(myToday, iconX, iconY, (uint16_t*)image_data_13d, 64, 64, true);
	else if (icon == "13n") drawIcon(myToday, iconX, iconY, (uint16_t*)image_data_13n, 64, 64, true);
	else if (icon == "50d") drawIcon(myToday, iconX, iconY, (uint16_t*)image_data_50d, 64, 64, true);
	else if (icon == "50n") drawIcon(myToday, iconX, iconY, (uint16_t*)image_data_50n, 64, 64, true);
	else drawIcon(myToday, iconX, iconY, (uint16_t*)image_data_unknown, 64, 64, true);

	myToday.setTextDatum(TL_DATUM);
	myToday.setTextSize(24);
	myToday.drawString(weather.hourlyMain[0], 5, 80);

	myToday.setTextSize(36);
	myToday.setTextDatum(TR_DATUM);
	myToday.drawString("A: " + getFloatString(weather.hourlyMaxTemp[0], " \u00b0C"), 330, 10);
	myToday.drawString("I: " + String(sht30Temperatur) + " \u00b0C", 330, 50);

	myToday.setTextSize(14);
	myToday.setTextDatum(TL_DATUM);
	strftime(buf, sizeof(buf), "Stand: %d.%m.%Y %H:%M:%S", &weatherTime);
	myToday.drawString(buf, 5, 110);

	myToday.pushCanvas(5, 210, UPDATE_MODE_GC16);
	myToday.deleteCanvas();

}

void printNextEvent() {
	byte p = 20;
	int checkDay;
	int checkMonth;
	int checkYear;
	int checkMin;
	int checkHour;

	rTC = readCalendar();

	Serial.println("Calendar OK? " + String(rTC));

	M5.RTC.getTime(&currentTime);
	M5.RTC.getDate(&currentDate);

	tmElements_t currentNow = { 0, currentTime.min, currentTime.hour, 0, currentDate.day, currentDate.mon, currentDate.year - 1970 };
	time_t currentNowTime = makeTime(currentNow);

	//Serial.println("CurrentNow: " + String(currentNowTime));

	for (byte r = 0; r < 11; r++) {
		checkDay = calEnt[r].calStartDate.substring(0, 2).toInt();
		checkMonth = calEnt[r].calStartDate.substring(3, 5).toInt();
		checkYear = calEnt[r].calStartDate.substring(6, 10).toInt();
		checkHour = calEnt[r].calStartTime.substring(0, 2).toInt();
		checkMin = calEnt[r].calStartTime.substring(3, 5).toInt();

		//Serial.println(String(checkMin) + " " + String(checkHour) + " " + String(checkDay) + " " + String(checkMonth) + " " + String(checkYear));

		tmElements_t calEvent = { 0, checkMin, checkHour, 0, checkDay, checkMonth, checkYear - 1970 };
		time_t calEventTime = makeTime(calEvent);

		//Serial.println("calEventTime: " + String(calEventTime));

		if (currentNowTime <= calEventTime && calEnt[r].calAllDay == "false") {
			p = r;
			break;
		}
		if (calEnt[r].calStartDate == "") break;
	}

	//Serial.println("p = " + String(p));

	myToday.createCanvas(595, 135);

	myToday.setTextSize(36);
	myToday.setTextDatum(TL_DATUM);
	if (!rTC) {
		myToday.drawString("Fehler beim Holen der Daten", 5, 20);
	}
	else if (p == 20) {
		myToday.drawString("Kein Termin in der n\u00e4chsten Zeit", 5, 20);
	}
	else {
		myToday.drawString(calEnt[p].calTitle, 5, 10);
		myToday.setTextSize(24);
		iconLocation(myToday, 5, 50, 25);
		myToday.drawString(calEnt[p].calLocation, 35, 50);
		myToday.drawString(calEnt[p].calStartDate + " / " + calEnt[p].calStartTime, 5, 80);
	}

	myToday.pushCanvas(355, 210, UPDATE_MODE_GC16);
	myToday.deleteCanvas();
}
