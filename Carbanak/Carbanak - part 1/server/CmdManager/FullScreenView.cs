using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace CmdManager
{
    public partial class FullScreenView : Form
    {
        public List<BotPage> botPages;
        public int port;
        public FullScreenView()
        {
            InitializeComponent();
            botPages = new List<BotPage>();
        }
        public System.Windows.Forms.TabControl GetTabs() { return tabsOpenBots; }
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

        public void Stop()
        {
            foreach (BotPage bp in botPages)
            {
                if (bp.controls != null)
                    bp.controls.Stop();
            }
        }
        public void Init(Server.IdUid currBot, int style)
        {
            BotPage ret = botPages.Find(b => b.bot == currBot);
            if (ret == null)
            {
                ret = new BotPage();
                ret.bot = currBot;
                if (GetTabs().TabPages.Count == 1 && botPages.Count == 0) //вставляем 1-ю вкладку (одна пустая вкладка уже есть, для красоты)
                    ret.page = GetTabs().TabPages[0];
                else
                {
                    ret.page = new TabPage();
                    GetTabs().TabPages.Add(ret.page);
                }
                botPages.Add(ret);
                ret.page.Text = currBot.uid;
            }
            if (style == 0)
            { 
                ControlVideo cv = new ControlVideo(ret.bot);
                if (ret.controls == null)
                {
                    ret.controls = new ControlBotPages();
                    ret.controls.Dock = DockStyle.Fill;
                    ret.page.Controls.Add(ret.controls);
                    ret.controls.Start();
                }
                ret.controls.AddPage(cv, "Видео");
            }
            if (style == 1)
            {
                RemoteControl rc = new RemoteControl();
				rc.host = currBot.server.IP;// "127.0.0.1";//currBot.ip;
                string []ports;
                ports = currBot.ports.Split(' ');
                rc.port = Convert.ToInt32(ports[0]);
                if (ret.controls == null)
                {
                    ret.controls = new ControlBotPages();
                    ret.controls.Dock = DockStyle.Fill;
                    ret.page.Controls.Add(ret.controls);
                    ret.controls.Start();
                }
                ret.controls.AddPage(rc, "VNC");
            }
            GetTabs().SelectedTab = ret.page;
        }

        private void FullScreenView_FormClosed(object sender, FormClosedEventArgs e)
        {
            Stop();
        }
    }
}
