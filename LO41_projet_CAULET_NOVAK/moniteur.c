/*	fichier :	moniteur.c
 * 18/06/2014
 * Florian CAULET 			Johann NOVAK
 *	florian.caulet@utbm.fr	johann.novak@utbm.fr
 */

#include "moniteur.h"

void creation_moniteurs_et_mutex_globaux()
{
	fprintf(stdout, "\nCreation of monitors et global mutexes");
	int i;
	pthread_cond_init(&cond_signalPoubelleNormale, NULL);
	pthread_cond_init(&cond_fin, NULL);
	pthread_mutex_init(&mutex_stop, NULL);
}

void destruction_moniteurs_et_mutex_globaux()
{
	fprintf(stdout, "\nDestruction of monitors and global mutexes.");
	pthread_cond_destroy(&cond_fin);
	pthread_cond_destroy(&cond_signalPoubelleNormale);
	pthread_mutex_destroy(&mutex_stop);
}
