#include "hospital.h"

// Função que lê os dados do ficheiro de doentes e os coloca na respetiva lista de doentes
void carregar_ficheiro_doentes(char *nome_ficheiro, ListaDoentes *doentes, int *id){

    FILE *ficheiro = fopen(nome_ficheiro, "r");

    if(ficheiro == NULL){
        printf("\nErro na abertura para leitura do ficheiro de doentes!\n");
        destroi_doentes(doentes);
        fclose(ficheiro);
        exit(1);
    }

    char data_string[MAX];
    char id_doente[MAX], nome[MAX], dia[MAX], mes[MAX], ano[MAX], cc[MAX], telefone[MAX], email[MAX];

    // Leitura (um a um) dos parâmetros de cada doente 
    while (fgets(id_doente, sizeof(id_doente), ficheiro)) {
        
        // Leitura do ID
        id_doente[strcspn(id_doente, "\n")] = '\0'; // Remover \n do fim da string

        // Leitura do Nome
        fgets(nome, sizeof(nome), ficheiro);
        nome[strcspn(nome, "\n")] = '\0'; // Remover \n do fim da string

        // Leitura da Data de Nascimento
        fgets(data_string, sizeof(data_string), ficheiro);
        data_string[strcspn(data_string, "\n")] = '\0'; // Remover \n do fim da string

        // Leitura do Cartão de Cidadão
        fgets(cc, sizeof(cc), ficheiro);
        cc[strcspn(cc, "\n")] = '\0'; // Remover \n do fim da string

        // Leitura do Telefone
        fgets(telefone, sizeof(telefone), ficheiro);
        telefone[strcspn(telefone, "\n")] = '\0'; // Remover \n do fim da string

        // Leitura do Email
        fgets(email, sizeof(email), ficheiro);
        email[strcspn(email, "\n")] = '\0'; // Remover \n do fim da string

        char *str;

        // Partição da string para separ o dia, o mês e o ano da data
        if((str = strtok(data_string, "/")) == NULL){
            printf("\nErro no strtok: leitura do ficheiro de doentes!\n");
            destroi_doentes(doentes);
            fclose(ficheiro);
            exit(1);
        }      
        strcpy(dia, str);

        if((str = strtok(NULL, "/")) == NULL){
            printf("\nErro no strtok: leitura do ficheiro de doentes!\n");
            destroi_doentes(doentes);
            fclose(ficheiro);
            exit(1);
        }           
        strcpy(mes, str);

        if((str = strtok(NULL, "/")) == NULL){
            printf("\nErro no strtok: leitura do ficheiro de doentes!\n");
            destroi_doentes(doentes);
            fclose(ficheiro);
            exit(1);
        }           
        strcpy(ano, str);

        // Criação de uma data auxiliar
        Data data;
        data.dia = atoi(dia);
        data.mes = atoi(mes);
        data.ano = atoi(ano);

        // Atualização do maior id encontrado no ficheiro
        if(atoi(id_doente) > *id){
            *id = atoi(id_doente);
        }
        
        // Chamada da função que adiciona um doente à lista de doentes
        adicionar_doentes(doentes, atoi(id_doente), nome, data, cc, telefone, email);
    }

    fclose(ficheiro);
}

// Função que escreve no ficheiro de doentes os dados armazenados na lista dos doentes
void guardar_ficheiro_doentes(char *nome_ficheiro, ListaDoentes *doentes){

    FILE *ficheiro = fopen(nome_ficheiro, "w");

    if(ficheiro == NULL){
        printf("\nErro na abertura para escrita do ficheiro dos doentes!\n");
        destroi_doentes(doentes);
        fclose(ficheiro);
        exit(1);
    }

    noDoente *aux = doentes->inicio;

    // Percorrer a lista de doentes e escrever no ficheiro os dados dos doentes
    while(aux != NULL){
        fprintf(ficheiro, "%d\n%s\n%d/%d/%d\n%s\n%s\n%s\n", aux->doente.id, aux->doente.nome, aux->doente.data.dia, 
                                    aux->doente.data.mes, aux->doente.data.ano, aux->doente.cc, aux->doente.telefone, aux->doente.email);
        aux = aux->prox;
    }

    fclose(ficheiro);
}

