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
void UartProcessDecodedMessage(int function,int payloadLength, unsigned char* payload);

#endif
