/* Example initialization:

   const unsigned int MaxStringLength = 256;
   size_t NumBytes = HashMapBytesRequired(MaxStringLength, 512);
   hash_map *HashMap = (hash_map *)alloca(NumBytes);
   HashMapInit(HashMap, MaxStringLength, 512);
*/
/*
  In-memory HashMap.
  Not suitable across application runs unless you can guarantee fixed memory addresses.
      This is because we use void* as the Value type. Otherwise, it's fine.
  Tries to be predictable with memory usage.
*/
#include <stdio.h>
#include <string.h> /* memset, memcpy */
#include <alloca.h>
#include <stdlib.h> /* rand, srand, used for testing. */
#include <time.h> /* time - used for srand. */
#include <libgen.h> /* POSIX basename */
#include "gs.h"

typedef struct
{
        unsigned int Count;
        size_t AllocatedBytes;
        unsigned int Capacity;
        unsigned int MaxKeyLength;

        char *Keys;
        void **Values;
} hash_map;

size_t HashMapBytesRequired(unsigned int MaxKeyLength, unsigned int NumEntries);
hash_map *HashMapInit(void *Memory, unsigned int MaxKeyLength, unsigned int NumEntries);
/* Guaranteed to produce a value in the range [0,Self->Capacity] */
unsigned int __HashMapComputeHash(hash_map *Self, char *String);
gs_bool HashMapGrow(hash_map **Self, unsigned int NumEntries, void *New);
gs_bool HashMapAdd(hash_map *Self, char *Key, void *Value);
void *HashMapGet(hash_map *Self, char *Key);
gs_bool HashMapHasKey(hash_map *Self, char *Key);
void *HashMapDelete(hash_map *Self, char *Key);
void PrintHashMap(hash_map *HashMap);

//#define DEBUG
#ifdef DEBUG
#define DebugPrint(...) printf(__VA_ARGS__); fflush(stdout);
#else
#define DebugPrint
#endif

unsigned int /* String must be a NULL-terminated string */
__HashMapComputeHash(hash_map *Self, char *String)
{
        /*
          sdbm hash function: http://stackoverflow.com/a/14409947
        */
        unsigned int HashAddress = 0;
        for(unsigned int Counter = 0; String[Counter] != '\0'; Counter++)
        {
                HashAddress = String[Counter] +
                        (HashAddress << 6) +
                        (HashAddress << 16) -
                        HashAddress;
        }
        unsigned int Result = HashAddress % Self->Capacity;
        return(Result);
}

size_t
HashMapBytesRequired(unsigned int MaxKeyLength, unsigned int NumEntries)
{
        int AllocSize =
                sizeof(hash_map) +
                (sizeof(char) * MaxKeyLength * NumEntries) +
                (sizeof(void *) * NumEntries);
}

hash_map *
HashMapInit(void *Memory, unsigned int MaxKeyLength, unsigned int NumEntries)
{
        DebugPrint("HashMapInit(%p, %u, %u)\n", Memory, MaxKeyLength, NumEntries);
        hash_map *Self = (hash_map *)Memory;

        char *KeyValueMemory = (char *)Memory + sizeof(hash_map);

        Self->MaxKeyLength = MaxKeyLength;
        Self->Capacity = NumEntries;
        Self->AllocatedBytes = HashMapBytesRequired(MaxKeyLength, NumEntries);
        Self->Count = 0;

        int KeysMemLength = MaxKeyLength * NumEntries;

        Self->Keys = KeyValueMemory;
        memset(Self->Keys, 0, KeysMemLength);

        Self->Values = (void **)(Self->Keys + KeysMemLength);
        memset(Self->Values, 0, (sizeof(void **) * NumEntries));

        return(Self);
}

gs_bool /* Memory must be large enough for the resized Hash. Memory _cannot_ overlap! */
HashMapGrow(hash_map **Self, unsigned int NumEntries, void *New)
{
        hash_map *Old = *Self;
        DebugPrint("HashMapGrow(%p, %u, %p)\n", (void *)Old, NumEntries, New);

        /* No point in making smaller... */
        if(NumEntries <= Old->Capacity) return(false);
        if(New == NULL) return(false);

        *Self = HashMapInit(New, Old->MaxKeyLength, NumEntries);
        for(int I=0; I<Old->Capacity; I++)
        {
                char *Key = &Old->Keys[I * Old->MaxKeyLength];
                char *Value = (char *)(Old->Values[I]);
                if(Key != NULL)
                {
                        gs_bool Success = HashMapAdd(*Self, Key, Value);
                        if(!Success)
                                GSAbortWithMessage("This should have worked!\n");
                }
        }

        return(true);
}

