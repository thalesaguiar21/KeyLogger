#include <cstdio>       //printf
#include <cstring>      //memset
#include <cstdlib>      //exit
#include <netinet/in.h> //htons
#include <arpa/inet.h>  //inet_addr
#include <sys/socket.h> //socket
#include <unistd.h>     //close
#include <fstream>

#define MAXMSG 1024
#define PORTNUM 4325

int main(int argc, char *argv[])
{
    //variáveis do servidor
    struct sockaddr_in endereco;
    int socketId;

    //variáveis relacionadas com as conexões clientes
    struct sockaddr_in enderecoCliente;
    socklen_t tamanhoEnderecoCliente = sizeof(struct sockaddr);
    int conexaoClienteId;

    //Permitir inserir o caractere de fim de msg \0
    char *msg = new char[MAXMSG+1];
    int byteslidos;

    /*
     * Configurações do endereço
    */
    memset(&endereco, 0, sizeof(endereco));
    endereco.sin_family = AF_INET;
    endereco.sin_port = htons(PORTNUM);
    //endereco.sin_addr.s_addr = INADDR_ANY;
    endereco.sin_addr.s_addr = inet_addr("127.0.0.1");

    /*d
     * Criando o Socket
     *
     * PARAM1: AF_INET ou AF_INET6 (IPV4 ou IPV6)
     * PARAM2: SOCK_STREAM ou SOCK_DGRAM
     * PARAM3: protocolo (IP, UDP, TCP, etc). Valor 0 escolhe automaticamente
    */
    socketId = socket(AF_INET, SOCK_STREAM, 0);

    //Verificar erros
    if (socketId == -1)
    {
        printf("Falha ao executar socket()\n");
        exit(EXIT_FAILURE);
    }

    //Conectando o socket a uma porta. Executado apenas no lado servidor
    if ( bind (socketId, (struct sockaddr *)&endereco, sizeof(struct sockaddr)) == -1 )
    {
        printf("Falha ao executar bind()\n");
        exit(EXIT_FAILURE);
    }

    //Habilitando o servidor a receber conexoes do cliente
    if ( listen( socketId, 10 ) == -1)
    {
        printf("Falha ao executar listen()\n");
        exit(EXIT_FAILURE);
    }

    //servidor ficar em um loop infinito
    while(1)
    {
        //Servidor fica bloqueado esperando uma conexão do cliente
        conexaoClienteId = accept( socketId,(struct sockaddr *) &enderecoCliente,&tamanhoEnderecoCliente );

        //Verificando erros
        if ( conexaoClienteId == -1)
        {
            printf("Falha ao executar accept()");
            exit(EXIT_FAILURE);
        }

        byteslidos = recv(conexaoClienteId,msg,MAXMSG,0);

        if (byteslidos == -1)
        {
            printf("Falha ao executar recv()");
            exit(EXIT_FAILURE);
        }
        else if (byteslidos == 0)
        {
            printf("Cliente finalizou a conexão\n");
            exit(EXIT_SUCCESS);
        }

        msg[byteslidos] = '\0';

        std::ofstream my_file("logs.txt");
        if(my_file.is_open()){
            my_file << msg;
            my_file.close();
        }else
            printf("Nao foi possivel abrir o arquivo!");
        close(conexaoClienteId);
    }

    return 0;
}
