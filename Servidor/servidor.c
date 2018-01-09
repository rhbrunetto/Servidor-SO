// +-----------------------------------+
// | Trabalho de Sistemas Operacionais |
// |             Servidor              |
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
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>


#define ERROR_CODE 1
#define PROTOCOL 0
#define PORT 5700
#define BUF_LEN 256
#define REQ_LOG "reqs.log"
#define REGS "registers.dat"
#define SHM_PATH "/tmp/shm_servidor"

pthread_mutex_t mutex_log = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
	int id;
	char tipo; // 1 pra consulta e 2 pra inserção
} requisicao;

void* atualizar_log(void* r) {
	requisicao *req = (requisicao*) r;

	// coletando data e hora da requisição
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	int logfd, status;
	char msg[128];
	bzero(msg, 128);
	sprintf(msg, "%d. Requisição de %s recebida em %s", req->id, (req->tipo == 1) ? "consulta" : "inserção", asctime(timeinfo)); // asctime já coloca \n no final
	free(req);

	pthread_mutex_lock(&mutex_log);
	if ((logfd = open(REQ_LOG, O_APPEND | 0666)) < 0) {
		printf("Impossível abrir log\n\n");
		status = 1;
	}
	else {
		write(logfd, msg, strlen(msg));
		close(logfd);
		status = 0;
	}
	pthread_mutex_unlock(&mutex_log);
	pthread_exit((void*) status);
}

typedef struct {
	FILE *regfd;
	sem_t semaforo;
} SHM;

SHM *memoria;
struct sockaddr_in servidor, cliente;

void tratar_requisicao(int, int);
void error(char *err){
  perror(err);
  exit(ERROR_CODE);
}

int main(){
	/* preparando memória compartilhada */
	key_t shmKey;
	int shmfd, shmsegid;
	
	if ((shmfd = open(SHM_PATH, O_CREAT | O_RDWR, 0666)) < 0) {
		error("Erro ao criar memória\n\n");
	}
	
	if ((shmKey = ftok(SHM_PATH, 5)) < 0) {
		error("Erro na chave\n\n");
	}
	
	if ((shmsegid = shmget(shmKey, sizeof(SHM), 0666 | IPC_CREAT)) < 0) {
		error("Erro ao associar segmento de memória compartilhada\n\n");
	}
	
	memoria = (SHM*) shmat(shmsegid, NULL, 0);
	sem_init(&(memoria->semaforo), 1, 1);
	
	/* preparando conexão */
	int sockfd_servidor, sockfd_cliente, req_id = 0;
	pid_t pid;
	socklen_t socklen = sizeof(servidor);
	pthread_t thread;

	if ((sockfd_servidor = socket(AF_INET, SOCK_STREAM, PROTOCOL)) < 0) {
		error("Erro ao criar socket do servidor\n\n");
	}

	bzero((char*) &servidor, socklen);
	servidor.sin_family = AF_INET;
    servidor.sin_port = htons(PORT);
	servidor.sin_addr.s_addr = INADDR_ANY;

	if (bind(sockfd_servidor, (struct sockaddr*) &servidor, socklen) < 0) {
		error("Erro ao fazer ligação com o descritor do socket\n\n");
	}

	printf("Servidor iniciado com sucesso!\n");
	listen(sockfd_servidor, 5);
	while(1) {
		if ((sockfd_cliente = accept(sockfd_servidor, (struct sockaddr*) &cliente, &socklen)) < 0) {
			error("Erro ao aceitar requisição do cliente\n\n");
		}
		req_id++;

		int fifofd;
		char nome_fifo[256];

		sprintf(nome_fifo, "/tmp/fifo_%d", req_id);
		mkfifo(nome_fifo, 0666);

		if ((pid = fork()) < 0) {
			error("Erro ao criar processo filho\n\n");
		}

		if (pid == 0) { // código do filho
			close(sockfd_servidor);
			if ((fifofd = open(nome_fifo, O_RDONLY)) < 0) {
				error("Erro ao abrir fifo no filho\n\n");
			}
			tratar_requisicao(sockfd_cliente, fifofd);
			close(fifofd);
			unlink(nome_fifo);
			exit(0);
		}
		else { // código do pai
			char tipo, buffer[BUF_LEN];
			read(sockfd_cliente, &tipo, sizeof(tipo));
			read(sockfd_cliente, buffer, sizeof(buffer));

			requisicao *req = malloc(sizeof(requisicao));
			req->id = req_id;
			req->tipo = tipo;

			pthread_create(&thread, NULL, atualizar_log, (void*) req);
			pthread_detach(thread);

			if ((fifofd = open(nome_fifo, O_WRONLY)) < 0) {
				perror("Erro ao abrir fifo no pai\n\n");
				continue;
			}
			int n;
			n = write(fifofd, &tipo, sizeof(tipo));
			if (n <= 0) {
				printf("Erro no pai ao escrever tipo\n");
			}
			n = write(fifofd, buffer, sizeof(buffer));
			if (n <= 0) {
				printf("Erro no pai ao escrever dados\n");
			}
			close(fifofd);
			close(sockfd_cliente);
		}
	}
}

void tratar_requisicao(int sockfd_cliente, int fifofd) {
	char tipo, buffer[BUF_LEN], resposta[3 * BUF_LEN];
	int n;

	bzero(buffer, sizeof(buffer));
	n = read(fifofd, &tipo, sizeof(tipo));
	if (n <= 0) {
		printf("Erro no filho ao ler tipo\n");
	}
	n = read(fifofd, buffer, sizeof(buffer));
	if (n <= 0) {
		printf("Erro no filho ao ler dados\n");
	}

	bzero(resposta, sizeof(resposta));
	if (tipo == 1) { // consulta
		sem_wait(&(memoria->semaforo));
		
		if ((memoria->regfd = fopen(REGS, "r")) == NULL) {
			printf("Erro ao acessar registros\n\n");
			sprintf(resposta, "ERROR");
		}
		else {
			char ID[BUF_LEN], nome[BUF_LEN], curso[BUF_LEN];
			bzero(ID, sizeof(ID));
			bzero(nome, sizeof(nome));
			bzero(curso, sizeof(curso));
			sprintf(resposta, "NULL");

			while(fscanf(memoria->regfd, "%[^|]|%[^|]|%[^|]|", ID, nome, curso) != EOF) {
				if (strcmp(buffer, ID) == 0) {
					sprintf(resposta, "%s|%s|%s|", ID, nome, curso);
					break;
				}
			}
			fclose(memoria->regfd);
		}
		sem_post(&(memoria->semaforo));
	}
	else { // inserção
		sem_wait(&(memoria->semaforo));
		
		if ((memoria->regfd = fopen(REGS, "a")) == NULL) {
			printf("Erro ao acessar registros\n\n");
			sprintf(resposta, "ERROR");
		}
		else {
			fprintf(memoria->regfd, "%s", buffer);
			sprintf(resposta, "Registro gravado com sucesso!\n\n");
			fclose(memoria->regfd);
		}
		sem_post(&(memoria->semaforo));
	}
	write(sockfd_cliente, resposta, sizeof(resposta));
	close(sockfd_cliente);
}
