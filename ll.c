/**
 * Generic Unioned Linked List
 * (C) 2013 Christian Gunderman
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as 
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.  If not, see 
 * <http://www.gnu.org/licenses/>.
 *
 * Contact Email: gundermanc@gmail.com 
 */

#include "ll.h"

// creates new linked list struct
LL * ll_new() {
  LL * newList = (LL*)malloc(sizeof(LL));
  if(newList != 0) {
    newList->size = 0;
    newList->head = 0;
    newList->tail = 0;
    return newList;
  } else
    return 0;
}

// gets size of linked list
int ll_size(LL * list) {
  return list->size;
}

// frees linked list. List MUST be empty
void ll_free(LL * list) {
  free(list); 
}

void ll_append(LL * list, LLValue item) {
  LLNode * newNode = (LLNode*)malloc(sizeof(LLNode));
  newNode->nextNode = 0; // last node in series
  newNode->payload = item;

  // if list is empty
  if(list->tail == 0) {
    list->head = newNode;
    list->tail = newNode;
  } else {
    list->tail->nextNode = newNode;
    list->tail = newNode;
  }
  list->size++;
}

// appends a double value to the list
void ll_append_double(LL * list, double value) {
  LLValue llValue;
  llValue.doubleVal = value;
  ll_append(list, llValue);
}

// appends a generic pointer to the list
void ll_append_void(LL * list, void * value) {
  LLValue llValue;
  llValue.voidVal = value;
  ll_append(list, llValue);
}

// appends an integer to the list
void ll_append_int(LL * list, int value) {
  LLValue llValue;
  llValue.intVal = value;
  ll_append(list, llValue);
}

// returns the node of the given index in the list
LLNode * ll_get_node(LL * list, int index) {
  int i = 0;
  LLNode * node;

  if(index == LL_TAIL)
    return list->tail;

  // iterate through entire list until index matches
  for(node = list->head;
      node != 0; node = (LLNode*) node->nextNode, i++) {
    if(i == index)
      return node;
  }
  return 0;
}

// gets an iterator to iterate through the list
void ll_iterator_get(LLIterator * iteratorObject, LL * list) {
  iteratorObject->list = list;
  iteratorObject->previous = 0;
  iteratorObject->head = list->head;
  iteratorObject->cursor = iteratorObject->head;
}

// pops current item off iterator, and switches to next one
// returns true if success and false if not success
bool ll_iterator_pop(LLIterator * iteratorObject, LLValue * value) {
  if(iteratorObject->cursor != 0) {
    LLValue payload = iteratorObject->cursor->payload;
    iteratorObject->previous = iteratorObject->cursor;
    iteratorObject->cursor = (LLNode*)iteratorObject->cursor->nextNode;
    memcpy(value, &payload, sizeof(LLValue));
    return true;
  } else 
    return false; // no more items
}

// gets current item in iterator, without switching to next one
bool ll_iterator_peek(LLIterator * iterator, LLValue * value) {
  if(iterator->cursor != 0) {
    memcpy(value, &iterator->cursor->payload, sizeof(LLValue));
    return true;
  }
  return false;
}

// are there items left in iterator
bool ll_iterator_has_next(LLIterator * iteratorObject) {
  return (iteratorObject->cursor != 0);
}

// removes the current item from the iterator
// be sure to use proper peek/pop combo
LLValue ll_iterator_remove(LLIterator * iterator) {
  LLValue payload;
  if(iterator->cursor != 0) {
    LLNode * node = iterator->cursor;
    payload = iterator->cursor->payload;
    iterator->list->size--; // decrement size
    if(iterator->list->size)
      iterator->list->tail = 0;
    if(iterator->cursor == iterator->list->head) {
      iterator->list->head = (LLNode*)iterator->cursor->nextNode; // change pointer in list
    } else {
      iterator->previous->nextNode = iterator->cursor->nextNode;
    }
    iterator->cursor = (LLNode*)iterator->cursor->nextNode;
    free(node); // free memory used by node
  }
  return payload;
}
