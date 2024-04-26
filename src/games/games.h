/*
 * games.h
 *
 *  Created on: Apr 22, 2024
 *      Author: James DESTINE
 */

#ifndef GAMES_H_
#define GAMES_H_
#include "../cJSON.h"
#include "../helpers.h"
#include "../gui.h"
#include "../print.h"



void getConfiguration(void);
int getTirageType(Tirage **selectedTirage, int *sizeTirage, char *name, char *idName);
int make_post_fiches(const List *list, const char *id_tirage, Tirage *tirages, int sizeTirage);

void postFiches(const char *buffBoules);
void getLotsGagnants(void);
void getLotsByDate(void);
void getReports(void);
void getTicketsWon(void);
#endif /* GAMES_H_ */
