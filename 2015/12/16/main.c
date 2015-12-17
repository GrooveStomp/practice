//#define _POSIX_C_SOURCE 199309L
//#include <features.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

typedef int (*gs_comparison_function_ptr)(void *, void *);

typedef struct gs_binary_insertion_tree
{
  void **Items;
  int *Marked;
  uint32_t Count;
} gs_binary_insertion_tree;

int
__gs_GetLeftRightIndex(gs_binary_insertion_tree *Tree, uint32_t Index)
{
  int Result = Index + 1;     // 0 =>  1 ( 1,  2)
  if (Result > Tree->Count) { // 1 =>  3 ( 3,  4)
    return -1;                // 2 =>  5 ( 5,  6)
  }                           // 3 =>  7 ( 7,  8)
  else {                      // 4 =>  9 ( 9, 10)
    return Result;            // 5 => 11 (11, 12)
  }
}

uint32_t
__gs_ClosestCount(uint32_t NumItems)
{
  uint32_t i = -1;
  uint32_t Count = 0;

  while (Count < NumItems) {
    Count = pow(2, ++i);
  }

  return Count;
}

uint32_t
gs_GetNumBytesRequired(uint32_t NumItems)
{
  uint32_t Count = __gs_ClosestCount(NumItems);
  uint32_t ItemsSize = Count * sizeof(void *);
  uint32_t MarkedSize = Count * sizeof(int);
  return sizeof(gs_binary_insertion_tree) + ItemsSize + MarkedSize;
}

gs_binary_insertion_tree *
gs_CreateBinaryInsertionTree(void *Memory, uint32_t NumItems)
{
  uint8_t *MemoryBase = (uint8_t *)Memory;
  gs_binary_insertion_tree *Tree = (gs_binary_insertion_tree *)Memory;

  uint32_t Count = __gs_ClosestCount(NumItems);
  uint32_t ItemsSize = Count * sizeof(void *);
  uint32_t MarkedSize = Count * sizeof(int);
  Tree->Count = Count;
  Tree->Items = (void *)(MemoryBase + sizeof(gs_binary_insertion_tree));
  Tree->Marked = (int *)(MemoryBase + sizeof(gs_binary_insertion_tree) + ItemsSize);

  memset(Tree->Marked, 0, Count);

  return Tree;
}

bool
gs_Insert(gs_binary_insertion_tree *Tree, void *Value, gs_comparison_function_ptr Comp)
{
  uint32_t i = 0;

  do {
    if (!Tree->Marked[i]) {
      Tree->Marked[i] = 1;
      Tree->Items[i] = Value;
      return true;
    }
    else {
      void *Sentinel = Tree->Items[i];
      int InsertionIndex = __gs_GetLeftRightIndex(Tree, i);
      if (InsertionIndex < 0) { return false; }

      int Diff = Comp(Sentinel, Value);

      if (Diff < 0) {
        i = InsertionIndex;
        continue;
      }
      else if (Diff > 0) {
        i = InsertionIndex + 1;
        continue;
      }
      else {
        return false;
      }
    }
  } while (i >= 0);

  return false;
}

/*************************************************************************************************************
 * Client Usage
 ************************************************************************************************************/

int MyComparisonFunction(void *Left, void *Right) {
  int MyLeft = *(int*)Left;
  int MyRight = *(int*)Right;

  if (MyLeft < MyRight) {
    return -1;
  }
  else if (MyLeft > MyRight) {
    return 1;
  }
  else {
    return 0;
  }
}

int main(int ArgC, char **ArgV) {
  uint32_t NumItems = 12;
  uint32_t RequiredSize = gs_GetNumBytesRequired(NumItems);

  int Values[6] = {5, 6, 8, 3, 2, 4};

  void *Memory = malloc(RequiredSize);
  gs_binary_insertion_tree *Tree = gs_CreateBinaryInsertionTree(Memory, NumItems);

  for (int i = 0; i < 6; ++i) {
    if (!gs_Insert(Tree, &Values[i], MyComparisonFunction)) {
      printf("Couldn't insert %d into Tree!\n", Values[i]);
    }
  }

  printf("Presumably completed successfully!\n");

  return 0;
}
