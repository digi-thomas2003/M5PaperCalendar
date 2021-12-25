// myUtils.h
//
// some helper utilities

#pragma once

/* build and show battery icon */
void drawBattery(M5EPD_Canvas& canvas1, int x, int y) {
	uint32_t vol = M5.getBatteryVoltage();

	if (vol < 3300) vol = 3300;
	else if (vol > 4350) vol = 4350;

	float battery = (float)(vol - 3300) / (float)(4350 - 3300);

	if (battery <= 0.01) battery = 0.01;
	if (battery > 1) battery = 1;

	uint32_t batteryCapacity = (int)(battery * 100);

	canvas1.setTextDatum(TR_DATUM);
	canvas1.drawString(String(batteryCapacity) + "%", x - 5, y);
	canvas1.drawRect(x, y, 40, 16, MYBLACK);
	canvas1.drawRect(x + 40, y + 3, 4, 10, MYBLACK);
	for (int i = x; i < x + 40; i++) {
		canvas1.drawLine(i, y, i, y + 15, MYBLACK);
		if ((i - x) * 100.0 / 40.0 > batteryCapacity) {
			break;
		}
	}
}

/* Draw a circle with optional start and end point */
void drawCircle(M5EPD_Canvas& canvas1, int32_t x, int32_t y, int32_t r, uint32_t color, int32_t degFrom /* = 0 */, int32_t degTo /* = 360 */) {
	for (int i = degFrom; i < degTo; i++) {
		double radians = i * PI / 180;
		double px = x + r * cos(radians);
		double py = y + r * sin(radians);

		canvas1.drawPixel(px, py, color);
	}
}


/* Draw a the rssi value as string and as circle parts */
void drawRSSI(M5EPD_Canvas& canvas1, int x, int y) {

	int quality = 0;
	int rssi = WiFi.RSSI();

	if (rssi <= -100) quality = 0;
	else if (rssi >= -50) quality = 100;
	else     quality = 2 * (rssi + 100);

	canvas1.setTextDatum(TR_DATUM);
	canvas1.drawString(String(quality) + "%", x - 5, y);
	if (quality >= 80) drawCircle(canvas1, x + 12, y + 15, 16, M5EPD_Canvas::G15, 225, 315);
	if (quality >= 40) drawCircle(canvas1, x + 12, y + 15, 12, M5EPD_Canvas::G15, 225, 315);
	if (quality >= 20) drawCircle(canvas1, x + 12, y + 15, 8, M5EPD_Canvas::G15, 225, 315);
	if (quality >= 10) drawCircle(canvas1, x + 12, y + 15, 4, M5EPD_Canvas::G15, 225, 315);
	drawCircle(canvas1, x + 12, y + 10, 2, M5EPD_Canvas::G15, 225, 315);
}


/* cut a text if it is too long and add "..." instead*/
void textEllipsis(M5EPD_Canvas& canvas1, int x, int y, int width, const char* text) {
	uint16_t w = canvas1.textWidth(text);
	uint16_t w_substr = w;
	String ellipsis = text;
	int length = ellipsis.length();
	if (w_substr > width) {
		while (w_substr > width) {
			length -= 1;
			ellipsis = ellipsis.substring(0, length);
			w_substr = canvas1.textWidth(ellipsis.c_str());
		}
		ellipsis = ellipsis.substring(0, length - 3) + "...";
	}
	canvas1.setCursor(x, y);
	canvas1.print(F(ellipsis.c_str()));
}

/* Convert the float value to string with unit */
String getFloatString(float value, const char* unit) {
	char buff[32];
	sprintf(buff, "%.1f%s", value, unit);
	return (String)buff;
}

/* Convert the minute of the time_t */
String getHourMinString(time_t rawtime) {
	char buff[32];
	sprintf(buff, "%02d:%02d",
		hour(rawtime), minute(rawtime));
	return (String)buff;
}

/* Convert the date part of the RTC timestamp */
String getRTCDateString() {
	char       buff[32];
	rtc_date_t date_struct;
	M5.RTC.getDate(&date_struct);
	sprintf(buff, "%02d.%02d.%04d",
		date_struct.day, date_struct.mon, date_struct.year);
	return (String)buff;
}

/* Convert the time part of the RTC timestamp */
String getRTCTimeString() {
	char       buff[32];
	rtc_time_t time_struct;
	M5.RTC.getTime(&time_struct);
	sprintf(buff, "%02d:%02d",
		time_struct.hour, time_struct.min);
	return (String)buff;
}

/* Convert the time_t to the date part MM/DD */
String getShortDayOfWeekString(time_t rawtime) {
	return (String)dayShortNamesDD[weekday(rawtime)-1];
}


/* Read the SHT30 environment chip data */
bool getSHT30Values() {
	M5.SHT30.UpdateData();
	if (M5.SHT30.GetError() == 0) {
		sht30Temperatur = (int)M5.SHT30.GetTemperature();
		sht30Humidity = (int)M5.SHT30.GetRelHumidity();
		return true;
	}
	return false;
}
