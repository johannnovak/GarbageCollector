/*	fichier :	moniteur.h
 * 18/06/2014
 * Florian CAULET 			Johann NOVAK
 *	florian.caulet@utbm.fr	johann.novak@utbm.fr
 *	Fichier qui regroupe tous les prototypes des fonctions du fichier moniteur.c .
 */

#ifndef	MONITEUR_H
#define	MONITEUR_H

#include "ipc.h"

/*	Prototypes des procédures et des fonctions de moniteur.c	*/

/*	Procédure creation_moniteurs_et_mutex_globaux
 *	Cette procédure initialise tous les moniteurs et mutex déclarés
 *	dans ipc.h.
 */
void creation_moniteurs_et_mutex_globaux();

/*	Procédure destruction_moniteurs_et_mutex_globaux
 * Cette procédure détruit tous les moniteurs et mutex définis dans ipc.h .
 */
void destruction_moniteurs_et_mutex_globaux();

#endif	//	MONITEUR_H
