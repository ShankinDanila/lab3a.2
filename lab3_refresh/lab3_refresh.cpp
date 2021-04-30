#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DIALOG_H.h"
#include "TABLE_H.h"

const char* MSGS[] = { "0. Quit", "1. Add the element", "2. Print the table", "3. Search",
                      "4. Delete element with key1", "5. Delete element with key2 and release",
                      "6. Search with key2", "7. Search with key2 and release" };

const int MSGS_SIZE = sizeof(MSGS) / sizeof(MSGS[0]);

int main() {
    int c;
    struct TTable* table = INIT_TABLE();
    do {
        c = DIALOG(MSGS, MSGS_SIZE);
        switch (c) {
        case 0:
            break;
        case 1:
            printf("Please, enter key1, key2, info_num: ");
            char key1[N];
            int key2;
            double num;
            scanf("%s %s %d", key1, &key2, &num);

            printf("Please, enter info_string: ");
            char* string = get_str();

            if (ADD_ITEM_TABLE(table, key1, key2, string, num) == -1) {
                printf("Error in adding");
            }
            break;
        
        case 2:
            D_PRINT_TABLE(table);
            break;
        case 3:
            printf("Enter key1: ");
            char key111[N];
            scanf("%s", key111);
            if (FIND_KS1(table, key111) > 1) {
                struct BBlock* tmp = table->Space2[HASH(table->Space1[FIND_KS1(table, key111)].data->key2)].head;
                int count = 0;
                while (tmp != NULL) {
                    if ((tmp->key2 = table->Space1[FIND_KS1(table, key111)].data->key2) && (tmp->release > count)) {
                        count = tmp->release;
                    }
                }
                printf("Element:");
                printf("| %6s | %10lf | %10d | %10d | %s |\n", key111, table->Space1[FIND_KS1(table, key111)].data->key2, (count + 1), table->Space1[FIND_KS1(table, key111)].data->info->number, table->Space1[FIND_KS1(table, key111)].data->info->string);
            }
            else {
                printf("No such elements in table");
            }
        case 4: 
            printf("Please, enter key1: ");
            char key11[N];
            scanf("%6s", key11);

            if (DELETE_ELEM_KS1(table, key11) == -1) {
                printf("Error in delete");
            }
            break;
        case 5:
            printf("Please, enter key2 and release:");
            int release;
            scanf("%d %d", &key2, &release);
            while (scanf("%*c"));
            int i = DELETE_KS2_RELEASE(table, key2, release);
            if (i < 0) {
                printf("Error in delete");
            }
            break;
        case 6:
            printf("Please, enter key2: ");
            scanf("%d", &key2);
            while (scanf("%*c"));
            struct BBlock **List1;
            int len;
            List1 = FIND_KS2(table, key2, &len);
            PRINT_LIST_KS2(List1, len);
            free(List1);
            break;
        case 7:
            printf("Please, enter key2 and release: ");
            scanf("%d, %d", &key2, &release);
            while (scanf("%*c"));
            struct BBlock** List;
            List = FIND_KS2_RELEASE(table, key2, release);
            PRINT_LIST_KS2(List, 2);
            break;
        }
    } while (c != 0);
    FREE_ALL_TABLE(table);
    return 0;
}