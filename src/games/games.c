/*
 * games.c
 *
 *  Created on: Apr 22, 2024
 *      Author: James DESTINE
 */
#include "games.h"


void getConfiguration(void)
{
	IDirectFB *dfb = NULL;
	IDirectFBSurface *main_surface = NULL;
	IDirectFBSurface *content_surface = NULL;
	DFBSurfaceDescription surfdesc;
	IDirectFBFont *font = NULL;
	DFBRectangle rect;

	char *bufferUser = NULL;
	cJSON *json = NULL;

	cJSON *mariage_gratuit = NULL;
	cJSON *delete_fiche_min = NULL;
	cJSON *companyName = NULL;
	cJSON *first_name = NULL;
	cJSON *id = NULL;

	int status = 0, running = 1, key = 0, curY = 0, speedScroll = 12;
	unsigned int percent = 0;
	char imei[15];
	int ch = 0;

	int wscreen = 0, hscreen = 0;

	battery_info_t battery_info;
	memset(imei, 0x00, sizeof(imei));

	status = sys_battery_info(&battery_info);
	sysReadSN(imei);

	dfb = dfb_get_directfb();
	main_surface = lcdGetSurface();
	lcdGetSize(&wscreen, &hscreen);
	ch = lcdGetFontHeight();


	surfdesc.flags = DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT;
	surfdesc.caps  = DSCAPS_SYSTEMONLY;
	surfdesc.width = wscreen;
	surfdesc.height = hscreen + (hscreen / 3);

	dfb->CreateSurface(dfb, &surfdesc, &content_surface);

	main_surface->GetFont(main_surface, &font); // Get font from main_surface

	content_surface->SetFont(content_surface, font);
	content_surface->Clear(content_surface, colorWhite.r, colorWhite.g, colorWhite.b, 0xFF);
	content_surface->SetColor(content_surface, colorBlack.r, colorBlack.g, colorBlack.b, 0xFF);

	lcdclean();
	lcd_header(ALG_CENTER, "CONFIGURATION");
	curY = get_current_y();

	rect.x = 0;
	rect.y = 1;
	rect.w = wscreen;
	rect.h = hscreen;

	read_from_file(USERNAME_FILE, &bufferUser);
	json = cJSON_Parse(bufferUser);

	if (status == 0)
	{
		percent = battery_info.percent;
	}

	if (json != NULL)
	{
		mariage_gratuit = cJSON_GetObjectItemCaseSensitive(json, "mariage_free");
		delete_fiche_min = cJSON_GetObjectItemCaseSensitive(json, "delete_ticket_min");	
		companyName = cJSON_GetObjectItemCaseSensitive(json, "bank_name");		
		id = cJSON_GetObjectItemCaseSensitive(json, "id");	
		first_name = cJSON_GetObjectItemCaseSensitive(json, "first_name");	

		lcdprintfon(ALG_LEFT, content_surface, wscreen, hscreen, 1, "Company : %s", companyName->valuestring);
		lcdprintfon(ALG_LEFT, content_surface, wscreen, hscreen, ch, "Vendor  : %s", first_name->valuestring);
		lcdprintfon(ALG_LEFT, content_surface, wscreen, hscreen, ch * 2, "Bank       : %d", id->valueint);
		lcdprintfon(ALG_LEFT, content_surface, wscreen, hscreen, ch * 3, "MA Free  : %s", cJSON_IsTrue(mariage_gratuit)? "Oui" : "Non");
		lcdprintfon(ALG_LEFT, content_surface, wscreen, hscreen, ch * 4, "Del Time : %d", delete_fiche_min->valueint);
		lcdprintfon(ALG_LEFT, content_surface, wscreen, hscreen, ch * 5, "Serial N0: %s", imei);
		lcdprintfon(ALG_LEFT, content_surface, wscreen, hscreen, ch * 6, "Battery   : %d%%", percent);
	}

	main_surface->Blit(main_surface, content_surface, &rect, 0, curY);
    lcdFlip();

	while (running == 1)
	{
		key = kbGetKey();
		switch(key)
		{
			case KEY_UP: // KEY_UP
				rect.y = rect.y - speedScroll;
				if (rect.y <= 0)
					rect.y = 1;
				break;
			case KEY_DOWN: // KEY_DOWN
				rect.y = rect.y + speedScroll;
				if (rect.y >  hscreen + (hscreen / 3))
					rect.y =  hscreen + (hscreen / 3);
				break;
			case KEY_CANCEL: // KEY_CANCEL
				running = 0;
				break;
			default:
				break;
		}
		usleep(10000);
		lcdclean();
		lcd_header(ALG_CENTER, "CONFIGURATION");
		main_surface->Blit(main_surface, content_surface, &rect, 0, curY);
		lcdFlip();
	}

	if (bufferUser != NULL) {
		free(bufferUser);
		bufferUser = NULL;
	}

	if (json != NULL) {
		cJSON_Delete(json);
		json = NULL;
	}
	return;
}



int getTirageType(Tirage **selectedTirage, int *sizeTirage, char *name, char *idName)
{
	Tirage *menu = NULL;
	char *bufferTirages = NULL;
	cJSON *json = NULL;
	int size = 0, toret = -1, selected = 0;

	if (fileExist(TIRAGES_FILE) >= 0)
		read_from_file(TIRAGES_FILE, &bufferTirages);

	json = cJSON_Parse(bufferTirages);

	
	if (json != NULL)
	{
		if (cJSON_IsArray(json))
		{
			cJSON *element;
			cJSON_ArrayForEach(element, json) 
			{
				cJSON *id = cJSON_GetObjectItemCaseSensitive(element, "code");
				cJSON *name = cJSON_GetObjectItemCaseSensitive(element, "name");
				addItem(&menu, &size, id->valuestring, name->valuestring);
			}
			toret = 0;
		}
	} else
		toret = -1;


	if (bufferTirages != NULL) {
		free(bufferTirages);
		bufferTirages = NULL;
	}
	// TODO, check size
	
	selected = 0;
	int id = 0, idToDelete = 0;

	while (selected >= 0)
	{
		selected = lcdmenu_tirage("TIRAGES", menu, size, selected, &id, *selectedTirage, *sizeTirage);
		if (selected < 0)
			toret = -1;
		else if (selected == 14)
		{
			idToDelete = hasInfoTirageByName(*selectedTirage, *sizeTirage, menu[id].name);
			if (idToDelete >= 0)
			{
				*sizeTirage = deleteInfoTirageByIndex(*selectedTirage, idToDelete, *sizeTirage);
			} else
				addItem(selectedTirage, sizeTirage,  menu[id].id,  menu[id].name);
			selected = id;
		} else if (selected == 15) {
			toret = 0;
			strcpy(name, menu[id].name);
			// *idName = menu[id].id;
			strcpy(idName, menu[id].id);
			selected = -1;
		}
	}

	freeItems(menu, size);

	if (json != NULL) {
		cJSON_Delete(json);
		json = NULL;
	}
	return toret;
}



