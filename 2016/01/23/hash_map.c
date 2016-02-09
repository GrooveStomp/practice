#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define InitialSize 50

struct hash_map {
  uint32_t Count;
  uint32_t Size;
  uint32_t *Keys;
  void *(*Values);
};

uint32_t __hash_map_compute(char *Key) {
  uint32_t Length = strlen(Key);
  uint32_t MultiplyAccumulator = 1;
  uint32_t AddAccumulator = 0;

  for (int i = 0; i < Length; ++i) {
    MultiplyAccumulator *= Key[i];
    AddAccumulator += Key[i];
  }

  return Length + MultiplyAccumulator + AddAccumulator;
}

struct hash_map *__hash_map_alloc(uint32_t NumItems) {
  uint32_t SizeOfKeys = sizeof(uint32_t) * NumItems;
  uint32_t SizeToAlloc = SizeOfKeys * 2 + sizeof(struct hash_map);

  struct hash_map *NewHash = (struct hash_map*)malloc(SizeToAlloc);
  memset((void *)NewHash, 0, sizeof(SizeToAlloc));

  NewHash->Keys = (uint32_t *)((uint8_t *)NewHash + sizeof(struct hash_map));
  NewHash->Values = (void *)((uint8_t *)NewHash + sizeof(struct hash_map) + SizeOfKeys);
  NewHash->Size = NumItems;

  return NewHash;
}

struct hash_map *hash_map_create() {
  struct hash_map *NewHash = __hash_map_alloc(InitialSize);
  return NewHash;
}

bool hash_map_add(struct hash_map *Hash, char *Key, void *Value) {
  if (Hash->Size == Hash->Count) { return false; }

  uint32_t IntKey = __hash_map_compute(Key);
  Hash->Keys[Hash->Count] = IntKey;
  Hash->Values[Hash->Count++] = Value;
  return true;
}

bool hash_map_get(struct hash_map *Hash, char *Key, void **Value) {
  uint32_t IntKey = __hash_map_compute(Key);
  for (int i = 0; i < Hash->Count; ++i) {
    if (Hash->Keys[i] == IntKey) {
      *Value = Hash->Values[i];
      return true;
    }
  }

  return false;
}
