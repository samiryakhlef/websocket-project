#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

// Handler pour le signal d'arrêt (SIGTERM)
void sigterm_handler(int signum) {
    printf("Arrêt du démon...\n");
    // Effectuer ici les opérations de nettoyage, fermeture de fichiers, etc.
    // ...

    exit(EXIT_SUCCESS);
}

int main() {
    // Enregistrement du gestionnaire de signal pour SIGTERM
    signal(SIGTERM, sigterm_handler);

    // Création du démon en utilisant la fonction daemon(0, 0)
    if (daemon(0, 0) == -1) {
        perror("Erreur lors de la création du démon");
        exit(EXIT_FAILURE);
    }

    // Boucle principale du démon
    while (1) {
        // Effectuer ici les tâches du démon
        // ...

        sleep(1); // Exemple : pause de 1 seconde entre chaque itération
    }

    return 0;
}