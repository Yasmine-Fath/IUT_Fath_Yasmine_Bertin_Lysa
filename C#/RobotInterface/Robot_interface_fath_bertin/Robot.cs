using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Robot_interface_fath_bertin
{
    internal class Robot
    {
        public string receivedText = "";
        public float distanceTelemetreDroit;
        public float distanceTelemetreCentre;
        public float distanceTelemetreGauche;
        public Queue<byte> byteListReceived = new Queue<byte>();
        public bool ModeAuto;
        public bool ModeManuel;

        public Robot()
        {
        }
        
    }
}
