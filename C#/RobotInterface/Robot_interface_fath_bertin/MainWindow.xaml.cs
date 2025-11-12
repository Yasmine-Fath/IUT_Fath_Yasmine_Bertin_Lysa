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

namespace Robot_interface_fath_bertin
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        
        public MainWindow()
        {
            InitializeComponent();

            // Initialiser le port série avec les paramètres spécifiés
            serialPort1 = new ExtendedSerialPort("COM3", 115200, Parity.None, 8, StopBits.One);
            try
            {
                serialPort1.Open();
                MessageBox.Show("Port série ouvert avec succès.");
            }
            catch (Exception ex)
            {
                MessageBox.Show("Erreur lors de l'ouverture du port série : " + ex.Message);
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
        
        bool toggle;
        int cpt = 0;
        ExtendedSerialPort serialPort1;

        private void TextBox_TextChanged(object sender, TextChangedEventArgs e)
        {

        }
        

        private void buttonEnvoyer_Click(object sender, RoutedEventArgs e)
        {
            
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
                    MessageBox.Show("Veuillez entrer un message dans l'émission.");
                }
            }
            else
            {
                buttonEnvoyer.Background = Brushes.RoyalBlue;
            }

           
            toggle = !toggle;
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
                // Incrémente le compteur de messages
                cpt++;

                // Affiche le message dans la TextBox "réception"
                textBoxReception.AppendText("Reçu : Message " + cpt + Environment.NewLine);

                // Envoi du message via le port série
                serialPort1.WriteLine(textBoxEmission.Text);

                // Vide la TextBox "émission" après envoi
                textBoxEmission.Clear();
            }
            else
            {
                // Si la TextBox "émission" est vide, tu peux afficher un message d'erreur ou rien faire
                MessageBox.Show("Veuillez entrer un message dans l'émission.");
            }
        }

       

        

    }
}