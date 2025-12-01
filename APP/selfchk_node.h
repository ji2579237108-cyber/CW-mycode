#ifndef _SELFCHK_NODE_H_
#define _SELFCHK_NODE_H_

#include "selfdiag.h"

extern void DiagM24c64(void);
extern void DiagW25n01g(void);
extern void DiagSim7600(void);
extern void SelfChkExCanCtr(void);
extern void DiagFireKey(void);
extern void DiagLevel(void);
extern void SelfChkLevel(void);
extern ENU_DETCT_RESULT SelfChkExCan(void);
extern ENU_DETCT_RESULT DetctM24c64(void);
extern ENU_DETCT_RESULT DetctW25n01g(void);
extern ENU_DETCT_RESULT DetctSim7600(void);
extern ENU_DETCT_RESULT DetctFireKey(void);
extern ENU_DETCT_RESULT CheckFireKey(void);
extern ENU_DETCT_RESULT CheckLevel(void);

#endif
