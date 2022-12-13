#include "mtwister.c"
#include "openssl/crypto.h" //arquivo de definição necessário para SHA256_DIGEST_LENGTH
#include "openssl/sha.h"    //arquivo de definição necessário função SHA256
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct BlocoNaoMinerado {
    unsigned int numero;
    unsigned int nonce;
    unsigned char data[184];
    unsigned char hashAnterior[SHA256_DIGEST_LENGTH];
}typedef BlocoNaoMinerado;

struct BlocoMinerado {
    BlocoNaoMinerado bloco;
    unsigned char hash[SHA256_DIGEST_LENGTH];
}typedef BlocoMinerado;

struct ABP {
    unsigned int saldo;
    unsigned char endereco;
    struct ABP *esq;
    struct ABP *dir;
}typedef ABP;

void minerarBlocoMenorComplex(BlocoNaoMinerado *blocoAMinerar, BlocoMinerado *blocoMinerado) {

    unsigned char hash[SHA256_DIGEST_LENGTH]; // vetor que armazenará o resultado do hash. Tamanho definido pela libssl

    SHA256((unsigned char *)blocoAMinerar, sizeof(*blocoAMinerar), hash); // gera hash do bloco

    while(hash[0] != 0 || hash[1] != 0) { // enquanto o hash não começar com 2 zeros
        if(blocoAMinerar->nonce < 4294967295) {
            blocoAMinerar->nonce++;                                               // incrementa o nonce
            SHA256((unsigned char *)blocoAMinerar, sizeof(*blocoAMinerar), hash); // gera hash do bloco
        }
    }
    // copia o bloco para o bloco minerado
    blocoMinerado->bloco = *blocoAMinerar;
    // copia o hash para o bloco minerado
    memcpy(blocoMinerado->hash, hash, SHA256_DIGEST_LENGTH);
}

void minerarBloco(BlocoNaoMinerado *blocoAMinerar, BlocoMinerado *blocoMinerado) {

    unsigned char hash[SHA256_DIGEST_LENGTH]; // vetor que armazenará o resultado do hash. Tamanho definido pela libssl

    SHA256((unsigned char *)blocoAMinerar, sizeof(*blocoAMinerar), hash); // gera hash do bloco

    while(hash[0] != 0 || hash[1] != 0 || hash[2] != 0) { // enquanto o hash não começar com 3 zeros
        if(blocoAMinerar->nonce < 4294967295) {
            blocoAMinerar->nonce++;                                               // incrementa o nonce
            SHA256((unsigned char *)blocoAMinerar, sizeof(*blocoAMinerar), hash); // gera hash do bloco
        }
        else{
            printf("Nonce chegou ao limite. Abortando operação.\n");
            // chama outra função com menos complexidade
            minerarBlocoMenorComplex(blocoAMinerar, blocoMinerado);
        }
    }
    // copia o bloco para o bloco minerado
    blocoMinerado->bloco = *blocoAMinerar;
    // copia o hash para o bloco minerado
    memcpy(blocoMinerado->hash, hash, SHA256_DIGEST_LENGTH);
}

void salvarBlocoMinerado(BlocoMinerado *blocoMinerado, int qtdBlocos) { // função que guarda o bloco minerado em um arquivo binario
    FILE *arquivo = fopen("blockchain.bin", "ab");
    for(int i = 0; i < qtdBlocos; i += 2) { 
        fwrite(&blocoMinerado[i], sizeof(*blocoMinerado), 2, arquivo);
    }
    fclose(arquivo);
}

