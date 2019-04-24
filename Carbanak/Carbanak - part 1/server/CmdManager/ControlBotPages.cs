using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace CmdManager
{
	public partial class ControlBotPages : UserControl, IControlPage
	{
		public ControlBotPages()
		{
			InitializeComponent();
		}

		public void Start()
		{
		}

		public void Stop()
		{
			foreach (TabPage page in tabPages.TabPages)
			{
				foreach (Control control in page.Controls)
				{
					IControlPage controlPage = control as IControlPage;
					if (controlPage != null)
						controlPage.Stop();
				}	
			}
		}

		public void AddPage(IControlPage page, string name)
		{
			TabPage tabPage = new TabPage(name);
			UserControl control = page as UserControl;
			tabPage.Controls.Add(control);
			control.Dock = DockStyle.Fill;

			tabPages.TabPages.Add(tabPage);
			tabPages.SelectedTab = tabPage;
			page.Start();
		}

		private void tabPages_DrawItem(object sender, DrawItemEventArgs e)
		{
			Brush textBrush;
			string textPage = tabPages.TabPages[e.Index].Text;
			Color textBack = tabPages.TabPages[e.Index].BackColor;
			Rectangle textRect = tabPages.GetTabRect(e.Index);
			Font font;
			if (e.State == DrawItemState.Selected)
			{
				textBrush = new SolidBrush(Color.DarkBlue);
				font = e.Font;
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

		private void tabPages_MouseDown(object sender, MouseEventArgs e)
		{
			Rectangle textRect = tabPages.GetTabRect(tabPages.SelectedIndex);
			Bitmap bmp = global::CmdManager.Properties.Resources.close_16;
			Rectangle closeArea = new Rectangle(textRect.X + textRect.Width - bmp.Width, (textRect.Height - bmp.Height) / 2, bmp.Width, bmp.Height);
			if (closeArea.Contains(e.X, e.Y))
			{
				foreach (Control control in tabPages.SelectedTab.Controls)
				{
					IControlPage controlPage = control as IControlPage;
					if (controlPage != null)
						controlPage.Stop();
				}
				tabPages.TabPages.Remove(tabPages.SelectedTab);
			}
		}
	}
}
