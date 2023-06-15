#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Indicateur de fin de programme
int end = 0;

// Gestionnaire de signaux
void sig_handler(int sig) {
	printf("\nSIGINT attrapé, on stop le programme %i\n", getpid());
	end = 1;
}

int main(void) {
	puts("Utilisez SIGINT pour interrompre l'exécution !");
	// Enregistrement du gestionnaire de signaux
	if(signal(SIGINT, sig_handler) == SIG_ERR){
		puts("Erreur à l'enregistrement du gestionnaire de signaux !");
	}
	// Boucle infinie
	while(end == 0);
	return EXIT_SUCCESS;
}