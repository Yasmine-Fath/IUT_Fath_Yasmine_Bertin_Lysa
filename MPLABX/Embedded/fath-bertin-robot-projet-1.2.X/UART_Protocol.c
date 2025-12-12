#include <xc.h>
#include "UART_Protocol.h"
#include "CB_TX1.h"
unsigned char UartCalculateChecksum(int msgFunction, int msgPayloadLength, unsigned char* msgPayload)
{
//Fonction prenant entree la trame et sa longueur pour calculer le checksum
// Checksum XOR exactement comme en C#
unsigned char checksum = 0x00;

checksum ^= 0xFE;                                // SOF
checksum ^= (msgFunction >> 8) & 0xFF;           // Function MSB
checksum ^= (msgFunction & 0xFF);                // Function LSB
checksum ^= (msgPayloadLength >> 8) & 0xFF;      // Length MSB
checksum ^= (msgPayloadLength & 0xFF);           // Length LSB

for(int i = 0; i < msgPayloadLength; i++)
    checksum ^= msgPayload[i];

return checksum;
}

void UartEncodeAndSendMessage(int msgFunction,int msgPayloadLength, unsigned char* msgPayload)
{
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
for (int i = 0; i < msgPayloadLength; i++)
{
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

void UartDecodeMessage(unsigned char c)
{
//Fonction prenant en entree un octet et servant a reconstituer les trames
/*static int state = 0;

switch(state)
{
    case 0: // WAIT_SOF
        if(c == 0xFE)
        {
            msgDecodedPayloadIndex = 0;
            state = 1;
        }
        break;

    case 1: // Function MSB
        msgDecodedFunction = c << 8;
        state = 2;
        break;

    case 2: // Function LSB
        msgDecodedFunction |= c;
        state = 3;
        break;

    case 3: // PayloadLength MSB
        msgDecodedPayloadLength = c << 8;
        state = 4;
        break;

    case 4: // PayloadLength LSB
        msgDecodedPayloadLength |= c;

        if(msgDecodedPayloadLength == 0)
            state = 6; // checksum
        else
            state = 5; // payload
        break;

    case 5: // Payload
        msgDecodedPayload[msgDecodedPayloadIndex++] = c;

        if(msgDecodedPayloadIndex >= msgDecodedPayloadLength)
            state = 6;
        break;

    case 6: // Checksum
    {
        unsigned char calc = UartCalculateChecksum(
            msgDecodedFunction,
            msgDecodedPayloadLength,
            msgDecodedPayload
        );

        if(calc == c)
        {
            UartProcessDecodedMessage(
                msgDecodedFunction,
                msgDecodedPayloadLength,
                msgDecodedPayload
            );
        }

        state = 0;
        break;
    }
}*/
}
void UartProcessDecodedMessage(int function,int payloadLength, unsigned char* payload)
{
//Fonction appelee apres le decodage pour executer l?action
//correspondant au message recu
/*switch(function)
{
    case 0x0020: // LED
    {
        int led = payload[0];
        int etat = payload[1];

        if(led == 1) LATBbits.LATB0 = etat;
        if(led == 2) LATBbits.LATB1 = etat;
        if(led == 3) LATBbits.LATB2 = etat;
        break;
    }

    case 0x0030: // IR Distances
        // payload[0]=G, payload[1]=C, payload[2]=D
        break;

    case 0x0040: // Moteurs
    {
//        char vg = payload[0];
//        char vd = payload[1];
        // MotorSetSpeed(vg, vd);
        break;
    }

    case 0x0080: // Texte
        // payload = texte brut
        break;
}*/
}
//*************************************************************************/
//Fonctions correspondant aux messages
//*************************************************************************/

