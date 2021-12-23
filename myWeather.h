// myWeather.h

// all the functions for weather

#pragma once

/* Convert UTC time to local time */
time_t LocalTime(time_t time) {
	return time + weather.currentTimeOffset;
}

/* Calls the openweathermap request and deserialisation the json data. */
bool GetOpenWeatherJsonDoc(DynamicJsonDocument& doc) {
	WiFiClient client;
	HTTPClient http;
	String     uri;

	uri += "/data/2.5/onecall";
	uri += "?lat=" + String((float)LATITUDE, 5);
	uri += "&lon=" + String((float)LONGITUDE, 5);
	uri += "&units=metric&lang=de&exclude=minutely,alerts";
	uri += "&appid=" + (String)APIKEY;

	if (!checkWiFi()) connectWiFi();

	client.stop();
	http.end();
	http.begin(client, OPENWEATHER_SRV, OPENWEATHER_PORT, uri);

	int httpCode = http.GET();

	if (httpCode != HTTP_CODE_OK) {
		Serial.printf("GetWeather failed, error: %s", http.errorToString(httpCode).c_str());
		client.stop();
		http.end();
		return false;
	}
	else {
		DeserializationError error = deserializeJson(doc, http.getStream());

		if (error) {
			Serial.print(F("deserializeJson() failed: "));
			Serial.println(error.c_str());
			client.stop();
			http.end();
			return false;
		}
		else {
			client.stop();
			http.end();
			return true;
		}
	}
}

/* Fill from the json data into the internal data. */
bool Fill(const JsonObject& root) {

	weather.currentTimeOffset = root["timezone_offset"].as<int>();
	weather.currentTime = LocalTime(root["current"]["dt"].as<int>());

	weather.sunrise = LocalTime(root["current"]["sunrise"].as<int>());
	weather.sunset = LocalTime(root["current"]["sunset"].as<int>());
	weather.winddir = root["current"]["wind_deg"].as<float>();
	weather.windspeed = root["current"]["wind_speed"].as<float>();

	JsonArray hourly_list = root["hourly"];
	weather.hourlyTime[0] = LocalTime(root["current"]["dt"].as<int>());
	weather.hourlyMaxTemp[0] = root["current"]["temp"].as<float>();
	weather.hourlyMain[0] = root["current"]["weather"][0]["description"].as<char*>();
	weather.hourlyRain[0] = root["current"]["rain"]["1h"].as<float>();
	weather.hourlyPop[0] = root["current"]["pop"].as<float>() * 100;
	weather.hourlyPressure[0] = root["current"]["pressure"].as<float>();
	weather.hourlyIcon[0] = root["current"]["weather"][0]["icon"].as<char*>();
	for (int i = 1; i < MAX_HOURLY; i++) {
		if (i < hourly_list.size()) {
			weather.hourlyTime[i] = LocalTime(hourly_list[i - 1]["dt"].as<int>());
			weather.hourlyMaxTemp[i] = hourly_list[i - 1]["temp"].as<float>();
			weather.hourlyMain[i] = hourly_list[i - 1]["weather"][0]["description"].as<char*>();
			weather.hourlyRain[i] = hourly_list[i - 1]["rain"]["1h"].as<float>();
			weather.hourlyPop[i] = hourly_list[i - 1]["pop"].as<float>() * 100;
			weather.hourlyPressure[i] = hourly_list[i - 1]["pressure"].as<float>();
			weather.hourlyIcon[i] = hourly_list[i - 1]["weather"][0]["icon"].as<char*>();
			if (weather.hourlyRain[i] > weather.hourlyMaxRain) {
				weather.hourlyMaxRain = weather.hourlyRain[i] + 4;
			}
			if (weather.hourlyMaxTemp[i] + 2 > weather.hourlyTempRange[1]) {
				weather.hourlyTempRange[1] = (int)((weather.hourlyMaxTemp[i] + 2) / 5) * 5 + 5;
			}
			if (weather.hourlyMaxTemp[i] - 2 < weather.hourlyTempRange[0]) {
				weather.hourlyTempRange[0] = (int)((weather.hourlyMaxTemp[i] - 2) / 5) * 5 - 5;
			}
		}
	}

	JsonArray dayly_list = root["daily"];
	for (int i = 0; i < MAX_FORECAST; i++) {
		if (i < dayly_list.size()) {
			weather.forecastTime[i] = LocalTime(dayly_list[i]["dt"].as<int>());
			weather.forecastMaxTemp[i] = dayly_list[i]["temp"]["max"].as<float>();
			weather.forecastMinTemp[i] = dayly_list[i]["temp"]["min"].as<float>();
			weather.forecastRain[i] = dayly_list[i]["rain"].as<float>();
			weather.forecastPop[i] = dayly_list[i]["pop"].as<float>() * 100;
			weather.forecastPressure[i] = dayly_list[i]["pressure"].as<float>();
			weather.forecastIcon[i] = dayly_list[i]["weather"][0]["icon"].as<char*>();
		}
		if (weather.forecastRain[i] > weather.forecastMaxRain) {
			weather.forecastMaxRain = weather.forecastRain[i] + 4;
		}
		if (weather.forecastMaxTemp[i] + 2 > weather.forecastTempRange[1]) {
			weather.forecastTempRange[1] = (int)((weather.forecastMaxTemp[i] + 2) / 5) * 5 + 5;
		}
		if (weather.forecastMinTemp[i] - 2 < weather.forecastTempRange[0]) {
			weather.forecastTempRange[0] = (int)((weather.forecastMinTemp[i] - 2) / 5) * 5 - 5;
		}
	}

	return true;
}


