/*
 * gui.c
 *
 *  Created on: Apr 24, 2024
 *      Author: James DESTINE
 */

#include "gui.h"


void renderDDate(IDirectFBSurface *surface, DDate *entry)
{
    IDirectFBFont *font = NULL;
    char _highlight[11];
    int font_height, _center;
    memset(_highlight, 0x00, sizeof(_highlight));

    surface->GetFont(surface, &font);
	font->GetHeight(font, &font_height);
	if (entry->active)
		surface->SetColor(surface, colorPrimary.r, colorPrimary.g, colorPrimary.b, colorPrimary.a);
	else
		surface->SetColor(surface, colorGrey.r, colorGrey.g, colorGrey.b, colorGrey.a);
	surface->DrawRectangle(surface, entry->x, entry->y, entry->width, entry->height);
	surface->SetColor(surface, colorBlack.r, colorBlack.g, colorBlack.b, colorBlack.a);
    _center = entry->y + (entry->height - font_height) / 2;
	surface->DrawString(surface, entry->text, -1, entry->x + 3, _center, DSTF_TOPLEFT);
    
    if (entry->cursor_pos >= 0 && entry->active)
    {
        if (entry->cursor_pos == 4 || entry->cursor_pos == 7)
            strncat(_highlight, entry->text, entry->cursor_pos + 2);
        else
            strncat(_highlight, entry->text, entry->cursor_pos + 1);


        // if (entry->cursor_pos == 4 || entry->cursor_pos == 7)
        //     _highlight[entry->cursor_pos + 1] = entry->text[entry->cursor_pos + 1];    
        // else
        //     _highlight[entry->cursor_pos] = entry->text[entry->cursor_pos];

		surface->SetColor(surface, colorPrimary.r, colorPrimary.g, colorPrimary.b, colorPrimary.a);
	    surface->DrawString(surface, _highlight, -1, entry->x + 3, _center + 1, DSTF_TOPLEFT);
    	surface->SetColor(surface, colorBlack.r, colorBlack.g, colorBlack.b, colorBlack.a);
    }
}


void renderEntry(IDirectFBSurface *surface, Entry* entry)
{
    IDirectFBFont *font = NULL;
    int font_height, _center, _i = 0;
    surface->GetFont(surface, &font);
	font->GetHeight(font, &font_height);
	if (entry->active)
		surface->SetColor(surface, colorPrimary.r, colorPrimary.g, colorPrimary.b, colorPrimary.a);
	else
		surface->SetColor(surface, colorGrey.r, colorGrey.g, colorGrey.b, colorGrey.a);
	surface->DrawRectangle(surface, entry->x, entry->y, entry->width, entry->height);
	surface->SetColor(surface, colorBlack.r, colorBlack.g, colorBlack.b, colorBlack.a);
    _center = entry->y + (entry->height - font_height) / 2;
    if (entry->etype == HIDDEN)
    {
        char _hidden[256] = {""};
        if (entry->cursor_pos > 0)
        {
            while (_i < entry->cursor_pos)
            {
                _hidden[_i] = '*';
                _i++;
            }
            _hidden[_i-1] = entry->text[entry->cursor_pos - 1];
            surface->DrawString(surface, _hidden, -1, entry->x + 3, _center + 4, DSTF_TOPLEFT);
        }
    } else
        surface->DrawString(surface, entry->text, -1, entry->x + 3, _center, DSTF_TOPLEFT);
}


static int lastKey = 0;    
static int count = 0;


void handleDDateInput(int keysym, DDate *entry)
{
    if (keysym >= 0x30 && keysym <= 0x39)
    {
        if (entry->cursor_pos < sizeof(entry->text) - 1)
        {
            if (entry->cursor_pos == 4 || entry->cursor_pos == 7) {
                entry->cursor_pos++;
            }
            entry->text[entry->cursor_pos] = keysym - DIKS_0 + '0';
            entry->cursor_pos++;
        }
        if (entry->cursor_pos >= 10)
            entry->cursor_pos = 0;
    }
}


