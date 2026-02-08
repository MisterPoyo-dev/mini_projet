#include "tcp.h"
#include "erreur.h"
#include "nombre.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>

int socket_global;

void gestionnaire_signal(int sig) {
	printf("\nFermeture du serveur...\n");
	fermer_connexion(socket_global);
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

	int socket_serveur = creer_socket("", HOST_PORT);
	socket_global = socket_serveur;
	attacher_socket(socket_serveur);
	dimensionner_file_attente_socket(5, socket_serveur);
	
	printf("Serveur TCP démarré sur le port %d\n", HOST_PORT);
	printf("En attente de connexions...\n");

	while(1) {
		int socket_client = attendre_connexion(socket_serveur);
		printf("Client connecté\n");
		
		while(1) {
			memset(buffer, 0, TAILLE_MSG);
			memset(reponse, 0, TAILLE_MSG);
			
			recevoir_message(socket_client, buffer);
			printf("Message reçu: %s\n", buffer);
			
			// Vérifier si le client demande l'arrêt
			if (strcmp(buffer, "stop") == 0) {
				sprintf(reponse, "Serveur arrêté");
				envoyer_message(socket_client, reponse);
				printf("Signal d'arrêt reçu. Fermeture de la connexion...\n");
				fermer_connexion(socket_client);
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
			
			envoyer_message(socket_client, reponse);
			printf("Réponse envoyée: %s\n\n", reponse);
		}
	}

	fermer_connexion(socket_serveur);
	exit(0);
}

