/*	fichier : serviceRamassage.h
 * 18/06/2014
 * Florian CAULET 			Johann NOVAK
 *	florian.caulet@utbm.fr	johann.novak@utbm.fr
 *	Fichier qui regroupe les structures liées au service de ramassage et tous les prototypes des fonctions du fichier serviceRamassage.c .
 */
 
#ifndef SERVICERAMASSAGE_H
#define SERVICERAMASSAGE_H

#include "camion.h"

/*	Structure ServiceRamassage
 *	Cette structure représente le service de ramassage. Il possède toutes les 
 *	informations concerant les poubelles de la ville. Il fait le lien entre 
 *	les poubelles qui signalent qu'elles sont pleines et les camions poubelles
 *	disponibles. Il va rester en attente jusqu'à ce qu'il reçoit un message d'une
 *	poubelle. Il enverra son adresse au premier camion disponible selon le type
 *	de la poubelle.
 */
typedef struct serviceRamassage
{
	/*	Pointeur sur les conteneurs normaux de la ville	*/
	Poubelle*					conteneursNormaux;
	/*	Pointeur sur les conteneurs cartons de la ville	*/
	Poubelle*					conteneursCarton;
	/*	Pointeur sur les conteneurs plastique de la ville	*/
	Poubelle*					conteneursPlastique;
	/*	Pointeur sur les conteneurs verre de la ville	*/
	Poubelle*					conteneursVerre;
	/*	Pointeur sur les bacs  individuels des utilisateurs	*/
	Poubelle*					bacs;
	/*	Pointeur sur les camions destinés aux poubelles normales	*/
	Camion*						camionsNormaux;				
	/*	Pointeur sur les camions destinés aux poubelles carton	*/
	Camion*						camionsCarton;				
	/*	Pointeur sur les camions destinés aux poubelles plastique	*/
	Camion*						camionsPlastique;			
	/*	Pointeur sur les camions destinés aux poubelles verre	*/
	Camion*						camionsVerre;				
}ServiceRamassage;

/* Procédure creation_service_ramassage
 * Cette procédure va créer le service de ramassage et donc créera également
 *	toutes les poubelles de la ville avec tous les camions poubelles.
 *	Paramètres	:	-	ServiceRamassage* sr	;	Service de ramassage à créer.
 */
void creation_service_ramassage(ServiceRamassage* sr);

/*	Procédure vie_ramassage
 * Cette procédure correspond au pointeur sur fonction passé en paramètre au thread 
 *	associé au service de ramassage.
 *	Paramètres	:	-	void* sr	:	Service de ramassage mais non typé.
 */
void* vie_service_ramassage(void* serviceRamassage);

/*	Fonction chercher_camion_poubelle
 * Cette fonction sert çà chercher le premier camion poubelle
 *	disponible pour l'envoyer vider une poubelle.
 *	Paramètres	:	-	ServiceRamassage* sr	:	Pointeur sur le service de ramassage
 *														   nécessaire pour déterminer quels camions
 *															envoyer.
 *						-	TypePoubelle type    : 	Type de la poubelle qu'il faut aller
 *														   chercher.
 *	Renvoie		Le premier camion disponible.
 */
Camion chercher_camion_poubelle(ServiceRamassage* sr, TypePoubelle type);

/*	Fonction envoyer_camion_poubelle
 * Cette fonction va envoyer le message passé en paramètre au Camion passé
 *	également en paramètre.
 *	Paramètres	:	-	Camion c		 :	Camion auquel il faut envoyer le message avec
 *												l'adresse de la poubelle pleine.
 *						-	MsgBuffer* m :	Message qu'il faut envoyer au camion.
 *	Renvoie		-	VRAI si le message a réussit à être envoyé;
 *					-	FAUX si la primitive msgsnd a renvoyé -1 (Survient lorsque l'on supprime
 *						la file de message avec msgctl).
 */
bool envoyer_camion_poubelle(Camion c, MsgBuffer* m);

/*	Procédure destruction_service_ramassage
 * Cette procédure va détruire le service de ramassage et donc va appeler les 
 *	procédure destruction_camion_poubelle et destruction_poubelles.
 *	Paramètres	:	-	ServiceRamassage* sr	:	Service de ramassage à détruire.
 */
void destruction_service_ramassage(ServiceRamassage* sr);

#endif	//	SERVICERAMASSAGE_H
