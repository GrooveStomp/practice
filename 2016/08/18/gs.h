/******************************************************************************
 * File: gs.h
 * Created: 2016-07-14
 * Last Updated: 2016-08-18
 * Creator: Aaron Oman (a.k.a GrooveStomp)
 * Notice: (C) Copyright 2016 by Aaron Oman
 *-----------------------------------------------------------------------------
 *
 * Standard library for personal use. Heavily influenced by Sean Barrett's stb.
 *
 ******************************************************************************/
#define GS_VERSION 0.1.0

#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* memset */
#include <stdarg.h> /* va_list */

#define GSArraySize(Array) (sizeof((Array)) / sizeof((Array)[0]))

/******************************************************************************
 * Usage:
 *
 * int Numbers[] = { 1, 2, 3, 4, 5 };
 * GSArrayForEach(int *Number, Numbers)
 * {
 *         printf("Number[%i]: %i\n", Index, *Number);
 * }
 *
 * NOTE:
 * The variable `Index' is automatically generated for you.
 * `Item' must be a pointer to the type of variable used in the Array.
 *
 * Implementation taken from: http://stackoverflow.com/a/400970
 ******************************************************************************/
#define GSArrayForEach(Item, Array) \
        for(int Keep##__LINE__ = 1, \
                Count##__LINE__ = 0, \
                Index = 0, \
                Size##__LINE__ = sizeof((Array)) / sizeof(*(Array)); \
            Keep##__LINE__ && Count##__LINE__ != Size##__LINE__; \
            Keep##__LINE__ = !Keep##__LINE__, Count##__LINE__++) \
                for(Item = (Array) + Count##__LINE__; Keep##__LINE__; Keep##__LINE__ = !Keep##__LINE__, Index++)

#define GSMax(A, B) ((A) < (B) ? (B) : (A))
#define GSMin(A, B) ((A) < (B) ? (A) : (B))

#define GSAbortWithMessage(...) \
        { \
                char String##__LINE__[256];                             \
                sprintf(String##__LINE__, "In %s() at line #%i: ", __func__, __LINE__); \
                fprintf(stderr, String##__LINE__);                       \
                __GSAbortWithMessage(__VA_ARGS__);                      \
        }

void
__GSAbortWithMessage(char *FormatString, ...)
{
        va_list Args;
        va_start(Args, FormatString);
        fprintf(stderr, FormatString, Args);
        exit(EXIT_FAILURE);
}

/******************************************************************************
 * Boolean Definitions
 ******************************************************************************/

typedef int gs_bool;
#ifndef false
#define false 0
#define true !false
#endif

/******************************************************************************
 * Character Definitions
 *-----------------------------------------------------------------------------
 * Functions to interact with C's basic ASCII char type.
 ******************************************************************************/

gs_bool
GSCharIsEndOfStream(char C)
{
	return(C == '\0');
}

gs_bool
GSCharIsEndOfLine(char C)
{
	return((C == '\n') ||
	       (C == '\r'));
}

gs_bool
GSCharIsWhitespace(char C)
{
	return((C == ' ') ||
	       (C == '\t') ||
	       (C == '\v') ||
	       (C == '\f') ||
	       GSCharIsEndOfLine(C));
}

gs_bool
GSCharIsOctal(char C)
{
	gs_bool Result = (C >= '0' && C <= '7');
	return(Result);
}

gs_bool
GSCharIsDecimal(char C)
{
	gs_bool Result = (C >= '0' && C <= '9');
	return(Result);
}

gs_bool
GSCharIsHexadecimal(char C)
{
	gs_bool Result = ((C >= '0' && C <= '9') ||
		       (C >= 'a' && C <= 'f') ||
		       (C >= 'A' && C <= 'F'));
	return(Result);
}

gs_bool
GSCharIsAlphabetical(char C)
{
	gs_bool Result = ((C >= 'a' && C <= 'z') || (C >= 'A' && C <= 'Z'));
	return(Result);
}

gs_bool
GSCharIsAlphanumeric(char C)
{
        gs_bool Result = GSCharIsAlphabetical(C) || GSCharIsDecimal(C);
        return(Result);
}

char
GSCharUpcase(char C)
{
        char Result = C;
        if(GSCharIsAlphabetical(C) &&
           (C >= 'a' && C <= 'z'))
        {
                int Delta = C - 'a';
                Result = Delta + 'A';
        }

        return(Result);
}

char
GSCharDowncase(char C)
{
        char Result = C;
        if(GSCharIsAlphabetical(C) &&
           (C >= 'A' &&C <= 'Z'))
        {
                int Delta = C - 'A';
                Result = Delta + 'a';
        }

        return(Result);
}

/******************************************************************************
 * String Definitions
 *-----------------------------------------------------------------------------
 * C string type. That is, ASCII characters with terminating NULL.
 ******************************************************************************/

gs_bool
GSStringIsEqual(char *LeftString, char *RightString, int MaxNumToMatch)
{
	int NumMatched = 0;

	while(NumMatched < MaxNumToMatch)
	{
		if(*LeftString == *RightString)
		{
			LeftString++;
			RightString++;
			NumMatched++;
		}
		else
		{
			return(false);
		}
	}

	return(true);
}

size_t
GSStringLength(char *String)
{
	char *P = String;
	while(*P != '\0') P++;
	return(P - String);
}

gs_bool
GSStringCopyWithNull(char *Source, char *Dest, int Max)
{
        if(Source == NULL || Dest == NULL)
        {
                return(false);
        }

        int I = 0;
        for(; Source[I] != '\0' && I < Max; I++)
        {
                Dest[I] = Source[I];
        }
        Dest[I] = '\0';

        return(true);
}

gs_bool
GSStringCopy(char *Source, char *Dest, int Max)
{
        if(Source == NULL || Dest == NULL)
        {
                return(false);
        }

        for(int I = 0; Source[I] != '\0' && I < Max; I++)
        {
                Dest[I] = Source[I];
        }

        return(true);
}

unsigned int /* Returns number of bytes copied. */
GSStringCopyWithoutSurroundingWhitespace(char *Source, char *Dest, unsigned int MaxLength)
{
        int FirstChar, LastChar;
        for(FirstChar = 0; GSCharIsWhitespace(Source[FirstChar]); FirstChar++);

        int StringLength = GSStringLength(Source);
        for(LastChar = StringLength - 1; GSCharIsWhitespace(Source[LastChar]); LastChar--);

        int Count = 0;
        for(int S=FirstChar; S<=LastChar && Count < MaxLength; Count++, S++)
        {
                Dest[Count] = Source[S];
        }

        return(Count);
}

unsigned int /* Returns number of bytes copied. */
GSStringCopyWithoutSurroundingWhitespaceWithNull(char *Source, char *Dest, unsigned int MaxLength)
{
        int FirstChar, LastChar;
        for(FirstChar = 0; GSCharIsWhitespace(Source[FirstChar]); FirstChar++);

        int StringLength = GSStringLength(Source);
        for(LastChar = StringLength - 1; GSCharIsWhitespace(Source[LastChar]); LastChar--);

        int Count = 0;
        for(int S=FirstChar; S<=LastChar && Count < MaxLength; Count++, S++)
        {
                Dest[Count] = Source[S];
        }
        Dest[Count] = '\0';

        return(Count);
}

/*
  eg.: hellOTherE -> Hellothere, my_friend -> MyFriend. Strips out
  non-alphanumeric chars.
*/
void
GSStringCapitalize(char *Source, char *Dest, unsigned int SourceLength)
{
        while(true)
        {
                if(GSCharIsAlphanumeric(*Source))
                        break;

                Source++;
                SourceLength--;
        }

        *Dest = GSCharUpcase(*Source);
        Dest++;

        gs_bool UpcaseNextChar = false;
        for(int I=1; I<SourceLength; I++)
        {
                if('_' == Source[I])
                {
                        UpcaseNextChar = true;
                }
                else if(GSCharIsAlphanumeric(Source[I]))
                {
                        if(UpcaseNextChar)
                        {
                                UpcaseNextChar = false;
                                *Dest = GSCharUpcase(Source[I]);
                                Dest++;
                        }
                        else
                        {
                                *Dest = GSCharDowncase(Source[I]);
                                Dest++;
                        }
                }
        }
}

/******************************************************************************
 * Hash Map
 *-----------------------------------------------------------------------------
 *
 * Usage:
 *     char *Value = "value";
 *     char *Result = NULL;
 *     gs_bool Found = false;
 *     gs_hash_map Map = GSHashMapCreate();
 *     GSHashMapAdd(&Map, "key", (void *)Value);
 *     Found = GSHashMapGet(&Map, "key", (void *)Result);
 *     if(Found) printf("Result: %s\n", Result);
 *
 ******************************************************************************/

#define GSHashMapInitialSize 50

typedef struct gs_hash_map {
        int Count;
        int Size;
        int *Keys;
        void *(*Values);
} gs_hash_map;

int
__GSHashMapCompute(char *Key)
{
        /*
          sdbm hash function: http://stackoverflow.com/a/14409947
        */
        int HashAddress = 0;
        for(int Counter = 0; Key[Counter] != '\0'; Counter++)
        {
                HashAddress = Key[Counter] +
                        (HashAddress << 6) +
                        (HashAddress << 16) -
                        HashAddress;
        }
        return(HashAddress);
}

size_t
GSHashMapSpaceRequired(int NumKeys)
{
        if(NumKeys == -1)
        {
                NumKeys = GSHashMapInitialSize;
        }

        int SizeOfKeys = sizeof(int) * NumKeys;
        int SizeOfValues = sizeof(void *) * NumKeys;
        int SizeToAlloc = SizeOfKeys + SizeOfValues + sizeof(gs_hash_map);
        return(SizeToAlloc);
}

gs_hash_map *
__GSHashMapAlloc(int NumKeys, void *Memory)
{
        gs_hash_map *NewHash;

        if(NumKeys == -1)
        {
                NumKeys = GSHashMapInitialSize;
        }

        int SizeToAlloc = GSHashMapSpaceRequired(NumKeys);
        int SizeOfKeys = sizeof(int) * NumKeys;

        if(Memory == NULL)
        {
                NewHash = (gs_hash_map*)calloc(1, SizeToAlloc);
        }
        else
        {
                NewHash = (gs_hash_map*)Memory;
                memset((void *)NewHash, 0, sizeof(SizeToAlloc));
        }

        NewHash->Keys = (int *)((char *)NewHash + sizeof(gs_hash_map));
        NewHash->Values = (void *)((char *)NewHash + sizeof(gs_hash_map) + SizeOfKeys);
        NewHash->Size = NumKeys;

        return(NewHash);
}

/*
  NumKeys: Number of initial key/value pairs expected. Specify -1 to use default.
  Memory:  Memory buffer to place HashMap into. Specify NULL to use malloc.
*/
gs_hash_map *
GSHashMapCreate(int NumKeys, void *Memory) {
        gs_hash_map *NewHash = __GSHashMapAlloc(NumKeys, Memory);
        return(NewHash);
}

gs_bool
GSHashMapAdd(gs_hash_map *Hash, char *Key, void *Value) {
        if(Hash->Size == Hash->Count) return false;

        int IntKey = __GSHashMapCompute(Key);
        Hash->Keys[Hash->Count] = IntKey;
        Hash->Values[Hash->Count++] = Value;
        return(true);
}

gs_bool
GSHashMapGet(gs_hash_map *Hash, char *Key, void **Value) {
        int IntKey = __GSHashMapCompute(Key);
        for(int i = 0; i < Hash->Count; ++i)
        {
                if(Hash->Keys[i] == IntKey)
                {
                        *Value = Hash->Values[i];
                        return true;
                }
        }
        return(false);
}

/******************************************************************************
 * Arg Parsing
 ******************************************************************************/

typedef struct gs_args
{
        int Count;
        char **Args;
} gs_args;

void
GSArgInit(gs_args *Self, int ArgCount, char **Args)
{
        Self->Count = ArgCount;
        Self->Args = Args;
}

char *
GSArgProgramName(gs_args *Self)
{
        return(Self->Args[0]);
}

gs_bool
GSArgIsPresent(gs_args *Args, char *Wanted)
{
        int StringLength = GSStringLength(Wanted);
        for(int I=0; I<Args->Count; I++)
        {
                if(GSStringIsEqual(Wanted, Args->Args[I], StringLength))
                {
                        return(true);
                }
        }
        return(false);
}

int /* Returns -1 if Arg not found. */
GSArgIndex(gs_args *Args, char *Wanted)
{
        int StringLength = GSStringLength(Wanted);
        for(int I=0; I<Args->Count; I++)
        {
                if(GSStringIsEqual(Wanted, Args->Args[I], StringLength))
                {
                        return(I);
                }
        }
        return(-1);
}

char * /* Returns NULL if Index is invalid. */
GSArgAtIndex(gs_args *Args, int Index)
{
        if((Index < 0) ||
           (Index > (Args->Count - 1)))
                return(NULL);
        else
                return(Args->Args[Index]);
}

char * /* Returns NULL if Marker is not found or no trailing arg. */
GSArgAfter(gs_args *Args, char *Marker)
{
        int Index = GSArgIndex(Args, Marker);
        if(Index < 0) return(NULL);

        char *Arg = GSArgAtIndex(Args, Index + 1);
        return(Arg);
}

gs_bool
GSArgHelpWanted(gs_args *Args)
{
        if(GSArgIsPresent(Args, "-h") ||
           GSArgIsPresent(Args, "--help"))
                return(true);
        else
                return(false);
}

/******************************************************************************
 * Byte streams / Buffers / File IO
 ******************************************************************************/

typedef struct gs_buffer
{
        char *Start;
        char *Cursor;
        size_t Capacity;
        size_t Length;
        char *SavedCursor;
} gs_buffer;

gs_buffer *
GSBufferInit(gs_buffer *Buffer, char *Start, size_t Size)
{
        Buffer->Start = Start;
        Buffer->Cursor = Start;
        Buffer->Length = 0;
        Buffer->Capacity = Size;
        Buffer->SavedCursor = NULL;
        return(Buffer);
}

gs_bool
GSBufferIsEOF(gs_buffer *Buffer)
{
        int Size = Buffer->Cursor - Buffer->Start;
        gs_bool Result = Size >= Buffer->Length;
        return(Result);
}

void
GSBufferNextLine(gs_buffer *Buffer)
{
        while(true)
        {
                if(Buffer->Cursor[0] == '\n' ||
                   Buffer->Cursor[0] == '\0')
                {
                        break;
                }
                Buffer->Cursor++;
        }
        Buffer->Cursor++;
}

gs_bool
GSBufferSaveCursor(gs_buffer *Buffer)
{
        Buffer->SavedCursor = Buffer->Cursor;
        return(true);
}

gs_bool
GSBufferRestoreCursor(gs_buffer *Buffer)
{
        if(Buffer->SavedCursor == NULL) return(false);

        Buffer->Cursor = Buffer->SavedCursor;
        Buffer->SavedCursor = NULL;
        return(true);
}

size_t
GSFileSize(char *FileName)
{
        size_t FileSize = 0;
        FILE *File = fopen(FileName, "r");
        if(File != NULL)
        {
                fseek(File, 0, SEEK_END);
                FileSize = ftell(File);
                fclose(File);
        }
        return(FileSize);
}

gs_bool
GSFileCopyToBuffer(char *FileName, gs_buffer *Buffer)
{
        FILE *File = fopen(FileName, "r");
        if(File == NULL) return(false);

        fseek(File, 0, SEEK_END);
        size_t FileSize = ftell(File);
        int Remaining = (Buffer->Start + Buffer->Capacity) - Buffer->Cursor;
        if(FileSize > Remaining) return(false);

        fseek(File, 0, SEEK_SET);
        fread(Buffer->Cursor, 1, FileSize, File);
        Buffer->Length += FileSize;
        Buffer->Cursor += FileSize;
        *(Buffer->Cursor) = '\0';

        return(true);
}
