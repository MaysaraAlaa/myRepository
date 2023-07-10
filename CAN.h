/*
 * CAN.h
 *
 *  Created on: Jun 25, 2023
 *      Author: MiSo
 */

#ifndef SRC_CAN_DRIVER_CAN_H_
#define SRC_CAN_DRIVER_CAN_H_

#define Successful                    0
#define Error                         1

#define CAN_TransmitMailbox_0         0
#define CAN_TransmitMailbox_1         1
#define CAN_TransmitMailbox_2         2
#define CAN_TransmitMailboxesNotEmpty 4

#define CAN_RTR_DATA                  0
#define CAN_RTR_REMOTE                1

#define CAN_ID_STD                    0
#define CAN_ID_EXT                    1

#define CAN_BS1_0TQ                   0
#define CAN_BS1_1TQ                   1
#define CAN_BS1_2TQ                   2
#define CAN_BS1_3TQ                   3
#define CAN_BS1_4TQ                   4
#define CAN_BS1_5TQ                   5
#define CAN_BS1_6TQ                   6
#define CAN_BS1_7TQ                   7
#define CAN_BS1_8TQ                   8
#define CAN_BS1_9TQ                   9
#define CAN_BS1_10TQ                 10
#define CAN_BS1_11TQ                 11
#define CAN_BS1_12TQ                 12
#define CAN_BS1_13TQ                 13
#define CAN_BS1_14TQ                 14
#define CAN_BS1_15TQ                 15

#define CAN_BS2_0TQ                   0
#define CAN_BS2_1TQ                   1
#define CAN_BS2_2TQ                   2
#define CAN_BS2_3TQ                   3
#define CAN_BS2_4TQ                   4
#define CAN_BS2_5TQ                   5
#define CAN_BS2_6TQ                   6
#define CAN_BS2_7TQ                   7

// Define the CAN message structure
typedef struct {
  uint32_t id;       // Message ID
  uint8_t  data[8];  // Message data (up to 8 bytes)
  uint8_t  len;      // Message length (up to 8 bytes)
} can_msg_t;

/* 1 */    void CAN_Init(void);

/* 2 */ uint8_t CAN_Transmit_Frame(uint32_t id, char data[], uint8_t length);

/* 3 */ uint8_t CAN_Transmit(uint32_t id, char data[], uint8_t length);

/* 4 */    void CAN_Receive(can_msg_t *msg);

#endif /* SRC_CAN_DRIVER_CAN_H_ */