static int displayGrid(const Ticket item, int selected)
{
	char url[192];
	char *buffer = NULL;
	char *bufferToken = NULL;
	char *server = NULL;
	long status_code = 0;

	char *boules = NULL;

	int screen_width, screen_height, font_height;	

	const char menu[][25] = {
		"1. Reimprimer",
		"2. Rejouer"
	};

	int _selected = (int)selected;
	int selected3 = 0;
	readServer(&server);
	lcdGetSize(&screen_width, &screen_height);
	font_height = lcdGetFontHeight();

	read_from_file(TOKEN_FILE, &bufferToken);
	sprintf(url, "%s/api/new8210/app/ticket-id?keyId=%s", server, item.id);


	if (make_get_request(url, &status_code, &buffer, bufferToken) < 0)
	{
		while(1)
			if (kbGetKey() == KEY_CANCEL)
				break;
	} else
	{
		if (status_code >= 200 && status_code <= 299) 
		{
			while (selected3 >= 0)
			{
				selected3 = lcdmenu("PLUS OPTIONS", menu, sizeof(menu) / 25, selected3);
				switch (selected3)
				{
				case 0:
					if (item.delete == 0)
						reprint_fiche(buffer);
					break;
				case 1:
					if (item.delete == 0) {
						postFiches(buffer);
						selected3 = -1;
						_selected = -1;
					}
					break;
				default:
					break;
				}
			}
		} else {
			lcdclean();
			lcdprintfex(ALG_CENTER, screen_height / 2 - font_height, "Pa Jwenn Fich Pou ID Sa.");
			lcdSetFont(FONT_ROBOTO, "UTF-8", 0,  15, 0);
			lcdprintfex(ALG_LEFT, screen_height - font_height, "CANCEL=Retounen");
			lcdSetFont(FONT_ROBOTO, "UTF-8", 0,  18, 0);

			lcdFlip();
			while(1)
				if (kbGetKey() == KEY_CANCEL)
					break;
		}
	}
	
	if (buffer != NULL) {
		free(buffer);
		buffer = NULL;
	}

	if (bufferToken != NULL) {
		free(bufferToken);
		bufferToken = NULL;
	}

	if (server != NULL) {
		free(server);
		server = NULL;
	}
					
	return _selected;
}



static int make_get_fiches(char *start_date, char *end_date)
{
	cJSON *json = NULL;
	cJSON *json2 = NULL;
	cJSON *tickets = NULL;
	cJSON *count = NULL;
	cJSON *page = NULL;
	cJSON *num_pages = NULL;

	cJSON *data = NULL;
	char *json_string = NULL;
	char url[250];
	char *buffer = NULL;
	char *bufferToken = NULL;	
	long status_code = 0;
	int size = 0;

	int screen_width, screen_height, font_height;	
	char* server = NULL;

	memset(url, 0x00, sizeof(url));
	Ticket *items = NULL;
	BouleItem *boules = NULL;
	int size2 = 0;
	int selected = 0, paginate = 0, tpage = 1;
	char htext[192];


	lcdGetSize(&screen_width, &screen_height);
	font_height = lcdGetFontHeight();
	readServer(&server);
	read_from_file(TOKEN_FILE, &bufferToken);

	while (paginate == 0) {
		selected = 0;
		size = 0;
		sprintf(url, "%s/api/new8210/app/tickets?start_date=%s&end_date=%s&page_size=30&page=%d", server, start_date, end_date, tpage);
		paginate = 1;
		if (make_get_request(url, &status_code, &buffer, bufferToken) < 0)
		{
			while(1)
				if (kbGetKey() == KEY_CANCEL)
					break;
			paginate = 1;
		} else
		{
			if (status_code >= 200 && status_code <= 299) 
			{
				json = cJSON_Parse(buffer);		
				
				if (json != NULL)
				{
					tickets = cJSON_GetObjectItemCaseSensitive(json, "data");
					count = cJSON_GetObjectItemCaseSensitive(json, "count");
					page = cJSON_GetObjectItemCaseSensitive(json, "page");
					num_pages = cJSON_GetObjectItemCaseSensitive(json, "num_pages");

					if (cJSON_IsArray(tickets))
					{
						cJSON *element;
						cJSON_ArrayForEach(element, tickets) 
						{
							int is_delete = 0;
							cJSON *id = cJSON_GetObjectItemCaseSensitive(element, "ref_code");
							cJSON *_id = cJSON_GetObjectItemCaseSensitive(element, "id");
							cJSON *tirage_name = cJSON_GetObjectItemCaseSensitive(element, "tirage_name");
							cJSON *montant = cJSON_GetObjectItemCaseSensitive(element, "montant");
							cJSON *delete = cJSON_GetObjectItemCaseSensitive(element, "delete");
							if (strcmp(delete->valuestring, "DELETE") == 0) {
								is_delete = 1;
							}
							addTirageItem(&items, &size, id->valuestring, NULL, tirage_name->valuestring, montant->valuestring, _id->valueint, is_delete);
						}
					}
				} else
				{
					lcdprintf(ALG_LEFT, "JSON error parsing");
					lcdFlip();
					kbGetKey();
					paginate = 1;
				}

				if (json != NULL) {
					cJSON_Delete(json);
					json = NULL;
				}
				if (buffer != NULL) {
					free(buffer);
					buffer = NULL;
				}

				int id = -1;

				if (size == 0)
				{
					lcdclean();
					lcdprintfex(ALG_CENTER, screen_height / 2 - font_height, "PAS DE TICKETS!!");
					lcdprintfex(ALG_LEFT, screen_height - font_height, "CANCEL=retour");
					lcdFlip();
					while(1)
						if (kbGetKey() == KEY_CANCEL)
							break;
				}
				memset(url, 0x00, sizeof(url));
				memset(htext, 0x00, sizeof(htext));

				sprintf(url, "%s/api/new8210/app/ticket-id", server);
				sprintf(htext, "FICHES (%d) PAGE %d : %d", count->valueint, page->valueint, num_pages->valueint);

				
				while (selected >= 0 && size > 0)
				{
					size2 = 0;
					selected = lcdmenu_ticket(htext, items, size, selected, &id);
					if (selected == -7) {
						// TODO
						int _page = askPage("Entrer Page");
						if (_page > 0) {
							paginate = 0;
							tpage = _page;
							selected = -1;
						} else 
							selected = id;
					}
					else if (selected == -8)
					{
						// delete tirage 
						data = cJSON_CreateObject();
						cJSON_AddStringToObject(data, "keyId", items[id].id);
						int yn = yesNo("Supprimer Fiche?");

						switch (yn)
						{
							case -10:
								json_string = cJSON_Print(data);
								status_code = 0;
								if (make_http_request(url, json_string, &status_code, &buffer, bufferToken, "DELETE") >= 0)
								{
									if (status_code >= 200 && status_code <= 299) 
									{
										// size = id; // deleteTirageByIndex(items, id, size);
										// selected = size;
										items[id].delete = 1;
									}
								} else {
									lcdclean();
									lcdprintf(ALG_LEFT, "%s", buffer);
									lcdFlip();
									while(1)
										if (kbGetKey() == KEY_CANCEL)
											break;
								}
								if (buffer != NULL) {
									free(buffer);
									buffer = NULL;
								}
								if (json_string != NULL) {
									free(json_string);
									json_string = NULL;
								}
								selected = id;			
								break;
							default:
								selected = 0;
								break;
						}
						if (data != NULL) {
							cJSON_Delete(data);
							data = NULL;
						}
					} else if (selected >= 0)
					{
						selected = displayGrid((Ticket)items[selected], selected);
					}
				}
			} else 
			{
				lcdclean();
				lcdprintf(ALG_LEFT, "%s", buffer);
				lcdFlip();
				while(1)
					if (kbGetKey() == KEY_CANCEL)
						break;
				paginate = 1;
			}
		}
	}
	freeTirageItems(items, size);
	if (buffer != NULL) {
		free(buffer);
		buffer = NULL;

	}
	if (bufferToken != NULL) {
		free(bufferToken);
		bufferToken = NULL;
	}
	if (server != NULL) {
		free(server);
		server = NULL;
	}
	items = NULL;
	return 0;
}



