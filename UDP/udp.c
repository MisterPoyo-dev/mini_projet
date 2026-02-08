#include "erreur.h"
#include "udp.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

/* Créer une socket */
void creer_socket(char* adresseIP, int port, SOCK* sock) {
	init_addr(sock);
	sock->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	traiter_erreur(__FUNCTION__);

	// adresse et port reutilisable
	int optval = 1;
    	setsockopt(sock->sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	traiter_erreur(__FUNCTION__);

	sock->adresse.sin_family = AF_INET;
	sock->adresse.sin_port = htons(port);

	if (strcmp(adresseIP, "") != 0){
		sock->adresse.sin_addr.s_addr = inet_addr(adresseIP);
	}
	else 
		sock->adresse.sin_addr.s_addr = INADDR_ANY;
	
	sock->longueur_adresse = sizeof(sock->adresse);
}

/* Attacher une socket */
void attacher_socket(SOCK* sock) {
	/*A COMPLETER*/
	bind(sock->sockfd,(struct sockaddr *) &(sock->adresse),sock->longueur_adresse);
	/*ICI*/
	traiter_erreur(__FUNCTION__);
}

/*Initialiser la structure adresse */
void init_addr(SOCK* sock) {
	memset(&sock->adresse, 0, sizeof(sock->adresse));
}

/* Dimensionner la file d'attente d'une socket */
void dimensionner_file_attente_socket(int taille, SOCK* sock) {
	listen(sock->sockfd, taille);
	traiter_erreur(__FUNCTION__);
}

/* Recevoir un message */
void recevoir_message(SOCK* dst, char * buffer) {
	int n = recvfrom(dst->sockfd, buffer, TAILLE_MSG-1, 0, (struct sockaddr *) &(dst->adresse), &dst->longueur_adresse);
	if (n > 0) {
		buffer[n] = '\0';
	}
	traiter_erreur(__FUNCTION__);
}

/* Émettre un message */
void envoyer_message(SOCK* dst, char * message) {
	sendto(dst->sockfd, message, strlen(message), 0, (struct sockaddr *) &(dst->adresse), dst->longueur_adresse);
	traiter_erreur(__FUNCTION__);
}
/* Fermer la connexion */
void fermer_connexion(SOCK* sock) {
	close(sock->sockfd);
	traiter_erreur(__FUNCTION__);
}
