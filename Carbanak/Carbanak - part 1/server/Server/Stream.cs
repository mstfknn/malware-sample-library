using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Net.Sockets;

namespace Server
{
	public class StreamType
	{
		public const int File = 0; //файл 
		public const int Video = Sender.ID_VIDEO; //видео поток
		public const int Log = 3; //логи
		public const int Socket = 4; //сокет
		public const int Memory = 5; //память
		public const int Proxy = 6; //запись в несколько потоков
		public const int Redirect = 7; //перенаправляет данные в другой поток
		public const int Pipe = 8; //данные с пайп канала
	}

	public abstract class StreamReceiver
	{
		protected int type;
		public abstract void Write(byte[] data, int offset, int size);
		public abstract void Flush();
		public abstract void Close();
		//перейти в режим ожидания в дальнейшем возможно опять будет запись (при разрыве связи поток должен освободить ресурсы и ждать когда опять пойдут данные)
		//аналогично Close(), только данные позже опять могут писаться
		public abstract void Waiting();
		public int Type
		{
			get { return type; }
		}
	}

	public class StreamProxy : StreamReceiver
	{
		public uint id { get; private set; }
		public string uid { get; private set; }
		public string name { get; set; }
		List<StreamReceiver> receivers;
		object lockReceivers;
		DateTime timeCreate;
		DateTime timeLastWrite;

		public StreamProxy(uint id, string uid)
		{
			this.id = id;
			this.uid = uid;
			type = StreamType.Proxy;
			receivers = new List<StreamReceiver>();
			timeCreate = DateTime.Now;
			timeLastWrite = timeCreate;
			lockReceivers = new object();
		}

		public void AddReceiver(StreamReceiver receiver)
		{ 
			lock(lockReceivers) 
				receivers.Add(receiver);
		}

		public void DelReceiver(StreamReceiver receiver)
		{
			lock (lockReceivers)
				receivers.Remove(receiver);
		}

		public override void Write(byte[] data, int offset, int size)
		{
			lock (lockReceivers)
			{
				foreach (StreamReceiver receiver in receivers)
					receiver.Write(data, offset, size);
			}
		}
		
		public void Write(byte[] data, int size)
		{
			Write(data, 0, size);
		}

		public override void Flush()
		{
			lock (lockReceivers)
			{
				foreach (StreamReceiver receiver in receivers)
					receiver.Flush();
			}
		}

		public override void Close()
		{
			lock (lockReceivers)
			{
				foreach (StreamReceiver receiver in receivers)
					receiver.Close();
			}
		}

		public override void Waiting()
		{
			lock (lockReceivers)
			{
				foreach (StreamReceiver receiver in receivers)
					receiver.Waiting();
			}
		}

		public void SetId(uint id)
		{
			this.id = id;
		}

		public void ResetStreamLogs()
		{
			lock (lockReceivers)
			{
				foreach(StreamReceiver stream in receivers)
					switch(stream.Type)
					{
						case StreamType.Log:
							StreamLog log = stream as StreamLog;
							if (log != null) log.Reset();
							break;
						case StreamType.File:
							StreamFile file = stream as StreamFile;
							if (file != null) file.Reset();
							break;
					}
			}
		}

		public int Type
		{
			get
			{
				int ret = 0;
				if (receivers.Count > 0)
					ret = receivers[0].Type;
				return ret;
			}
		}

		public List<StreamReceiver> Receivers
		{
			get { return receivers; }
		}
	}

	public class StreamFactory
	{
		Dictionary<uint, StreamProxy> streams;
		ReaderWriterLockSlim lockStreams;
		uint lastid = 0;

		public StreamFactory()
		{
			streams = new Dictionary<uint, StreamProxy>();
			lockStreams = new ReaderWriterLockSlim();
		}

		public StreamProxy CreateStream(string uid)
		{
			lockStreams.EnterWriteLock();
			try
			{
				lastid++;
				StreamProxy stream = new StreamProxy(lastid, uid);
				streams[lastid] = stream;
				return stream;
			}
			finally
			{
				lockStreams.ExitWriteLock();
			}
		}