// Função que lê os dados do ficheiro de registos e os coloca na lista de registos do respetivo doente
void carregar_ficheiro_registos(char *nome_ficheiro, ListaDoentes *doentes){

    FILE *ficheiro = fopen(nome_ficheiro, "r");

    if(ficheiro == NULL){
        printf("\nErro na abertura para leitura do ficheiro de doentes!\n");
        destroi_doentes(doentes);
        fclose(ficheiro);
        exit(1);
    }

    char id_doente[MAX], data_string[MAX], dia[MAX], mes[MAX], ano[MAX], tensao_max[MAX], tensao_min[MAX], peso[MAX], altura[MAX];


    while (fgets(id_doente, sizeof(id_doente), ficheiro)) {

        int encontrado = 0;
        
        // Leitura do ID
        id_doente[strcspn(id_doente, "\n")] = '\0';     // Remover \n do fim da string
  
        // Leitura da Data do Registo
        fgets(data_string, sizeof(data_string), ficheiro);
        data_string[strcspn(data_string, "\n")] = '\0'; // Remover \n do fim da string

        // Leitura da Tensão Máxima
        fgets(tensao_max, sizeof(tensao_max), ficheiro);
        tensao_max[strcspn(tensao_max, "\n")] = '\0'; // Remover \n do fim da string

        // Leitura da Tensão Mínima
        fgets(tensao_min, sizeof(tensao_min), ficheiro);
        tensao_min[strcspn(tensao_min, "\n")] = '\0'; // Remover \n do fim da string

        // Leitura do Peso
        fgets(peso, sizeof(peso), ficheiro);
        peso[strcspn(peso, "\n")] = '\0'; // Remover \n do fim da string

        // Leitura da Altura
        fgets(altura, sizeof(altura), ficheiro);
        altura[strcspn(altura, "\n")] = '\0'; // Remover \n do fim da string

        char *str;

        // Partição da string para separ o dia, o mês e o ano da data
        if((str = strtok(data_string, "/")) == NULL){
            printf("\nErro no strtok: leitura do ficheiro de registos!\n");
            destroi_doentes(doentes);
            fclose(ficheiro);
            exit(1);
        }      
        strcpy(dia, str);

        if((str = strtok(NULL, "/")) == NULL){
            printf("\nErro no strtok: leitura do ficheiro de registos!\n");
            destroi_doentes(doentes);
            fclose(ficheiro);
            exit(1);
        }      
        strcpy(mes, str);

        if((str = strtok(NULL, "/")) == NULL){
            printf("\nErro no strtok: leitura do ficheiro de registos!\n");
            destroi_doentes(doentes);
            fclose(ficheiro);
            exit(1);
        }      
        strcpy(ano, str);

        // Criação de uma data auxiliar
        Data data;
        data.dia = atoi(dia);
        data.mes = atoi(mes);
        data.ano = atoi(ano);

        noDoente *aux = doentes->inicio;

        // Percorrer lista dos doentes e inserir a lista de registos no nó do doente correto
        while(aux != NULL){
            if(aux->doente.id == atoi(id_doente)){
                efetuar_registos(aux, data, atoi(tensao_min), atoi(tensao_max), atoi(peso), atoi(altura));
                encontrado = 1;
            }
            aux = aux->prox;
        }

        // Mensagem de erro caso o ID do registo não coincida com o ID de nenhum doente da lista de doentes
        if(encontrado == 0){
            printf("\nErro ao ler o ficheiro de registos: ");
            printf("O ID fornecido no ficheiro de registos não corresponde ao ID de nenhum doente da lista de doentes!\n");
            destroi_doentes(doentes);
            fclose(ficheiro);
            exit(1);
        }
    }

    fclose(ficheiro);
}

// Função que escreve no ficheiro de registos os dados armazenados na lista dos registos de cada doente
void guardar_ficheiro_registos(char *nome_ficheiro, ListaDoentes *doentes){

    FILE *ficheiro = fopen(nome_ficheiro, "w");

    if(ficheiro == NULL){
        printf("\nErro na abertura para escrita do ficheiro dos registos!\n");
        destroi_doentes(doentes);
        fclose(ficheiro);
        exit(1);
    }

    noDoente *aux = doentes->inicio;

    // Percorrer a lista de registos e escrever no ficheiro os dados dos registos
    while(aux != NULL){

        noRegisto *no = aux->doente.registos->inicio;
        while(no != NULL){
            fprintf(ficheiro, "%d\n%d/%d/%d\n%d\n%d\n%d\n%d\n", aux->doente.id, no->registo.data.dia, no->registo.data.mes, no->registo.data.ano,
                                            no->registo.tensao_max, no->registo.tensao_min, no->registo.peso, no->registo.altura);
            no = no->prox;
        }
        aux = aux->prox;
    }

    fclose(ficheiro);
}

