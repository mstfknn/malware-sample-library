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
	public partial class FormConfig : Form
	{
		BindingList<Config.IpPort> src;
		public FormConfig()
		{
			InitializeComponent();
			src = new BindingList<Config.IpPort>();
			foreach (Config.IpPort i in Program.cfg.servers)
			{
				src.Add(new Config.IpPort(i));
			}
			gridServers.DataSource = src;
		}

		private void btOk_Click(object sender, EventArgs e)
		{
			Program.cfg.servers.Clear();
			foreach (Config.IpPort i in src)
				Program.cfg.servers.Add(i);
			Program.cfg.Save();
			DialogResult = System.Windows.Forms.DialogResult.OK;
		}

		private void btCancel_Click(object sender, EventArgs e)
		{
			DialogResult = System.Windows.Forms.DialogResult.Cancel;
		}


	}
}
