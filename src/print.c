/*
 * print.c
 *
 *  Created on: Apr 22, 2024
 *      Author: James DESTINE
 */

#include "print.h"
#include <seos.h>





void TestPrinter(void) 
{
    int status = 0;
    prnInit();

    lcdclean();
	lcdprintf(ALG_LEFT, "Prepare for print...");
	lcdFlip();


    prnPrintf("Mgn&Co\n");
    prnPrintf("Printer test is OK\n");
    prnPrintf("Close!!!\n");
    prnStart();
    do
    {
        usleep(100000);
        status = prnStatus();
        if (((status  >> PRINTER_STATUS_BUSY) & 0x01) == 0x01)
            lcdprintf(ALG_LEFT, "printer status = busy");
        else if (((status  >> PRINTER_STATUS_HIGHT_TEMP) & 0x01) == 0x01)
            lcdprintf(ALG_LEFT, "printer status = tempreture high");
        else if (((status  >> PRINTER_STATUS_PAPER_LACK) & 0x01) == 0x01)
            lcdprintf(ALG_LEFT, "printer status = no paper");
        else if (((status  >> PRINTER_STATUS_FEED) & 0x01) == 0x01)
            lcdprintf(ALG_LEFT, "printer status = feed paper");
        else if (((status  >> PRINTER_STATUS_PRINT) & 0x01) == 0x01)
            lcdprintf(ALG_LEFT, "printer status = printing");
        else if (((status  >> PRINTER_STATUS_FORCE_FEED) & 0x01) == 0x01)
                lcdprintf(ALG_LEFT, "printer status = force feed paper");
        else if (((status  >> PRINTER_STATUS_POWER_ON) & 0x01) == 0x01)
            lcdprintf(ALG_LEFT, "printer status = power on");
        lcdFlip();

    } while (status != 0);
    prnStep(5);

    prnClose();
	lcdprintf(ALG_LEFT, "Press any key.");
    lcdFlip();

    kbGetKey();
    return;
}


