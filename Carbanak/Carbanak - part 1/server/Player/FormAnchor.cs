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
    public partial class FormAnchor : Form
    {
        public FormAnchor()
        {
            InitializeComponent();
        }

        private void btOk_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.OK;
        }

        public string Anchor
        {
            get { return tbAnchor.Text.Trim(); }
        }
    }
}
