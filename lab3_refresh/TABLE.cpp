#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TABLE_H.h"
#include "DIALOG_H.h"



//////////////////////////////////////////////////////////////////////////////////////////////// TABLE

//Инициализация таблицы
struct TTable* INIT_TABLE() {
	struct TTable* new_tab = (struct TTable*)calloc(1, sizeof(struct TTable));
	struct KeySpace1* KS1 = (struct KeySpace1*)calloc(MSIZE1, sizeof(struct KeySpace1));
	struct KeySpace2* KS2 = (struct KeySpace2*)calloc(MSIZE2, sizeof(struct KeySpace2));
	/*for (int i = 0; i < MSIZE2; i++) {
		KS2[i].head = (struct BBlock*)calloc(1, sizeof(struct BBlock));
	}*/

	new_tab->Space1 = KS1;
	for (int i = 0; i < MSIZE1; i++) {
		KS1[i].busy = 0;
	}
	new_tab->Space2 = KS2;

	return new_tab;
}

//Новый Item, пока не в таблице
struct IItem* NEW_ITEM(char* key1, int key2, char* string, double number) {
	struct InfoType* info = (struct InfoType*)calloc(1, sizeof(struct InfoType));
	info->string = string;
	info->number = number;

	struct IItem* new_item = (struct IItem*)calloc(1, sizeof(struct IItem));
	new_item->info = info;
	new_item->key1 = key1;
	new_item->key2 = key2;

	return new_item;
}

//Удаление элемента вне таблицы(элемент не подошел)
void FREE_ITEM(struct IItem* item) {
	free(item->info->string);
	free(item->key1);
	free(item);
}

//Полная очистка таблицы
void FREE_ALL_TABLE(struct  TTable* table) {
	for (int i = 0; i < MSIZE1; i++) {
		if ((table->Space1[i].data != NULL) && (table->Space1[i].busy != 0)) {
			
			int key2 = table->Space1[FIND_KS1(table, table->Space1[i].key1)].data->key2;
			struct BBlock* tmp = table->Space2[HASH(key2)].head;
			int ver = 0;
			while (tmp != NULL) {
				if (tmp->data = table->Space1[FIND_KS1(table, table->Space1[i].key1)].data) {
					ver = tmp->release; // Считаем версию элемента из второго пространства по заданному ключу из первого пространства
				}
				tmp = tmp->next;
			}
			table->Space2[key2].head = FREE_BLOCK_KS2_RELEASE(table->Space2[HASH(table->Space1[i].data->key2)].head, table->Space1[i].data->key2, ver);
		}
	}
	free(table->Space1);
	free(table->Space2);
	free(table);
}

// Добавление элемента в таблицу
int ADD_ITEM_TABLE(struct TTable* table, char* key1, int key2, char* str, double number) {
	struct IItem* temp = NEW_ITEM(key1, key2, str, number);
	if (ADD_KS1(table, temp) < 0) {
		return -1; // Ошибка в добавлнии в первое пространтсво
	}
	ADD_KS2(table, temp);
	REORGANIZATION_KS1(table);
	
	return 0; // Добавление прошло успешно
}

//////////////////////////////////////////////////////////////////////////////////////////////// KEYSPACE1
/* Поиск в елемента в первом пространстве ключей */
int FIND_KS1(struct TTable* table, char* key1) {
	for (int i = 0; i < MSIZE1; i++) {
		if ((table->Space1[i].busy == 1) && (strcmp(table->Space1[i].key1, key1) == 0)) {
			return i; // возвращаем индекс элемента
		}
	}
	return -1; // элемент не найден
}

int COUNT(struct TTable* table) {
	int count = 0;

	while (table->Space1[count].busy == 1) {
		count++;
	}

	return count;
}

// Функция реорганизации таблицы, возвращает заполненность таблицы
int REORGANIZATION_KS1(struct TTable* table) {
	int i = 0;
	int j = 0;
	int count = COUNT(table);

	while (count + i < MSIZE1) {
		if (table->Space1[count + i].busy == 1) {
			i = j;
			break;
		}
		i++;
	}

	table->Space1[count] = table->Space1[count + j];
	if (count == 1 && j == 0) {
		return COUNT(table) - 1;
	}
	table->Space1[count + j].busy = 0;


	/*while (i < MSIZE1) {
		if (table->Space1[i].busy == 1) {
			tmp = table->Space1[i];
			j = i;
			while (j != COUNT(table)) {
				table->Space1[j] = table->Space1[j - 1];
				j--;
			}
			table->Space1[j] = tmp;

		}
		i++;
	}*/
	// сборка мусора, организованная циклическим сдвигом 

	
	return COUNT(table) - 1;
}

// Вставка в первое пространство ключей
int ADD_KS1(struct TTable* table, struct IItem* item) {
	int fullness = REORGANIZATION_KS1(table);
	if (fullness == MSIZE1) {
		return -2; // Переполенение первого простраства ключей
	}
	
	if (FIND_KS1(table, item->key1) != -1) {
		return -1; // дублирование ключа в первом пространстве
	}

	table->Space1[fullness + 1].busy = 1;
	table->Space1[fullness + 1].data = item;
	table->Space1[fullness + 1].key1 = item->key1;
	
	return 0; // Вставка прошла успешно
}