int print_fiche(const char *buffer)
{
	cJSON *json = NULL;
	cJSON *ficheId = NULL;
	cJSON *ficheTirage = NULL;
	cJSON *total = NULL;
	cJSON *bouleList = NULL;
	cJSON *ficheDate = NULL;
	cJSON *agentId = NULL;

	cJSON *json2 = NULL;
	cJSON *banknameIn = NULL;

	cJSON *fiches = NULL;

	cJSON *company_desc = NULL;
	cJSON *company_name = NULL;

	cJSON *first_ticket = NULL;
	cJSON *agentaddressIn = NULL;
	cJSON *companyphoneIn = NULL;
	cJSON *companyPhone = NULL;

	int ifd = -1;
	int retval = 0;

	printer_param_t  param;
	printer_status_t status;
	IDirectFB *dfb = NULL;
	DFBSurfaceDescription	surfdesc;

	IDirectFBSurface        *surface = NULL;
	IDirectFBFont		    *font_24 = NULL;
	IDirectFBFont		    *font_32 = NULL;
	IDirectFBFont		    *font_48 = NULL;
	DFBFontDescription	    fdesc;

	char *userData = NULL;
	char imei[15], str[12];

	int x, y, height, width = 0;
	dfb = dfb_get_directfb();
	
	memset(imei, 0x00, sizeof(imei));
	memset(&fdesc, 0, sizeof(fdesc));

	sysReadSN(imei);

	fdesc.flags  = DFDESC_HEIGHT;
	fdesc.height = 32;

	dfb->CreateFont(dfb, ARIAL_FONT, &fdesc, &font_32);
	
    if (NULL == font_32){
		lcdprintf(ALG_LEFT, "craete font32 failed.");
		lcdFlip();
		retval = -1;
	}

	if (0 == retval){
		fdesc.height = 24;	
		dfb->CreateFont(dfb, ARIAL_FONT, &fdesc, &font_24);
		if (NULL == font_24){
			lcdprintf(ALG_LEFT, "craete font24 failed");
			lcdFlip();
			retval = -1;	
		}
	}

	if (0 == retval){
		fdesc.height = 42;	
		dfb->CreateFont(dfb, ARIAL_FONT, &fdesc, &font_48);
		if (NULL == font_48){
			lcdprintf(ALG_LEFT, "craete font48 failed");
			lcdFlip();
			retval = -1;	
		}
	}

	json = cJSON_Parse(buffer);

	if (json == NULL)
	{
		lcdprintf(ALG_LEFT, "Error while parsing..");
		lcdFlip();
		kbGetKey();
	} else 
	{
		if (0 == retval){
			ifd = printer_open(printer_device_name, O_WRONLY | O_NONBLOCK);
			if (ifd < 0){
				lcdprintf(ALG_LEFT, "Device Open failed");
				lcdprintf(ALG_LEFT, "Module Not Support");
				lcdFlip();
				kbGetKey();
				return 1;
			}
		}

		if (0 == retval)
		{
			printer_get_param(ifd, &param);
			surfdesc.flags = DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT;
			surfdesc.caps  = DSCAPS_NONE;
			surfdesc.width = param.ro_width;
			surfdesc.height= 6000;
			dfb->CreateSurface(dfb, &surfdesc, &surface);
			surface->Clear(surface, 
				0xFF,
				0xFF,
				0xFF,
				0xFF
			);
			
			surface->SetColor(surface, 0x00, 0x00, 0x00, 0xFF);

			width = surfdesc.width;
			x = param.ro_width/2;
			y = 20;	
			read_from_file(USERNAME_FILE, &userData);
			
            fiches = cJSON_GetObjectItemCaseSensitive(json, "tickets");
            total = cJSON_GetObjectItemCaseSensitive(json, "montants");
			ficheDate = cJSON_GetObjectItemCaseSensitive(json, "ticket_date");

			surface->SetFont(surface, font_48);
			font_48->GetHeight(font_48, &height);	

			json2 = cJSON_Parse(userData);
			first_ticket = cJSON_GetArrayItem(fiches, 0);

			agentId = cJSON_GetObjectItemCaseSensitive(json2, "username");
			company_name = cJSON_GetObjectItemCaseSensitive(json2, "bank_name");
			banknameIn = cJSON_GetObjectItemCaseSensitive(json2, "banknameIn");
			company_desc = cJSON_GetObjectItemCaseSensitive(json2, "bank_description");
			agentaddressIn = cJSON_GetObjectItemCaseSensitive(json2, "vendoraddressIn");
			// cJSON *agentnameIn = cJSON_GetObjectItemCaseSensitive(user, "agentnameIn");
			companyphoneIn = cJSON_GetObjectItemCaseSensitive(json2, "bankphoneIn");

			cJSON *addresse_complet = cJSON_GetObjectItemCaseSensitive(json2, "address");
			// cJSON *username = cJSON_GetObjectItemCaseSensitive(user, "username");
			companyPhone = cJSON_GetObjectItemCaseSensitive(json2, "phone");
			
			// Set Logo
			drawpicture(x - (176 / 2), y, 176, 144, "/home/user0/mgnco/logo_48.png", surface);
			y += 147;
			surface->DrawString(surface, company_name->valuestring, -1, x, y, DSTF_TOPCENTER);
			y+= height + 3;

			surface->SetFont(surface, font_24);
			font_24->GetHeight(font_24, &height);
			setPrnY(y);
			printerprintf(ALG_CENTER, surface, "POS: %s", imei);
			y = getPrnY();
			setPrnY(y);
			printerprintf(ALG_CENTER, surface, "%s", ficheDate->valuestring);
			y = getPrnY();

			// Display new field
			if (cJSON_IsBool(companyphoneIn) && cJSON_IsTrue(companyphoneIn))
			{
				setPrnY(y);
				printerprintf(ALG_CENTER, surface, "Tel: %s", companyPhone->valuestring);
				y = getPrnY();
			}
			setPrnY(y);
			printerprintf(ALG_CENTER, surface, "Agent: %s", agentId->valuestring);
			y = getPrnY();

			if (cJSON_IsBool(agentaddressIn) && cJSON_IsTrue(agentaddressIn))
			{
				setPrnY(y);
				printerprintf(ALG_CENTER, surface, "Address: %s", addresse_complet->valuestring);
				y = getPrnY();
			}

			if (cJSON_IsArray(fiches))
			{
				cJSON *element;
				// Iterate over the array elements
				cJSON_ArrayForEach(element, fiches) 
				{
					cJSON *tn = cJSON_GetObjectItemCaseSensitive(element, "ticket_tirage");
					cJSON *tid = cJSON_GetObjectItemCaseSensitive(element, "ticket_ref_code");

					setPrnY(y);
					printerprintf(ALG_CENTER, surface, "Tirage: %s", tn->valuestring);
					y = getPrnY();	
					setPrnY(y);
					printerprintf(ALG_CENTER, surface, "#Ticket: %s", tid->valuestring);
					y = getPrnY();
				}
			}

			setPrnY(y);
			// if (reprint == 1)
			// 	printerprintf(ALG_CENTER, surface, "*****Fiche Copie*****");
			// else
			printerprintf(ALG_CENTER, surface, "*****Fiche Originale*****");

			y = getPrnY();

			setPrnY(y);
			printerprintf(ALG_CENTER, surface, "-----------------------------------------------");
			y = getPrnY() + 12;

			surface->DrawString(surface, "Jeu", -1, 3, y,  DSTF_LEFT);
			surface->DrawString(surface, "Boule", -1, width*0.3, y,  DSTF_CENTER);
			surface->DrawString(surface, "Option", -1, width*0.6, y,  DSTF_CENTER);
			surface->DrawString(surface, "Mise", -1, width*0.9 - 5, y,  DSTF_RIGHT);
			y+= height + 3;

			bouleList = cJSON_GetObjectItemCaseSensitive(first_ticket, "bouleList");

			if (cJSON_IsArray(bouleList))
			{
				cJSON *element;
				// Iterate over the array elements
				cJSON_ArrayForEach(element, bouleList) 
				{
					cJSON *boule = cJSON_GetObjectItemCaseSensitive(element, "boule");
					cJSON *lotto = cJSON_GetObjectItemCaseSensitive(element, "lotto");
					cJSON *montant = cJSON_GetObjectItemCaseSensitive(element, "montant");
					cJSON *option = cJSON_GetObjectItemCaseSensitive(element, "option");
					surface->DrawString(surface, lotto->valuestring, -1, 3, y,  DSTF_LEFT);
					surface->DrawString(surface, boule->valuestring, -1, width*0.3, y,  DSTF_CENTER);
					surface->DrawString(surface, option->valuestring, -1, width*0.6, y,  DSTF_CENTER);
					sprintf(str, "%.2f", montant->valuedouble);
					surface->DrawString(surface,  str, -1, width*0.9 - 5, y,  DSTF_CENTER);
					y+= height + 10;
				}
			}
		

			surface->SetFont(surface, font_32);
			font_32->GetHeight(font_32, &height);	
			surface->DrawString(surface, "TOTAL: ", -1, 3, y,  DSTF_LEFT);
			sprintf(str, "%.2f HTG", total->valuedouble);
			surface->DrawString(surface, str, -1, width - 12, y,  DSTF_RIGHT);
			surface->SetFont(surface, font_24);
			font_24->GetHeight(font_24, &height);		
			y+= height + 1;
			surface->DrawString(surface, "---------------------------------------------", -1, 0, y,  DSTF_LEFT);
			y+= height + 1;
			setPrnY(y);
			printerprintf(ALG_LEFT, surface, "%s", company_desc->valuestring);
			y = getPrnY();
		}

		if (0 == retval)
		{		
			lcdclean();
			lcdprintf(ALG_LEFT, "Printing....");
			lcdFlip();
			print_surface(ifd, surface, y + 100);
			do
			{
				usleep(100000);
				printer_get_status(ifd, &status);
				if (((status.status  >> PRINTER_STATUS_BUSY) & 0x01) == 0x01)
					lcdprintf(ALG_LEFT, "printer status = busy");
				else if (((status.status  >> PRINTER_STATUS_HIGHT_TEMP) & 0x01) == 0x01)
					lcdprintf(ALG_LEFT, "printer status = tempreture high");
				else if (((status.status  >> PRINTER_STATUS_PAPER_LACK) & 0x01) == 0x01)
					lcdprintf(ALG_LEFT, "printer status = no paper");
				else if (((status.status  >> PRINTER_STATUS_FEED) & 0x01) == 0x01)
					lcdprintf(ALG_LEFT, "printer status = feed paper");
				else if (((status.status  >> PRINTER_STATUS_PRINT) & 0x01) == 0x01)
					lcdprintf(ALG_LEFT, "printer status = printing");
				else if (((status.status  >> PRINTER_STATUS_FORCE_FEED) & 0x01) == 0x01)
						lcdprintf(ALG_LEFT, "printer status = force feed paper");
				else if (((status.status  >> PRINTER_STATUS_POWER_ON) & 0x01) == 0x01)
					lcdprintf(ALG_LEFT, "printer status = power on");
				lcdFlip();

			} while (status.status != 0);
			
			lcdprintf(ALG_LEFT, "Finished.");
			lcdFlip();
		}
        if (json != NULL) {
		    cJSON_Delete(json);
            json = NULL;
        }

	}

	if (userData != NULL) {
		free(userData);
        userData = NULL;
    }

	if (NULL != surface) {
		surface->Release(surface);
        surface = NULL;

    }

	if (NULL != font_32) {
		font_32->Release(font_32);
        font_32 = NULL;
    }

	if (NULL != font_24) {
		font_24->Release(font_24);
        font_24 = NULL;
    }

    if (NULL != font_48) {
		font_48->Release(font_48);
        font_48 = NULL;
    }

	if (ifd >= 0)
	{
		printer_close(ifd);
		ifd = -1;
	}
	return ifd;
}