bool readWeather() {
	DynamicJsonDocument doc(35 * 1024);

	if (GetOpenWeatherJsonDoc(doc)) {
		return Fill(doc.as<JsonObject>());
	}
	return false;
}

/* Draw a the head with version, city, rssi and battery */
void drawHead() {
	myWeather.drawString(FIRMWARE, 20, 10);
	myWeather.drawCentreString(CITY_NAME, maxX / 2, 10, 1);
	myWeather.drawString(weatherOK, maxX - 275, 10);
	drawRSSI(myWeather, maxX - 155, 10);
	drawBattery(myWeather, maxX - 65, 10);
}

/* Draw one icon from the binary data */
void drawIcon(M5EPD_Canvas& canvas1, int x, int y, const uint16_t* icon, int dx = 64, int dy = 64, bool highContrast = false) {
	for (int yi = 0; yi < dy; yi++) {
		for (int xi = 0; xi < dx; xi++) {
			uint16_t pixel = icon[yi * dx + xi];

			if (highContrast) {
				if (15 - (pixel / 4096) > 0) canvas1.drawPixel(x + xi, y + yi, MYBLACK);
			}
			else {
				canvas1.drawPixel(x + xi, y + yi, 15 - (pixel / 4096));
			}
		}
	}
}

/* Draw current weather information */
void drawWeatherInfo(int x, int y, int dx, int dy) {
	myWeather.setTextSize(36);
	myWeather.drawCentreString("Wetter", x + dx / 2, y + 5, 1);
	myWeather.drawLine(x, y + 35, x + dx, y + 35, MYBLACK);

	String icon = weather.hourlyIcon[0];
	int iconX = x + dx / 2 - 32;
	int iconY = y + 45;

	if (icon == "01d") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_01d, 64, 64, true);
	else if (icon == "01n") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_03n, 64, 64, true);
	else if (icon == "02d") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_02d, 64, 64, true);
	else if (icon == "02n") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_02n, 64, 64, true);
	else if (icon == "03d") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_03d, 64, 64, true);
	else if (icon == "03n") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_03n, 64, 64, true);
	else if (icon == "04d") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_04d, 64, 64, true);
	else if (icon == "04n") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_03n, 64, 64, true);
	else if (icon == "09d") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_09d, 64, 64, true);
	else if (icon == "09n") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_09n, 64, 64, true);
	else if (icon == "10d") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_10d, 64, 64, true);
	else if (icon == "10n") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_03n, 64, 64, true);
	else if (icon == "11d") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_11d, 64, 64, true);
	else if (icon == "11n") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_11n, 64, 64, true);
	else if (icon == "13d") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_13d, 64, 64, true);
	else if (icon == "13n") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_13n, 64, 64, true);
	else if (icon == "50d") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_50d, 64, 64, true);
	else if (icon == "50n") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_50n, 64, 64, true);
	else drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_unknown, 64, 64, true);

	//myWeather.drawCentreString(weather.hourlyMain[0], x + dx / 2, y + 110, 1);
	myWeather.setTextArea(x + 4, y + 110, dx - 4, 70);
	myWeather.setTextWrap(true);
	myWeather.setTextSize(24);
	myWeather.print(weather.hourlyMain[0]);

	myWeather.setTextDatum(TL_DATUM);
	myWeather.setTextSize(36);
	myWeather.drawString(getFloatString(weather.hourlyMaxTemp[0], "  \u00b0C"), x + 30, y + 170, 1);
	myWeather.drawString(getFloatString(weather.hourlyRain[0], " mm"), x + 30, y + 210, 1);
}

