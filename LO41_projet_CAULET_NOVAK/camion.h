/*	fichier : camion.h	
 * 18/06/2014
 * Florian CAULET 			Johann NOVAK
 *	florian.caulet@utbm.fr	johann.novak@utbm.fr
 *	Fichier qui regroupe les structures liées au camion et tous les prototypes des fonctions du fichier camion.c .
 */

#ifndef	CAMION_H
#define	CAMION_H

#include "poubelle.h"

/*	Structure Camion
 *	Cette structure représente un camion poubelle. Son rôle est d'être
 *	en standby jusqu'à ce qu'il reçoit un message du service de ramassage
 *	lui indiquant quelle poubelle il doit aller vider immédiatement. Chaque
 *	camion est pareil, cependant ils peuvent être désignés pour n'aller 
 *	ramasser qu'un seul type de déchets (NORMAL, CARTON, PLASTIQUE, VERRE).
 */
typedef struct camion
{
	/*	ID unique du camion.	*/
	int 					id;					
	/*	Pointeur sur poubelle que la camion doit vider. NULL si camion en attente*/
	Poubelle*		 	poubelle;			
	/*	Booléen égal à vrai lorsque le camion est en déplacement.	*/
	bool 					enDeplacement;	
	/*	Mutex servant à accéder de façon protégée aux ressources critiques du camion.	*/
	pthread_mutex_t   mutex_camion;		
}Camion;

/*	Prototypes des procédures et des fonctions de camion.c	*/

/* Procédure creation_camion_poubelles
 *	Cette procédure crée un tableau de camions de taille size. Chaques attributs
 *	de chaques camions créés seront initialisées.
 *	Paramètres :	-	Camion* c	: Pointeur sur camion;
 *						-	int size		: Nombre de camions qu'il faut créer.
 */
void creation_camions_poubelles(Camion* c, int size);

/*	Procédure vie_camion_poubelles
 *	Cette procédure est le pointeur sur fonction passé en paramètre au thread associé
 *	à un camion. Le camion exécutera cette procédure en boucle jusqu'à la fin du programme.
 *	Paramètres :	-	void* cp	 	: Pointeur sur le camion poubelle qui doit vivre sa vie.
 */
void*	vie_camion_poubelles(void* cp);

/*	Procédure destruction_camions
 *	Cette procédure détruit tous les camions du pointeur entré en paramètre. Le deuxième
 *	paramètre est là pour généraliser la procédure pour toute longueur de tableaux.
 *	Paramètres :	-	Camion* c	:	Camions qui doivent être détruits;
 *						-	int size		:	Nombre de camions à détruire.
 */
void destruction_camions(Camion* c, int size);

#endif	//	CAMION_H