int print_rapport(const char *buffer)
{
	int key;
	int screen_height, screen_width;
	int ifd = -1;
	int retval = 0;

	printer_param_t  param;
	printer_status_t status;
	IDirectFB *dfb = NULL;
	DFBSurfaceDescription	surfdesc;
	IDirectFBSurface        *surface = NULL;
	IDirectFBFont		    *font_24 = NULL;
	IDirectFBFont		    *font_32 = NULL;
	IDirectFBFont		    *font_48 = NULL;
	DFBFontDescription	    fdesc;

	char *userData = NULL;
	cJSON *json = NULL;
	cJSON *json2 = NULL;
	cJSON *user = NULL;
	cJSON *company_name = NULL;

	int x, y, height, width = 0;
	char imei[20];
	memset(imei, 0x00, sizeof(imei));
	memset(&fdesc, 0, sizeof(fdesc));

	sysReadSN(imei);
	dfb = dfb_get_directfb();
	lcdGetSize(&screen_width, &screen_height);

	fdesc.flags  = DFDESC_HEIGHT;
	fdesc.height = 32;
	dfb->CreateFont(dfb, ARIAL_FONT, &fdesc, &font_32);

	if (NULL == font_32){
		lcdprintf(ALG_LEFT, "craete font32 failed.");
		lcdFlip();
		retval = -1;
	}

	if (0 == retval){
		fdesc.height = 24;	
		dfb->CreateFont(dfb, ARIAL_FONT, &fdesc, &font_24);
		if (NULL == font_24){
			lcdprintf(ALG_LEFT, "craete font24 failed");
			lcdFlip();
			retval = -1;	
		}
	}

	if (0 == retval){
		fdesc.height = 42;	
		dfb->CreateFont(dfb, ARIAL_FONT, &fdesc, &font_48);
		if (NULL == font_48){
			lcdprintf(ALG_LEFT, "craete font48 failed");
			lcdFlip();
			retval = -1;	
		}
	}

	json = cJSON_Parse(buffer);

	if (json == NULL)
	{
		lcdprintf(ALG_LEFT, "Error while parsing..");
		lcdFlip();
		kbGetKey();
	} else 
	{
		if (0 == retval)
		{
			ifd = printer_open(printer_device_name, O_WRONLY | O_NONBLOCK);
			if (ifd < 0)
			{
				lcdprintf(ALG_LEFT, "Device Open failed");
				lcdprintf(ALG_LEFT, "Module Not Support");
				lcdFlip();
				kbGetKey();
				return 1;
			}
		}

		if (0 == retval)
		{
			printer_get_param(ifd, &param);
			surfdesc.flags = DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT;
			surfdesc.caps  = DSCAPS_NONE;
			surfdesc.width = param.ro_width;
			surfdesc.height= 3000;
			dfb->CreateSurface(dfb, &surfdesc, &surface);
			surface->Clear(surface, 
				0xFF,
				0xFF,
				0xFF,
				0xFF
			);
			
			surface->SetColor(surface, 0x00, 0x00, 0x00, 0xFF);
			width = surfdesc.width;
			x = param.ro_width/2;
			y = 20;	
			read_from_file(USERNAME_FILE, &userData);
			json2 = cJSON_Parse(userData);
			company_name = cJSON_GetObjectItemCaseSensitive(json2, "bank_name");
			
			surface->SetFont(surface, font_48);
			font_48->GetHeight(font_48, &height);		
			drawpicture(x - (176 / 2), y, 176, 144, "/home/user0/mgnco/logo_48.png", surface);
			y += 147;
			surface->DrawString(surface, company_name->valuestring, -1, x, y, DSTF_TOPCENTER);
			y+= height + 3;


			surface->SetFont(surface, font_24);
			font_24->GetHeight(font_24, &height);
			setPrnY(y);
			printerprintf(ALG_LEFT, surface, "Companie  : %s", company_name->valuestring);
			y = getPrnY();
			setPrnY(y);
			printerprintf(ALG_LEFT, surface, "POS : %s", imei);
			y = getPrnY() + 14;
			surface->DrawString(surface, "---------------------------------------------", -1, 0, y,  DSTF_LEFT);
			y+= 4;
			

			cJSON *start = cJSON_GetObjectItemCaseSensitive(json, "start_date");
			cJSON *end = cJSON_GetObjectItemCaseSensitive(json, "end_date");

			cJSON *quantite = cJSON_GetObjectItemCaseSensitive(json, "counts");
			cJSON *commission = cJSON_GetObjectItemCaseSensitive(json, "gain_with_percent_vendor");
			cJSON *withoutCommission = cJSON_GetObjectItemCaseSensitive(json, "gain_without_percent_vendor");
			cJSON *fGagant = cJSON_GetObjectItemCaseSensitive(json, "wins");
			cJSON *vente = cJSON_GetObjectItemCaseSensitive(json, "montants");
			cJSON *perte = cJSON_GetObjectItemCaseSensitive(json, "pertes");

			if (cJSON_IsNumber(quantite) && cJSON_IsNumber(commission) && cJSON_IsNumber(perte)) {
			
				setPrnY(y);
				printerprintf(ALG_LEFT, surface, "Start Date     : %s", start->valuestring);
				y = getPrnY();
                setPrnY(y);
				printerprintf(ALG_LEFT, surface, "End Date     : %s", end->valuestring);
				y = getPrnY();
				setPrnY(y);
				printerprintf(ALG_LEFT, surface, "Num Fiches     : %d", quantite->valueint);
				y = getPrnY();
				setPrnY(y);
				printerprintf(ALG_LEFT, surface, "Fiches Gagnants: %d", fGagant->valueint);
				y = getPrnY();
				setPrnY(y);
				printerprintf(ALG_LEFT, surface, "Commissions    : %.2f", commission->valuedouble);
				y = getPrnY();
                setPrnY(y);
				printerprintf(ALG_LEFT, surface, "Comm Sans Vendeurs : %.2f", withoutCommission->valuedouble);
				y = getPrnY();
				setPrnY(y);
				printerprintf(ALG_LEFT, surface, "Ventes         : %.2f", vente->valuedouble);
				y = getPrnY();
				setPrnY(y);
				printerprintf(ALG_LEFT, surface, "Pertes         : %.2f", perte->valuedouble);
				
				y = getPrnY() + 14;
				surface->DrawString(surface, "---------------------------------------------", -1, 0, y,  DSTF_LEFT);
				y+= 4;
			}
		
			while(1) 
			{
				lcdclean();
				lcd_header(ALG_CENTER, "RAPPORTS");
				lcdprintf(ALG_LEFT, "Start Date   : %s", start->valuestring);
				lcdprintf(ALG_LEFT, "End   Date   : %s", end->valuestring);
				lcdprintf(ALG_LEFT, "Num Fiches   : %d", quantite->valueint);
				lcdprintf(ALG_LEFT, "Ventes       : %.2f", vente->valuedouble);
				lcdprintf(ALG_LEFT, "Commissions  : %.2f", commission->valuedouble);
				height = lcdGetFontHeight();
				lcdSetFont(FONT_ROBOTO, "UTF-8", 0,  15, 0);
				lcdprintfex(ALG_LEFT, screen_height - height , "CANCEL=Retour, FUNC=Imprimer");
				lcdSetFont(FONT_ROBOTO, "UTF-8", 0,  15, 0);

				lcdFlip();

				key = kbGetKey();
				if (key == KEY_CANCEL)
					break;
				else if (key == KEY_FN)
				{
					if (0 == retval)
					{		
						lcdclean();
						lcdprintf(ALG_LEFT, "Printing....");
						lcdFlip();
						print_surface(ifd, surface, y + 100);
						do
						{
							usleep(100000);
							printer_get_status(ifd, &status);
							if (((status.status  >> PRINTER_STATUS_BUSY) & 0x01) == 0x01)
								lcdprintf(ALG_LEFT, "printer status = busy");
							else if (((status.status  >> PRINTER_STATUS_HIGHT_TEMP) & 0x01) == 0x01)
								lcdprintf(ALG_LEFT, "printer status = tempreture high");
							else if (((status.status  >> PRINTER_STATUS_PAPER_LACK) & 0x01) == 0x01)
								lcdprintf(ALG_LEFT, "printer status = no paper");
							else if (((status.status  >> PRINTER_STATUS_FEED) & 0x01) == 0x01)
								lcdprintf(ALG_LEFT, "printer status = feed paper");
							else if (((status.status  >> PRINTER_STATUS_PRINT) & 0x01) == 0x01)
								lcdprintf(ALG_LEFT, "printer status = printing");
							else if (((status.status  >> PRINTER_STATUS_FORCE_FEED) & 0x01) == 0x01)
									lcdprintf(ALG_LEFT, "printer status = force feed paper");
							else if (((status.status  >> PRINTER_STATUS_POWER_ON) & 0x01) == 0x01)
								lcdprintf(ALG_LEFT, "printer status = power on");
							lcdFlip();

						} while (status.status != 0);
						
						lcdprintf(ALG_LEFT, "Finished.");
						lcdFlip();
					}	
				}
			}

		}
	}

	if (userData != NULL) {
		free(userData);
        userData = NULL;
    }

	if (NULL != surface) {
		surface->Release(surface);
        surface = NULL;

    }

	if (NULL != font_32) {
		font_32->Release(font_32);
        font_32 = NULL;

    }

	if (NULL != font_24) {
		font_24->Release(font_24);
        font_24 = NULL;
    }

    if (NULL != font_48) {
		font_48->Release(font_48);
        font_48 = NULL;
    }

	if (ifd >= 0)
	{
		printer_close(ifd);
		ifd = -1;
	}

	userData = NULL;
	return ifd;
}