void postFiches(const char *buffBoules)
{
	IDirectFBSurface *main_surface = NULL;
	int key = 0, running = 1, font_height = 0, current_y, stop = -1;
	int tip = 0;
	int sselected = 0;
	List *list = NULL;
	char name[96];
	char id[32];
	int screen_width, screen_height;
	int selected = 0, selected2 = 0, running3 = 0, key3 = 0;

	Entry *entry = NULL;
	cJSON *json = NULL;
	cJSON *json_boules = NULL;


	Tirage *tirages = NULL;
	int sizeTirage = 0;

	const char menu[][25] = {
		"1. Imprimer",
		"2. Pair Auto",
		"3. L3 Auto",
		"4. Revers",
		"5. Mariage Auto",
		"6. L4 Auto",
		"7. Pointe"
	};

	memset(name, 0x00, sizeof(name));
	memset(id, 0x00, sizeof(id));

    list = createList();

	// if buffBoules, there is an initial tirage boules
	// else, start empty
	if (buffBoules != NULL)
	{
		json = cJSON_Parse(buffBoules);
		if (json != NULL)
		{
			json_boules = cJSON_GetObjectItemCaseSensitive(json, "boules");
			if (cJSON_IsArray(json_boules))
			{
				cJSON *element;
				// Iterate over the array elements
				cJSON_ArrayForEach(element, json_boules) 
				{
					cJSON *lotto = cJSON_GetObjectItemCaseSensitive(element, "lotto");
					cJSON *boule = cJSON_GetObjectItemCaseSensitive(element, "boule");
					cJSON *montant = cJSON_GetObjectItemCaseSensitive(element, "montant");
					cJSON *option = cJSON_GetObjectItemCaseSensitive(element, "option");
					BouleItem _new = {0,"", "", "", ""};
					sprintf(&_new.pri, "%.2f", montant->valuedouble);
					strcpy(&_new.boul, boule->valuestring);
					strcpy(&_new.lotto, lotto->valuestring);
					strcpy(&_new.option, option->valuestring);

					addElement(list, _new); // add boule in list boules
				}
				// cJSON_Delete(json);
			}
		}
		cJSON_Delete(json);
		json = NULL;
	}

    if (list == NULL) {
		lcdprintf(ALG_CENTER, "Erreur! Creation ticket!");
		lcdFlip();
		kbGetKey();
        return;
    }

	lcdclean();
	main_surface = lcdGetSurface();
	font_height = lcdGetFontHeight();
	lcdGetSize(&screen_width, &screen_height);
	running3 = 1;
	int state = -1, canSelectBoulePaire = 1, canSelectBouleTriple = 1;

	Entry ppri_entry = {screen_width / 2 - 80, 0, 210, SIZE_WIDGET, "", 0, 1, 6, 'A'};


	// get lotto
	// return -1 if leave
	tip = getTirageType(&tirages, &sizeTirage, name, id);

	if (tip == -1)
		selected = -1;

	while (selected >= 0)
	{
		selected = editableList(list->items, list->size, selected, name, &sselected, tirages, sizeTirage);
		
		selected2 = 0;
		switch(selected) 
		{
			// ENTER KEY
			case 10:
				lcdclean();
				running = 1;
				selected = list->size;
				Entry boul_entry = {screen_width / 2 - 90, 0, 210, SIZE_WIDGET, "", 0, 1, 5, 'A'};
				Entry pri_entry = {screen_width / 2 - 90, 0, 210, SIZE_WIDGET, "", 0, 0, 6, 'A'};
				BouleItem _new = {0, "", "", "", '1'};

				while(running == 1)
				{			
					lcdclean();
					lcd_header(ALG_CENTER, "ENTRER BOULE");
					current_y = get_current_y();
					entry = boul_entry.active ? &boul_entry : &pri_entry;
					lcdprintfex(ALG_LEFT, current_y + 7, "Boule  :");
					boul_entry.y = current_y + 7;
					renderEntry(main_surface, &boul_entry);

					current_y = get_current_y();
					lcdprintfex(ALG_LEFT, current_y + (font_height), "Mise    :");
					pri_entry.y = current_y + font_height;
					renderEntry(main_surface, &pri_entry);
					lcdFlip();
					key = kbGetKey();

					handleEntryInput(key, entry);
					switch (key)
					{
						case KEY_UP: // KEY_UP
							boul_entry.active = !boul_entry.active;
							pri_entry.active = !pri_entry.active;
							break;
						case KEY_DOWN: // KEY_DOWN
							boul_entry.active = !boul_entry.active;
							pri_entry.active = !pri_entry.active;
							break;
						case KEY_ENTER:
							if (boul_entry.cursor_pos > 1 && pri_entry.cursor_pos > 0)
							{
								sprintf(&_new.boul, "%s", boul_entry.text);
								sprintf(&_new.pri, "%s", pri_entry.text);
								setOptionBouleItem(&_new);
								running = 0;
								state = 1;
							}
							break;
						case KEY_CANCEL:
							running = 0;
							state = -1;
							break;
						default:
							break;
					}
				}

				Entry wi_entry = {screen_width / 2 - 90, 0, 35, SIZE_WIDGET, "Oui", 2, 1, 2, 'A'};
				Entry non_entry = {screen_width / 2 - 90, 0, 35, SIZE_WIDGET, "Non", 3, 0, 3, 'A'};

				int MA = 0;
				if (strlen(_new.boul) == 4 && state == 1)
				{
					running = 1;
					while (running == 1)
					{
						lcdclean();	
						lcd_header(ALG_CENTER, "ENTRER BOULE");

						current_y = get_current_y();
						lcdprintfex(ALG_LEFT, current_y + 7, "Boule  :");
						boul_entry.y = current_y + 7;
						boul_entry.active = 0;
						renderEntry(main_surface, &boul_entry);
						current_y = get_current_y();
						lcdprintfex(ALG_LEFT, current_y + (font_height), "Mise     :");
						pri_entry.y = current_y + (font_height - 7);
						pri_entry.active = 0;
						renderEntry(main_surface, &pri_entry);
						lcdFlip();
						// ********************************************************************
						current_y = get_current_y() + font_height;
						entry = wi_entry.active ? &wi_entry : &non_entry;
						lcdprintfex(ALG_LEFT, current_y, "Marriage? ");
						wi_entry.y = current_y;

						renderEntry(main_surface, &wi_entry);
						non_entry.x = 130;
						non_entry.y = wi_entry.y;
						renderEntry(main_surface, &non_entry);
						lcdFlip();
						
						key = kbGetKey();
						switch (key)
						{
							case KEY_UP: // KEY_UP
								wi_entry.active = !wi_entry.active;
								non_entry.active = !non_entry.active;
								break;
							case KEY_DOWN: // KEY_DOWN
								wi_entry.active = !wi_entry.active;
								non_entry.active = !non_entry.active;
								break;
							case KEY_ENTER:
								if (strcmp(entry->text, "Oui") == 0)
								{
									MA = 1;
									sprintf(&_new.lotto, "%s", "MA");
								}
								else
								{
									MA = 0;
									sprintf(&_new.lotto, "%s", "L4");
								}
								state = 1;
								running = 0;
								break;
							case KEY_CANCEL:
								running = 0;
								state = -1;
								break;
							default:
								break;
						}
						
					}
				}
				
				Entry opsyon_entry = {screen_width / 2 - 90, 0, 35, SIZE_WIDGET, "1", 1, 1, 1, 'A'};

				if (strlen(_new.boul) >= 4 && state == 1 && (MA == 0))
				{
					running = 1;
					while (running == 1)
					{
						lcdclean();
						lcd_header(ALG_CENTER, "ENTRER BOULE");

						current_y = get_current_y();
						lcdprintfex(ALG_LEFT, current_y + 7, "Boule  :");

						boul_entry.y = current_y + 7;
						renderEntry(main_surface, &boul_entry);
						current_y = get_current_y();
						lcdprintfex(ALG_LEFT, current_y + font_height, "Mise     :");

						pri_entry.y = current_y + (font_height - 7);
						renderEntry(main_surface, &pri_entry);
						lcdFlip();
						
						// ***************************************************************
						current_y = get_current_y() + font_height;
						lcdprintfex(ALG_LEFT, current_y, "Option: ");
						opsyon_entry.y = current_y;
						renderEntry(main_surface, &opsyon_entry);
						lcdFlip();
						key = kbGetKey();
						if ((key >=  0x31 && key <= 0x33) || key == 0x08)
						{
							handleEntryInput(key, &opsyon_entry);
						}
						switch (key)
						{
							case KEY_ENTER:
							if (opsyon_entry.cursor_pos > 0)
							{
								sprintf(&_new.option, opsyon_entry.text);
								state = 1;
							}
								running = 0;
								break;
							case KEY_CANCEL:
								running = 0;
								state = -1;
								break;
							default:
								break;
						}
					
					}
				}
				if (state == 1)
				{
					addElement(list, _new);
				}
				break;
			// FUNC KEY
			case 11:
				stop = -1;
				while (selected2 >= 0)
				{
					selected2 = lcdmenu("PLUS OPTIONS", menu, sizeof(menu) / 25, selected2);
					running3 = 1;
					switch(selected2) 
					{
						case 0:
							if (make_post_fiches(list, id, tirages, sizeTirage) == 0)
							{
								canSelectBoulePaire = 1;
								canSelectBouleTriple = 1;
								destroyList(list);
								// freeItems(tirages, sizeTirage);
								list = createList();
								selected2 = -1;
								if (buffBoules != NULL)
								{
									selected2 = -1;
									stop = 0;
								}
							}
							break;
						case 1:
							if (canSelectBoulePaire == 1)
							{
								while (running3 == 1)
								{
									lcdclean();
									lcd_header(ALG_CENTER, "MISE");
									current_y = get_current_y();
									lcdprintfex(ALG_LEFT, current_y + 7, "Mise  :");
									ppri_entry.y = current_y + 7;
									renderEntry(main_surface, &ppri_entry);
									lcdFlip();
									key3 = kbGetKey();
									handleEntryInput(key3, &ppri_entry);

									switch (key3)
									{
									case KEY_CANCEL:
										running3 = 0;
										break;
									case KEY_ENTER:
										if (ppri_entry.cursor_pos > 0)
										{
											addBoulePaire(list, ppri_entry.text);
											canSelectBoulePaire = -1;
											running3 = 0;
										}
										
										break;
									default:
										break;
									}		
								}
								selected2 = -1;
							}
							break;
						case 2:
							if (canSelectBouleTriple == 1)
							{
								while (running3 == 1)
								{
									lcdclean();
									lcd_header(ALG_CENTER, "MISE");
									current_y = get_current_y();
									lcdprintfex(ALG_LEFT, current_y + 7, "Mise   :");
									ppri_entry.y = current_y + 7;
									renderEntry(main_surface, &ppri_entry);
									lcdFlip();
									key3 = kbGetKey();
									handleEntryInput(key3, &ppri_entry);

									switch (key3)
									{
									case KEY_CANCEL:
										running3 = 0;
										break;
									case KEY_ENTER:
										if (ppri_entry.cursor_pos > 0)
										{
											addBouleTriple(list, ppri_entry.text);
											canSelectBouleTriple = -1;
											running3 = 0;
										}										
										break;
									default:
										break;
									}		
								}
								selected2 = -1;
							}
							break;
						case 3:
							while (running3 == 1)
							{
								lcdclean();
								lcd_header(ALG_CENTER, "MISE");
								current_y = get_current_y();
								lcdprintfex(ALG_LEFT, current_y + 7, "Mise  :");
								ppri_entry.y = current_y + 7;
								renderEntry(main_surface, &ppri_entry);
								lcdFlip();
								key3 = kbGetKey();
								handleEntryInput(key3, &ppri_entry);

								switch (key3)
								{
								case KEY_CANCEL:
									running3 = 0;
									break;
								case KEY_ENTER:
									if (ppri_entry.cursor_pos > 0)
									{
										addBouleRevers(list, ppri_entry.text);
										running3 = 0;
									}
									break;
								default:
									break;
								}		
							}
							selected2 = -1;
							break;
						case 4:
							while (running3 == 1)
							{
								lcdclean();
								lcd_header(ALG_CENTER, "MISE");
								current_y = get_current_y();
								lcdprintfex(ALG_LEFT, current_y + 7, "Mise  :");
								ppri_entry.y = current_y + 7;
								renderEntry(main_surface, &ppri_entry);
								lcdFlip();
								key3 = kbGetKey();
								handleEntryInput(key3, &ppri_entry);

								switch (key3)
								{
								case KEY_CANCEL:
									running3 = 0;
									break;
								case KEY_ENTER:
									if (ppri_entry.cursor_pos > 0)
									{
										addBouleMarriage(list, ppri_entry.text);
										running3 = 0;
									}
									break;
								default:
									break;
								}		
							}
							selected2 = -1;
							break;
						case 5: // lotto 4
							while (running3 == 1)
							{
								lcdclean();
								lcd_header(ALG_CENTER, "MISE");
								current_y = get_current_y();
								lcdprintfex(ALG_LEFT, current_y + 7, "Mise  :");
								ppri_entry.y = current_y + 7;
								renderEntry(main_surface, &ppri_entry);
								lcdFlip();
								key3 = kbGetKey();
								handleEntryInput(key3, &ppri_entry);

								switch (key3)
								{
								case KEY_CANCEL:
									running3 = 0;
									break;
								case KEY_ENTER:
									if (ppri_entry.cursor_pos > 0)
									{
										addBouleL4(list, ppri_entry.text);
										running3 = 0;
									}
									break;
								default:
									break;
								}		
							}
							selected2 = -1;
							break;
						case 6: // pwent
							running3 = 1;
							// Pwent, pri
							Entry bboul_entry = {screen_width / 2 - 90, 0, 210, SIZE_WIDGET, "", 0, 1, 1, 'A'};
							Entry ppri_entry = {screen_width / 2 - 90, 0, 210, SIZE_WIDGET, "", 0, 0, 6, 'A'};
							
							while (running3 == 1)
							{
								lcdclean();
								lcd_header(ALG_CENTER, "POINTE");
								current_y = get_current_y();
								entry = bboul_entry.active ? &bboul_entry : &ppri_entry;

								lcdprintfex(ALG_LEFT, current_y + 7, "Pointe :");
								bboul_entry.y = current_y + 7;
								renderEntry(main_surface, &bboul_entry);
								current_y = get_current_y();

								lcdprintfex(ALG_LEFT, current_y + (font_height), "Mise   :");
								ppri_entry.y = current_y + font_height;
								renderEntry(main_surface, &ppri_entry);
								lcdFlip();
								key3 = kbGetKey();
								handleEntryInput(key3, entry);

								switch (key3)
								{
									case KEY_UP: // KEY_UP
										bboul_entry.active = !bboul_entry.active;
										ppri_entry.active = !ppri_entry.active;
										break;
									case KEY_DOWN: // KEY_DOWN
										bboul_entry.active = !bboul_entry.active;
										ppri_entry.active = !ppri_entry.active;
										break;
									case KEY_CANCEL:
										running3 = 0;
										break;
									case KEY_ENTER:
										if (ppri_entry.cursor_pos > 0 && bboul_entry.cursor_pos > 0)
										{
											// addBouleL4(list, ppri_entry.text);
											addPwent(list, ppri_entry.text, atoi(bboul_entry.text));
											running3 = 0;
										}
										break;
									default:
										break;
								}		
							}
							selected2 = -1;
							break;
						default:
							break;
					}
				}
				selected = list->size;
				if (stop == 0)
					selected = -1;
				break;
			// CLEAR KEY
			case 12:
				deleteByIndex(list, sselected);
				selected = list->size;

				break;
			default:
				break;
		}
	
	}
	destroyList(list);
	freeItems(tirages, sizeTirage);
	sizeTirage = 0;
	return;
} // => POST




