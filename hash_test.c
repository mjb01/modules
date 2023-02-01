/* hash_test.c --- 
 * 
 * 
 * Author: Gebriel Belaineh, Andy Trinh & Michael Bongo 
 * Created: Wed Feb  1 13:02:04 2023 (-0500)
 * Version: 
 * 
 * Description: tests hash.c  
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"
#include "queue.h"

#define MAX_KEYLEN 32
#define MAX_VALUELEN 128

typedef struct {
  char key[MAX_KEYLEN];
  char value[MAX_VALUELEN];
} test_entry;

void print_entry(void *ep) {
  test_entry *entry = (test_entry *) ep;
  printf("Key: %s, Value: %s\n", entry->key, entry->value);
}

bool search_fn(void *elementp, const void *searchkeyp) {
  test_entry *entry = (test_entry *) elementp;
  return strcmp(entry->key, (char *) searchkeyp) == 0;
}

int main() {
  int num_entries;
  hashtable_t *hashtable;
  test_entry *entry;

  // Small hash table (10 entries)
  num_entries = 10;
  hashtable = hopen(num_entries);
  for (int i = 0; i < num_entries; i++) {
    entry = (test_entry *) malloc(sizeof(test_entry));
    sprintf(entry->key, "key%d", i);
    sprintf(entry->value, "value%d", i);
    hput(hashtable, entry, entry->key, strlen(entry->key));
  }
  printf("Entries in small hash table:\n");
  happly(hashtable, print_entry);
  hclose(hashtable);

  // Medium hash table (100 entries)
  num_entries = 100;
  hashtable = hopen(num_entries);
  for (int i = 0; i < num_entries; i++) {
    entry = (test_entry *) malloc(sizeof(test_entry));
    sprintf(entry->key, "key%d", i);
    sprintf(entry->value, "value%d", i);
    hput(hashtable, entry, entry->key, strlen(entry->key));
  }
  printf("\nEntries in medium hash table:\n");
  happly(hashtable, print_entry);
  hclose(hashtable);

  // Large hash table (1000 entries)
  num_entries = 1000;
  hashtable = hopen(num_entries);
  for (int i = 0; i < num_entries; i++) {
    entry = (test_entry *) malloc(sizeof(test_entry));
    sprintf(entry->key, "key%d", i);
    sprintf(entry->value, "value%d", i);
    hput(hashtable, entry, entry->key, strlen(entry->key));
  }
  printf("\nEntries in large hash table:\n");
  happly(hashtable, print_entry);
  hclose(hashtable);

  return 0;
}
