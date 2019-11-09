/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Organizacao e Recuperacao da Informacao
 * Prof. Tiago A. Almeida
 *
 * Trabalho 03B - Hashing com encadeamento
 *
 * RA: 743541
 * Aluno: Giovanni Marçon Rossi
 * ========================================================================== */

/* Bibliotecas */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY 11
#define TAM_NOME 31
#define TAM_GENERO 2
#define TAM_NASCIMENTO 11
#define TAM_CELULAR 16
#define TAM_VEICULO 31
#define TAM_PLACA 9
#define TAM_DATA 9
#define TAM_HORA 6
#define TAM_TRAJETO 121
#define TAM_VALOR 7
#define TAM_VAGAS 2

#define TAM_REGISTRO 256
#define MAX_REGISTROS 1000
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)

/* Saídas do usuário */
#define OPCAO_INVALIDA "Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE "Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO "Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO "Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA "ERRO: Ja existe um registro com a chave primaria: %s.\n\n"
#define ARQUIVO_VAZIO "Arquivo vazio!"
#define INICIO_BUSCA "********************************BUSCAR********************************\n"
#define INICIO_LISTAGEM "********************************LISTAR********************************\n"
#define INICIO_ALTERACAO "********************************ALTERAR*******************************\n"
#define INICIO_ARQUIVO "********************************ARQUIVO*******************************\n"
#define INICIO_EXCLUSAO "**********************EXCLUIR*********************\n"
#define SUCESSO "OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA "FALHA AO REALIZAR OPERACAO!\n"
#define REGISTRO_INSERIDO "Registro %s inserido com sucesso.\n\n"

/* Registro da Carona */
typedef struct
{
	char pk[TAM_PRIMARY_KEY];
	char nome[TAM_NOME];
	char genero[TAM_GENERO];
	char nascimento[TAM_NASCIMENTO]; /* DD/MM/AAAA */
	char celular[TAM_CELULAR];
	char veiculo[TAM_VEICULO];
	char placa[TAM_PLACA];
	char trajeto[TAM_TRAJETO];
	char data[TAM_DATA];   /* DD/MM/AA, ex: 24/09/19 */
	char hora[TAM_HORA];   /* HH:MM, ex: 07:30 */
	char valor[TAM_VALOR]; /* 999.99, ex: 004.95 */
	char vagas[TAM_VAGAS];
} Carona;

/* Registro da Tabela Hash
 * Contém a chave primária, o RRN do registro atual e o ponteiro para o próximo
 * registro. */
typedef struct chave
{
	char pk[TAM_PRIMARY_KEY];
	int rrn;
	struct chave *prox;
} Chave;

/* Estrutura da Tabela Hash */
typedef struct
{
	int tam;
	Chave **v;
} Hashtable;

/* Variáveis globais */
char ARQUIVO[TAM_ARQUIVO];
int nregistros;

/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo();

/* Exibe a Carona */
int exibir_registro(int rrn);

/*Recupera um registro do arquivo de dados*/
Carona recuperar_registro(int rrn);

/*Gera Chave da struct Carona*/
void gerarChave(Carona *novo);


/* FUNÇÕES DO MENU */

/* Cadastra um registro */
void cadastrar(Hashtable *tabela);

/* Altera um registro dado a chave primária e uma entrada */
int alterar(Hashtable tabela);

/* Busca por um registro a partir da chave primária */
void buscar(Hashtable tabela);

/* Remove um registro a partir da chave primária */
int remover(Hashtable *tabela);

/* Imprime a tabela Hash */
void imprimir_tabela(Hashtable tabela);

/* Libera o espaço ocupado pela tabela */
void liberar_tabela(Hashtable *tabela);


/* FUNÇÕES DE APOIO */

/* Cria uma tabela Hash */
void criar_tabela(Hashtable *tabela, int tam);

/* Carrega os registros do arquivo de dados e os insere na tabela Hash */
void carregar_tabela(Hashtable *tabela);

/* Função auxiliar para a inserção dos registros na criação da tabela */
void inserir_carona_aux(Hashtable *tabela, Carona *nova, int rrn);

/*Retorna o primeiro número primo >= a*/
int prox_primo(int a);

/* Verifica se um número é primo ou não */
int is_prime(int num);

/* Passa os arquivos da struct Carona para a string buffer com todos os marcadores */
void construir_buffer(char *buffer, Carona *novo);

/* Insere no campo no registro do arq */
void inserir_campo_arq(char *arq, char *campo, int *reg_preenchido);