/* Draw the sun information with sunrise and sunset */
void drawSunInfo(int x, int y, int dx, int dy) {
	myWeather.setTextSize(36);
	myWeather.drawCentreString("Sonne", x + dx / 2, y + 5, 1);
	myWeather.drawLine(x, y + 35, x + dx, y + 35, MYBLACK);

	myWeather.setTextSize(36);
	drawIcon(myWeather, x + 25, y + 55, (uint16_t*)SUNRISE64x64);
	myWeather.drawString(getHourMinString(weather.sunrise), x + 105, y + 80, 1);

	drawIcon(myWeather, x + 25, y + 150, (uint16_t*)SUNSET64x64);
	myWeather.drawString(getHourMinString(weather.sunset), x + 105, y + 175, 1);
}

/* Draw the in the wind section
 * The wind section drawing was from the github project
 * https://github.com/G6EJD/ESP32-Revised-Weather-Display-42-E-Paper
 * See http://www.dsbird.org.uk
 * Copyright (c) David Bird
 */
void Arrow(int x, int y, int asize, float aangle, int pwidth, int plength) {
	float dx = (asize + 21) * cos((aangle - 90) * PI / 180) + x; // calculate X position
	float dy = (asize + 21) * sin((aangle - 90) * PI / 180) + y; // calculate Y position
	float x1 = 0;           float y1 = plength;
	float x2 = pwidth / 2;  float y2 = pwidth / 2;
	float x3 = -pwidth / 2; float y3 = pwidth / 2;
	float angle = aangle * PI / 180;
	float xx1 = x1 * cos(angle) - y1 * sin(angle) + dx;
	float yy1 = y1 * cos(angle) + x1 * sin(angle) + dy;
	float xx2 = x2 * cos(angle) - y2 * sin(angle) + dx;
	float yy2 = y2 * cos(angle) + x2 * sin(angle) + dy;
	float xx3 = x3 * cos(angle) - y3 * sin(angle) + dx;
	float yy3 = y3 * cos(angle) + x3 * sin(angle) + dy;
	myWeather.fillTriangle(xx1, yy1, xx3, yy3, xx2, yy2, MYBLACK);
}

