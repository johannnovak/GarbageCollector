/*	fichier : poubelle.h
 * 18/06/2014
 * Florian CAULET 			Johann NOVAK
 *	florian.caulet@utbm.fr	johann.novak@utbm.fr
 *	Fichier qui regroupe les structures liées aux poubelles et tous les prototypes des fonctions du fichier poubelle.c .
 */

#ifndef POUBELLE_H
#define POUBELLE_H

#include "moniteur.h"

/*	Enumération TypePoubelle
 * Cette énumération liste les différents type qu'un poubelle peut avoir.
 *	Si elle est recyclable, alors ce sera soit une poubelle pour carton, soit
 *	pour plastique, soit pour verre. Sinon c'est juste une poubelle normale.
 */
typedef enum 
{
	CARTON, 
	PLASTIQUE, 
	VERRE, 
	NORMAL
}TypePoubelle;

/* Structure Poubelle
 * Cette structure représente les composantes d'une poubelle réelle. Elle est définie
 *	par un type possède des capacités de stockage et peut dire si elle est occupée ou non.
 *	Nous avons décider d'inclure les moniteurs et mutex dans la structure car cela
 *	simplifie grandement le code, et évite d'avoir un tableau globale de moniteurs. De
 *	même pour les mutex.
 */
typedef struct poubelle
{
	/*	Type de la poubelle. */
	TypePoubelle		type;
	/*	ID unique de la poubelle	*/
	int	 				id;									
	/*	Capacité de stockage maximale	de la poubelle. Il est impossible de dépasser ce seuil lors du remplissage de la poubelle	*/
	int 					capaMax;								
	/*	Capacité de stockage courante	de la poubelle. Initialisé à 0, la poubelle au début est vide	*/
	int 					capaCourante;							
	/*	Moniteur servant à attendre que le camion vienne la vider	*/
	pthread_cond_t 	viderPoubelle;
	/*	Moniteur servant à signaler à l'utilisateur qu'il peut jeter son sac	*/
	pthread_cond_t 	remplirPoubelle;		
	/*	Moniteur servant à signaler le prochain utilisateur de la file d'attente	*/
	pthread_cond_t 	prochainUtilisateur;
	/*	Mutex servant à protéger les ressources critiques de la poubelle	*/
	pthread_mutex_t 	mutex_poubelle;	
	/*	booléen indiquant si la poubelle est occupée ou non	*/
	bool 					occupee;							
	/*	booléen indiquant si la poubelle est pleine ou non 	*/
	bool 					pleine;							
}Poubelle;

/*	Prototypes des procédures et des fonctions de poubelle.c	*/

/*	procédure creation_poubelles
 *	Cette procédure créé un nombre 'size' de poubelle dans 'p' qui ont uen capacité
 *	de stockage maximale égale à 'capaMax' et sont du type 'type'.
 *	Paramètres	:	-	Poubelle* p 		: 	Pointeur sur les poubelles à créer.
 *						-	int size				:	Nombre de poubelle à créer.
 *						-	int capaMax			:	Capacité de stockage maximale des poubelles à créer.
 *						-	TypePoubelle type	:	Type des poubelles à creer.
 */
void creation_poubelles(Poubelle* p, int size, int capaMax, TypePoubelle type);

/*	Procédure vie_poubelle
 *	Cette procédure correspond au pointeur sur fonction passé en paramètre au thread
 *	associé à la poubelle.
 *	Paramètres	:	-	void* p	:	Pointeur sur poubelle mais non typé.
 */
void *vie_poubelle(void* p);

/* Fonction poubelle_label
 * Cette fonction sert uniquement à renvoyer le bon string à printer slon la poubelle.
 *	Paramètres 	: 	-	Poubelle* p	:	Pointeur sur poubelle pour laquelle il faut savoir quoi 
 *												printer.
 * Renvoie		:	-	"bac" si la poubelle est un bac individuel;
 *						-	"conteneur normal" si la poubelle est un conteneur normal.
 *						-	"conteneur carton" si la poubelle est un conteneur carton.
 *						-	"conteneur plastique" si la poubelle est un conteneur plastique.
 *						-	"conteneur verre" si la poubelle est un conteneur verre.
 */
char* poubelle_label(Poubelle* p);

/*	Procédure destruction_poubelle
 * Cette procédure va détruire une seule poubelle. Il va free toutes les ressources
 *	de la poubelle.
 *	Paramètres	:	-	Poubelle* p	:	Pointeur sur poubelle qu'il faut détruire.
 */
void destruction_poubelle(Poubelle* p);

/* Procédure destrction_poubelles
 *	Cette procédure va détruire 'size' poubelles dans 'p'. A l'intérieur sera utilisé
 *	destruction_poubelle définie juste au-dessus.
 *	Paramètres	:	-	Poubelle* p	:	Pointeur sur poubelles qu'il faut détruire.
 *						-	int size		:	Nombre de poubelles qu'il faut détruire.
 */
void destruction_poubelles(Poubelle* p, int size);

#endif	//	POUBELLE_H
