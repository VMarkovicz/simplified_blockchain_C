#include "mtwister.c"
#include "openssl/crypto.h" //arquivo de definição necessário para SHA256_DIGEST_LENGTH
#include "openssl/sha.h"    //arquivo de definição necessário função SHA256
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct BlocoNaoMinerado
{
    unsigned int numero;
    unsigned int nonce;
    unsigned char data[184];
    unsigned char hashAnterior[SHA256_DIGEST_LENGTH];
} typedef BlocoNaoMinerado;

struct BlocoMinerado
{
    BlocoNaoMinerado bloco;
    unsigned char hash[SHA256_DIGEST_LENGTH];
} typedef BlocoMinerado;

struct ABP
{
    unsigned int saldo;
    unsigned char endereco;
    struct ABP *esq;
    struct ABP *dir;
} typedef ABP;

void minerarBloco(BlocoNaoMinerado *blocoAMinerar, BlocoMinerado *blocoMinerado)
{

    unsigned char hash[SHA256_DIGEST_LENGTH]; // vetor que armazenará o resultado do hash. Tamanho definido pela libssl

    SHA256((unsigned char *)blocoAMinerar, sizeof(*blocoAMinerar), hash); // gera hash do bloco

    while (hash[0] != 0 || hash[1] != 0 || hash[2] != 0)
    { // enquanto o hash não começar com 3 zeros
        if (blocoAMinerar->nonce <= 4294967295)
        {
            blocoAMinerar->nonce++;                                               // incrementa o nonce
            SHA256((unsigned char *)blocoAMinerar, sizeof(*blocoAMinerar), hash); // gera hash do bloco
        }
        else
        {
            printf("Nonce chegou ao limite. Abortando operação.\n");
            // chama outra função com menos complexidade
        }
    }

    // copia o bloco para o bloco minerado
    blocoMinerado->bloco = *blocoAMinerar;
    // copia o hash para o bloco minerado
    memcpy(blocoMinerado->hash, hash, SHA256_DIGEST_LENGTH);
}

void minerarBlocoMenorComplex(BlocoNaoMinerado *blocoAMinerar, BlocoMinerado *blocoMinerado)
{

    unsigned char hash[SHA256_DIGEST_LENGTH]; // vetor que armazenará o resultado do hash. Tamanho definido pela libssl

    SHA256((unsigned char *)blocoAMinerar, sizeof(*blocoAMinerar), hash); // gera hash do bloco

    while (hash[0] != 0 || hash[1] != 0)
    { // enquanto o hash não começar com 2 zeros
        if (blocoAMinerar->nonce <= 4294967295)
        {
            blocoAMinerar->nonce++;                                               // incrementa o nonce
            SHA256((unsigned char *)blocoAMinerar, sizeof(*blocoAMinerar), hash); // gera hash do bloco
        }
        else
        {
            printf("Nonce chegou ao limite. Abortando operação.\n");
            // aborta operação
        }
    }

    // copia o bloco para o bloco minerado
    blocoMinerado->bloco = *blocoAMinerar;
    // copia o hash para o bloco minerado
    memcpy(blocoMinerado->hash, hash, SHA256_DIGEST_LENGTH);
}

void salvarBlocoMinerado(BlocoMinerado *blocoMinerado, int qtdBlocos)
{ // função que guarda o bloco minerado em um arquivo binario

    for (int i = 0; i < qtdBlocos; i += 2)
    {
        FILE *arquivo = fopen("blockchain.bin", "ab");
        fwrite(&blocoMinerado[i], sizeof(*blocoMinerado), 2, arquivo);
        fclose(arquivo);
    }
}

BlocoMinerado buscaBloco(int numeroBloco)
{ // função que busca um bloco no arquivo binario

    BlocoMinerado blocoMinerado;
    FILE *arquivo = fopen("blockchain.bin", "rb");
    fseek(arquivo, numeroBloco * sizeof(blocoMinerado), SEEK_SET);
    fread(&blocoMinerado, sizeof(blocoMinerado), 1, arquivo);
    fclose(arquivo);
    return blocoMinerado;
}

