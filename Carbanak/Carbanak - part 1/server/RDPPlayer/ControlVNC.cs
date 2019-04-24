using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using MSTSCLib;

namespace RDPPlayer
{
    public partial class ControlVNC : UserControl
    {
        VNC vnc;
        bool runVNC = false; //запущен ли RDP

        public ControlVNC(string ipServer, int portServer, string ipClient)
        {
            InitializeComponent();
            vnc = new VNC(ipServer, portServer, ipClient, 5900);
            tbServer.Text = ipServer;
            tbClient.Text = ipClient;
        }

        public void Close()
        {
            if (runVNC)
            {
                axVNC.Disconnect();
            }
            vnc.Stop();
        }

        private void ControlVNC_Load(object sender, EventArgs e)
        {
            vnc.Run();
            timer.Start();
        }

        private void timer_Tick(object sender, EventArgs e)
        {
            if (runVNC)
            {
                if (!axVNC.IsConnected)
                {
                    tbState.Text = "Нет соединения";
                    runVNC = false;
                    vnc.Run();
                }
            }
            else
                if (vnc.IsPortOpen)
                {
                    axVNC.VncPort = vnc.PortTunnel;
                    axVNC.Connect(vnc.IPServer, false, true);
                    tbState.Text = "Подключен через порт " + vnc.PortTunnel;
                    runVNC = true;
                }
                else
                {
                    tbState.Text = "Проброс порта";
                }
        }
    }
}
