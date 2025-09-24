#include "hospital.h"

int main(int argc, char *argv[]){

	if(argc != 3){
		printf("Dica: %s <ficheiro doentes> <ficheiro registos>\n", argv[0]);
		return - 1;
	}

	int id = 0;
    ListaDoentes doentes;
    cria_doentes(&doentes);
	carregar_ficheiro_doentes(argv[1], &doentes, &id);
	carregar_ficheiro_registos(argv[2], &doentes);
	char opc[100];
	int opcao;
    do{
        do{
			printf("\n\t----------------------------------------");
			printf("\n\t| 1 - Adicionar Novo Doente            |");
			printf("\n\t| 2 - Eliminar Doente                  |");
			printf("\n\t| 3 - Listar Doentes                   |");
			printf("\n\t| 4 - Listar Doentes com Tensões Altas |");
			printf("\n\t| 5 - Listar Informação de um Doente   |");
			printf("\n\t| 6 - Efetuar um Registo               |");
			printf("\n\t| 7 - Sair                             |");
			printf("\n\t----------------------------------------\n");   
			printf("\n\tOpção: ");
			scanf("%s", opc);
   		} while (strlen(opc) != 1 || atoi(opc) < 1 || atoi(opc) > 7);
		while ((getchar()) != '\n'){
        	getchar();
		}
		printf("\n");
		opcao = atoi(opc);
		switch(opcao){
			case 1:
				ler_adicionar_doentes(&doentes, id);
				id++;
				break;

			case 2:
				ler_apagar_doentes(&doentes);
				break;

			case 3:
				imprime_doentes(&doentes);
				break;

			case 4:
				ler_imprime_tensoes_altas(&doentes);
				break;
			
			case 5:
				ler_imprime_registos(&doentes);
				break;
			
			case 6:
				ler_efetuar_registos(&doentes);
				break;

			case 7: 
				printf("\n\n\tA encerrar a aplicação . . . \n\n");
				guardar_ficheiro_doentes(argv[1], &doentes);
				guardar_ficheiro_registos(argv[2], &doentes);
				destroi_doentes(&doentes);
				break;
			
			default: 
				break;
		}
	}
	while(opcao != 7);

	return 0;
}