/*
  Input: Key as string
  Computation: Hash key value into an integer.
  Algorithm: Open-addressing hash. Easy to predict space usage.
             See: https://en.wikipedia.org/wiki/Open_addressing
  Key must be a NULL terminated string.
 */
gs_bool
HashMapAdd(hash_map *Self, char *Key, void *Value)
{
        DebugPrint("HashMapAdd(%p, %s, %p)\n", (void *)Self, Key, Value);
        if(Self->Count >= Self->Capacity) return(false);

        unsigned int KeyLength = GSStringLength(Key);
        unsigned int HashIndex = __HashMapComputeHash(Self, Key);
        DebugPrint("Key(%s) hashes to %i\n", Key, HashIndex);
        if(Self->Keys[HashIndex * Self->MaxKeyLength] == '\0')
        {
                GSStringCopyWithNull(Key, &Self->Keys[HashIndex * Self->MaxKeyLength], KeyLength);
                Self->Values[HashIndex] = Value;
                Self->Count++;
                return(true);
        }

        unsigned int StartHash = HashIndex;
        HashIndex = (HashIndex + 1) % Self->Capacity;

        while(true)
        {
                if(HashIndex == StartHash) break;

                if(Self->Keys[HashIndex * Self->MaxKeyLength] == '\0')
                {
                        GSStringCopyWithNull(Key, &Self->Keys[HashIndex * Self->MaxKeyLength], KeyLength);
                        Self->Values[HashIndex] = Value;
                        Self->Count++;
                        return(true);
                }
                HashIndex = (HashIndex + 1) % Self->Capacity;
        }

        return(false);
}

gs_bool /* Wanted must be a NULL terminated string */
HashMapHasKey(hash_map *Self, char *Wanted)
{
        DebugPrint("HashMapHasKey(%p, %s)\n", (void *)Self, Wanted);
        unsigned int HashIndex = __HashMapComputeHash(Self, Wanted);
        char *Key = &Self->Keys[HashIndex * Self->MaxKeyLength];
        if(GSStringIsEqual(Wanted, Key, GSStringLength(Key)))
        {
                return(true);
        }

        unsigned int StartHash = HashIndex;
        HashIndex = (HashIndex + 1) % Self->Capacity;

        while(true)
        {
                if(HashIndex == StartHash) break;

                Key = &Self->Keys[HashIndex * Self->MaxKeyLength];
                if(GSStringIsEqual(Wanted, Key, GSStringLength(Key)))
                {
                        return(true);
                }
                HashIndex = (HashIndex + 1) % Self->Capacity;
        }

        return(false);
}

void * /* Wanted must be a NULL terminated string */
HashMapGet(hash_map *Self, char *Wanted)
{
        DebugPrint("HashMapGet(%p, %s)\n", (void *)Self, Wanted);
        unsigned int HashIndex = __HashMapComputeHash(Self, Wanted);
        char *Key = &Self->Keys[HashIndex * Self->MaxKeyLength];
        DebugPrint("Getting Key(%s) at HashIndex(%i)\n", Key, HashIndex);
        if(GSStringIsEqual(Wanted, Key, GSStringLength(Key)))
        {
                void *Result = Self->Values[HashIndex];
                return(Result);
        }

        unsigned int StartHash = HashIndex;
        HashIndex = (HashIndex + 1) % Self->Capacity;

        while(true)
        {
                if(HashIndex == StartHash) break;

                Key = &Self->Keys[HashIndex * Self->MaxKeyLength];
                if(GSStringIsEqual(Wanted, Key, GSStringLength(Key)))
                {
                        void *Result = Self->Values[HashIndex];
                        return(Result);
                }
                HashIndex = (HashIndex + 1) % Self->Capacity;
        }

        return(NULL);
}

