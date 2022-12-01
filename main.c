#include "mtwister.c"
#include "openssl/crypto.h" //arquivo de definição necessário para SHA256_DIGEST_LENGTH
#include "openssl/sha.h" //arquivo de definição necessário função SHA256
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct BlocoNaoMinerado{
    unsigned int numero;
    unsigned int nonce;
    unsigned char data[184]; 
    unsigned char hashAnterior[SHA256_DIGEST_LENGTH];
}typedef BlocoNaoMinerado;

struct BlocoMinerado{
    BlocoNaoMinerado bloco;
    unsigned char hash[SHA256_DIGEST_LENGTH];
}typedef BlocoMinerado;

void minerarBloco(BlocoNaoMinerado *blocoAMinerar, BlocoMinerado *blocoMinerado){

    unsigned char hash[SHA256_DIGEST_LENGTH];//vetor que armazenará o resultado do hash. Tamanho definido pela libssl
    
    SHA256((unsigned char *)blocoAMinerar, sizeof(*blocoAMinerar), hash);//gera hash do bloco

    while(hash[0] != 0 || hash[1] != 0 || hash[2] != 0){//enquanto o hash não começar com 3 zeros
        if(blocoAMinerar->nonce <= 4294967295){
            blocoAMinerar->nonce++;//incrementa o nonce
            SHA256((unsigned char *)blocoAMinerar, sizeof(*blocoAMinerar), hash);//gera hash do bloco
        }
        else{
            printf("Nonce chegou ao limite. Abortando operação.\n");
            //chama outra função com menos complexidade
        }
    }

    //copia o bloco para o bloco minerado
    blocoMinerado->bloco = *blocoAMinerar;
    //copia o hash para o bloco minerado
    memcpy(blocoMinerado->hash, hash, SHA256_DIGEST_LENGTH);
}

void minerarBlocoMenorComplex(BlocoNaoMinerado *blocoAMinerar, BlocoMinerado *blocoMinerado){

    unsigned char hash[SHA256_DIGEST_LENGTH];//vetor que armazenará o resultado do hash. Tamanho definido pela libssl
    
    SHA256((unsigned char *)blocoAMinerar, sizeof(*blocoAMinerar), hash);//gera hash do bloco

    while(hash[0] != 0 || hash[1] != 0 ){//enquanto o hash não começar com 2 zeros
        if(blocoAMinerar->nonce <= 4294967295){
            blocoAMinerar->nonce++;//incrementa o nonce
            SHA256((unsigned char *)blocoAMinerar, sizeof(*blocoAMinerar), hash);//gera hash do bloco
        }
        else{
            printf("Nonce chegou ao limite. Abortando operação.\n");
            //aborta operação
        }
    }

    //copia o bloco para o bloco minerado
    blocoMinerado->bloco = *blocoAMinerar;
    //copia o hash para o bloco minerado
    memcpy(blocoMinerado->hash, hash, SHA256_DIGEST_LENGTH);
}

void salvarBlocoMinerado(BlocoMinerado *blocoMinerado, int qtdBlocos){//função que guarda o bloco minerado em um arquivo binario
    
    for(int i = 0; i < qtdBlocos; i+=2){
        FILE *arquivo = fopen("blockchain.bin", "ab");
        fwrite(&blocoMinerado[i], sizeof(*blocoMinerado), 2, arquivo);
        fclose(arquivo);
    }
}

BlocoMinerado buscaBloco(int numeroBloco){//função que busca um bloco no arquivo binario

    BlocoMinerado blocoMinerado;
    FILE *arquivo = fopen("blockchain.bin", "rb");
    fseek(arquivo, numeroBloco*sizeof(blocoMinerado), SEEK_SET);
    fread(&blocoMinerado, sizeof(blocoMinerado), 1, arquivo);
    fclose(arquivo);
    return blocoMinerado;
}

void imprimirBloco(BlocoMinerado *blocoMinerado){//funçao que imprime o bloco
    
    printf(" Bloco %u - Nonce: %u\nHash:  -", blocoMinerado->bloco.numero, blocoMinerado->bloco.nonce);
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++){
        printf("%02x", blocoMinerado->hash[i]);
    } 
    printf("\n");
    
    printf("Hash anterior: ");
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++){
        printf("%02x", blocoMinerado->bloco.hashAnterior[i]);
    }
    printf("\n");

    printf("Data: \n");
    for(int i = 0; i < 183; i+=3){
        printf("%d ", blocoMinerado->bloco.data[i]);
        printf("%d ", blocoMinerado->bloco.data[i+1]);
        printf("%d ", blocoMinerado->bloco.data[i+2]);
        printf("\n");
    }
}

void imprimirHash(unsigned char *hash){//função que imprime um hash
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++){
        printf("%02x", hash[i]);
    }
    printf("\n");
}

void imprimirSaldos(unsigned int saldoBitcoin[256]){
    for(int i = 1; i < 256; i++){
        printf("Saldo da carteira %d: %u\n", i, saldoBitcoin[i]);
    }
}

