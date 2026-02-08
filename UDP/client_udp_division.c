#include "udp.h"
#include "erreur.h"
#include "nombre.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/time.h>

int main(int argc, char** argv) {
	traiter_commande(argc!=3, argv[0], "<adresse IP> <port>\nmauvais nombre d'arguments");
	traiter_commande(!est_une_adresse_IP(argv[1]), argv[0], "<adresse IP> <port>\n<adresse IP> est une adresse IP au format décimal pointé");
	traiter_commande(!est_un_port_non_reserve(argv[2]), argv[0], "<adresse IP> <port>\n<port> est un port non réservé");

	char* SERVER_IP = argv[1];
	int SERVER_PORT = atoi(argv[2]);
	
	char buffer[TAILLE_MSG];
	char reponse[TAILLE_MSG];
	long double dividende, diviseur;
	
	SOCK socket;
	init_addr(&socket);
	
	creer_socket(SERVER_IP, SERVER_PORT, &socket);
	
	// Configurer un timeout de 5 secondes pour le client
	struct timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	setsockopt(socket.sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
	
	printf("Client UDP - Calculatrice de division\n");
	printf("Connecté à %s:%d\n", SERVER_IP, SERVER_PORT);
	
	while(1) {
		memset(buffer, 0, TAILLE_MSG);
		
		// Lire le dividende
		if (fgets(buffer, TAILLE_MSG, stdin) == NULL) break;
		
		// Enlever le newline
		size_t len = strlen(buffer);
		if (len > 0 && buffer[len-1] == '\n') {
			buffer[len-1] = '\0';
		}
		
		// Vérifier si l'utilisateur veut quitter
		if (strcmp(buffer, "quit") == 0 || strcmp(buffer, "exit") == 0) {
			break;
		}
		
		// Ignorer les lignes vides
		if (strlen(buffer) == 0) {
			continue;
		}
		
		dividende = convertir_nombre(buffer);
		
		memset(buffer, 0, TAILLE_MSG);
		
		// Lire le diviseur
		if (fgets(buffer, TAILLE_MSG, stdin) == NULL) break;
		
		// Enlever le newline
		len = strlen(buffer);
		if (len > 0 && buffer[len-1] == '\n') {
			buffer[len-1] = '\0';
		}
		
		// Ignorer les lignes vides
		if (strlen(buffer) == 0) {
			continue;
		}
		
		diviseur = convertir_nombre(buffer);
		
		// Formater le message pour envoyer au serveur
		sprintf(buffer, "%.10Lf %.10Lf", dividende, diviseur);
		
		// Envoyer au serveur
		envoyer_message(&socket, buffer);
		
		// Recevoir la réponse
		memset(reponse, 0, TAILLE_MSG);
		recevoir_message(&socket, reponse);
		
		printf("résultat : %s\n", reponse);
	}
	
	fermer_connexion(&socket);
	exit(0);
}
