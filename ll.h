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

#ifndef LL__H__
#define LL__H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define LL_TAIL -1

typedef union {
  double doubleVal;
  int intVal;
  void * voidVal;
}LLValue;

typedef struct tagLLNode {
  void * nextNode;
  LLValue payload;
}LLNode;

typedef struct tagLL {
  LLNode * head;
  LLNode * tail;
  int size;
}LL;

typedef struct tagLLIterator {
  LL * list;
  LLNode * head;
  LLNode * cursor;
  LLNode * previous;
}LLIterator;

LL * ll_new();
void ll_free(LL * list);
void ll_append(LL * list, LLValue item);
int ll_size(LL * list);
LLNode * ll_get_node(LL * list, int index);
void ll_iterator_get(LLIterator * iteratorObject, LL * list);
bool ll_iterator_pop(LLIterator * iteratorObject, LLValue * value);
bool ll_iterator_peek(LLIterator * iteratorObject, LLValue * value);
bool ll_iterator_has_next(LLIterator * iteratorObject);
LLValue ll_iterator_remove(LLIterator * iteratorObject);
void ll_append_double(LL * list, double value);
void ll_append_void(LL * list, void * value);
void ll_append_int(LL * list, int value);
#endif //LL__H__
