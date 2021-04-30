#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TABLE_H.h"
#include "DIALOG_H.h"



//////////////////////////////////////////////////////////////////////////////////////////////// TABLE

//������������� �������
struct TTable* INIT_TABLE() {
	struct TTable* new_tab = (struct TTable*)calloc(1, sizeof(struct TTable));
	struct KeySpace1* KS1 = (struct KeySpace1*)calloc(MSIZE1, sizeof(struct KeySpace1));
	struct KeySpace2* KS2 = (struct KeySpace2*)calloc(MSIZE2, sizeof(struct KeySpace2));

	new_tab->Space1 = KS1;
	new_tab->Space2 = KS2;

	return new_tab;
}

//����� Item, ���� �� � �������
struct IItem* NEW_ITEM(char* key1, int key2, char* string, int number) {
	struct InfoType* info = (struct InfoType*)calloc(1, sizeof(struct InfoType));
	info->string = string;
	info->number = number;

	struct IItem* new_item = (struct IItem*)calloc(1, sizeof(struct IItem));
	new_item->info = info;
	new_item->key1 = key1;
	new_item->key2 = key2;

	return new_item;
}

//�������� �������� ��� �������(������� �� �������)
void FREE_ITEM(struct IItem* item) {
	free(item->info->string);
	free(item->key1);
	free(item);
}