void * /* Wanted must be a NULL terminated string */
HashMapDelete(hash_map *Self, char *Wanted)
{
        DebugPrint("HashMapDelete(%p, %s)\n", (void *)Self, Wanted);
        unsigned int HashIndex = __HashMapComputeHash(Self, Wanted);
        char *Key = &Self->Keys[HashIndex * Self->MaxKeyLength];
        if(GSStringIsEqual(Wanted, Key, GSStringLength(Key)))
        {
                void *Result = Self->Values[HashIndex];
                Self->Values[HashIndex] = NULL;
                Self->Keys[HashIndex * Self->MaxKeyLength] = '\0';
                Self->Count--;
                return(Result);
        }

        unsigned int StartHash = HashIndex;
        HashIndex = (HashIndex + 1) % Self->Capacity;

        while(true)
        {
                if(HashIndex == StartHash) break;

                Key = &Self->Keys[HashIndex * Self->MaxKeyLength];
                if(GSStringIsEqual(Wanted, Key, GSStringLength(Key)))
                {
                        void *Result = Self->Values[HashIndex];
                        Self->Values[HashIndex] = NULL;
                        Self->Keys[HashIndex * Self->MaxKeyLength] = '\0';
                        Self->Count--;
                        return(Result);
                }
                HashIndex = (HashIndex + 1) % Self->Capacity;
        }

        return(NULL);
}

unsigned int
RandomString(char *Memory, unsigned int MaxLength, unsigned int MinLength)
{
        int Length = rand() % (MaxLength - MinLength) + MinLength;
        int Range = 'z' - 'a';
        for(int I=0; I<Length; I++)
        {
                Memory[I] = rand() % Range + 'a';
        }
        Memory[Length] = '\0';

        return(Length);
}

void
PrintHashMap(hash_map *HashMap)
{

        printf("--------------------------------------------------------------------------------\n");
        printf(" HashMap(%p)\n", (void *)HashMap);
        printf("--------------------------------------------------------------------------------\n");
        printf("HashMap(\n\tCapacity: %u, \n\tCount: %u, \n\tAllocatedBytes: %lu, \n\tMaxKeyLength: %u, \n\tKeys: %p, \n\tValues: %p)\n",
               HashMap->Capacity,
               HashMap->Count,
               HashMap->AllocatedBytes,
               HashMap->MaxKeyLength,
               (void *)HashMap->Keys,
               (void *)HashMap->Values);

        for(int I=0; I<HashMap->Capacity; I++)
        {
                char *Key = &HashMap->Keys[I * HashMap->MaxKeyLength];
                char *Value = (char *)HashMap->Values[I];
                if(Key[0] != '\0')
                        printf("Index %i: (%s, %s)\n", I, Key, (char *)Value);
        }
}

void
Usage(char *ProgramName)
{
        printf("Usage: %s count\n", basename(ProgramName));
        puts("");
        puts("Creates a hash map big enough to hold count items and then tests its functionality.");
        puts("count: Number of key/value pairs to store in the hash map.");
        puts("");
        exit(EXIT_SUCCESS);
}

