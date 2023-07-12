/*
 * CAN.c
 *
 *  Created on: Jun 25, 2023
 *      Author: Maysara
 */

#include "stm32f1xx.h"
#include "stm32f103xb.h"
#include "CAN.h"

// 1) Initialize the CAN Peripheral
void CAN_Init(void)
{
  // Enable the CAN peripheral clock
  RCC->APB1ENR |= RCC_APB1ENR_CAN1EN;

  // Reset the CAN peripheral
  CAN1->MCR |= CAN_MCR_RESET;
  while (CAN1->MCR & CAN_MCR_RESET);

  // Configure the CAN peripheral
  CAN1->MCR &= ~CAN_MCR_TTCM;  // Time Triggered Communication mode disabled

  CAN1->MCR &= ~CAN_MCR_SLEEP; // Leave Sleep Mode
  while (CAN1->MSR & CAN_MSR_SLAK);

  CAN1->MCR |= CAN_MCR_INRQ;   // Initialization request
  while (!(CAN1->MSR & CAN_MSR_INAK)); // Wait for Initialization acknowledge

  // Set the CAN prescaler to achieve the desired baudrate
    uint32_t can_clk = SystemCoreClock / 2;
    uint16_t brp = can_clk / (Baudrate * 16); // Baud rate Formula
    CAN1->BTR = (brp - 1) | CAN_BTR_TS1_3TQ | CAN_BTR_TS2_2TQ | CAN_BTR_SJW_1TQ;

  CAN1->MCR &= ~CAN_MCR_INRQ;  // Enter Normal Mode
  while (CAN1->MSR & CAN_MSR_INAK); // Wait for Response !

  // Configure the GPIO pins for CAN1
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;
  GPIOA->CRH &= ~(GPIO_CRH_CNF11 | GPIO_CRH_MODE11 | GPIO_CRH_CNF12 | GPIO_CRH_MODE12);
  GPIOA->CRH |= GPIO_CRH_CNF11_0 | GPIO_CRH_CNF12_1 | GPIO_CRH_MODE12;
  // CAN_RX: PA11->Input Floating | CAN_RX: PA12->Output Alternate Function push-pull |
  // Maximum output Speed for PA12 = 50 MHz
}

// 2) Transmit 1 Frame
uint8_t CAN_Transmit_Frame(uint32_t id, char data[], uint8_t length)
{
  // Wait for any transmit mailbox to be empty
  while ( !(CAN1->TSR & (CAN_TSR_TME0 | CAN_TSR_TME1 | CAN_TSR_TME2)) );

  // Configure the transmit header using registers
  uint32_t txHeaderWord = 0;

  CAN1->sTxMailBox[0].TDLR = 0;
  CAN1->sTxMailBox[0].TDHR = 0;

  CAN1->sTxMailBox[1].TDLR = 0;
  CAN1->sTxMailBox[1].TDHR = 0;

  CAN1->sTxMailBox[2].TDLR = 0;
  CAN1->sTxMailBox[2].TDHR = 0;

  // Set the standard identifier
  txHeaderWord |= (id & 0x7FF) << 21;

  // Set the remote transmission request (RTR) bit
  txHeaderWord |= CAN_RTR_DATA << 1;

  // Set the identifier type to standard
  txHeaderWord |= CAN_ID_STD << 2;

  // Copy the data to the transmit buffer
  uint8_t mailbox = CAN_TransmitMailboxesNotEmpty;

  if ((CAN1->TSR & CAN_TSR_TME0))
  {
    mailbox = CAN_TransmitMailbox_0;

    for (int i = 0; i < length; i++)
    {
    	if (i < 4)
    		CAN1->sTxMailBox[0].TDLR |= (data[i] << (8 * i));
    	else
    		CAN1->sTxMailBox[0].TDHR |= (data[i] << (8 * (i-4)));
    }
  }
  else if ((CAN1->TSR & CAN_TSR_TME1))
  {
    mailbox = CAN_TransmitMailbox_1;

    for (int i = 0; i < length; i++)
    {
    	if (i < 4)
			CAN1->sTxMailBox[1].TDLR |= (data[i] << (8 * i));
		else
			CAN1->sTxMailBox[1].TDHR |= (data[i] << (8 * (i-4)));
    }
  }
  else if ((CAN1->TSR & CAN_TSR_TME2))
  {
    mailbox = CAN_TransmitMailbox_2;

    for (int i = 0; i < length; i++)
    {
		if (i < 4)
			CAN1->sTxMailBox[2].TDLR |= (data[i] << (8 * i));
		else
			CAN1->sTxMailBox[2].TDHR |= (data[i] << (8 * (i-4)));
    }
  }
  else
  {
	  return Error;
  }

  if (mailbox <= CAN_TransmitMailbox_2)
  {
	  // Write the header word to the appropriate transmit mailbox registers
	  CAN1->sTxMailBox[mailbox].TIR = txHeaderWord;

	  CAN1->sTxMailBox[mailbox].TDTR = 0; // Clear Whole Register
	  CAN1->sTxMailBox[mailbox].TDTR &= ~(0xFUL); // Clear bits [0:3]
	  CAN1->sTxMailBox[mailbox].TDTR |= length & 0xF; // Set bits [0:3] to the desired value
  }
  else
	  return Error;

  // Start transmission
  if (mailbox == CAN_TransmitMailbox_0)
  {
	  CAN1->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ_Msk;
	  while(!(CAN1->TSR & (CAN_TSR_RQCP0_Msk | CAN_TSR_TXOK0_Msk)));
	  CAN1->TSR |= (CAN_TSR_RQCP0_Msk | CAN_TSR_TXOK0_Msk); // Clear Flags
  }
  else if (mailbox == CAN_TransmitMailbox_1)
  {
	  CAN1->sTxMailBox[1].TIR |= CAN_TI1R_TXRQ_Msk;
	  while(!(CAN1->TSR & (CAN_TSR_RQCP1_Msk | CAN_TSR_TXOK1_Msk)));
	  CAN1->TSR |= (CAN_TSR_RQCP1_Msk | CAN_TSR_TXOK1_Msk); // Clear Flags
  }

  else if (mailbox == CAN_TransmitMailbox_2)
  {
	  CAN1->sTxMailBox[2].TIR |= CAN_TI2R_TXRQ_Msk;
	  while(!(CAN1->TSR & (CAN_TSR_RQCP2_Msk | CAN_TSR_TXOK2_Msk)));
	  CAN1->TSR |= (CAN_TSR_RQCP2_Msk | CAN_TSR_TXOK2_Msk); // Clear Flags
  }
  else {
	  return Error; // MISRA
  }

  return Successful;
}

