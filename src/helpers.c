/*
 * helpers.c
 *
 *  Created on: Apr 22, 2024
 *      Author: James DESTINE
 */

#include "helpers.h"


static int current_y = 1; // Global position y



int check_connection(void)
{
	int32_t iRet;
	int width, height;
	int font_height, toggle = 0;
	lcdGetSize(&width, &height);
	font_height = lcdGetFontHeight();
	lcdclean();
	lcdprintfex(ALG_CENTER, height / 2 - font_height, "Attendez...");
	lcdFlip();

	iRet = PPPCheck(PPP_DEV_GPRS);
	if(iRet==0)
		return 0; //connected

	iRet = WnetInit(20000);
	if(0 != iRet)
	{
		return -1;
	}
	//check SIM card
	iRet = WnetCheckSim();
	if(0 != iRet)
	{
		return -1;
	}

	// iRet = PPPLogin(PPP_DEV_GPRS, "natcom", "", "", 0, 45);
	iRet = PPPLogin(PPP_DEV_GPRS, "natcom", "card", "card", 0, 65000);
	if(iRet != NET_OK)
	{
		return -1;
	}

	while(1)
	{
		lcdclean();
		toggle += 1;
		if (toggle == 1)
			lcdprintfex(ALG_CENTER, height / 2 - font_height, "Attendez.  ");
		else if (toggle == 2)
			lcdprintfex(ALG_CENTER, height / 2 - font_height, "Attendez.. ");
		else if (toggle == 3)
			lcdprintfex(ALG_CENTER, height / 2 - font_height, "Attendez...");

		toggle %= 3;
		lcdFlip();
		iRet = PPPCheck(PPP_DEV_CDMA);
		if(iRet != -NET_ERR_LINKOPENING)
			break;
		sysDelayMs(250);
	}
	return 0;
}


static size_t write_function_callback(void *data, size_t size, size_t nmemb, void *clientp)
{
	size_t realsize = size * nmemb;
	Memory *mem = (Memory *)clientp;

	char *ptr = realloc(mem->response, mem->size + realsize + 1);
	if(ptr == NULL)
		return 0;  /* out of memory! */

	mem->response = ptr;
	memcpy(&(mem->response[mem->size]), data, realsize);
	mem->size += realsize;
	mem->response[mem->size] = 0;
	return realsize;
}

// Make a GET request and return JSON data
int make_get_request(const char *url, long *status_code, char **buffer, const char *token)
{
	CURL *curl_handle;
	CURLcode res;
    struct curl_slist *headers = NULL;
	int retValue = 0;
	char ttk[250];

	Memory memory;

	memory.response = malloc(1);  /* will be grown as needed by the realloc above */
	memory.size = 0;

	curl_handle = curl_easy_init();
	check_connection();
	/* init the curl session */
	headers = curl_slist_append(headers, "Content-Type: application/json");
	// Set token
	if (token != NULL) {
		memset(ttk, 0x00, sizeof(ttk));
		sprintf(ttk, "Authorization: Token %s", token);
		headers = curl_slist_append(headers, ttk);
	}
	curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
	/* specify URL to get */
	curl_easy_setopt(curl_handle, CURLOPT_URL, url);
	/* send all data to this function  */
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_function_callback);
	/* we pass our 'chunk' struct to the callback function */
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&memory);
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "new8210/1.0");
	curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
	/* get it! */
	res = curl_easy_perform(curl_handle);
	curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, status_code);
	/* check for errors */
	if(res != CURLE_OK)
	{
		lcdclean();
		lcdprintf(ALG_LEFT, "NET ERROR: %s", curl_easy_strerror(res));
		lcdFlip();
		retValue = -1;
	}
	else{
		int len = strlen(memory.response) + 1;
		char* tempBuffer = realloc(*buffer, len + 1);
		if (tempBuffer == NULL)
		{
			retValue = -1;
			return retValue;
		}
		*buffer = tempBuffer;
		memcpy(*buffer, memory.response, len);
		retValue = 0;
	}
	/* cleanup curl stuff */
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl_handle);
	free(memory.response);
	return retValue;
}

// Make a POST request with JSON data and return JSON response
int make_post_request(const char *url, const char *data, long *status_code, char **buffer, const char *token)
{
    CURL *curl_handle;
	CURLcode res;
    struct curl_slist *headers = NULL;
	int retValue = 0;
	char ttk[250];
	Memory memory;
	memory.response = malloc(1);  /* will be grown as needed by the realloc above */
	memory.size = 0;
	curl_handle = curl_easy_init();

	check_connection();
	/* init the curl session */
	headers = curl_slist_append(headers, "Content-Type: application/json");
	if (token != NULL) {
		memset(ttk, 0x00, sizeof(ttk));
		sprintf(ttk, "Authorization: Token %s", token);
		headers = curl_slist_append(headers, ttk);
	}
	/* specify URL to get */
	curl_easy_setopt(curl_handle, CURLOPT_URL, url);
        
	curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
	/* Now specify the POST data */
	curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, data);
	/* send all data to this function  */
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_function_callback);
	/* we pass our 'chunk' struct to the callback function */
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&memory);
	// curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "new8210/1.0");
	curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
	/* get it! */
	res = curl_easy_perform(curl_handle);
	curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, status_code);
	
	if(res != CURLE_OK)
	{
		lcdclean();
		lcdprintf(ALG_LEFT, "NET ERROR: %s", curl_easy_strerror(res));
		lcdFlip();
		retValue = -1;
	}
	else{
		int len = strlen(memory.response) + 1;
		char* tempBuffer = realloc(*buffer, len + 1);
		if (tempBuffer == NULL)
		{
			retValue = -1;
			return retValue;
		}
		*buffer = tempBuffer;
		memcpy(*buffer, memory.response, len);
		retValue = 0;
	}
	/* cleanup curl stuff */
    curl_slist_free_all(headers);
	curl_easy_cleanup(curl_handle);
	free(memory.response);
	return retValue;
}