// Удаление элемента из таблицы по первому пространству ключей
int DELETE_ELEM_KS1(struct TTable* table, char* key1) {
	if (FIND_KS1(table, key1) == -1) {
		return -1; // Элемента с данным ключом не найдено
	}

	int key2 = table->Space1[FIND_KS1(table, key1)].data->key2;
	struct BBlock* tmp = table->Space2[HASH(key2)].head;
	int ver = 0;
	while (tmp->next != NULL) {
		if (tmp->data = table->Space1[FIND_KS1(table, key1)].data){
			ver = tmp->release;
		}
		tmp = tmp->next;
	}

	table->Space1[FIND_KS1(table, key1)].busy = 0;
	table->Space2[HASH(key2)].head = FREE_BLOCK_KS2_RELEASE(table->Space2[HASH(key2)].head, key2, ver);

	return 0; // Удаление прошло успешно
}



//////////////////////////////////////////////////////////////////////////////////////////////// KEYSPACE2
/*Хэш-функция */
int HASH(int key2) {
	return (key2 % MSIZE2);
}

//Удаление блока
struct BBlock* FREE_BLOCK_KS2_RELEASE(struct BBlock* head, int key2, int release) {
	struct BBlock* temp1 = head;
	struct BBlock* temp2 = head;
	/*while (temp != NULL) {
		if ((temp->key2 == key2) && (temp->release == release)) {
			if (temp == head) {
				head = head->next;
			}
			if (temp->prev) {
				temp->prev->next = temp->next;
			}
			if (temp->next) {
				temp->next->prev = temp->prev;
			}
			FREE_ITEM(temp->data);
			free(temp);
			return head;
		}
		temp = temp->next;
	}*/
	if (temp1->next == NULL) {
		FREE_ITEM(temp1->data);
		free(temp1);
		return head;
	}

	while (temp1->next->key2 != key2 && temp1->next->release != release) {
		temp1 = temp1->next;
	}
	while (temp2->key2 != key2 && temp2->release != release) {
		temp2 = temp2->next;
	}
	temp1->next = temp2->next;
	FREE_ITEM(temp2->data);
	free(temp2);
	
	return head;
}

// Очистка списка коллизий
struct BBlock* FREE_LIST_KS2(struct BBlock* head) {
	struct BBlock* tmp = head;
	while (tmp != NULL) {
		tmp = tmp->next;
		FREE_ITEM(tmp->data);
		free(head);
		head = tmp;
	}
	return head;
}

// Добавление элемента во второе пространство ключей
void ADD_KS2(struct TTable* table, struct IItem* item) {

	table->Space2[HASH(item->key2)].head = NEW_BLOCK(table->Space2[HASH(item->key2)].head, item);
}

// Добавление блока в список коллизий
struct BBlock* NEW_BLOCK(struct BBlock* head, struct IItem* item) {
	int count = 0;
	struct BBlock* tmp = head;
	struct BBlock* previous = NULL;
	int rel = 0;
	while (tmp) {
		previous = tmp;
		tmp = tmp->next;
		rel++;
	}
	free(tmp);

	struct BBlock* addind = (struct BBlock*)calloc(1, sizeof(struct BBlock));
	addind->release = rel;
	addind->data = item;
	addind->key2 = item->key2;

	if (head == NULL) {
		head = addind;
	}
	else {
		addind->next = head->next;
		head->next = addind;
		addind->prev = head;
		head->next->prev = addind;
	}

	return head;
}

// Поиск по второму пространству ключей с версией элемента (возвращается только один элемент)
struct BBlock** FIND_KS2_RELEASE(struct TTable* table, int key2, int release) {
	struct BBlock* tmp = table->Space2[HASH(key2)].head;
	struct BBlock** list = NULL;

	while (tmp != NULL) {
		if ((tmp->data->key2 == key2) && (tmp->release == release)) {
			list = (struct BBlock**)realloc(list, 1 * sizeof(struct KeySpace2*));
			return list;
		}
		tmp = tmp->next;
	}
	return NULL;
}

// Поиск по второму пространству ключей (возвращается список элементов с тем же ключом)
struct BBlock** FIND_KS2(struct TTable* table, int key2, int *len) {
	int count = 0;
	struct BBlock** list = NULL;
	struct BBlock* tmp = table->Space2[HASH(key2)].head;
	
	while (tmp != NULL) {
		if (tmp->data->key2 == key2) {
			count++;
			list = (struct BBlock**)realloc(list, count * sizeof(struct KeySpace2*));
			list[count - 1] = tmp;
		}
		tmp = tmp->next;
	}
	*len = count;
	return list;
}

int DELETE_KS2_RELEASE(struct TTable* table, int key2, int release) {
	struct BBlock* tmp = table->Space2[HASH(key2)].head;
	while (tmp != NULL) {
		if (tmp->release == release) {
			for (int i = 0; i <= REORGANIZATION_KS1(table); i++) {
				if (strcmp(table->Space1[i].key1, tmp->data->key1) == 0) {
					table->Space1[i].busy = 0;
					break;
				}
			}
			if (tmp == table->Space2[HASH(key2)].head) {
				table->Space2[HASH(key2)].head = FREE_BLOCK_KS2_RELEASE(tmp, key2, release);
				return 0;
			}
			else {
				FREE_BLOCK_KS2_RELEASE(tmp, key2, release);
				return 0; // Удаление прошло успешно
			}
		}
		tmp = tmp->next;
	}
	return -1; // Удаление неуспешно
}