//======================================================================================================

// 3) Transmit up to 3 Frames
uint8_t CAN_Transmit(uint32_t id, char data[], uint8_t length)
{
  // Abort any pending or scheduled Transmission to insure all 3 mailboxes are empty
//  CAN1->TSR &= ~(CAN_TSR_ABRQ0 | CAN_TSR_ABRQ1 | CAN_TSR_ABRQ2);

  // Wait for the 3 transmit mailboxes to be empty
  while ((CAN1->TSR & (CAN_TSR_TME0 | CAN_TSR_TME1 | CAN_TSR_TME2)) != (CAN_TSR_TME0 | CAN_TSR_TME1 | CAN_TSR_TME2));

  // Configure the transmit header using registers
  uint32_t txHeaderWord = 0;

  CAN1->sTxMailBox[0].TDLR = 0;
  CAN1->sTxMailBox[0].TDHR = 0;

  CAN1->sTxMailBox[1].TDLR = 0;
  CAN1->sTxMailBox[1].TDHR = 0;

  CAN1->sTxMailBox[2].TDLR = 0;
  CAN1->sTxMailBox[2].TDHR = 0;
  // Set the standard identifier
  txHeaderWord |= (id & 0x7FF) << 21;

  // Set the remote transmission request (RTR) bit
  txHeaderWord |= CAN_RTR_DATA << 1;

  // Set the identifier type to standard
  txHeaderWord |= CAN_ID_STD << 2;

  // Copy the data to the transmit buffer
  uint8_t mailbox = CAN_TransmitMailboxesNotEmpty;
  if (length <= 8)
  {
    mailbox = CAN_TransmitMailbox_0;


    for (int i = 0; i < length; i++)
    {
    	if (i < 4)
			CAN1->sTxMailBox[0].TDLR |= (data[i] << (8 * i));
		else
			CAN1->sTxMailBox[0].TDHR |= (data[i] << (8 * (i-4)));
    }
  }
  else if (length <= 16)
  {
    mailbox = CAN_TransmitMailbox_1;

    for (int i = 0; i < length - 8; i++) {
    	if (i < 4)
			CAN1->sTxMailBox[0].TDLR |= (data[i] << (8 * i));
		else
			CAN1->sTxMailBox[0].TDHR |= (data[i] << (8 * (i-4)));
    }
    for (int i = 8; i < length; i++) {
    	if (i < 12)
			CAN1->sTxMailBox[1].TDLR |= (data[i] << (8 * (i-8)));
		else
			CAN1->sTxMailBox[1].TDHR |= (data[i] << (8 * (i-12)));
    }
  }
  else if (length <= 24)
  {
    mailbox = CAN_TransmitMailbox_2;

    for (int i = 0; i < length - 16; i++) {
    	if (i < 4)
			CAN1->sTxMailBox[0].TDLR |= (data[i] << (8 * i));
		else
			CAN1->sTxMailBox[0].TDHR |= (data[i] << (8 * (i-4)));
    }
    for (int i = 8; i < length - 8; i++) {
    	if (i < 12)
			CAN1->sTxMailBox[1].TDLR |= (data[i] << (8 * (i-8)));
		else
			CAN1->sTxMailBox[1].TDHR |= (data[i] << (8 * (i-12)));
    }
    for (int i = 16; i < length; i++) {
    	if (i < 20)
			CAN1->sTxMailBox[2].TDLR |= (data[i] << (8 * (i-16)));
		else
			CAN1->sTxMailBox[2].TDHR |= (data[i] << (8 * (i-20)));
    }
  }
  else
	  return Error;

  if (mailbox <= CAN_TransmitMailbox_2)
  {
	  for (int i = 0; i <= mailbox; i++)
	  {
		  // Write the header word to the appropriate transmit mailbox registers
		  CAN1->sTxMailBox[i].TIR = txHeaderWord;

		  CAN1->sTxMailBox[i].TDTR = 0; // Clear Whole Register
		  CAN1->sTxMailBox[i].TDTR &= ~(0xFUL); // Clear bits [0:3]

		  if (i == 0)
		  {
			  if (length <= 8)
			  {
				  CAN1->sTxMailBox[0].TDTR |= length & 0xF; // Set bits [0:3] to the desired value
			  }
			  else if (length > 8 && length < 17)
			  {
				  CAN1->sTxMailBox[0].TDTR |= 8 & 0xF;
				  CAN1->sTxMailBox[1].TDTR |= (length-8) & 0xF;
			  }
			  else if (length > 16 && length < 25)
			  {
				  CAN1->sTxMailBox[0].TDTR |= 8 & 0xF;
				  CAN1->sTxMailBox[1].TDTR |= 8 & 0xF;
				  CAN1->sTxMailBox[2].TDTR |= (length-16) & 0xF;
			  }
			  else
				  return Error;
		  }
	  }
  }
  else
	  return Error;

  // Start transmission
  if (mailbox == CAN_TransmitMailbox_0)
  {
	  CAN1->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ_Msk;
	  while(!(CAN1->TSR & (CAN_TSR_RQCP0_Msk | CAN_TSR_TXOK0_Msk)));
	  CAN1->TSR |= (CAN_TSR_RQCP0_Msk | CAN_TSR_TXOK0_Msk); // Clear Flags
  }

  else if (mailbox == CAN_TransmitMailbox_1)
  {
	  CAN1->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ_Msk;
	  while(!(CAN1->TSR & (CAN_TSR_RQCP0_Msk | CAN_TSR_TXOK0_Msk)));
	  CAN1->TSR |= (CAN_TSR_RQCP0_Msk | CAN_TSR_TXOK0_Msk); // Clear Flags

	  CAN1->sTxMailBox[1].TIR |= CAN_TI1R_TXRQ_Msk;
	  while(!(CAN1->TSR & (CAN_TSR_RQCP1_Msk | CAN_TSR_TXOK1_Msk)));
	  CAN1->TSR |= (CAN_TSR_RQCP1_Msk | CAN_TSR_TXOK1_Msk); // Clear Flags
  }

  else if (mailbox == CAN_TransmitMailbox_2)
  {
	  CAN1->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ_Msk;
	  while(!(CAN1->TSR & (CAN_TSR_RQCP0_Msk | CAN_TSR_TXOK0_Msk)));
	  CAN1->TSR |= (CAN_TSR_RQCP0_Msk | CAN_TSR_TXOK0_Msk); // Clear Flags

	  CAN1->sTxMailBox[1].TIR |= CAN_TI1R_TXRQ_Msk;
	  while(!(CAN1->TSR & (CAN_TSR_RQCP1_Msk | CAN_TSR_TXOK1_Msk)));
	  CAN1->TSR |= (CAN_TSR_RQCP1_Msk | CAN_TSR_TXOK1_Msk); // Clear Flags

	  CAN1->sTxMailBox[2].TIR |= CAN_TI2R_TXRQ_Msk;
	  while(!(CAN1->TSR & (CAN_TSR_RQCP2_Msk | CAN_TSR_TXOK2_Msk)));
	  CAN1->TSR |= (CAN_TSR_RQCP2_Msk | CAN_TSR_TXOK2_Msk); // Clear Flags
  }

  else
	  return Error;

  return Successful;
}

