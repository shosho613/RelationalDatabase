#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LinkedList.h"
/* relation for StudentID-Name-Address-Phone */
typedef struct SNAP *SNAP_LIST;
struct SNAP{
	int studentID;
	char* name;
	char* address;
	char* phone;
	LinkedList collisions;
};
SNAP_LIST SNAP_HASHTABLE[1009];

void SNAP_write()
{    FILE *f = fopen("SNAP.txt", "w");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    
    for(int i = 0; i < 1009; i++)
    {
        if(SNAP_HASHTABLE[i] != NULL){
            char text[50];
            sprintf(text, "%d \t %s \t %s \t %s\n", SNAP_HASHTABLE[i]->studentID, SNAP_HASHTABLE[i]->name, SNAP_HASHTABLE[i]->address, SNAP_HASHTABLE[i]->phone);
            fprintf(f, "%s\n", text);
        }
    }
    fclose(f);
}

void SNAP_read()
{
    printf("Student-ID \t Name \t Address \t Phone\n");
    char ch;
    FILE *f;
    f = fopen("SNAP.txt", "r");
    
    if (f == NULL)
    {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }
    
    while((ch = fgetc(f)) != EOF)
        printf("%c", ch);
    
    fclose(f);
}
SNAP_LIST new_SNAP(int id,char* name, char* address, char* phone){
	SNAP_LIST this =  (SNAP_LIST)malloc(sizeof(struct SNAP));
	if(this == NULL){
		return NULL; //out of memory
	}
	this->studentID = id;
	this->name = name;
	this->address = address;
	this->phone = phone;
	this->collisions = new_LinkedList();
	return this;
}
void free_SNAP(SNAP_LIST this){
	if(this == NULL){
		return;
	}
	free(this->collisions);
	free(this); 
}
int SNAP_getID(SNAP_LIST this){
	return this->studentID;
}
/** key is {course, student} */
int SNAP_hash(int id){
	return id % 1009;
}
/** if hashed index is empty, simply insert there. If there is value, add to then end of the linked list.
If already in the list, dont add
 */
