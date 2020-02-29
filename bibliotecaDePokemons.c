#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// Biblioteca feita para padronizar nomes de funções com windows
// Apenas utilizavel no linux
#include "myconio.h" // No windows, usar <conio.h>

//definição da estrutura de um ataque
typedef struct ataques{
    char nome[10];
    char tipo[5];
    int danobasico;
    int powerpoints;
    char efeito[10];
}ataque;

//definição da estrutura de um pokemon
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

//definição da estrutura de um item
typedef struct itens{
    int codi;
    char nome[20];
    char descricao[200];
}item;

//definição da estrutura de um treinador
typedef struct treinadores{
    char nome[15];
    char senha[10];
    int codt;
    pokemon meuspokemons[166];
    int qntdpokemons[166];
    item meusitens[50];
}treinador;

//Matriz para armazenar todos os pokemons carregados
pokemon* matrizdepokemons[26][35] = {0};

//Vetor para armazenar todos os treinadores carregados
treinador* vetordetreinadores[50] = {NULL};

//Vetor para armazenar todos os itens carregados
item* vetordeitens[15] = {NULL};

//Função para imprimir um pokemon na tela com uma estrutura fixa
//Recebe um ponteiro para um pokemon a ser impresso na tela
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

//Função para imprimir um treinador na tela com uma estrutura fixa
//Recebe um ponteiro para um treinador a ser impresso na tela
void printTreinador(treinador* t){
    if(t == NULL) return;
    printf("Nome: %s\nCódigo: %d\n", t->nome,t->codt);
}

/*
 * Função: lerLinha
 *       Lê uma linha de um arquivo csv delimitado por ;
 *       e retornar um array com o valor de cada campo do csv
 * 
 * arquivo (FILE*): ponteiro para o arquivo na linha a ser lida
 * tam (int*): ponteiro para armazenar o tamanho do vetor de saida
 * 
 * retorno (char**): ponteiro para vetor de strings
 * 
 * Obs.: O delimitador de separação é o ; e o tamanho máximo 
 *       de uma linha deve ser 100 caracteres
*/
char** lerLinha(FILE* arquivo, int* tam){
    int pos[100]; //vetor para armazenar o índice de cada delimitador encontrado
    int indice = -1; //índice final do vetor de delimitadores
    char linha[100]={0}; //vetor para armazenar a uma linha inteira

    //fgets lê a linha até um "\n" e retorna a linha lida ou null
    //caso encontr EOF ou uma linha em branco
    if(!fgets(linha, 100, arquivo)) return NULL; //Se encontrar EOF ou linha em branco retorna

    //Guarda a posição de cada delimitador ; em pos[]
    for (int i = 0; i < strlen(linha); i++){
        if(linha[i] == ';') pos[++indice] = i;
    }

    // Aloca espaço para ponteiros de strings com a quantidada de campos do csv
    char** vetordecampos = (char**)calloc(indice+1,sizeof(char*));

    //Aloca e armazena o primeiro campo da linha
    vetordecampos[0] = (char*)calloc(pos[0], sizeof(char));
    strncpy(vetordecampos[0],linha,pos[0]);

    //Aloca e armazena os campos intermediários da linha
    for (int i = 1; i <= indice; i++){
        vetordecampos[i] = (char*)calloc(pos[i]-pos[i-1]+1, sizeof(char));
        strncpy(vetordecampos[i],linha+(pos[i-1]+1),pos[i]-pos[i-1]-1);
    }

    //Aloca e armazena o último campo da linha
    vetordecampos[indice+1] = (char*)calloc(strlen(linha)-pos[indice]-1, sizeof(char));
    strncpy(vetordecampos[indice+1],linha+(pos[indice]+1),strlen(linha)-pos[indice]-2);

    //Armazena o tamanho do vetor em tam
    *tam = indice+2;
    return vetordecampos; //Retorna o vetor de strings
}