// Função que lê da consola o ID de um doente e o elimina da lisat de doentes
void ler_apagar_doentes(ListaDoentes *doentes){
    char id_doente[MAX];
    do{
        printf("Insira o id do doente: ");
        scanf("%s", id_doente);
    } while (verificar_id(id_doente) == 0);
    while ((getchar()) != '\n'){
        getchar();
	}
    retira_doentes(doentes,atoi(id_doente));
}

// Função que lê da consola os dados de um doente e o adiciona à lista de doentes
void ler_adicionar_doentes(ListaDoentes *doentes, int id){
    Data data;
    char nome[MAX];
    char cc[MAX];
    char telefone[MAX];
    char email[MAX];
    int dataValida;
    do { 
        printf("Nome: ");
        fgets(nome, MAX, stdin);
        nome[strcspn(nome, "\n")] = '\0';
    } while (verificar_nome(nome) == 0);
    do {
        printf("Data de nascimento (Dia/Mês/Ano): ");
        if (scanf("%d/%d/%d", &data.dia, &data.mes, &data.ano) != 3) {
            printf("Formato inválido. Por favor, insira a data no formato Dia/Mês/Ano.\n");
            while (getchar() != '\n');
            dataValida = 0;
        } else {
            dataValida = verificar_data(data.dia, data.mes, data.ano);
            if (!dataValida) {
                printf("Data inválida. Por favor, tente novamente.\n");
            }
        }
    } while (!dataValida);
    while (getchar() != '\n');
    do {
        printf("Número de CC (00000000-0-XX0): ");
        fgets(cc, MAX, stdin);
        cc[strcspn(cc, "\n")] = '\0';
    } while (verificar_cc(doentes,cc) == 0);
    do {
        printf("Número de Telefone (000000000): ");
        fgets(telefone, MAX, stdin);
        telefone[strcspn(telefone, "\n")] = '\0';
    } while (verificar_telefone(telefone) == 0);
    do {
        printf("Endereço de Email (@gmail.com/@sapo.pt/@outlook.com/@dei.uc.pt): ");
        fgets(email, MAX, stdin);
        email[strcspn(email, "\n")] = '\0'; 
    } while (verificar_mail(email) == 0);

    int id_doente = id + 1;
    adicionar_doentes(doentes,id_doente,nome,data,cc,telefone,email);
    printf("\nO doente foi adicionado com sucesso!\n");
}

// Função que lê da consola os dados de um registo e o adiciona à lista de registos de um certo doente
void ler_efetuar_registos(ListaDoentes *doentes){
    char id_doente[MAX];
    Data data;
    char tensao_min[20];
    char tensao_max[20];
    char peso[20];
    char altura[20];
    int encontrado = 0;
    do{
        printf("Insira o ID do doente: ");
        scanf("%s", id_doente);
    }  while (verificar_id(id_doente) == 0);
    while ((getchar()) != '\n'){
    	getchar();
    }
    noDoente *no = doentes->inicio;
    while(no != NULL){
        // Verificar se existe um doente com o ID pretendido
        if(no->doente.id == atoi(id_doente)){
            encontrado = 1;
            break;
        }
        no = no->prox;
    }
    if(encontrado == 1){
        printf("\n");
        int dataValida;
        do {
            printf("Data do registo (Dia/Mês/Ano): ");
            if (scanf("%d/%d/%d", &data.dia, &data.mes, &data.ano) != 3) {
                printf("Formato inválido. Por favor, insira a data no formato Dia/Mês/Ano.\n");
                while (getchar() != '\n');
                dataValida = 0;
            } else {
                dataValida = verificar_data(data.dia, data.mes, data.ano);
                if (!dataValida) {
                    printf("Data inválida. Por favor, tente novamente.\n");
                }
            }
        } while (!dataValida);
        do{
            printf("Insira a Tensão Mínima (60-85): ");
            scanf("%s", tensao_min);
        } while (verificar_tensao_min(tensao_min) == 0);
        do{
            printf("Insira a Tensão Máxima (75-140): ");
            scanf("%s", tensao_max);
        } while (verificar_tensao_max(tensao_max) == 0);
        do{
            printf("Insira o Peso (10-200): ");
            scanf("%s", peso);
        } while (verificar_peso(peso) == 0);
        do{
            printf("Insira a Altura (50-230): ");
            scanf("%s", altura);
        } while (verificar_altura(altura) == 0);
        efetuar_registos(no,data,atoi(tensao_min),atoi(tensao_max),atoi(peso), atoi(altura));
        printf("\nRegisto efetuado com sucesso!\n");
    }
    // Caso não seja encontrado um doente com o ID pretendido
    else{
        printf("\nNão foi encontrado nenhum doente com esse id!\n");
    }
}

