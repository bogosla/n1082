/*
* modalYesNo..c
*
*  Created on: Jul 31, 2023
*      Author: James DESTINE
*/
#include "modalYesNo.h"


int yesNo(char* text)
{
    IDirectFB *dfb = NULL;
	DFBSurfaceDescription	surfdesc;
	IDirectFBDisplayLayer	*layer	= NULL;
    IDirectFBWindow			*window	= NULL;
    IDirectFBEventBuffer 	*events	= NULL;

    IDirectFBSurface        *main_surface = NULL;
    IDirectFBFont		    *font_32 = NULL;
    IDirectFBFont		    *font_16 = NULL;

	DFBWindowDescription	desc;
	DFBDisplayLayerConfig	config;
	DFBFontDescription	    fdesc;
    int retval = 0;

	int x, y, height, width = 0;


    int font_width, font_height;

	dfb = dfb_get_directfb();

    layer = dfb_get_primay_layer();
    lcdGetSize(&width, &height);

    desc.flags  = DWDESC_POSX | DWDESC_POSY | DWDESC_WIDTH | DWDESC_HEIGHT | DWDESC_CAPS;
    desc.posx	= 32;
    desc.posy	= 76;
    desc.width  = width - 64;
    desc.height = 96;
    desc.caps	= DWCAPS_ALPHACHANNEL;

    if (0 !=  layer->CreateWindow(layer, &desc, &window))
    {
        printf("Create Window Failed\r\n");
        retval = -1;
    }
    else if (0 !=  window->CreateEventBuffer(window, &events))
    {
        printf("GetConfiguration Failed\r\n");
        retval = -1;
    }else if (0 != window->GetSurface(window, &main_surface))
    {
        printf("GetSurface Failed\r\n");
        retval = -1;
    }else if (0 != main_surface->GetSize(main_surface, &width, &height))
    {
        printf("GetSize Failed\r\n");
        retval = -1;
    }


    if (0 == retval){
		fdesc.flags  = DFDESC_HEIGHT;
		fdesc.height = 16;
		dfb->CreateFont(dfb, FONT_ROBOTO, &fdesc, &font_16);
		if (NULL == font_16){
			printf("create font16 failed.\r\n");
			retval = -1;
		}

		fdesc.height = 23;
		dfb->CreateFont(dfb, FONT_ROBOTO, &fdesc, &font_32);
		if (NULL == font_32){
			printf("create font16 failed.\r\n");
			retval = -1;
		}
	}

    if (0 == retval){
		// window->SetOptions(window, DWOP_ALPHACHANNEL);
		window->SetOpacity(window, 0xAA);
    	window->RaiseToTop(window);
		main_surface->SetFont(main_surface, font_32);
        main_surface->Clear(main_surface, colorLight.r, colorLight.g, colorLight.b, colorLight.a);
		main_surface->SetColor(main_surface, colorBlack.r, colorBlack.g, colorBlack.b, colorBlack.a);
		main_surface->DrawString(main_surface, text, -1, width / 2, 7, DSTF_TOPCENTER);

        // Button Cancel
		main_surface->SetFont(main_surface, font_16);
		main_surface->DrawString(main_surface, "Non(CANCEL)", -1, 10, 37, DSTF_TOPLEFT);
        // Button Continue
		main_surface->DrawString(main_surface, "Wi(ENTER)", -1, width - 25, 37, DSTF_TOPRIGHT);
		// main_surface->DrawRectangle(main_surface, 1, 1, desc.width - 1, desc.height - 1);

        main_surface->Flip(main_surface, NULL, 0);
        IDirectFBWindow* curWin = lcdGetFgWindow();
        int running = 1;

        while (running == 1)
        {
            int key = kb_getkey(events);
            switch (key)
            {
                case KEY_ENTER:
                    retval = -10;
                    running = 0;
                    break;
                case KEY_CANCEL:
                    retval = 1;
                    running = 0;
                    break;
                default:
                    break;
            }
        }

        window->Close(window);

       	if (NULL != events)
		    events->Release(events);

        if (NULL != main_surface)
            main_surface->Release(main_surface);

        if (NULL != window)
            window->Release(window);
	}

    return retval;
}