// Make a POST request with JSON data and return JSON response
int make_http_request(const char *url, const char *data, long *status_code, char **buffer, const char *token, const char *verb)
{
    CURL *curl_handle;
	CURLcode res;
    struct curl_slist *headers = NULL;
	int retValue = 0;
	char ttk[250];
	Memory memory;
	memory.response = malloc(1);
	memory.size = 0;
	curl_handle = curl_easy_init();

	check_connection();
	/* init the curl session */
	headers = curl_slist_append(headers, "Content-Type: application/json");
	if (token != NULL) {
		memset(ttk, 0x00, sizeof(ttk));
		sprintf(ttk, "Authorization: Token %s", token);
		headers = curl_slist_append(headers, ttk);
	}
	/* specify URL to get */
	curl_easy_setopt(curl_handle, CURLOPT_URL, url);
        
	curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
	// Set the request type to PUT
    curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, verb);
	/* Now specify the POST data */
	curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, data);
	/* send all data to this function  */
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_function_callback);
	/* we pass our 'chunk' struct to the callback function */
	if (buffer != NULL)
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&memory);
	// curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "new8210/1.0");
	curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
	/* get it! */
	res = curl_easy_perform(curl_handle);
	curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, status_code);
	
	if(res != CURLE_OK)
	{
		lcdclean();
		lcdprintf(ALG_LEFT, "NET ERROR: %s", curl_easy_strerror(res));
		lcdFlip();
		retValue = -1;
	}
	else{
		int len = strlen(memory.response) + 1;
		char* tempBuffer = realloc(*buffer, len + 1);
		if (tempBuffer == NULL)
		{
			retValue = -1;
			return retValue;
		}
		*buffer = tempBuffer;
		memcpy(*buffer, memory.response, len);
		retValue = 0;
	}
	/* cleanup curl stuff */
    curl_slist_free_all(headers);
	curl_easy_cleanup(curl_handle);
	free(memory.response);
	return retValue;
}



// Write to file
int write_to_file(const char* file, const char* contents)
{
	FILE *fp;
	fp = fopen(file, "w");
	if (fp == NULL)
	{
		puts("I/O ERROR COULD NOT OPEN FILE");
		return -1;
	}
	else
	{
		fprintf(fp, "%s", contents);
		fclose(fp);
        return 0;
	}
}

// Read from file
int read_from_file(const char* file, char** buffer)
{
    INT32 fhandle = 0;
    UINT8 *DataBuf = NULL;
    INT32 Len;
    INT32 success;

    fhandle = fileOpen(file, O_RDWR);
    if (fhandle == -1)
    {
        puts("I/O ERROR INVALID FILE HANDLE");
        return -1;
    }

    Len = fileSize(file);
    DataBuf = (UINT8*)malloc(Len);
    if (DataBuf == NULL)
    {
        fileClose(fhandle);
        return -1;  // Return error code indicating memory allocation failure
    }

    success = fileRead(fhandle, DataBuf, Len);
    if (success >= 0)
    {
        *buffer = (char*)malloc(success + 1);  // Allocate memory for the buffer (+1 for null-terminator)
        if (*buffer == NULL)
        {
            free(DataBuf);
            fileClose(fhandle);
            return -1;  // Return error code indicating memory allocation failure
        }
        memcpy(*buffer, DataBuf, success);
        (*buffer)[success] = '\0';  // Null-terminate the buffer
        fileClose(fhandle);
        free(DataBuf);
        return 0;
    }

    free(DataBuf);
    fileClose(fhandle);
    return -1;
}


// get length of a string
int string_len(const char *text)
{
	int retval = 0;
	if (text != NULL)
	{
		while (*text)
		{
			text++;
			retval++;
		}
	}
	return retval;
}


// COnvert string
const char *string_covert( const char *text, int textlen)
{
	static char *ret = NULL;

	if (NULL != ret)
		free(ret);
	ret = NULL;

	if (!text)
		return NULL;
	ret = (char*)malloc(textlen * 4 + 1);

	if (charset_convert("GB18030", "UTF-8", text, textlen, ret, textlen * 4 + 1) != 0)
		memset(ret, 0, textlen * 4 + 1);
	return ret;
}



// get current y position
int get_current_y(void)
{
	return current_y;
}


// Clear LCD and set y to initial
void lcdclean(void)
{
	lcdCls();
	current_y = 1;
}


