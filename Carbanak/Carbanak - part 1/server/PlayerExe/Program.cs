using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using Player;
namespace PlayerExe
{
    static class Program
    {
        public static Player.Config cfg;
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            cfg = new Player.Config();
            cfg.Load();
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            FormPlayer pl = new FormPlayer();
            FormPlayer.cfg = cfg;
            Application.Run(pl);
        }
    }
}
