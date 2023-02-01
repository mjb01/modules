/* 
 * hash.c -- implements a generic hash table as an indexed set of queues.
 *
 */
#include "hash.h"
#include "queue.c"
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


/* 
 * SuperFastHash() -- produces a number between 0 and the tablesize-1.
 * 
 * The following (rather complicated) code, has been taken from Paul
 * Hsieh's website under the terms of the BSD license. It's a hash
 * function used all over the place nowadays, including Google Sparse
 * Hash.
 */
#define get16bits(d) (*((const uint16_t *) (d)))

static uint32_t SuperFastHash (const char *data,int len,uint32_t tablesize) {
  uint32_t hash = len, tmp;
  int rem;
  
  if (len <= 0 || data == NULL)
		return 0;
  rem = len & 3;
  len >>= 2;
  /* Main loop */
  for (;len > 0; len--) {
    hash  += get16bits (data);
    tmp    = (get16bits (data+2) << 11) ^ hash;
    hash   = (hash << 16) ^ tmp;
    data  += 2*sizeof (uint16_t);
    hash  += hash >> 11;
  }
  /* Handle end cases */
  switch (rem) {
  case 3: hash += get16bits (data);
    hash ^= hash << 16;
    hash ^= data[sizeof (uint16_t)] << 18;
    hash += hash >> 11;
    break;
  case 2: hash += get16bits (data);
    hash ^= hash << 11;
    hash += hash >> 17;
    break;
  case 1: hash += *data;
    hash ^= hash << 10;
    hash += hash >> 1;
  }
  /* Force "avalanching" of final 127 bits */
  hash ^= hash << 3;
  hash += hash >> 5;
  hash ^= hash << 4;
  hash += hash >> 17;
  hash ^= hash << 25;
  hash += hash >> 6;
  return hash % tablesize;
}
// structure of hashtable internal
// holds size of table and whether table is empty
typedef struct hashtableArray {
	uint32_t size;
	bool empty;
	queue_t** table_p;
} hashtableArray_t;


// initializes a table
static hashtableArray_t* makeTable(uint32_t hsize) {
	// calculate amount of memory to allocate (memory for struct,
	// size of table * memory for queue
	size_t n_bytes = sizeof(hashtableArray_t);
	// allocate memory
	hashtableArray_t* internalTable = (hashtableArray_t*) malloc(n_bytes);
	if (internalTable != NULL) {
		queue_t* array = (queue_t*) calloc((size_t) hsize, sizeof(queue_t*));
		internalTable -> size = hsize;
		internalTable -> table_p = array;
		// figure this out
		//		for (int i=0; i<hsize; i++) array[i] = NULL;
		
		internalTable -> empty = true;
		return internalTable;
	} else
		return NULL;
}

// get rid of table from memory (i.e. deallocate memory)
static void removeTable(hashtableArray_t *hashtable) {
	for (uint32_t i=0; i<(hashtable -> size); i++) {
		queue_t* queue = (hashtable -> table_p)[i];
		if (queue != NULL)
			qclose(queue);
	}
	free(hashtable -> table_p);
	free(hashtable);
	return;
}


// hopen -- opens a hash table with initial size hsize
hashtable_t *hopen(uint32_t hsize) {
	hashtableArray_t* table = makeTable(hsize);
	return (hashtable_t*) table;
}                                                                              

// hclose -- closes a hash table
void hclose(hashtable_t *htp) {                                                
	if (htp != NULL)
		removeTable((hashtableArray_t*) htp);
	return;
}                                                                              


// hput --  puts an element into a hashtable under designated key
// returns 0 for success, non-zero otherwise
// TODO: handle errors in returns
int32_t hput(hashtable_t *htp, void *ep, const char *key, int keylen) {        
	int32_t res;
	// convert input pointer to internal hashtable pointer
	hashtableArray_t *hashtable = (hashtableArray_t*) htp;
	if (hashtable == NULL) {
		return 255;
	} else {
		// run hash function to get index for the table
		uint32_t index = SuperFastHash(key, keylen, hashtable -> size);
		// if there is nothing at the index from the hash function make a queue
		// and pass it to that index of the table
		if (hashtable -> empty == true)
			hashtable -> empty = false;
		if ((hashtable -> table_p)[index] == NULL) {
			queue_t* qp = qopen();
			if (qp != NULL) {
				res = qput(qp, ep);
				(hashtable -> table_p)[index] = qp;
			} else {
				return 255;
			}
		} else {
			queue_t *qp = (hashtable -> table_p)[index];
			res = qput(qp, ep);
		}
		return res;
	}
}                                                                              

// happly -- applies a function to every entry in hash table
void happly(hashtable_t *htp, void (*fn)(void* ep)) {                         
	// convert input pointer to internal hashtable pointer
	hashtableArray_t *hashtable = (hashtableArray_t*) htp;
	// if hashtable is not empty
	if (hashtable -> empty == false) {
		for (uint32_t i=0;i<(hashtable->size);i++) {
			// get the pointer to the queue at index i
			queue_t *queue = (hashtable->table_p)[i];
			// if the queue is not empty
			if (queue != NULL) {
				qapply(queue, fn); // apply function
			}
		}
	}
	return;
}                                                                              

/* hsearch -- searches for an entry under a designated key using a
 * designated search fn -- returns a pointer to the entry or 
 * NULL if not found
 */


void *hsearch(hashtable_t *htp,                                                
              bool (*searchfn)(void* elementp, const void* searchkeyp),        
              const char *key,                                                 
              int32_t keylen) {
	// convert input pointer to internal hashtable pointer
	hashtableArray_t *hashtable = (hashtableArray_t*) htp;
	if (hashtable != NULL) {
		// hash the key to get the index
		uint32_t index = SuperFastHash(key, keylen, hashtable -> size);
		// get the pointer to the queue
		queue_t *queue = (hashtable -> table_p)[index];
		// search the queue for the element
		void *element = qsearch(queue, searchfn, key);
		return element;
	}
	return NULL;
} 
                                                                             

/* hremove -- removes and returns an entry under a designated key
 * using a designated search fn -- returns a pointer to the entry or
 * NULL if not found
 */


void *hremove(hashtable_t *htp,                                                
              bool (*searchfn)(void* elementp, const void* searchkeyp),
              const char *key,
              int32_t keylen) {
	// convert input pointer to internal hashtable pointer
	hashtableArray_t *hashtable = (hashtableArray_t*) htp;
	if (hashtable != NULL) {
		// hash the key to get the index
		uint32_t index = SuperFastHash(key, keylen, hashtable->size);
		// get the pointer to the queue
		queue_t *queue = (hashtable->table_p)[index];
		// search the queue for the element and remove it
		void *element = qremove(queue, searchfn, key);
		return element;
	}
	return NULL;
}

