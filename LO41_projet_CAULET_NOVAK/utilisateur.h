/*	fichier : utilisateur.h
 * 18/06/2014
 * Florian CAULET 			Johann NOVAK
 *	florian.caulet@utbm.fr	johann.novak@utbm.fr
 *	Fichier qui regroupe les structures liées aux utilisateurs et tous les prototypes des fonctions du fichier utilisateur.c
 */

#ifndef	UTILISATEUR_H
#define 	UTILISATEUR_H

#include "poubelle.h"

/*	Structue Utilisateur
 * Ctete structure représente un utilisateur. C'est une entité qui ne sers qu'à
 *	consommer, remplir les poubelles et faire la queue pour y accéder.
 */
typedef struct utilisateur
{
	/*	ID unique de l'utilisateur	*/
	int id;
	/*	Nombre d'individus dans une famille. Il est est compris entre 1 et 13	*/
	int nbFamille;
	/*	Booléen VRAI si l'utilisateur possède une clé	*/
	bool cle;
	/*	Booléen VRAI si l'utilisateur possède un bac	*/
	bool bac;
	/*	Pointeur sur Poubelle. Correspond au bac individuel de l'utilisateur. Sa capaMax dépend de NBFamille. Si l'utilisateur ne possède pas de bac, cet attribut est égal à NULL.	*/
	Poubelle* pbac;
	/*	Pointeur sur Poubelle. Correspond au sac poubelle que l'utilisateur va remplir avant de le jeter dans un conteneur public. Si l'utilisateur ne possède pas de clé, alors ce pointeur est NULL.	*/
	Poubelle* psac;
	/*	Pointeur sur Poubelle. Correspond au conteneur public dans lequel il va pouboir jeter son sac. Si l'utilisateur ne possède pas de clé, alors ce pointeur est NULL.	*/
	Poubelle* conteneur_normal;
	/*	Pointeur sur Poubelle. Correspond au conteneur public dans lequel il va pouboir jeter ses déchets cartons.	*/
	Poubelle* conteneur_carton;
	/*	Pointeur sur Poubelle. Correspond au conteneur public dans lequel il va pouboir jeter ses déchets plastique.	*/
	Poubelle* conteneur_plastique;
	/*	Pointeur sur Poubelle. Correspond au conteneur public dans lequel il va pouboir jeter ses déchets verre.	*/
	Poubelle* conteneur_verre;
	/*	Message que l'utilisateur va remplir avec son id et sa consommation du jour et qu'il va envoyer à la poubelle dans laquelle il va jeter ses déchets	*/
	MsgBuffer* msgPoubelle;
}Utilisateur;

/*	Procédure creation_utilisateurs
 * Cette procédure va créer *NB_UTILISATEUR qu'il va stocker dans 'u' et leur assignera
 *	s'ils ont des clés, des conteneurs publics	
 *	Paramètres	:	-	Utilisateur* u	:	Pointeur sur Utilisateur qui vont être créés.
 *						-	Poubelle* pn	:	Pointeur sur Poubelle pointant sur les conteneurs
 *													conteneurs normaux.
 *						-	Poubelle* pc	:	Pointeur sur Poubelle pointant sur les conteneurs
 *													conteneurs carton.
 *						-	Poubelle* pp	:	Pointeur sur Poubelle pointant sur les conteneurs
 *													conteneurs plastique.
 *						-	Poubelle* pv	:	Pointeur sur Poubelle pointant sur les conteneurs
 *													conteneurs vrttr.
 */
void creation_utilisateurs(Utilisateur* u, Poubelle* pn, Poubelle* pc, Poubelle* pp, Poubelle* pv);

/*	Procédure vie_utilisateur
 * Cette procédure correspond au pointeur ssur fonction passé en paramètre au thread
 *	associé à un utilisateur. 
 *	Paramètres	:	-	void* utilisateur	:	Pointeur sur Utilisateur mais non typé.
 */
void *vie_utilisateur(void* utilisateur);

/*	Procédure consommer_avec_bac(Utilisateur* u)
 * Cette procédure correspond au moment où un utilisateur va consommer
 *	des déchets et les jeter dans son bac s'il en a un.
 * Paramètres 	:	-	Utilisateur* u	:	Pointeur sur Utilisateur qui va consommer dans
 *													son bac.
 */
void consommer_avec_bac(Utilisateur* u);

/*	Procédure consommer_avec_cle(Utilisateur* u)
 * Cette procédure correspond au moment où un utilisateur va consommer
 *	des déchets dans son sac poubelle. S'il est plein, il ira le jeter dans son
 *	conteneur public attitré.
 * Paramètres 	:	-	Utilisateur* u	:	Pointeur sur Utilisateur qui va consommer dans
 *													son bac.
 */
void consommer_avec_cle(Utilisateur* u);

/*	Fonction va_jeter_poubelle
 *	Cette fonction correspond au moment où l'utilisateur va jeter ses 
 *	poubelles soit dans un conteneur public soit dans son bac. Il va donc 
 *	à l'intérieur réveiller la poubelle avec un message et communiquera avec
 *	elle jusqu'à ce que l'utilisateur ait la réponse finale de la poubelle
 *	indiquant que tout est bon et qu'il peut repartir.
 *	Paramètres	:	-	Utilisateur* u	:	Pointeur sur Utilisateur qui va jeter ses déchets.
 *						-	Poubelle* p		:	Poubelle dans laquelle l'utilisateur jette ses déchets.
 *						-	int LDechets	:	Litrage de déchets que l'utilisateur jette.
 *	Renvoie 			-	VRAI si l'utilisateur a correctement pu jeter ses déchets.
 *						-	FAUX si l'utilisateur n'a pas pu jeter ses déchets comme il faut.
 */
bool va_jeter_poubelle(Utilisateur* u, Poubelle* p, int LDechets);

/*	Procédure destruction_utilisateurs
 * Cette procédure va détruire tous les utilisateurs de la ville.
 *	Paramètres	:	-	Utilisateur* u	:	Pointeur sur utilisateurs qui vont être détruits.
 */
void destruction_utilisateurs(Utilisateur* u);

#endif	//	UTILISATEUR_H
