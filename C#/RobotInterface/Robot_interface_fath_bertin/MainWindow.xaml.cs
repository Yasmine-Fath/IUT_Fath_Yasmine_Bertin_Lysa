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
        }

        private void TextBox_TextChanged(object sender, TextChangedEventArgs e)
        {

        }
        bool toggle;
        private void buttonEnvoyer_Click(object sender, RoutedEventArgs e)
        {
            
            if (toggle) {
                buttonEnvoyer.Background = Brushes.Beige; 
            }
            else
            {
                buttonEnvoyer.Background = Brushes.RoyalBlue;
            }
            toggle = !toggle;
        }
    }
}