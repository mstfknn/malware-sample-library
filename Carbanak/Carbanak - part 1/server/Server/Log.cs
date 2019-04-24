using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Server
{
    public class Log
    {
        string nameFile;
        object lockObj;

        public Log()
        {
            nameFile = System.Windows.Forms.Application.StartupPath + "\\server" + DateTime.Now.ToString("yyyyMMddhhmmss") + ".log";
            lockObj = new object();
        }

        public void print(string s)
        {
            string ss = DateTime.Now.ToString("dd.MM.yyyy HH:mm:ss - ") + s;
            lock(lockObj)
            {
                System.IO.StreamWriter sw = new System.IO.StreamWriter(nameFile, true, Encoding.GetEncoding(1251));
                sw.WriteLine(ss);
                sw.Close();
            }
        }
    }

    public class LogBot
    {
        string mainFolder; //имя папки куда пишем
        string currNameFile = string.Empty; //открытое имя файла текущего лога
        System.IO.StreamWriter file = null; //открытый файл с именем currNameFile
        object lockObj;

        public LogBot(string path)
        {
            mainFolder = path;
            lockObj = new object();
        }

        public void Print( string nameLog, DateTime dateBot, int code, string text)
        {
            string nameFile = System.IO.Path.Combine(mainFolder, nameLog);
            if (!System.IO.Directory.Exists(nameFile))
                System.IO.Directory.CreateDirectory(nameFile);
            string dateName = DateTime.Today.ToString("yyyyMMdd") + ".txt";
            nameFile = System.IO.Path.Combine(nameFile, dateName);
            lock (lockObj)
            {
                if (currNameFile != nameFile)
                {
                    if (file != null)
                        file.Close();
                    file = new System.IO.StreamWriter(nameFile, true, Encoding.GetEncoding(1251));
                    currNameFile = nameFile;
                }
                string s = '[' + DateTime.Now.ToString("dd.MM.yyyy HH:mm:ss") + ']';
                s += " [" + dateBot.ToString("dd.MM.yyyy HH:mm:ss") + ']';
                s += " (" + code.ToString() + "): " + text;
                file.WriteLine(s);
                file.Flush();
            }
        }

		public void Close()
		{
			if (file != null) file.Close();
			file = null;
		}
    }
}