// Função que lista os dados de todos os doentes existentes na lista de doentes
void imprime_doentes(ListaDoentes *doentes){

    //Caso a lista de doentes se encontre vazia
    if (doentes->inicio == NULL){
        printf("\nNão existem doentes listados no hospital!\n");
        return;
    }
    printf("\n");
    noDoente *no = doentes->inicio;
    while(no != NULL){
        printf("ID: %d\n", no->doente.id);
        printf("Nome: %s\n", no->doente.nome);
        printf("Data de nascimento: %d/%d/%d\n", no->doente.data.dia,no->doente.data.mes,no->doente.data.ano);
        printf("CC: %s\n", no->doente.cc);
        printf("Telefone: %s\n", no->doente.telefone);
        printf("Email: %s\n", no->doente.email);
        if (no->prox != NULL){
            printf("----------------------------------\n");
        }
        no = no->prox;
    }
}

// Função que lê da consola um valor de tensão máxima e lista todos os doentes e os seus registos que apresentem tensões máximas acima desse valor (ordenados por ordem decresecente da maior tensão máxima) 
void ler_imprime_tensoes_altas(ListaDoentes *doentes){

    // Lista de doentes auxiliar que guarda os doentes com registos que apresentam tensões máximas acima do valor pretendido 
    ListaDoentes aux_doentes;
    cria_doentes(&aux_doentes);
    char tensao_max[MAX];
    int encontrado = 0;
    do{
        printf("Insira a Tensão Máxima (75-140): ");
        scanf("%s", tensao_max);
    } while (verificar_tensao_max(tensao_max) == 0);
    printf("\n");
    noDoente *no = doentes->inicio;
    // Percorrer lista dos doentes do hospital
    while(no != NULL){
        int verifica = 0;
        Doente *aux = (Doente*)malloc(sizeof(Doente));
        aux->id = no->doente.id;
        strcpy(aux->nome,no->doente.nome);
        aux->data = no->doente.data;
        strcpy(aux->cc,no->doente.cc);
        strcpy(aux->telefone,no->doente.telefone);
        strcpy(aux->email,no->doente.email);
        // Lista de registos auxiliar que guarda os registos de um doente que apresentam tensões máxima acima do valor pretendido
        ListaRegistos *registos = (ListaRegistos*)malloc(sizeof(ListaRegistos));
        cria_registos(registos);
        noRegisto *no_registo = no->doente.registos->inicio;
        // Percorrer lista dos registos do doente
        while(no_registo != NULL){
            if(no_registo->registo.tensao_max > atoi(tensao_max)){
                // Adicionar (por ordem decrescente de tensão máxima) registo à lista auxiliar caso o valor da tensão máxima do registo esteja acima do valor pretendido 
                insere_registos_max(registos, no_registo->registo);
                verifica = 1;
                encontrado = 1;
            }
            no_registo = no_registo->prox;
        }
        // Inserir (por ordem decrescente da maior tensão máxima da lista de registos) a lista auxiliar dos registos, caso esta não se escontre vazia, no nó do doente da lista auxiliar de doentes 
        if(verifica){
            aux->registos = registos;
            insere_doentes_max(&aux_doentes, *aux);
        }
        else{
            destroi_registos(registos);
        }
        free(aux);
        no = no->prox;
    }
    noDoente *no_aux = aux_doentes.inicio;
    // Percorrer lista auxiliar de doentes e imprimir os dados de cada doente e os seus registos
    while(no_aux != NULL){
        printf("ID: %d\n", no_aux->doente.id);
        printf("Nome: %s\n", no_aux->doente.nome);
        printf("\n\tRegistos:\n\n");
        noRegisto *no_imprime = no_aux->doente.registos->inicio;
        while(no_imprime != NULL){
            printf("\tData: %d/%d/%d\n", no_imprime->registo.data.dia, no_imprime->registo.data.mes, no_imprime->registo.data.ano);
            printf("\tTensão Min: %d\n", no_imprime->registo.tensao_min);
            printf("\tTensão Max: %d\n", no_imprime->registo.tensao_max);
            printf("\tPeso: %d\n", no_imprime->registo.peso);
            printf("\tAltura: %d\n", no_imprime->registo.altura);
            if (no_imprime->prox != NULL){
                printf("\t---------------------\n");
            }
            no_imprime = no_imprime->prox;
        }
        printf("\n");
        if (no_aux->prox != NULL){
            printf("----------------------------------\n");
        }
        no_aux = no_aux->prox;
    }    
    // Caso a lista de doentes auxiliar se encontre vazia
    if(encontrado == 0){
        printf("\nNão existem doentes com tensões máximas acima de %d\n", atoi(tensao_max));  
    }
    destroi_doentes(&aux_doentes);
}

