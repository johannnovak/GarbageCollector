/*	fichier : mairie.h
 * 18/06/2014
 * Florian CAULET				Johann NOVAK
 *	florian.caulet@utbm.fr	johann.novak@utbm.fr
 *	Fichier qui regroupe toutes les structures liées à la mairie et les prototypes des fonctions définies dans le fichier mairie.c
 */

#ifndef MAIRIE_H
#define MAIRIE_H

#include "utilisateur.h"
#include "serviceRamassage.h"

/*	Structure Mairie
 *	Structure qui représente la mairie d'une ville. Elle possède toutes 
 *	les informations concernant la ville. Le rôle de la mairie est de facturer
 *	les utilisateurs par rapport aux litres de déchets qu'ils ont jeté dans le mois.
 */
typedef struct mairie
{
	/*	Pointeur sur le service de ramassage. Sert à lui envoyer des messages et à pouvoir accéder aux données de toutes les poubelles existantes dans la ville.	*/
	ServiceRamassage*		serviceRamassage;				
	/*	Pointeur sur Utilisateur. Sert à pouvoir facturer les utilisateurs, et à faire le lien avec le service de ramassage lors de la création des bacs individuels.	*/
	Utilisateur*	 		utilisateurs;
	/*	Tableau d'entiers de taille NB_UTILISATEUR. Chaque case est associée à un utilisateur et correspond au nombre de déchets qu'il accumule pendant le mois.	*/
	int*						compteurDechets;
	/*	Correspond à l'ID à partir duquel les utilisateurs ont été créés. Sert à incrémenter facilement le tableau des déchets ci-dessus.	*/
	int*						indexPremierUtilisateur;
}Mairie;

/*	Prototypes des procédures et des fonctions définies dans mairie.c	*/

/*	Procédure création_mairie
 * Cette procédure sert à créer la mairie et ses attributs. A l'intérieur sera appelé 
 *	les autres procédures de créations, c'est donc d'ici que se crée toutes les données
 *	de notre programme.
 *	Paramètres	:	-	Mairie* m	:	Pointeur sur Mairie qui va être instanciée.
 */
void creation_mairie(Mairie* m);

/*	Procédure creation_bacs
 * Cette procédure crée les bacs individuels. Une fois créés, il faut lier les bacs aux
 *	utilisateurs qui possèdent un bac.
 *	Paramètres	:	Mairie* m	:	Pointeur sur Mairie qui possède à la fois les utilisateurs
 *											et l'attribut "bacs" du service de ramassage.
 */
void creation_bacs(Mairie* m);

/*	Procédure vie_mairie
 * Cette procédure est le pointeur sur fonction qui est passé en paramètre au thread 
 *	associé à la mairie.
 *	Paramètres	:	-	void* m	:	Pointeur sur Mairie, mais non typé.
 */
void* vie_mairie(void* m);

/*	Procédure destruction_mairie
 * Cette procédure sert à détruire la mairie. De ce fait, elle va également détruire 
 *	toutes les autres structure de notre programme.
 *	Paramètres	:	-	Mairie* m	:	Pointeur sur Mairie qui va être détruite.
 */
void destruction_mairie(Mairie* m);

#endif	//	MAIRIE_H
