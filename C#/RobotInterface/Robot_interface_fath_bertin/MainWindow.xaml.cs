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


        public MainWindow()
        {
            InitializeComponent();

            // Initialiser le port série avec les paramètres spécifiés
            serialPort1 = new ExtendedSerialPort("COM7", 115200, Parity.None, 8, StopBits.One);
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

            toggle = !toggle;
            if (toggle) {
                buttonEnvoyer.Background =   Brushes.Beige;
                // Vérifie si la TextBox "émission" contient du texte
                if (!string.IsNullOrEmpty(textBoxEmission.Text))
                {
                    SendMessage();

                    toggle = !toggle;

                }
                else
                {
                    // Si la TextBox "émission" est vide, tu peux afficher un message d'erreur ou autre
                    //MessageBox.Show("Veuillez entrer un message dans l'émission.");
                }
            }
            else
            {
                buttonEnvoyer.Background = Brushes.RoyalBlue;
            }

           
        }

        private void buttonClear_Click(object sender, RoutedEventArgs e)
        {

            toggle = !toggle;
            if (toggle)
            {
                buttonClear.Background = Brushes.Beige;
                // Vérifie si la TextBox "émission" contient du texte
                if (textBoxReception.Text != "")
                {
                    textBoxReception.Clear();

                }
            }
            else
            {
                buttonEnvoyer.Background = Brushes.RoyalBlue;
            }


            
        }

        private void buttonTest_Click(object sender, RoutedEventArgs e)
        {

            toggle = !toggle;
            if (toggle)
            {
                buttonTest.Background = Brushes.Beige;
                byte[] byteList = new byte[20];
                for(int i = 0; i<20; i++)
                {
                    byteList[i] = (byte)(2 * i);
                }
                serialPort1.Write(byteList, 0, byteList.Length);
            }
            else
            {
                buttonTest.Background = Brushes.RoyalBlue;
            }


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

       

        

    }
}