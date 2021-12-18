// myToday.h

// functions for the pure clock page
// tbd: shows time, date, the next event, the current weather

#pragma once

void printTime() {
	char timeString[16];
	char buf1[64];
	char last[64];

	sprintf(timeString, " %02d:%02d ", currentTime.hour, currentTime.min);

	myToday.createCanvas(460, 100);
	myToday.setTextSize(92);
	myToday.setTextDatum(TC_DATUM);
	myToday.drawString(timeString, 230, 0);
	myToday.pushCanvas(250, 80, UPDATE_MODE_GC16);
	myToday.deleteCanvas();

	myToday.createCanvas(310, 95);
	myToday.fillCanvas(MYWHITE);
	myToday.setTextSize(18);
	drawBattery(myToday, 250, 10);
	drawRSSI(myToday, 250, 50);
	myToday.pushCanvas(650, 445, UPDATE_MODE_GC16);
	myToday.deleteCanvas();
}

void printDate() {
	char dateString[64];
	char buf1[64];
	char buf2[30];

	sprintf(dateString, "%s, %02d. %s %04d", dayNames[currentDate.week], currentDate.day, monthNames[currentDate.mon], currentDate.year);
	strftime(buf1, sizeof(buf1), "KW: %V / Tag im Jahr: %j", &tm);
	strftime(buf2, sizeof(buf2), "Zeitzone: %Z", &tm);

	myToday.createCanvas(750, 180);
	myToday.setTextSize(48);
	myToday.setTextDatum(TC_DATUM);
	myToday.drawString(dateString, 350, 0);
	myToday.drawString(buf1, 350, 60);
	myToday.drawString(buf2, 350, 120);
	myToday.pushCanvas(105, 200, UPDATE_MODE_GC16);
	myToday.deleteCanvas();

	myToday.createCanvas(600, 150);
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
	myToday.pushCanvas(0, 390, UPDATE_MODE_GC16);
	myToday.deleteCanvas();


}
