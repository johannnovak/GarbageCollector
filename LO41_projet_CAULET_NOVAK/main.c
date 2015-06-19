/*	fichier : main.c
 * 18/06/2014
 * Florian CAULET				Johann NOVAK
 *	florian.caulet@utbm.fr	johann.novak@utbm.fr
 *	Fichier main où s'éxecute le programme.
 */

#include "mairie.h"

/* Procédure stop_simulation
 * Cette procédure est appelée lorsqu'elle recoit le signal Ctrl-Z. Elle sert à faire
 *	terminer correctement tout le programme.
 *	Paramètres	:	-	int num	:	numéro du signal qui a lancé cette fonction.
 */
void stop_simulation(int num);

int main(int argc, char** argv)
{
	time_t debut, fin;
	int i;
	time(&debut);
	if(argc == 10)
	{
		for(i = 1; i < argc; ++i)
			if(atoi(argv[i]) == 0)
			{
				fprintf(stdout,"Error ! Argument %d equal to 0, please insert a not null value.",i);
				return EXIT_FAILURE;
			}

		/*	On assigne les paramètres aux variables globales	*/
		NB_BAC = (int*)malloc(sizeof(int));
		NB_UTILISATEUR = (int*)malloc(sizeof(int));
		*NB_UTILISATEUR = atoi(argv[1]);
		NB_PBLN = (int*)malloc(sizeof(int));
		*NB_PBLN = atoi(argv[2]);
		NB_PBLC = (int*)malloc(sizeof(int));
		*NB_PBLC = atoi(argv[3]);
		NB_PBLP = (int*)malloc(sizeof(int));
		*NB_PBLP = atoi(argv[4]);
		NB_PBLV = (int*)malloc(sizeof(int));
		*NB_PBLV = atoi(argv[5]);
		NB_CPBLN = (int*)malloc(sizeof(int));
		*NB_CPBLN = atoi(argv[6]);
		NB_CPBLC = (int*)malloc(sizeof(int));
		*NB_CPBLC = atoi(argv[7]);
		NB_CPBLP = (int*)malloc(sizeof(int));
		*NB_CPBLP = atoi(argv[8]);
		NB_CPBLV = (int*)malloc(sizeof(int));
		*NB_CPBLV = atoi(argv[9]);
		
		ID = (int*)malloc(sizeof(int));
		*ID = 1;

		/*	Initialisation des variables	*/
		pthread_t 	thread_mairie;
		pthread_t	thread_service_ramassage;
		pthread_t* 	threads_camions_poubelles_normaux;
		pthread_t* 	threads_camions_poubelles_carton;
		pthread_t* 	threads_camions_poubelles_plastique;
		pthread_t* 	threads_camions_poubelles_verre;
		pthread_t* 	threads_conteneurs_normaux;
		pthread_t* 	threads_conteneurs_carton;
		pthread_t* 	threads_conteneurs_plastique;
		pthread_t* 	threads_conteneurs_verre;
		pthread_t* 	threads_bacs;
		pthread_t* 	threads_utilisateurs;
		pthread_attr_t attribut;
		
		Mairie* mairie;
		
		/*	On crée la mairie	*/
		mairie = (Mairie*)malloc(sizeof(Mairie));
		creation_mairie(mairie);

		/*	Set des variables	*/
		threads_camions_poubelles_normaux = (pthread_t*)malloc(sizeof(pthread_t)*(*NB_CPBLN));
		threads_camions_poubelles_carton = (pthread_t*)malloc(sizeof(pthread_t)*(*NB_CPBLC));
		threads_camions_poubelles_plastique = (pthread_t*)malloc(sizeof(pthread_t)*(*NB_CPBLP));
		threads_camions_poubelles_verre = (pthread_t*)malloc(sizeof(pthread_t)*(*NB_CPBLV));
		threads_conteneurs_normaux = (pthread_t*)malloc(sizeof(pthread_t)*(*NB_PBLN));
		threads_conteneurs_carton = (pthread_t*)malloc(sizeof(pthread_t)*(*NB_PBLC));
		threads_conteneurs_plastique = (pthread_t*)malloc(sizeof(pthread_t)*(*NB_PBLP));
		threads_conteneurs_verre = (pthread_t*)malloc(sizeof(pthread_t)*(*NB_PBLV));
		threads_bacs = (pthread_t*)malloc(sizeof(pthread_t)*(*NB_BAC));
		threads_utilisateurs = (pthread_t*)malloc(sizeof(pthread_t)*(*NB_UTILISATEUR));

		pthread_attr_init(&attribut);
		pthread_attr_setdetachstate(&attribut, PTHREAD_CREATE_DETACHED);
	
		srand(time(NULL));

		/*	On crée les moniteurs */
		creation_moniteurs_et_mutex_globaux();

		/*	On crée l'ipc	*/
		creation_ipc(argv[0]);

		/* Print des infos	*/
		fprintf(stdout, "\n");
		for(i = 0; i < *NB_UTILISATEUR; ++i)
		{
			if(mairie->utilisateurs[i].cle && mairie->utilisateurs[i].bac)
				fprintf(stdout, "\nUser %d composed of %d individuals\t=>  garbage container %d and individual trash container %d",mairie->utilisateurs[i].id, mairie->utilisateurs[i].nbFamille,mairie->utilisateurs[i].conteneur_normal->id,mairie->utilisateurs[i].pbac->id);
			else if(mairie->utilisateurs[i].cle)
				fprintf(stdout, "\nUser %d composed of %d individuals\t=> garbage container %d",mairie->utilisateurs[i].id, mairie->utilisateurs[i].nbFamille,mairie->utilisateurs[i].conteneur_normal->id);
			else if(mairie->utilisateurs[i].bac)
				fprintf(stdout, "\nUser %d composed of %d individuals\t=> individual garbage container %d",mairie->utilisateurs[i].id, mairie->utilisateurs[i].nbFamille,mairie->utilisateurs[i].pbac->id);
		}

		fprintf(stdout,  "\n\n******************************************************************");
		fprintf(stdout,  "\n***********      Composition of the city hall      *******************\n\n");
		fprintf(stdout,  "\t\t%d User family(ies)\n", *NB_UTILISATEUR);
		fprintf(stdout,  "\t\t%d Individual garbage container(s)\n",*NB_BAC);
		fprintf(stdout,  "\t\t%d Collective normal trash garbage container(s)\n",*NB_PBLN);
		fprintf(stdout,  "\t\t%d Collective paper trash container(s)\n",*NB_PBLC);
		fprintf(stdout,  "\t\t%d Collective plastic trash container(s)\n",*NB_PBLP);
		fprintf(stdout,  "\t\t%d Collective glass trash container(s)\n",*NB_PBLV);
		fprintf(stdout,  "\t\t%d Normal garbage truck(s)\n", *NB_CPBLN);
		fprintf(stdout,  "\t\t%d Paper garbage trucks(s)\n", *NB_CPBLC);
		fprintf(stdout,  "\t\t%d Plastic garbage truck(s)\n", *NB_CPBLP);
		fprintf(stdout,  "\t\t%d Glass garbage truck(s)\n", *NB_CPBLV);
		fprintf(stdout,  "\n******************************************************************\n");
		sleep(1);
		fprintf(stdout, "\Beginning of the simulation		(Ctrl-Z to correctly stop the simulation)\n");
		
		/*	On crée le thread mairie	*/
		if(pthread_create(&thread_mairie, &attribut, vie_mairie, mairie) != 0)
			fprintf(stdout, "\Thread error for city hall");

		/*	On crée le thread pour le service de ramassage */
		if(pthread_create(&thread_service_ramassage, &attribut, vie_service_ramassage, mairie->serviceRamassage) != 0)
			fprintf(stdout, "\nThread error for trash-collecter service.");

					
		/*	On crée les threads camions poubelles normaux	*/
		for(i = 0; i < *NB_CPBLN; ++i)
			if(pthread_create(&threads_camions_poubelles_normaux[i], &attribut, vie_camion_poubelles, &mairie->serviceRamassage->camionsNormaux[i]) != 0)
				fprintf(stdout, "\nThread error for normal garbage truck.");
		
		/*	On crée les threads camions poubelles carton	*/
		for(i = 0; i < *NB_CPBLC; ++i)
			if(pthread_create(&threads_camions_poubelles_carton[i], &attribut, vie_camion_poubelles, &mairie->serviceRamassage->camionsCarton[i]) != 0)
				fprintf(stdout, "\nThread error for paper garbage truck.");
		
		/*	On crée les threads camions poubelles plastique	*/
		for(i = 0; i < *NB_CPBLP; ++i)
			if(pthread_create(&threads_camions_poubelles_plastique[i], &attribut, vie_camion_poubelles, &mairie->serviceRamassage->camionsPlastique[i]) != 0)
				fprintf(stdout, "\nThread error for plastic garbage truck");
		
		/*	On crée les threads camions poubelles verre	*/
		for(i = 0; i < *NB_CPBLV; ++i)
			if(pthread_create(&threads_camions_poubelles_verre[i], &attribut, vie_camion_poubelles, &mairie->serviceRamassage->camionsVerre[i]) != 0).
				fprintf(stdout, "\nThread error for glass garbage truck");
	
		/*	On crée les threads des utilisateurs	*/
		for(i = 0; i < *NB_UTILISATEUR; ++i)
			if(pthread_create(&threads_utilisateurs[i], &attribut, vie_utilisateur, &mairie->utilisateurs[i]) != 0)
				fprintf(stdout, "\nThread error for user creation.");
    
		/*	On crée les threads des conteneurs normaux */
		for(i = 0; i < (*NB_PBLN) ; ++i)
			if(pthread_create(&threads_conteneurs_normaux[i], &attribut, vie_poubelle, &mairie->serviceRamassage->conteneursNormaux[i]) != 0)
				fprintf(stdout, "\nThread error for normal trash collective container.");

		/*	On crée les threads des conteneurs carton */
		for(i = 0; i < (*NB_PBLC) ; ++i)
			if(pthread_create(&threads_conteneurs_carton[i], &attribut, vie_poubelle, &mairie->serviceRamassage->conteneursCarton[i]) != 0)
				fprintf(stdout, "\nThread error for paper trash collective container.");

		/*	On crée les threads des conteneurs plastique */
		for(i = 0; i < (*NB_PBLP) ; ++i)
			if(pthread_create(&threads_conteneurs_plastique[i], &attribut, vie_poubelle, &mairie->serviceRamassage->conteneursPlastique[i]) != 0)
				fprintf(stdout, "\nThread error for plastic trash collective container.");

		/*	On crée les threads des conteneurs verre */
		for(i = 0; i < (*NB_PBLV) ; ++i)
			if(pthread_create(&threads_conteneurs_verre[i], &attribut, vie_poubelle, &mairie->serviceRamassage->conteneursVerre[i]) != 0)
				fprintf(stdout, "\nThread error for glass trash collective container.");

		/*	On crée les threads des bacs	*/
		for(i = 0; i < (*NB_BAC) ; ++i)
			if(pthread_create(&threads_bacs[i], &attribut, vie_poubelle, &mairie->serviceRamassage->bacs[i]) != 0)
				fprintf(stdout, "\nThread error for individual container.");

		/*	On intercepte le signal Ctrl-Z pour arrêter correctement le programme	*/
		signal(SIGTSTP, stop_simulation);

		/*	On attend le signal pour terminer le programme	*/
		pthread_mutex_lock(&mutex_stop);
		pthread_cond_wait(&cond_fin, &mutex_stop);
		pthread_mutex_unlock(&mutex_stop);

		/*	Libération des données	*/
		fprintf(stdout, "\nFreeing all data !");
		//destruction_mairie(mairie);
		//destruction_moniteurs_et_mutex_globaux();
		destruction_ipc();
	}
	else
	{
		fprintf(stdout, "\nNumber of arguments incorrect ! The command line is formatted as the following :");
		fprintf(stdout, "\n./main.out 1 2 3 4 5 6 7 8 9");
		fprintf(stdout, "\n1");
		fprintf(stdout, "\tNB_UTILISATEUR");
		fprintf(stdout, "\n2");
		fprintf(stdout, "\tNB_CONTENEURS_NORMAUX");
		fprintf(stdout, "\n3");
		fprintf(stdout, "\tNB_CONTENEURS_CARTONS");
		fprintf(stdout, "\n4");
		fprintf(stdout, "\tNB_CONTENEURS_PLASTIQUE");
		fprintf(stdout, "\n5");
		fprintf(stdout, "\tNB_CONTENEURS_VERRE");
		fprintf(stdout, "\n6");
		fprintf(stdout, "\tNB_CAMION_POUBELLES_NORMAUX");
		fprintf(stdout, "\n7");
		fprintf(stdout, "\tNB_CAMION_POUBELLES_CARTON");
		fprintf(stdout, "\n8");
		fprintf(stdout, "\tNB_CAMION_POUBELLES_PLASTIQUE");
		fprintf(stdout, "\n9");
		fprintf(stdout, "\tNB_CAMION_POUBELLES_VERRE");
	}
	time(&fin);
	fprintf(stdout, "\n\nElapsed seconds : %.2f seconds\n\n",difftime(fin,debut));
	return EXIT_SUCCESS;
}

void stop_simulation(int num)
{
	fprintf(stdout, "\n\nSimulation stopped !\n");
	pthread_mutex_lock(&mutex_stop);
	stop = true;
	pthread_mutex_unlock(&mutex_stop);
	
	pthread_mutex_lock(&mutex_stop);
	pthread_cond_signal(&cond_fin);
	pthread_mutex_unlock(&mutex_stop);
}