int make_post_fiches(const List *list, const char *id_tirage, Tirage *tirages, int sizeTirage)
{
	long status_code = 0;
	char *buffer = NULL;
	char *bufferToken = NULL;	
	cJSON *root = NULL;
	cJSON *arrayBoul = NULL;
	cJSON *ficheData = NULL;

	float montantTotal = 0.0f;

	char url[64];

	cJSON *json2 = NULL;

	int state = 0;

	char* bufferUser = NULL;
	char* server = NULL;

	memset(url, 0x00, sizeof(url));
	read_from_file(TOKEN_FILE, &bufferToken);
	readServer(&server);

	root = cJSON_CreateObject();
	arrayBoul = cJSON_CreateArray();
	ficheData = cJSON_CreateArray();

	srand(time(NULL));
	
	int i = 0;
	while (i < list->size)
	{
		cJSON *tirage = cJSON_CreateObject();
		cJSON_AddStringToObject(tirage, "boule", list->items[i].boul);
		cJSON_AddStringToObject(tirage, "option", list->items[i].option);
		cJSON_AddStringToObject(tirage, "lotto", list->items[i].lotto);
		cJSON_AddNumberToObject(tirage, "montant", atoi(list->items[i].pri));
		montantTotal = montantTotal + atoi(list->items[i].pri);
		cJSON_AddItemToArray(arrayBoul, tirage);
		i++;
	}

	if (sizeTirage == 0)
	{
		cJSON *tir = cJSON_CreateObject();
		cJSON_AddStringToObject(tir, "tirage", id_tirage);
        cJSON_AddItemReferenceToObject(tir, "bouleLists", arrayBoul);

		cJSON_AddItemToArray(ficheData, tir);
		cJSON_AddItemToObject(root, "ticketData", ficheData);
	} else 
	{
		int x = 0;
		while (x < sizeTirage)
		{
			cJSON *tir = cJSON_CreateObject();
			cJSON_AddStringToObject(tir, "tirage", tirages[x].id);
            cJSON_AddItemReferenceToObject(tir, "bouleLists", arrayBoul);

			cJSON_AddItemToArray(ficheData, tir);
			x++;
		}
		cJSON_AddItemToObject(root, "ticketData", ficheData);
	}
	
	char *jsonStr = cJSON_Print(root);
	sprintf(url, "%s/api/mobile/app/create-ticket", server);

	if (make_post_request(url, jsonStr, &status_code, &buffer, bufferToken) < 0)
	{
		while(1)
			if (kbGetKey() == KEY_CANCEL)
				break;
		state = -1;
	} else
	{
		if (status_code >= 200 && status_code <= 299) 
		{
			state = 0;
			print_fiche(buffer);
		} else 
		{
			lcdprintf(ALG_LEFT, "%s", buffer);
			lcdFlip();
			while(1)
				if (kbGetKey() == KEY_CANCEL)
					break;
			state = -1;
		}
	}
	
	if (jsonStr != NULL)
	{
		free(jsonStr);
		jsonStr = NULL;
	}
	if (bufferToken != NULL)
	{
		free(bufferToken);
		bufferToken = NULL;
	}
	if (bufferUser != NULL)
	{
		free(bufferUser);
		bufferUser = NULL;
	}
	if (buffer != NULL)
	{
		free(buffer);
		buffer = NULL;
	}
	if (server != NULL)
	{
		free(server);
		server = NULL;
	}
	if (arrayBoul != NULL) {
		cJSON_Delete(arrayBoul);
		arrayBoul = NULL;

	}
	if (root != NULL) {
		cJSON_Delete(root);
		root = NULL;
	}
	return state;
}



