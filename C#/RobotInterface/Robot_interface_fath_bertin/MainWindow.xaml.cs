using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using ExtendedSerialPort_NS;
using System.IO.Ports;
using System.Windows.Threading;
using System.Net;

namespace Robot_interface_fath_bertin
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        bool toggle;
        int cpt = 0;
        ExtendedSerialPort serialPort1;
        //string receivedText=""; //à été remplacé par : robot.receivedText.

        DispatcherTimer timerAffichage;
        Robot robot;

        byte previousStateRobot = 0xFF; // 0xFF = valeur invalide pour forcer le premier envoi
        uint timestamp = 0; // Timestamp en ms, à incrémenter dans ton timer


        public MainWindow()
        {
            InitializeComponent();

            // Initialiser le port série avec les paramètres spécifiés
            serialPort1 = new ExtendedSerialPort("COM11", 115200, Parity.None, 8, StopBits.One); //com à vérifier dans le gestionnaire de périferique -> Ports (COM et LPT)
            serialPort1.DataReceived += SerialPort1_DataReceived;
            serialPort1.Open();

            //Initialisation du timer affichage
            timerAffichage = new DispatcherTimer();
            timerAffichage.Interval = new TimeSpan(0, 0, 0, 0, 100);
            timerAffichage.Tick += TimerAffichage_Tick;
            timerAffichage.Start();

            //classe Robot
            robot = new Robot();
        }

        private void TimerAffichage_Tick(object? sender, EventArgs e)
        {
            //throw new NotImplementedException();
            if (robot.receivedText != "")
            {
                textBoxReception.AppendText(" Message Reçu : " + robot.receivedText);
                robot.receivedText = "";
            }
            if (robot.byteListReceived.Count > 0)
            {
                StringBuilder sbHex = new StringBuilder();

                while (robot.byteListReceived.Count > 0)
                {
                    byte b = robot.byteListReceived.Dequeue();
                    DecodeMessage(b);
                    // sbHex.Append("0x" + b.ToString("X2") + " "); 
                }

                // sbHex.Append("\r\n");
                //textBoxReception.AppendText(sbHex.ToString());
            }

        }

        public void SerialPort1_DataReceived(object sender, DataReceivedArgs e)
        {
            robot.receivedText += Encoding.UTF8.GetString(e.Data, 0, e.Data.Length);

            foreach (byte b in e.Data)
            {
                robot.byteListReceived.Enqueue(b);
            }

        }


        protected override void OnClosed(EventArgs e)
        {
            //Cette méthode s'assure que le port série est fermé lorsque la fenêtre est fermée.
            base.OnClosed(e);

            // Vérifie si le port série est encore ouvert et ferme-le proprement
            if (serialPort1.IsOpen)
            {
                serialPort1.Close();
            }
        }



        private void TextBox_TextChanged(object sender, TextChangedEventArgs e)
        {

        }


        private void buttonEnvoyer_Click(object sender, RoutedEventArgs e)
        {
            // Vérifie si la TextBox "émission" contient du texte
            if (!string.IsNullOrEmpty(textBoxEmission.Text))
            {
                SendMessage();
            }
            else
            {
                // Si la TextBox "émission" est vide, tu peux afficher un message d'erreur ou autre
                //MessageBox.Show("Veuillez entrer un message dans l'émission.");
            }


        }

        private void buttonClear_Click(object sender, RoutedEventArgs e)
        {
            // Vérifie si la TextBox "émission" contient du texte
            if (textBoxReception.Text != "")
            {
                textBoxReception.Clear();
            }

        }

        private void buttonTest_Click(object sender, RoutedEventArgs e)
        {
            //byte[] byteList = new byte[20];
            //for (int i = 0; i < 20; i++)
            //{
            //    byteList[i] = (byte)(2 * i);
            //}
            //serialPort1.Write(byteList, 0, byteList.Length);

            string messageString = "Bonjour";

            // 2️⃣ Convertir la chaîne en byte[]
            byte[] payload = Encoding.ASCII.GetBytes(messageString);

            // 3️⃣ Appeler la fonction d'envoi
            UartEncodeAndSendMessage(0x0080, payload.Length, payload);

            //Leds
            UartEncodeAndSendMessage(0x0020, 2, new byte[] { 1, 0 });
            UartEncodeAndSendMessage(0x0020, 2, new byte[] { 2, 1 });
            UartEncodeAndSendMessage(0x0020, 2, new byte[] { 3, 1 });

            //Telemetre
            UartEncodeAndSendMessage(0x0030, 3, new byte[] { 40, 50, 24 });

            //Vitesse
            UartEncodeAndSendMessage(0x0040, 2, new byte[] { 50, 100 });
        }

        private void TextBoxEmission_PreviewKeyDown(object sender, KeyEventArgs e)
        {
            // Vérifie si la touche pressée est Entrée
            if (e.Key == Key.Enter)
            {
                // Empêche le retour à la ligne

                e.Handled = true;

                // Appelle la méthode pour envoyer le message
                SendMessage();
            }
        }


        private void SendMessage()
        {
            if (!string.IsNullOrEmpty(textBoxEmission.Text)) // Vérifie que la TextBox n'est pas vide
            {
                // Envoi du message via le port série
                serialPort1.WriteLine(textBoxEmission.Text);

                // Vide la TextBox "émission" après envoi
                textBoxEmission.Clear();
            }
        }

        private byte CalculateChecksum(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            byte checksum = 0x00;

            // Ajouter le SOF
            checksum ^= 0xFE;

            // Ajouter les 2 octets de commande
            checksum ^= 0x00;           // premier octet de commande
            checksum ^= (byte)(msgFunction >> 8); // second octet de commande bit de poids fort 
            checksum ^= (byte)(msgFunction >> 0); // second octet de commande bit de poids faible 

            // Ajouter les 2 octets de la longeur du payload 
            checksum ^= 0x00;           // premier octet de commande
            checksum ^= (byte)(msgPayloadLength >> 8); // second octet de la longeur du payload bit de poids fort 
            checksum ^= (byte)(msgPayloadLength >> 0); // second octet de la longeur du payload bit de poids faible 

            // Ajouter les octets du payload
            for (int i = 0; i < msgPayloadLength; i++)
            {
                checksum ^= msgPayload[i];
            }

            return checksum;
        }

        private void UartEncodeAndSendMessage(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            int totalLength = msgPayloadLength + 6;
            byte[] frame = new byte[totalLength];

            int index = 0;
            // 3️⃣ Ajouter SOF
            frame[index++] = 0xFE;
            frame[index++] = (byte)(msgFunction >> 8);
            frame[index++] = (byte)(msgFunction >> 0);
            frame[index++] = (byte)(msgPayloadLength >> 8);
            frame[index++] = (byte)(msgPayloadLength >> 0);

            // 5️⃣ Ajouter le payload
            for (int i = 0; i < msgPayloadLength; i++)
            {
                frame[index++] = msgPayload[i];
            }

            // 6️⃣ Calculer le checksum avec la fonction précédente
            byte checksum = CalculateChecksum(msgFunction, msgPayloadLength, msgPayload);

            // 7️⃣ Ajouter le checksum à la trame
            frame[index++] = checksum;

            // 8️⃣ Envoyer la trame sur l'UART
            serialPort1.Write(frame, 0, totalLength);
        }


        public enum StateReception
        {
            Waiting,
            FunctionMSB,
            FunctionLSB,
            PayloadLengthMSB,
            PayloadLengthLSB,
            Payload,
            CheckSum
        }


        StateReception rcvState = StateReception.Waiting;
        int msgDecodedFunction = 0;
        int msgDecodedPayloadLength = 0;
        byte[] msgDecodedPayload;
        int msgDecodedPayloadIndex = 0;


        int calculatedChecksum = 0;
        byte receivedChecksum = 0;
        int VG = 0, VD = 0, DG = 0, DC = 0, DD = 0;


        private void DecodeMessage(byte c)
        {
            switch (rcvState)
            {
                case StateReception.Waiting:
                    if (c == 0xFE)
                    {
                        calculatedChecksum = 0;
                        calculatedChecksum ^= c;
                        rcvState = StateReception.FunctionMSB;
                    }
                    break;

                case StateReception.FunctionMSB:
                    msgDecodedFunction = c << 8;
                    rcvState = StateReception.FunctionLSB;
                    break;

                case StateReception.FunctionLSB:
                    msgDecodedFunction |= c;
                    rcvState = StateReception.PayloadLengthMSB;

                    break;

                case StateReception.PayloadLengthMSB:
                    msgDecodedPayloadLength = c << 8;
                    rcvState = StateReception.PayloadLengthLSB;
                    break;

                case StateReception.PayloadLengthLSB:
                    msgDecodedPayloadLength |= c;
                    msgDecodedPayload = new byte[msgDecodedPayloadLength];
                    msgDecodedPayloadIndex = 0;
                    rcvState = StateReception.Payload;
                    break;

                case StateReception.Payload:
                    msgDecodedPayload[msgDecodedPayloadIndex++] = c;
                    if (msgDecodedPayloadIndex >= msgDecodedPayloadLength)
                        rcvState = StateReception.CheckSum;
                    break;
                case StateReception.CheckSum:
                    calculatedChecksum = CalculateChecksum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
                    if (calculatedChecksum == c)
                    {
                        //Success, on a un message valide
                        //textBoxReception.AppendText("Success, on a un message valide"); // désactiver l’affichage de caractères reçus dans la console de réception
                                                                                         //en C# afin de ne pas surcharger l’affichage.
                        ProcessDecodedMessage(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
                    }
                    rcvState = StateReception.Waiting;

                    break;
                default:
                    rcvState = StateReception.Waiting;
                    break;
            }
        }


        public enum StateRobot
        {
            STATE_ATTENTE = 0,
            STATE_ATTENTE_EN_COURS = 1,
            STATE_AVANCE = 2,
            STATE_AVANCE_EN_COURS = 3,
            STATE_TOURNE_GAUCHE = 4,
            STATE_TOURNE_GAUCHE_EN_COURS = 5,
            STATE_TOURNE_DROITE = 6,
            STATE_TOURNE_DROITE_EN_COURS = 7,
            STATE_TOURNE_SUR_PLACE_GAUCHE = 8,
            STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS = 9,
            STATE_TOURNE_SUR_PLACE_DROITE = 10,
            STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS = 11,
            STATE_ARRET = 12,
            STATE_ARRET_EN_COURS = 13,
            STATE_RECULE = 14,
            STATE_RECULE_EN_COURS = 15
        }


        void ProcessDecodedMessage(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            if (msgFunction == 0x0020)
            {
                if (msgPayload[0] == 1)
                {
                    if (msgPayload[1] == 1)
                    {
                        checkBoxLed1.IsChecked = true;
                    }
                    else if (msgPayload[1] == 0)
                    {
                        checkBoxLed1.IsChecked = false;
                    }
                }

                if (msgPayload[0] == 2)
                {
                    if (msgPayload[1] == 1)
                    {
                        checkBoxLed2.IsChecked = true;
                    }
                    else if (msgPayload[1] == 0)
                    {
                        checkBoxLed2.IsChecked = false;
                    }
                }

                if (msgPayload[0] == 3)
                {
                    if (msgPayload[1] == 1)
                    {
                        checkBoxLed3.IsChecked = true;
                    }
                    else if (msgPayload[1] == 0)
                    {
                        checkBoxLed3.IsChecked = false;
                    }
                }


            }

            if (msgFunction == 0x0030)
            {
                TextBoxIRGauche.Text = $"IR Gauche : {msgPayload[0]} cm";
                TextBoxIRCentre.Text = $"IR Centre : {msgPayload[1]} cm";
                TextBoxIRDroit.Text = $"IR Droit : {msgPayload[2]} cm";
            }

            if (msgFunction == 0x0040)
            {
                TextBoxVitesseGauche.Text = $"Vitesse Gauche : {msgPayload[0]} %";
                TextBoxVitesseDroit.Text = $"Vitesse Droit : {msgPayload[1]} %";
                if (msgPayload[0] != previousStateRobot)
                {
                    SendStepInfo(msgPayload[0], timestamp);
                    previousStateRobot = msgPayload[0];
                }
                if (msgPayload[1] != previousStateRobot)
                {
                    SendStepInfo(msgPayload[1], timestamp);
                    previousStateRobot = msgPayload[1];
                }
            }

            if (msgFunction == 0x0050) {
                int instant = (((int)msgPayload[1]) << 24) + (((int)msgPayload[2]) << 16) + (((int)msgPayload[3]) << 8) + ((int)msgPayload[4]);
                //rtbReception.Text += "\nRobot␣State␣:␣" + ((StateRobot)(msgPayload[0])).ToString() + "␣-␣" + instant.ToString() + "␣ms";
                textBoxReception.AppendText($"\nRobot State: {((StateRobot)msgPayload[0]).ToString()} - {instant} ms");
            }



            }

        private void SendStepInfo(byte stepNumber, uint timestampMs)
        {
            // Préparer la payload de 5 octets
            byte[] payload = new byte[5];
            payload[0] = stepNumber;

            // Timestamp codé sur 4 octets, big-endian
            payload[1] = (byte)((timestampMs >> 24) & 0xFF);
            payload[2] = (byte)((timestampMs >> 16) & 0xFF);
            payload[3] = (byte)((timestampMs >> 8) & 0xFF);
            payload[4] = (byte)(timestampMs & 0xFF);

            // Envoyer le message avec l’ID 0x0050
            UartEncodeAndSendMessage(0x0050, payload.Length, payload);
        }

    }
}