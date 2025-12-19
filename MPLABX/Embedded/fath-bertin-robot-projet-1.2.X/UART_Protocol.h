/* 
 * File:   UART_Protocol.h
 * Author: E306-PC6
 *
 * Created on 12 décembre 2025, 14:18
 */

#ifndef UART_PROTOCOL_H
#define	UART_PROTOCOL_H

unsigned char UartCalculateChecksum(int msgFunction, int msgPayloadLength, unsigned char* msgPayload);
void UartEncodeAndSendMessage(int msgFunction,int msgPayloadLength, unsigned char* msgPayload);
void UartDecodeMessage(unsigned char c);

#define SET_ROBOT_STATE 0x0051
#define SET_ROBOT_MANUAL_CONTROL 0x0052

void UartProcessDecodedMessage(unsigned char msgFunction,unsigned char msgPayloadLength, unsigned char msgPayload[]);

int SetRobotAutoControlState(unsigned char msgPayload);
void SetRobotState(unsigned char msgPayload);
#endif
