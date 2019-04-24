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
    public partial class FormDateDelete : Form
    {
        public FormDateDelete()
        {
            InitializeComponent();
            dtDate.Value = DateTime.Today.AddDays(-7);
        }

        private void btOk_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.OK;
        }

        private void btCancel_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
        }

        public DateTime Date
        {
            get { return dtDate.Value; }
        }
    }
}