void getLotsGagnants(void)
{
	long status_code = 0;
	char url[192];
	char *bufferToken = NULL;
	char *buffer = NULL;
	char start_date[11];
	cJSON *json = NULL;

	struct tm *timeinfo;
	time_t rawtime;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	char** server = NULL;
	int width, height, font_height;

	memset(url, 0x00, sizeof(url));
	memset(start_date, 0x00, sizeof(start_date));
	lcdGetSize(&width, &height);
	font_height = lcdGetFontHeight();

	read_from_file(TOKEN_FILE, &bufferToken);
	readServer(&server);


	sprintf(start_date, "%4d-%02d-%02d", timeinfo->tm_year + 1900, timeinfo->tm_mon+1, timeinfo->tm_mday);
	sprintf(url, "%s/api/mobile/app/lot-gagnant?date=%s", server, start_date);
	
	if (make_get_request(url, &status_code, &buffer, bufferToken) < 0)
	{
		lcdprintfex(ALG_CENTER, height / 2 - font_height, "Verifye entenet ou!!!");
		lcdFlip();
		while(1)
			if (kbGetKey() == KEY_CANCEL)
				break;
	} else
	{
		if (status_code >= 200 && status_code <= 299) 
		{
			lcdclean();
			lcd_header(ALG_CENTER, "LOTS GAGNANTS");

			json = cJSON_Parse(buffer);
			if (json != NULL)
			{
				if (cJSON_IsArray(json))
				{
					cJSON *element;
					cJSON_ArrayForEach(element, json) 
					{
						cJSON *list = cJSON_GetObjectItemCaseSensitive(element, "list");
						if (cJSON_IsArray(list)) 
						{
							cJSON *itemList;
							cJSON_ArrayForEach(itemList, list) 
							{
								cJSON *name = cJSON_GetObjectItemCaseSensitive(itemList, "id");
								cJSON *t1 = cJSON_GetObjectItemCaseSensitive(itemList, "name");
								cJSON *t2 = cJSON_GetObjectItemCaseSensitive(itemList, "lots");
								cJSON *t3 = cJSON_GetObjectItemCaseSensitive(itemList, "date");

								lcdprintf(ALG_LEFT, "%s", name->valuestring);
							}
						}
						cJSON_Delete(list);
					}
				}
				lcdFlip();
			} else 
			{
				lcdprintf(ALG_LEFT, "Json GT NULL.");
				lcdFlip();
			}
		} else {
			lcdprintf(ALG_LEFT, "%s", buffer);
			lcdFlip();
		}

		while(1)
			if (kbGetKey() == KEY_CANCEL)
				break;
	}

	
	if (json != NULL) {
		cJSON_Delete(json);
		json = NULL;
	}

	if (bufferToken != NULL) {
		free(bufferToken);
		bufferToken = NULL;
	}

	if (buffer != NULL) {
		free(buffer);
		buffer = NULL;
	}

	if (server != NULL) {
		free(server);
		server = NULL;
	}
	return;
}