// Display header
void lcd_header(LCD_ALG alg, const char *pszFmt)
{
    IDirectFBSurface *main_surface = NULL;
    IDirectFBFont * font = NULL;
    int status_bar_height, width, height, font_height;
    main_surface = lcdGetSurface();
    lcdSetFont(FONT_ROBOTO, "UTF-8", 0,  18, 0);
    status_bar_height = sys_get_status_bar_height() + 1;
    main_surface->GetSize(main_surface, &width, &height);
    main_surface->GetFont(main_surface, &font);
    font->GetHeight(font, &font_height);

    main_surface->SetColor(main_surface, colorPrimary.r, colorPrimary.g, colorPrimary.b, colorPrimary.a);
    main_surface->FillRectangle(main_surface, 0, 0, width, status_bar_height);
    main_surface->SetColor(main_surface, colorWhite.r, colorWhite.g, colorWhite.b, colorWhite.a);
    lcdprintfex(ALG_CENTER, (status_bar_height - font_height) / 2 + 2, pszFmt);
    main_surface->SetColor(main_surface, colorBlack.r, colorBlack.g, colorBlack.b, colorBlack.a);
	current_y = current_y + 1;
    lcdSetFont(FONT_ROBOTO, "UTF-8", 0, 16, 0);

}



//Draw picture on surface, x, y, width, height, filename, surface
int drawpicture(int x, int y, int w, int h, const char *filename, IDirectFBSurface *target)
{
	IDirectFBSurface	    *logo = NULL;
	DFBSurfaceDescription   sdsc;
	IDirectFBImageProvider	*Imageprovider = NULL;
	IDirectFB				*dfb = dfb_get_directfb();

	if (access(filename, F_OK))
		return -ENOENT;

	dfb->CreateImageProvider(dfb, filename, &Imageprovider);
	Imageprovider->GetSurfaceDescription(Imageprovider, &sdsc);

	sdsc.flags		 = DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT;
	sdsc.pixelformat = DSPF_ARGB;
	sdsc.width		 = w;
	sdsc.height		 = h;

	dfb->CreateSurface(dfb, &sdsc, &logo);
	Imageprovider->RenderTo(Imageprovider, logo, NULL);
	target->Blit(target, logo, NULL, x, y);
	Imageprovider->Release(Imageprovider);
	logo->Release(logo);

	return 0;
}


// Draw rectangle into LCD
void lcd_draw_rectangle(unsigned int x, unsigned int y, unsigned int width, unsigned int height, int isFIll)
{
	int screen_width, screen_height;
	IDirectFBSurface *main_surface = NULL;
	main_surface = lcdGetSurface();
	main_surface->GetSize(main_surface, &screen_width, &screen_height);

	if ((x+1 < screen_width) && (y +1 < screen_height)){
		if (x + 1 + width  > (unsigned int)screen_width)
			width = screen_width - x -1;
		if (x + 1 + height  > (unsigned int)screen_height)
			height = screen_height - y -1;

		if (isFIll == 1)
			main_surface->FillRectangle(main_surface, x, y, width, height);
		else
			main_surface->DrawRectangle(main_surface, x, y, width, height);
	}
}


int lcdprintfon(LCD_ALG alg, IDirectFBSurface *main_surface, int width, int height, int current_y, const char * pszFmt,...)
{
	int font_height;
	IDirectFB *dfb = NULL;
	IDirectFBSurface *sub_surface = NULL;
	IDirectFBSurface *store_surface = NULL;

	DFBRectangle rect;
	DFBSurfaceDescription surfdesc;
	IDirectFBFont *font = NULL;

	char textbuf[2048];
	const char *utf8text;
	const char *pnewline = NULL;
	int str_length, temp_width;

	va_list arg;
	dfb = dfb_get_directfb();
	va_start(arg, pszFmt);
	vsnprintf(textbuf, sizeof(textbuf), pszFmt, arg);
	va_end (arg);

	main_surface->GetFont(main_surface, &font);
	font->GetHeight(font, &font_height);
	main_surface->GetSize(main_surface, &width, &height);

	if (current_y + font_height > height)
	{
		rect.x = 0;
		rect.y = current_y + font_height - height;
		rect.w = width;
		rect.h = current_y - rect.y;
		main_surface->GetSubSurface (main_surface, &rect, &sub_surface);

		surfdesc.flags = DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT;
		surfdesc.caps  = DSCAPS_SYSTEMONLY;
		surfdesc.width = width;
		surfdesc.height= height;

		dfb->CreateSurface(dfb, &surfdesc, &store_surface);
		store_surface->SetBlittingFlags(store_surface, DSBLIT_BLEND_ALPHACHANNEL);
		store_surface->Blit(store_surface, sub_surface, NULL, 0, 0);
		lcdclean();

		main_surface->SetBlittingFlags(main_surface, DSBLIT_BLEND_ALPHACHANNEL);
		rect.y = 0;
		main_surface->Blit(main_surface, store_surface, &rect, 0, 0);

		store_surface->Release(store_surface);
		sub_surface->Release(sub_surface);
		current_y = rect.h;
	}

	font->GetStringBreak(font, textbuf, strlen(textbuf), width - 3, &temp_width, &str_length, &pnewline);
	utf8text = string_covert(textbuf, str_length);

	if (ALG_CENTER == alg)
		main_surface->DrawString(main_surface, utf8text, string_len(utf8text), width / 2 - string_len(utf8text) / 2, current_y, DSTF_TOPCENTER);
	else if (ALG_LEFT == alg)
		main_surface->DrawString(main_surface, utf8text, string_len(utf8text), 3, current_y, DSTF_TOPLEFT);
	else
		main_surface->DrawString(main_surface, utf8text, string_len(utf8text), width - 3, current_y, DSTF_TOPRIGHT);

	current_y = current_y + font_height;
	// current_y = current_y + font_height;

	if (NULL != pnewline)
		current_y += lcdprintfon(alg, main_surface, width, height, current_y, pnewline);

	return current_y;
}