void displayDisplayWindSection(int x, int y, float angle, float windspeed, int cradius) {
	int dxo, dyo, dxi, dyi;

	myWeather.setTextSize(24);
	myWeather.drawLine(0, 15, 0, y + cradius + 30, MYBLACK);
	for (float a = 0; a < 360; a = a + 22.5) {
		dxo = cradius * cos((a - 90) * PI / 180);
		dyo = cradius * sin((a - 90) * PI / 180);
		if (a == 45)  myWeather.drawCentreString("NO", dxo + x + 22, dyo + y - 20, 1);
		if (a == 135) myWeather.drawCentreString("SO", dxo + x + 20, dyo + y + 10, 1);
		if (a == 225) myWeather.drawCentreString("SW", dxo + x - 20, dyo + y + 10, 1);
		if (a == 315) myWeather.drawCentreString("NW", dxo + x - 22, dyo + y - 20, 1);
		dxi = dxo * 0.9;
		dyi = dyo * 0.9;
		myWeather.drawLine(dxo + x, dyo + y, dxi + x, dyi + y, MYBLACK);
		dxo = dxo * 0.7;
		dyo = dyo * 0.7;
		dxi = dxo * 0.9;
		dyi = dyo * 0.9;
		myWeather.drawLine(dxo + x, dyo + y, dxi + x, dyi + y, MYBLACK);
	}
	myWeather.drawCentreString("N", x, y - cradius - 25, 1);
	myWeather.drawCentreString("S", x, y + cradius + 5, 1);
	myWeather.drawCentreString("W", x - cradius - 17, y - 6, 1);
	myWeather.drawCentreString("O", x + cradius + 15, y - 6, 1);
	myWeather.drawCentreString("m/s", x, y, 1);
	myWeather.drawCentreString(String(windspeed, 1), x, y - 20, 1);

	myWeather.drawCircle(x, y, cradius, MYBLACK);       // Draw compass circle
	myWeather.drawCircle(x, y, cradius + 1, MYBLACK);   // Draw compass circle
	myWeather.drawCircle(x, y, cradius * 0.7, MYBLACK); // Draw compass inner circle

	Arrow(x, y, cradius - 17, angle, 15, 27);
}

/* Draw the wind information part */
void drawWindInfo(int x, int y, int dx, int dy)
{
	myWeather.setTextSize(36);
	myWeather.drawCentreString("Wind", x + dx / 2, y + 5, 1);
	myWeather.drawLine(x, y + 35, x + dx, y + 35, MYBLACK);

	displayDisplayWindSection(x + dx / 2, y + dy / 2 + 20, weather.winddir, weather.windspeed, 75);
}

/* Draw the M5Paper environment and RTC information */
void drawM5PaperInfo(int x, int y, int dx, int dy) {
	myWeather.setTextSize(36);
	myWeather.drawCentreString("Innen", x + dx / 2, y + 5, 1);
	myWeather.drawLine(x, y + 35, x + dx, y + 35, MYBLACK);

	myWeather.setTextSize(36);
	myWeather.drawCentreString(getRTCDateString(), x + dx / 2, y + 50, 1);
	myWeather.drawCentreString(getRTCTimeString(), x + dx / 2, y + 90, 1);
	myWeather.setTextSize(24);

	myWeather.setTextSize(36);
	myWeather.setTextDatum(TL_DATUM);
	drawIcon(myWeather, x + 35, y + 140, (uint16_t*)TEMPERATURE64x64);
	myWeather.drawString(String(sht30Temperatur) + " \u00b0C", x + 35, y + 210, 1);
	drawIcon(myWeather, x + 145, y + 140, (uint16_t*)HUMIDITY64x64);
	myWeather.drawString(String(sht30Humidity) + "%", x + 150, y + 210, 1);

}

