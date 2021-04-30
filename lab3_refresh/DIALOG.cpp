#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DIALOG_H.h"
#include "TABLE_H.h"

// Диалоговое сообщение
int DIALOG(const char* msgs[], int n) {
    char* error = "";
    int choice;
    do {
        puts(error);
        error = "You're wrong. Try again!";
        for (int i = 0; i < n; ++i) {
            puts(msgs[i]);
        }
        printf("Make your choice: ");
        choice = getchar() - '0';
        while (getchar() != '\n') {}
    } while (choice < 0 || choice >= n);
    return choice;
}

// Вывод таблицы, возможны 3 варианта
void D_PRINT_TABLE(struct TTable* table) {
	if (table == NULL) {
		printf("You should make a table!\n");
		return;
	}

	int n;

	printf("How do you want to print the table?\n");
	printf("If you want to see minimum information, enter 1\n");
	printf("If you want to see only first keyspace, enter 2\n");
	printf("If you want to see only second keyspace, enter 3\n");

	GetInt(&n);

	if ((n > 3) || (n < 0)) {
		printf("Incorrect number of case");
		return;
	}

	switch (n) {
	case 1: {
		printf("\n");
		printf("----------------------------------------------------\n");
		printf("                  The Table:\n");

		printf("----------------------------------------------------\n");
		printf("|  KEY1 |    KEY2    |    NUMBER    |    STRING    |\n");
		printf("----------------------------------------------------\n");

		for (int i = 0; i < REORGANIZATION_KS1(table); i++)
		{
			printf("| %10s | %10lf | %10d | %s |\n", table->Space1[i].data->key1, table->Space1[i].data->key2, table->Space1[i].data->info->number, table->Space1[i].data->info->string);
		}
		printf("----------------------------------------------------\n");
		printf("\n");
	}
		  break;


	case 2:
	{
		printf("\n");
		printf("----------------------------------------------------\n");
		printf("            The Table (first keyspace):\n");
		printf("----------------------------------------------------\n");
		printf("Current size: %d\n", REORGANIZATION_KS1(table));
		printf("----------------------------------------------------\n");
		printf("| # |  KEY1  |   BUSY  |    NUMBER   |    STRING    |\n");
		printf("----------------------------------------------------\n");

		for (int i = 0; i < MSIZE1; i++)
		{
			if (i < REORGANIZATION_KS1(table))
			{
				printf("| %d | %6s | %d | %10lf | %s |\n", i, table->Space1[i].data->key1, table->Space1[i].busy, table->Space1[i].data->info->number, table->Space1[i].data->info->string);
			}
			else
			{
				printf("| %d | 0x000 | 0x00000000 | 0x00000000 |\n", i);
			}
		}
		printf("-----------------------------------------------------\n");
		printf("\n");
	}
	break;


	case 3:
	{
		printf("\n");
		printf("--------------------------------------------------\n");
		printf("          The Table (second keyspace):\n");
		printf("--------------------------------------------------\n");
		printf("| # | Key2 | Release |   NUMBER   |    STRING    |\n");
		printf("--------------------------------------------------\n");

		for (int i = 0; i < MSIZE2; i++) {
			while (table->Space2[i].head->next != NULL) {
				printf("| %d | %10d | %4d  | %10lf | %s |\n", i, table->Space2[i].head->key2, table->Space2[i].head->release, table->Space2[i].head->data->info->number, table->Space2[i].head->data->info->string);
			}
		}
		printf("--------------------------------------------------\n");
		printf("\n");
	}
	break;
	}
}

void PRINT_LIST_KS2(struct BBlock** List, int len) {
	for (int i = 0; i < len; i++) {
		printf("KEYSP2: %d %d %lf %s\n", List[i]->data->key2, List[i]->release, List[i]->data->info->number, List[i]->data->info->string);
	}	
}


// Функция для получения строки любой длины
char* get_str() {
    char buf[81] = { 0 };
    char* res = NULL;
    int len = 0;
    int n = 0;
    do {
        n = scanf("%80[^\n]", buf);
        if (n < 0) {
            if (!res) {
                return NULL;
            }
        }
        else if (n > 0) {
            int chunk_len = strlen(buf);
            int str_len = len + chunk_len;
            res = realloc(res, str_len + 1);
            memcpy(res + len, buf, chunk_len);
            len = str_len;
        }
        else {
            scanf("%*c");
        }
    } while (n > 0);

    if (len > 0) {
        res[len] = '\0';
    }
    else {
        res = calloc(1, sizeof(char));
    }
    return res;
}

int GetInt(int* number) {
	int status;
	do
	{
		status = scanf_s("%d", number);
		if (status < 0)
		{
			return 0;
		}
		if (status == 0)
		{
			printf("%s\n", "Error! Please enter integer number.");
			scanf_s("%*[^\n]");
			scanf_s("%*c");
		}
	} while (status == 0);
	scanf_s("%*c");
	return 1;
}