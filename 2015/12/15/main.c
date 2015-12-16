#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define hsh_InitialSize 50
struct hsh_hash {
  uint32_t Count;
  uint32_t Size;
  uint32_t *Keys;
  void *(*Values);
};

uint32_t __hsh_ComputeHash(char *Key) {
  uint32_t Length = strlen(Key);
  uint32_t MultiplyAccumulator = 1;
  uint32_t AddAccumulator = 0;

  for (int i = 0; i < Length; ++i) {
    MultiplyAccumulator *= Key[i];
    AddAccumulator += Key[i];
  }

  return Length + MultiplyAccumulator + AddAccumulator;
}

struct hsh_hash *__hsh_Alloc(uint32_t NumItems) {
  uint32_t SizeOfKeys = sizeof(uint32_t) * NumItems;
  uint32_t SizeToAlloc = SizeOfKeys * 2 + sizeof(struct hsh_hash);

  struct hsh_hash *NewHash = (struct hsh_hash*)malloc(SizeToAlloc);

  NewHash->Keys = (uint32_t *)((uint8_t *)&NewHash + sizeof(struct hsh_hash));
  NewHash->Values = (void *)((uint8_t *)&NewHash + sizeof(struct hsh_hash) + SizeOfKeys);
  NewHash->Size = NumItems;

  return NewHash;
}

struct hsh_hash *hsh_CreateHash() {
  struct hsh_hash *NewHash = __hsh_Alloc(hsh_InitialSize);
  return NewHash;
}

bool hsh_Add(struct hsh_hash *Hash, char *Key, void *Value) {
  if (Hash->Size == Hash->Count) { return false; }

  uint32_t IntKey = __hsh_ComputeHash(Key);
  Hash->Keys[Hash->Count] = IntKey;
  Hash->Values[Hash->Count++] = Value;
  return true;
}

bool hsh_Get(struct hsh_hash *Hash, char *Key, void **Value) {
  uint32_t IntKey = __hsh_ComputeHash(Key);
  for (int i = 0; i < Hash->Count; ++i) {
    if (Hash->Keys[i] == IntKey) {
      *Value = Hash->Values[i];
      return true;
    }
  }

  return false;
}

int main(int ArgC, char **ArgV) {
  int Var1 = 27;
  char *Var2 = "The number 4";

  struct hsh_hash *Hash = hsh_CreateHash();
  hsh_Add(Hash, "Var 1", &Var1);
  hsh_Add(Hash, "Var 2", &Var2);

  void *Result;

  if (hsh_Get(Hash, "Var 2", &Result)) {
    fprintf(stdout, "Var 2: %s\n", *(char**)Result);
  }
  else {
    fprintf(stdout, "Couldn't fetch result for Var 2\n");
  }

  if (hsh_Get(Hash, "Var 1", &Result)) {
    fprintf(stdout, "Var 1: %d\n", *(int*)Result);
  }
  else {
    fprintf(stdout, "Couldn't fetch result for Var 1\n");
  }

  return 0;
}
