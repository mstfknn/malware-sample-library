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
    public partial class RemoteControl : UserControl, IControlPage
    {
        public int port;
        public string host;
        public RemoteControl()
        {
            InitializeComponent();
        }

        private void RemoteControl_Load(object sender, EventArgs e)
        {
            remoteDesktop1.VncPort = port;
            remoteDesktop1.Connect(host, 0, false, false);
        }
        public void Start()
        {
        
        }

        public void Stop()
        {
      
        }
    }
}
