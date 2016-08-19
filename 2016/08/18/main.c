/* Example initialization:

   const unsigned int MaxStringLength = 256;
   size_t NumBytes = HashMapBytesRequired(MaxStringLength, 512);
   hash_map *HashMap = (hash_map *)alloca(NumBytes);
   HashMapInit(HashMap, MaxStringLength, 512);
/*
  In-memory HashMap.
  Not suitable across application runs unless you can guarantee fixed memory addresses.
      This is because we use void* as the Value type. Otherwise, it's fine.
  Tries to be predictable with memory usage.
*/
#include <stdio.h>
#include <string.h> /* memset, memcpy */
#include <alloca.h>
#include "gs.h"

size_t HashMapBytesRequired(unsigned int MaxKeyLength, unsigned int NumEntries);
void HashMapInit(hash_map *Self, unsigned int MaxKeyLength, unsigned int NumEntries);
/* Guaranteed to produce a value in the range [0,Self->MaxNumEntries] */
unsigned int __HashMapComputeHash(hash_map *Self, char *String, unsigned int StringLength);
gs_bool HashMapGrow(hash_map *Self, unsigned int NumEntries);
gs_bool HashMapAdd(hash_map *Self, char *Key, unsigned int KeyLength, void *Value);
void *HashMapGet(hash_map *Self, char *Key);
gs_bool HashMapHasKey(hash_map *Self, char *Key);
void *HashMapDelete(hash_map *Self, char *Key);

typedef struct
{
        unsigned int Count;
        size_t AllocatedBytes;
        unsigned int Capacity;
        unsigned int MaxKeyLength;

        char **Keys;
        void **Values;
} hash_map;

int
main(int ArgCount, char **Arguments)
{
        gs_args Args;
        GSArgInit(&Args, ArgCount, Arguments);

        hash_map *HashMap;
        size_t NumBytes = HashMapBytesRequired(256, 512);
        HashMapInit(HashMap, 256, 512);

        HashMapAdd("Aaron", ...);
        HashMapAdd("Johnny", ...);
        HashMapAdd("Pedro", ...);
        HashMapAdd("Rockford", ...);

        Result = HashMapGet(HashMap, "Aaron");
        Result = HashMapGet(HashMap, "Johnny");
        Result = HashMapGet(HashMap, "Pedro");
        Result = HashMapGet(HashMap, "Rockford");
}

size_t
HashMapBytesRequired(unsigned int MaxKeyLength, unsigned int NumEntries)
{
        int AllocSize =
                sizeof(unsigned int) + /* Count */
                sizeof(size_t) + /* AllocatedBytes */
                sizeof(unsigned int) + /* Capacity */
                sizeof(unsigned int) + /* MaxKeyLength */
                (sizeof(char) * MaxKeyLength * NumEntries) +
                (sizeof(void *) * NumEntries);
}

void
HashMapInit(hash_map *Self, unsigned int MaxKeyLength, unsigned int NumEntries)
{
        char *KeyValueMemory =
                sizeof(unsigned int) + /* Count */
                sizeof(size_t) + /* AllocatedBytes */
                sizeof(unsigned int) + /* Capacity */
                sizeof(unsigned int) ;/* MaxKeyLength */

        Self->MaxKeyLength = MaxKeyLength;
        Self->Capacity = NumEntries;
        Self->AllocatedBytes = HashMapBytesRequired(MaxKeyLength, NumEntries);
        Self->Count = 0;

        Self->Keys = KeyValueMemory;
        memset(Self->Keys, 0, (MaxKeyLength * NumEntries));

        Self->Values = KeyValueMemory + (MaxKeyLength * NumEntries);
        memset(Self->Keys, 0, (MaxKeyLength * NumEntries));
}

gs_bool /* Memory must be large enough for the resized Hash. Memory *can* overlap! */
HashMapGrow(hash_map *Self, unsigned int NumEntries, char *Memory)
{
        /* No point in making smaller... */
        if(NumEntries <= Self->Capacity) return(false);

        hash_map *Old = (hash_map *)alloca(Self->AllocatedBytes);
        memcpy((void *)Old, (void *)Self, Self->AllocatedBytes);

        Self = Memory;
        HashMapInit(Self, Old->MaxKeyLength, NumEntries);

        for(int I=0; I<Old->Capacity; I++)
        {
                char *Key = Self->Keys[I];
                if(Key != NULL)
                {
                        gs_bool Success = HashMapAdd(
                                Self,
                                Key,
                                GSStringLength(Key),
                                Self->Values[I]
                        );

                        if(!Success) GSAbortWithMessage("This should have worked!\n");
                }
        }

        return(true);
}