/* HASHING */

/*Auxiliar para a função de hash*/
short f(char x);

/*Função de Hash*/
short hash(const char *chave, int tam);


/* INSERÇÕES */

/* Lê da entrada padão e insere na struct Carona 'novo' */
void criar_carona(Carona *novo);

/* Insere uma carona na tabela Hash */
void inserir_carona(Hashtable *tabela, Carona *nova);


/* BUSCAS */

/* Busca por um registro na tabela Hash */
int buscar_iprimary(Hashtable *tabela, char *pk);


/* REMOÇÃO */

/* Remove um registro da tabela hash */
void remover_iprimary(Hashtable *tabela, char *pk);


/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */
int main()
{

	/* Arquivo */
	int carregarArquivo = nregistros = 0;
	scanf("%d%*c", &carregarArquivo); // 1 (sim) | 0 (nao)
	if (carregarArquivo)
		carregar_arquivo();

	/* Tabela Hash */
	int tam;
	scanf("%d%*c", &tam);
	tam = prox_primo(tam);

	Hashtable tabela;
	criar_tabela(&tabela, tam);
	if (carregarArquivo)
		carregar_tabela(&tabela);

	/* Execução do programa */
	int opcao = 0;
	while (opcao != 6)
	{
		scanf("%d%*c", &opcao);
		switch (opcao)
		{
			case 1:
				cadastrar(&tabela);
				break;
			case 2:
				printf(INICIO_ALTERACAO);
				printf("%s", (alterar(tabela)) ? SUCESSO : FALHA);
				break;
			case 3:
				printf(INICIO_BUSCA);
				buscar(tabela);
				break;
			case 4:
				printf(INICIO_EXCLUSAO);
				printf("%s", (remover(&tabela)) ? SUCESSO : FALHA);
				break;
			case 5:
				printf(INICIO_LISTAGEM);
				imprimir_tabela(tabela);
				break;
			case 6:
				liberar_tabela(&tabela);
				break;
			case 7:
				printf(INICIO_ARQUIVO);
				printf("%s\n", (*ARQUIVO != '\0') ? ARQUIVO : ARQUIVO_VAZIO);
				break;
			default:
				printf(OPCAO_INVALIDA);
				break;
		}
	}
	return 0;
}


/* ==========================================================================
 * ================================= FUNÇÕES ================================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo(){
	scanf("%[^\n]%*c", ARQUIVO);
	nregistros = strlen(ARQUIVO) / TAM_REGISTRO;
}

/* Exibe a Carona */
int exibir_registro(int rrn){
	if (rrn < 0)
		return 0;

	Carona j = recuperar_registro(rrn);
	char *traj, trajeto[TAM_TRAJETO];

	printf("%s\n", j.pk);
	printf("%s\n", j.nome);
	printf("%s\n", j.genero);
	printf("%s\n", j.nascimento);
	printf("%s\n", j.celular);
	printf("%s\n", j.veiculo);
	printf("%s\n", j.placa);
	printf("%s\n", j.data);
	printf("%s\n", j.hora);
	printf("%s\n", j.valor);
	printf("%s\n", j.vagas);

	strcpy(trajeto, j.trajeto);

	traj = strtok(trajeto, "|");

	while (traj != NULL)
	{
		printf("%s", traj);
		traj = strtok(NULL, "|");
		if (traj != NULL)
		{
			printf(", ");
		}
	}

	printf("\n");

	return 1;
}

/* Recupera do arquivo o registro com o rrn informado e retorna os dados na
 * struct Carona */
Carona recuperar_registro(int rrn){
	char temp[257], *p;
	strncpy(temp, ARQUIVO + ((rrn)*TAM_REGISTRO), TAM_REGISTRO);
	temp[TAM_REGISTRO] = '\0';
	Carona j;

	p = strtok(temp, "@");
	strcpy(j.nome, p);
	p = strtok(NULL, "@");
	strcpy(j.genero, p);
	p = strtok(NULL, "@");
	strcpy(j.nascimento, p);
	p = strtok(NULL, "@");
	strcpy(j.celular, p);
	p = strtok(NULL, "@");
	strcpy(j.veiculo, p);
	p = strtok(NULL, "@");
	strcpy(j.placa, p);
	p = strtok(NULL, "@");
	strcpy(j.trajeto, p);
	p = strtok(NULL, "@");
	strcpy(j.data, p);
	p = strtok(NULL, "@");
	strcpy(j.hora, p);
	p = strtok(NULL, "@");
	strcpy(j.valor, p);
	p = strtok(NULL, "@");
	strcpy(j.vagas, p);

	gerarChave(&j);

	return j;
}

