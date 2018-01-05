# Servidor
Aplicação de servidor (Linux).
Deve ouvir as requisições em uma porta através dos sockets e tratar da seguinte forma:
  - #1 Ao chegar uma requisição, clonar o processo [Fork]
  - #2 Enviar os dados de entrada para a filho (Dados da requisição + tipo de processamento)  [Pipe]
  - #3 Criar thread para processar a requisição [Thread]
  - #4 A thread deve escrever em um arquivo de Log, compartilhado por todas as threads [Memória Compartilhada + Semáforo]

# TODO
- [ ] Implementar o listening (socket)
  - [ ] Testar a comunicação
    - [ ] Receber conexão
    - [ ] Receber requisição
- [ ] Tratar as requisições
  - [ ] Comunicação servidor-filho
  - [ ] Comunicação filho-thread
  - [ ] Processar requisição (funções simples)
    - [ ] Ordenação
    - [ ] R/W em arquivo
- [ ] Escrever no Log
  - [ ] Tratamento de Região Crítica
    - [ ] Implementação do Semáforo

## Links possivelmente úteis
http://www.linuxhowtos.org/manpages/2/socket.htm  
http://www.linuxhowtos.org/C_C++/socket.htm
https://stackoverflow.com/questions/2784500/how-to-send-a-simple-string-between-two-programs-using-pipes
http://16hop.com.br/tag/canalizacao-encadeamento-pipe-fifo-pipe-nomeado-socket-ipc-socket-docker/