void handleEntryInput(int keysym, Entry *entry)
{
    int _char;
    if (keysym >= 0x30 && keysym <= 0x39)
    {
        if (entry->cursor_pos < sizeof(entry->text) - 1)
        {
            if (entry->cursor_pos > entry->max_length - 1)
                return;
            if (entry->type == ALPHA)
                lastKey = keysym;
            count = 0;
            memmove(&entry->text[entry->cursor_pos + 1], &entry->text[entry->cursor_pos], sizeof(entry->text) - entry->cursor_pos - 1);
            entry->text[entry->cursor_pos] = keysym - DIKS_0 + '0';
            entry->cursor_pos++;
        }
    } 
    else if (keysym == 0x08)
    {
        lastKey = 0;
        count = 0;
        if (entry->cursor_pos > 0)
        {
            entry->cursor_pos--;
            memmove(&entry->text[entry->cursor_pos], &entry->text[entry->cursor_pos + 1], sizeof(entry->text) - entry->cursor_pos);
        }
    }
    else if (keysym == 0xF201)
    {
        switch (lastKey) 
        {
            case 0x31: // Button 1
                count++;
                count = count % 5;
                switch (count)
                {
                    case 0:
                        _char = 49;
						break;
                    case 1:
                        _char = 113;
						break;
                    case 2:
                        _char = 122;
						break;
                    case 3:
                        _char = 81;
						break;
                    case 4:
                        _char = 90;
						break;
                    default:
                        break;
                }
                entry->text[entry->cursor_pos - 1] = (char) _char;
                break;
            case 0x32: // Button 2
                count++;
                count = count % 7;
                switch (count)
                {
                    case 0:
                        _char = 50;
						break;
                    case 1:
                        _char = 97;
						break;
                    case 2:
                        _char = 98;
						break;
                    case 3:
                        _char = 99;
						break;
                    case 4:
                        _char = 65;
						break;
                    case 5:
                        _char = 66;
						break;
                    case 6:
                        _char = 67;
                        break;
                    default:
                        break;
                }
                entry->text[entry->cursor_pos - 1] = (char) _char;
                break;
            case 0x33: // Button 3
                count++;
                count = count % 7;
                switch (count)
                {
                    case 0:
                        _char = 51;
						break;
                    case 1:
                        _char = 100;
						break;
                    case 2:
                        _char = 101;
						break;
                    case 3:
                        _char = 102;
						break;
                    case 4:
                        _char = 68;
						break;
                    case 5:
                        _char = 69;
						break;
                    case 6:
                        _char = 70;
						break;
                    default:
                        break;
                }
                entry->text[entry->cursor_pos - 1] = (char) _char;
                break;
            case 0x34: // Button 4
                count++;
                count = count % 7;
                switch (count)
                {
                    case 0:
                        _char = 52;
						break;
                    case 1:
                        _char = 103;
						break;
                    case 2:
                        _char = 104;
						break;
                    case 3:
                        _char = 105;
						break;
                    case 4:
                        _char = 71;
						break;
                    case 5:
                        _char = 72;
						break;
                    case 6:
                        _char = 73;
						break;
                    default:
                        break;
                }
                entry->text[entry->cursor_pos - 1] = (char) _char;
                break;
            case 0x35: // Button 5
                count++;
                count = count % 7;
                switch (count)
                {
                    case 0:
                        _char = 53;
						break;
                    case 1:
                        _char = 106;
						break;
                    case 2:
                        _char = 107;
						break;
                    case 3:
                        _char = 108;
						break;
                    case 4:
                        _char = 74;
						break;
                    case 5:
                        _char = 75;
						break;
                    case 6:
                        _char = 76;
						break;
                    default:
                        break;
                }
                entry->text[entry->cursor_pos - 1] = (char) _char;
                break;
            case 0x36: // Button 6
                count++;
                count = count % 7;
                switch (count)
                {
                    case 0:
                        _char = 54;
						break;
                    case 1:
                        _char = 109;
						break;
                    case 2:
                        _char = 110;
						break;
                    case 3:
                        _char = 111;
						break;
                    case 4:
                        _char = 77;
						break;
                    case 5:
                        _char = 78;
						break;
                    case 6:
                        _char = 79;
						break;
                    default:
                        break;
                }
                entry->text[entry->cursor_pos - 1] = (char) _char;
                break;
            case 0x37: // Button 7
                count++;
                count = count % 7;
                switch (count)
                {
                    case 0:
                        _char = 55;
						break;
                    case 1:
                        _char = 112;
						break;
                    case 2:
                        _char = 114;
						break;
                    case 3:
                        _char = 115;
						break;
                    case 4:
                        _char = 80;
						break;
                    case 5:
                        _char = 82;
						break;
                    case 6:
                        _char = 83;
						break;
                    default:
                        break;
                }
                entry->text[entry->cursor_pos - 1] = (char) _char;
                break;
            case 0x38: // Button 8
                count++;
                count = count % 7;
                switch (count)
                {
                    case 0:
                        _char = 56;
						break;
                    case 1:
                        _char = 116;
						break;
                    case 2:
                        _char = 117;
						break;
                    case 3:
                        _char = 118;
						break;
                    case 4:
                        _char = 84;
						break;
                    case 5:
                        _char = 85;
						break;
                    case 6:
                        _char = 86;
						break;
                    default:
                        break;
                }
                entry->text[entry->cursor_pos - 1] = (char) _char;
                break;
            case 0x39: // Button 9
                count++;
                count = count % 7;
                switch (count)
                {
                    case 0:
                        _char = 57;
						break;
                    case 1:
                        _char = 119;
						break;
                    case 2:
                        _char = 120;
						break;
                    case 3:
                        _char = 121;
						break;
                    case 4:
                        _char = 87;
						break;
                    case 5:
                        _char = 88;
						break;
                    case 6:
                        _char = 89;
						break;
                    default:
                        break;
                }
                entry->text[entry->cursor_pos - 1] = (char) _char;
                break;
            case 0x30: // Button 0
                count++;
                count = count % 29;
                switch (count)
                {
                    case 0:
                        _char = 48;
						break;
                    case 1:
                        _char = 39;
						break;
                    case 2:
                        _char = 33;
						break;
                    case 3:
                        _char = 34;
						break;
                    case 4:
                        _char = 35;
						break;
                    case 5:
                        _char = 36;
						break;
                    case 6:
                        _char = 37;
						break;
                    case 7:
                        _char = 38;
						break;
                    case 8:
                        _char = 40;
						break;
                    case 9:
                        _char = 41;
						break;
                    case 10:
                        _char = 42;
						break;
                    case 11:
                        _char = 43;
						break;
                    case 12:
                        _char = 44;
						break;
                    case 13:
                        _char = 45;
						break;
                    case 14:
                        _char = 46;
						break;
                    case 15:
                        _char = 47;
						break;
                    case 16:
                        _char = 58;
						break;
                    case 17:
                        _char = 59;
						break;
                    case 18:
                        _char = 60;
						break;
                    case 19:
                        _char = 61;
						break;
                    case 20:
                        _char = 62;
						break;
                    case 21:
                        _char = 63;
						break;
                    case 22:
                        _char = 64;
						break;
                    case 23:
                        _char = 91;
						break;
                    case 24:
                        _char = 92;
						break;
                    case 25:
                        _char = 93;
						break;
                    case 26:
                        _char = 94;
						break;
                    case 27:
                        _char = 95;
						break;
                    case 28:
                        _char = 96;
						break;
                    default:
                        break;
                }
                entry->text[entry->cursor_pos - 1] = (char) _char;
                break;
            default:
                break;
        }
    }
}
