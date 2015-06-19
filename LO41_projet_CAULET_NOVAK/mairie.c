/*	fichier : mairie.c
 * 18/06/2014
 * Florian CAULET				Johann NOVAK
 *	florian.caulet@utbm.fr	johann.novak@utbm.fr
 */

#include "mairie.h"

void creation_mairie(Mairie* m)
{
	fprintf(stdout,"\nCreation of the city hall.");
	int i;
	
	/*	on crée le service de ramassage avec les conteneurs publics	*/
	m->serviceRamassage = (ServiceRamassage*)malloc(sizeof(ServiceRamassage));
	creation_service_ramassage(m->serviceRamassage);
	
	/*	On crée les utilisateurs	*/
	m->indexPremierUtilisateur=(int*)malloc(sizeof(int));
	*m->indexPremierUtilisateur = *ID;
	m->utilisateurs = (Utilisateur*)malloc(sizeof(Utilisateur)*(*NB_UTILISATEUR));
	creation_utilisateurs(m->utilisateurs, m->serviceRamassage->conteneursNormaux,
														m->serviceRamassage->conteneursCarton,
														m->serviceRamassage->conteneursPlastique,
														m->serviceRamassage->conteneursVerre);

	/*	Création des bacs et assignation aux utilisateurs 	*/
	creation_bacs(m);
	
	/*	On crée le compteur de déchets associé à chaque utilisateur	*/
	m->compteurDechets = (int*)malloc(sizeof(int)*(*NB_UTILISATEUR));
	for(i = 0; i < *NB_UTILISATEUR; ++i)
		m->compteurDechets[i] = 0;
}

void creation_bacs(Mairie* m)
{
	fprintf(stdout, "\nCreation of individual garbage container.");
	int i, j = 0;
	
	m->serviceRamassage->bacs = (Poubelle*)malloc(sizeof(Poubelle)*(*NB_BAC));
	for(i = 0; i < *NB_UTILISATEUR; ++i)
	{
		if(m->utilisateurs[i].bac)		/*	Si l'utilisateur a un bac	*/
		{
			m->utilisateurs[i].pbac = &m->serviceRamassage->bacs[j];
			/*	On définit les caractéristiques du bac	*/
			m->utilisateurs[i].pbac->id = *ID;
			++(*ID);
			m->utilisateurs[i].pbac->capaCourante = 0;
			if(m->utilisateurs[i].nbFamille == 1)
				m->utilisateurs[i].pbac->capaMax = 80;
			else if(m->utilisateurs[i].nbFamille == 2)
				m->utilisateurs[i].pbac->capaMax = 120;
			else if(m->utilisateurs[i].nbFamille <  5)
				m->utilisateurs[i].pbac->capaMax = 180;
			else if(m->utilisateurs[i].nbFamille >  4)
				m->utilisateurs[i].pbac->capaMax = 240;
			
			pthread_cond_init(&m->utilisateurs[i].pbac->viderPoubelle, NULL);
			pthread_cond_init(&m->utilisateurs[i].pbac->remplirPoubelle, NULL);
			pthread_cond_init(&m->utilisateurs[i].pbac->prochainUtilisateur, NULL);
			pthread_mutex_init(&m->utilisateurs[i].pbac->mutex_poubelle, NULL);
			m->utilisateurs[i].pbac->occupee = false;
			m->utilisateurs[i].pbac->pleine = false;
			m->utilisateurs[i].pbac->type = NORMAL;

			++j;
		}
	}
}

void* vie_mairie(void* mairie)
{
	MsgBuffer* msgPoubelle = NULL;
	Mairie* m = (Mairie*)mairie;
	int i=0;
	time_t debut, fin;

	pthread_mutex_lock(&mutex_stop);
	
	msgPoubelle = (MsgBuffer*)malloc(sizeof(long)+2*sizeof(int));
	msgPoubelle->arg =  (void**)malloc(sizeof(int)*2);

	time(&debut);
	while(!stop)
	{
		pthread_mutex_unlock(&mutex_stop);
		
		/*	On attend un message d'une poubelle qui informe du volume jetté par un utilisateur	*/
		if(msgrcv(*ipcSignaux, msgPoubelle, 2*sizeof(int*), ID_SIGNAL_POUBELLE_MAIRIE,0) != -1)
		{
			m->compteurDechets[(*(int*)msgPoubelle->arg[0])-(*m->indexPremierUtilisateur)] = *(int*)msgPoubelle->arg[1];
		
			time(&fin);
			if(difftime(fin,debut)>=10)
			{
				fprintf(stdout,"\nFin du mois");
				/* Fin du mois, on affiche la facture globale */
				for(i = 0; i < *NB_UTILISATEUR; ++i)
				{
					if(m->compteurDechets[i] != 0)
					{
						fprintf(stdout, "\nThe user %d has consumed %dL during the month.", *m->indexPremierUtilisateur+i,m->compteurDechets[i]);
						m->compteurDechets[i] = 0;
					}
				}
				time(&debut);
			}
		}
		pthread_mutex_lock(&mutex_stop);
	}
	pthread_mutex_unlock(&mutex_stop);

	if(msgPoubelle != NULL)
	{
		free(msgPoubelle);
		msgPoubelle = NULL;
	}
}

void destruction_mairie(Mairie* m)
{
	fprintf(stdout, "\nDestruction of the city hall.");
	if(m != NULL)
	{
		if(m->compteurDechets != NULL)
		{
			free(m->compteurDechets);
			m->compteurDechets = NULL;
		}
		destruction_service_ramassage(m->serviceRamassage);
		if(m->serviceRamassage != NULL)
		{
			free(m->serviceRamassage);
			m->serviceRamassage = NULL;
		}

		destruction_utilisateurs(m->utilisateurs);
		if(m->utilisateurs != NULL)
		{
			free(m->utilisateurs);
			m->utilisateurs = NULL;
		}
		if(m->indexPremierUtilisateur != NULL)
		{
			free(m->indexPremierUtilisateur);
			m->indexPremierUtilisateur = NULL;
		}

		free(m);
		m = NULL;
	}
}
