#include <xc.h>
#include "UART_Protocol.h"
#include "CB_TX1.h"
#include "Robot.h"
#include "main.h"

#define StateRobotWaiting 0
#define StateRobotFunctionMSB 1
#define StateRobotFunctionLSB 2
#define StateRobotPayloadLengthMSB 3
#define StateRobotPayloadLengthLSB 4
#define StateRobotPayload 5
#define StateRobotCheckSum 6

unsigned char UartCalculateChecksum(int msgFunction, int msgPayloadLength, unsigned char* msgPayload) {
    //Fonction prenant entree la trame et sa longueur pour calculer le checksum
    // Checksum XOR exactement comme en C#
    unsigned char checksum = 0x00;

    checksum ^= 0xFE; // SOF
    checksum ^= (msgFunction >> 8) & 0xFF; // Function MSB
    checksum ^= (msgFunction & 0xFF); // Function LSB
    checksum ^= (msgPayloadLength >> 8) & 0xFF; // Length MSB
    checksum ^= (msgPayloadLength & 0xFF); // Length LSB

    for (int i = 0; i < msgPayloadLength; i++)
        checksum ^= msgPayload[i];

    return checksum;
}

void UartEncodeAndSendMessage(int msgFunction, int msgPayloadLength, unsigned char* msgPayload) {
    //Fonction d?encodage et d?envoi d?un message
    int totalLength = msgPayloadLength + 6;
    unsigned char frame[totalLength];

    int index = 0;
    // 3?? Ajouter SOF
    frame[index++] = 0xFE;
    frame[index++] = msgFunction >> 8;
    frame[index++] = msgFunction >> 0;
    frame[index++] = msgPayloadLength >> 8;
    frame[index++] = msgPayloadLength >> 0;

    // 5?? Ajouter le payload
    for (int i = 0; i < msgPayloadLength; i++) {
        frame[index++] = msgPayload[i];
    }

    // 6?? Calculer le checksum avec la fonction précédente
    unsigned char checksum = UartCalculateChecksum(msgFunction, msgPayloadLength, msgPayload);

    // 7?? Ajouter le checksum à la trame
    frame[index++] = checksum;

    // 8?? Envoyer la trame sur l'UART
    //serialPort1.Write(frame, 0, totalLength);
    SendMessage(frame, totalLength);
}


int msgDecodedFunction = 0;
int msgDecodedPayloadLength = 0;
unsigned char msgDecodedPayload[128];
int msgDecodedPayloadIndex = 0;

void UartDecodeMessage(unsigned char c) {
    //Fonction prenant en entree un octet et servant a reconstituer les trames
    static int state = 0;

    switch (state) {
        case StateRobotWaiting:
            if (c == 0xFE) {
                msgDecodedPayloadIndex = 0;
                state = StateRobotFunctionMSB;
            }
            break;

        case StateRobotFunctionMSB:
            msgDecodedFunction = c << 8;
            state = StateRobotFunctionLSB;
            break;

        case StateRobotFunctionLSB:
            msgDecodedFunction |= c;
            state = StateRobotPayloadLengthMSB;
            break;

        case StateRobotPayloadLengthMSB:
            msgDecodedPayloadLength = c << 8;
            state = StateRobotPayloadLengthLSB;
            break;

        case StateRobotPayloadLengthLSB:
            msgDecodedPayloadLength |= c;
            msgDecodedPayloadIndex = 0;

            if (msgDecodedPayloadLength == 0)
                state = StateRobotCheckSum;
            else if (msgDecodedPayloadLength < 1024)
                state = StateRobotPayload;
            else
                state = StateRobotWaiting;
            break;

        case StateRobotPayload:
            msgDecodedPayload[msgDecodedPayloadIndex++] = c;

            if (msgDecodedPayloadIndex >= msgDecodedPayloadLength)
                state = StateRobotCheckSum;
            break;

        case StateRobotCheckSum:
        {
            unsigned char calc = UartCalculateChecksum(
                    msgDecodedFunction,
                    msgDecodedPayloadLength,
                    msgDecodedPayload
                    );

            if (calc == c) {
                UartProcessDecodedMessage(
                        msgDecodedFunction,
                        msgDecodedPayloadLength,
                        msgDecodedPayload
                        );
            }

            state = StateRobotWaiting;
            break;
        }
    }
}

void UartProcessDecodedMessage(unsigned char msgFunction, unsigned char msgPayloadLength, unsigned char msgPayload[]) {
    //Fonction appelée après le édcodage pour éxcuter l'action
    //correspondant au message reçu
    switch (msgFunction) {
        case SET_ROBOT_STATE:
            SetRobotState(msgPayload[0]);
            break;

        case SET_ROBOT_MANUAL_CONTROL:
            SetRobotAutoControlState(msgPayload[0]);
            break;

        default:
            break;

    }
}
//*************************************************************************/
//Fonctions correspondant aux messages
//*************************************************************************/

void SetRobotState(unsigned char msgPayload) {
    stateRobot = msgPayload;
    robotState.Mode = MODE_MANUEL;
}

int SetRobotAutoControlState(unsigned char msgPayload) {
    return robotState.Mode = MODE_AUTO;
}