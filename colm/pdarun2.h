#ifndef _PDARUN2_H
#define _PDARUN2_H

#include "input.h"
#include "fsmrun2.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char Code;
typedef unsigned long Word;
typedef unsigned long Half;

typedef struct _File
{
	struct _File *prev;
	struct _File *next;
} File;

typedef struct _Location
{
	File *file;
	long line;
	long column;
	long byte;
} Location;

/* Header located just before string data. */
typedef struct _Head
{
	const char *data; long length;
	Location *location;
} Head;

typedef struct _Kid
{
	/* The tree needs to be first since pointers to kids are used to reference
	 * trees on the stack. A pointer to the word that is a Tree* is cast to
	 * a Kid*. */
	struct _Tree *tree;
	struct _Kid *next;
} Kid;

typedef struct _Ref
{
	Kid *kid;
	struct _Ref *next;
} Ref;

typedef struct _Tree
{
	/* First four will be overlaid in other structures. */
	short id;
	unsigned short flags;
	long refs;
	Kid *child;

	Head *tokdata;
} Tree;

typedef struct _ParseTree
{
	/* Entire structure must overlay Tree. */
	short id;
	unsigned short flags;
	long refs;
	Kid *child;

	Head *tokdata;

	/* Parsing algorithm. */
	long state;
	long region;
	char causeReduce;
	char retry_lower;
	char retry_upper;
} ParseTree;

typedef struct _Int
{
	/* Must overlay Tree. */
	short id;
	unsigned short flags;
	long refs;
	Kid *child;

	long value;
} Int;

typedef struct _Pointer
{
	/* Must overlay Tree. */
	short id;
	unsigned short flags;
	long refs;
	Kid *child;

	Kid *value;
} Pointer;

typedef struct _Str
{
	/* Must overlay Tree. */
	short id;
	unsigned short flags;
	long refs;
	Kid *child;

	Head *value;
} Str;

typedef struct _ListEl
{
	/* Must overlay kid. */
	Tree *value;
	struct _ListEl *next;
	struct _ListEl *prev;
} ListEl;

/*
 * Maps
 */
typedef struct _GenericInfo
{
	long type;
	long typeArg;
	long keyOffset;
	long keyType;
	long langElId;
	long parserId;
} GenericInfo;

typedef struct _List
{
	/* Must overlay Tree. */
	short id;
	unsigned short flags;
	long refs;
	ListEl *head;

	ListEl *tail;
	long listLen;
	GenericInfo *genericInfo;

} List;


void listAddAfter( List *list, ListEl *prev_el, ListEl *new_el );
void listAddBefore( List *list, ListEl *next_el, ListEl *new_el );

inline void listPrepend( List *list, ListEl *new_el) { listAddBefore(list, list->head, new_el); }
inline void listAppend( List *list, ListEl *new_el)  { listAddAfter(list, list->tail, new_el); }

ListEl *listDetach( List *list, ListEl *el );
inline ListEl *listDetachFirst(List *list )        { return listDetach(list, list->head); }
inline ListEl *listDetachLast(List *list )         { return listDetach(list, list->tail); }

inline long listLength(List *list)
	{ return list->listLen; }

typedef struct _Stream
{
	/* Must overlay Tree. */
	short id;
	unsigned short flags;
	long refs;
	Kid *child;

	FILE *file;
	InputStream *in;
} Stream;

typedef struct _FunctionInfo
{
	const char *name;
	long frameId;
	long argSize;
	long ntrees;
	long frameSize;
} FunctionInfo;

typedef struct _UserIter
{
	/* The current item. */
	Ref ref;
	Tree **stackRoot;
	long argSize;
	long stackSize;
	Code *resume;
	Tree **frame;
	long searchId;
} UserIter;

/*
 * Program Data.
 */

typedef struct _PatReplInfo
{
	long offset;
	long numBindings;
} PatReplInfo;

typedef struct _PatReplNode
{
	long id;
	long next;
	long child;
	long bindId;
	const char *data;
	long length;
	long ignore;

	/* Just match nonterminal, don't go inside. */
	bool stop;
} PatReplNode;

/* FIXME: should have a descriptor for object types to give the length. */

typedef struct _LangElInfo
{
	const char *name;
	bool repeat;
	bool list;
	bool literal;
	bool ignore;

	long frameId;

	long objectTypeId;
	long ofiOffset;
	long objectLength;

//	long contextTypeId;
//	long contextLength;

	long termDupId;
	long genericId;
	long markId;
	long captureAttr;
	long numCaptureAttr;
} LangElInfo;

typedef struct _ObjFieldInfo
{
	int typeId;
} ObjFieldInfo;

typedef struct _ProdInfo
{
	long length;
	unsigned long lhsId;
	const char *name;
	long frameId;
	bool lhsUpref;
} ProdInfo;

typedef struct _FrameInfo
{
	Code *codeWV;
	long codeLenWV;
	Code *codeWC;
	long codeLenWC;
	char *trees;
	long treesLen;
} FrameInfo;

typedef struct _RegionInfo
{
	const char *name;
	long defaultToken;
	long eofFrameId;
} RegionInfo;

typedef struct _CaptureAttr
{
	long mark_enter;
	long mark_leave;
	long offset;
} CaptureAttr;

typedef struct _RuntimeData
{
	LangElInfo *lelInfo;
	long numLangEls;

	ProdInfo *prodInfo;
	long numProds;

	RegionInfo *regionInfo;
	long numRegions;

	Code *rootCode;
	long rootCodeLen;

	FrameInfo *frameInfo;
	long numFrames;

	FunctionInfo *functionInfo;
	long numFunctions;

	PatReplInfo *patReplInfo;
	long numPatterns;

	PatReplNode *patReplNodes;
	long numPatternNodes;

	GenericInfo *genericInfo;
	long numGenerics;

	const char **litdata;
	long *litlen;
	Head **literals;
	long numLiterals;

	CaptureAttr *captureAttr;
	long numCapturedAttr;

	FsmTables *fsmTables;
	struct _PdaTables *pdaTables;
	int *startStates;
	int *eofLelIds;
	int *parserLelIds;
	long numParsers;

	long globalSize;

	long firstNonTermId;

	long integerId;
	long stringId;
	long anyId;
	long eofId;
	long noTokenId;
} RuntimeData;

typedef struct _PdaTables
{
	/* Parser table data. */
	int *indicies;
	int *keys;
	unsigned int *offsets;
	unsigned int *targs;
	unsigned int *actInds;
	unsigned int *actions;
	int *commitLen;
	int *tokenRegionInds;
	int *tokenRegions;

	int numIndicies;
	int numKeys;
	int numStates;
	int numTargs;
	int numActInds;
	int numActions;
	int numCommitLen;
	int numRegionItems;

	RuntimeData *rtd;
} PdaTables;

typedef struct _Program
{
	int argc;
	char **argv;

	bool ctxDepParsing;
	RuntimeData *rtd;
	Tree *global;

	PoolAlloc kidPool;
	PoolAlloc treePool;
	PoolAlloc parseTreePool;
	PoolAlloc listElPool;
	PoolAlloc mapElPool;
	PoolAlloc headPool;
	PoolAlloc locationPool;

	Tree *trueVal;
	Tree *falseVal;

	Kid *heap;

	Stream *stdinVal;
	Stream *stdoutVal;
	Stream *stderrVal;
} Program;


#ifdef __cplusplus
}
#endif

#endif