int
main(int ArgCount, char **Arguments)
{
        gs_args Args;
        GSArgInit(&Args, ArgCount, Arguments);
        if(GSArgHelpWanted(&Args) || ArgCount == 1) Usage(GSArgProgramName(&Args));

        int Count = strtol(Arguments[1], NULL, 10);
        int MaxStringLength = 256;

        size_t NumBytes = HashMapBytesRequired(MaxStringLength, Count);
        DebugPrint("Requested Entries: %u\n", Count);
        DebugPrint("Requested MaxKeyLength: %u\n", MaxStringLength);
        DebugPrint("Required total size: %lu B, %.2f KB, %.2f MB\n",
               NumBytes,
               (double)NumBytes / 1024,
               (double)NumBytes / 1024 / 1024);

        hash_map *HashMap = HashMapInit(malloc(NumBytes), MaxStringLength, Count);
        // PrintHashMap(HashMap);

        /*--------------------------------------------------------------------
           Testing!
          --------------------------------------------------------------------*/

        char *Keys   = (char *)malloc(sizeof(char) * Count * 2 * MaxStringLength);
        char *Values = (char *)malloc(sizeof(char) * Count * 2 * MaxStringLength);
        char *ExtraKey = "Key N+1";
        char *ExtraValue = "Value N+1";

        for(int I=0; I<Count * 2; I++)
        {
                sprintf(&Keys[I * MaxStringLength], "Key %i", I);
                sprintf(&Values[I * MaxStringLength], "Value %i", I);
        }

        srand(time(NULL));

        /* Add key/value pairs */
        for(int I=0; I<Count; I++)
        {
                char *Key = &Keys[I * MaxStringLength];
                char *Value = &Values[I * MaxStringLength];

                gs_bool Added = HashMapAdd(HashMap, Key, Value);
                if(Added != true)
                        GSAbortWithMessage("Expected to be able to add (%s, %s)\n", Key, Value);
        }

        /* Interact with key/value pairs */
        for(int I=0; I<Count; I++)
        {
                char *Key = &Keys[I * MaxStringLength];
                char *Value = &Values[I * MaxStringLength];

                gs_bool HasKey = HashMapHasKey(HashMap, Key);
                if(HasKey != true)
                        GSAbortWithMessage("Expected to have key (%s)\n", Key);

                char *GetResult = HashMapGet(HashMap, Key);
                if(GetResult == NULL)
                        GSAbortWithMessage("Expected value for key (%s) not to be NULL\n", Key);
                if(!GSStringIsEqual(GetResult, Value, GSStringLength(Value)))
                        GSAbortWithMessage("Expected %s == %s\n", GetResult, Value);
        }

        // PrintHashMap(HashMap);

        /* Try to add in past capacity */
        {
                char *Key = ExtraKey;
                char *Value = ExtraValue;

                gs_bool Added = HashMapAdd(HashMap, Key, Value);
                if(Added == true)
                        GSAbortWithMessage("Expected not to be able to add (%s, %s)\n", Key, Value);

                gs_bool HasKey = HashMapHasKey(HashMap, Key);
                if(HasKey == true)
                        GSAbortWithMessage("Expected hash not to have key: (%s)\n", Key);
                char *GetResult = HashMapGet(HashMap, Key);
                if(GetResult != NULL)
                        GSAbortWithMessage("Expected hash to have value (%s) for key (%s)\n", Value, Key);

                void *DelResult = HashMapDelete(HashMap, Key);
                if(DelResult != NULL)
                        GSAbortWithMessage("Expected not to be able to delete missing key (%s)\n", Key);
        }

        /* Remove key/value pairs */
        for(int I=0; I<Count; I++)
        {
                char *Key = &Keys[I * MaxStringLength];
                char *Value = &Values[I * MaxStringLength];

                void *DelResult = HashMapDelete(HashMap, Key);
                if(DelResult == NULL)
                        GSAbortWithMessage("Expected to be able to delete (%s)\n", Key);
        }

        /* Growing */
        for(int I=0; I<Count; I++)
        {
                char *Key = &Keys[I * MaxStringLength];
                char *Value = &Values[I * MaxStringLength];

                gs_bool Added = HashMapAdd(HashMap, Key, Value);
                if(Added != true)
                        GSAbortWithMessage("Expected to be able to add (%s, %s)\n", Key, Value);
        }

        gs_bool GrowResult = HashMapGrow(&HashMap, 0, NULL);
        if(GrowResult == true)
                GSAbortWithMessage("Expected not to be able to shrink the hash table\n");
        GrowResult = HashMapGrow(&HashMap, Count * 2, NULL);
        if(GrowResult == true)
                GSAbortWithMessage("Expected not to be able to grow into NULL memory\n");

        int NewCount = Count * 2;
        void *Memory = malloc(NewCount);
        GrowResult = HashMapGrow(&HashMap, NewCount, Memory);
        if(GrowResult == false)
                GSAbortWithMessage("Expected to be able to grow into new memory\n");

        // PrintHashMap(HashMap);

        /* Interact with key/value pairs */
        for(int I=0; I<Count; I++)
        {
                char *Key = &Keys[I * MaxStringLength];
                char *Value = &Values[I * MaxStringLength];

                gs_bool HasKey = HashMapHasKey(HashMap, Key);
                if(HasKey != true)
                        GSAbortWithMessage("Expected to have key (%s)\n", Key);

                char *GetResult = HashMapGet(HashMap, Key);
                if(GetResult == NULL)
                        GSAbortWithMessage("Expected value for key (%s) not to be NULL\n", Key);
                if(!GSStringIsEqual(GetResult, Value, GSStringLength(Value)))
                {
                        printf("(%s, %s), result: '%s'\n", Key, Value, GetResult);
                        GSAbortWithMessage("Expected '%s' == '%s'\n", GetResult, Value);
                }
        }

        {
                char *Key = ExtraKey;
                char *Value = ExtraValue;

                gs_bool Added = HashMapAdd(HashMap, Key, Value);
                if(Added != true)
                        GSAbortWithMessage("Expected to be able to add (%s, %s)\n", Key, Value);
        }

        printf("All tests completed successfully!\n");

}
