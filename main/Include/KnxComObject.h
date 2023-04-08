
#ifndef KNXCOMOBJECT_H
#define KNXCOMOBJECT_H

#include "KnxTelegram.h"

#define KNXCOMOBJ_C_FLAG	(0x20U) /* Communication Flag (C) */ 
#define KNXCOMOBJ_R_FLAG	(0x10U) /* Read Flag (R) */ 
#define KNXCOMOBJ_W_FLAG	(0x08U) /* Write Flag (W) */ 
#define KNXCOMOBJ_T_FLAG	(0x04U) /* Transmit Flag (T) */ 
#define KNXCOMOBJ_U_FLAG	(0x02U) /* Update Flag (U) */ 
#define KNXCOMOBJ_I_FLAG	(0x01U) /* Initialization Flag (I) */ 

#define KNXCOMOBJ_OK    (0x00U)
#define KNXCOMOBJ_ERROR (0xFFU)


typedef struct {
    uint16_t addr;
    uint8_t dptId;
    uint8_t indicators;
    uint8_t length;
    uint8_t priority;
    bool valid;
    uint8_t * value;
} KnxComObject_Type;


void KnxComObject_CopyToTelegram(KnxComObject_Type * src, KnxTelegram_Type * dst);


#endif /* #ifndef KNXCOMOBJECT_H */