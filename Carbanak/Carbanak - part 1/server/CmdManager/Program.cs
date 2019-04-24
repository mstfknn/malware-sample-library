using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace CmdManager
{
	static class Program
	{
		static public Config cfg;
		static public FormMain formMain;
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
			formMain = new FormMain();
			Application.Run(formMain);
			Application.Exit();
		}

		static public void AddLog(string s)
		{
			formMain.Invoke(formMain.funcAddLog, new object[] { s });
		}
	}
}
