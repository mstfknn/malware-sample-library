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
    public partial class FormConnectRDP : Form
    {
        public FormConnectRDP()
        {
            InitializeComponent();
            Program.cfg.InitCBServers(cbServer);
        }

        bool Verify()
        {
            /*
            if (cbServer.Text.Trim().Length == 0)
            {
                Output.MsgErr("Вы не указали IP сервера");
                return false;
            }
            */
            if( IPServer.Length > 0)
                if (!Misc.IsValidIP(IPServer))
                {
                    Output.MsgErr("IP адрес неверный");
                    return false;
                }
            if (tbClient.Text.Trim().Length == 0)
            {
                Output.MsgErr("Вы не указали IP клиента");
                return false;
            }
            if (!Misc.IsValidIP(IPClient))
            {
                Output.MsgErr("IP адрес клиента неверный");
                return false;
            }
            return true;
        }

        private void btConnect_Click(object sender, EventArgs e)
        {
            if (Verify())
            {
                Program.cfg.AddClient(tbClient.Text, tbUser.Text, tbPsw.Text);
                DialogResult = DialogResult.OK;
            }
        }

        private void btCancel_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
        }

        public string IPServer
        {
            get { return Config.Server.GetIP(cbServer.Text); }
        }

        public int PortServer
        {
            get { return Config.Server.GetPort(cbServer.Text); }
        }

        public string IPClient
        {
            get { return tbClient.Text.Trim(); }
        }

        public string NameUser
        {
            get { return tbUser.Text.Trim(); }
        }

        public string Password
        {
            get { return tbPsw.Text.Trim(); }
        }

        private void cbServer_Validating(object sender, CancelEventArgs e)
        {
            if (cbServer.SelectedIndex < 0 && cbServer.Text.Length != 0) //новое значение
            {
                string ip = IPServer;
                if (Misc.IsValidIP(ip))
                {
                    Program.cfg.AddServer(cbServer.Text);
                    cbServer.Items.Add(cbServer.Text);
                }
            }
        }

        private void tbClient_Validated(object sender, EventArgs e)
        {
            Config.Client cln = Program.cfg.GetClient(tbClient.Text);
            if (cln != null)
            {
                tbUser.Text = cln.user;
                tbPsw.Text = cln.psw;
            }
        }
    }
}