void getLotsByDate(void)
{
	IDirectFBSurface *main_surface = NULL;
	int screen_width, screen_height, font_height, speedScroll = 12, i = 0;

	IDirectFB *dfb = NULL;
	IDirectFBSurface *content_surface = NULL;
	DFBSurfaceDescription surfdesc;
	IDirectFBFont *font = NULL;
	DFBRectangle rect;	

	cJSON *json = NULL;
	cJSON *json2 = NULL;

	char url[148];
	char *buffer = NULL;
	char *bufferToken = NULL;	
	long status_code = 0;
	int size = 0;

	char* server = NULL;
	struct tm *timeinfo;
	time_t rawtime;

	memset(url, 0x00, sizeof(url));

	int current_y = 0;

	int running = 1, key = 0, running3 = 1;


	dfb = dfb_get_directfb();
	main_surface = lcdGetSurface();
	font_height = lcdGetFontHeight();
	lcdGetSize(&screen_width, &screen_height);
	time(&rawtime);
	timeinfo = localtime(&rawtime);


	surfdesc.flags = DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT;
	surfdesc.caps  = DSCAPS_SYSTEMONLY;
	surfdesc.width = screen_width;
	surfdesc.height = screen_height * 4 + font_height;

	dfb->CreateSurface(dfb, &surfdesc, &content_surface);

	main_surface->GetFont(main_surface, &font); // Get font from main_surface

	content_surface->SetFont(content_surface, font);
	content_surface->Clear(content_surface, colorWhite.r, colorWhite.g, colorWhite.b, 0xFF);
	content_surface->SetColor(content_surface, colorBlack.r, colorBlack.g, colorBlack.b, 0xFF);


	rect.x = 0;
	rect.y = 1;
	rect.w = screen_width;
	rect.h = screen_height;

	readServer(&server);
	read_from_file(TOKEN_FILE, &bufferToken);

	DDate id_entry = {screen_width / 2 - 90, 0, 230, SIZE_WIDGET, "", 0, 1};
	sprintf(id_entry.text, "%4d-%02d-%02d", timeinfo->tm_year + 1900, timeinfo->tm_mon+1, timeinfo->tm_mday);

	while (running)
	{
		lcdclean();
		lcd_header(ALG_LEFT, "LOTS GAGNANTS");
		current_y = get_current_y();
		lcdprintfex(ALG_LEFT, current_y + 7, "Date  :");
		id_entry.y = current_y + 7;
		renderDDate(main_surface, &id_entry);

		lcdFlip();
		key = kbGetKey();
		handleDDateInput(key, &id_entry);

		switch(key) 
		{
			case KEY_CANCEL: // KEY_CANCEL
				running = 0;
				break;
			case KEY_ENTER: // KEY_ENTER
				sprintf(url, "%s/api/new8210/app/lot-gagnant?date=%s", server, id_entry.text);

				if (make_get_request(url, &status_code, &buffer, bufferToken) < 0)
				{
					while(1)
						if (kbGetKey() == KEY_CANCEL)
							break;
				} else
				{
					if (status_code >= 200 && status_code <= 299) 
					{
						lcdclean();
						memset(url, 0x00, sizeof(url)); // use the same url variable
						sprintf(url, "LOTS GAGNANTS: %s", id_entry.text);
						i = 0;
						running = 1;

						lcd_header(ALG_LEFT, url);

						json = cJSON_Parse(buffer);
						if (json != NULL)
						{
							if (cJSON_IsArray(json))
							{
								cJSON *element;
								int lotsSize = 0;
								cJSON_ArrayForEach(element, json) 
								{
									cJSON *list = cJSON_GetObjectItemCaseSensitive(element, "list");
									if (cJSON_IsArray(list)) 
									{
										cJSON *itemList;
										
										cJSON_ArrayForEach(itemList, list) 
										{
											cJSON *name = cJSON_GetObjectItemCaseSensitive(itemList, "id");
											cJSON *date = cJSON_GetObjectItemCaseSensitive(itemList, "date");
											cJSON *lots = cJSON_GetObjectItemCaseSensitive(itemList, "lots");
											lcdprintfon(ALG_LEFT, content_surface, screen_width, screen_height,  font_height * i, "%s : %s", name->valuestring, date->valuestring);
											lotsSize = cJSON_GetArraySize(lots);

											if (lotsSize == 3) {
												cJSON *t1 = cJSON_GetArrayItem(lots, 0);
												cJSON *t2 = cJSON_GetArrayItem(lots, 1);
												cJSON *t3 = cJSON_GetArrayItem(lots, 2);
												if (!(font_height * (i + 1) > screen_height * 4 - font_height))
													lcdprintfon(ALG_LEFT, content_surface, screen_width, screen_height,  font_height * (i + 1), "%s, %s, %s", t1->valuestring, t2->valuestring, t3->valuestring);

											} else if (lotsSize == 4) {
												cJSON *t1 = cJSON_GetArrayItem(lots, 0);
												cJSON *t2 = cJSON_GetArrayItem(lots, 1);
												cJSON *t3 = cJSON_GetArrayItem(lots, 2);
												cJSON *t4 = cJSON_GetArrayItem(lots, 3);
												if (!(font_height * (i + 1) > screen_height * 4 - font_height))
													lcdprintfon(ALG_LEFT, content_surface, screen_width, screen_height,  font_height * (i + 1), "%s :: %s, %s, %s", t4->valuestring, t1->valuestring, t2->valuestring, t3->valuestring);
											}
											i += 2;
										}
									}

									if (list != NULL) {
										cJSON_Delete(list);
										list = NULL;
									}
								}
							}
							main_surface->Blit(main_surface, content_surface, &rect, 0, current_y);
    						lcdFlip();
						} else 
						{
							lcdprintf(ALG_LEFT, "Json GT NULL.");
							lcdFlip();
							kbGetKey();
						}
					} else {
						lcdprintf(ALG_LEFT, "%s", buffer);
						lcdFlip();
						kbGetKey();
					}
					// Here
					while (running3 == 1)
					{
						key = kbGetKey();
						switch(key)
						{
							case KEY_UP: // KEY_UP
								rect.y = rect.y - speedScroll;
								if (rect.y <= 0)
									rect.y = 1;
								break;
							case KEY_DOWN: // KEY_DOWN
								rect.y = rect.y + speedScroll;
								if (rect.y >  screen_height * 4)
									rect.y =  screen_height * 4;
								break;
							case KEY_CANCEL: // KEY_CANCEL
								running3 = 0;
								break;
							default:
								break;
						}
						usleep(10000);
						lcdclean();
						lcd_header(ALG_LEFT, url);

						main_surface->Blit(main_surface, content_surface, &rect, 0, current_y);
						lcdFlip();
					}
				}
				break;
			default:
				break;
		}
	}

	// if (json != NULL) {
	// 	cJSON_Delete(json);
	// 	json = NULL;
	// }

	if (bufferToken != NULL) {
		free(bufferToken);
		bufferToken = NULL;
	}

	if (buffer != NULL) {
		free(buffer);
		buffer = NULL;
	}

	if (server != NULL) {
		free(server);
		server = NULL;
	}

} // => PUT