void excluirArquivos(){
    
    int exclui = 1;
    printf("0 - NAO EXCLUIR ARQUIVOS ANTERIORES\n1 - EXCLUIR ARQUIVOS ANTERIORES\n");
    scanf("%d", &exclui);

    if(exclui){
        remove("blockchain.bin");
    }
}

int main() {
    
    excluirArquivos();
    
    BlocoNaoMinerado blocoAMinerar;
    BlocoMinerado blocoMinerado;
    MTRand randNumber = seedRand(1234567);//objeto gerador com semente 1234567
    unsigned char guarda_hash[SHA256_DIGEST_LENGTH];
    unsigned int saldoBitcoin[256];
    memset(saldoBitcoin, 0, sizeof(unsigned int)*256);

    //pedir ao usuario quantos blocos deseja minerar
    int qtdBlocos;
    printf("Quantos blocos deseja minerar? ");
    scanf("%d", &qtdBlocos);

    BlocoMinerado *vetorBlocosMinerados = (BlocoMinerado *)malloc(qtdBlocos * sizeof(BlocoMinerado));//aloca espaço para o vetor de blocos minerados

    for(int num_bloco = 0; num_bloco < qtdBlocos; num_bloco++){
        
        blocoAMinerar.numero = num_bloco;
        blocoAMinerar.nonce = 0;
        
        
        if(num_bloco == 0) {
            memset(blocoAMinerar.hashAnterior, 0, SHA256_DIGEST_LENGTH);
        } 
        else {
            memcpy(blocoAMinerar.hashAnterior, guarda_hash, SHA256_DIGEST_LENGTH);
        }
        
        unsigned char qtdTransacoes = (unsigned char) (1 + (genRandLong(&randNumber) % 61));//gera aleatorio de 1 a 61
        memset(blocoAMinerar.data, 0, sizeof(blocoAMinerar.data));

        for(int i = 0; i < qtdTransacoes; i+=3){
            blocoAMinerar.data[i] = (unsigned char) genRandLong(&randNumber) % 256;//gera aleatorio de 0 a 255
            blocoAMinerar.data[i+1] = (unsigned char) genRandLong(&randNumber) % 256;//gera aleatorio de 0 a 255
            blocoAMinerar.data[i+2] = (unsigned char) (1 + genRandLong(&randNumber) % 51);//gera aleatorio de 1 a 50
            
            if(saldoBitcoin[blocoAMinerar.data[i]] >= blocoAMinerar.data[i+2]){
                saldoBitcoin[blocoAMinerar.data[i]] -= blocoAMinerar.data[i+2];
            }
            else{
                saldoBitcoin[blocoAMinerar.data[i]] = 0;
            }
            saldoBitcoin[blocoAMinerar.data[i+1]] += blocoAMinerar.data[i+2];

        }

        minerarBloco(&blocoAMinerar, &blocoMinerado);
        
        memcpy(guarda_hash, blocoMinerado.hash, SHA256_DIGEST_LENGTH);

        memcpy((vetorBlocosMinerados+num_bloco), &blocoMinerado, sizeof(BlocoMinerado));
    }

    salvarBlocoMinerado(vetorBlocosMinerados, qtdBlocos);




    BlocoMinerado blocoMineradoqueeucriei0 = buscaBloco(0);

    imprimirBloco(&blocoMineradoqueeucriei0);

    BlocoMinerado blocoMineradoqueeucriei = buscaBloco(1);

    imprimirBloco(&blocoMineradoqueeucriei);

    BlocoMinerado blocoMineradoqueeucriei2 = buscaBloco(2);

    imprimirBloco(&blocoMineradoqueeucriei2);

    imprimirSaldos(saldoBitcoin);

    return 0;
}

