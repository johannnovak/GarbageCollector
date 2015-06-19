/*	fichier : camion.c
 * 18/06/2014
 * Florian CAULET				Johann NOVAK
 *	florian.caulet@utbm.fr	johann.novak@utbm.fr
 */

#include "camion.h"

void creation_camions_poubelles(Camion* c, int size)
{
	int i;
	if(size == *NB_CPBLN)
		fprintf(stdout, "\nCreation of the garbage trucks for normal trashes");
	else 
		fprintf(stdout, "\nCreation of the garbage trucks for trashes to recycle (paper / plastic / glass)");
	for(i = 0; i < size; ++i)
	{
		c[i].id = *ID;
		++(*ID);
		c[i].poubelle = NULL;
		c[i].enDeplacement = false;
		pthread_mutex_init(&c[i].mutex_camion,NULL);
	}
}

void*	vie_camion_poubelles(void* cp)
{
	Camion* c = (Camion*)cp;
	MsgBuffer* m = (MsgBuffer*)malloc(sizeof(Poubelle)+sizeof(long));
	m->arg = (void**)malloc(sizeof(int)*2);
	
	pthread_mutex_lock(&mutex_stop);
	while(!stop)
	{
		pthread_mutex_unlock(&mutex_stop);

		/*	On attend que le SR nous préviennent qu'une poubelle est pleine
			avec l'adresse de la poubelle dans le message	
		*/
		if(msgrcv(*ipcID, m, sizeof(Poubelle*), c->id, 0) != -1)
		{
			/*	Le camion se déplace vers la poubelle	*/
			pthread_mutex_lock(&c->mutex_camion);
			c->poubelle = ((Poubelle*)m->arg[0]);
			c->enDeplacement = true;
			pthread_mutex_unlock(&c->mutex_camion);

			fprintf(stdout, "\n%d) Truck going to garbage container %s %d",c->id, poubelle_label(c->poubelle), c->poubelle->id);
			
			/*	temps de déplacement	vers la poubelle	*/
			sleep(CAMION_TEMPS_DE_DEPLACEMENT);

			/*	On signale à la poubelle que le camion est là et qu'elle se fait vider	*/
			pthread_mutex_lock(&c->poubelle->mutex_poubelle);
			fprintf(stdout, "\nI'm arrived to %s %d!",poubelle_label(c->poubelle),((Poubelle*)m->arg[0])->id);
			pthread_cond_signal(&c->poubelle->viderPoubelle);
		 	fprintf(stdout, "\n%d) I'm emptying %s %d !",c->id, poubelle_label(c->poubelle), c->poubelle->id);
			pthread_mutex_unlock(&c->poubelle->mutex_poubelle);

			/*	Attente de la réponse de la poubelle pour repartir	*/
			if(msgrcv(*ipcPoubelleCamion, m, sizeof(Poubelle*), c->poubelle->id, 0) != -1)
			{
				/*	le camion peut repartir au service de ramassage */
				sleep(CAMION_TEMPS_DE_DEPLACEMENT);

				/*	Une fois arrivé, il n'a plus de poubelles à vider et n'est plus en déplacement	*/
				pthread_mutex_lock(&c->mutex_camion);
				c->enDeplacement = false;
				c->poubelle = NULL;
				fprintf(stdout, "\n%d) Back home !\n******************************\n",c->id);
				pthread_mutex_unlock(&c->mutex_camion);
			}
		}
		pthread_mutex_lock(&mutex_stop);
	}
	pthread_mutex_unlock(&mutex_stop);
}

void destruction_camions(Camion* c, int size)
{
	int i;
	if(size == *NB_CPBLC)
		fprintf(stdout, "\nDestruction of garbage trucks for paper trashes.");
	else if(size == *NB_CPBLP)
		fprintf(stdout, "\nDestruction of garbage trucks for plastic trashes.");
	else if(size == *NB_CPBLV)
		fprintf(stdout, "\nDestruction of garbage trucks for glass trashes.");
	else if(size == *NB_CPBLN)
		fprintf(stdout, "\nDestruction of garbage trucks for normal trash.");
	if(c != NULL)
	{
		for(i = 0; i < size; ++i)
			if(c[i].poubelle != NULL)
			{
				c[i].poubelle = NULL;
				pthread_mutex_destroy(&c[i].mutex_camion);
			}
	}
	if(size == *NB_CPBLC && NB_CPBLC)
	{
		free(NB_CPBLC);
		NB_CPBLC = NULL;
	}
	else if(size == *NB_CPBLP && NB_CPBLP)
	{
		free(NB_CPBLP);
		NB_CPBLP = NULL;
	}
	else if(size == *NB_CPBLV && NB_CPBLV)
	{
		free(NB_CPBLV);
		NB_CPBLV = NULL;
	}
	else if(size == *NB_CPBLN && NB_CPBLN)
	{
		free(NB_CPBLN);
		NB_CPBLN = NULL;
	}
}