// OK
void getReports(void)
{	
	IDirectFB *dfb = NULL;
	IDirectFBSurface *main_surface = NULL;

	int key, running = 1, font_height, width, height, current_y;

	long status_code = 0;
	char url[144];
	char *buffer = NULL;
	char *bufferToken = NULL;
	char* server = NULL;

	struct tm *timeinfo;
	time_t rawtime;
	memset(url, 0x00, sizeof(url));

	dfb = dfb_get_directfb();
	main_surface = lcdGetSurface();
	font_height = lcdGetFontHeight();
	lcdGetSize(&width, &height);
	
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	readServer(&server);
			
	DDate start_date = {24, 0, width - 24 * 2, SIZE_WIDGET, "", 0, 1};
	DDate end_date = {24, 0, width - 24 * 2, SIZE_WIDGET, "", 0, 0};

	sprintf(start_date.text, "%4d-%02d-%02d", timeinfo->tm_year + 1900, timeinfo->tm_mon+1, timeinfo->tm_mday);
	sprintf(end_date.text, "%4d-%02d-%02d", timeinfo->tm_year + 1900, timeinfo->tm_mon+1, timeinfo->tm_mday);

	while (running)
	{
		lcdclean();
		lcd_header(ALG_LEFT, "Rapports");

		current_y = get_current_y();
		DDate *entry = (start_date.active ? &start_date : &end_date);
		main_surface->DrawString(main_surface, "De Date", -1, entry->x, current_y + 2, DSTF_TOPLEFT);

		start_date.y = current_y + (font_height + 3);
		renderDDate(main_surface, &start_date);
		main_surface->DrawString(main_surface, "A  Date", -1, entry->x, current_y + (font_height * 3 + 2), DSTF_TOPLEFT);
		current_y = current_y + (font_height * 3 + 2);
		current_y += font_height + 1;
		end_date.y = current_y + 3;
		renderDDate(main_surface, &end_date);
		lcdFlip(); // Display

		key = kbGetKey();
		handleDDateInput(key, entry);

		switch(key) 
		{
			case KEY_UP: // KEY_UP
				start_date.active = !start_date.active;
				end_date.active = !end_date.active;
				break;
			case KEY_DOWN: // KEY_DOWN
				start_date.active = !start_date.active;
				end_date.active = !end_date.active;
				break;
			case KEY_CANCEL: // KEY_CANCEL
				running = 0;
				break;
			case KEY_ENTER: // KEY_ENTER
				read_from_file(TOKEN_FILE, &bufferToken);
				lcdclean();
				sprintf(url, "%s/api/new8210/app/reports?start_date=%s&end_date=%s", server, start_date.text, end_date.text);
				if (make_get_request(url, &status_code, &buffer, bufferToken) < 0)
				{
					while(1)
						if (kbGetKey() == KEY_CANCEL)
							break;
				} else
				{
					if (status_code >= 200 && status_code <= 299) 
					{
						print_rapport(buffer);
					} else {
						lcdprintf(ALG_LEFT, "%s, %ld", buffer, status_code);
						lcdFlip();
						while(1)
							if (kbGetKey() == KEY_CANCEL)
								break;
					}
				}
				break;
			default:
				break;
		}
	}

	if (server != NULL) {
		free(server);
		server = NULL;
	}

	if (bufferToken != NULL) {
		free(bufferToken);
		bufferToken = NULL;
	}

	if (buffer != NULL) {
		free(buffer);
		buffer = NULL;
	}
	return;
}



static void _getWinningFiches(const char *server, const char *start_date, const char *end_date, int width, int height, int font_height)
{
	IDirectFB *dfb = NULL;
	IDirectFBSurface *main_surface = NULL;
	IDirectFBSurface *content_surface = NULL;
	DFBSurfaceDescription surfdesc;
	IDirectFBFont *font = NULL;
	DFBRectangle rect;
	char url[250];


	char *bufferToken = NULL;
	char *buffer = NULL;

	long status_code = 0;
	cJSON *root = NULL, *count = NULL, *data22 = NULL;

	int running = 1, currY = 1, index = 0, speedScroll = 12, count_items = 0, count_current_page = 0;
	int key = -1;
	int yyy = 0, size_tirage = 0;

	Tirage *tirages = NULL;
	cJSON *page = NULL, *num_pages = NULL;
	int ppage = 1, paginate = 1, _page = 0;

	char htext[20];
	char str[12];
	memset(htext, 0x00, sizeof(htext));

	dfb = dfb_get_directfb();
	main_surface = lcdGetSurface();
	
	read_from_file(TOKEN_FILE, &bufferToken);
	lcdclean(); // clean main surface
	memset(str, 0x00, sizeof(str));
	memset(url, 0x00, sizeof(url));

	while(paginate == 1) {
		sprintf(url, "%s/api/new8210/app/tickets-won?start_date=%s&end_date=%s&page_size=12&page=%d", server, start_date, end_date, ppage);
		running = 1;
		paginate = 0;
		yyy = 0;

		if (make_get_request(url, &status_code, &buffer, bufferToken) >= 0)
		{
			if (status_code >= 200 && status_code <= 299)
			{
				root = cJSON_Parse(buffer);
				if (root != NULL)
				{
					cJSON *data = cJSON_GetObjectItemCaseSensitive(root, "data");
					count = cJSON_GetObjectItemCaseSensitive(root, "count");
					page = cJSON_GetObjectItemCaseSensitive(root, "page");
					num_pages = cJSON_GetObjectItemCaseSensitive(root, "num_pages");
					
					if (buffer != NULL) {
						free(buffer);
						buffer = NULL;
					}

					count_items = (int)(count->valueint);
					surfdesc.flags = DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT;
					surfdesc.caps  = DSCAPS_SYSTEMONLY;
					surfdesc.width = width;
					surfdesc.height = height * 6 + font_height;
					dfb->CreateSurface(dfb, &surfdesc, &content_surface);

					main_surface->GetFont(main_surface, &font); // Get font from main_surface

					content_surface->SetFont(content_surface, font);
					content_surface->Clear(content_surface, colorWhite.r, colorWhite.g, colorWhite.b, 0xFF);
					content_surface->SetColor(content_surface, colorBlack.r, colorBlack.g, colorBlack.b, 0xFF);

					if (cJSON_IsArray(data)) {
						index = 0;
						count_current_page = cJSON_GetArraySize(data);
		
						while (index < count_current_page)
						{
							cJSON *element = cJSON_GetArrayItem(data, index);
							cJSON *created_on = cJSON_GetObjectItemCaseSensitive(element, "created_on");
							cJSON *ref_code = cJSON_GetObjectItemCaseSensitive(element, "ref_code");
							cJSON *perte = cJSON_GetObjectItemCaseSensitive(element, "perte");

							// addTirageItem(&tirages, &size_tirage, ref_code->valuestring, "0", "0", perte->valuestring, "0", created_on->valuestring);

							sprintf(str, "%.2f", perte->valuedouble);

							content_surface->DrawString(content_surface, "ID: ", -1, 3, yyy + font_height * index, DSTF_TOPLEFT);
							content_surface->DrawString(content_surface, ref_code->valuestring, -1, 27, yyy + font_height * index, DSTF_TOPLEFT);
							
							content_surface->DrawString(content_surface, "Date: ", -1, 3, yyy + font_height * (index + 1), DSTF_TOPLEFT);
							content_surface->DrawString(content_surface, created_on->valuestring, -1, 40, yyy + font_height * (index + 1), DSTF_TOPLEFT);

							content_surface->DrawString(content_surface, "Gagnée: ", -1, 3, yyy + font_height * (index + 2), DSTF_TOPLEFT);
							content_surface->DrawString(content_surface, str, -1, 107, yyy + font_height * (index + 2), DSTF_TOPLEFT);

							content_surface->SetColor(content_surface, colorGrey.r, colorGrey.g, colorGrey.b, 0xFF);
							content_surface->DrawLine(content_surface, 3, yyy + font_height * (index + 3), width - 3, yyy + font_height * (index + 3));
							content_surface->SetColor(content_surface, colorBlack.r, colorBlack.g, colorBlack.b, 0xFF);
							yyy = yyy + font_height * 2 + 4;
							index ++;	
						}
					}

					lcdclean();
					// sprintf(htext, "Fiches (%d) %d:%d ", count_items, page->valueint, num_pages->valueint);
					sprintf(htext, "FICHES (%d) PAGE %d : %d", count_items, page->valueint, num_pages->valueint);

					lcd_header(ALG_CENTER, htext);
					currY = get_current_y();
					
					rect.x = 0;
					rect.y = 1;
					rect.w = width;
					rect.h = height;
					main_surface->Blit(main_surface, content_surface, &rect, 0, currY);
					lcdFlip();

					while (running == 1)
					{
						key = kbGetKey();
						switch (key)
						{
							case KEY_UP: // KEY_UP
								rect.y = rect.y - speedScroll;
								if (rect.y <= 0)
									rect.y = 1;
								break;
							case KEY_DOWN: // KEY_DOWN
								rect.y = rect.y + speedScroll;
								if (rect.y > height * 6 + 3)
									rect.y = height * 6 + 1;
								break;
							case KEY_CANCEL: // KEY_CANCEL
								running = 0;
								break;
							case KEY_FN:
								// print_winnings_fiche(tirages, size_tirage); // Print list of winning tickets
								break;
							case KEY_MENU:
								_page = askPage("Entrer Page");
								if (_page > 0) {
									ppage = _page;
									paginate = 1;
									running = 0;
								}
								break;
							default:
								break;
						}
						usleep(10000);
						lcdclean();
						lcd_header(ALG_CENTER, htext);
						main_surface->Blit(main_surface, content_surface, &rect, 0, currY);
						lcdFlip();
					}
				} 
				else 
				{
					// TODO, display errors
					lcdprintf(ALG_LEFT, "Error Parsing");
					lcdFlip();
					kbGetKey();
				}            
			} else {
				// TODO, display errors
					lcdprintf(ALG_LEFT, "Status:%lg", status_code);
					lcdFlip();
					kbGetKey();
			}
		} else {
			while (1)
				if (kbGetKey() == KEY_CANCEL)
					break;
		}
	}

	if (buffer != NULL) {
		free(buffer);
		buffer = NULL;
	}

	if (root != NULL) {
		cJSON_Delete(root);
		root = NULL;
	}

	if (bufferToken != NULL) {
		free(bufferToken);
		bufferToken = NULL;
	}

	if (content_surface != NULL) {
		content_surface->Release(content_surface);
		content_surface = NULL;
	}
	return;
}


