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
	public partial class FormEnterText : Form
	{
		public FormEnterText(string caption)
		{
			InitializeComponent();
			this.Text = caption;
		}

		public string Content
		{
			get { return tbText.Text; }
		}

		private void btOk_Click(object sender, EventArgs e)
		{
			DialogResult = System.Windows.Forms.DialogResult.OK;
		}

		private void btCancel_Click(object sender, EventArgs e)
		{
			DialogResult = System.Windows.Forms.DialogResult.Cancel;
		}

	}
}
