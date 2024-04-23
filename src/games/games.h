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



void getConfiguration(void);
int getTirageType(Tirage **selectedTirage, int *sizeTirage, char *name, int *idName);
void postFiches(const char *buffBoules);
#endif /* GAMES_H_ */
