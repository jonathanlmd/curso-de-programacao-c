#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "myconio.h"

const char USUARIO[] = "monge";
const char SENHA[] = "123456";

typedef struct ataques{
    char nome[10];
    char tipo[5];
    int danobasico;
    int powerpoints;
    char efeito[10];
}ataque;

typedef struct pokemons{
    char nome[20];
    int codp;
    char tipop[2][10];
    char especie[20];
    char classe[20];
    int pontosdevida;
    ataque *atk;
    //struct pokemons *evolucao;
    char evolucao[50];
}pokemon;

typedef struct itens{
    char nome[20];
    char descricao[200];
}item;

typedef struct treinadores{
    char nome[15];
    char senha[10];
    int codt;
    pokemon meuspokemons[166];
    int qntdpokemons[166];
    item meusitens[50];
}treinador;


pokemon* matrizdepokemons[26][35] = {0};
treinador* vetordetreinadores[50] = {NULL};

void printPokemon(pokemon* poke){
    if(poke == NULL) return;
    printf("Espécie: %s\nCódigo: %d\nEvolução: %s\nTipoA: %s\nTipoB: %s\nAtk: %d\nPV: %d\n",
        poke->especie,
        poke->codp,
        poke->evolucao,
        poke->tipop[0],
        poke->tipop[1],
        poke->atk,
        poke->pontosdevida
    );
}

char** lerLinha(FILE* arquivo, int* tam){
    int pos[100];
    int indice = -1;
    char linha[100]={0};

    if(!fgets(linha, 100, arquivo)) return NULL;
    for (int i = 0; i < strlen(linha); i++){
        if(linha[i] == ';') pos[++indice] = i;
    }

    char** buffer = (char**)calloc(indice+1,sizeof(char*));

    buffer[0] = (char*)calloc(pos[0], sizeof(char));
    strncpy(buffer[0],linha,pos[0]);

    for (int i = 1; i <= indice; i++){
        buffer[i] = (char*)calloc(pos[i]-pos[i-1]+1, sizeof(char));
        strncpy(buffer[i],linha+(pos[i-1]+1),pos[i]-pos[i-1]-1);
    }
    buffer[indice+1] = (char*)calloc(strlen(linha)-pos[indice]-1, sizeof(char));
    strncpy(buffer[indice+1],linha+(pos[indice]+1),strlen(linha)-pos[indice]-2);

    *tam = indice+2;
    return buffer;
}

void carregarPokemons(FILE* arquivo){
    pokemon *novopokemon = NULL;
    int tam;
    char** linha = lerLinha(arquivo, &tam);
    while(linha){
        novopokemon = (pokemon*)calloc(1,sizeof(pokemon));
        strcpy(novopokemon->especie,linha[0]);
        char codtemp[3] = {0};
        strcpy(codtemp,linha[1]);
        novopokemon->codp=atoi(codtemp);
        strcpy(novopokemon->tipop[0],linha[2]);
        strcpy(novopokemon->tipop[1],linha[3]);
        strcpy(novopokemon->evolucao,linha[4]);
        int j;
        for (j = 0; j < 35 && matrizdepokemons[novopokemon->especie[0]-65][j] != NULL ; j++);
        if(j<35){
            matrizdepokemons[novopokemon->especie[0]-65][j] = novopokemon;
        }else{
            fprintf(stderr, "Erro ao gravar dados, vetor cheio\n");
            fclose(arquivo);
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < tam; i++) free(linha[i]);
        free(linha);
        tam = 0;
        linha = lerLinha(arquivo, &tam);
    }
}

void carregarTreinadores(FILE* arquivo){
    treinador *novotreinador = NULL;
    int tam;
    char** linha = lerLinha(arquivo, &tam);
    while(linha){
        novotreinador = (treinador*)calloc(1,sizeof(treinador));
        strcpy(novotreinador->nome,linha[0]);
        strcpy(novotreinador->senha,linha[1]);
        char codtemp[5] = {0};
        strcpy(codtemp,linha[2]);
        novotreinador->codt=atoi(codtemp);

        int j;
        for (j = 0; j < 50 && vetordetreinadores[j] != NULL ; j++);
        if(j<50){
            vetordetreinadores[j] = novotreinador;
        }else{
            fprintf(stderr, "Erro ao gravar dados, vetor cheio\n");
            fclose(arquivo);
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < tam; i++) free(linha[i]);
        free(linha);
        tam = 0;
        linha = lerLinha(arquivo, &tam);
    }
}

void carregarDados(){
    FILE* arquivo;

    arquivo = fopen("pokemons.csv", "r");
    if(arquivo == NULL) {
        fprintf(stderr, "Erro ao abrir arquivo de dados\n");
        exit(EXIT_FAILURE);
    }

    carregarPokemons(arquivo);

    fclose(arquivo);

    arquivo = fopen("treinadores.csv", "r");
    if(arquivo == NULL) {
        fprintf(stderr, "Erro ao abrir arquivo de dados\n");
        exit(EXIT_FAILURE);
    }

    carregarTreinadores(arquivo);

    fclose(arquivo);
}