/* Draw one daily weather information */
void drawDaily(int x, int y, int dx, int dy, Weather& weather, int index) {
	time_t time = weather.forecastTime[index];
	int    tMin = weather.forecastMinTemp[index];
	int    tMax = weather.forecastMaxTemp[index];
	int    pop = weather.forecastPop[index];
	String icon = weather.forecastIcon[index];

	myWeather.setTextSize(24);
	myWeather.drawCentreString(index == 0 ? "Heute" : getShortDayOfWeekString(time), x + dx / 2, y + 5, 1);

	int iconX = x + dx / 2 - 32;
	int iconY = y + 20;

	if (icon == "01d") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_01d, 64, 64, true);
	else if (icon == "01n") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_03n, 64, 64, true);
	else if (icon == "02d") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_02d, 64, 64, true);
	else if (icon == "02n") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_02n, 64, 64, true);
	else if (icon == "03d") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_03d, 64, 64, true);
	else if (icon == "03n") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_03n, 64, 64, true);
	else if (icon == "04d") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_04d, 64, 64, true);
	else if (icon == "04n") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_03n, 64, 64, true);
	else if (icon == "09d") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_09d, 64, 64, true);
	else if (icon == "09n") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_09n, 64, 64, true);
	else if (icon == "10d") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_10d, 64, 64, true);
	else if (icon == "10n") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_03n, 64, 64, true);
	else if (icon == "11d") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_11d, 64, 64, true);
	else if (icon == "11n") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_11n, 64, 64, true);
	else if (icon == "13d") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_13d, 64, 64, true);
	else if (icon == "13n") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_13n, 64, 64, true);
	else if (icon == "50d") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_50d, 64, 64, true);
	else if (icon == "50n") drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_50n, 64, 64, true);
	else drawIcon(myWeather, iconX, iconY, (uint16_t*)image_data_unknown, 64, 64, true);

	myWeather.drawCentreString(String(tMin) + "/" + String(tMax), x + dx / 2, y + 76, 1);
	myWeather.drawCentreString(String(pop) + "%", x + dx / 2, y + 99, 1);
}

/* Draw a graph with x- and y-axis and values */
void drawGraph(int x, int y, int dx, int dy, String title, int xMin, int xMax, int yMin, int yMax, float values[], float values2[]) {
	String yMinString = String(yMin);
	String yMaxString = String(yMax);
	int    textWidth = 5 + max(yMinString.length() * 3.5, yMaxString.length() * 3.5);
	int    graphX = x + 5 + textWidth + 5;
	int    graphY = y + 35;
	int    graphDX = dx - textWidth - 20;
	int    graphDY = dy - 35 - 20;
	float  xStep = graphDX / xMax;
	float  yStep = graphDY / (yMax - yMin);
	int    iOldX = 0;
	int    iOldY = 0;

	myWeather.setTextSize(24);
	myWeather.drawCentreString(title, x + dx / 2, y + 10, 1);
	myWeather.setTextSize(14);
	myWeather.drawString(yMaxString, x + 5, graphY - 5);
	myWeather.drawString(yMinString, x + 5, graphY + graphDY - 3);
	for (int i = 0; i <= xMax; i++) {
		myWeather.drawString(String(i), graphX + i * xStep, graphY + graphDY + 5);
	}

	myWeather.drawRect(graphX, graphY, graphDX, graphDY, MYBLACK);
	if (yMin < 0 && yMax > 0) { // null line?
		float yValueDX = (float)graphDY / (yMax - yMin);
		int   yPos = graphY + graphDY - (0.0 - yMin) * yValueDX;

		if (yPos > graphY + graphDY) yPos = graphY + graphDY;
		if (yPos < graphY)           yPos = graphY;

		myWeather.drawString("0", graphX - 20, yPos);
		for (int xDash = graphX; xDash < graphX + graphDX - 10; xDash += 10) {
			myWeather.drawLine(xDash, yPos, xDash + 5, yPos, MYBLACK);
		}
	}
	for (int i = xMin; i <= xMax; i++) {
		float yValue = values[i];
		float yValueDY = (float)graphDY / (yMax - yMin);
		int   xPos = graphX + graphDX / xMax * i;
		int   yPos = graphY + graphDY - (yValue - yMin) * yValueDY;

		if (yPos > graphY + graphDY) yPos = graphY + graphDY;
		if (yPos < graphY)           yPos = graphY;

		myWeather.fillCircle(xPos, yPos, 2, MYBLACK);
		if (i > xMin) {
			myWeather.drawLine(iOldX, iOldY, xPos, yPos, MYBLACK);
		}
		iOldX = xPos;
		iOldY = yPos;
	}
	if (values2 != NULL) {
		for (int i = xMin; i <= xMax; i++) {
			float yValue = values2[i];
			float yValueDY = (float)graphDY / (yMax - yMin);
			int   xPos = graphX + graphDX / xMax * i;
			int   yPos = graphY + graphDY - (yValue - yMin) * yValueDY;

			if (yPos > graphY + graphDY) yPos = graphY + graphDY;
			if (yPos < graphY)           yPos = graphY;

			if (i > xMin) {
				myWeather.drawLine(iOldX, iOldY, xPos, yPos, MYBLACK);
				myWeather.drawLine(iOldX, iOldY + 1, xPos, yPos + 1, MYWHITE);
			}
			myWeather.fillCircle(xPos, yPos, 3, MYBLACK);
			myWeather.fillCircle(xPos, yPos, 2, MYWHITE);
			iOldX = xPos;
			iOldY = yPos;
		}
	}
}