void salvarBlocoMineradoTxt(BlocoMinerado *blocoMinerado, int qtdBlocos) { //funcao que guarda o bloco minerado em um arquivo txt de 2 em 2 blocos
    FILE *arquivo = fopen("blockchain.txt", "a");
    for(int i = 1; i <= qtdBlocos; i += 2) {
        fprintf(arquivo, "Bloco %u - Nonce: %u\nHash: ", blocoMinerado[i].bloco.numero, blocoMinerado[i].bloco.nonce);
        for(int j = 0; j < SHA256_DIGEST_LENGTH; j++) {
            fprintf(arquivo, "%02x", blocoMinerado[i].hash[j]);
        }
        fprintf(arquivo, "\nHash anterior: ");
        for(int j = 0; j < SHA256_DIGEST_LENGTH; j++) {
            fprintf(arquivo, "%02x", blocoMinerado[i].bloco.hashAnterior[j]);
        }
        fprintf(arquivo, "\n\n");
        if(i + 1 <= qtdBlocos) {
            fprintf(arquivo, "Bloco %u - Nonce: %u\nHash: ", blocoMinerado[i + 1].bloco.numero, blocoMinerado[i + 1].bloco.nonce);
            for(int j = 0; j < SHA256_DIGEST_LENGTH; j++) {
                fprintf(arquivo, "%02x", blocoMinerado[i + 1].hash[j]);
            }
            fprintf(arquivo, "\nHash anterior: ");
            for(int j = 0; j < SHA256_DIGEST_LENGTH; j++) {
                fprintf(arquivo, "%02x", blocoMinerado[i + 1].bloco.hashAnterior[j]);
            }
            fprintf(arquivo, "\n\n");
        }
    }
    fclose(arquivo);
}

void salvarSaldoBitcoin(unsigned int *saldoBitcoin) {//funçao que salva vetor de saldobitcoin em arquivo binario saldobitcoin[256]
    FILE *arquivo = fopen("saldobitcoin.bin", "wb");
    fwrite(saldoBitcoin, sizeof(*saldoBitcoin), 256, arquivo);
    fclose(arquivo);
}

void salvarSaldoBitcoinTxt(unsigned int *saldoBitcoin) {//funçao que salva vetor de saldobitcoin em arquivo txt saldobitcoin[256]
    FILE *arquivo = fopen("saldobitcoin.txt", "w");
    for(int i = 0; i < 256; i++) {
        fprintf(arquivo, "Saldo do usuário %d: %u\n", i, saldoBitcoin[i]);
    }
    fclose(arquivo);
}

void pegarSaldoBitcoin(unsigned int *saldoBitcoin) {//funcao que pega as informaçoes do arquivo saldobitcoin.bin e salva no vetor saldobitcoin[256]
    FILE *arquivo = fopen("saldobitcoin.bin", "rb");
    fread(saldoBitcoin, sizeof(*saldoBitcoin), 256, arquivo);
    fclose(arquivo);
}

BlocoMinerado buscaBloco(int numeroBloco) { // função que busca um bloco no arquivo binario
    BlocoMinerado blocoMinerado;
    FILE *arquivo = fopen("blockchain.bin", "rb");
    fseek(arquivo, numeroBloco * sizeof(blocoMinerado), SEEK_SET);
    fread(&blocoMinerado, sizeof(blocoMinerado), 1, arquivo);
    fclose(arquivo);
    return blocoMinerado;
}

void imprimirBloco(BlocoMinerado *blocoMinerado) { // funçao que imprime o bloco

    printf(" Bloco %u - Nonce: %u\nHash:  -", blocoMinerado->bloco.numero, blocoMinerado->bloco.nonce);
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        printf("%02x", blocoMinerado->hash[i]);
    }

    printf("\n");

    printf("Hash anterior: ");
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        printf("%02x", blocoMinerado->bloco.hashAnterior[i]);
    }

    printf("\n");

    printf("Data: \n");
    for(int i = 0; i < 183; i += 3) {
        printf("%d ", blocoMinerado->bloco.data[i]);
        printf("%d ", blocoMinerado->bloco.data[i + 1]);
        printf("%d ", blocoMinerado->bloco.data[i + 2]);
        printf("\n");
    }
}

void imprimirHash(unsigned char *hash) { // função que imprime um hash
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");
}

void imprimirSaldos(unsigned int saldoBitcoin[256]) {
    for(int i = 1; i < 256; i++) {
        printf("Saldo da carteira %d: %u\n", i, saldoBitcoin[i]);
    }
}