void lcdprintf(LCD_ALG alg, const char * pszFmt,...)
{
	int width, height;
	int font_height;
	IDirectFB *dfb = NULL;
	IDirectFBSurface *main_surface = NULL;
	IDirectFBSurface *sub_surface = NULL;
	IDirectFBSurface *store_surface = NULL;

	DFBRectangle rect;
	DFBSurfaceDescription surfdesc;
	IDirectFBFont *font = NULL;

	char textbuf[2048];
	const char *utf8text;
	const char *pnewline = NULL;
	int str_length, temp_width;

	va_list arg;
	dfb = dfb_get_directfb();
	main_surface = lcdGetSurface();
	va_start(arg, pszFmt);
	vsnprintf(textbuf, sizeof(textbuf), pszFmt, arg);
	va_end (arg);

	main_surface->GetFont(main_surface, &font);
	font->GetHeight(font, &font_height);
	main_surface->GetSize(main_surface, &width, &height);

	if (current_y + font_height > height)
	{
		rect.x = 0;
		rect.y = current_y + font_height - height;
		rect.w = width;
		rect.h = current_y - rect.y;
		main_surface->GetSubSurface (main_surface, &rect, &sub_surface);

		surfdesc.flags = DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT;
		surfdesc.caps  = DSCAPS_SYSTEMONLY;
		surfdesc.width = width;
		surfdesc.height= height;

		dfb->CreateSurface(dfb, &surfdesc, &store_surface);
		store_surface->SetBlittingFlags(store_surface, DSBLIT_BLEND_ALPHACHANNEL);
		store_surface->Blit(store_surface, sub_surface, NULL, 0, 0);
		lcdclean();

		main_surface->SetBlittingFlags(main_surface, DSBLIT_BLEND_ALPHACHANNEL);
		rect.y = 0;
		main_surface->Blit(main_surface, store_surface, &rect, 0, 0);

		store_surface->Release(store_surface);
		sub_surface->Release(sub_surface);
		current_y = rect.h;
	}

	font->GetStringBreak(font, textbuf, strlen(textbuf), width - 3, &temp_width, &str_length, &pnewline);
	utf8text = string_covert(textbuf, str_length);

	if (ALG_CENTER == alg)
		main_surface->DrawString(main_surface, utf8text, string_len(utf8text), width / 2 - string_len(utf8text) / 2, current_y, DSTF_TOPCENTER);
	else if (ALG_LEFT == alg)
		main_surface->DrawString(main_surface, utf8text, string_len(utf8text), 3, current_y, DSTF_TOPLEFT);
	else
		main_surface->DrawString(main_surface, utf8text, string_len(utf8text), width - 3, current_y, DSTF_TOPRIGHT);

	current_y = current_y + font_height;
	// current_y = current_y + font_height;

	if (NULL != pnewline)
		lcdprintf(alg, pnewline);
}


// Display into LCD at specific y index and set y as global
void lcdprintfex(LCD_ALG alg, unsigned y, const char * pszFmt,...)
{
	char textbuf[2048];
	va_list arg;
	va_start(arg, pszFmt);
	vsnprintf(textbuf, sizeof(textbuf), pszFmt, arg);
	va_end (arg);
	current_y = y;
	lcdprintf(alg, textbuf);
}



// Menu
int lcdmenu(const char *pszTitle, const char menu[][25], unsigned int count, int select)
{
	IDirectFBSurface *main_surface = NULL;
	int running, key, retval = -1;
	int screen_width, screen_height, font_height;

	unsigned int i;
	unsigned int max_lines = 0;
	unsigned int istart = 0;

	main_surface = lcdGetSurface();
	font_height = lcdGetFontHeight();
	lcdGetSize(&screen_width, &screen_height);

	if (select < 0)
		select = 0;

	if (count <= 0)
		select = -1;
	else
	{
		running = 1;
		while (running == 1)
		{
			lcdclean();
			lcd_header(ALG_LEFT, pszTitle);
			max_lines = 6; // (screen_height - current_y - 1) / font_height;
			istart = (select / max_lines) * max_lines;

			for (i = 0;  i < max_lines; i++)
			{
				if (istart + i < count)
				{
					if (istart + i == select)
					{
						main_surface->SetColor(main_surface, colorPrimary.r, colorPrimary.g, colorPrimary.b, colorPrimary.a);
						lcd_draw_rectangle(0, current_y - 1, screen_width - 44, font_height + 1, 1);
						main_surface->SetColor(main_surface, colorWhite.r, colorWhite.g, colorWhite.b, colorWhite.a);
						lcdprintf(ALG_LEFT, menu[istart + i]);
						main_surface->SetColor(main_surface, colorBlack.r, colorBlack.g, colorBlack.b, colorBlack.a);
					}
					else
						lcdprintf(ALG_LEFT, menu[istart + i]);
				}
				else
					break;
			}
			lcdFlip();
	LOOP:
			key = kbGetKeyMs(10 * 1000);
			switch(key)
			{
				case KEY_UP:
					select--;
					if (select < 0)
						select = count - 1;
					break;
				case KEY_DOWN:
					select++;
					if (select >= count)
						select = 0;
					break;
				case KEY_CANCEL:
					select = -1;
					running = 0;
					break;
				case KEY_ENTER:
					running = 0;
					break;
				default:
					goto LOOP;
			}
		}
	}
	retval = select;
	return retval;
}