void SNAP_insert(int id, char* name, char* address, char* phone){
	SNAP_LIST this = new_SNAP(id,name,address,phone);
	int index = SNAP_hash(id);
	if(SNAP_HASHTABLE[index] == NULL){
		SNAP_HASHTABLE[index] = this;
	}else{
		SNAP_LIST node = SNAP_HASHTABLE[index];
		if(strcmp(node->name,name) == 0 && strcmp(node->address, address) == 0 && node->phone == phone  && node->studentID == id){
			printf("Tuple already in relation.\n");
		}else{LinkedList_add_at_end(node->collisions, this);}
	}
}
/** specific lookup when using a key. */
LinkedList SNAP_keyLookup(int id){
	LinkedList result = new_LinkedList();
	int index = SNAP_hash(id);
	LinkedList_add_at_end(result, SNAP_HASHTABLE[index]);
	return result;
}
LinkedList SNAP_lookup(int id, char* name, char* address, char* phone){
	LinkedList result = new_LinkedList();
	if(id != 0 && name == NULL && address == NULL && phone == NULL){ //key is not empty, lookup(id, *, *,*)
		return SNAP_keyLookup(id);
	}
	if(id != 0 && name != NULL && address == NULL && phone == NULL){//lookup(id, name, *, *)
		LinkedList list = SNAP_keyLookup(id);
		LinkedListIterator iterator = LinkedList_iterator(list);
		while (LinkedListIterator_hasNext(iterator)) {
			SNAP_LIST node = LinkedListIterator_next(iterator);
			if(node->studentID == id && strcmp(node->name, name) == 0){
				LinkedList_add_at_end(result, node);
			}
		}
		free(iterator);
			
	}
	if(id != 0 && name != NULL && address != NULL && phone == NULL){//lookup(id, name, address, *)
		LinkedList list = SNAP_keyLookup(id);
		LinkedListIterator iterator = LinkedList_iterator(list);
		while (LinkedListIterator_hasNext(iterator)) {
			SNAP_LIST node = LinkedListIterator_next(iterator);
			if(node->studentID == id && strcmp(node->name, name) == 0 && strcmp(node->address,address) == 0){
				LinkedList_add_at_end(result, node);
			}
		}
		free(iterator);
	}
	if(id != 0 && name != NULL && address != NULL && phone != NULL){//lookup(id, name, address, phone)
		LinkedList list = SNAP_keyLookup(id);
		LinkedListIterator iterator = LinkedList_iterator(list);
		while (LinkedListIterator_hasNext(iterator)) {
			SNAP_LIST node = LinkedListIterator_next(iterator);
			if(node->studentID == id && strcmp(node->name, name) == 0 && strcmp(node->address,address) == 0 && strcmp(node->phone,phone) == 0){
				LinkedList_add_at_end(result, node);
			}
		}
		free(iterator);
	}
	if(id != 0 && name == NULL && address != NULL && phone != NULL){//lookup(id, *, address, phone)
		LinkedList list = SNAP_keyLookup(id);
		LinkedListIterator iterator = LinkedList_iterator(list);
		while (LinkedListIterator_hasNext(iterator)) {
			SNAP_LIST node = LinkedListIterator_next(iterator);
			if(node->studentID == id && strcmp(node->address,address) == 0 && strcmp(node->phone,phone) == 0){
				LinkedList_add_at_end(result, node);
			}
		}
		free(iterator);
	}
	if(id != 0 && name == NULL && address == NULL && phone != NULL){//lookup(id, *, *, phone)
		LinkedList list = SNAP_keyLookup(id);
		LinkedListIterator iterator = LinkedList_iterator(list);
		while (LinkedListIterator_hasNext(iterator)) {
			SNAP_LIST node = LinkedListIterator_next(iterator);
			if(node->studentID == id && strcmp(node->phone,phone) == 0){
				LinkedList_add_at_end(result, node);
			}
		}
		free(iterator);
	}
	if(id == 0 && name != NULL && address == NULL && phone == NULL){//lookup(*, name, *, *)
		for(int i = 0; i < 1009; i++){
			if(SNAP_HASHTABLE[i] != NULL){
				if(strcmp(SNAP_HASHTABLE[i]->name, name) == 0){LinkedList_add_at_end(result, SNAP_HASHTABLE[i]);}
				LinkedList list = SNAP_HASHTABLE[i]->collisions;
				LinkedListIterator iterator = LinkedList_iterator(list);
				while (LinkedListIterator_hasNext(iterator)) {
					SNAP_LIST node = LinkedListIterator_next(iterator);
					if(strcmp(node->name, name) == 0){
						LinkedList_add_at_end(result, node);
					}
				}
				free(iterator);
			}
		}
	}
	if(id == 0 && name == NULL && address != NULL && phone == NULL){//lookup(*, *, address, *)
		for(int i = 0; i < 1009; i++){
			if(SNAP_HASHTABLE[i] != NULL){
				if(strcmp(SNAP_HASHTABLE[i]->address, address) == 0){LinkedList_add_at_end(result, SNAP_HASHTABLE[i]);}
				LinkedList list = SNAP_HASHTABLE[i]->collisions;
				LinkedListIterator iterator = LinkedList_iterator(list);
				while (LinkedListIterator_hasNext(iterator)) {
					SNAP_LIST node = LinkedListIterator_next(iterator);
					if(strcmp(node->address, address) == 0){
						LinkedList_add_at_end(result, node);
					}
				}
				free(iterator);
			}
		}
	}
	if(id == 0 && name == NULL && address != NULL && phone != NULL){//lookup(*, *, address, phone)
		for(int i = 0; i < 1009; i++){
			if(SNAP_HASHTABLE[i] != NULL){
				if(strcmp(SNAP_HASHTABLE[i]->address, address) == 0 && strcmp(SNAP_HASHTABLE[i]->phone, phone) == 0){LinkedList_add_at_end(result, SNAP_HASHTABLE[i]);}
				LinkedList list = SNAP_HASHTABLE[i]->collisions;
				LinkedListIterator iterator = LinkedList_iterator(list);
				while (LinkedListIterator_hasNext(iterator)) {
					SNAP_LIST node = LinkedListIterator_next(iterator);
					if(strcmp(node->address, address) == 0 && strcmp(node->phone, phone) == 0){
						LinkedList_add_at_end(result, node);
					}
				}
				free(iterator);
			}
		}
	}
	if(id == 0 && name == NULL && address == NULL && phone != NULL){//lookup(*, *, *, phone)
		for(int i = 0; i < 1009; i++){
			if(SNAP_HASHTABLE[i] != NULL){
				if(strcmp(SNAP_HASHTABLE[i]->phone, phone) == 0){LinkedList_add_at_end(result, SNAP_HASHTABLE[i]);}
				LinkedList list = SNAP_HASHTABLE[i]->collisions;
				LinkedListIterator iterator = LinkedList_iterator(list);
				while (LinkedListIterator_hasNext(iterator)) {
					SNAP_LIST node = LinkedListIterator_next(iterator);
					if(strcmp(node->phone, phone) == 0){
						LinkedList_add_at_end(result, node);
					}
				}
				free(iterator);
			}
		}
	}
	if(id == 0 && name != NULL && address != NULL && phone == NULL){//lookup(*, name, address, *)
		for(int i = 0; i < 1009; i++){
			if(SNAP_HASHTABLE[i] != NULL){
				if(strcmp(SNAP_HASHTABLE[i]->name, name) == 0 && strcmp(SNAP_HASHTABLE[i]->address,address) == 0){LinkedList_add_at_end(result, SNAP_HASHTABLE[i]);}
				LinkedList list = SNAP_HASHTABLE[i]->collisions;
				LinkedListIterator iterator = LinkedList_iterator(list);
				while (LinkedListIterator_hasNext(iterator)) {
					SNAP_LIST node = LinkedListIterator_next(iterator);
					if(strcmp(node->name, name) == 0 && strcmp(node->address,address) == 0){
						LinkedList_add_at_end(result, node);
					}
				}
				free(iterator);
			}
		}
	}
	if(id == 0 && name != NULL && address != NULL && phone != NULL){//lookup(*, name, address, phone)
		for(int i = 0; i < 1009; i++){
			if(SNAP_HASHTABLE[i] != NULL){
				if(strcmp(SNAP_HASHTABLE[i]->name, name) == 0 && strcmp(SNAP_HASHTABLE[i]->address,address) == 0 && strcmp(SNAP_HASHTABLE[i]->phone,phone) == 0){LinkedList_add_at_end(result, SNAP_HASHTABLE[i]);}
				LinkedList list = SNAP_HASHTABLE[i]->collisions;
				LinkedListIterator iterator = LinkedList_iterator(list);
				while (LinkedListIterator_hasNext(iterator)) {
					SNAP_LIST node = LinkedListIterator_next(iterator);
					if(strcmp(node->name, name) == 0 && strcmp(node->address,address) == 0 && strcmp(node->phone,phone) == 0){
						LinkedList_add_at_end(result, node);
					}
				}
				free(iterator);
			}
		}
	}
	if(id == 0 && name != NULL && address == NULL && phone != NULL){//lookup(*, name, *, phone)
		for(int i = 0; i < 1009; i++){
			if(SNAP_HASHTABLE[i] != NULL){
				if(strcmp(SNAP_HASHTABLE[i]->name, name) == 0  && strcmp(SNAP_HASHTABLE[i]->phone,phone) == 0){LinkedList_add_at_end(result, SNAP_HASHTABLE[i]);}
				LinkedList list = SNAP_HASHTABLE[i]->collisions;
				LinkedListIterator iterator = LinkedList_iterator(list);
				while (LinkedListIterator_hasNext(iterator)) {
					SNAP_LIST node = LinkedListIterator_next(iterator);
					if(strcmp(node->name, name) == 0 && strcmp(node->phone,phone) == 0){
						LinkedList_add_at_end(result, node);
					}
				}
				free(iterator);
			}
		}
	}
	return result; // will return empty linked list or a linked list of nodes that match criteria.
}
void SNAP_delete(int id, char* name, char* address, char* phone){
	if(id != 0 && name == NULL && address == NULL && phone == NULL){ //key is not empty, delete(id, *, *,*)
		int index = SNAP_hash(id);
		free(SNAP_HASHTABLE[index]);
		SNAP_HASHTABLE[index] = NULL;
	}
	if(id != 0 && name != NULL && address == NULL && phone == NULL){//delete(id, name, *, *)
		int index = SNAP_hash(id);
		LinkedList list = SNAP_HASHTABLE[index]->collisions;
		LinkedListIterator iterator = LinkedList_iterator(list);
		while (LinkedListIterator_hasNext(iterator)) {
			SNAP_LIST node = LinkedListIterator_next(iterator);
			if(node->studentID == id && strcmp(node->name, name) == 0){
				LinkedList_remove(list, node);
				free(node);
			}
		}
		free(iterator);
		if(strcmp(SNAP_HASHTABLE[index]->name, name) == 0){
			free(SNAP_HASHTABLE[index]);
			SNAP_HASHTABLE[index] = NULL;
		}
			
	}
	if(id != 0 && name != NULL && address != NULL && phone == NULL){//delete(id, name, address, *)
		int index = SNAP_hash(id);
		LinkedList list = SNAP_HASHTABLE[index]->collisions;
		LinkedListIterator iterator = LinkedList_iterator(list);
		while (LinkedListIterator_hasNext(iterator)) {
			SNAP_LIST node = LinkedListIterator_next(iterator);
			if(node->studentID == id && strcmp(node->name, name) == 0 && strcmp(node->address, address) == 0){
				LinkedList_remove(list, node);
				free(node);
			}
		}
		free(iterator);
		if(strcmp(SNAP_HASHTABLE[index]->name, name) == 0 && strcmp(SNAP_HASHTABLE[index]->address, address) == 0){
			free(SNAP_HASHTABLE[index]);
			SNAP_HASHTABLE[index] = NULL; 
		}
			
	}
	if(id != 0 && name != NULL && address != NULL && phone != NULL){//delete(id, name, address, phone)
		int index = SNAP_hash(id);
		LinkedList list = SNAP_HASHTABLE[index]->collisions;
		LinkedListIterator iterator = LinkedList_iterator(list);
		while (LinkedListIterator_hasNext(iterator)) {
			SNAP_LIST node = LinkedListIterator_next(iterator);
			if(node->studentID == id && strcmp(node->name, name) == 0 && strcmp(node->address, address) == 0 && strcmp(node->phone,phone) == 0){
				LinkedList_remove(list, node);
				free(node);
			}
		}
		free(iterator);
		if(strcmp(SNAP_HASHTABLE[index]->name, name) == 0 && strcmp(SNAP_HASHTABLE[index]->address, address) == 0 && strcmp(SNAP_HASHTABLE[index]->phone,phone) == 0){
			free(SNAP_HASHTABLE[index]);
			SNAP_HASHTABLE[index] = NULL; 
		}
			
	}
	if(id != 0 && name == NULL && address != NULL && phone != NULL){//delete(id, *, address, phone)
		int index = SNAP_hash(id);
		LinkedList list = SNAP_HASHTABLE[index]->collisions;
		LinkedListIterator iterator = LinkedList_iterator(list);
		while (LinkedListIterator_hasNext(iterator)) {
			SNAP_LIST node = LinkedListIterator_next(iterator);
			if(node->studentID == id && strcmp(node->address, address) == 0 && strcmp(node->phone,phone) == 0){
				LinkedList_remove(list, node);
				free(node);
			}
		}
		free(iterator);
		if(strcmp(SNAP_HASHTABLE[index]->address, address) == 0 && strcmp(SNAP_HASHTABLE[index]->phone,phone) == 0){
			free(SNAP_HASHTABLE[index]);
			SNAP_HASHTABLE[index] = NULL; 
		}
			
	}
	if(id != 0 && name == NULL && address == NULL && phone != NULL){//delete(id, *, *, phone)
		int index = SNAP_hash(id);
		LinkedList list = SNAP_HASHTABLE[index]->collisions;
		LinkedListIterator iterator = LinkedList_iterator(list);
		while (LinkedListIterator_hasNext(iterator)) {
			SNAP_LIST node = LinkedListIterator_next(iterator);
			if(node->studentID == id && strcmp(node->phone,phone) == 0){
				LinkedList_remove(list, node);
				free(node);
			}
		}
		free(iterator);
		if(strcmp(SNAP_HASHTABLE[index]->phone,phone) == 0){
			free(SNAP_HASHTABLE[index]);
			SNAP_HASHTABLE[index] = NULL; 
		}
	}
	if(id == 0 && name != NULL && address != NULL && phone == NULL){//delete(*, name, *, *)
		for(int i = 0; i < 1009; i++){
			if(SNAP_HASHTABLE[i] != NULL){
				LinkedList list = SNAP_HASHTABLE[i]->collisions;
				LinkedListIterator iterator =  LinkedList_iterator(list);
				while (LinkedListIterator_hasNext(iterator)) {
					SNAP_LIST node = LinkedListIterator_next(iterator);
					if(strcmp(node->name,name) == 0){
						LinkedList_remove(list, node);
						free(node);
					}
				}
				free(iterator);
				if(strcmp(SNAP_HASHTABLE[i]->name,name) == 0){
					free(SNAP_HASHTABLE[i]);
					SNAP_HASHTABLE[i] = NULL;
				}
			}
		}
	}
	if(id == 0 && name == NULL && address != NULL && phone == NULL){//delete(*, *, address, *)
		for(int i = 0; i < 1009; i++){
			if(SNAP_HASHTABLE[i] != NULL){
				LinkedList list = SNAP_HASHTABLE[i]->collisions;
				LinkedListIterator iterator =  LinkedList_iterator(list);
				while (LinkedListIterator_hasNext(iterator)) {
					SNAP_LIST node = LinkedListIterator_next(iterator);
					if(strcmp(node->address,address) == 0){
						LinkedList_remove(list, node);
						free(node);
					}
				}
				free(iterator);
				if(strcmp(SNAP_HASHTABLE[i]->address,address) == 0){
					free(SNAP_HASHTABLE[i]);
					SNAP_HASHTABLE[i] = NULL;
				}
			}
		}
	}
	if(id == 0 && name == NULL && address != NULL && phone != NULL){//lookup(*, *, address, phone)
		for(int i = 0; i < 1009; i++){
			if(SNAP_HASHTABLE[i] != NULL){
				LinkedList list = SNAP_HASHTABLE[i]->collisions;
				LinkedListIterator iterator =  LinkedList_iterator(list);
				while (LinkedListIterator_hasNext(iterator)) {
					SNAP_LIST node = LinkedListIterator_next(iterator);
					if(strcmp(node->address,address) == 0 && strcmp(node->phone,phone) == 0){
						LinkedList_remove(list, node);
						free(node);
					}
				}
				free(iterator);
				if(strcmp(SNAP_HASHTABLE[i]->address,address) == 0 && strcmp(SNAP_HASHTABLE[i]->phone,phone) == 0){
					free(SNAP_HASHTABLE[i]);
					SNAP_HASHTABLE[i] = NULL;
				}
			}
		}
	}
	if(id == 0 && name == NULL && address == NULL && phone != NULL){//lookup(*, *, *, phone)
		for(int i = 0; i < 1009; i++){
			if(SNAP_HASHTABLE[i] != NULL){
				LinkedList list = SNAP_HASHTABLE[i]->collisions;
				LinkedListIterator iterator =  LinkedList_iterator(list);
				while (LinkedListIterator_hasNext(iterator)) {
					SNAP_LIST node = LinkedListIterator_next(iterator);
					if(strcmp(node->phone,phone) == 0){
						LinkedList_remove(list, node);
						free(node);
					}
				}
				free(iterator);
				if(strcmp(SNAP_HASHTABLE[i]->phone,phone) == 0){
					free(SNAP_HASHTABLE[i]);
					SNAP_HASHTABLE[i] = NULL;
				}
			}
		}
	}
	if(id == 0 && name != NULL && address != NULL && phone == NULL){//lookup(*, name, address, *)
		for(int i = 0; i < 1009; i++){
			if(SNAP_HASHTABLE[i] != NULL){
				LinkedList list = SNAP_HASHTABLE[i]->collisions;
				LinkedListIterator iterator =  LinkedList_iterator(list);
				while (LinkedListIterator_hasNext(iterator)) {
					SNAP_LIST node = LinkedListIterator_next(iterator);
					if(strcmp(node->name,name) == 0 && strcmp(node->address,address) == 0){
						LinkedList_remove(list, node);
						free(node);
					}
				}
				free(iterator);
				if(strcmp(SNAP_HASHTABLE[i]->name,name) == 0 && strcmp(SNAP_HASHTABLE[i]->address,address) == 0){
					free(SNAP_HASHTABLE[i]);
					SNAP_HASHTABLE[i] = NULL;
				}
			}
		}
	}
	if(id == 0 && name != NULL && address != NULL && phone != NULL){//delete(*, name, address, phone)
		for(int i = 0; i < 1009; i++){
			if(SNAP_HASHTABLE[i] != NULL){
				LinkedList list = SNAP_HASHTABLE[i]->collisions;
				LinkedListIterator iterator =  LinkedList_iterator(list);
				while (LinkedListIterator_hasNext(iterator)) {
					SNAP_LIST node = LinkedListIterator_next(iterator);
					if(strcmp(node->name,name) == 0 && strcmp(node->address,address) == 0 && strcmp(node->phone,phone) == 0){
						LinkedList_remove(list, node);
						free(node);
					}
				}
				free(iterator);
				if(strcmp(SNAP_HASHTABLE[i]->name,name) == 0 && strcmp(SNAP_HASHTABLE[i]->address,address) == 0 && strcmp(SNAP_HASHTABLE[i]->phone,phone) == 0){
					free(SNAP_HASHTABLE[i]);
					SNAP_HASHTABLE[i] = NULL;
				}
			}
		}
	}
	if(id == 0 && name != NULL && address == NULL && phone != NULL){//lookup(*, name, *, phone)
		for(int i = 0; i < 1009; i++){
			if(SNAP_HASHTABLE[i] != NULL){
				LinkedList list = SNAP_HASHTABLE[i]->collisions;
				LinkedListIterator iterator =  LinkedList_iterator(list);
				while (LinkedListIterator_hasNext(iterator)) {
					SNAP_LIST node = LinkedListIterator_next(iterator);
					if(strcmp(node->name,name) == 0  && strcmp(node->phone,phone) == 0){
						LinkedList_remove(list, node);
						free(node);
					}
				}
				free(iterator);
				if(strcmp(SNAP_HASHTABLE[i]->name,name) == 0 && strcmp(SNAP_HASHTABLE[i]->phone,phone) == 0){
					free(SNAP_HASHTABLE[i]);
					SNAP_HASHTABLE[i] = NULL;
				}
			}
		}
	}
}
void SNAP_print(){
	printf("Student-ID\tName\tAddress\tPhone\n");
	for(int i = 0; i < 1009; i++){
		if(SNAP_HASHTABLE[i] != NULL){
			SNAP_LIST row = SNAP_HASHTABLE[i];
			printf("%d \t %s \t %s \t %s\n", row->studentID, row->name, row->address, row->phone);							
		}
	}
}
void SNAP_printSingle(SNAP_LIST this){
	printf("%d \t %s\t %s\t %s\n", this->studentID, this->name, this->address, this->phone);
}
void SNAP_printList(LinkedList list){
	printf("Student-ID\tName\tAddress\tPhone\n");
	LinkedListIterator iterator2 =  LinkedList_iterator(list);
	while (LinkedListIterator_hasNext(iterator2)) {
		SNAP_LIST node = LinkedListIterator_next(iterator2);
		SNAP_printSingle(node);
	}
	free(iterator2);
}																				
/*int main(){
	printf("------TESTING SNAP--------");
	SNAP_insert(12345,"C.Brown", "12 Apple St.", "555-1234");
	SNAP_insert(67890,"L.Van Pelt", "34 Pear Ave.", "555-5678");																	
	SNAP_insert(22222,"P. Patty", "56 Grape Blvd.", "555-9999");
	printf("Finished inserting\n");
	SNAP_print();
	
	LinkedList test = SNAP_lookup(12345,NULL,NULL,NULL); //lookup(12345, *, *,*,)
	SNAP_printList(test);
	
	printf("\nDELete TEST\n\n");
	SNAP_delete(12345,NULL,NULL,NULL);
	SNAP_print();

}*/