/* Gera uma chave primária para a struct Carona novo */
void gerarChave(Carona *novo){
    // Nome
    novo->pk[0] = novo->nome[0];
    // Placa
    novo->pk[1] = novo->placa[0];
    novo->pk[2] = novo->placa[1];
    novo->pk[3] = novo->placa[2];
    // Data
    novo->pk[4] = novo->data[0];
    novo->pk[5] = novo->data[1];
    novo->pk[6] = novo->data[3];
    novo->pk[7] = novo->data[4];
    // Hora
    novo->pk[8] = novo->hora[0];
    novo->pk[9] = novo->hora[1];
    // \0
    novo->pk[10] = '\0';

    return;
}

/* ==========================================================================
 * ============================ FUNÇÕES DO MENU =============================
 * ========================================================================== */

/* Cadastra um registro */
void cadastrar(Hashtable *tabela){
	Carona nova;
	int rrn;

	criar_carona(&nova);

	rrn = buscar_iprimary(tabela, nova.pk);

	if(rrn == -1){
		inserir_carona(tabela, &nova);

		printf(REGISTRO_INSERIDO, nova.pk);

		char buffer[TAM_REGISTRO + 1];
		char *pos_arq = ARQUIVO + (nregistros * TAM_REGISTRO);	//Coloca o ponteiro para onde o registro será inserido no arquivo de dados
		construir_buffer(buffer, &nova);
		strncpy(pos_arq, buffer, (TAM_REGISTRO + 1));

		nregistros++;
	}else
		printf(ERRO_PK_REPETIDA, nova.pk);

	return;
}

/* Altera um registro dado a chave primária e uma entrada */
int alterar(Hashtable tabela){
	char pk[TAM_PRIMARY_KEY];
	char nvagas[2];
	
	int n_vag = 0;
	int rrn = 0;

	scanf("%[^\n]%*c", pk);

	rrn = buscar_iprimary(&tabela, pk);

	if(rrn == -1){
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}else{

		scanf("%d%*c", &n_vag);
	
		while(1){							// Checando se o campo isnerido respeita as regras
			if(n_vag > 9 || n_vag < 0){
				printf(CAMPO_INVALIDO);
				scanf("%d%*c", &n_vag);
			}else{
				sprintf(nvagas, "%d", n_vag);
				break;
			}
		}

		char buffer[TAM_REGISTRO + 1];		// Criando um buffer para inserir no arquivo
		char *p_arq;

		p_arq = ARQUIVO + (rrn * TAM_REGISTRO);

		strncpy(buffer, p_arq, TAM_REGISTRO);	// Recuperando o registro

		buffer[TAM_REGISTRO] = '\0';

		for(int i = TAM_REGISTRO; i > 0; i--){	// Encontrando o lugar no buffer em que o registro deve ser inserido
			if(buffer[i] == '@'){
				buffer[i - 1] = nvagas[0];
				break;
			}
		}

		strncpy(p_arq, buffer, TAM_REGISTRO);	// Colocando o buffer no arquivo de dados

		return 1;
	}
}

/* Busca por um registro a partir da chave primária */
void buscar(Hashtable tabela){
	char pk[TAM_PRIMARY_KEY];
	int rrn;

	scanf("%[^\n]%*c", pk);

	rrn = buscar_iprimary(&tabela, pk);

	if(rrn != -1)
		exibir_registro(rrn);
	else
		printf(REGISTRO_N_ENCONTRADO);
	
	return;
}

/* Remove um registro a partir da chave primária */
int remover(Hashtable *tabela){
	int rrn = 0;
	
	char pk[TAM_PRIMARY_KEY];
	char *p_arq;

	scanf("%[^\n]%*c", pk);
	
	rrn = buscar_iprimary(tabela, pk);

	if(rrn != -1){
		p_arq = ARQUIVO + (rrn * TAM_REGISTRO);

		strncpy(p_arq, "*|", 2);
		
		remover_iprimary(tabela, pk);

		return 1;
	}else{
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}
}

/* Imprime a tabela Hash */
void imprimir_tabela(Hashtable tabela){
	Chave *atual;
	
	for(int i = 0; i < tabela.tam; i++){
		printf("[%d]", i);
		atual = tabela.v[i];
		if(atual != NULL){
			while(atual->prox != NULL){
				printf(" %s", atual->pk);
				atual = atual->prox;
			}
			printf(" %s", atual->pk);
		}
		printf("\n");
	}

	return;
}

