/*	fichier : utilisateur.c
 * 18/06/2014
 * Florian CAULET 			Johann NOVAK
 *	florian.caulet@utbm.fr	johann.novak@utbm.fr
 */

#include "utilisateur.h"

void creation_utilisateurs(Utilisateur* u, Poubelle* pn, Poubelle* pc, Poubelle* pp, Poubelle* pv)
{
	fprintf(stdout, "\nCreation of the users.");
	int i;
	srand(time(NULL));

	for(i = 0; i < *NB_UTILISATEUR; ++i)
	{
		u[i].id = *ID;
		++(*ID);
		u[i].nbFamille = rand()%(UTILISATEUR_MAX_MEMBRE_FAMILLE)+1;
		u[i].cle = rand()%2;
		u[i].bac = (u[i].cle)?rand()%2:1;
		u[i].msgPoubelle = (MsgBuffer*)malloc(sizeof(MsgBuffer));
		u[i].msgPoubelle->arg = (void**)malloc(2*sizeof(int));
		if(u[i].cle)	
		{
			u[i].psac = (Poubelle*)malloc(sizeof(Poubelle));
			u[i].psac->id = -1;
			u[i].psac->capaCourante = 0;
			u[i].psac->capaMax = SAC_POUBELLE_CAPA_MAX;
			u[i].psac->type = NORMAL;
			u[i].conteneur_normal = &pn[rand()%(*NB_PBLN)];
		}
		else
		{
			u[i].psac = NULL;
			u[i].conteneur_normal = NULL;
		}

		if(u[i].bac)
			++(*NB_BAC);
		else
			u[i].pbac = NULL;
		u[i].conteneur_carton = &pc[rand()%(*NB_PBLC)];
		u[i].conteneur_plastique = &pp[rand()%(*NB_PBLP)];
		u[i].conteneur_verre = &pv[rand()%(*NB_PBLV)];
	}
}

void *vie_utilisateur(void* utilisateur)
{
	Utilisateur* u = (Utilisateur*)utilisateur;

	int compteurRecyclable = 0;
	
	pthread_mutex_lock(&mutex_stop);
	while(!stop)
	{
		pthread_mutex_unlock(&mutex_stop);
		
		sleep(UTILISATEUR_TEMPS_CONSOMMATION);
		if(u->bac)			/*	S'il a un bac, il consomme avec	*/
			consommer_avec_bac(u);
		else if(u->cle)	/*	Sinon il consomme avec son sac et son conteneur public	*/
			consommer_avec_cle(u);
		if(compteurRecyclable == 7)
		{	
			va_jeter_poubelle(u, u->conteneur_carton, u->nbFamille);
			va_jeter_poubelle(u, u->conteneur_plastique, u->nbFamille);
			va_jeter_poubelle(u, u->conteneur_verre, u->nbFamille);
			compteurRecyclable = 0;
		}

		++compteurRecyclable;
		pthread_mutex_lock(&mutex_stop);
	}
	pthread_mutex_unlock(&mutex_stop);
}

void consommer_avec_bac(Utilisateur* u)
{
	/*	L'utilisateur peut remplir son bac	*/
	if(!va_jeter_poubelle(u, u->pbac, u->nbFamille))
		if(u->cle)			/*	Si son bac est plein et s'il a une clé, alors il consomme avec sa clé	*/
			consommer_avec_cle(u);
}

void consommer_avec_cle(Utilisateur* u)
{
	/*	Si on ajoute déchets à sac et que < à capamax	*/
	if(u->psac->capaCourante+u->nbFamille <= u->psac->capaMax) 	
	{
		/*	On ajoute à capaCourante du sac	*/
		u->psac->capaCourante += u->nbFamille;

		/*	Si pile poil capa Max du sac	*/
		if(u->psac->capaCourante == SAC_POUBELLE_CAPA_MAX)
		{
			va_jeter_poubelle(u, u->conteneur_normal, u->psac->capaCourante);
			u->psac->capaCourante = 0;
		}
	}
	else		/*	si les déchets > capaMax du sac, l'utilisateur va jeter sa poubelle à son conteneur	*/
	{
		va_jeter_poubelle(u, u->conteneur_normal, u->psac->capaCourante);
		u->psac->capaCourante = u->nbFamille;
	}
}

