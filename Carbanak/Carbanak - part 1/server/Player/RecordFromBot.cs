using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Threading;
using System.Net.Sockets;

namespace Player
{
    public class ServerBot
    {
        int port;
        Socket sc;
        List<ClientBot> bots;

        public ServerBot(int port)
        {
            this.port = port;
            bots = new List<ClientBot>();
        }

        public void Start()
        {
            sc = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            IPEndPoint pnt = new IPEndPoint(IPAddress.Any, port);
            sc.Bind(pnt);
            sc.Listen(100);
            try
            {
                while(true)
                {
                    Socket botSc = sc.Accept();
                    if (botSc.Connected)
                    {
                        ClientBot bot = new ClientBot(botSc, this);
                        bots.Add(bot);
                        Thread thread = new Thread(bot.Record);
                        thread.Start();
                    }
                }
            }
            catch
            {
            }
        }

        public void Close()
        {
            foreach (ClientBot bot in bots)
                bot.Stop(false);
            bots.Clear();
        }

        public void Stop()
        {
            sc.Close();
        }

        public void DelBot(ClientBot bot)
        {
            bots.Remove(bot);
        }

        public ClientBot[] ArrayBots
        {
            get { return bots.ToArray(); }
        }

    }

    public class ClientBot
    {
        Socket sc;
        byte[] buffer;
        ServerBot server;
        string uid;
        string nameFile;

        public ClientBot(Socket sc, ServerBot srv)
        {
            this.sc = sc;
            buffer = new byte[8192];
            server = srv;
        }

        public void Record()
        {
            int count = 0;
            int c_uid = 0;
            while (c_uid <= 0)
            {
                int count2 = sc.Receive(buffer, count, buffer.Length - count, SocketFlags.None);
                count += count2;
                c_uid = Array.IndexOf(buffer, (byte)0, 0, count);
            }
            if (c_uid > 0)
            {
                uid = Encoding.ASCII.GetString(buffer, 0, c_uid);
                string path = System.Windows.Forms.Application.StartupPath + "\\video\\" + uid;
                if (!System.IO.Directory.Exists(path))
                {
                    System.IO.Directory.CreateDirectory(path);
                }
                DateTime dateBeg = DateTime.Now;
                nameFile = path + '\\' + dateBeg.ToString("yyyyMMddHHmmss") + ".frm";
                int beg = c_uid + 1;
                //System.IO.StreamWriter sw = new System.IO.StreamWriter(@"c:\bot.txt");
                System.IO.FileStream fs = new System.IO.FileStream(nameFile, System.IO.FileMode.CreateNew);
                fs.WriteByte((byte)'T'); fs.WriteByte((byte)'A'); fs.WriteByte((byte)'G');
                byte[] m = VideoHead.DateTimeToBytes(dateBeg);
                fs.Write(m, 0, m.Length); //начало записи
                fs.Write(m, 0, m.Length); //конец записи
                fs.Write(BitConverter.GetBytes(((IPEndPoint)sc.RemoteEndPoint).Address.Address), 0, 4);
                while (true)
                {
                    try
                    {
                        if (beg == 0)
                            count = sc.Receive(buffer, 0, buffer.Length, SocketFlags.None);
                        if (count == 0) break;
                        fs.Write(buffer, beg, count - beg);
                        beg = 0;
                        //sw.Write(count);
                        //sw.Write(' ');
                    }
                    catch
                    {
                        break;
                    }
                }
                DateTime dateEnd = DateTime.Now;
                m = VideoHead.DateTimeToBytes(dateEnd);
                fs.Seek(10, System.IO.SeekOrigin.Begin);
                fs.Write(m, 0, m.Length);
                fs.Close();
                sc.Close();
                //sw.Close();
            }
            Stop(true);
        }

        public void Stop(bool del)
        {
            sc.Close();
            if (del)
                server.DelBot(this);
        }

        public string IP
        {
            get { return ((IPEndPoint)sc.RemoteEndPoint).Address.ToString(); } 
        }

        public string UID
        {
            get { return uid; }
        }
    }
}
