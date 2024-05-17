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
#include "constants.h"

#include "auth.h"
#include "games/games.h"
#include "print.h"


int main(int argc, char *argv[])
{
    int selected = 0, yn = 0;

	const char menu[][25] = {
		"1. NOUVELLE FICHE",
		"2. MES FICHES",
		"3. RAPPORTS",
		"4. NUMEROS GAGNANTS",
		"5. FICHES GAGNANTES",
		"6. CONFIGURATION",
		"7. TEST IMPRIMANTE",
		"8. DECONNECTION"
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
	
	selected = postHandleLogin();


	while (selected >= 0)
	{
		selected = lcdmenu("MENU PRINCIPALE", menu, sizeof(menu) / 25, selected);
		switch (selected)
		{
			case 0:
				postFiches(NULL);
				break;
			case 1:
				getFiches();
				break;
			case 2:
				getReports();
				break;
			case 3:
				getLotsByDate();
				break;
			case 4:
				getTicketsWon();
				break;
			case 5:
				getConfiguration();
				break;
			case 6:
				TestPrinter();
				break;
			case 7:
				yn = yesNo("Quitter?");
				switch (yn)
				{
					case -10:
						// Reset contents in those files 
						write_to_file(TOKEN_FILE, "");
						write_to_file(TIRAGES_FILE, "");
						selected = -1;
						break;
					default:
						selected = 0;
						break;
				}
				break;
			case -1:	
				yn = yesNo("Quitter?");
				switch (yn)
				{
					case -10:
						// Reset contents in those files 
						write_to_file(TOKEN_FILE, "");
						write_to_file(TIRAGES_FILE, "");
						selected = -1;
						break;
					default:
						selected = 0;
						break;
				}
				break;
			default:
				break;
		}
	}


    // Clean up stuffs
	PPPLogout(PPP_DEV_GPRS);
	curl_global_cleanup();
	application_exit();
	return EXIT_SUCCESS;
}