/* Libera a Tabela Hash */
void liberar_tabela(Hashtable *tabela){
	Chave *aux1, *aux2, *aux3;
	short i;
	for (i = 0, aux1 = tabela->v[i]; i < tabela->tam; aux1 = tabela->v[++i])
		for (aux2 = aux1; aux2; aux3 = aux2, aux2 = aux2->prox, free(aux3), aux3 = NULL)
			;
	free(tabela->v);
}

/* ==========================================================================
 * =========================== FUNÇÕES DE APOIO =============================
 * ========================================================================== */

/* Cria uma nova tabela hash */
void criar_tabela(Hashtable *tabela, int tam){
	tabela->tam = tam;
	tabela->v = (Chave**)malloc(sizeof(Chave*)*tam);

	for(int i = 0; i < tam; i++)
		tabela->v[i] = NULL;

	return;


}

/* Carrega os registros do arquivo de dados e os insere na tabela Hash */
void carregar_tabela(Hashtable *tabela){
	Carona atual;

	for(int i = 0; i < nregistros; i++){
		atual = recuperar_registro(i);

		inserir_carona_aux(tabela, &atual, i);
	}

	return;
}

/* Função auxiliar para a inserção dos registros na criação da tabela */
/* Assume que a tabela hash nunca estará cheia */
void inserir_carona_aux(Hashtable *tabela, Carona *nova, int rrn){
	int pos;
	Chave *atual;
	Chave *proximo;

	Chave *novo;
	novo = (Chave*)malloc(sizeof(Chave));

	strcpy(novo->pk, nova->pk);
	novo->prox = NULL;
	novo->rrn = rrn;

	pos = hash(nova->pk, tabela->tam);

	atual = tabela->v[pos];
	if(atual == NULL){
		tabela->v[pos] = novo;
		return;
	}

	proximo = atual->prox;

	if(strcmp(atual->pk, novo->pk) > 0){
		tabela->v[pos] = novo;
		novo->prox = atual;
		return;
	}else{
		while(1){
			if(proximo == NULL){
				atual->prox = novo;
				return;
			}
			
			if(strcmp(proximo->pk, novo->pk) > 0){
				atual->prox = novo;
				novo->prox = proximo;
				return;
			}

			atual = atual->prox;
			proximo = atual->prox;
		}
	}

	return;
}

/* Retorna o primeiro número primo >= a */
int prox_primo(int a){
	while(1){
		if(is_prime(a))
			break;
		else
			a++;
	}

	return a;
}

/* Verifica se um número é primo ou não */
/* Retorna 1 se num é primo */
/* Retorna 0 se num não é primo */
int is_prime(int num){
	if(num == 1)
		return 0;

	if(num == 2)
		return 1;

	for(int i = 2; i < num/2; i++){
        if(num % i == 0)
            return 0;
    }

    return 1;
}

/* Passa os arquivos da struct Carona para a string buffer com todos os marcadores */
/* A função espera que o buffer fornecido tenhe o tamanho de de TAM_REGISTROS + 1 */
/* A variável reg_preenchido serve como referência para a função inserir_campo_arq */
/* para saber quantos campos existem no registro atualmente, assim permitindo escrever */
/* na string arq, onde a função anterior havia parado. */
void construir_buffer(char *buffer, Carona *novo){
	int reg_preenchido = 0;

	inserir_campo_arq(buffer, novo->nome, &reg_preenchido);
	inserir_campo_arq(buffer, novo->genero, &reg_preenchido);
	inserir_campo_arq(buffer, novo->nascimento, &reg_preenchido);
	inserir_campo_arq(buffer, novo->celular, &reg_preenchido);
	inserir_campo_arq(buffer, novo->veiculo, &reg_preenchido);
	inserir_campo_arq(buffer, novo->placa, &reg_preenchido);
	inserir_campo_arq(buffer, novo->trajeto, &reg_preenchido);
	inserir_campo_arq(buffer, novo->data, &reg_preenchido);
	inserir_campo_arq(buffer, novo->hora, &reg_preenchido);
	inserir_campo_arq(buffer, novo->valor, &reg_preenchido);
	inserir_campo_arq(buffer, novo->vagas, &reg_preenchido);

	for(int i = reg_preenchido; i < TAM_REGISTRO; i++){
		buffer[i] = '#';
	}

	buffer[TAM_REGISTRO] = '\0';

	return;
}

