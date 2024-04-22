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
//#include "games/games.h"


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
		"8. TEST IMPRIMANTE",
		"9. NETWORK"
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


	while (selected >= 0){
		selected = lcdmenu("MENU PRINCIPALE", menu, sizeof(menu) / 25, selected);
		switch (selected)
		{
			case 0:
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
				break;
			case 7:
				break;
			case 8:
				break;
			case 9:
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
