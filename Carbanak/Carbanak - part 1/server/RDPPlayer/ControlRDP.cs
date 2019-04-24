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
    public partial class ControlRDP : UserControl
    {
        RDP rdp;
        string user, psw;
        bool runRDP = false; //запущен ли RDP

        public ControlRDP(string ipServer, int portServer, string ipClient, string user, string psw)
        {
            InitializeComponent();
            rdp = new RDP(ipServer, portServer, ipClient, 3389);
            this.user = user;
            this.psw = psw;
            tbServer.Text = ipServer;
            tbClient.Text = ipClient;
            tbUser.Text = user;
        }

        public void Close()
        {
            if (runRDP)
            {
                if (axRDP.Connected != 0)
                    axRDP.Disconnect();
            }
            if (rdp != null) rdp.Stop();
        }

        void Connect(string ip, int port)
        {
            axRDP.Server = ip;
            axRDP.AdvancedSettings2.RDPPort = port;
            axRDP.UserName = user;
            axRDP.AdvancedSettings2.ClearTextPassword = psw;
            axRDP.DesktopHeight = 768;
            axRDP.DesktopWidth = 1024;
            axRDP.Connect();
            
        }

        private void ControlRDP_Load(object sender, EventArgs e)
        {
            if (rdp.IPServer.Length > 0)
            {
                rdp.Run();
                timer.Start();
            }
            else
            {
                Connect(rdp.IPClient, 3389);
                tbState.Text = "Подключен на прямую к порту 3389";
            }
        }

        private void timer_Tick(object sender, EventArgs e)
        {
            if (runRDP)
            {
                if (!rdp.IsPortOpen)
                {
                    tbState.Text = "Нет соединения";
                    runRDP = false;
                    if (axRDP.Connected != 0)
                        axRDP.Disconnect();
                    rdp.Run();
                }
            }
            else
                if (rdp.IsPortOpen)
                {
                    Connect(rdp.IPServer, rdp.PortTunnel);
                    tbState.Text = "Подключен через порт " + rdp.PortTunnel;
                    runRDP = true;
                }
                else
                {
                    tbState.Text = "Проброс порта";
                }
        }

        protected override void WndProc(ref Message m)
        {
            const int WM_MOUSEACTIVATE = 0x0021;

            //System.Diagnostics.Debug.WriteLine(m.Msg.ToString());
            //этот костыль для того чтобы поставить фокус на axRDP, так как при его потере он самом при клике мышей
            //не активировался, а в не фокусе компонент RDP не работает. Поэтому когда кликаем мышей, то шлется сообщение
            //WM_MOUSEACTIVATE и мы реагируем на него
            if (m.Msg == WM_MOUSEACTIVATE)
            {
                if (!axRDP.Focused)
                {
                    Point pm = axRDP.PointToClient(Cursor.Position);
                    if (pm.X >= 0 && pm.Y >= 0 && pm.X < axRDP.Width && pm.Y < axRDP.Height)
                        axRDP.Focus();
                }
            }

            base.WndProc(ref m);
        }

    }
}