/*
 * Função: carregarPokemons
 *       Lê os pokemons armazenados em um csv linha por linha
 *       com a função lerLinhas e armazena cada um em na matrizdepokemons[]
 * 
 * arquivo (FILE*): ponteiro para o arquivo a ser lido
 * 
*/
void carregarPokemons(FILE* arquivo){
    pokemon *novopokemon = NULL; //Ponteiro par novo pokemon a ser lido
    int tam;    //Inteiro para guardar numero de campos da linha
    char** linha = lerLinha(arquivo, &tam); //Lê a linha do arquivo

    // Enquanto ouver uma linha a ser lida, executa o carregamento
    while(linha){
        //Aloca espaço para o novo pokemon
        novopokemon = (pokemon*)calloc(1,sizeof(pokemon)); 
        //copia string de cada campo nos atributos do pokemon
        strcpy(novopokemon->especie,linha[0]);
        char codtemp[3] = {0};
        strcpy(codtemp,linha[1]);
        novopokemon->codp=atoi(codtemp); //atoi converte uma string em inteiro
        strcpy(novopokemon->tipop[0],linha[2]);
        strcpy(novopokemon->tipop[1],linha[3]);
        strcpy(novopokemon->evolucao,linha[4]);

        // Encontra a posição correta para armazenaro o pokemon na matrizdepokemons[]
        // e grava o pokemon na matriz se ouver espaço
        int j;
        for (j = 0; j < 35 && matrizdepokemons[novopokemon->especie[0]-65][j] != NULL ; j++);
        if(j<35){
            matrizdepokemons[novopokemon->especie[0]-65][j] = novopokemon;
        }else{
            // Em caso de erro encerra o programa
            fprintf(stderr, "Erro ao gravar dados, vetor cheio\n");
            fclose(arquivo);
            exit(EXIT_FAILURE);
        }

        //Libera o espaço alocado em cada posição da linha
        for (int i = 0; i < tam; i++) free(linha[i]);

        //Libera o espaço alocado para o vetor linha
        free(linha);
        //Zera o tamanho dos campos e tenta ler uma nova linha
        tam = 0;
        linha = lerLinha(arquivo, &tam);
    }
}

/*
 * Função: carregarTreinadores
 *       Lê os treinadores armazenados em um csv linha por linha
 *       com a função lerLinhas e armazena cada um em no vetordetreinadores[]
 * 
 * arquivo (FILE*): ponteiro para o arquivo a ser lido
 * 
*/
void carregarTreinadores(FILE* arquivo){
    treinador *novotreinador = NULL; //Ponteiro par novo treinador a ser lido
    int tam; //Inteiro para guardar numero de campos da linha
    char** linha = lerLinha(arquivo, &tam); //Lê a linha do arquivo

    // Enquanto ouver uma linha a ser lida, executa o carregamento
    while(linha){
        //Aloca espaço para o novo treinador
        novotreinador = (treinador*)calloc(1,sizeof(treinador));

        //copia string de cada campo nos atributos do pokemon
        strcpy(novotreinador->nome,linha[0]);
        strcpy(novotreinador->senha,linha[1]);
        char codtemp[5] = {0};
        strcpy(codtemp,linha[2]);
        novotreinador->codt=atoi(codtemp); //atoi converte uma string em inteiro

        // Encontra a posição correta para armazenaro o treinador no vetordetreinadores[]
        // e grava o treinador na vetor se ouver espaço
        int j;
        for (j = 0; j < 50 && vetordetreinadores[j] != NULL ; j++);
        if(j<50){
            vetordetreinadores[j] = novotreinador;
        }else{
            // Em caso de erro encerra o programa
            fprintf(stderr, "Erro ao gravar dados, vetor cheio\n");
            fclose(arquivo);
            exit(EXIT_FAILURE);
        }

        //Libera o espaço alocado em cada posição da linha
        for (int i = 0; i < tam; i++) free(linha[i]);
        //Libera o espaço alocado para o vetor linha
        free(linha);
        //Zera o tamanho dos campos e tentar ler uma nova linha
        tam = 0;
        linha = lerLinha(arquivo, &tam);
    }
}