/* Insere no campo no registro do arq */
void inserir_campo_arq(char *arq, char *campo, int *reg_preenchido){
	int temp;

	temp = sprintf((arq + *reg_preenchido), "%s", campo);	// Colocando o campo no registro
	*reg_preenchido += temp;								// Adicionando o número total de caracteres adicionados no registro preenchido
	arq[*reg_preenchido] = '@';								// Adicionando o caractere separador
	*reg_preenchido += 1;									// Adicionando o número total de caracteres devido a adição do caractere separador

	return;
}


/* ==========================================================================
 * ================================ HASHING =================================
 * ========================================================================== */

/* Auxiliar da Função de Hash/*/
short f(char x){
	return (x < 59) ? x - 48 : x - 54;
}

/* Função de Hash */
short hash(const char *chave, int tam){
	short total = 0;

	for(int i = 0; i < 8; i++)
		total += (i+1) * f(chave[i]);

	return total % tam;
}


/* ==========================================================================
 * =============================== INSERÇÕES ================================
 * ========================================================================== */

/* Lê da entrada padão e insere na struct Carona 'novo' */
void criar_carona(Carona *novo){
    scanf("%[^\n]%*c", novo->nome);
    scanf("%[^\n]%*c", novo->genero);
    scanf("%[^\n]%*c", novo->nascimento);
    scanf("%[^\n]%*c", novo->celular);
    scanf("%[^\n]%*c", novo->veiculo);
    scanf("%[^\n]%*c", novo->placa);
    scanf("%[^\n]%*c", novo->trajeto);
    scanf("%[^\n]%*c", novo->data);
    scanf("%[^\n]%*c", novo->hora);
    scanf("%[^\n]%*c", novo->valor);
    scanf("%[^\n]%*c", novo->vagas);

    gerarChave(novo);

    return;
}

/* Insere uma carona na tabela Hash */
void inserir_carona(Hashtable *tabela, Carona *nova){
	int pos;
	Chave *atual;
	Chave *proximo;

	Chave *novo;
	novo = (Chave*)malloc(sizeof(Chave));

	strcpy(novo->pk, nova->pk);
	novo->prox = NULL;
	novo->rrn = nregistros;

	pos = hash(nova->pk, tabela->tam);

	atual = tabela->v[pos];
	if(atual == NULL){
		tabela->v[pos] = novo;
		return;
	}

	proximo = atual->prox;

	if(strcmp(atual->pk, novo->pk) > 0){
		tabela->v[pos] = novo;
		novo->prox = atual;
		return;
	}else{
		while(1){
			if(proximo == NULL){
				atual->prox = novo;
				return;
			}
			
			if(strcmp(proximo->pk, novo->pk) > 0){
				atual->prox = novo;
				novo->prox = proximo;
				return;
			}

			atual = atual->prox;
			proximo = atual->prox;
		}
	}
}

/* ==========================================================================
 * ================================ BUSCAS ==================================
 * ========================================================================== */

/* Busca por um registro na tabela Hash */
/* Retorna o rrn do registro caso o encontrou */
/* Retorna -1 caso o registro não exista */
int buscar_iprimary(Hashtable *tabela, char *pk){
	int pos;
	Chave *atual;

	pos = hash(pk, tabela->tam);

	atual = tabela->v[pos];

	if(atual != NULL){
		while(atual->prox != NULL){
			if(strcmp(atual->pk, pk) == 0)
				return atual->rrn;
			
			atual = atual->prox;
		}

		if(strcmp(atual->pk, pk) == 0)
			return atual->rrn;
		else
			return -1;
	}else
		return -1;
}

/* ==========================================================================
 * ================================ REMOÇÃO =================================
 * ========================================================================== */

/* Remove um registro da tabela hash */
void remover_iprimary(Hashtable *tabela, char *pk){
	int pos;
	Chave *atual;
	Chave *proximo;

	pos = hash(pk, tabela->tam);

	atual = tabela->v[pos];
	proximo = atual->prox;
	
	if(strcmp(atual->pk, pk) == 0){
		tabela->v[pos] = proximo;
		free(atual);
		return;
	}

	while(1){
		if(strcmp(proximo->pk, pk) == 0){
			atual->prox = proximo->prox;
			free(proximo);
			return;
		}
		atual = atual->prox;
		proximo = atual->prox;
	}
}