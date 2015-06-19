/*	fichier : ipc.h
 *	18/06/2014
 *	Florian CAULET				Johann NOVAK
 *	florian.caulet@utbm.fr	johann.novak@utbm.fr
 *	Fichier qui regroupe toutes les constantes (car c'est le fichier que tous les autres header incluent), les variables globales, les structures liées aux ipc, et les prototypes des fonctions du fichier ipc.c.
 */

#ifndef	IPC_H
#define	IPC_H

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <signal.h>

/*	Définition des constantes de flags des ipc	*/
#define IPC_FLAGS	0777 | IPC_CREAT | IPC_EXCL

/*	Définition des constantes des type des messages de l'ipc ipcSignaux	*/

/*	ID du signal d'une poubelle pleine qui alerte le service de ramassage	*/
#define ID_SIGNAL_POUBELLE_PLEINE 1				
/*	ID du signal d'une poubelle à la mairie pour lui transférer l'ID du jetteur et le litre de déchets jetés 	*/
#define ID_SIGNAL_POUBELLE_MAIRIE 2				

/*	Définition des constantes des poubelles	*/

/*	Capacité maximale de stockage d'un conteneur normal	*/
#define CONTENEUR_NORMAL_CAPA_MAX 200				
/*	Capacité maximale de stockage d'un conteneur recyclable	*/	
#define CONTENEUR_RECYCLABLE_CAPA_MAX 1000		
/*	Capacité de stockage maximal d'un sac poubelle	*/
#define SAC_POUBELLE_CAPA_MAX 30						

/*	Définition des constantes des camions poubelles	*/

/*	Temps que met un camion poubelle à vider une poubelle	*/
#define CAMION_TEMPS_DE_VIDE	3						
/*	Temps que met un camion poubelle à se déplacer à une poubelle pleine (même temps pour repartir)	*/
#define CAMION_TEMPS_DE_DEPLACEMENT	7				

/*	Définition des constantes de l'utilisateur	*/

/*	Intervalle de temps que l'utilisateur attend avant de consommer de nouveau	*/
#define UTILISATEUR_TEMPS_CONSOMMATION	2		
/*	Nombre maximal de membres dans une famille	*/
#define UTILISATEUR_MAX_MEMBRE_FAMILLE	13		
/*	Temps que prend l'utilisateur pour jeter une poubelle ou des déchets	*/
#define UTILISATEUR_TEMPS_DE_JETTE	1			

/*	Définition du type booléen	*/
#define false 0
#define true 1
typedef int bool;

/*	Structure MsgBuffer
 *	Structure utilisée pour transmettre des messages via les ipc, grâce aux primitives
 *	msgsnd et msgrcv.
 */
typedef struct msgBuffer
{
	/*	Type du message, sert de moyen de filtrage des messages	*/
	long type;		
	/*	Tableau de pointeurs non typés. Permet de mettre autant d'argument que l'on veut dans le message	*/
	void** arg;		
}MsgBuffer;

/*	Variables globales	*/
pthread_mutex_t mutex_stop;						/*	mutex de la condition pour stopper le programme	*/
pthread_cond_t cond_signalPoubelleNormale;	/*	moniteur d'attente pour le service de ramssage d'un signal d'une poubelle pleine	*/
pthread_cond_t cond_fin;	/*	moniteur d'attente du main de fin de programme	*/
static bool stop = false;	/*	Variable permettant la continuation du programme	*/
int* ipcID;						/*	ID de l'ipc permettant les messages entre entités	*/
int* ipcPoubelleCamion;		/*	ID de l'ipc permettant les messages entre poubelles et camions	*/
int* ipcSignaux;		/*	ID de l'ipc permettant les messages de plusieurs émetteur à un seul receveur	*/
int* ID;					/*	Variable permettant d'assigner un ID unique à toutes les entités de notre programme	*/
int* NB_UTILISATEUR;	/*	Nombre de famille d'utilisateurs	*/
int* NB_PBLN;			/* Nombre de conteneurs normales	*/
int* NB_PBLC;			/*	Nombre de conteneurs carton	*/
int* NB_PBLP;			/*	Nombre de conteneurs plastique	*/
int* NB_PBLV;			/*	Nombre de conteneurs verre	*/
int* NB_BAC;			/*	Nombre de bacs individuels	*/
int* NB_CPBLN;			/*	Nombre de camions poubelle destinés aux conteneurs normaux	*/
int* NB_CPBLC;			/*	Nombre de camions poubelle destinés aux conteneurs cartons	*/
int* NB_CPBLP;			/*	Nombre de camions poubelle destinés aux conteneurs plastiques	*/
int* NB_CPBLV;			/*	Nombre de camions poubelle destinés aux conteneurs verres	*/

/*	Prototypes des procédures et des fonctions de ipc.c	*/

/*	Procédure creation_ipc
 *	Cette procédure sert à créer les 3 ipc déclarées au dessus avec la primitive
 *	msgget.
 *	Paramètres	:	-	char* name	:	Nom de l'éxécutable.
 */
void creation_ipc(char* name);

/*	Procédure creation_message1
 *	Cette procédure assigne les différentes valeurs en paramètre au message également passé en paramètres. Le 1 signifie qu'il n'y a qu'un seul paramètre non typé.
 *	Paramètres	:	-	MsgBuffer* m	:	Message dont les attributs vont être modifiés;
 *						-	long type		:	Type du message servant à être filtré parmi les autres;
 *						-	void* arg1		:	Argument qui va être incorporé au message.
 */
void creation_message1(MsgBuffer* m, long type, void* arg1);

/*	Procédure creation_message2
 *	Cette procédure assigne les différentes valeurs en paramètre au message également passé en paramètres. Le 2 signifie qu'il y a deux paramètres non typés.
 *	Paramètres	:	-	MsgBuffer* m	:	Message dont les attributs vont être modifiés;
 *						-	long type		:	Type du message servant à être filtré parmi les autres;
 *						-	void* arg1		:	Argument qui va être incorporé au message.
 *						-	void* arg2		:	Argument qui va être incorporé au message.
 */
void creation_message2(MsgBuffer* m, long type, void* arg1, void* arg2);

/*	Procédure destruction_message
 *	Cette procédure va détruire tous les arguments que comporte ce message.
 *	Paramètres	:	-	MsgBuffer* m	:	Message dont les arguments vont être détruits.
 */
void destruction_message(MsgBuffer* m);

/*	Procédure destruction_ipc
 *	Cette procédure va détruire les 3 ipc de notre projet avec la primitive msgctl.
 */
void destruction_ipc();

#endif	//	IPC_H
