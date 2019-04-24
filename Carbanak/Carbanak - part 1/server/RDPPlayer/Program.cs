using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace RDPPlayer
{
    static class Program
    {
        static public Config cfg;
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            cfg = new Config();
            cfg.Load();

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new FormRDP());

            Server.CloseAllServers();
            cfg.Save();
        }
    }
}
