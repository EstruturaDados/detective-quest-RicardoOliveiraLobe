#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Definicao do tamanho da tabela Hash
#define TAMANHO_HASH 10

// =============================================================
// ESTRUTURA 1: TABELA HASH (BANCO DE DADOS DE SUSPEITOS)
// =============================================================

typedef struct HashNode {
    char pista[100];        // CHAVE
    char suspeito[50];      // VALOR
    struct HashNode* proximo; 
} HashNode;

HashNode* tabelaVerdade[TAMANHO_HASH];

// Funcao Hash Ponderada
int funcaoHash(char* chave) {
    int hash = 0;
    for (int i = 0; chave[i] != '\0'; i++) {
        hash += chave[i] * (i + 1);
    }
    return hash % TAMANHO_HASH;
}

void inicializarTabela() {
    for (int i = 0; i < TAMANHO_HASH; i++) tabelaVerdade[i] = NULL;
}

void inserirNaHash(char* pista, char* suspeito) {
    int indice = funcaoHash(pista);
    
    HashNode* novo = (HashNode*) malloc(sizeof(HashNode));
    if (novo == NULL) exit(1);
    
    strcpy(novo->pista, pista);
    strcpy(novo->suspeito, suspeito);
    
    novo->proximo = tabelaVerdade[indice];
    tabelaVerdade[indice] = novo;
}

char* encontrarSuspeito(char* pista) {
    int indice = funcaoHash(pista);
    HashNode* atual = tabelaVerdade[indice];
    
    while (atual != NULL) {
        if (strcmp(atual->pista, pista) == 0) {
            return atual->suspeito;
        }
        atual = atual->proximo;
    }
    return NULL;
}

// =============================================================
// ESTRUTURA 2: BST (INVENTARIO DO JOGADOR)
// =============================================================

typedef struct PistaNode {
    char conteudo[100];
    struct PistaNode *esquerda;
    struct PistaNode *direita;
} PistaNode;

PistaNode* criarPistaNode(char* conteudo) {
    PistaNode* novo = (PistaNode*) malloc(sizeof(PistaNode));
    if (novo == NULL) exit(1);
    strcpy(novo->conteudo, conteudo);
    novo->esquerda = NULL;
    novo->direita = NULL;
    return novo;
}

PistaNode* inserirPista(PistaNode* raiz, char* conteudo) {
    if (raiz == NULL) return criarPistaNode(conteudo);

    if (strcmp(conteudo, raiz->conteudo) < 0)
        raiz->esquerda = inserirPista(raiz->esquerda, conteudo);
    else if (strcmp(conteudo, raiz->conteudo) > 0)
        raiz->direita = inserirPista(raiz->direita, conteudo);
    
    return raiz;
}

void exibirPistas(PistaNode* raiz) {
    if (raiz != NULL) {
        exibirPistas(raiz->esquerda);
        printf("- %s\n", raiz->conteudo);
        exibirPistas(raiz->direita);
    }
}

// =============================================================
// ESTRUTURA 3: MAPA (ARVORE BINARIA)
// =============================================================

typedef struct Sala {
    char nome[50];
    char pista[100];
    struct Sala* esquerda;
    struct Sala* direita;
} Sala;

Sala* criarSala(char* nome, char* pistaOpcional) {
    Sala* nova = (Sala*) malloc(sizeof(Sala));
    if (nova == NULL) exit(1);
    
    strcpy(nova->nome, nome);
    if (pistaOpcional) strcpy(nova->pista, pistaOpcional);
    else strcpy(nova->pista, "");
    
    nova->esquerda = NULL;
    nova->direita = NULL;
    return nova;
}

// =============================================================
// LOGICA DE JULGAMENTO
// =============================================================

void contarProvas(PistaNode* raizBST, char* acusado, int* contador) {
    if (raizBST == NULL) return;

    char* suspeitoReal = encontrarSuspeito(raizBST->conteudo);
    
    // Compara ignorando maiusculas/minusculas (strcasecmp no Linux, stricmp no Windows)
    // Vamos fazer manual para garantir compatibilidade total
    if (suspeitoReal != NULL) {
        if (strcasecmp(suspeitoReal, acusado) == 0) {
            (*contador)++;
        }
    }

    contarProvas(raizBST->esquerda, acusado, contador);
    contarProvas(raizBST->direita, acusado, contador);
}