// OK
void getTicketsWon(void)
{	
	IDirectFB *dfb = NULL;
	IDirectFBSurface *main_surface = NULL;
	int key, running = 1, font_height, width, height, current_y;

	char url[200];
	char* server = NULL;

	struct tm *timeinfo;
	time_t rawtime;
	memset(url, 0x00, sizeof(url));

	dfb = dfb_get_directfb();
	main_surface = lcdGetSurface();

	font_height = lcdGetFontHeight();
	lcdGetSize(&width, &height);
	readServer(&server);
	
	time(&rawtime);
	timeinfo = localtime(&rawtime);
			
	DDate start_date = {24, 0, width - 24 * 2, SIZE_WIDGET, "", 0, 1};
	DDate end_date = {24, 0, width - 24 * 2, SIZE_WIDGET, "", 0, 0};

	sprintf(start_date.text, "%4d-%02d-%02d", timeinfo->tm_year + 1900, timeinfo->tm_mon+1, timeinfo->tm_mday);
	sprintf(end_date.text, "%4d-%02d-%02d", timeinfo->tm_year + 1900, timeinfo->tm_mon+1, timeinfo->tm_mday);

	while (running)
	{
		lcdclean();
		lcd_header(ALG_LEFT, "FICHES GAGNANTS");
		current_y = get_current_y();
		DDate *entry = (start_date.active ? &start_date : &end_date);
		main_surface->DrawString(main_surface, "De Date", -1, entry->x, current_y + 2, DSTF_TOPLEFT);

		start_date.y = current_y + (font_height + 3);
		renderDDate(main_surface, &start_date);
		main_surface->DrawString(main_surface, "A Date", -1, entry->x, current_y + (font_height * 3 + 2), DSTF_TOPLEFT);
		current_y = current_y + (font_height * 3 + 2);
		current_y += font_height + 1;
		end_date.y = current_y + 3;
		renderDDate(main_surface, &end_date);
		lcdFlip(); // Display

		key = kbGetKey();
		handleDDateInput(key, entry);

		switch(key) 
		{
			case KEY_UP: // KEY_UP
				start_date.active = !start_date.active;
				end_date.active = !end_date.active;
				break;
			case KEY_DOWN: // KEY_DOWN
				start_date.active = !start_date.active;
				end_date.active = !end_date.active;
				break;
			case KEY_CANCEL: // KEY_CANCEL
				running = 0;
				break;
			case KEY_ENTER: // KEY_ENTER
				lcdclean();
				_getWinningFiches(server, start_date.text, end_date.text, width, height, font_height);
				break;
			default:
				break;
		}
	}

	if (server != NULL) {
		free(server);
		server = NULL;
	}
	return;
}


static void getFicheById(void)
{
	IDirectFBSurface *main_surface = NULL;
	int screen_width, screen_height, font_height;	

	cJSON *json = NULL;
	cJSON *json2 = NULL;

	char url[250];
	char *buffer = NULL;
	char *bufferToken = NULL;	
	long status_code = 0;
	int size = 0;

	char* server = NULL;

	memset(url, 0x00, sizeof(url));
// 	Tirage *items = NULL;

	int current_y = 0;


	int running = 1, key;

	lcdGetSize(&screen_width, &screen_height);
	main_surface = lcdGetSurface();
	font_height = lcdGetFontHeight();
	readServer(&server);
	read_from_file(TOKEN_FILE, &bufferToken);

	Entry id_entry = {screen_width / 2 - 90, 0, 230, SIZE_WIDGET, "", 0, 1, 50, ALPHA, NORMAL};
	Ticket item = {"", 0, "", "", 0};

	while (running)
	{
		lcdclean();
		lcd_header(ALG_LEFT, "ID");
		current_y = get_current_y();
		lcdprintfex(ALG_LEFT, current_y + 7, "ID   :");
		id_entry.y = current_y + 7;
		renderEntry(main_surface, &id_entry);
		lcdFlip();
		key = kbGetKey();
		handleEntryInput(key, &id_entry);
		switch(key) 
		{
			case KEY_CANCEL: // KEY_CANCEL
				running = 0;
				break;
			case KEY_ENTER: // KEY_ENTER
				sprintf(item.id, id_entry.text);
				displayGrid(item, 0);
				break;
			default:
				break;
		}
	}

	
	if (json != NULL) {
		cJSON_Delete(json);
		json = NULL;
	}

	if (bufferToken != NULL) {
		free(bufferToken);
		bufferToken = NULL;
	}

	if (buffer != NULL) {
		free(buffer);
		buffer = NULL;
	}

	if (server != NULL) {
		free(server);
		server = NULL;
	}

} 


// OK
void getFiches(void)
{
	struct tm *timeinfo;
	time_t rawtime;
	int screen_width, screen_height, font_height, selected = 0;
	char start_date[11];
	char end_date[11];
	
	memset(start_date, 0x00, sizeof(start_date));
	memset(end_date, 0x00, sizeof(end_date));

	const char menu[][25] = {
		"1. PAR ID",
		"2. AUJOURD'HUI"
	};

	lcdGetSize(&screen_width, &screen_height);
	font_height = lcdGetFontHeight();
	
	while (selected >= 0)
	{
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		selected = lcdmenu("RECHERCHE FICHE", menu, sizeof(menu) / 25, selected);
		switch (selected)
		{
			case 0:
				getFicheById();
				break;
			case 1:
				sprintf(end_date, "%4d-%02d-%02d", timeinfo->tm_year + 1900, timeinfo->tm_mon+1, timeinfo->tm_mday);
				sprintf(start_date, "%4d-%02d-%02d", timeinfo->tm_year + 1900, timeinfo->tm_mon+1, timeinfo->tm_mday);
				make_get_fiches(start_date, end_date);
				break;
			default:
				break;
		}
	}
}
