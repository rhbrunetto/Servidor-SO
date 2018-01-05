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
#include <pthread.h> // API pra trabalhar com threads
#include <time.h>    // biblioteca pra pegar a hora do sistema

#define ERROR_CODE 1
#define PROTOCOL 0

pthread_mutex_t mutex_log = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_reg = PTHREAD_MUTEX_INITIALIZER;

void atualizar_log(char* tipo_req) {
	// coletando data e hora da requisição
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	
	FILE *log;
	if ((log = fopen("requests.log", "a")) == NULL) {
		printf("Impossível abrir log\n\n");
		return;
	}
	fprintf(log, "Requisição de %s recebida em %s", tipo_req, asctime(timeinfo)); // asctime já coloca \n no final
	fclose(log);
}

void* thread_de_consulta(void* t) {
	/*
		tipo_param param = (tipo_param) t;
		esse casting é usado pra recuperar o argumento passado pra thread
		se não precisar de parâmetro, pode tirar o void* t da declaração
		lembrar de passar NULL na criação da thread caso não precise de parâmetro
		se precisar de mais de um parâmetro, deve ser encapsulado em uma struct
	*/

	// se precisar fazer algo antes de escrever no log, fazer aqui
	
	/*
		a thread só escreve no log se o mutex não estiver bloqueado
		ao chamar mutex_lock, a thread fica esperando o mutex estar livre
		quando estiver livre, bloqueia o mutex, escreve no log e desbloqueia
	*/
	pthread_mutex_lock(&mutex_log);
	atualizar_log("consulta");
	pthread_mutex_unlock(&mutex_log);
	
	// operações antes da leitura do arquivo de registros
	
	pthread_mutex_lock(&mutex_reg);
	FILE *reg;
	if ((reg = fopen("regs.dat", "r")) == NULL) {
		printf("Impossível ler registros\n\n");
		// talvez criar uma função pra registrar esse erro no log
	}
	else {
		/* 
			---------------
			   leitura do arquivo
			---------------
		*/
	}
	pthread_mutex_unlock(&mutex_reg);
	
	/*
		a última instrução da thread deve ser pthread_exit
		o argumento passado é um status, que pode ser recuperado depois
		se não for precisar desse status, passa NULL
	*/
	pthread_exit(NULL);
}

void* thread_de_insercao(void* t) {
	/*
		tipo_param param = (tipo_param) t;
		esse casting é usado pra recuperar o argumento passado pra thread
		se não precisar de parâmetro, pode tirar o void* t da declaração
		lembrar de passar NULL na criação da thread caso não precise de parâmetro
		se precisar de mais de um parâmetro, deve ser encapsulado em uma struct
	*/

	// se precisar fazer algo antes de escrever no log, fazer aqui
	
	/*
		a thread só escreve no log se o mutex não estiver bloqueado
		ao chamar mutex_lock, a thread fica esperando o mutex estar livre
		quando estiver livre, bloqueia o mutex, escreve no log e desbloqueia
	*/
	
	pthread_mutex_lock(&mutex_log);
	atualizar_log("inserção");
	pthread_mutex_unlock(&mutex_log);
	
	// operações antes da escrita no arquivo de registros
	
	pthread_mutex_lock(&mutex_reg);
	FILE *reg;
	if ((reg = fopen("regs.dat", "r+")) == NULL) {
		printf("Impossível escrever registros\n\n");
		// talvez criar uma função pra registrar esse erro no log
	}
	else {
		/* 
			---------------
			   escrita no arquivo
			---------------
		*/
	}
	pthread_mutex_unlock(&mutex_reg);
	
	/*
		a última instrução da thread deve ser pthread_exit
		o argumento passado é um status, que pode ser recuperado depois
		se não for precisar desse status, passa NULL
	*/
	pthread_exit(NULL);
}

void criar_thread(pthread_t *thread, pthread_attr_t *attr, char tipo_req) {
	/*
		pthread_create permite passar 1 argumento pra função que a thread criada vai executar
		independente do tipo do argumento, ele deve ser passado como (void*)arg
		se não precisar passar argumento, deve passar NULL e modificar a declaração da função que está pedindo um parâmetro
	*/
	int arg = 1; // apenas pra exemplificar passagem de argumento
	
	if (tipo_req == 1) { // 1 pra thread de consulta, 2 pra thread de inserção
		pthread_create(thread, attr, thread_de_consulta, (void*) arg);
	}
	else {
		pthread_create(thread, attr, thread_de_insercao, (void*) arg);
	}
}

void tratar_requisicao() {
	/*
		if ((v = fork())) {
			// código do pai
		}
		else {
			// código do filho
			
			/*
				se cada filho criar apenas uma thread, desse jeito está bom
				caso contrário será necessário um vetor de threads em algum lugar fora
			*/
			char tipo_req; // de alguma forma determinar se é consulta (1) ou escrita (2)
			pthread_t thread;
			pthread_attr_t attr;
			pthread_attr_init(&attr);
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
			criar_thread(&thread, &attr, tipo_req);
			
			/*
				quando o filho criar a thread, ele não pode terminar antes da thread
				fazer um join com a thread bloqueia o processo criador da thread até que a thread criada termine,
				isto é, até que a thread chame pthread_exit
				o void* status passado no join pega o status passado em pthread_exit
			*/
			
			void* status;
			pthread_join(thread, &status);
			
			// finalizar/destruir filho
		}
	*/
}

typedef struct addr {
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