void imprimirBloco(BlocoMinerado *blocoMinerado)
{ // funçao que imprime o bloco

    printf(" Bloco %u - Nonce: %u\nHash:  -", blocoMinerado->bloco.numero, blocoMinerado->bloco.nonce);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        printf("%02x", blocoMinerado->hash[i]);
    }
    printf("\n");

    printf("Hash anterior: ");
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        printf("%02x", blocoMinerado->bloco.hashAnterior[i]);
    }
    printf("\n");

    printf("Data: \n");
    for (int i = 0; i < 183; i += 3)
    {
        printf("%d ", blocoMinerado->bloco.data[i]);
        printf("%d ", blocoMinerado->bloco.data[i + 1]);
        printf("%d ", blocoMinerado->bloco.data[i + 2]);
        printf("\n");
    }
}

void imprimirHash(unsigned char *hash)
{ // função que imprime um hash
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        printf("%02x", hash[i]);
    }
    printf("\n");
}

void imprimirSaldos(unsigned int saldoBitcoin[256])
{
    for (int i = 1; i < 256; i++)
    {
        printf("Saldo da carteira %d: %u\n", i, saldoBitcoin[i]);
    }
}

unsigned int verificaSaldoBitcoin(unsigned char endereco, unsigned int saldoBitcoin[256])
{ // função que verifica o saldo de uma carteira
    printf("Endereço %u tem %u bitcoins\n", endereco, saldoBitcoin[endereco]);

    return saldoBitcoin[endereco];
}

unsigned char enderecoMaisBitcoin(unsigned int saldoBitcoin[256])
{

    unsigned int max = saldoBitcoin[0];
    unsigned char pos = 0;
    for (int i = 1; i < 256; i++)
    {
        if (saldoBitcoin[i] > max)
        {
            max = saldoBitcoin[i];
            pos = i;
        }
    }
    printf("O endereco com maior saldo de BitCoin é %u, com %u BitCoins.\n", pos, max);
    return pos;
}

ABP *inserirABP(ABP *raiz, unsigned char endereco, unsigned int saldoBitcoin[256])
{ // função que insere um nó na árvore binária de busca

    if (raiz == NULL)
    {
        raiz = (ABP *)malloc(sizeof(ABP));
        raiz->endereco = endereco;
        raiz->saldo = saldoBitcoin[endereco];
        raiz->esq = NULL;
        raiz->dir = NULL;
    }
    else if (saldoBitcoin[endereco] < raiz->saldo)
    {
        raiz->esq = inserirABP(raiz->esq, endereco, saldoBitcoin);
    }
    else
    {
        raiz->dir = inserirABP(raiz->dir, endereco, saldoBitcoin);
    }
    return raiz;
}

void imprimirABP(ABP *raiz)
{ // função que imprime a árvore binária de busca em ordems

    if (raiz != NULL)
    {
        imprimirABP(raiz->esq);
        printf("Endereço: %u - Saldo: %u\n", raiz->endereco, raiz->saldo);
        imprimirABP(raiz->dir);
    }
}

void excluirArquivos()
{

    int exclui = 1;
    printf("0 - NAO EXCLUIR ARQUIVOS ANTERIORES\n1 - EXCLUIR ARQUIVOS ANTERIORES\n");
    scanf("%d", &exclui);

    if (exclui)
    {
        remove("blockchain.bin");
    }
}