/*
 * Função: carregarItens
 *       Lê os itens armazenados em um csv linha por linha
 *       com a função lerLinhas e armazena cada um em no vetordeitens[]
 * 
 * arquivo (FILE*): ponteiro para o arquivo a ser lido
 * 
*/
void carregarItens(FILE* arquivo){
    item* novoitem = NULL; //Ponteiro par novo item a ser lido
    int tam; //Inteiro para guardar numero de campos da linha
    char** linha = lerLinha(arquivo, &tam); //Lê a linha do arquivo

    // Enquanto ouver uma linha a ser lida, executa o carregamento
    while(linha){
        //Aloca espaço para o novo treinador
        novoitem = (item*)calloc(1,sizeof(item));

        //copia string de cada campo nos atributos do item
        char codtemp[5] = {0};
        strcpy(codtemp,linha[0]);
        novoitem->codi = atoi(codtemp); //atoi converte uma string em inteiro
        strcpy(novoitem->nome,linha[1]);
        strcpy(novoitem->descricao,linha[2]);
        

        // Encontra a posição correta para armazenaro o item no vetordeitens[]
        // e grava o item na vetor se ouver espaço
        int j;
        for (j = 0; j < 15 && vetordeitens[j] != NULL ; j++);
        if(j<15){
            vetordeitens[j] = novoitem;
        }else{
            // Em caso de erro encerra o programa
            fprintf(stderr, "Erro ao gravar dados, vetor cheio\n");
            fclose(arquivo);
            exit(EXIT_FAILURE);
        }

        //Libera o espaço alocado em cada posição da linha
        for (int i = 0; i < tam; i++) free(linha[i]);
        //Libera o espaço alocado para o vetor linha
        free(linha);
        //Zera o tamanho dos campos e tentar ler uma nova linha
        tam = 0;
        linha = lerLinha(arquivo, &tam);
    }
}

/*
 * Função: carregarDados
 *       Chama os demais procedimentos de carregamento de arcodo com cada arquivo
 * 
*/
void carregarDados(){
    FILE* arquivo; //Ponteiro para arquivo

    //Tenta abrir arquivo pokemons
    arquivo = fopen("pokemons.csv", "r");
    //Em caso de erro na abertura do arquivo, fecha o programa
    if(arquivo == NULL) {
        fprintf(stderr, "Erro ao abrir arquivo de dados\n");
        exit(EXIT_FAILURE);
    }
    //Chama procedimento de leitura de pokemons
    carregarPokemons(arquivo);
    //Fecha arquivo pokemons
    fclose(arquivo);

    //Tenta abrir arquivo de treinadores
    arquivo = fopen("treinadores.csv", "r");
    //Em caso de erro na abertura do arquivo, fecha o programa
    if(arquivo == NULL) {
        fprintf(stderr, "Erro ao abrir arquivo de dados\n");
        exit(EXIT_FAILURE);
    }
    //Chama procedimento de leitura de treinadores
    carregarTreinadores(arquivo);
    //Fecha arquivo treinadores
    fclose(arquivo);

    //Tenta abrir arquivo de itens
    arquivo = fopen("itens.csv", "r");
    //Em caso de erro na abertura do arquivo, fecha o programa
    if(arquivo == NULL) {
        fprintf(stderr, "Erro ao abrir arquivo de dados\n");
        exit(EXIT_FAILURE);
    }
    //Chama procedimento de leitura de itens
    carregarItens(arquivo);
    //Fecha arquivo itens
    fclose(arquivo);
}

/*
 * Função: listarPokemons
 *       Imprime em tela os códigos de cada pokemon na matrizdepokemons[]
 * 
*/
void listarPokemons(){
    //Lista todos os pokemons de acordo com suas letras iniciais da especie
    for (int i = 0; i < 26; i++)
    {
        int j=0;
        while (matrizdepokemons[i][j] != NULL){
            printf("Codigo: %d \n", matrizdepokemons[i][j]->codp);
            j++;
        }
    }
}