void verificarSuspeitoFinal(PistaNode* inventario) {
    char acusado[50];
    int provas = 0;

    printf("\n=== SESSAO DE JULGAMENTO ===\n");
    
    if (inventario == NULL) {
        printf("ATENCAO: Voce nao coletou nenhuma pista!\n");
        printf("Sem evidencias, sua acusacao sera apenas um chute.\n");
    } else {
        printf("Baseado nas pistas que voce coletou:\n");
        exibirPistas(inventario);
    }
    
    printf("\nQuem voce acusa de ser o culpado?\n> ");
    // Limpar buffer antes de ler string com espacos
    scanf(" %[^\n]", acusado);

    contarProvas(inventario, acusado, &provas);

    printf("\nAnalisando evidencias...\n");
    if (provas >= 2) {
        printf(">>> SUCESSO! <<<\n");
        printf("Voce apresentou %d provas validas contra %s.\n", provas, acusado);
        printf("O culpado foi preso gracas a sua logica!\n");
    } else {
        printf(">>> FRACASSO <<<\n");
        printf("Voce tem apenas %d prova(s) valida(s) contra %s.\n", provas, acusado);
        printf("O juri exige pelo menos 2 provas concretas. O culpado escapou!\n");
    }
}

// =============================================================
// MOTOR DO JOGO
// =============================================================

void explorarSalas(Sala* mapa) {
    Sala* atual = mapa;
    PistaNode* inventario = NULL;
    char opcao;

    printf("\n--- INICIO DA INVESTIGACAO ---\n");

    while (atual != NULL) {
        printf("\n================================================\n");
        printf("[LOCAL]: %s\n", atual->nome);

        if (strlen(atual->pista) > 0) {
            printf("[!] PISTA ENCONTRADA: \"%s\"\n", atual->pista);
            inventario = inserirPista(inventario, atual->pista);
            strcpy(atual->pista, ""); 
        }
        printf("================================================\n");

        printf("OPCOES:\n");
        printf("[e] Esquerda (%s)\n", atual->esquerda ? "Disponivel" : "Fechado");
        printf("[d] Direita  (%s)\n", atual->direita ? "Disponivel" : "Fechado");
        printf("[i] Ver Inventario\n");
        printf("[s] Encerrar e Julgar\n");
        printf("> ");
        scanf(" %c", &opcao);

        // Converte para minusculo para aceitar E ou e
        opcao = tolower(opcao);

        if (opcao == 'e') {
            if (atual->esquerda) atual = atual->esquerda;
            else printf("\n>> Porta trancada.\n");
        }
        else if (opcao == 'd') {
            if (atual->direita) atual = atual->direita;
            else printf("\n>> Porta trancada.\n");
        }
        else if (opcao == 'i') {
            printf("\n--- INVENTARIO (Ordem Alfabetica) ---\n");
            if (inventario == NULL) printf("(Vazio)\n");
            else exibirPistas(inventario);
        }
        else if (opcao == 's') {
            verificarSuspeitoFinal(inventario);
            break; 
        }
        else {
            printf("\nOpcao invalida.\n");
        }
    }
}

// =============================================================
// MAIN
// =============================================================

int main() {
    // 1. SETUP DA HASH
    inicializarTabela();
    
    // Cadastro da "Verdade" (Pista -> Culpado)
    inserirNaHash("Relogio quebrado", "Mordomo");
    inserirNaHash("Luvas brancas sujas", "Mordomo");
    
    inserirNaHash("Pa com lama", "Jardineiro");
    inserirNaHash("Botas sujas", "Jardineiro");
    
    inserirNaHash("Faca torta", "Cozinheira");

    // 2. SETUP DO MAPA
    Sala* mapa = criarSala("Hall de Entrada", NULL);

    // Esquerda (Jardineiro)
    mapa->esquerda = criarSala("Jardim de Inverno", "Pa com lama");
    mapa->esquerda->esquerda = criarSala("Estufa", "Botas sujas");

    // Direita (Mordomo e Cozinheira)
    mapa->direita = criarSala("Sala de Jantar", "Relogio quebrado");
    mapa->direita->esquerda = criarSala("Cozinha", "Faca torta");
    mapa->direita->direita = criarSala("Quarto dos Empregados", "Luvas brancas sujas");

    // 3. INICIO
    printf("OBJETIVO: Colete 2 pistas do mesmo suspeito para prende-lo!\n");
    explorarSalas(mapa);

    return 0;
}