/*
⠀
⣿⣿⣿⣿⣿⣿⣿⣿⡿⠿⠛⠛⠛⠋⠉⠈⠉⠉⠉⠉⠛⠻⢿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⡿⠋⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠛⢿⣿⣿⣿⣿
⣿⣿⣿⣿⡏⣀⠀⠀⠀⠀⠀⠀⠀⣀⣤⣤⣤⣄⡀⠀⠀⠀⠀⠀⠀⠀⠙⢿⣿⣿
⣿⣿⣿⢏⣴⣿⣷⠀⠀⠀⠀⠀⢾⣿⣿⣿⣿⣿⣿⡆⠀⠀⠀⠀⠀⠀⠀⠈⣿⣿
⣿⣿⣟⣾⣿⡟⠁⠀⠀⠀⠀⠀⢀⣾⣿⣿⣿⣿⣿⣷⢢⠀⠀⠀⠀⠀⠀⠀⢸⣿
⣿⣿⣿⣿⣟⠀⡴⠄⠀⠀⠀⠀⠀⠀⠙⠻⣿⣿⣿⣿⣷⣄⠀⠀⠀⠀⠀⠀⠀⣿
⣿⣿⣿⠟⠻⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠶⢴⣿⣿⣿⣿⣿⣧⠀⠀⠀⠀⠀⠀⣿
⣿⣁⡀⠀⠀⢰⢠⣦⠀⠀⠀⠀⠀⠀⠀⠀⢀⣼⣿⣿⣿⣿⣿⡄⠀⣴⣶⣿⡄⣿
⣿⡋⠀⠀⠀⠎⢸⣿⡆⠀⠀⠀⠀⠀⠀⣴⣿⣿⣿⣿⣿⣿⣿⠗⢘⣿⣟⠛⠿⣼
⣿⣿⠋⢀⡌⢰⣿⡿⢿⡀⠀⠀⠀⠀⠀⠙⠿⣿⣿⣿⣿⣿⡇⠀⢸⣿⣿⣧⢀⣼
⣿⣿⣷⢻⠄⠘⠛⠋⠛⠃⠀⠀⠀⠀⠀⢿⣧⠈⠉⠙⠛⠋⠀⠀⠀⣿⣿⣿⣿⣿
⣿⣿⣧⠀⠈⢸⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠟⠀⠀⠀⠀⢀⢃⠀⠀⢸⣿⣿⣿⣿
⣿⣿⡿⠀⠴⢗⣠⣤⣴⡶⠶⠖⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⡸⠀⣿⣿⣿⣿
⣿⣿⣿⡀⢠⣾⣿⠏⠀⠠⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠛⠉⠀⣿⣿⣿⣿
⣿⣿⣿⣧⠈⢹⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣰⣿⣿⣿⣿
⣿⣿⣿⣿⡄⠈⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣠⣴⣾⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣧⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣠⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣷⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣴⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣦⣄⣀⣀⣀⣀⠀⠀⠀⠀⠘⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⡄⠀⠀⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⠀⠀⠀⠙⣿⣿⡟⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠇⠀⠁⠀⠀⠹⣿⠃⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⡿⠛⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⢐⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⠿⠛⠉⠉⠁⠀⢻⣿⡇⠀⠀⠀⠀⠀⠀⢀⠈⣿⣿⡿⠉⠛⠛⠛⠉⠉
⣿⡿⠋⠁⠀⠀⢀⣀⣠⡴⣸⣿⣇⡄⠀⠀⠀⠀⢀⡿⠄⠙⠛⠀⣀⣠⣤⣤⠄⠀

*/
/*

c) CONDIÇÃO DE MINERAÇÃO E ARMAZENAMENTO EM ARQUIVO
Para este projeto, um bloco será considerado minerado se as três primeiras posições de seu valor hash forem iguais a zero (decimal). Caso a condição NÃO seja satisfeita, o valor nonce deverá 
ser incrementado e um nova tentativa deve ser feita invocando a função SHA256 que produzirá outro valor no vetor hash. Quando a condição for satisfeita, o bloco junto com seu hash minerado 
válido formarão um bloco minerado e poderão ser gravados em arquivo. Contudo, seguindo as boas práticas para escrita eficiente em arquivo, convêm realizar gravações/leituras a cada 512 bytes 
(no caso de SSDs) ou 4096 bytes (no caso de HDs magnéticos) de informação acumuladas. Por exemplo, se considerarmos arquivo em SSD, um bloco minerado (isto é, bloco não minerado + seu 
hash válido), resulta em 256 bytes. Assim, após minerar um bloco, você deverá gravá-lo numa variável do tipo BlocoMinerado até que outro bloco seja minerado e, assim, ambos possam ser gravados 
em um arquivo binário através da função fwrite (pesquise o uso desta função). A struct de uma variável do tipo bloco minerado é:

struct BlocoMinerado
{
  BlocoNaoMinerado bloco;//neste campo você pode atribuir a variável blocoAMinerar diretamente
  unsigned char hash[SHA256_DIGEST_LENGTH]; //Neste campo você põe o hash válido da variável blocoAMinerar
};
typedef struct BlocoMinerado BlocoMinerado;



===============================> CONSULTAS QUE DEVEM SER SUPORTADAS PELO CÓDIGO FONTE
Após a mineração, O programa simulador em C (ambiente gcc/linux) deverá suportar os seguintes consultas LENDO OS DADOS DIRETAMENTE DO ARQUIVO PRODUZIDO:
1.Dado o número do bloco imprimir todos seus dados, incluindo o hash válido no formato hexadecimal.
2.Quantos bitcoins um dado endereço tem?
3.Qual o endereço tem mais bitcoins?
4.Listar endereços com respectivas quantidades de bitcoins em ordem crescente.
DICA: você pode manter um vetor unsigned int com 256 posições inicializadas com zero para ir acumulando o saldo das carteiras durante a simulação. Por exemplo, se numa transação o endereço 100 
envia 1 bitcoin ao endereço 4, você credita 1 bitcoin na posição 4 do vetor e debita 1 da posição 100 (se o débito levar a posição a ficar negativa, apenas deixe zerada para evitar saldos 
negativos).



RESTRIÇÕES.
. Para dúvidas cujas respostas não se encontrem neste enunciado, a equipe deverá ter iniciativa para tomar suas próprias decisões (velando sempre pela eficiência do código) e sujeitando tais 
decisões às exigências mínimas do enunciado. Neste caso, um breve relatório deverá ser elaborado para explicar as tomadas de decisões do projeto.
 Permanencendo as dúvidas, o professor deverá ser consultado.
*/

