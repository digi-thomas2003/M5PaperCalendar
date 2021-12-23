// myToday.h

// functions for the pure clock page
// tbd: shows time, date, the next event, the current weather

#pragma once
//#include "Icons.h"
//#include "myWeather.h"


void printTime() {
	char timeString[16];

	sprintf(timeString, " %02d:%02d ", currentTime.hour, currentTime.min);

	myToday.createCanvas(330, 100);
	myToday.setTextSize(92);
	myToday.setTextDatum(TC_DATUM);
	myToday.drawString(timeString, 165, 5);
	myToday.pushCanvas(5, 60, UPDATE_MODE_GC16);
	myToday.deleteCanvas();

	myToday.createCanvas(310, 80);
	myToday.fillCanvas(MYWHITE);
	myToday.setTextSize(18);
	drawBattery(myToday, 250, 10);
	drawRSSI(myToday, 250, 45);
	myToday.pushCanvas(650, 410, UPDATE_MODE_GC16);
	myToday.deleteCanvas();
}

void printDate() {
	char dateString[64];
	char buf1[64];
	char buf2[30];

	sprintf(dateString, "%s, %02d. %s %04d", dayNames[currentDate.week], currentDate.day, monthNames[currentDate.mon], currentDate.year);
	strftime(buf1, sizeof(buf1), "KW: %V / Tag im Jahr: %j", &tm);
	strftime(buf2, sizeof(buf2), "Zeitzone: %Z", &tm);

	myToday.createCanvas(600, 115);
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
	myToday.drawFastVLine(960, 0, 540, 3, MYBLACK);
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
	myToday.drawString(getFloatString(weather.hourlyMaxTemp[0], "  \u00b0C"), 150, 30);

	myToday.setTextSize(14);
	strftime(buf, sizeof(buf), "Stand: %d.%m.%Y %H:%M:%S", &weatherTime);
	myToday.drawString(buf, 5, 110);

	myToday.pushCanvas(5, 210, UPDATE_MODE_GC16);
	myToday.deleteCanvas();

}
