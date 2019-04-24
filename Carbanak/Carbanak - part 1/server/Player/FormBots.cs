using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Player
{
    public partial class FormBots : Form
    {
        ServerBot server;

        public FormBots(ServerBot server)
        {
            InitializeComponent();
            this.server = server;
        }

        private void btOk_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void FormBots_Load(object sender, EventArgs e)
        {
            UpdateBots();
        }

        void UpdateBots()
        {
            lvBots.Items.Clear();
            ClientBot[] bots = server.ArrayBots;
            for (int i = 0; i < bots.Length; i++)
            {
                lvBots.Items.Add(bots[i].UID);
                lvBots.Items[i].SubItems.Add(bots[i].IP);
            }
        }

        private void btUpdate_Click(object sender, EventArgs e)
        {
            UpdateBots();
        }
    }
}
