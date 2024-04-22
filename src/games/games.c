/*
 * games.c
 *
 *  Created on: Apr 22, 2024
 *      Author: James DESTINE
 */

#include "games.h"


void GetNetwork(void)
{
	int status = 0, running = 1, key = 0;
	unsigned int percent = 0;

	battery_info_t battery_info;

	status = sys_battery_info(&battery_info);


	lcdclean();
	lcd_header(ALG_CENTER, "CONFIGURATION");

	if (status == 0)
	{
		percent = battery_info.percent;
	}

	lcdprintf(ALG_LEFT, "Battery Percent: %d%%", percent);


    lcdFlip();

	while (running == 1)
	{
		key = kbGetKey();
		switch(key)
		{
			case KEY_CANCEL: // KEY_CANCEL
				running = 0;
				break;
			default:
				break;
		}
	}
	return;
}


void getConfiguration(void)
{
	char imei[15];


	memset(imei, 0x00, sizeof(imei));
	sysReadSN(imei);

	return;
}
