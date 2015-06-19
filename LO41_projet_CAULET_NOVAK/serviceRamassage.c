/*	fichier : serviceRamassage.c
 * 18/06/2014
 * Florian CAULET 			Johann NOVAK
 *	florian.caulet@utbm.fr	johann.novak@utbm.fr
 */

#include "serviceRamassage.h"

void creation_service_ramassage(ServiceRamassage* sr)
{
	fprintf(stdout, "\nCréation of the trash-collecter service.");
	int i;

	/*	On crée les poubelles	*/
	sr->conteneursNormaux = (Poubelle*)malloc(sizeof(Poubelle)*(*NB_PBLN));
	creation_poubelles(sr->conteneursNormaux, *NB_PBLN, CONTENEUR_NORMAL_CAPA_MAX, NORMAL);
	sr->conteneursCarton = (Poubelle*)malloc(sizeof(Poubelle)*(*NB_PBLC));
	creation_poubelles(sr->conteneursCarton, *NB_PBLC, CONTENEUR_RECYCLABLE_CAPA_MAX, CARTON);
	sr->conteneursPlastique = (Poubelle*)malloc(sizeof(Poubelle)*(*NB_PBLP));
	creation_poubelles(sr->conteneursPlastique, *NB_PBLP, CONTENEUR_RECYCLABLE_CAPA_MAX, PLASTIQUE);
	sr->conteneursVerre = (Poubelle*)malloc(sizeof(Poubelle)*(*NB_PBLV));
	creation_poubelles(sr->conteneursVerre, *NB_PBLV, CONTENEUR_RECYCLABLE_CAPA_MAX, VERRE);

	/*	On crée les camions	*/
	sr->camionsNormaux = (Camion*)malloc(sizeof(Camion)*(*NB_CPBLN));
	creation_camions_poubelles(sr->camionsNormaux, *NB_CPBLN);
	sr->camionsCarton = (Camion*)malloc(sizeof(Camion)*(*NB_CPBLC));
	creation_camions_poubelles(sr->camionsCarton, *NB_CPBLC);
	sr->camionsPlastique = (Camion*)malloc(sizeof(Camion)*(*NB_CPBLP));
	creation_camions_poubelles(sr->camionsPlastique, *NB_CPBLP);
	sr->camionsVerre = (Camion*)malloc(sizeof(Camion)*(*NB_CPBLV));
	creation_camions_poubelles(sr->camionsVerre, *NB_CPBLV);
}


void* vie_service_ramassage(void* serviceRamassage)
{
	ServiceRamassage* sr	= (ServiceRamassage*)serviceRamassage;
	MsgBuffer* adressePoubelle = (MsgBuffer*)malloc(sizeof(Poubelle)+sizeof(long));
	adressePoubelle->arg = (void**)malloc(sizeof(Poubelle));

	pthread_mutex_t mumu;
	int i;
	bool envoiCamion;

	pthread_mutex_lock(&mutex_stop);
	while(!stop)
	{
		pthread_mutex_unlock(&mutex_stop);

		/*	On attend un message d'une poubelle pleine qui contient son adresse	*/
		if(msgrcv(*ipcSignaux, adressePoubelle, sizeof(Poubelle*), ID_SIGNAL_POUBELLE_PLEINE, 0) != -1)
		{
			/*	On cherche un camion poubelle dispo et on lui envoie un
			 * message contenant l'adresse de la poubelle	*/
			envoiCamion = envoyer_camion_poubelle(chercher_camion_poubelle(sr, ((Poubelle*)adressePoubelle->arg[0])->type), adressePoubelle);
			
			if(envoiCamion)	/*	Si la file de message existe toujours	*/
			{
				/*	On print les camions en déplacement	*/
				pthread_mutex_lock(&mumu);
				fprintf(stdout, "\n\nT-C S : Received msg from full %s : %d",poubelle_label(((Poubelle*)adressePoubelle->arg[0])),((Poubelle*)adressePoubelle->arg[0])->id);
				fprintf(stdout, "\nNormal garbage trucks moving : \n");
				for(i = 0; i < *NB_CPBLN; ++i)
				{
					if(sr->camionsNormaux[i].poubelle != NULL)
						fprintf(stdout, "\t%d",sr->camionsNormaux[i].poubelle->id);
					else
						fprintf(stdout, "\t/");
				}
				fprintf(stdout, "\nPaper garbage trucks moving : \n");
				for(i = 0; i < *NB_CPBLC; ++i)
				{
					if(sr->camionsCarton[i].poubelle != NULL)
						fprintf(stdout, "\t%d",sr->camionsCarton[i].poubelle->id);
					else
						fprintf(stdout, "\t/");
				}
				fprintf(stdout, "\nPlastic garbage trucks moving : \n");
				for(i = 0; i < *NB_CPBLP; ++i)
				{
					if(sr->camionsPlastique[i].poubelle != NULL)
						fprintf(stdout, "\t%d",sr->camionsPlastique[i].poubelle->id);
					else
						fprintf(stdout, "\t/");
				}
				fprintf(stdout, "\nGlass garbage trucks moving : \n");
				for(i = 0; i < *NB_CPBLV; ++i)
				{
					if(sr->camionsVerre[i].poubelle != NULL)
						fprintf(stdout, "\t%d",sr->camionsVerre[i].poubelle->id);
					else
						fprintf(stdout, "\t/");
				}
				pthread_mutex_unlock(&mumu);
			}
		}
		pthread_mutex_lock(&mutex_stop);
	}
	pthread_mutex_unlock(&mutex_stop);
}