// Função que lê da consolo o ID de um doente e imprime os seus dados e os seus registos
void ler_imprime_registos(ListaDoentes *doentes){
    char id_doente[MAX];
    do {
        printf("Insira o ID do doente: ");
        scanf("%s", id_doente);
    }  while (verificar_id(id_doente) == 0);
    while ((getchar()) != '\n'){
    	getchar();
    }
    int encontrado = 0;
    noDoente *no = doentes->inicio;
    while(no != NULL){
        // Verificar se existe um doente com o ID pretendido
        if(no->doente.id == atoi(id_doente)){
            encontrado = 1;
            break;
        }
        no = no->prox;
    }
    if(encontrado == 1){
        printf("\nID: %d\n", no->doente.id);
        printf("Nome: %s\n", no->doente.nome);
        printf("Data de nascimento: %d/%d/%d\n", no->doente.data.dia,no->doente.data.mes,no->doente.data.ano);
        printf("CC: %s\n", no->doente.cc);
        printf("Telefone: %s\n", no->doente.telefone);
        printf("Email: %s\n", no->doente.email);
        printf("\n\tRegistos:\n\n");
        if (no->doente.registos->inicio == NULL){
            printf("\tO doente não tem registos efetuados!\n");
            return;
        }
        noRegisto *no_registo = no->doente.registos->inicio;
        while(no_registo != NULL){
            printf("\tData: %d/%d/%d\n", no_registo->registo.data.dia, no_registo->registo.data.mes, no_registo->registo.data.ano);
            printf("\tTensão Min: %d\n", no_registo->registo.tensao_min);
            printf("\tTensão Max: %d\n", no_registo->registo.tensao_max);
            printf("\tPeso: %d\n", no_registo->registo.peso);
            printf("\tAltura: %d\n", no_registo->registo.altura);
            if (no_registo->prox != NULL){
                printf("\t---------------------\n");
            }
            no_registo = no_registo->prox;
        }
    }
    // Caso não tenha sido encontrado um doente com o ID pretendido
    else{
        printf("\nNão foi encontrado nenhum doente com esse id!\n");
    }
}

// Função que verifica se o email é válido
int verificar_mail(char *email){
    char *terminacoes[] = {"@gmail.com", "@sapo.pt", "@outlook.com","@dei.uc.pt","@hotmail.com"};
    char *posArroba = strchr(email, '@');
    if (posArroba == NULL){
        return 0;
    }
    for (int i = 0; i < 5; i++){
        if (strcmp(terminacoes[i], posArroba) == 0){
            return 1;
        }
    }
    return 0;
}

// Função que verifica se a data á válida
int verificar_data(int dia, int mes, int ano){

    if (dia < 1){
        return 0;
    }
    if (mes < 1 || mes > 12){
        return 0;
    }
    if (ano < 1900 || ano > 2030){
        return 0;
    }
    if (mes == 1 || mes == 3 || mes == 5 || mes == 7 || mes == 8 || mes == 10 || mes == 12 ){
        if (dia > 31){
            return 0;
        }
    }
    else if (mes == 4 || mes == 6 || mes == 9 || mes == 11){
        if (dia > 30){
            return 0;
        }
    }
    else if (mes == 2){
        if (dia > 29){
            return 0;
        }
    }
    return 1;
}