/*
 * Função: buscarTreinador
 *       Procura um treinador na lista pelo nome e retorna um ponteiro para ele
 *       ou NULL caso não seja encontrado
 * 
 * nome (char*): string nome do treinador
 * 
 * retorno (treinador*): se for encontrado retorna um ponteiro para um treinador
 *                       caso contrário, retorna NULL
 * 
*/
treinador* buscarTreinador(char *nome){
    for (int i = 0; i < 50 && vetordetreinadores[i] != NULL; i++){
        if(!strcmp(nome, vetordetreinadores[i]->nome)){
            return vetordetreinadores[i];
        }
    }
    return NULL;
}

/*
 * Função: listarTreinadores
 *       Imprime em tela os nomes de cada treinador no vetordetreinadores[]
 * 
*/
void listarTreinadores(){
    //Percorre vetordetreinadores[] imprimindo o nome
    for (int i = 0; i < 50 && vetordetreinadores[i] != NULL; i++){
        printf("Codigo: %s \n", vetordetreinadores[i]->nome);
    }
}

/*
 * Função: login
 *       Procedimento responsável por imprimir interface de login do usuario
 *       e validar os dados de login e senha inseridos
 * 
 * retorno (int): retorna 1 se usuario e senha estão corretos ou 0
 *                se estiverem incorretos
 * 
 * Obs.: Tamanho máximo da senha é 10 caracteres e de login 15 caracteres
 * 
*/
int login(){
    clrscr(); //Limpa a tela
    char ch = ' '; //Variavel para armazenar caractere lido
    char login[15] = {0}; //String para armazenar login
    char senha[10] = {0}; //String para armazenar senha
    int indicedelinha = 8; //Indice de controle da posição do cursor na linha em tela
    int tamanhodasenha = 0; //Tamanho da senha inserida
    // Leitura do login do usuario
    printf("Login: ");
    scanf("%s", &login); getch(); //getch() é chamado para limpar o buffer

    //Leitura da senha do usuario
    printf("Senha: ");
    //Enquanto o usuario não teclar enter para finalizar a entrada
    while (ch != '\n'){
        //Lê o caracter digitado
        ch = getch();
        //Caso tenha digitado backspace
        if(ch == 127){
            //Verifica se existe caractere a ser deletado
            if(indicedelinha > 8){
                //Muda a posição do cursor e limpa ultimo caracter
                indicedelinha--;
                gotoxy(indicedelinha,2);
                putchar(' ');
                gotoxy(indicedelinha,2);
            }
            //Apaga último caractere da senha
            if(tamanhodasenha<10){
                senha[tamanhodasenha-1]='\000';
            }
            //Decrementa tamanho da senha se possível
            if(tamanhodasenha>0){
                tamanhodasenha--;
            }
        //Caso entre com um caracter diferente de enter
        }else if(ch != '\n'){
            //Imprime * e muda posição do cursor
            putchar('*');
            indicedelinha++;
            //Armazena caracter na senha se for possível e incrementa a senha digitada
            if(tamanhodasenha<10){
                senha[tamanhodasenha] = ch;
            }
            tamanhodasenha++;
        }
    }
    putchar('\n'); //Enter

    //Busca o treinador pelo login
    treinador* t = buscarTreinador(login); 

    //Valida login e senha inseridos e retorna 0 ou 1
    if(strcmp(login,t->nome) || strcmp(senha,t->senha)){
        printf("Usuario ou senha inválidos\nPressione qualquer tecla\n");
        getch();
        return 0;
    }else{
        return 1;
    }   
}

/*
 * Função: buscarpokemon
 *       Procura um pokemon na matriz pelo nome e retorna um ponteiro para ele
 *       ou NULL caso não seja encontrado
 * 
 * nome (char*): string nome do pokemon
 * 
 * retorno (pokemon*): se for encontrado retorna um ponteiro para um pokemon
 *                       caso contrário, retorna NULL
 * 
*/
pokemon* buscarPokemon(char *nomepokemon){
    //Transforma a primeira letra em maiúscula, caso não seja
    if(nomepokemon[0] >= 97 && nomepokemon[0] < 123){
        nomepokemon[0] = (char)(nomepokemon[0])-32;
    }
    //Verifica se a primeira letra é válida
    if(nomepokemon[0] < 65 && nomepokemon[0] >= 91){
        //Se primeira letra não for válida, retorna NULL
        printf("Não encontrado\n");
        return NULL;
    }
    //Percorre o vetor da letra Inicial e retorna o pokemon, caso encontre
    for (int i = 0; i < 35; i++){
        if(matrizdepokemons[(int)nomepokemon[0]-65][i] != NULL &&
            !strcmp(nomepokemon, matrizdepokemons[(int)nomepokemon[0]-65][i]->especie)){
            return matrizdepokemons[(int)nomepokemon[0]-65][i];
        }        
    }
    //Caso não encontre, retorna NULL
    printf("Não encontrado\n");
    printf("Pressione qualquer tecla para voltar");
    return NULL;
}