Camion chercher_camion_poubelle(ServiceRamassage* sr, TypePoubelle type)
{	
	int i = 0, temp, size;
	bool cont = true;
	Camion* ca;
	Camion c;
	switch(type)
	{
		case NORMAL:
			size = *NB_CPBLN;
			ca = sr->camionsNormaux;
			break;
		case CARTON:
			size = *NB_CPBLC;
			ca = sr->camionsCarton;
			break;
		case PLASTIQUE:
			size = *NB_CPBLP;
			ca = sr->camionsPlastique;
			break;
		case VERRE:
			size = *NB_CPBLV;
			ca = sr->camionsVerre;
			break;
	}

	while(i < size && cont)	/*	On continue tant que l'on a pas trouvé de camion	*/
	{
		pthread_mutex_lock(&ca[i].mutex_camion);
		temp = i;
		if(!ca[i].enDeplacement)		/*	S'il n'est pas en déplacement, on arrête la boucle	*/
		{	
			pthread_mutex_unlock(&ca[i].mutex_camion);
			cont = false;
			--i;
		}
		if(temp == i)
			pthread_mutex_unlock(&ca[temp].mutex_camion);
		++i;
		if(i == *NB_CPBLN)			/*	Si tous les camions sont en déplacement, on recommence depuis 0	*/
			i = 0;
	}
	c = ca[i];
	ca = NULL;
	return c;
}

bool envoyer_camion_poubelle(Camion c, MsgBuffer* m)
{
	/*	on envoie un message au camion avec l'adresse de la poubelle	*/
	fprintf(stdout,"\nt-C S: sending truck %d to %s %d",c.id,poubelle_label(((Poubelle*)m->arg[0])),((Poubelle*)m->arg[0])->id);
	m->type = c.id;
	if(msgsnd(*ipcID, m, sizeof(Poubelle*), 0) != -1)
		return true;
	return false;
}

void destruction_service_ramassage(ServiceRamassage* sr)
{
	fprintf(stdout, "\nDestruction of the trash-collecter service.");
	
	destruction_poubelles(sr->conteneursNormaux, *NB_PBLN);
	if(sr->conteneursNormaux != NULL)
	{
		free(sr->conteneursNormaux);
		sr->conteneursNormaux = NULL;
	}
			
	destruction_poubelles(sr->conteneursCarton, *NB_PBLC);
	if(sr->conteneursCarton != NULL)
	{
		free(sr->conteneursCarton);
		sr->conteneursCarton = NULL;
	}
	destruction_poubelles(sr->conteneursPlastique, *NB_PBLP);
	if(sr->conteneursPlastique != NULL)
	{
		free(sr->conteneursPlastique);
		sr->conteneursPlastique = NULL;
	}
	destruction_poubelles(sr->conteneursVerre, *NB_PBLV);
	if(sr->conteneursVerre != NULL)
	{
		free(sr->conteneursVerre);
		sr->conteneursVerre = NULL;
	}
	destruction_poubelles(sr->bacs, *NB_BAC);
	if(sr->bacs != NULL)
	{
		free(sr->bacs);
		sr->bacs = NULL;
	}

	destruction_camions(sr->camionsNormaux, *NB_CPBLN);
	if(sr->camionsNormaux != NULL)
	{
		free(sr->camionsNormaux);
		sr->camionsNormaux = NULL;
	}

	destruction_camions(sr->camionsCarton, *NB_CPBLC);
	if(sr->camionsCarton != NULL)
	{
		free(sr->camionsCarton);
		sr->camionsCarton = NULL;
	}

	destruction_camions(sr->camionsPlastique, *NB_CPBLP);
	if(sr->camionsPlastique != NULL)
	{
		free(sr->camionsPlastique);
		sr->camionsPlastique = NULL;
	}

	destruction_camions(sr->camionsVerre, *NB_CPBLV);
	if(sr->camionsVerre != NULL)
	{
		free(sr->camionsVerre);
		sr->camionsVerre= NULL;
	}

}
