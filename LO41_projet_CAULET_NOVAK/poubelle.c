/*	fichier : poubelle.c
 * 18/06/2014
 * Florian CAULET 			Johann NOVAK
 *	florian.caulet@utbm.fr	johann.novak@utbm.fr
 */

#include "poubelle.h"

void creation_poubelles(Poubelle* p, int size, int capaMax, TypePoubelle type)
{
	switch(type)
	{	
		case NORMAL:
			fprintf(stdout, "\nCreation of the normal trash collective containers");
			break;
		case CARTON:
			fprintf(stdout, "\nCreation of the paper trash collective containers");
			break;
		case PLASTIQUE:
			fprintf(stdout, "\nCreation of the plastic trash collective containers");
			break;
		case VERRE:
			fprintf(stdout, "\nCreation of the glass trash collective containers");
			break;
	}
	int i;
	for(i = 0; i < size; ++i)
	{
		p[i].id = *ID;
		++(*ID);
		p[i].capaMax = capaMax;
		p[i].capaCourante = 0;
		pthread_mutex_init(&p[i].mutex_poubelle,NULL);
		pthread_cond_init(&p[i].viderPoubelle, NULL);
		pthread_cond_init(&p[i].remplirPoubelle, NULL);
		pthread_cond_init(&p[i].prochainUtilisateur, NULL);
		p[i].occupee = false;
		p[i].pleine = false;
		p[i].type = type;
	}
}

