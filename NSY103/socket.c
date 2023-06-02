#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in.h>

#define int domain AF_INET;
#define int type SOCK_STREAM;
#define int protocol IPPROTO_TCP;

// creation objet socket
// domain AF_INET
// type => SOCK_STREAM
// protocol => 0
socket(domain, type, protocol);

// pour fermer la socket
close (fd);

// pour envoyer les datas
// message send => mode connecté
// sento => mode non connecte
send(s, msg, len, flags);

// recv => mode connecté
// recvfrom => mode non connecté
recv(s, buf, len, flags);

// bind => lie une socket avec 
// structure soackaddr
//int bind(int sockfd, struct sockaddr *my_addr, socklen_t addrlen);

//int bind(int sockfd, struct sockaddr *my_addr, socklen_t addrlen);

//int listen(int s, int backlog);

// int listen(int s, int backlog);