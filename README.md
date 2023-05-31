# websocket-project
## Objectifs
Développement d’une application mettant en œuvre une exécution parallélisée, des
sockets et gérant les sémaphores (norme POSIX)

### Étape 1
Choix d’un type d’application (Chat, Serveur Web, etc…) et validation par l’enseignant ;
### Étape 2
L’application doit gérer les signaux grâce à la fonction « signal() » :
• SIGHUP : redémarrage ;
• SIGTERM / SIGINT : arrêt ;
### Étape 3
L’application se détache du processus courant et devient chef du fil d’exécution en utilisant
la fonction « daemon() » ;
### Étape 4
Utilisation des sockets pour réceptionner des connexions externes à l’application. Vous
pouvez utiliser une application comme « telnet » pour tester votre application (si socket en
TCP).
### Étape 5
Communication inter-processus pour que les processus se terminent proprement.
### Étape 6
Réalisation de tests pour vérifier le bon fonctionnement de l’application
### Étape 7 (Bonus)
Utilisation d’un système de journalisation (log) pour suivre le fonctionnement de
l’application.