/* Draw a dual graph */
void drawDualGraph(int x, int y, int dx, int dy, String title, int xMin, int xMax, int yMin, int yMax, float values[], int offset, int yMinB, int yMaxB, float valuesB[]) {
	String yMinString = String(yMinB);
	String yMaxString = String(yMaxB);
	int    textWidth = 5 + max(yMinString.length() * 3.5, yMaxString.length() * 3.5);
	int    graphX = x + 5 + textWidth + 5;
	int    graphY = y + 35;
	int    graphDX = dx - textWidth - 20;
	int    graphDY = dy - 35 - 20;
	float  xStep = graphDX / xMax;
	float  yStep = graphDY / (yMax - yMin);
	int    iOldX = 0;
	int    iOldY = 0;

	myWeather.setTextSize(24);
	myWeather.drawCentreString(title, x + dx / 2, y + 10, 1);
	myWeather.setTextSize(14);
	myWeather.drawString(yMaxString, x + 5, graphY - 5);
	myWeather.drawString(yMinString, x + 5, graphY + graphDY - 3);
	for (int i = 0; i <= xMax; i++) {
		myWeather.drawString(String(i), graphX + i * xStep, graphY + graphDY + 5);
	}

	myWeather.drawRect(graphX, graphY, graphDX, graphDY, MYBLACK);
	if (yMin < 0 && yMax > 0) { // null line?
		float yValueDX = (float)graphDY / (yMax - yMin);
		int   yPos = graphY + graphDY - (0.0 - yMin) * yValueDX;

		if (yPos > graphY + graphDY) yPos = graphY + graphDY;
		if (yPos < graphY)           yPos = graphY;

		myWeather.drawString("0", graphX - 20, yPos);
		for (int xDash = graphX; xDash < graphX + graphDX - 10; xDash += 10) {
			myWeather.drawLine(xDash, yPos, xDash + 5, yPos, MYBLACK);
		}
	}
	for (int i = xMin; i < xMax; i++) {
		float yValue = valuesB[i - xMin];
		float yValueDY = (float)graphDY / (float)(yMaxB - yMinB);
		int   xPos = graphX + graphDX / (xMax - xMin) * i;
		int   yPos = graphY + graphDY - ((yValue - (float)yMinB) * yValueDY);

		if (yPos > graphY + graphDY) yPos = graphY + graphDY;
		if (yPos < graphY)           yPos = graphY;

		int width = graphDX / (xMax - xMin);
		int height = (graphY + graphDY) - yPos;
		if (height > 0) {
			myWeather.fillRect(xPos, yPos, width, height, MYBLACK);
		}
	}
	for (int i = xMin + offset; i <= xMax; i++) {
		float yValue = values[i];
		float yValueDY = (float)graphDY / (yMax - yMin);
		int   xPos = graphX + graphDX / xMax * i;
		int   yPos = graphY + graphDY - (yValue - yMin) * yValueDY;

		if (yPos > graphY + graphDY) yPos = graphY + graphDY;
		if (yPos < graphY)           yPos = graphY;

		if (i > xMin + offset) {
			myWeather.drawLine(iOldX, iOldY, xPos, yPos, MYBLACK);
			myWeather.drawLine(iOldX, iOldY + 1, xPos, yPos + 1, MYWHITE);
		}
		myWeather.fillCircle(xPos, yPos, 3, MYWHITE);
		myWeather.fillCircle(xPos, yPos, 2, MYBLACK);
		iOldX = xPos;
		iOldY = yPos;
	}
}