void preencher(){
    for (int i = 0; i < 26; i++)
    {
        for (int j = 0; j < 100; j++)
        {
            pokemon *novo = (pokemon *) malloc(sizeof(pokemon));
            matrizdepokemons[i][j] = novo;
            matrizdepokemons[i][j]->codp = (i+1)*(j+1);
        }
    }
}

void listarPokemons(){
    for (int i = 0; i < 26; i++)
    {
        int j=0;
        while (matrizdepokemons[i][j] != NULL){
            printf("Codigo: %d \n", matrizdepokemons[i][j]->codp);
            j++;
        }
    }
}

void listarTreinadores(){
    for (int i = 0; i < 26; i++)
    {
        printf("Codigo: %s \n", vetordetreinadores[i]->nome);
    }
}

int login(){
    clrscr();
    char ch = ' ';
    char login[15] = {0};
    char senha[10] = {0};
    int indicedelinha = 8;
    int tamanhodasenha = 0;
    printf("Login: ");
    scanf("%s", &login); getch();
    printf("Senha: ");
    while (ch != '\n'){
        ch = getch();
        if(ch == 127){
            if(indicedelinha > 8){
                indicedelinha--;
                gotoxy(indicedelinha,2);
                putchar(' ');
                gotoxy(indicedelinha,2);
            }
            if(tamanhodasenha<10){
                senha[tamanhodasenha-1]='\000';
            }
            if(tamanhodasenha>0){
                tamanhodasenha--;
            }
        }else if(ch != '\n'){
            putchar('*');
            indicedelinha++;
            if(tamanhodasenha<10){
                senha[tamanhodasenha] = ch;
            }
            tamanhodasenha++;
        }
    }
    putchar('\n');
    if(strcmp(login,USUARIO) || strncmp(senha,SENHA, strlen(senha))){
        printf("Usuario ou senha inválidos\nPressione qualquer tecla\n");
        getch();
        return 0;
    }else{
        return 1;
    }   
}

pokemon* buscarPokemon(char *nomepokemon){
    if(nomepokemon[0] >= 97 && nomepokemon[0] < 123){
        nomepokemon[0] = (char)(nomepokemon[0])-32;
    }
    if(nomepokemon[0] < 65 && nomepokemon[0] >= 91){
        printf("Não encontrado\n");
        return NULL;
    }
    for (int i = 0; i < 35; i++){
        if(matrizdepokemons[(int)nomepokemon[0]-65][i] != NULL &&
            !strcmp(nomepokemon, matrizdepokemons[(int)nomepokemon[0]-65][i]->especie)){
            return matrizdepokemons[(int)nomepokemon[0]-65][i];
        }        
    }
    printf("Não encontrado\n");
    printf("Pressione qualquer tecla para voltar");
    return NULL;
}

void pokedex(){
    clrscr();
    char opc = ' ';
    char sair ='f';
    char temp[15] = {0};
    while (sair == 'f')
    {
        clrscr();
        printf("Pokedex\n");
        printf("1 - Listar pokemons\n");
        printf("2 - Buscar pokemon\n");
        printf("3 - Ver meus pokemons\n");
        printf("4 - Meus Itens\n");
        printf("5 - Trocar pokemon\n");
        printf("6 - Sair\n");
        opc = getchar(); getch();
        switch (opc){
            case '1':
                listarPokemons();
                printf("Pressione qualquer tecla para voltar ");
                getch();
                break;
            case '2':
                printf("Digite a especie: ");
                scanf("%s", &temp); getch();
                printPokemon(buscarPokemon(temp));
                getch();
                break;
            case '6':
                sair = 't';
                break;
            default:
                break;
        }
    }
}

void menu(){
    clrscr();
    char sair = 'f';
    char opc;
    //preencher();
    while (sair == 'f')
    {
        clrscr();
        printf("Menu\n");
        printf("1 - Pokedex\n");
        printf("2 - Ver treinadores\n");
        printf("3 - Batalhar\n");
        printf("4 - Sair\n");
        printf("5 - Encerrar\n");
        opc = getchar(); getch();
        switch (opc){
            case '1':
                pokedex();
                break;
            case '2':
                listarTreinadores();
                getch();
                break;
            case '4':
                printf("Saindo....\n");
                printf("Pressione qualquer tecla\n");
                getch();
                sair = 't';
                break;
            case '5':
                printf("Até a próxima...\nPressione qualquer tecla\n");
                getch();
                exit(EXIT_SUCCESS);
                break;
            default:
                break;
        }
    }
}

int main(int argc, char const *argv[]){   
    carregarDados(); 
    while (1){
        if(login()){
            menu();
        }
    }
    return 0;
}