unsigned int verificaSaldoBitcoin(unsigned char endereco, unsigned int saldoBitcoin[256]) { // função que verifica o saldo de uma carteira
   
    printf("Endereço %u tem %u bitcoins\n", endereco, saldoBitcoin[endereco]);
    return saldoBitcoin[endereco];
}

unsigned char enderecoMaisBitcoin(unsigned int saldoBitcoin[256]) {
    unsigned int max = saldoBitcoin[0];
    unsigned char pos = 0;
    for(int i = 1; i < 256; i++) {
        if(saldoBitcoin[i] > max) {
            max = saldoBitcoin[i];
            pos = i;
        }
    }
    printf("\nO endereco com maior saldo de BitCoin é %u, com %u BitCoins.\n", pos, max);
    return pos;
}

ABP *inserirABP(ABP *raiz, unsigned char endereco, unsigned int saldoBitcoin[256]) { // função que insere um nó na árvore binária de busca - usada para imprimir o saldo em ordem crescente

    if(raiz == NULL) {
        raiz = (ABP *)malloc(sizeof(ABP));
        raiz->endereco = endereco;
        raiz->saldo = saldoBitcoin[endereco];
        raiz->esq = NULL;
        raiz->dir = NULL;
    }
    else if(saldoBitcoin[endereco] < raiz->saldo) {
        raiz->esq = inserirABP(raiz->esq, endereco, saldoBitcoin);
    }
    else {
        raiz->dir = inserirABP(raiz->dir, endereco, saldoBitcoin);
    }
    return raiz;
}

void imprimirABP(ABP *raiz) { // função que imprime a árvore binária de busca em ordems

    if(raiz != NULL) {
        imprimirABP(raiz->esq);
        printf("Endereço: %u - Saldo: %u\n", raiz->endereco, raiz->saldo);
        imprimirABP(raiz->dir);
    }
}

void excluirArquivoBlockchain() {
    int exclui = 1;
    printf("0 - NAO EXCLUIR ARQUIVOS ANTERIORES\n1 - EXCLUIR ARQUIVOS ANTERIORES\n");
    scanf("%d", &exclui);

    if(exclui) {
        remove("blockchain.bin");
        remove("blockchain.txt");
    }
}

void excluirArquivoSaldo() {
    int exclui = 1;
    printf("0 - NAO EXCLUIR ARQUIVO SALDO\n1 - EXCLUIR ARQUIVO SALDO\n");
    scanf("%d", &exclui);

    if(exclui) {
        remove("saldobitcoin.bin");
        remove("saldobitcoin.txt"); 
    }
}

