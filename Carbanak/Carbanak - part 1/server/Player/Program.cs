using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace Player
{
    static class Program
    {
        public static Config cfg;
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
            Application.Run(new FormPlayer());
        }
    }
}