int reprint_fiche(const char *buffer)
{
	cJSON *json = NULL;
	cJSON *ficheId = NULL;
	cJSON *ficheTirage = NULL;
	cJSON *total = NULL;
	cJSON *bouleList = NULL;
	cJSON *ficheDate = NULL;
	cJSON *agentId = NULL;

	cJSON *json2 = NULL;
	cJSON *banknameIn = NULL;

	cJSON *vendornameIn = NULL;

	cJSON *company_desc = NULL;
	cJSON *company_name = NULL;

	cJSON *first_ticket = NULL;
	cJSON *agentaddressIn = NULL;
	cJSON *companyphoneIn = NULL;
	cJSON *companyPhone = NULL;

	int ifd = -1;
	int retval = 0;

	printer_param_t  param;
	printer_status_t status;
	IDirectFB *dfb = NULL;
	DFBSurfaceDescription	surfdesc;

	IDirectFBSurface        *surface = NULL;
	IDirectFBFont		    *font_24 = NULL;
	IDirectFBFont		    *font_32 = NULL;
	IDirectFBFont		    *font_48 = NULL;
	DFBFontDescription	    fdesc;

	char *userData = NULL;
	char imei[15], str[12];

	int x, y, height, width = 0;
	dfb = dfb_get_directfb();
	
	memset(imei, 0x00, sizeof(imei));
	memset(&fdesc, 0, sizeof(fdesc));

	sysReadSN(imei);

	fdesc.flags  = DFDESC_HEIGHT;
	fdesc.height = 32;

	dfb->CreateFont(dfb, ARIAL_FONT, &fdesc, &font_32);
	
    if (NULL == font_32){
		lcdprintf(ALG_LEFT, "craete font32 failed.");
		lcdFlip();
		retval = -1;
	}

	if (0 == retval){
		fdesc.height = 24;	
		dfb->CreateFont(dfb, ARIAL_FONT, &fdesc, &font_24);
		if (NULL == font_24){
			lcdprintf(ALG_LEFT, "craete font24 failed");
			lcdFlip();
			retval = -1;	
		}
	}

	if (0 == retval){
		fdesc.height = 42;	
		dfb->CreateFont(dfb, ARIAL_FONT, &fdesc, &font_48);
		if (NULL == font_48){
			lcdprintf(ALG_LEFT, "craete font48 failed");
			lcdFlip();
			retval = -1;	
		}
	}

	json = cJSON_Parse(buffer);

	if (json == NULL)
	{
		lcdprintf(ALG_LEFT, "Error while parsing..");
		lcdFlip();
		kbGetKey();
	} else 
	{
		if (0 == retval){
			ifd = printer_open(printer_device_name, O_WRONLY | O_NONBLOCK);
			if (ifd < 0){
				lcdprintf(ALG_LEFT, "Device Open failed");
				lcdprintf(ALG_LEFT, "Module Not Support");
				lcdFlip();
				kbGetKey();
				return 1;
			}
		}

		if (0 == retval)
		{
			printer_get_param(ifd, &param);
			surfdesc.flags = DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT;
			surfdesc.caps  = DSCAPS_NONE;
			surfdesc.width = param.ro_width;
			surfdesc.height= 6000;
			dfb->CreateSurface(dfb, &surfdesc, &surface);
			surface->Clear(surface, 
				0xFF,
				0xFF,
				0xFF,
				0xFF
			);
			
			surface->SetColor(surface, 0x00, 0x00, 0x00, 0xFF);

			width = surfdesc.width;
			x = param.ro_width/2;
			y = 20;	
			read_from_file(USERNAME_FILE, &userData);
			

            total = cJSON_GetObjectItemCaseSensitive(json, "montant");
			ficheDate = cJSON_GetObjectItemCaseSensitive(json, "created_on");

			surface->SetFont(surface, font_48);
			font_48->GetHeight(font_48, &height);	

			json2 = cJSON_Parse(userData);
			

			banknameIn = cJSON_GetObjectItemCaseSensitive(json2, "banknameIn");
			vendornameIn = cJSON_GetObjectItemCaseSensitive(json2, "vendornameIn");

			agentId = cJSON_GetObjectItemCaseSensitive(json2, "username");
			company_name = cJSON_GetObjectItemCaseSensitive(json2, "bank_name");
			company_desc = cJSON_GetObjectItemCaseSensitive(json2, "bank_description");
			agentaddressIn = cJSON_GetObjectItemCaseSensitive(json2, "vendoraddressIn");
			companyphoneIn = cJSON_GetObjectItemCaseSensitive(json2, "bankphoneIn");

			cJSON *addresse_complet = cJSON_GetObjectItemCaseSensitive(json2, "address");
			companyPhone = cJSON_GetObjectItemCaseSensitive(json2, "phone");
			
			// Set Logo
			drawpicture(x - (176 / 2), y, 176, 144, "/home/user0/mgnco/logo_48.png", surface);
			y += 147;
			surface->DrawString(surface, company_name->valuestring, -1, x, y, DSTF_TOPCENTER);
			y+= height + 3;

			surface->SetFont(surface, font_24);
			font_24->GetHeight(font_24, &height);
			setPrnY(y);
			printerprintf(ALG_CENTER, surface, "POS: %s", imei);
			y = getPrnY();
			setPrnY(y);
			printerprintf(ALG_CENTER, surface, "%s", ficheDate->valuestring);
			y = getPrnY();

			// Display new field
			if (cJSON_IsBool(companyphoneIn) && cJSON_IsTrue(companyphoneIn))
			{
				setPrnY(y);
				printerprintf(ALG_CENTER, surface, "Tel: %s", companyPhone->valuestring);
				y = getPrnY();
			}
			setPrnY(y);
			printerprintf(ALG_CENTER, surface, "Agent: %s", agentId->valuestring);
			y = getPrnY();

			if (cJSON_IsBool(agentaddressIn) && cJSON_IsTrue(agentaddressIn))
			{
				setPrnY(y);
				printerprintf(ALG_CENTER, surface, "Address: %s", addresse_complet->valuestring);
				y = getPrnY();
			}

			{
				cJSON *tn = cJSON_GetObjectItemCaseSensitive(json, "tirage_name");
				cJSON *tid = cJSON_GetObjectItemCaseSensitive(json, "ref_code");
				setPrnY(y);
				printerprintf(ALG_CENTER, surface, "Tirage: %s", tn->valuestring);
				y = getPrnY();	
				setPrnY(y);
				printerprintf(ALG_CENTER, surface, "#Ticket: %s", tid->valuestring);
				y = getPrnY();
			}
		
			setPrnY(y);
			printerprintf(ALG_CENTER, surface, "*****Fiche Copie*****");
			y = getPrnY();

			setPrnY(y);
			printerprintf(ALG_CENTER, surface, "-----------------------------------------------");
			y = getPrnY() + 12;

			surface->DrawString(surface, "Jeu", -1, 3, y,  DSTF_LEFT);
			surface->DrawString(surface, "Boule", -1, width*0.3, y,  DSTF_CENTER);
			surface->DrawString(surface, "Option", -1, width*0.6, y,  DSTF_CENTER);
			surface->DrawString(surface, "Mise", -1, width*0.9 - 5, y,  DSTF_RIGHT);
			y+= height + 3;

			bouleList = cJSON_GetObjectItemCaseSensitive(json, "boules");

			if (cJSON_IsArray(bouleList))
			{
				cJSON *element;
				// Iterate over the array elements
				cJSON_ArrayForEach(element, bouleList) 
				{
					cJSON *boule = cJSON_GetObjectItemCaseSensitive(element, "boule");
					cJSON *lotto = cJSON_GetObjectItemCaseSensitive(element, "lotto");
					cJSON *montant = cJSON_GetObjectItemCaseSensitive(element, "montant");
					cJSON *option = cJSON_GetObjectItemCaseSensitive(element, "option");
					surface->DrawString(surface, lotto->valuestring, -1, 3, y,  DSTF_LEFT);
					surface->DrawString(surface, boule->valuestring, -1, width*0.3, y,  DSTF_CENTER);
					surface->DrawString(surface, option->valuestring, -1, width*0.6, y,  DSTF_CENTER);
					sprintf(str, "%.2f", montant->valuedouble);
					surface->DrawString(surface,  str, -1, width*0.9 - 5, y,  DSTF_CENTER);
					y+= height + 10;
				}
			}
		

			surface->SetFont(surface, font_32);
			font_32->GetHeight(font_32, &height);	
			surface->DrawString(surface, "TOTAL: ", -1, 3, y,  DSTF_LEFT);
			sprintf(str, "%.2f HTG", total->valuedouble);
			surface->DrawString(surface, str, -1, width - 12, y,  DSTF_RIGHT);
			surface->SetFont(surface, font_24);
			font_24->GetHeight(font_24, &height);		
			y+= height + 1;
			surface->DrawString(surface, "---------------------------------------------", -1, 0, y,  DSTF_LEFT);
			y+= height + 1;
			setPrnY(y);
			printerprintf(ALG_LEFT, surface, "%s", company_desc->valuestring);
			y = getPrnY();
		}

		if (0 == retval)
		{		
			lcdclean();
			lcdprintf(ALG_LEFT, "Printing....");
			lcdFlip();
			print_surface(ifd, surface, y + 100);
			do
			{
				usleep(100000);
				printer_get_status(ifd, &status);
				if (((status.status  >> PRINTER_STATUS_BUSY) & 0x01) == 0x01)
					lcdprintf(ALG_LEFT, "printer status = busy");
				else if (((status.status  >> PRINTER_STATUS_HIGHT_TEMP) & 0x01) == 0x01)
					lcdprintf(ALG_LEFT, "printer status = tempreture high");
				else if (((status.status  >> PRINTER_STATUS_PAPER_LACK) & 0x01) == 0x01)
					lcdprintf(ALG_LEFT, "printer status = no paper");
				else if (((status.status  >> PRINTER_STATUS_FEED) & 0x01) == 0x01)
					lcdprintf(ALG_LEFT, "printer status = feed paper");
				else if (((status.status  >> PRINTER_STATUS_PRINT) & 0x01) == 0x01)
					lcdprintf(ALG_LEFT, "printer status = printing");
				else if (((status.status  >> PRINTER_STATUS_FORCE_FEED) & 0x01) == 0x01)
						lcdprintf(ALG_LEFT, "printer status = force feed paper");
				else if (((status.status  >> PRINTER_STATUS_POWER_ON) & 0x01) == 0x01)
					lcdprintf(ALG_LEFT, "printer status = power on");
				lcdFlip();

			} while (status.status != 0);
			
			lcdprintf(ALG_LEFT, "Finished.");
			lcdFlip();
		}
		
        if (json != NULL) {
		    cJSON_Delete(json);
            json = NULL;
        }

		 if (json2 != NULL) {
		    cJSON_Delete(json2);
            json2 = NULL;
        }

	}

	if (userData != NULL) {
		free(userData);
        userData = NULL;
    }

	if (NULL != surface) {
		surface->Release(surface);
        surface = NULL;

    }

	if (NULL != font_32) {
		font_32->Release(font_32);
        font_32 = NULL;
    }

	if (NULL != font_24) {
		font_24->Release(font_24);
        font_24 = NULL;
    }

    if (NULL != font_48) {
		font_48->Release(font_48);
        font_48 = NULL;
    }

	if (ifd >= 0)
	{
		printer_close(ifd);
		ifd = -1;
	}
	return ifd;
}