int main() {
    BlocoNaoMinerado blocoAMinerar;
    BlocoMinerado blocoMinerado;
    MTRand randNumber = seedRand(1234567); // objeto gerador com semente 1234567
    unsigned char guarda_hash[SHA256_DIGEST_LENGTH];
    unsigned int saldoBitcoin[256];
    memset(saldoBitcoin, 0, sizeof(unsigned int) * 256);
    ABP *raiz = NULL;

    printf("Bem vindo ao simulador de blockchain!\n");
    printf("Lembre-se que voce deve salvar os blocos minerados para que eles sejam salvos na blockchain!\n");
    printf("Se voce ja minerou alguma vez, nao esqueca de salvar o arquivo blockchain.bin e .txt para nao perder os blocos minerados anteriormente!\n");
    printf("Mas ao minerar novamente, exclua o arquivo blockchain.bin!\n");

    int op = 0;
    while (op != 9) {
        printf("\n\nMENU: \n");
        printf("1 - Minerar e Salvar Blocos \n");
        printf("2 - Imprimir Bloco \n");
        printf("3 - Imprimir Saldo \n");
        printf("4 - Endereco Com Mais Bitcoins \n");
        printf("5 - Imprimir em Ordem Crescente \n");
        printf("6 - Imprimir todos os saldos \n");
        printf("7 - Salvar saldos em arquivo \n");
        printf("8 - Carregar saldos de arquivo \n");
        printf("9 - Sair \n");
        scanf("%d", &op);
        switch(op) {
        case 1:
            // pedir ao usuario quantos blocos deseja minerar
            excluirArquivoBlockchain();

            int qtdBlocos;
            printf("Quantos blocos deseja minerar? ");
            scanf("%d", &qtdBlocos);
            printf("Minerando...\n");

            BlocoMinerado *vetorBlocosMinerados = (BlocoMinerado *)malloc(((qtdBlocos + 1) * sizeof(BlocoMinerado))); // aloca espaço para o vetor de blocos minerados

            for(int num_bloco = 1; num_bloco <= qtdBlocos; num_bloco++) {

                blocoAMinerar.numero = num_bloco;
                blocoAMinerar.nonce = 0;
                //a

                if(num_bloco == 1) {
                    memset(blocoAMinerar.hashAnterior, 0, SHA256_DIGEST_LENGTH);
                }
                else {
                    memcpy(blocoAMinerar.hashAnterior, guarda_hash, SHA256_DIGEST_LENGTH);
                }

                
                unsigned char qtdTransacoes = (unsigned char)(1 + (genRandLong(&randNumber) % 61)); // gera aleatorio de 1 a 61
                memset(blocoAMinerar.data, 0, sizeof(blocoAMinerar.data));

                for(int i = 0; i < qtdTransacoes; i++) {
                    blocoAMinerar.data[i * 3] = (unsigned char)genRandLong(&randNumber) % 256;          // gera aleatorio de 0 a 255
                    blocoAMinerar.data[i * 3 + 1] = (unsigned char)genRandLong(&randNumber) % 256;      // gera aleatorio de 0 a 255
                    blocoAMinerar.data[i * 3 + 2] = (unsigned char)(1 + genRandLong(&randNumber) % 50); // gera aleatorio de 1 a 50

                    if(saldoBitcoin[blocoAMinerar.data[i]] >= blocoAMinerar.data[i + 2]) {
                        saldoBitcoin[blocoAMinerar.data[i]] -= blocoAMinerar.data[i + 2];
                    }
                    else {
                        saldoBitcoin[blocoAMinerar.data[i]] = 0;
                    }

                    saldoBitcoin[blocoAMinerar.data[i + 1]] += blocoAMinerar.data[i + 2];
                }
                printf("Bloco %d\n", num_bloco);
                minerarBloco(&blocoAMinerar, &blocoMinerado);

                memcpy(guarda_hash, blocoMinerado.hash, SHA256_DIGEST_LENGTH);

                vetorBlocosMinerados[num_bloco] = blocoMinerado;
            }
            salvarBlocoMinerado(vetorBlocosMinerados, qtdBlocos);
            salvarBlocoMineradoTxt(vetorBlocosMinerados, qtdBlocos);
            printf("Blocos minerados com sucesso!\n");
            break;

        case 2:
            int numBloco;
            printf("Insira o bloco que deseja imprimir: \n");
            scanf("%d", &numBloco);
            BlocoMinerado blocoImprimir = buscaBloco(numBloco);
            imprimirBloco(&blocoImprimir);
            break;

        case 3:
            // impressao do saldo de um determinado endereco
            unsigned char endVerificacao = 0;
            printf("Insira o endereco que deseja verificar o saldo: \n");
            scanf("%hhu", &endVerificacao);
            verificaSaldoBitcoin(endVerificacao, saldoBitcoin);
            break;

        case 4:
            enderecoMaisBitcoin(saldoBitcoin);
            break;

        case 5:
            for(unsigned int i = 0; i < 256; i++) {
                raiz = inserirABP(raiz, i, saldoBitcoin);
            }
            imprimirABP(raiz);
            break;

        case 6:
            imprimirSaldos(saldoBitcoin);
            break;

        case 7:
            excluirArquivoSaldo();
            salvarSaldoBitcoin(saldoBitcoin);
            salvarSaldoBitcoinTxt(saldoBitcoin);
            printf("Saldo salvo com sucesso!\n");
            break;
        case 8:
            pegarSaldoBitcoin(saldoBitcoin);
            printf("Saldo carregado com sucesso!\n");
            break;

        default:
            printf("Insira uma opcao valida!");
            break;
        }
    }
    return 0;
}
