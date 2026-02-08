#include "erreur.h"
#include "tcp.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

/* Variables globales pour stocker les structures d'adresse */
static struct sockaddr_in server_addr;
static struct sockaddr_in client_addr;
static socklen_t client_addr_len = sizeof(client_addr);

/* Créer une socket serveur */
int creer_socket(char* adresseIP, int port) {
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	traiter_erreur(__FUNCTION__);

	// adresse et port reutilisable
	int optval = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	traiter_erreur(__FUNCTION__);

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);

	if (strcmp(adresseIP, "") != 0) {
		server_addr.sin_addr.s_addr = inet_addr(adresseIP);
	} else {
		server_addr.sin_addr.s_addr = INADDR_ANY;
	}

	return sock;
}

/* Connecter une socket client */
void connecter_socket(int sock) {
	connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
	traiter_erreur(__FUNCTION__);
}

/* Attacher une socket serveur */
void attacher_socket(int sock) {
	bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
	traiter_erreur(__FUNCTION__);
}

/* Dimensionner la file d'attente d'une socket serveur */
void dimensionner_file_attente_socket(int taille, int sock) {
	listen(sock, taille);
	traiter_erreur(__FUNCTION__);
}

/* Initialiser la structure adresse client */
void init_addr_client() {
	memset(&client_addr, 0, sizeof(client_addr));
	client_addr_len = sizeof(client_addr);
}

/* Attendre une connexion */
int attendre_connexion(int sock) {
	init_addr_client();
	int socktraitement = accept(sock, (struct sockaddr *)&client_addr, &client_addr_len);
	traiter_erreur(__FUNCTION__);

	return socktraitement;
}

/* Recevoir un message */
void recevoir_message(int socktraitement, char * buffer) {
	int n = recv(socktraitement, buffer, TAILLE_MSG-1, 0);
	if (n > 0) {
		buffer[n] = '\0';
	}
	traiter_erreur(__FUNCTION__);
}

/* Émettre un message */
void envoyer_message(int socktraitement, char * message) {
	send(socktraitement, message, strlen(message), 0);
	traiter_erreur(__FUNCTION__);
}

/* Fermer la connexion */
void fermer_connexion(int socktraitement) {
	close(socktraitement);
	traiter_erreur(__FUNCTION__);
}