bool va_jeter_poubelle(Utilisateur* u, Poubelle* p, int LDechets)
{
	/*	Tant que la poubelle est occupée	*/
	pthread_mutex_lock(&p->mutex_poubelle);			
	while(p->occupee)	
	{
		pthread_mutex_unlock(&p->mutex_poubelle);
	
		/*	on attend que la poubelle ne soit plus occupée	*/
		pthread_mutex_lock(&p->mutex_poubelle);
		fprintf(stdout, "\n%d) collective container %d occupied, waiting...",u->id, p->id);
		pthread_cond_wait(&p->prochainUtilisateur, &p->mutex_poubelle);
		pthread_mutex_unlock(&p->mutex_poubelle);

		pthread_mutex_lock(&p->mutex_poubelle);
	}
	pthread_mutex_unlock(&p->mutex_poubelle);
	
	/*	L'utilisateur occupe la poubelle	*/
	pthread_mutex_lock(&p->mutex_poubelle);
	p->occupee = true;
	pthread_mutex_unlock(&p->mutex_poubelle);

	/*	On envoie l'id et le litre de déchets de l'utilisateur à la poubelle	*/
	creation_message2(u->msgPoubelle, p->id, &u->id, &LDechets);
	if(msgsnd(*ipcID, u->msgPoubelle, 2*sizeof(int*), 0) != -1)
	{
		pthread_mutex_lock(&p->mutex_poubelle);
		fprintf(stdout, "\n%d) Occupying %s %d :\t%d => %d/%d", u->id, poubelle_label(p), p->id, LDechets, p->capaCourante, p->capaMax);
		pthread_mutex_unlock(&p->mutex_poubelle);
		
		/*	On teste si elle est pleine ou si elle va le devenir 	*/
		pthread_mutex_lock(&p->mutex_poubelle);
		if(p->capaCourante + LDechets > p->capaMax || p->pleine)		/*	POUBELLE PLEINE	*/
		{	
			pthread_mutex_unlock(&p->mutex_poubelle);
		
			pthread_mutex_lock(&p->mutex_poubelle);
			fprintf(stdout, "\n%d) leaving because %s %d full", u->id, poubelle_label(p),p->id);
			pthread_mutex_unlock(&p->mutex_poubelle);

			/*	L'utilisateur part et la poubelle est de nouveau libre	*/
			pthread_mutex_lock(&p->mutex_poubelle);
			p->occupee = false;
			//fprintf(stdout, "\n%s %d libéré\n", poubelle_label(p), p->id);
			pthread_mutex_unlock(&p->mutex_poubelle);
			
			/*	On signale au prochain utilisateur qu'il peut jeter ses déchets dans la poubelle	*/
			pthread_mutex_lock(&p->mutex_poubelle);
			pthread_cond_signal(&p->prochainUtilisateur);
			pthread_mutex_unlock(&p->mutex_poubelle);

			return false;
		}
		pthread_mutex_unlock(&p->mutex_poubelle);

		/*	On attend d'avoir la permission de la poubelle	*/
		pthread_mutex_lock(&p->mutex_poubelle);
		pthread_cond_wait(&p->remplirPoubelle, &p->mutex_poubelle);
		pthread_mutex_unlock(&p->mutex_poubelle);
		
		/*	On ajoute le nombre de L du sac dans la poubelle	*/
		pthread_mutex_lock(&p->mutex_poubelle);
		p->capaCourante += LDechets;
		pthread_mutex_unlock(&p->mutex_poubelle);

		/*	la poubelle est de nouveau libre	*/
		pthread_mutex_lock(&p->mutex_poubelle);
		p->occupee = false;
		//fprintf(stdout, "\n%s %d libéré\n", poubelle_label(p), p->id);
		pthread_mutex_unlock(&p->mutex_poubelle);
	}
	return true;
}

void destruction_utilisateurs(Utilisateur* u)
{
	fprintf(stdout, "\nDestruction of users");
	int i;
	if(u != NULL)
	{
		for(i = 0; i < *NB_UTILISATEUR; ++i)
		{
			destruction_poubelle(u[i].psac);
			if(u[i].psac != NULL)
			{
				free(u[i].psac);
				u[i].psac = NULL;
			}
			if(u[i].pbac != NULL)
				u[i].pbac = NULL;
			if(u[i].conteneur_normal != NULL)
				u[i].conteneur_normal = NULL;
			u[i].conteneur_carton = NULL;
			u[i].conteneur_plastique = NULL;
			u[i].conteneur_verre = NULL;
		}
	}
}