		public StreamProxy GetStream(uint id)
		{
			lockStreams.EnterReadLock();
			try
			{
				StreamProxy stream;
				if (streams.TryGetValue(id, out stream))
					return stream;
				return null;
			}
			finally
			{
				lockStreams.ExitReadLock();
			}
		}

		public void CloseStream(uint id)
		{
			StreamProxy stream = GetStream(id);
			if (stream == null) return;
			stream.Close();
			lockStreams.EnterWriteLock();
			try
			{
				streams.Remove(id);
			}
			finally
			{
				lockStreams.ExitWriteLock();
			}
		}

		//назначает потоку новый ид
		public void ChangeId(StreamProxy stream)
		{
			lockStreams.EnterWriteLock();
			streams.Remove(stream.id);
			lastid++;
			stream.SetId(lastid);
			streams[lastid] = stream;
			lockStreams.ExitWriteLock();
		}

		//сбрасывает все потоки для логгирования и открывает для них новые файлы, в результате можно сразу посмотреть результаты логгирования, без остановки сервера
		public void ResetStreamLogs()
		{
			lockStreams.EnterReadLock();
			foreach (StreamProxy stream in streams.Values)
			{
				stream.ResetStreamLogs();
			}
			lockStreams.ExitReadLock();
		}

	}

	public class StreamFile : StreamReceiver
	{
		string fileName;
		protected System.IO.FileStream fs = null;
		object lockFile;

		public StreamFile(string fileName)
		{
			type = StreamType.File;
			this.fileName = fileName;
			lockFile = new object();
		}

		protected bool Create()
		{
			try
			{
				if (System.IO.File.Exists(fileName))
				{
					fs = new System.IO.FileStream(fileName, System.IO.FileMode.Open, System.IO.FileAccess.Write);
					fs.Seek(0, System.IO.SeekOrigin.End);
				}
				else
					fs = new System.IO.FileStream(fileName, System.IO.FileMode.Create, System.IO.FileAccess.Write);
				return true;
			}
			catch
			{
				return false;
			}
		}

		public override void Write(byte[] data, int offset, int size)
		{
			lock (lockFile)
			{
				if (fs == null)
					if (!Create())
						return;
				fs.Write(data, offset, size);
			}
		}

		public override void Flush()
		{
			lock (lockFile)
			{
				if (fs != null) fs.Flush();
			}
		}

		public override void Close()
		{
			lock (lockFile)
			{
				if (fs != null)
				{
					fs.Flush();
					fs.Close();
				}
				fs = null;
			}
		}

		public override void Waiting()
		{
			lock(lockFile)
				Close();
		}

		//закрывает текущий файл и создает новый, используется для сброса накопленных данных с целью их просмотра без отключения сервера
		public void Reset()
		{
			if (fs == null) return;
			lock (lockFile)
			{
				string name = System.IO.Path.GetFileNameWithoutExtension(fileName);
				if (name.Length > 6)
				{
					int c;
					if (int.TryParse(name.Substring(name.Length - 6), out c))
					{
						c++;
						string name2 = name.Substring(0, name.Length - 6) + c.ToString("000000");
						fileName = fileName.Replace(name, name2);
						fs.Flush();
						fs.Close();
						fs = null;
					}
				}
			}
		}
	}

	public class StreamVideo : StreamFile
	{
		public StreamVideo( string fileName, byte[] ip) : base(fileName)
		{
			type = StreamType.Video;
			if (Create())
			{
				byte[] data = new byte[32];
				data[0] = (byte)'T';
				data[1] = (byte)'A';
				data[2] = (byte)'G';
				DateTime date = DateTime.Now;
				int i = DataConvert.DateTimeToBytes(date, data, 3); //начало записи
				i = DataConvert.DateTimeToBytes(date, data, i); //конец записи
				Array.Copy(ip, 0, data, i, 4); i += 4;
				Write(data, 0, i);
			}
		}

