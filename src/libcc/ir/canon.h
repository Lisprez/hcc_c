/* \brief
 *		canon.h  
 * 1. convert the ir-tree to code-list (linearized).
 * 2. convert code-list to basic-blocks. 
 */

#ifndef __CANON_H__
#define __CANON_H__

#include "ir.h"

/* linearize expression to code list */
BOOL cc_canon_expr_linearize(FCCIRCodeList* list, FCCIRTree* expr, FCCSymbol* tlab, FCCSymbol* flab, FCCIRTree** outexpr, enum EMMArea where);

/* simplify code list */
BOOL cc_canon_codelist_simplify(FCCIRCodeList* list, enum EMMArea where);

/* generate basic blocks, basic block is begin with label(maybe implicit) end with jump(maybe implicit) */
FCCIRBasicBlock* cc_canon_gen_basicblocks(FCCIRCodeList* list, enum EMMArea where);

#endif /* __CANON_H__ */

