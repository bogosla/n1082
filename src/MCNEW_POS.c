/*
 ============================================================================
 Name        : MCNEW_POS.c
 Author      : DESTINE James
 Version     :
 Copyright   : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
/*
* main.c
* Author: James DESTINE
*/

#include <stdio.h>
#include <stdlib.h>
#include <seos.h>
#include <curl/curl.h>
#include "helpers.h"

//#include "games/auth.h"
#include "games/games.h"
#include "print.h"


int main(int argc, char *argv[])
{
    int selected = 0;

	const char menu[][25] = {
		"1. NOUVELLE FICHE",
		"2. MES FICHES",
		"3. RAPPORTS",
		"4. TIRAGES DISPONIBLE",
		"5. NUMEROS GAGNANTS",
		"6. FICHES GAGNANTES",
		"7. CONFIGURATION",
		"8. TEST IMPRIMANTE"
	};

    application_init(&argc, &argv);
	curl_global_init(CURL_GLOBAL_ALL); // Initialize curl for request http

	dfb_create_font(FONT_ROBOTO, 18, 0);
    dfb_create_font(FONT_ROBOTO, 16, 0);
	dfb_create_font(FONT_ROBOTO, 14, 0);

	lcdSetFont(FONT_ROBOTO, "UTF-8", 0,  16, 0);

    lcdSetFgColor(colorBlack);
	lcdSetBgColor(colorWhite);

	kbFlush(); // Flush Keyboard

//    selected = handleLogin();
//	char name[96];
//	int retType = 0;
//	memset(name, 0x00, sizeof(name));
const char *C = "[\n"
    "    {\n"
    "        \"code\": \"GG_MD\",\n"
    "        \"name\": \"Georgia Soir\",\n"
    "        \"status\": true,\n"
    "        \"start_time\": \"05:00:00\",\n"
    "        \"end_time\": \"18:45:00\"\n"
    "    },\n"
    "    {\n"
    "        \"code\": \"GG_MT\",\n"
    "        \"name\": \"Georgia Matin\",\n"
    "        \"status\": false,\n"
    "        \"start_time\": \"05:00:00\",\n"
    "        \"end_time\": \"12:25:00\"\n"
    "    },\n"
    "    {\n"
    "        \"code\": \"FL_S\",\n"
    "        \"name\": \"Florida Soir\",\n"
    "        \"status\": true,\n"
    "        \"start_time\": \"05:00:00\",\n"
    "        \"end_time\": \"21:25:00\"\n"
    "    },\n"
    "    {\n"
    "        \"code\": \"FL_M\",\n"
    "        \"name\": \"Florida Midi\",\n"
    "        \"status\": false,\n"
    "        \"start_time\": \"05:00:00\",\n"
    "        \"end_time\": \"13:25:00\"\n"
    "    },\n"
    "    {\n"
    "        \"code\": \"NY_S\",\n"
    "        \"name\": \"New York Soir\",\n"
    "        \"status\": true,\n"
    "        \"start_time\": \"05:00:00\",\n"
    "        \"end_time\": \"22:25:00\"\n"
    "    },\n"
    "    {\n"
    "        \"code\": \"NY_M\",\n"
    "        \"name\": \"New York Midi\",\n"
    "        \"status\": false,\n"
    "        \"start_time\": \"05:00:00\",\n"
    "        \"end_time\": \"14:25:00\"\n"
    "    }\n"
    "]";
	write_to_file(TIRAGES_FILE, C);


	while (selected >= 0)
	{
		selected = lcdmenu("MENU PRINCIPALE", menu, sizeof(menu) / 25, selected);
		switch (selected)
		{
			case 0:
				postFiches(NULL);
				break;
			case 1:
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				break;
			case 5:
				break;
			case 6:
				getConfiguration();
				break;
			case 7:
				TestPrinter();
				break;
			default:
				break;
		}
	}


    // Clean up stuffs
	curl_global_cleanup();
	application_exit();
	PPPLogout(PPP_DEV_GPRS);
//
	return EXIT_SUCCESS;
}
