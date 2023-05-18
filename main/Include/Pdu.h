#ifndef PDU_H
#define PDU_H

#include "esp_system.h"

/*
  * L_Data Standard_Frame

  +---------+---------+---------+---------+---------+---------+-----------------------------+---------+
  | octet 0 | octet 1 | octet 2 | octet 3 | octet 4 | octet 5 | octet 6 | octet 7 |   ...   | octet N |
  +---------+---------+---------+---------+---------+---------+-----------------------------+---------+
  |  CTRL   | SA (MSB)| SA (LSB)| DA (MSB)| DA (LSB)| LENGTH  |      TPDU (L_Data)          |  FCS    |
  +---------+---------+---------+---------+---------+---------+-----------------------------+---------+


  * NPDU :: The NPDU shall correspond to the LPDU of an L_Data-Frame without the Control Field,
            Source Address, Destination Address, Address Type Flag and the octet count.

  +-----------------+-----------------------------+
  |   octet 5       | octet 6 | octet 7 |   ...   |
  +-----------------+-----------------------------+
  | NPCI (bits 4-6) |      NSDU                   |
  +-----------------+-----------------------------+


  * TPDU :: The TPDU shall correspond to the NPDU, but reduced by the Network Control Field.

  +-----------------+-----------------------------+
  |   octet 6       | octet 6 | octet 7 |   ...   |
  +-----------------+-----------------------------+
  | TPCI (bits 2-7) |      TSDU                   |
  +-----------------+-----------------------------+


  * APDU :: The APDU shall correspond to the TPDU, but shall be reduced by the Transport Control field. 

  * The Application Control field shall have a length of 4 - or 10 bits, 
  * depending on the Application Layer service.

  +-----------------+-------------------------+-------------------+
  |   octet 6       | octet 7                 | octet 8 |   ...   |
  +-----------------+-------------------------+-------------------+
  | APCI (bits 0-1) | APCI (bits 6-7 to 0-7 ) +      ASDU         |
  +-----------------+-------------------------+-------------------+

*/
#define APDU_TPDU_OFFSET     (0U)
#define TPDU_NPDU_OFFSET     (1U)
#define NPDU_LPDU_OFFSET     (5U)
#define APDU_GROUPVALUE_RESPONSE_DATA_OFFSET (1U) /* APCI in octets 6-7, data starting from octet 8 */

#define EMI_FRAME_DATA_OFFSET          (0x06U)

/* octet 0 - Control Field Masks/Offsets/Values */
#define CTRL_FIELD_FRAME_FORMAT_MASK   (0x80U)
#define CTRL_FIELD_REPEAT_FLAG_MASK    (0x20U)
#define CTRL_FIELD_PRIORITY_MASK       (0x0CU)

#define CTRL_FIELD_FRAME_FORMAT_OFFSET (7U)
#define CTRL_FIELD_REPEAT_FLAG_OFFSET  (5U)
#define CTRL_FIELD_PRIORITY_OFFSET     (2U)

#define CTRL_FIELD_REPEATED_FRAME     (0x00U)
#define CTRL_FIELD_NON_REPEATED_FRAME (0x01U)
#define CTRL_FIELD_L_DATA_FRAME       (0x10U)
#define CTRL_FIELD_L_POLL_DATA_FRAME  (0xF0U)
#define CTRL_FIELD_ACK_FRAME          (0x00U)

/* octet 1 - CTRLE (optional) */
#define CTRLE_FIELD_SIZE                (1U)
#define CTRLE_FIELD_ADDRESS_TYPE_OFFSET (7U)
#define CTRLE_FIELD_HOP_COUNT_OFFSET    (4U)
#define CTRLE_FIELD_EFF_OFFSET          (0U)

#define CTRLE_FIELD_ADDRESS_TYPE_MASK (0x80U)
#define CTRLE_FIELD_HOP_COUNT_MASK    (0x70U)
#define CTRLE_FIELD_EFF_MASK          (0x0FU)

/* octet 5 - Length Field Masks */
#define LENGTH_FIELD_AT_HC_MASK        (0xF0U)
#define LENGHT_FIELD_ADDRESS_TYPE_MASK (0x80U)
#define LENGTH_FIELD_NPCI_MASK         (0x70U)
#define LENGTH_FIELD_LG_MASK           (0x0FU)

#define LENGTH_FIELD_ADDRESS_TYPE_OFFSET (7U)
#define LENGTH_FIELD_NPCI_OFFSET         (4U) /* Hop Count (bits 4-6) */

/* octet 6 - TPDU Field Masks */
#define TPDU_FIELD_TPCI_MASK (0xFCU)
#define TPDU_FIELD_APCI_MASK (0x03U)

/* octet 7 - APDU Field Masks */
/* Together with 2 bits in Octet 6, APCI shall have a length of */
/*  4 - 10 bits depending on Application Layer Service.         */
#define APDU_FIELD_APCI_MASK       (0x03U)
#define ASDU_FIELD_APCI_4BIT_MASK  (0xC0U)
#define ASDU_FIELD_APCI_10BIT_MASK (0xFFU)

/* octet N - FCS Field */
#define FCS_FIELD_SIZE (1U)

typedef enum {
  /* GroupValue */
  A_GroupValue_Read     = 0x00U,
  A_GroupValue_Response = 0x01U,
  A_GroupValue_Write    = 0x02U,

  /* Individual Address */
  A_IndividualAddress_Write    = 0x0C0U,
  A_IndividualAddress_Read     = 0x100U,
  A_IndividualAddress_Response = 0x140U,
} APCIType;

typedef enum {
  T_Data_Broadcast  = 0x00U,
  T_Data_Group      = 0x00U,
  T_Data_Tag_Group  = 0x04U,
  T_Data_Individual = 0x00U,
  T_Data_Connected  = 0x40U, /* 0x40 + (SeqNo << 2) */
  T_Data_Connect    = 0x80U,
  T_Data_Disconnect = 0x81U,
  T_Ack             = 0xC2U, /* 0xC2 + (SeqNo << 2) */
  T_Nak             = 0xC3U, /* 0xC3 + (SeqNo << 2) */
} TPCIType;

typedef enum {
    E_OK,
    E_NOT_OK,
} StatusType;

typedef struct {
  uint8_t * SduDataPtr;
  uint8_t SduLength;
} PduInfoType;

#endif /* #ifndef PDU_H */
