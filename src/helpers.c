/*
 * helpers.c
 *
 *  Created on: Apr 22, 2024
 *      Author: James DESTINE
 */

#include "helpers.h"


static int current_y = 1; // Global position y


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
    status_bar_height = sys_get_status_bar_height();
    main_surface->GetSize(main_surface, &width, &height);
    main_surface->GetFont(main_surface, &font);
    font->GetHeight(font, &font_height);

    main_surface->SetColor(main_surface, colorPrimary.r, colorPrimary.g, colorPrimary.b, colorPrimary.a);
    main_surface->FillRectangle(main_surface, 0, 0, width, status_bar_height);
    main_surface->SetColor(main_surface, colorWhite.r, colorWhite.g, colorWhite.b, colorWhite.a);
    lcdprintfex(ALG_CENTER, (status_bar_height - font_height) / 2 + 2, pszFmt);
    main_surface->SetColor(main_surface, colorBlack.r, colorBlack.g, colorBlack.b, colorBlack.a);
	current_y = current_y + 3;
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

	current_y = current_y + font_height + 1;
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
			max_lines = 8; // (screen_height - current_y - 1) / font_height;
			istart = (select / max_lines) * max_lines;

			for (i = 0;  i < max_lines; i++)
			{
				if (istart + i < count)
				{
					if (istart + i == select)
					{
						main_surface->SetColor(main_surface, colorSecondary.r, colorSecondary.g, colorSecondary.b, colorSecondary.a);
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


// Beep (330)
void Beep(unsigned int frequency, unsigned int keepms)
{
	beep((unsigned int)frequency, (unsigned int)keepms);
	return;
}


