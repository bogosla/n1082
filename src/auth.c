/*
 * auth.c
 *
 *  Created on: Apr 24, 2024
 *      Author: James DESTINE
 */

#include "auth.h"


extern int SIZE_WIDGET;
// -------------------------- -------------------------------

int postHandleLogin(void)
{
	IDirectFB *dfb = NULL;
	IDirectFBSurface *main_surface = NULL;
	
	long status_code = 0;
	int state = -1, key;
	int width, height, font_height;
	int running = 1, current_y = 0;
	char imeiStr[25];

	char* server_url = NULL;
	char path[148];
	
	cJSON *json = NULL;
	cJSON *accessToken = NULL;
	cJSON *role = NULL;
	cJSON *user = NULL;
	cJSON *imei = NULL;
	cJSON *tirages = NULL;

    cJSON *data;

	char *json_string;
	char *buffer = NULL;
	Entry *entry = NULL;

	Entry phone_entry = {24, 0, 0, SIZE_WIDGET, "", 0, 1, 11, ALPHA, NORMAL};
    Entry password_entry = {24, 0, 0, SIZE_WIDGET, "", 0, 0, 25, ALPHA, HIDDEN};

	memset(imeiStr, 0x00, sizeof(imeiStr));
	memset(path, 0x00, sizeof(path));

	sysReadSN(imeiStr);

	dfb = dfb_get_directfb();
	main_surface = lcdGetSurface();
	lcdGetSize(&width, &height);
	font_height = lcdGetFontHeight();
	
	phone_entry.width = width - 24 * 2;
	password_entry.width = width - 24 * 2;
    data = cJSON_CreateObject();


	readServer(&server_url);
	
	while(running == 1)
	{
		lcdclean();
		lcd_header(ALG_CENTER, "CONNEXION");
		current_y = get_current_y();
		entry = phone_entry.active ? &phone_entry : &password_entry;
		main_surface->DrawString(main_surface, "Phone/Pseudo", -1, entry->x, current_y + 2, DSTF_TOPLEFT);

		phone_entry.y = current_y + (font_height + 3);

		main_surface->DrawString(main_surface, "MotDePasse", -1, entry->x, current_y + (font_height * 3), DSTF_TOPLEFT);
		current_y = current_y + (font_height * 3);
		current_y += font_height + 1;
		password_entry.y = current_y + 3;

		renderEntry(main_surface, &phone_entry);
		renderEntry(main_surface, &password_entry);
		lcdFlip();

		key = kbGetKey();
		handleEntryInput(key, entry);
	
		switch (key)
		{
			case KEY_UP: // KEY_UP
				phone_entry.active = !phone_entry.active;
				password_entry.active = !password_entry.active;
				break;
			case KEY_DOWN: // KEY_DOWN
				phone_entry.active = !phone_entry.active;
				password_entry.active = !password_entry.active;
				break;
			case KEY_ENTER: // KEY_ENTER
				// must be 'username' for the '.net'
				cJSON_AddStringToObject(data, "phone_or_email_or_username", phone_entry.text);
				cJSON_AddStringToObject(data, "password", password_entry.text);
				// Convert the JSON object to a string
				json_string = cJSON_Print(data);
				lcdclean();
				sprintf(path, "%s%s", server_url, "/api/new8210/app/login");

				if(make_post_request(path, json_string, &status_code, &buffer, NULL) < 0)
				{
					lcdprintfex(ALG_CENTER, height / 2 - font_height, "Veuillez vérifier votre connexion!");
					lcdFlip();
					state = -1;
					free(json_string);
					kbGetKey();
					continue;
				} else 
				{
					json = cJSON_Parse(buffer);
					if(json == NULL)
					{
						lcdclean();
						lcdprintf(ALG_LEFT, "Erreur %s", "Parsing JSON error");
						lcdFlip();
						state = -1;
						kbGetKey();
						continue;
					}
					else
					{
						if (status_code >= 200 && status_code <= 299)
						{
							accessToken = cJSON_GetObjectItemCaseSensitive(json, "accessToken");
							tirages = cJSON_GetObjectItemCaseSensitive(json, "tirages");
							user = cJSON_GetObjectItemCaseSensitive(json, "user");
							imei = cJSON_GetObjectItemCaseSensitive(user, "serie");
							role = cJSON_GetObjectItemCaseSensitive(user, "role");


							if (strcmp(role->valuestring, "vendor") == 0 && strcmp(imei->valuestring, imeiStr) == 0)
							{
								state = 0;
								write_to_file(TOKEN_FILE, accessToken->valuestring);
								write_to_file(USERNAME_FILE, buffer);

								if (buffer != NULL) {
									free(buffer);
								    buffer = NULL;
                                }

                                buffer = cJSON_Print(tirages);
								write_to_file(TIRAGES_FILE, buffer);
								memset(path, 0x00, sizeof(path));

                                if (buffer != NULL) {
									free(buffer);
								    buffer = NULL;
                                }
								
							} else
							{
								state = -1;
								lcdclean();
								lcdprintfex(ALG_CENTER, height / 2 - font_height, "Vous n'êtes pas autorisé!");
								lcdFlip();
								kbGetKey();
							}
						} else 
						{
							state = -1;
							lcdclean();
							lcdprintf(ALG_LEFT, "%s", buffer);
							lcdFlip();
							kbGetKey();
							continue;
						}
					}
				}
				running = 0;
				break;
			case KEY_CANCEL: // KEY_CANCEL
				running = 0;
				// for testing, dont need to authenticate
				// state = 0;
				state = -1;
				break;
			default:
				break;
		}
	}

	if (buffer != NULL) {
		free(buffer);
        buffer = NULL;
    }

	if (json != NULL) {
		cJSON_Delete(json);
        json = NULL;
    }

	if (data != NULL) {
		cJSON_Delete(data);
        data = NULL;
    }
	
	if (server_url) {
		free(server_url);
        server_url = NULL;
	}	
	return state;
}