int lcdmenu_tirage(const char *pszTitle, const Tirage *menu, unsigned int count, int select, int *id, const Tirage *selectedTirage, int sizeSelected)
{	
	IDirectFBSurface *main_surface = NULL;
	int running, key, retval = -1, beenSelect = -1;
	int screen_width, screen_height, font_height;
	unsigned int i, hasSelect;
	unsigned int max_lines = 0;
	unsigned int istart = 0;

	main_surface = lcdGetSurface();
	font_height = lcdGetFontHeight();
	lcdGetSize(&screen_width, &screen_height);

	if (select < 0)
		select = 0;
	if (count <= 0)
		select = -1;
	else
	{
		running = 1;
		while (running == 1)
		{
			lcdclean();
			lcd_header(ALG_CENTER, pszTitle);
			// max_lines = (screen_height - current_y - 1) / font_height;
			max_lines = 6;
			istart = (select / max_lines) * max_lines; 

			for (i = 0;  i < max_lines; i++)
			{
				
				if (istart + i < count)
				{
					beenSelect = hasInfoTirageByName(selectedTirage, sizeSelected, menu[istart + i].name);
					if (beenSelect >= 0)
						main_surface->SetColor(main_surface, colorWarning.r, colorWarning.g, colorWarning.b, colorWarning.a);
					else
						main_surface->SetColor(main_surface, colorBlack.r, colorBlack.g, colorBlack.b, colorBlack.a);
					
					if (istart + i == select)
					{
						main_surface->SetColor(main_surface, colorPrimary.r, colorPrimary.g, colorPrimary.b, colorPrimary.a);
						lcd_draw_rectangle(0, current_y - 1, screen_width - 44, font_height + 1, 1);
						
						if (beenSelect >= 0)
							main_surface->SetColor(main_surface, colorWarning.r, colorWarning.g, colorWarning.b, colorWarning.a);
						else
							main_surface->SetColor(main_surface, colorWhite.r, colorWhite.g, colorWhite.b, colorWhite.a);
					
						lcdprintf(ALG_LEFT, "%d. %s", istart + i + 1,  menu[istart + i].name);		
						main_surface->SetColor(main_surface, colorBlack.r, colorBlack.g, colorBlack.b, colorBlack.a);	
						*id = istart + i;
					}
					else {
						lcdprintf(ALG_LEFT, "%d. %s", istart + i + 1,  menu[istart + i].name);			
					}
				}
				else
					break;
			}
			lcdFlip();	
	LOOP:
			key = kbGetKeyMs(10 * 1000);
			switch(key)
			{
				case KEY_UP:
					select--;
					if (select < 0)
					{
						select = count -1;
					}
					running = 0;
					break;
				case KEY_DOWN:
					select++;
					if (select >= count)
					{
						select = 0;
					}
					running = 0;
					break;
				case KEY_CANCEL:
					select = -1;
					running = 0;
					break;
				case KEY_FN:
					select = 14;
					running = 0;
					break;
				case KEY_ENTER:
					select = 15;
					running = 0;
					break;
				default:
					goto LOOP;
			}
		}
	}
	retval = select;
	lcdclean();
	return retval;
}



static char* getFirstTwoChars(const char* string)
{
    int length = strlen(string);
    int resultLength = 0;
    int i = 0;
    while (i < length)
    {
        while (i < length && isspace(string[i]))
            i++;
        if (i < length && isalpha(string[i]))
            resultLength++;
        i++;
        while (i < length && !isspace(string[i]))
            i++;
        if (i < length && isalpha(string[i]))
            resultLength++;
        i++;
    }

    // Allocate memory for the resulting string
    char* result = (char *)malloc((resultLength + 1) * sizeof(char));
    if (result != NULL)
    {
        int resultIndex = 0;
        i = 0;
        while (i < length)
        {
            while (i < length && isspace(string[i]))
                i++;

            if (i < length && isalpha(string[i]))
                result[resultIndex++] = string[i];
            i++;
            while (i < length && !isspace(string[i]))
                i++;
            if (i < length && isalpha(string[i]))
                result[resultIndex++] = string[i];
            i++;
        }
        result[resultIndex] = '\0'; // Null
    }
    return result;
}