void *vie_poubelle(void* pn)
{
	Poubelle* p = (Poubelle*)pn;

	MsgBuffer* msgSR = (MsgBuffer*)malloc(sizeof(long)+sizeof(Poubelle*));
	msgSR->arg = (void**)malloc(sizeof(Poubelle*));
	MsgBuffer* msgUtilisateur = (MsgBuffer*)malloc(sizeof(long)+2*sizeof(int));
	msgUtilisateur->arg = (void**)malloc(sizeof(int)*2);
	MsgBuffer* msgMairie = (MsgBuffer*)malloc(sizeof(long)+2*sizeof(int));
	msgMairie->arg = (void**)malloc(sizeof(int)*2);

	int* IDUtilisateur = (int*)malloc(sizeof(int));
	int* LDechet = (int*)malloc(sizeof(int));

	pthread_mutex_lock(&mutex_stop);
	while(!stop)
	{
		pthread_mutex_unlock(&mutex_stop);

		/*	La poubelle attend un message d'un utilisateur pour se réveiller	*/
		if(msgrcv(*ipcID, msgUtilisateur, 2*sizeof(int*), p->id, 0) != -1)
		{
			/*	On enregistre l'id et le litre de déchets reçus depuis le message	*/
			*IDUtilisateur = *(int*)msgUtilisateur->arg[0];
			*LDechet = *(int*)msgUtilisateur->arg[1];
			creation_message2(msgMairie, ID_SIGNAL_POUBELLE_MAIRIE, IDUtilisateur, LDechet);
			
			/*	On teste si on peut bien rajouter le nombre de déchets à la capaCourante	*/
			pthread_mutex_lock(&p->mutex_poubelle);
			if(p->capaCourante + *LDechet > p->capaMax)		/* POUBELLE PLEINE	*/
			{									
				pthread_mutex_unlock(&p->mutex_poubelle);
				
				/*	La poubelle est pleine	*/
				pthread_mutex_lock(&p->mutex_poubelle);
				p->pleine = true;
				fprintf(stdout, "\n%s %d plein", poubelle_label(p), p->id);
				pthread_mutex_unlock(&p->mutex_poubelle);

				/*	On signale à l'utilisateur suivant de la file d'attente que la poubelle est libre	*/
				pthread_mutex_lock(&p->mutex_poubelle);
				pthread_cond_signal(&p->prochainUtilisateur);
				pthread_mutex_unlock(&p->mutex_poubelle);
				
				/*	On envoie l'adresse de la poubelle au SR, en disant qu'elle est pleine	*/
				creation_message1(msgSR, ID_SIGNAL_POUBELLE_PLEINE, p);
				if(msgsnd(*ipcSignaux, msgSR, sizeof(Poubelle*), 0) != -1)
				{
					/*	On attend que le camion vienne */
					pthread_mutex_lock(&p->mutex_poubelle);
					pthread_cond_wait(&p->viderPoubelle, &p->mutex_poubelle);
					pthread_mutex_unlock(&p->mutex_poubelle);
				
					/*	On attends que le camion vide la poubelle	*/
					sleep(CAMION_TEMPS_DE_VIDE);
					pthread_mutex_lock(&p->mutex_poubelle);
					p->capaCourante = 0;
					pthread_mutex_unlock(&p->mutex_poubelle);
					
					/*	La poubelle envoie un message au camion pour lui dire qu'il peut repartir	*/
					msgSR->type = p->id;
					if(msgsnd(*ipcPoubelleCamion, msgSR, sizeof(Poubelle*), 0) != -1)
					{
						/*	La poubelle est de nouveau remplissable	*/
						pthread_mutex_lock(&p->mutex_poubelle);
						p->pleine = false;
						pthread_mutex_unlock(&p->mutex_poubelle);
						
						/*	La poubelle prévient l'utilisateur suivant */
						pthread_mutex_lock(&p->mutex_poubelle);
						pthread_cond_signal(&p->prochainUtilisateur);
						pthread_mutex_unlock(&p->mutex_poubelle);
					}
				}
			}
			else 		/*		POUBELLE NON PLEINE		*/
			{	
				pthread_mutex_unlock(&p->mutex_poubelle);
				
				/* Envoi de la facturation à la mairie */
				pthread_mutex_lock(&p->mutex_poubelle);
				if(p->type == NORMAL)		/*	POUBELLE NORMALE => FACTURE	*/
				{
					pthread_mutex_unlock(&p->mutex_poubelle);
					if(msgsnd(*ipcSignaux, msgMairie,2*sizeof(int*),0) != -1)
					{
						/*	Remplissage de la poubelle par l'utilisateur	*/
						sleep(UTILISATEUR_TEMPS_DE_JETTE);
						
						/*	La poubelle signale l'utilisateur qu'il peut la remplir	*/
						pthread_mutex_lock(&p->mutex_poubelle);
						pthread_cond_signal(&p->remplirPoubelle);
						pthread_mutex_unlock(&p->mutex_poubelle);

						/*	On signale au prochain utilisateur de venir jeter ses déchets	*/
						pthread_mutex_lock(&p->mutex_poubelle);
						pthread_cond_signal(&p->prochainUtilisateur);
						pthread_mutex_unlock(&p->mutex_poubelle);
					}
				}
				else			/*	POUBELLE RECYCLABLE => PAS DE FACTURE	*/
				{
					pthread_mutex_unlock(&p->mutex_poubelle);

					/*	Remplissage de la poubelle par l'utilisateur	*/
					sleep(UTILISATEUR_TEMPS_DE_JETTE);
					
					/*	La poubelle signale l'utilisateur qu'il peut la remplir	*/
					pthread_mutex_lock(&p->mutex_poubelle);
					pthread_cond_signal(&p->remplirPoubelle);
					pthread_mutex_unlock(&p->mutex_poubelle);

					/*	On signale au prochain utilisateur de venir jeter ses déchets	*/
					pthread_mutex_lock(&p->mutex_poubelle);
					pthread_cond_signal(&p->prochainUtilisateur);
					pthread_mutex_unlock(&p->mutex_poubelle);
				}
			}
		}
		pthread_mutex_lock(&mutex_stop);
	}
	pthread_mutex_unlock(&mutex_stop);

	if(msgSR != NULL)
	{
		free(msgSR);
		msgSR = NULL;
	}
	if(msgUtilisateur != NULL)
	{
		free(msgUtilisateur);
		msgUtilisateur = NULL;
	}
	if(msgMairie != NULL)
	{
		free(msgMairie);
		msgMairie = NULL;
	}
	free(IDUtilisateur);
	IDUtilisateur = NULL;
	free(LDechet);
	LDechet = NULL;
}

char* poubelle_label(Poubelle* p)
{
	switch(p->type)
	{
		case CARTON:
			return "paper collective container";
		case PLASTIQUE:
			return "plastic collective container";
		case VERRE:
			return "glass collective container";
		case NORMAL:
			if(p->capaMax == CONTENEUR_NORMAL_CAPA_MAX)
				return "normal collective container";
			else
				return "bac";
		default:
			return "";
	}
}

void destruction_poubelle(Poubelle* p)
{
	if(p != NULL)
	{
		pthread_mutex_destroy(&p->mutex_poubelle);
		pthread_cond_destroy(&p->viderPoubelle);
		pthread_cond_destroy(&p->remplirPoubelle);
		pthread_cond_destroy(&p->prochainUtilisateur);
	}
}

void destruction_poubelles(Poubelle* p, int size)
{	
	fprintf(stdout,"\nDestruction of the %ss",poubelle_label(p));
	int i;
	if(p != NULL)
	{
		for(i = 0; i < size; ++i)
			destruction_poubelle(&p[i]);
		free(p);
		p = NULL;
	}
}
