using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Server
{
	public partial class FormConfig : Form
	{
		BindingList<Config.Client> clients;
		public FormConfig()
		{
			InitializeComponent();
			tbPort.Text = Program.cfg.port.ToString();
			tbPortClient.Text = Program.cfg.portClient.ToString();
			gridIPClients.AutoGenerateColumns = false;
			clients = new BindingList<Config.Client>();
			foreach (Config.Client c in Program.cfg.clients)
			{
				clients.Add(new Config.Client(c));
			}
			gridIPClients.DataSource = clients;
		}

		bool Verify()
		{
			int port = 0;
			int.TryParse(tbPort.Text, out port);
			if (port <= 0 || port > 65535)
			{
				MessageBox.Show("Порт для ботов неверный", "Ошибка");
				return false;
			}
			int portClient = 0;
			int.TryParse(tbPortClient.Text, out portClient);
			if (portClient <= 0 || portClient > 65535)
			{
				MessageBox.Show("Порт для клиентов неверный", "Ошибка");
				return false;
			}
			Program.cfg.port = port;
			Program.cfg.portClient = portClient;
			Program.cfg.clients.Clear();
			foreach (Config.Client c in clients)
				Program.cfg.clients.Add(c);

			return true;
		}

		private void btOk_Click(object sender, EventArgs e)
		{
			if (Verify())
			{
				Program.cfg.Save();
				DialogResult = DialogResult.OK;
			}
		}

		private void btCancel_Click(object sender, EventArgs e)
		{
			DialogResult = DialogResult.Cancel;
		}

		private void btChangePsw_Click(object sender, EventArgs e)
		{
			if (!Program.cfg.CheckPassword(tbPassword.Text))
			{
				MessageBox.Show("Старый пароль неверный", "Ошибка");
				return;
			}
			if (tbNewPassword.Text != tbCheckPassword.Text)
			{
				MessageBox.Show("Потверждение пароля не совпадает", "Ошибка");
				return;
			}
			Program.cfg.ChangePassword(tbNewPassword.Text);
			MessageBox.Show("Пароль успешно изменен", "Поздравляю!");
		}



	}
}