//������ ������� �������
void FREE_ALL_TABLE(struct  TTable* table) {
	for (int i = 0; i < MSIZE1; i++) {
		if ((table->Space1[i].data != NULL) && (table->Space1[i].busy != 0)) {
			
			int key2 = table->Space1[FIND_KS1(table, table->Space1[i].key1)].data->key2;
			struct BBlock* tmp = table->Space2[HASH(key2)].head;
			int ver = 0;
			while (tmp->next != NULL) {
				if (tmp->data = table->Space1[FIND_KS1(table, table->Space1[i].key1)].data) {
					ver = tmp->release; // ������� ������ �������� �� ������� ������������ �� ��������� ����� �� ������� ������������
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

// ���������� �������� � �������
int ADD_ITEM_TABLE(struct TTable* table, char* key1, int key2, char* str, int number) {
	struct IItem* temp = NEW_ITEM(key1, key2, number, str);
	if (ADD_KS1(table, temp) < 0) {
		return -1; // ������ � ��������� � ������ ������������
	}
	ADD_KS2(table, temp);
	REORGANIZATION_KS1(table);
	return 0; // ���������� ������ �������
}

//////////////////////////////////////////////////////////////////////////////////////////////// KEYSPACE1
/* ����� � �������� � ������ ������������ ������ */
int FIND_KS1(struct TTable* table, char* key1) {
	for (int i = 0; i < MSIZE1; i++) {
		if ((table->Space1[i].busy == 1) && (table->Space1[i].key1 == key1)) {
			return i; // ���������� ������ ��������
		}
	}
	return -1; // ������� �� ������
}

// ������� ������������� �������, ���������� ������������� �������
int REORGANIZATION_KS1(struct TTable* table) {
	int i = 0;
	int j = 0;
	struct KeySpace1 tmp;

	while (i < MSIZE1) {
		if (table->Space1[i].busy == 1) {
			tmp = table->Space1[i];
			j = i;
			while (j != 0) {
				table->Space1[j] = table->Space1[j - 1];
				j--;
			}
			table->Space1[0] = tmp;
		}
		i++;
	}
	// ������ ������, �������������� ����������� ������� 

	int fullness = 0;
	int count = 0;

	while (table->Space1[count].busy == 1) {
		fullness++;
		count++;
	}
	// �������� ������������� �������
	return fullness;
}

// ������� � ������ ������������ ������
int ADD_KS1(struct TTable* table, struct IItem* item) {
	int fullness = REORGANIZATION_KS1(table);
	if (fullness == MSIZE1) {
		return -2; // ������������� ������� ����������� ������
	}
	
	if (FIND_KS1(table, item->key1) != -1) {
		return -1; // ������������ ����� � ������ ������������
	}

	table->Space1[fullness + 1].busy = 1;
	table->Space1[fullness + 1].data = item;
	table->Space1[fullness + 1].key1 = item->key1;
	
	return 0; // ������� ������ �������
}

// �������� �������� �� ������� �� ������� ������������ ������
int DELETE_ELEM_KS1(struct TTable* table, char* key1) {
	if (FIND_KS1(table, key1) == -1) {
		return -1; // �������� � ������ ������ �� �������
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

	return 0; // �������� ������ �������
}



//////////////////////////////////////////////////////////////////////////////////////////////// KEYSPACE2
/*���-������� */
int HASH(int key2) {
	return (key2 % MSIZE2);
}

//�������� �����
struct BBlock* FREE_BLOCK_KS2_RELEASE(struct BBlock* head, int key2, int release) {
	struct BBlock* temp = head;
	while (temp != NULL) {
		if ((temp->key2 == key2) & (temp->release)) {
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
	}
	return head;
}

// ������� ������ ��������
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

// ���������� �������� �� ������ ������������ ������
void ADD_KS2(struct TTable* table, struct IItem* item) {
	struct KeySpace2* List = table->Space2 + HASH(item->key2);
	/*List->head->next->release = 0;

	struct BBlock* tmp = table->Space2[HASH(item->key2)].head;
	int count = 0;
	while (tmp != NULL) {
		if ((tmp->key2 = item->key2) && (tmp->release > count)) {
			count = tmp->release;
		}
	}
	List->head->release = count + 1;*/

	List->head = NEW_BLOCK(List->head, item);
}

// ���������� ����� � ������ ��������
struct BBlock* NEW_BLOCK(struct BBlock* head, struct IItem* item) {
	int count = 0;
	struct BBlock* tmp = head;
	struct BBlock* previous = NULL;
	int rel = 1;
	while (tmp) {
		previous = tmp;
		tmp = tmp->next;
		rel++;
	}
	
	tmp = (struct BBlock*)calloc(1, sizeof(struct BBlock));
	tmp->prev = previous;
	if (tmp->prev) { 
		tmp = tmp->prev->next; 
	}
	else { 
		head = tmp; 
	}
	tmp->release = rel;
	tmp->next = NULL;
	tmp->data = item;
	return head;
}

// ����� �� ������� ������������ ������ � ������� �������� (������������ ������ ���� �������)
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

// ����� �� ������� ������������ ������ (������������ ������ ��������� � ��� �� ������)
struct BBlock** FIND_KS2(struct TTable* table, int key2, int *len) {
	int *count = 0;
	struct BBlock** list = NULL;
	struct BBlock* tmp = table->Space2[HASH(key2)].head;
	
	while (tmp != NULL) {
		if (tmp->data->key2 == key2) {
			(*count)++;
			list = (struct BBlock**)realloc(list, *count * sizeof(struct KeySpace2*));
			list[*count - 1] = tmp;
		}
		tmp = tmp->next;
	}
	return list;
}

int DELETE_KS2_RELEASE(struct TTable* table, int key2, int release) {
	struct BBlock* tmp = table->Space2[HASH(key2)].head;
	while (tmp != NULL) {
		if (tmp->release == release) {
			for (int i = 0; i < REORGANIZATION_KS1(table); i++) {
				if (table->Space1[i].key1 == tmp->data->key1) {
					table->Space1[i].busy == 0;
					break;
				}
			}
			if (tmp == table->Space2[HASH(key2)].head) {
				table->Space2[HASH(key2)].head = FREE_BLOCK_KS2_RELEASE(tmp, key2, release);
			}
			else {
				FREE_BLOCK_KS2_RELEASE(tmp, key2, release);
				return 0; // �������� ������ �������
			}
		}
		tmp = tmp->next;
	}
	return -1; // �������� ���������
}