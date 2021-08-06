/* \brief
 *		symbols APIs
 */

#include "cc.h"
#include "symbols.h"
#include "types.h"
#include "logger.h"

#include <string.h>


#define SYM_HASHSIZE 256

typedef struct tagCCSymbolTable {
	int _level;
	struct tagCCSymbolTable* _previous;
	struct tagSymEntry {
		struct tagCCSymbol _sym;
		struct tagSymEntry* _link;
	} *_buckets[SYM_HASHSIZE];

	struct tagCCSymbol* _all; /* symbols in this level scope */
} FCCSymbolTable;

static struct tagCCSymbolTable sConstants;
static struct tagCCSymbolTable sExternals;
static struct tagCCSymbolTable sIdentifiers;
static struct tagCCSymbolTable sLabels;
static struct tagCCSymbolTable sTypes;

struct tagCCSymbolTable* gConstants = &sConstants;
struct tagCCSymbolTable* gExternals = &sExternals;
struct tagCCSymbolTable* gGlobals = &sIdentifiers;
struct tagCCSymbolTable* gIdentifiers = &sIdentifiers;
struct tagCCSymbolTable* gLabels = &sLabels;
struct tagCCSymbolTable* gTypes = &sTypes;

int gCurrentLevel = SCOPE_GLOBAL;
static int slabelId = 0;
static int stempId = 0;


static void cc_init_table(FCCSymbolTable* tp, int level)
{
	tp->_level = level;
	tp->_previous = NULL;
	tp->_all = NULL;
	memset(tp->_buckets, 0, sizeof(tp->_buckets));
}

/* create a symbol table */
static FCCSymbolTable* cc_new_table(FCCSymbolTable* up, int level)
{
	FCCSymbolTable* tp = (FCCSymbolTable*)mm_alloc_area(sizeof(FCCSymbolTable), CC_MM_TEMPPOOL);
	
	if (!tp) {
		logger_output_s("out of memory at %s:%d\n", __FILE__, __LINE__);
		return NULL;
	}

	cc_init_table(tp, level);
	tp->_previous = up;
	if (up) {
		tp->_all = up->_all;
	}

	return tp;
}

void cc_symbol_init()
{
	cc_init_table(&sConstants, SCOPE_CONST);
	cc_init_table(&sLabels, SCOPE_LABEL);
	cc_init_table(&sExternals, SCOPE_GLOBAL);
	cc_init_table(&sIdentifiers, SCOPE_GLOBAL);
	cc_init_table(&sTypes, SCOPE_GLOBAL);
	gCurrentLevel = SCOPE_GLOBAL;
	slabelId = 1;
}

int cc_symbol_genlabel(int cnt)
{
	slabelId += cnt;
	return slabelId - cnt;
}

void cc_symbol_enterscope()
{
	if (++gCurrentLevel == SCOPE_LOCAL)
	{
		stempId = 0;
	}
}

void cc_symbol_exitscope()
{
	cc_type_remove(gCurrentLevel);

	if (gTypes->_level == gCurrentLevel) {
		gTypes = gTypes->_previous;
	}
	if (gIdentifiers->_level == gCurrentLevel) {
		gIdentifiers = gIdentifiers->_previous;
	}

	assert(gCurrentLevel >= SCOPE_GLOBAL);
	--gCurrentLevel;
}

FCCSymbol* cc_symbol_install(const char* name, struct tagCCSymbolTable** tpp, int level, enum EMMArea where)
{
	FCCSymbolTable* tp = *tpp;
	struct tagSymEntry* p;
	int h = (int)name & (SYM_HASHSIZE - 1);

	assert(level == SCOPE_GLOBAL || level >= tp->_level);
	if (tp->_level < level) {
		tp = *tpp = cc_new_table(tp, level);
	}

	assert(tp);
	p = (struct tagSymEntry*)mm_alloc_area(sizeof(struct tagSymEntry), where);
	if (!p) {
		logger_output_s("out of memory: %s:%d\n", __FILE__, __LINE__);
		return NULL;
	}

	memset(&p->_sym, 0, sizeof(p->_sym));
	p->_sym._type = NULL;
	p->_sym._name = name;
	p->_sym._scope = level;
	p->_sym._up = tp->_all;
	tp->_all = &p->_sym;
	p->_link = tp->_buckets[h];
	tp->_buckets[h] = p;
	return &p->_sym;
}

FCCSymbol* cc_symbol_relocate(const char* name, struct tagCCSymbolTable* src, struct tagCCSymbolTable* dst)
{
	struct tagSymEntry *p, **q;
	struct tagCCSymbol **r;
	int h = (int)name & (SYM_HASHSIZE - 1);
	
	for (q = &src->_buckets[h]; *q; q = &(*q)->_link)
	{
		if (name == (*q)->_sym._name)
			break;
	} /* end for q */
	assert(*q);

	/* remove the entry from src's hash chain and from its list of all symbols */
	p = *q;
	*q = (*q)->_link;
	for (r = &src->_all; *r && *r != &p->_sym; r = &(*r)->_up)
	{
		/* do nothing */
	}

	assert(*r == &p->_sym);
	*r = p->_sym._up;

	/* insert the entry into dst's hash chain and into its list of all symbols */
	p->_link = dst->_buckets[h];
	dst->_buckets[h] = p;
	p->_sym._up = dst->_all;
	dst->_all = &p->_sym;
	return &p->_sym;
}

FCCSymbol* cc_symbol_lookup(const char* name, struct tagCCSymbolTable* tp)
{
	struct tagSymEntry* p;
	int h = (int)name & (SYM_HASHSIZE - 1);

	assert(tp);
	do 
	{
		for (p = tp->_buckets[h]; p; p = p->_link)
		{
			if (name == p->_sym._name)
				return &p->_sym;
		}
	} while ((tp = tp->_previous) != NULL);

	return NULL;
}

FCCSymbol* cc_symbol_constant(struct tagCCType* ty, FValue val)
{
	/* TODO: */
	return NULL;
}