//======================================================================================================

// 4) Receive a CAN message
void CAN_Receive(can_msg_t *msg) 
{
  // Check if a message is available
  if (CAN1->RF0R & CAN_RF0R_FMP0)
  {
    // Get the message identifier and length
    uint32_t tir = CAN1->sFIFOMailBox[0].RIR;
    msg->id = tir >> 21;
    msg->len = CAN1->sFIFOMailBox[0].RDTR & CAN_RDT0R_DLC;

    // Get the message data
    msg->data[0] = CAN1->sFIFOMailBox[0].RDLR & 0xFF;
    msg->data[1] = (CAN1->sFIFOMailBox[0].RDLR >> 8) & 0xFF;
    msg->data[2] = (CAN1->sFIFOMailBox[0].RDLR >> 16) & 0xFF;
    msg->data[3] = (CAN1->sFIFOMailBox[0].RDLR >> 24) & 0xFF;
    msg->data[4] = CAN1->sFIFOMailBox[0].RDHR & 0xFF;
    msg->data[5] = (CAN1->sFIFOMailBox[0].RDHR >> 8) & 0xFF;
    msg->data[6] = (CAN1->sFIFOMailBox[0].RDHR >> 16) & 0xFF;
    msg->data[7] = (CAN1->sFIFOMailBox[0].RDHR >> 24) & 0xFF;

    // Release the FIFO mailbox
    CAN1->RF0R |= CAN_RF0R_RFOM0;
  }
}