void bouleItemPrintf(const BouleItem *item, const char *tip, int icount)
{
	int width, height;
	int font_height;
	IDirectFB *dfb = NULL;
	IDirectFBSurface *main_surface = NULL;
	IDirectFBSurface *sub_surface = NULL;
	IDirectFBSurface *store_surface = NULL;
	DFBRectangle rect;
	DFBSurfaceDescription surfdesc;
	IDirectFBFont *font = NULL;
	int istart;
	char countStr[12];

	dfb = dfb_get_directfb();
	main_surface = lcdGetSurface();
	memset(countStr, 0x00, sizeof(countStr));
	lcdSetFont(FONT_ROBOTO, "UTF-8", 0,  16, 0);
	main_surface->GetFont(main_surface, &font);
	font->GetHeight(font, &font_height);	
	main_surface->GetSize(main_surface, &width, &height);
	height = height - (font_height);

	if (current_y + font_height > height)
	{
		rect.x = 0;
		rect.y = current_y + font_height - height;
		rect.w = width;
		rect.h = current_y - rect.y;
		main_surface->GetSubSurface (main_surface, &rect, &sub_surface);
		
		surfdesc.flags = DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT;
		surfdesc.caps  = DSCAPS_SYSTEMONLY;
		surfdesc.width = width;
		surfdesc.height= height;

		dfb->CreateSurface(dfb, &surfdesc, &store_surface);
		store_surface->SetBlittingFlags(store_surface, DSBLIT_BLEND_ALPHACHANNEL);
		store_surface->Blit(store_surface, sub_surface, NULL, 0, 0);
		lcdclean();
		main_surface->SetBlittingFlags(main_surface, DSBLIT_BLEND_ALPHACHANNEL);
		rect.y = 0;
		main_surface->Blit(main_surface, store_surface, &rect, 0, 0);
		store_surface->Release(store_surface);
		sub_surface->Release(sub_surface);
		current_y = rect.h;   
	}

	istart = (width - 12) / 4;
	sprintf(countStr, "%d", icount);
	main_surface->SetColor(main_surface, colorGrey.r, colorGrey.g, colorGrey.b, colorGrey.a);
	main_surface->DrawString(main_surface, countStr, -1, 1, current_y, DSTF_TOPLEFT);
	main_surface->SetColor(main_surface, colorBlack.r, colorBlack.g, colorBlack.b, colorBlack.a);

	if (strlen(item->boul) >= 4 && strcmp(item->lotto, "MA") != 0)
		sprintf(countStr, "%s(%s)", item->boul, item->option);
	else
		sprintf(countStr, "%s", item->boul);

	main_surface->DrawString(main_surface, countStr, -1, 22, current_y, DSTF_TOPLEFT);
	main_surface->DrawString(main_surface, getFirstTwoChars(tip), -1, 31 + (istart), current_y, DSTF_TOPCENTER );
	main_surface->DrawString(main_surface, item->pri, -1, 31 + (istart * 2) + 2, current_y, DSTF_TOPLEFT);		
	main_surface->DrawString(main_surface, item->lotto, -1, 38 + (istart * 3), current_y, DSTF_TOPLEFT);		

	current_y = current_y + font_height;
}



