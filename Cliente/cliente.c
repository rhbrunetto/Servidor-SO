// +-----------------------------------+
// | Trabalho de Sistemas Operacionais |
// |             Cliente              |
// |                                   |
// |     Ricardo Henrique Brunetto     |
// |    Rafael Rodrigues dos Santos    |
// |                                   |
// |          Dezembro de 2017         |
// |            Maringá - PR           |
// +-----------------------------------+
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h> // API pra trabalhar com threads
#include <time.h>    // biblioteca pra pegar a hora do sistema
#include <unistd.h>

#define ERROR_CODE 1
#define PROTOCOL 0
#define PORT 5700
#define HOST "localhost"
#define BUF_LEN 256

struct sockaddr_in servidor;
struct hostent *host_servidor;

void menu();
void consultar();
void inserir();
void fazer_requisicao(char, char*, int);
void mostrar_resposta(char, char*);

void error(char *err){
  perror(err);
  exit(ERROR_CODE);
}

int main() {
	socklen_t socklen = sizeof(servidor);
	
	if ((host_servidor = gethostbyname(HOST)) == NULL) {
		error("Erro ao localizar host\n\n");
	}
	
	bzero((char *) &servidor, socklen);
    servidor.sin_family = AF_INET;
	servidor.sin_port = htons(PORT);
    bcopy((char*) host_servidor->h_addr, (char*) &servidor.sin_addr.s_addr, host_servidor->h_length);
	
	while(1) menu();
}

void menu() {
	char opc;
	printf("\tEscolha uma opção:\n");
	printf("1. Buscar um registro\n2. Inserir um registro\n3. Sair\n\n");
	do {
		printf("OPC: ");
		opc = getchar();
		__fpurge(stdin);
		if ((opc < '0' || opc > '2')) {
			printf("Opção inválida\n\n");
		}
	} while(opc < '0' || opc > '2');
	
	if (opc == '1') {
		consultar();
	}
	else if (opc == '2') {
		inserir();
	}
	else {
		exit(0);
	}
}

void consultar() {
	char buffer[BUF_LEN];
	bzero(buffer, BUF_LEN);
	printf("ID a ser procurado: ");
	fgets(buffer, 100, stdin);
	if (buffer[strlen(buffer) - 1] == '\n') {
		buffer[strlen(buffer) - 1] = '\0';
	}
	fazer_requisicao(1, buffer, sizeof(buffer));
}

void inserir() {
	char buffer[3 * BUF_LEN], campo[BUF_LEN];
	int len = 0;
	bzero(buffer, sizeof(buffer));
	
	printf("\tInsira os campos do registro\n");
	printf("ID: ");
	bzero(campo, sizeof(campo));
	fgets(campo, sizeof(campo), stdin);
	if (campo[strlen(campo) - 1] == '\n') campo[strlen(campo) - 1] = '\0';
	strcat(buffer, campo);
	strcat(buffer, "|");
	
	printf("Nome: ");
	bzero(campo, sizeof(campo));
	fgets(campo, sizeof(campo), stdin);
	if (campo[strlen(campo) - 1] == '\n') campo[strlen(campo) - 1] = '\0';
	strcat(buffer, campo);
	strcat(buffer, "|");
	
	printf("Curso: ");
	bzero(campo, sizeof(campo));
	fgets(campo, sizeof(campo), stdin);
	if (campo[strlen(campo) - 1] == '\n') campo[strlen(campo) - 1] = '\0';
	strcat(buffer, campo);
	strcat(buffer, "|");
	printf("%s\n", buffer);
	
	fazer_requisicao(2, buffer, strlen(buffer) + 1);
}

void fazer_requisicao(char tipo, char* dados, int bytes) {
	char resposta[3 * BUF_LEN];
	int sockfd, n;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, PROTOCOL)) < 0) {
		error("Erro ao criar socket do cliente\n\n");
	}
	
	if (connect(sockfd, (struct sockaddr*) &servidor, sizeof(servidor)) < 0) {
		perror("Erro ao se conectar com o servidor\n\n");
	}
	else {
		n = write(sockfd, &tipo, sizeof(tipo));
		if (n < 0) {
			perror("Erro ao enviar tipo da requisição para o servidor\n\n");
		}
		n = write(sockfd, dados, bytes);
		if (n < 0) {
			perror("Erro ao enviar dados para o servidor\n\n");
		}
		
		printf("Esperando resposta do servidor...\n\n");
		bzero(resposta, sizeof(resposta));
		n = read(sockfd, resposta, sizeof(resposta));
		if (n < 0) {
			perror("Erro ao receber resposta do servidor\n\n");
		}
		else {
			mostrar_resposta(tipo, resposta);
		}
	}
	close(sockfd);
}

void mostrar_resposta(char tipo, char* resposta) {
	char ID[BUF_LEN], nome[BUF_LEN], curso[BUF_LEN];
	
	if (strcmp(resposta, "ERROR") == 0) {
		printf("Não foi possível acessar os registros\n\n");
	}
	else if (tipo == 1) {
		if (strcmp(resposta, "NULL") == 0) {
			printf("Registro não encontrado\n\n");
		}
		else {
			sscanf(resposta, "%[^|]|%[^|]|%[^|]|", ID, nome, curso);
			
			printf("ID: %s\n", ID);
			printf("Nome: %s\n", nome);
			printf("Curso: %s\n", curso);
			
			printf("\n");
		}
	}
	else {
		printf("%s\n\n", resposta);
	}
}