int main()
{

    excluirArquivos();

    BlocoNaoMinerado blocoAMinerar;
    BlocoMinerado blocoMinerado;
    MTRand randNumber = seedRand(1234567); // objeto gerador com semente 1234567
    unsigned char guarda_hash[SHA256_DIGEST_LENGTH];
    unsigned int saldoBitcoin[256];
    memset(saldoBitcoin, 0, sizeof(unsigned int) * 256);
    ABP *raiz = NULL;

    int op = 0;
    printf("\nMENU: \n1 - Minerar \n2 - Salvar Blocos \n3 - Imprimir Bloco \n4 - Imprimir Saldo
    \n5 - Endereco Com Mais Bitcoins \n6 - Imprimir em Ordem Crescente");
    scanf("%d", &op);
    switch (op != 0)
    {
    case 1:
        // pedir ao usuario quantos blocos deseja minerar
        int qtdBlocos;
        printf("Quantos blocos deseja minerar? ");
        scanf("%d", &qtdBlocos);

        BlocoMinerado *vetorBlocosMinerados = (BlocoMinerado *)malloc(qtdBlocos * sizeof(BlocoMinerado)); // aloca espaço para o vetor de blocos minerados

        for (int num_bloco = 0; num_bloco < qtdBlocos; num_bloco++)
        {

            blocoAMinerar.numero = num_bloco;
            blocoAMinerar.nonce = 0;

            if (num_bloco == 0)
            {
                memset(blocoAMinerar.hashAnterior, 0, SHA256_DIGEST_LENGTH);
            }
            else
            {
                memcpy(blocoAMinerar.hashAnterior, guarda_hash, SHA256_DIGEST_LENGTH);
            }

            unsigned char qtdTransacoes = (unsigned char)(1 + (genRandLong(&randNumber) % 61)); // gera aleatorio de 1 a 61
            memset(blocoAMinerar.data, 0, sizeof(blocoAMinerar.data));

            for (int i = 0; i < qtdTransacoes; i += 3)
            {
                blocoAMinerar.data[i] = (unsigned char)genRandLong(&randNumber) % 256;          // gera aleatorio de 0 a 255
                blocoAMinerar.data[i + 1] = (unsigned char)genRandLong(&randNumber) % 256;      // gera aleatorio de 0 a 255
                blocoAMinerar.data[i + 2] = (unsigned char)(1 + genRandLong(&randNumber) % 51); // gera aleatorio de 1 a 50

                if (saldoBitcoin[blocoAMinerar.data[i]] >= blocoAMinerar.data[i + 2])
                {
                    saldoBitcoin[blocoAMinerar.data[i]] -= blocoAMinerar.data[i + 2];
                }
                else
                {
                    saldoBitcoin[blocoAMinerar.data[i]] = 0;
                }
                saldoBitcoin[blocoAMinerar.data[i + 1]] += blocoAMinerar.data[i + 2];
            }

            minerarBloco(&blocoAMinerar, &blocoMinerado);

            memcpy(guarda_hash, blocoMinerado.hash, SHA256_DIGEST_LENGTH);

            memcpy((vetorBlocosMinerados + num_bloco), &blocoMinerado, sizeof(BlocoMinerado));
        }
        break;

    case 2:
        salvarBlocoMinerado(vetorBlocosMinerados, qtdBlocos);
        break;

    case 3:
        int numBloco;
        printf("Insira o bloco que deseja imprimir: ");
        scanf("%d", &numBloco);
        BlocoMinerado blocoImprimir = buscaBloco(numBloco);
        imprimirBloco(&blocoImprimir);
        break;

    case 4:
        // impressao do saldo de um determinado endereco

    case 5:
        enderecoMaisBitcoin(saldoBitcoin);
        break;

    case 6:
        for (unsigned int i = 0; i < 256; i++)
        {
            raiz = inserirABP(raiz, i, saldoBitcoin);
        }
        imprimirABP(raiz);
        break;

    default:
        printf("Insira uma opcao valida!");
        break;
    }

    // imprimirSaldos(saldoBitcoin); //imprime todos os saldos

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

===============================> CONSULTAS QUE DEVEM SER SUPORTADAS PELO CÓDIGO FONTE
Após a mineração, O programa simulador em C (ambiente gcc/linux) deverá suportar os seguintes consultas LENDO OS DADOS DIRETAMENTE DO ARQUIVO PRODUZIDO:

1.Dado o número do bloco imprimir todos seus dados, incluindo o hash válido no formato hexadecimal.

2.Quantos bitcoins um dado endereço tem?

3.Qual o endereço tem mais bitcoins?

4.Listar endereços com respectivas quantidades de bitcoins em ordem crescente.

RESTRIÇÕES.
. Para dúvidas cujas respostas não se encontrem neste enunciado, a equipe deverá ter iniciativa para tomar suas próprias decisões (velando sempre pela eficiência do código) e sujeitando tais
decisões às exigências mínimas do enunciado. Neste caso, um breve relatório deverá ser elaborado para explicar as tomadas de decisões do projeto.
 Permanencendo as dúvidas, o professor deverá ser consultado.
*/