int editableList(const BouleItem items[], unsigned int count, int select, const char *tip, int *s, Tirage *tirages, int sizeTirage)
{
	int retval = -1;
	int key;
	unsigned int i;
	int running = 1, myi;
	char dateStr[125];
	struct tm *timeinfo;
	time_t rawtime;
	
	IDirectFBSurface *main_surface = NULL;
	IDirectFBFont * font = NULL;
	unsigned int max_lines = 0;
	unsigned int istart = 0;
	double total = 0.0;
	int screen_width, screen_height, font_height, wstart, status_bar_height = 0, y_bottom_text = 0;
	memset(dateStr, 0x00, sizeof(dateStr));
	main_surface = lcdGetSurface();

	lcdSetFont(FONT_ROBOTO, "UTF-8", 0,  16, 0);
	status_bar_height = sys_get_status_bar_height() + 1;

	main_surface->GetSize(main_surface, &screen_width, &screen_height);
	main_surface->GetFont(main_surface, &font);
	font->GetHeight(font, &font_height);

	if (sizeTirage > 1)
		screen_height = (screen_height - (font_height * 2));
	else
		screen_height = (screen_height - font_height);
	wstart = (screen_width - 12) / 4;

	if (sizeTirage > 0)
		tip = tirages[0].name;

	if (select < 0)
		select = 0;
	running = 1;
	while (running == 1)
		{
			time(&rawtime);
			timeinfo = localtime(&rawtime);
			total = 0;
			lcdSetFont(FONT_ROBOTO, "UTF-8", 0,  18, 0);
			font->GetHeight(font, &font_height);
			lcdclean();
			main_surface->SetColor(main_surface, colorPrimary.r, colorPrimary.g, colorPrimary.b, colorPrimary.a);
			main_surface->FillRectangle(main_surface, 0, 0, screen_width, status_bar_height);
			main_surface->SetColor(main_surface, colorWhite.r, colorWhite.g, colorWhite.b, colorWhite.a);
			main_surface->DrawString(main_surface, "BOULE", -1, 22, (28 - font_height) / 2, DSTF_TOPLEFT);
			main_surface->DrawString(main_surface, "TIRAGE", -1, 30 + wstart, (28 - font_height) / 2, DSTF_TOPCENTER );
			main_surface->DrawString(main_surface, "PRIX", -1, 29 + wstart * 2, (28 - font_height) / 2, DSTF_TOPLEFT);
			main_surface->DrawString(main_surface, "JEU", -1, 29 + wstart * 3, (28 - font_height) / 2, DSTF_TOPLEFT);	
			main_surface->SetColor(main_surface, colorBlack.r, colorBlack.g, colorBlack.b, colorBlack.a);
			// lcdprintfex(ALG_CENTER, get_current_y() - (font_height / 3), "");
			lcdSetFont(FONT_ROBOTO, "UTF-8", 0,  16, 0);

			myi = 0;
			while(myi < count)
			{
				total += atoi(items[myi].pri);
				myi++;
			}
			
			current_y = current_y + font_height + 2;
			max_lines = (screen_height - current_y - 1) / font_height;
			istart = (select / max_lines) * max_lines; 

			for (i = 0;  i < max_lines; i++)
			{
				if (istart + i < count)
				{
					if (istart + i == select)
					{
						main_surface->SetColor(main_surface, colorLight.r, colorLight.g, colorLight.b, colorLight.a);
						lcd_draw_rectangle(1, current_y - 1, screen_width - 1, font_height + 1, 1);
						main_surface->SetColor(main_surface, colorBlack.r, colorBlack.g, colorBlack.b, colorBlack.a);
						bouleItemPrintf(&items[istart + i], tip, istart + i + 1);
					}
					else {
						bouleItemPrintf(&items[istart + i], tip, istart + i + 1);
					}
				}
				else
					break;
			}
			int ii = 0;
			if (sizeTirage > 1)
			{
				ii = 1;
				while (ii < sizeTirage && ii <= 8)
				{
					main_surface->DrawString(main_surface, getFirstTwoChars(tirages[ii].name), -1, 22 + ((ii - 1) * 37), screen_height, DSTF_TOPCENTER );
					ii ++;	
				}
				y_bottom_text = screen_height + font_height;
			} else
				y_bottom_text = screen_height;
			
			sprintf(dateStr, "%02d:%02d:%02d, %02d-%02d-%4d",timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900);
			lcdSetFont(FONT_ROBOTO, "UTF-8", 0,  14, 0);
			lcdprintfex(ALG_LEFT, y_bottom_text, dateStr);
			main_surface->SetColor(main_surface, colorInfo.r, colorInfo.g, colorInfo.b, colorInfo.a);
			lcdSetFont(FONT_ROBOTO, "UTF-8", 0,  16, 0);
			lcdprintfex(ALG_RIGHT, y_bottom_text, "TT: %.2f HT", total);
			main_surface->SetColor(main_surface, colorBlack.r, colorBlack.g, colorBlack.b, colorBlack.a);
			lcdSetFont(FONT_ROBOTO, "UTF-8", 0,  16, 0);
			lcdFlip();	
	LOOP:
			key = kbGetKeyMs(1 * 1000);
			switch(key)
			{
				case KEY_UP:
					select--;
					if (select < 0)
					{
						select = 0;
					}
					break;
				case KEY_DOWN:
					select++;
					if (select >= count)
					{
						select = count - 1;
					}
					break;
				case KEY_CANCEL:
					select = -1;
					running = 0;
					break;
				case KEY_TIMEOUT:
					break;
				case KEY_ENTER:
					*s = select;
					select = 10;
					running = 0;
					break;
				case KEY_FN:
					*s = select;
					select = 11;
					running = 0;
					break;
				case 0x08:
					*s = select;
					select = 12;
					running = 0;
					break;
				default:
					goto LOOP;
			}
		}
	retval = select;
	lcdclean();
	return retval;
}




// Beep (330)
void Beep(unsigned int frequency, unsigned int keepms)
{
	beep((unsigned int)frequency, (unsigned int)keepms);
	return;
}



void freeItems(Tirage* array, int size)
{
	int i = 0;
    if (array == NULL)
        return;
    while (i < size)
    {
        free(array[i].name);
		i++;
    }
    free(array);
}


int deleteInfoTirageByName(Tirage *array, int size, const char *name)
{
    int i = 0, y = 0;
    while (i < size)
    {
        if (strcmp(array[i].name, name) == 0 )
		{
			free(array[i].name);
			y = i;
			while (y < size - 1) {
				array[y] = array[y + 1];
				y++;
			}
			return size - 1;
		}
        i++;
    }
    return -1;
}


int hasInfoTirageByName(Tirage *array, int size, const char *name)
{
    int i = 0;
    while (i < size)
    {
        if (strcmp(array[i].name, name) == 0 )
            return i;
        i++;
    }
    return -1;
}


void addItem(Tirage** array, int* size, char *id, const char* name)
{
    Tirage* newArray = NULL;
    *size += 1;

    // Reallocate memory for the array of structures
    newArray = realloc(*array, *size * sizeof(Tirage));

    if (newArray != NULL)
    {
        *array = newArray;
        // Initialize the new item
//        (*array)[*size - 1].id = id;
        strcpy((*array)[*size - 1].id, id);
        (*array)[*size - 1].name = malloc(strlen(name) + 1);
        strcpy((*array)[*size - 1].name, name);
    }
    else
    {
        printf("Memory allocation failed.\n");
    }
}