// Função que verifca se o número de telefone é válido
int verificar_telefone(char *telefone){

    if(strlen(telefone) != 9){
        return 0;
    }

    for (int i = 0; i < 9; i++) {
        if (!isdigit(telefone[i])) {
            return 0;
        }
    }
    return 1; 
}

// Função que verifica se o cc é válido
int verificar_cc(ListaDoentes *doentes, char *cc){
    noDoente *no = doentes->inicio;
    char aux[MAX], aux_1[MAX];
    strcpy(aux,cc);
    for(int i = 0; aux[i] != '\0'; i++){
        if(aux[i] >= 'a' && aux[i] <= 'z'){
            aux[i] = aux[i] - 'a' + 'A';
        }
    }
    while(no != NULL){ 
        strcpy(aux_1,no->doente.cc);
        for(int i = 0; aux_1[i] != '\0'; i++){
            if(aux_1[i] >= 'a' && aux_1[i] <= 'z'){
                aux_1[i] = aux_1[i] - 'a' + 'A';
            }
        }
        if(strcmp(aux_1,aux) == 0){
            return 0;
        }
        no = no->prox;
    }
    if (strlen(cc) != 14) {
        return 0;

    }
    for (int i = 0; i < 8; i++) {
        if (!isdigit(cc[i])) {
            return 0;
        }
    }
    if (cc[8] != '-') {
        return 0;
    }
    if (!isdigit(cc[9]) ) {
        return 0;
    }
    if (cc[10] != '-') {
        return 0;
    }
    for (int i = 11; i < 13; i++) {
        if (!isalpha(cc[i])) {
            return 0;
        }
    }
    if (!isdigit(cc[13])) {
        return 0;
    }
    return 1;
}

// Função que verifica se o ID é válido
int verificar_id(char *id){

    if(atoi(id) <= 0){
        return 0;
    }

    for(int i = 0; id[i] != '\0'; i++){
        if(!isdigit(id[i])){
            return 0;
        }
    }
    return 1;
}

// Função que verifica se o nome é válido
int verificar_nome(char *nome){
    for (size_t i = 0; nome[i] != '\0'; i++){
        if (!isalpha(nome[i]) && nome[i] != ' '){
            return 0;
        }
    }
    return 1;
}

// Função que verifica se o valor da tensão mínima é válido
int verificar_tensao_min(char *tensao_min){

    for(int i = 0; tensao_min[i] != '\0'; i++){
        if(!isdigit(tensao_min[i])){
            return 0;
        }
    }

    if(atoi(tensao_min) < 60 || atoi(tensao_min) > 85){
        return 0;
    }
    return 1;
}

// Função que verifica se o valor da tensão máxima é válido
int verificar_tensao_max(char *tensao_max){

       for(int i = 0; tensao_max[i] != '\0'; i++){
        if(!isdigit(tensao_max[i])){
            return 0;
        }
    }

    if(atoi(tensao_max) < 75 || atoi(tensao_max) > 140){
        return 0;
    }
    return 1;
}

// Função que verifica se o valor do peso é válido
int verificar_peso(char *peso){

    for(int i = 0; peso[i] != '\0'; i++){
        if(!isdigit(peso[i])){
            return 0;
        }
    }

    if(atoi(peso) < 10 || atoi(peso) > 200){
        return 0;
    }
    return 1;
}

// Função que verifica se o valor da altura é válido
int verificar_altura(char *altura){

    for(int i = 0; altura[i] != '\0'; i++){
        if(!isdigit(altura[i])){
            return 0;
        }
    }

    if(atoi(altura) < 50 || atoi(altura) > 230){
        return 0;
    }
    return 1;
}

// Função utilizada para comparar duas datas
int comparar_datas(Data data1, Data data2){
    if(data1.ano != data2.ano){
        return (data1.ano > data2.ano) - (data1.ano < data2.ano);
    }
    else if(data1.mes != data2.mes){
        return (data1.mes > data2.mes) - (data1.mes < data2.mes);
    }
    else{
        return (data1.dia > data2.dia) - (data1.dia < data2.dia);
    }
}