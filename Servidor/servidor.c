// +-----------------------------------+
// | Trabalho de Sistemas Operacionais |
// |             Servidor              |
// |                                   |
// |     Ricardo Henrique Brunetto     |
// |    Rafael Rodrigues dos Santos    |
// |   Thais Aparecida Silva Camacho   |
// |                                   |
// |          Dezembro de 2017         |
// |            Maringá - PR           |
// +-----------------------------------+
#include <stdio.h>
#include <stlib.h>
#include <sys/types.h>
#include <sys/sockets.h>
#include <netinet/in.h>

#define ERROR_CODE 1
#define PROTOCOL 0

typedef struct addr{
  short familia;
  u_short porta;
  struct addr endereco;
  char zero[8];
}SocketEndereco;

SocketEndereco server, cliente;

/* int socket(int domain, int type, int protocol) */
// +-----------------------------------+
// |     Gerenciamento de Sockets      |
// +-----------------------------------+
int main(){
    int socketfd = socket(AF_INET, SOCK_STREAM, PROTOCOL);
}

void error(char *err){
  perror(err);
  exit(ERROR_CODE);
}
