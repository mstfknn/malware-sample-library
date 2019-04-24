using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace RDPPlayer
{
    public partial class FormRDP : Form
    {
        public FormRDP()
        {
            InitializeComponent();
        }

        void AddRDP(string ipServer, int portServer, string ipClient, string user, string psw)
        {
            TabPage tab = new TabPage("RDP: " + ipServer + " <-> " + ipClient);
            ControlRDP rdp = new ControlRDP(ipServer, portServer, ipClient, user, psw);
            tab.Controls.Add(rdp);
            rdp.Dock = DockStyle.Fill;
            tabRDP.TabPages.Add(tab);
            tabRDP.SelectedIndex = tabRDP.TabPages.Count - 1;
        }

        void AddVNC(string ipServer, int portServer, string ipClient)
        {
            TabPage tab = new TabPage("VNC: " + ipServer + " <-> " + ipClient);
            ControlVNC vnc = new ControlVNC(ipServer, portServer, ipClient);
            tab.Controls.Add(vnc);
            vnc.Dock = DockStyle.Fill;
            tabRDP.TabPages.Add(tab);
            tabRDP.SelectedIndex = tabRDP.TabPages.Count - 1;
        }

        private void itemConnectRDP_Click(object sender, EventArgs e)
        {
            FormConnectRDP form = new FormConnectRDP();
            if (form.ShowDialog() == DialogResult.OK)
            {
                AddRDP(form.IPServer, form.PortServer, form.IPClient, form.NameUser, form.Password);
            }
        }

        private void itemConnectVNC_Click(object sender, EventArgs e)
        {
            FormConnectVNC form = new FormConnectVNC();
            if (form.ShowDialog() == DialogResult.OK)
            {
                AddVNC(form.IPServer, form.PortServer, form.IPClient);
            }
        }

        private void itemCloseConnect_Click(object sender, EventArgs e)
        {
            if (tabRDP.SelectedIndex >= 0)
            {
                TabPage tab = tabRDP.TabPages[tabRDP.SelectedIndex];
                if (Output.MsgQue("Вы уверены что соединение " + tab.Text + " нужно закрыть?") == DialogResult.Yes)
                {
                    Control uc = tab.Controls[0];
                    if (uc is ControlRDP)
                        ((ControlRDP)uc).Close();
                    else
                        if (uc is ControlVNC)
                            ((ControlVNC)uc).Close();
                    tabRDP.TabPages.Remove(tab);
                    tab.Dispose();
                }
            }
        }
    }
}
