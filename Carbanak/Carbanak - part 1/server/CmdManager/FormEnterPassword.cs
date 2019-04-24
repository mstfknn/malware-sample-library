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
	public partial class FormEnterPassword : Form
	{
		public FormEnterPassword(string ip, int port)
		{
			InitializeComponent();
			labelEnterPassword.Text = "Введите пароль для сервера " + ip + ':' + port.ToString();
		}

		private void btOk_Click(object sender, EventArgs e)
		{
			DialogResult = System.Windows.Forms.DialogResult.OK;
		}

		private void btCancel_Click(object sender, EventArgs e)
		{
			DialogResult = System.Windows.Forms.DialogResult.Cancel;
		}

		public string Password
		{
			get { return tbPassword.Text; }
		}
	}
}