/*
  Input: Key as string
  Computation: Hash key value into an integer.
  Algorithm: Open-addressing hash. Easy to predict space usage.
             See: https://en.wikipedia.org/wiki/Open_addressing
 */
gs_bool
HashMapAdd(hash_map *Self, char *Key, unsigned int KeyLength, void *Value)
{
        if(Self->Count >= Self->Capacity) return(false);

        unsigned int HashIndex = __HashMapComputeHash(Self, Key, KeyLength);
        if(Self->Keys[HashIndex] == NULL)
        {
                GSStringCopyWithNull(Key, Self->Keys[HashIndex], KeyLength);
                Self->Values[HashIndex] = Value;
                Self->Count++;
                return(true);
        }

        unsigned int StartHash = HashIndex;
        HashIndex = (HashIndex + 1 % Self->Capacity);

        while(true)
        {
                if(HashIndex == StartHash) break;

                if(Self->Keys[HashIndex] == NULL)
                {
                        GSStringCopyWithNull(Key, Self->Keys[HashIndex], KeyLength);
                        Self->Values[HashIndex] = Value;
                        Self->Count++;
                        return(true);
                }
                HashIndex = (HashIndex + 1 % Self->Capacity);
        }

        return(false);
}

gs_bool
HashMapHasKey(hash_map *Self, char *Wanted)
{
        unsigned int HashIndex = __HashMapComputeHash(Self, Key);
        char *Key = Self->Keys[HashIndex];
        if(GSStringIsEqual(Wanted, Key, GSStringLength(Key)))
        {
                return(true);
        }

        unsigned int StartHash = HashIndex;
        HashIndex = (HashIndex + 1 % Self->Capacity);

        while(true)
        {
                if(HashIndex == StartHash) break;

                Key = Self->Keys[HashIndex];
                if(GSStringIsEqual(Wanted, Key, GSStringLength(Key)))
                {
                        return(true);
                }
        }

        return(false);
}

void *
HashMapGet(hash_map *Self, char *Key)
{
        unsigned int HashIndex = __HashMapComputeHash(Self, Key);
        char *Key = Self->Keys[HashIndex];
        if(GSStringIsEqual(Wanted, Key, GSStringLength(Key)))
        {
                void *Result = Self->Values[HashIndex];
                return(Result);
        }

        unsigned int StartHash = HashIndex;
        HashIndex = (HashIndex + 1 % Self->Capacity);

        while(true)
        {
                if(HashIndex == StartHash) break;

                Key = Self->Keys[HashIndex];
                if(GSStringIsEqual(Wanted, Key, GSStringLength(Key)))
                {
                        void *Result = Self->Values[HashIndex];
                        return(Result);
                }
        }

        return(NULL);
}

void *
HashMapDelete(hash_map *Self, char *Key)
{
        unsigned int HashIndex = __HashMapComputeHash(Self, Key);
        char *Key = Self->Keys[HashIndex];
        if(GSStringIsEqual(Wanted, Key, GSStringLength(Key)))
        {
                void *Result = Self->Values[HashIndex];
                Self->Values[HashIndex] = NULL;
                Self->Keys[HashIndex] = NULL;
                Self->Count--;
                return(Result);
        }

        unsigned int StartHash = HashIndex;
        HashIndex = (HashIndex + 1 % Self->Capacity);

        while(true)
        {
                if(HashIndex == StartHash) break;

                Key = Self->Keys[HashIndex];
                if(GSStringIsEqual(Wanted, Key, GSStringLength(Key)))
                {
                        void *Result = Self->Values[HashIndex];
                        Self->Values[HashIndex] = NULL;
                        Self->Keys[HashIndex] = NULL;
                        Self->Count--;
                        return(Result);
                }
        }

        return(NULL);
}