int deleteInfoTirageByIndex(Tirage* list, int index, int size) 
{
	int i = 0;
    if (index < 0 || index >= size) {
        printf("Invalid index.\n");
        return -1;
    }
    i = (int)index;
    free(list[i].name);
    while (i < size - 1) {
        list[i] = list[i + 1];
        i++;
    }
    return size - 1;
}



int readServer(char ** data)
{
	int width, height, font_height;

	int ret =  read_from_file(SERVER_FILE, data);
	
	if (ret != 0) {
		lcdGetSize(&width, &height);
		font_height = lcdGetFontHeight();
		lcdclean();
		lcdprintfex(ALG_CENTER, height / 2 - font_height, "No Server Found!");
		lcdFlip();
		kbGetKey();
	}
	return ret;
}


void removeCharAtIndex(char* str, const int index) 
{
    int len = string_len(str);

    // Check if the index is valid
    if (index >= 0 && index < len) {
		int i = index;
        // Shift the characters to the left starting from the index
        while (i < len - 1) 
		{
            str[i] = str[i + 1];
			i++;
        }
        str[len - 1] = '\0';  // Null-terminate the string
    }
}


void setOptionBouleItem(BouleItem *item)
{
	int size = strlen(item->boul);
	if (size == 3)
		sprintf(&item->lotto, "L3");
	else if (size == 2)
		sprintf(&item->lotto, "BO");
	else if (size == 5)
	{
		sprintf(&item->lotto, "L5");
	}
	return;
}



int prn_current_y = 1;


void setPrnY(int _y)
{
	prn_current_y = _y;
}


int getPrnY()
{
	return prn_current_y;
}


void printerprintf(LCD_ALG alg, IDirectFBSurface *surface, const char * pszFmt,...)
{
	int width, height;
	int font_height;
	IDirectFBFont *font = NULL;

	char textbuf[4096];
	const char *utf8text;
	const char *pnewline = NULL;
	int str_length, temp_width;
	
	va_list arg;
	va_start(arg, pszFmt);
	vsnprintf(textbuf, sizeof(textbuf), pszFmt, arg);	
	va_end (arg);
	
	surface->GetFont(surface, &font);
	font->GetHeight(font, &font_height);	
	surface->GetSize(surface, &width, &height);

	font->GetStringBreak(font, textbuf, strlen(textbuf), width - 3, &temp_width, &str_length, &pnewline);
	utf8text = string_covert(textbuf, str_length);

	if (ALG_CENTER == alg)
		surface->DrawString(surface, utf8text, string_len(utf8text), width / 2 - (string_len(utf8text) / 2), prn_current_y, DSTF_TOPCENTER);
	else if (ALG_LEFT == alg)
		surface->DrawString(surface, utf8text, string_len(utf8text), 3, prn_current_y, DSTF_TOPLEFT);
	else 
		surface->DrawString(surface, utf8text, string_len(utf8text), width - 5, prn_current_y, DSTF_TOPRIGHT);		
		
	prn_current_y = prn_current_y + font_height + 1;

	if (NULL != pnewline)
		printerprintf(alg, surface, pnewline);
}




// void addTirageItem(Tirage** array, int* size, char *id, const char* boules, const char *tirage_name, const char *montant, int _id, const char* created)
// {
//     Tirage* newArray = NULL;
//     *size += 1;
//     // Reallocate memory for the array of structures
//     newArray = realloc(*array, *size * sizeof(Tirage));

//     if (newArray != NULL)
//     {
//         *array = newArray;
//         // Initialize the new item
//         // (*array)[*size - 1].id = id;
// 		sprintf((*array)[*size - 1].id, id);
// 		(*array)[*size - 1]._id =  _id;

//         (*array)[*size - 1].boules = malloc(strlen(boules) + 1);
//         strcpy((*array)[*size - 1].boules, boules);
// 		(*array)[*size - 1].created = malloc(strlen(created) + 1);
//         strcpy((*array)[*size - 1].created, created);

//         (*array)[*size - 1].tirage_name = malloc(strlen(tirage_name) + 1);
//         strcpy((*array)[*size - 1].tirage_name, tirage_name);
// 		sprintf((*array)[*size - 1].montant, montant);
//     }
//     else
//     {
//         printf("Memory allocation failed.\n");
//     }
// }


// void freeTirageItems(Tirage* array, int size)
// {
// 	int i = 0;
//     if (array == NULL)
//         return;
//     while (i < size)
//     {
//         free(array[i].boules);
//         free(array[i].tirage_name);
//         free(array[i].created);

// 		i++;
//     }
//     free(array);
// 	array = NULL;
// }


// int deleteTirageByIndex(Tirage* list, int index, int size) 
// {
// 	int i = 0;
//     if (index < 0 || index >= size) {
//         printf("Invalid index.\n");
//         return -1;
//     }
//     i = (int)index;
//     free(list[i].boules);
// 	free(list[i].tirage_name);
// 	free(list[i].created);
//     while (i < size - 1) {
//         list[i] = list[i + 1];
//         i++;
//     }
//     return size - 1;// Decrement the size to reflect the deleted element
// }