/*
 * Função: pokedex
 *         Imprime e lê a entrada do menu da pokedex
 *
*/
void pokedex(){
    clrscr(); //Limpa tela
    char opc = ' '; //Caractere para armazenar a opção do usuario
    char sair ='f'; //Caractere de controle para sair do menu
    char temp[15] = {0}; //String para armazenar nome para buscas
    //Loop de menu
    while (sair == 'f')
    {
        clrscr(); //Limpa tela
        //Imprime menu em tela
        printf("Pokedex\n");
        printf("1 - Listar pokemons\n");
        printf("2 - Buscar pokemon\n");
        printf("3 - Ver meus pokemons\n");
        printf("4 - Meus Itens\n");
        printf("5 - Trocar pokemon\n");
        printf("6 - Sair\n");
        //Lê a opção do usuário e chama os procedimentos conforme a escolha
        opc = getchar(); getch(); //getch() para limpar o buffer
        switch (opc){
            case '1':
                listarPokemons();
                printf("Pressione qualquer tecla para voltar ");
                getch(); //getch() para esperar ação do usuário
                break;
            case '2':
                printf("Digite a especie: ");
                scanf("%s", &temp); getch(); //getch() para limpar o buffer
                printPokemon(buscarPokemon(temp));
                printf("\nPressione qualquer tecla para voltar ");
                getch(); //getch() para esperar ação do usuário
                break;
            case '4':
                //listarItens();
                printf("\nPressione qualquer tecla para voltar ");
                getch(); //getch() para esperar ação do usuário
                break;
            case '6':
                sair = 't';
                break;
            default:
                break;
        }
    }
}

/*
 * Função: menu
 *         Imprime menu principal e chama os procedimentos conforme a
 *         opção escolhida.
 *
*/
void menu(){
    clrscr(); //Limpa a Tela
    char sair = 'f'; //Caractere de controle para sair do menu
    char opc; //Caractere para armazenar a opção do usuario
    //Loop de menu
    while (sair == 'f')
    {
        clrscr(); //Limpa a Tela
        //Imprime menu em tela
        printf("Menu\n");
        printf("1 - Pokedex\n");
        printf("2 - Ver treinadores\n");
        printf("3 - Batalhar\n");
        printf("4 - Sair\n");
        printf("5 - Encerrar\n");
        //Lê a opção do usuário e chama os procedimentos conforme a escolha
        opc = getchar(); getch(); //getch() para limpar o buffer
        switch (opc){
            case '1':
                pokedex();
                break;
            case '2':
                listarTreinadores();
                printf("Pressione qualquer tecla\n");
                getch(); //getch() para esperar ação do usuário
                break;
            case '4':
                printf("Saindo....\n");
                printf("Pressione qualquer tecla\n");
                getch(); //getch() para esperar ação do usuário
                sair = 't';
                break;
            case '5':
                printf("Até a próxima...\nPressione qualquer tecla\n");
                getch(); printf("Pressione qualquer tecla\n");
                exit(EXIT_SUCCESS); //Sair do programa
                break;
            default:
                break;
        }
    }
}

//Função principal
int main(int argc, char const *argv[]){   
    //Carregar dados dos arquivos
    carregarDados(); 
    //Loop do jogo
    while (1){
        //Se login for válido entra no menu
        if(login()){
            menu();
        }
    }
    return 0;
}