		public override void Close()
		{
			if (fs == null)
				if (!Create())
					return;
			fs.Seek(10, System.IO.SeekOrigin.Begin);
			byte[] data = new byte[8];
			int n = DataConvert.DateTimeToBytes(DateTime.Now, data, 0);
			Write(data, 0, n);
			base.Close();
		}

		public override void Waiting()
		{
			Close();
		}
	}

	public class StreamLog : StreamFile
	{
		public StreamLog(string fileName)
			: base(fileName)
		{
			type = StreamType.Log;
		}

		public override void Write(byte[] data, int offset, int size)
		{
			base.Write(data, offset, size);
			if (fs != null)
			{
				//перевод строки
				fs.WriteByte(0x0d);
				fs.WriteByte(0x0a);
			}
		}
	}

	public class StreamSocket : StreamReceiver
	{
		Socket sc;

		public StreamSocket(Socket sc)
		{
			type = StreamType.Socket;
			this.sc = sc;
		}

		public override void Write(byte[] data, int offset, int size)
		{
			if (sc != null && sc.Connected && size > 0)
			{
				try
				{
					byte[] size2 = BitConverter.GetBytes(size);
					sc.Send(size2, 0, size2.Length, SocketFlags.None);
					sc.Send(data, offset, size, SocketFlags.None);
				}
				catch
				{
					sc.Close();
					sc = null;
				}
			}
		}

		public override void Flush()
		{
		}

		public override void Close()
		{
			if (sc != null)
			{
				try
				{
					sc.Close();
				}
				catch
				{
				}
				sc = null;
			}
		}

		public override void Waiting()
		{
		}
	}

	class StreamMemory : StreamReceiver
	{
		byte[] mem;
		int c_mem;

		public StreamMemory()
		{
			type = StreamType.Memory;
			mem = null;
			c_mem = 0;
		}

		public override void Write(byte[] data, int offset, int size)
		{
			if (mem == null)
			{
				mem = new byte[size * 2];
			}
			else
			{
				if (mem.Length - c_mem < size)
				{
					int newSize = 2 * mem.Length;
					if( newSize - c_mem < size)
						newSize = mem.Length + size + size / 2;
					byte[] mem2 = new byte[newSize];
					Array.Copy(mem, mem2, c_mem);
					mem = mem2;
				}
			}
			Array.Copy(data, offset, mem, c_mem, size);
			c_mem += size;
		}

		public override void Flush()
		{
		}

		public void Flush(StreamReceiver stream)
		{
			if( c_mem > 0 ) stream.Write(mem, 0, c_mem);
		}

		public override void Close()
		{
			mem = null;
			c_mem = 0;
		}

		public override void Waiting()
		{
		}
	}

	public class StreamRedirect : StreamReceiver
	{
		protected StreamReceiver stream;

		public StreamRedirect(StreamReceiver stream)
		{
			this.stream = stream;
			type = StreamType.Redirect;
		}

		public override void Write(byte[] data, int offset, int size)
		{
			stream.Write(data, offset, size);
		}

		public override void Flush()
		{
			stream.Flush();
		}

		public override void Close()
		{
			stream.Close();
		}

		public override void Waiting()
		{
			stream.Waiting();
		}

		public void ChangeDirect(StreamReceiver stream)
		{
			this.stream = stream;
		}
	}

	public class StreamPipe : StreamRedirect
	{
		public string name { get; private set; }
		
		public StreamPipe(string name) : base( new StreamMemory() )
		{
			this.name = name;
			type = StreamType.Pipe;
		}

		public bool IsStreamMemory()
		{
			return stream is StreamMemory;
		}

		//при подключение к пайп потоку смотрим если пишет в память, то все что записалось в память отправляем в переданный поток и заменяем поток памяти на переданный
		public bool Update(StreamReceiver s)
		{
			StreamMemory sm = stream as StreamMemory;
			if (sm == null) return false;
			sm.Flush(s);
			stream = s;
			return true;
		}
	}

}
