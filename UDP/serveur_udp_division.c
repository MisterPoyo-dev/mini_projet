#include "udp.h"
#include "erreur.h"
#include "nombre.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>

SOCK socket_global;

void gestionnaire_signal(int sig) {
	printf("\nFermeture du serveur...\n");
	fermer_connexion(&socket_global);
	exit(0);
}

int main(int argc, char** argv) {
	traiter_commande(argc!=2, argv[0], "<port>\nmauvais nombre d'arguments");
	traiter_commande(!est_un_port_non_reserve(argv[1]), argv[0], "<port>\n<port> est un port non réservé");

	int HOST_PORT = atoi(argv[1]);
	char buffer[TAILLE_MSG];
	char reponse[TAILLE_MSG];

	// Configurer le gestionnaire de signal pour Ctrl+C
	signal(SIGINT, gestionnaire_signal);

	SOCK socket;
	socket_global = socket;
	init_addr(&socket);
	socket_global = socket;
	
	creer_socket("", HOST_PORT, &socket);
	socket_global = socket;
	attacher_socket(&socket);
	
	printf("Serveur UDP démarré sur le port %d\n", HOST_PORT);
	printf("En attente de messages...\n");

	while(1) {
		memset(buffer, 0, TAILLE_MSG);
		memset(reponse, 0, TAILLE_MSG);
		
		recevoir_message(&socket, buffer);
		printf("Message reçu: %s\n", buffer);
		
		// Vérifier si le client demande l'arrêt
		if (strcmp(buffer, "stop") == 0) {
			sprintf(reponse, "Serveur arrêté");
			envoyer_message(&socket, reponse);
			printf("Signal d'arrêt reçu. Serveur en arrêt...\n");
			break;
		}
		
		// Parcourir le buffer pour trouver le séparateur (espace ou autre)
		long double dividende = 0, diviseur = 0;
		char *endptr;
		
		dividende = strtold(buffer, &endptr);
		
		// Vérifier qu'il y a un second nombre
		if (*endptr != '\0') {
			diviseur = strtold(endptr, &endptr);
		}
		
		// Vérifier division par zéro
		if (diviseur == 0) {
			sprintf(reponse, "ERREUR: Division par zero");
		} else {
			long double resultat = dividende / diviseur;
			sprintf(reponse, "%.2Lf", resultat);
		}
		
		envoyer_message(&socket, reponse);
		printf("Réponse envoyée: %s\n\n", reponse);
	}

	fermer_connexion(&socket);
	exit(0);
}