/* Main function to show all the data to the e-paper */
void showWeather() {
	Serial.println("Build the weather page");

	rW = readWeather();

	Serial.println("weather OK? " + String(rW));
	if (rW) weatherOK = "+";
	else weatherOK = "-";

	myWeather.createCanvas(960, 540);
	myWeather.fillCanvas(MYWHITE);
	myWeather.setTextSize(24);
	myWeather.setTextColor(MYBLACK, MYWHITE);
	myWeather.setTextDatum(TL_DATUM);

	drawHead();

	// x = 960 y = 540
	// 540 - oben 35 - unten 10 = 495

	myWeather.drawRect(14, 34, maxX - 28, maxY - 43, MYBLACK);

	drawWeatherInfo(15, 35, 232, 251);
	drawSunInfo(232, 35, 232, 251);
	drawWindInfo(465, 35, 232, 251);
	drawM5PaperInfo(697, 35, 245, 251); // 697, 35, 245, 251
	myWeather.drawRect(15, 35, maxX - 30, 251, MYBLACK);
	myWeather.drawLine(232, 35, 232, 286, MYBLACK);
	myWeather.drawLine(465, 35, 465, 286, MYBLACK);
	myWeather.drawLine(697, 35, 697, 286, MYBLACK);

	for (int x = 15, i = 0; x <= 930; x += 116, i++) {
		drawDaily(x, 286, 116, 122, weather, i);
		myWeather.drawLine(x, 286, x, 408, MYBLACK);
	}
	myWeather.drawRect(15, 286, maxX - 30, 122, MYBLACK);

	drawGraph(15, 408, 232, 122, "Temp 12h (C)", 0, 12, weather.hourlyTempRange[0], weather.hourlyTempRange[1], weather.hourlyMaxTemp, NULL);
	drawDualGraph(247, 408, 232, 122, "Regen 12h (mm/%)", 0, 12, 0, 100, weather.hourlyPop, 1, 0, weather.hourlyMaxRain, weather.hourlyRain);
	myWeather.drawLine(480, 408, 480, 530, MYBLACK);
	drawGraph(481, 408, 232, 122, "Temp 7Tage (C)", 0, 7, weather.forecastTempRange[0], weather.forecastTempRange[1], weather.forecastMinTemp, weather.forecastMaxTemp);
	drawDualGraph(713, 408, 232, 122, "Regen 7Tage (mm/%)", 0, 7, 0, 100, weather.forecastPop, 0, 0, weather.forecastMaxRain, weather.forecastRain);
	myWeather.drawRect(15, 408, maxX - 30, 122, MYBLACK);

	myWeather.pushCanvas(0, 0, UPDATE_MODE_GC16);
	myWeather.deleteCanvas();
}

/* Update only the M5Paper part of the global data */
void showM5PaperInfo() {
	Serial.println("ShowM5PaperInfo");

	myWeather.createCanvas(245, 251);

	myWeather.setTextSize(24);
	myWeather.setTextColor(MYBLACK, MYWHITE);
	myWeather.setTextDatum(TL_DATUM);

	myWeather.drawRect(0, 0, 245, 251, MYBLACK);
	drawM5PaperInfo(0, 0, 245, 251);

	myWeather.pushCanvas(697, 35, UPDATE_MODE_GC16);
	myWeather.deleteCanvas();
}
