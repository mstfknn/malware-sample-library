using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Drawing.Imaging;
using System.Threading;

namespace Player
{
    public class VideoHead
    {
        int freq;
        int bitsColor;
        string name;
        string nameProcess;
        DateTime timeBeg, timeEnd;
        string ip;
        List<string> captions;
        bool bad = false;

        public VideoHead(string nameFile)
        {
            byte[] data = new byte[512];
            System.IO.FileStream fs = new System.IO.FileStream(nameFile, System.IO.FileMode.Open, System.IO.FileAccess.Read);
            int rd = fs.Read(data, 0, 3);
            if (data[0] == 'T' && data[1] == 'A' && data[2] == 'G')
            {
                rd = fs.Read(data, 0, 18);
                if (rd == 18)
                {
                    timeBeg = GetDateTime(data, 0);
                    timeEnd = GetDateTime(data, 7);
                    ip = new System.Net.IPAddress(BitConverter.ToUInt32(data, 14)).ToString();
                }
            }
            else
            {
                fs.Seek(0, System.IO.SeekOrigin.Begin);
                timeBeg = DateTime.Now;
                timeEnd = DateTime.Now;
                ip = "nothing";
            }
            rd = fs.Read(data, 0, 10);
            if (data[0] == 'F' && data[1] == 'R' && data[2] == 'M' && rd == 10)
            {
                int sz = GetShort(data, 3);
                freq = data[5];
                int szNameVideo = GetShort(data, 6);
                int szNameProcess = GetShort(data, 8);
                sz += szNameVideo + szNameProcess;
                rd += fs.Read(data, rd, sz - rd);
                name = Encoding.ASCII.GetString(data, 10, szNameVideo);
                nameProcess = Encoding.ASCII.GetString(data, 10 + szNameVideo, szNameProcess);
                try
                {
                    captions = new List<string>();
					bool stop = false;
					byte[] data2 = null;
                    while (fs.Read(data, 0, 4) == 4 && !stop)
                    {
						sz = BitConverter.ToInt32(data, 0);
						if (data.Length < sz) data = new byte[sz];
						if (fs.Read(data, 0, sz) != sz) break;
						Server.LZW lzw = new Server.LZW();
						if (data2 == null || data2.Length < 20 * sz) data2 = new byte[20 * sz];
						int sz2 = lzw.from_lzw(data, data2);
						int i = 0;
						while (i < sz2)
						{
							int szStruct = VideoHead.GetShort(data2, i);
							if (szStruct < 0)
							{
								stop = true;
								break;
							}
							int szData = VideoHead.GetInt(data2, i + 6);
							if (szData < 0)
							{
								stop = true;
								break;
							}
							int szCaption = VideoHead.GetShort(data2, i + 12);
							if (szCaption < 0)
							{
								stop = true;
								break;
							}
							i += szStruct + szData;
							if (szCaption > 0 && i <= sz2)
							{
								string s = GetCaption(data2, i, szCaption);
								if (!captions.Contains(s))
									captions.Add(s);
								i += szCaption;
							}
						}
                    }
/*
					while (fs.Read(data, 0, 2) > 0)
					{
						int szStruct = VideoHead.GetShort(data, 0);
						if (szStruct < 0) break;
						fs.Read(data, 2, szStruct - 2);
						int szData = VideoHead.GetInt(data, 6);
						if (szData < 0) break;
						int szCaption = VideoHead.GetShort(data, 12);
						if (szCaption < 0) break;
						fs.Seek(szData, System.IO.SeekOrigin.Current);
						if (szCaption > 0 && fs.Length > fs.Position)
						{
							int c = fs.Read(data, 0, szCaption);
							if (c == szCaption)
							{
								string s = GetCaption(data, 0, szCaption);
								if (!captions.Contains(s))
									captions.Add(s);
							}
						}
					}
*/ 
				}
                catch
                {
					if( captions.Count == 0 )
						bad = true;
                }
            }
            else
                bad = true;
            fs.Close();
        }

        public bool IsBad
        {
            get { return bad; }
        }

        public string Name
        {
            get { return name; }
        }

        public string NameProcess
        {
            get { return nameProcess; }
        }

        public string IP
        {
            get { return ip; }
        }

        public DateTime DateBeg
        {
            get { return timeBeg; }
        }

        public DateTime DateEnd
        {
            get { return timeEnd; }
        }

        public List<string> Captions
        {
            get { return captions; }
        }

        static public int GetShort(byte[] data, int i)
        {
            return data[i] + data[i + 1] * 256;
        }

        static public int GetInt(byte[] data, int i)
        {
            return data[i] | (data[i + 1] << 8) | (data[i + 2] << 16) | (data[i + 3] << 24);
        }

        static public byte[] DateTimeToBytes(DateTime dt)
        {
            byte[] m = new byte[7];
            m[0] = (byte)(dt.Year & 0xff);
            m[1] = (byte)(dt.Year >> 8);
            m[2] = (byte)dt.Month;
            m[3] = (byte)dt.Day;
            m[4] = (byte)dt.Hour;
            m[5] = (byte)dt.Minute;
            m[6] = (byte)dt.Second;
            return m;
        }

        static public DateTime GetDateTime(byte[] data, int i)
        {
            int year = GetShort(data, i);
            int month = data[i + 2];
            int day = data[i + 3];
            int hour = data[i + 4];
            int minute = data[i + 5];
            int second = data[i + 6];
			try
			{
				return new DateTime(year, month, day, hour, minute, second);
			}
			catch
			{
				return DateTime.Now;
			}
        }

        static public string GetCaption(byte[] data, int i, int sz)
        {
            if (sz == 0)
                return string.Empty;
            int j = 0;
            while (j < sz && j < 5)
            {
                if (data[i + j] < 32)
                    return "---Испорченный заголовок---";
                j++;
            }
            return Encoding.GetEncoding(1251).GetString(data, i, sz);
        }
    }

    public class Video
    {
        const int acrossFull = 100; //через сколько кадров формировать полный кадр

        int freq;
        int bitsColor;
        string name;
        string nameProcess;
        DateTime dateBeg;
        DateTime dateEnd;
        string ip;

        List<Frame> frames;
        Frame lastFrame = null; //последний полученный фрейм (создается независимо от предыдущих)
        int lastNumFrame = -1;

        Filter filter = null;
        bool filterOn = false;
        List<int> filterFrame;
        int posLoadVideo;

        Hashtable captions = new Hashtable();

        public Video(int freq, int bits, DateTime dateBeg, DateTime dateEnd, string ip)
        {
            frames = new List<Frame>();
            this.freq = freq;
            bitsColor = bits;
            this.dateBeg = dateBeg;
            this.dateEnd = dateEnd;
            this.ip = ip;
        }

        public static Video LoadVideo(string nameFile)
        {
            byte[] data = System.IO.File.ReadAllBytes(nameFile);
            DateTime dateBeg = DateTime.Now, dateEnd = dateBeg;
            string ip = string.Empty;
            int i = 0;
            if (data[i] == 'T' && data[i + 1] == 'A' && data[i + 2] == 'G')
            {
                i += 3;
                dateBeg = VideoHead.GetDateTime(data, i); i += 7;
                dateEnd = VideoHead.GetDateTime(data, i); i += 7;
                ip = new System.Net.IPAddress(BitConverter.ToUInt32(data, i)).ToString(); i += 4;
            }
            if (data[i] == 'F' && data[i + 1] == 'R' && data[i + 2] == 'M')
            {
                i += 3;
                int sz = VideoHead.GetShort(data, i); i += 2;
                int freq = data[i]; i++; //частота кадров
                int szNameVideo = VideoHead.GetShort(data, i); i += 2;
                int szNameProcess = VideoHead.GetShort(data, i); i += 2;
                Video video = new Video(freq, 2, dateBeg, dateEnd, ip);
                video.NameVideo = Encoding.ASCII.GetString(data, i, szNameVideo);
                sz = i + szNameVideo + szNameProcess;
                int numFrame = 0; //номер кадра
				byte[] data2 = null;
				byte[] dataTmp = null;
                while (sz + 4 < data.Length && sz > 0)
                {
					int sz2 = BitConverter.ToInt32(data, sz); sz += 4;
					if (sz + sz2 > data.Length) break;
					Server.LZW lzw = new Server.LZW();
					if (dataTmp == null || dataTmp.Length < sz2) dataTmp = new byte[sz2];
					if (data2 == null || data2.Length < 20 * sz2) data2 = new byte[20 * sz2];
					Array.Copy(data, sz, dataTmp, 0, sz2);
					int sz3 = lzw.from_lzw(dataTmp, data2);
					i = 0;
					while (i < sz3)
					{
						numFrame++;
						int szStruct = VideoHead.GetShort(data2, i);
						if (i + szStruct >= sz3 || szStruct < 0) break;
						int width = VideoHead.GetShort(data2, i + 2);
						int height = VideoHead.GetShort(data2, i + 4);
						int szData = VideoHead.GetInt(data2, i + 6);
						int type = VideoHead.GetShort(data2, i + 10);
						int szCaption = VideoHead.GetShort(data2, i + 12);
						DateTime timeFrame = dateBeg.AddSeconds(numFrame / freq); //предположительное время кадра
						if (szStruct > 14) //было добавлено время на стороне бота
						{
							timeFrame = VideoHead.GetDateTime(data2, i + 14);
						}
						if (sz3 >= i + szStruct + szData && width < 6000 && height < 3000)
							video.AddFrame(width, height, type, data2, i + szStruct, szData, szCaption, timeFrame);
						i += szStruct + szData + szCaption;
//						if (video.frames.Count == 168)
//							video.frames.Count.ToString();
					}
					sz += sz2;
				}
                video.ArrangeCaptions();
                video.CorrectTime();
                return video;
            }
            return null;
        }

/*
		public static Video LoadVideo(string nameFile)
		{
			byte[] data = System.IO.File.ReadAllBytes(nameFile);
			DateTime dateBeg = DateTime.Now, dateEnd = dateBeg;
			string ip = string.Empty;
			int i = 0;
			if (data[i] == 'T' && data[i + 1] == 'A' && data[i + 2] == 'G')
			{
				i += 3;
				dateBeg = VideoHead.GetDateTime(data, i); i += 7;
				dateEnd = VideoHead.GetDateTime(data, i); i += 7;
				ip = new System.Net.IPAddress(BitConverter.ToUInt32(data, i)).ToString(); i += 4;
			}
			if (data[i] == 'F' && data[i + 1] == 'R' && data[i + 2] == 'M')
			{
				i += 3;
				int sz = VideoHead.GetShort(data, i); i += 2;
				int freq = data[i]; i++; //частота кадров
				int szNameVideo = VideoHead.GetShort(data, i); i += 2;
				int szNameProcess = VideoHead.GetShort(data, i); i += 2;
				Video video = new Video(freq, 2, dateBeg, dateEnd, ip);
				video.NameVideo = Encoding.ASCII.GetString(data, i, szNameVideo);
				sz = i + szNameVideo + szNameProcess;
				int numFrame = 0; //номер кадра
				int prevSz = sz, prevSzStruct = 0;
				while (sz + 2 < data.Length && sz > 0)
				{
					numFrame++;
					int szStruct = VideoHead.GetShort(data, sz);
					if (sz + szStruct >= data.Length || szStruct < 0) break;
					if (prevSzStruct > 0 && prevSzStruct != szStruct) //ошибка в файле
					{
						while (sz + 6 < data.Length)
						{
							int ii = 0;
							while (ii < 6)
							{
								if (data[sz + ii] != data[prevSz + ii]) break;
								ii++;
							}
							if (ii == 6) break;//нашли начало следующего кадра
							sz++;
						}
						if (sz + 6 >= data.Length) break; //конец файла испорчен
						szStruct = VideoHead.GetShort(data, sz);
					}
					prevSzStruct = szStruct;
					prevSz = sz;
					int width = VideoHead.GetShort(data, sz + 2);
					int height = VideoHead.GetShort(data, sz + 4);
					int szData = VideoHead.GetInt(data, sz + 6);
					int type = VideoHead.GetShort(data, sz + 10);
					int szCaption = VideoHead.GetShort(data, sz + 12);
					DateTime timeFrame = dateBeg.AddSeconds(numFrame / freq); //предположительное время кадра
					if (szStruct > 14) //было добавлено время на стороне бота
					{
						timeFrame = VideoHead.GetDateTime(data, sz + 14);
					}
					if (data.Length >= sz + szStruct + szData && width < 6000 && height < 3000)
						video.AddFrame(width, height, type, data, sz + szStruct, szData, szCaption, timeFrame);
					sz += szStruct + szData + szCaption;
					if (video.frames.Count == 168)
						video.frames.Count.ToString();
				}
				video.ArrangeCaptions();
				video.CorrectTime();
				return video;
			}
			return null;
		}
*/ 

		public void AddFrame(int w, int h, int t, byte[] data, int beg, int sz, int szCaption, DateTime date)
        {
            frames.Add(new Frame(w, h, t, data, beg, sz, szCaption, date));
        }

        public int CountFrames
        {
            get { return frames.Count; }
        }

        public int Freq
        {
            get { return freq; }
        }

        public int LenVideo
        {
            get { return frames.Count / freq; }
        }

        static public string FindAnchor(string path, string anchor, out DateTime posTime, out string uid)
        {
            string ret = string.Empty;
            string[] ss = anchor.Split(' ');
            posTime = DateTime.Today;
            uid = string.Empty;
            if (ss.Length == 3) //должна быть строка в виде "уид 2011-10-18 16:31:54"
            {
                uid = ss[0].Trim();
                string[] ss1 = ss[1].Trim().Split('-');
                string[] ss2 = ss[2].Trim().Split(':');
                if (ss1.Length == 3 && ss2.Length == 3) //формат даты и времени верный 
                {
                    int y = int.Parse(ss1[0]);//год 
                    int m = int.Parse(ss1[1]);//месяц
                    int d = int.Parse(ss1[2]); //день
                    int h = int.Parse(ss2[0]);//часы
                    int mm = int.Parse(ss2[1]); //минуты
                    int s = int.Parse(ss2[2]); //секунды
                    DateTime dt = new DateTime(y, m, d, h, mm, s);
                    string[] dirs = System.IO.Directory.GetDirectories(path, uid, System.IO.SearchOption.AllDirectories);
                    for (int i = 0; i < dirs.Length && ret.Length == 0; i++)
                    {
                        string[] frm = System.IO.Directory.GetFiles(dirs[i], "*.frm", System.IO.SearchOption.AllDirectories);
                        for(int j = 0; j < frm.Length; j++)
                        {
                            VideoHead video = new VideoHead(frm[j]);
                            if (!video.IsBad)
                            {
                                if (video.DateBeg <= dt && dt <= video.DateEnd)
                                {
                                    ret = frm[j];
                                    posTime = dt;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            return ret;
        }

        Frame[] GetPrevFrames(int n)
        {
            Frame[] prev = null;
            if (lastFrame != null && lastNumFrame + 1 == n)
            {
                prev = new Frame[1];
                prev[0] = lastFrame;
            }
            else
            {
                Frame frame = frames[n];
                if (frame.Type == 1)
                {
                    int n2 = n - 1;
                    while (frames[n2].Type == 1)
                        n2--;
                    prev = new Frame[n - n2];
                    for (int i = n2; i < n; i++)
                        prev[i - n2] = frames[i];
                }
                if ((n % acrossFull) == 0 && frame.Type == 1)
                    frame.SetFullFrame(prev, bitsColor);
            }
            return prev;
        }

        public void SetFullFrames()
        {
			try
			{
				posLoadVideo = 0;
				for (int i = 8 * acrossFull; i < frames.Count; i += 8 * acrossFull)
				{
					GetPrevFrames(i);
					posLoadVideo = i;
				}
				posLoadVideo = CountFrames;
			}
			catch (Exception e)
			{
				System.Diagnostics.Debug.WriteLine(e.ToString());
			}
			posLoadVideo.ToString();
        }

        public Bitmap GetFrameBitmap(int n)
        {
            Frame frame;
            Bitmap bmp = frames[n].GetBitmap(GetPrevFrames(n), bitsColor, out frame);
            lastFrame = frame;
            lastNumFrame = n;
            return bmp;
        }

        public Frame GetFrame(int n)
        {
            if (n >= 0 && n < frames.Count)
                return frames[n];
            return null;
        }

        public Frame GetUnpackFrame(int n)
        {
            Frame frame = null;
            if (lastNumFrame == n)
                frame = lastFrame;
            else
            {
                frame = frames[n].UnpackFrame(GetPrevFrames(n), bitsColor);
                lastFrame = frame;
                lastNumFrame = n;
            }
            return frame;
        }

        public void CorrectTime()
        {
            int s1 = CountFrames / freq;
            int s2 = (int)dateEnd.Subtract(dateBeg).TotalSeconds;
            int r = s2 - s1;
            if (r < 0) r = -r;
            if (s2 < 0 || r > 10)
                dateEnd = dateBeg.AddSeconds(s1);
        }

        public byte[] GetPartFrame(int n, Rectangle r)
        {
            Frame frame = GetUnpackFrame(n);
            byte[] ret = new byte[r.Width * r.Height];
            for (int i = 0; i < r.Height; i++)
                for (int j = 0; j < r.Width; j++)
                {
                    ret[i * r.Width + j] = frame.Data[(i + r.Y) * frame.Width + r.X + j];
                }
            return ret;
        }

        public void SetFilter(Filter f)
        {
            filter = f;
            filterFrame = new List<int>();
        }

        public int NextFrameFilter(int prev)
        {
            if (filterOn)
            {
                int next = -1;
                foreach (int n in filterFrame)
                    if (n > prev)
                    {
                        next = n;
                        break;
                    }
                if (next == int.MaxValue) //больше кадров нет
                {
                    return -1;
                }
                if (next >= 0) //нашли следующий кадр по фильтру
                    return next;
                if (filterFrame.Count > 0)
                    next = filterFrame[filterFrame.Count - 1]; //последний найденный кадр 
                //ищем следующий кадр
                Point pnt = new Point();
                for (int n = next + 1; n < frames.Count; n++)
                {
                    Frame frame = GetUnpackFrame(n);
                    if (filter.Match(frame.Data, frame.Width, frame.Height, ref pnt))
                    {
                        filterFrame.Add(n);
                        return n;
                    }
                }
                //больше нет кадров по фильтру
                filterFrame.Add(int.MaxValue);
                return -1;
            }
            else
            {
                if (prev < frames.Count - 1)
                    return prev + 1;
                return -1;
            }
        }

        public bool IsBelongFilter(int n)
        {
            if (filterOn)
                return filterFrame.Exists(n2 => n2 == n);
            return true;
        }

        public void ArrangeCaptions()
        {
            string s = null;
            int[] fm = new int[] { 0, -1 };
            foreach (Frame frm in frames)
            {
                if (frm.Caption.Length == 0 && s != null)
                {
                    frm.Caption = s;
                    fm[1]++;
                }
                else
                {
                    if (s != null)
                    {
                        fm = (int[])fm.Clone();
                        if (captions.ContainsKey(frm.Caption))
                        {
                            List<int[]> l = (List<int[]>)captions[frm.Caption];
                            l.Add(fm);
                        }
                        else
                        {
                            List<int[]> l = new List<int[]>();
                            l.Add(fm);
                            captions[frm.Caption] = l;
                        }
                    }
                    else
                    {
                        List<int[]> l = new List<int[]>();
                        l.Add(fm);
                        captions[frm.Caption] = l;
                    }
                    fm[1]++;
                    fm[0] = fm[1];
                    s = frm.Caption;
                }
            }
        }

        public bool FilterOn
        {
            set { filterOn = value; }
            get { return filterOn; }
        }

        public string NameVideo
        {
            set { name = value; }
            get { return name; }
        }

        public void FillCBCaptions(ComboBox cb)
        {
            cb.Items.Clear();
            foreach (object s in captions.Keys)
            {
                cb.Items.Add(s);
            }
        }

        public void FillCBFrames(string s, ComboBox cb)
        {
            cb.Items.Clear();
            List<int[]> l = (List<int[]>)captions[s];
            foreach (int[] fm in l)
            {
                string text = fm[0].ToString() + " - " + fm[1].ToString();
                cb.Items.Add(text);
            }
        }

        public int[] GetPosFrames(string s, int num)
        {
            List<int[]> lc = (List<int[]>)captions[s];
            if (lc == null)
                return null;
			if (num >= 0 && num < lc.Count)
			{
				int[] fm = lc[num];
				return fm;
			}
			return null;
        }

        public int[] GetNearPosFrames(string s, int numFrame, ref int n)
        {
            List<int[]> l = (List<int[]>)captions[s];
            n = 0;
            foreach (int[] fm in l)
            {
                if (fm[0] <= numFrame && numFrame <= fm[1])
                    return fm;
                n++;
            }
            n = 0;
            return null;
        }

        //Возвращает номер кадра по времени на сервере
        public int GetFrameForServerTime(DateTime time)
        {
            TimeSpan diffFull = dateEnd - dateBeg;
            TimeSpan diffTime = time.TimeOfDay - dateBeg.TimeOfDay;
            if (diffTime >= diffFull)
                return frames.Count - 1;
            else
                if (diffTime.Seconds < 0)
                    return 0;
                else
                    return diffTime.Seconds * frames.Count / (int)diffFull.TotalSeconds;
        }

        //возвращает время кадра на сервере
        public DateTime GetServerTimeForFrame(int n)
        {
            if (n >= frames.Count) return dateEnd;
            if (n < 0) return dateBeg;
            return dateBeg.AddSeconds(n * (dateEnd - dateBeg).Seconds / frames.Count);
        }

        //возвращает номер кадра по времени бота
        public int GetFrameForBotTime(DateTime time)
        {
            int n = 0;
            TimeSpan ts = time.TimeOfDay;
            foreach (Frame f in frames)
            {
                if (f.Time.TimeOfDay > ts)
                    break;
                n++;
            }
            return n;
        }

        public DateTime DateBeg
        {
            get { return dateBeg; }
        }

        public DateTime DateEnd
        {
            get { return dateEnd; }
        }

        public int PosLoadVideo
        {
            get { return posLoadVideo; }
        }
    }

    public class Frame
    {
        int width, height;
        int type;
        int szPack;
        int szData;
        byte[] data;
        string caption;
        DateTime time;

        public Frame(int w, int h, int t, byte[] data, int beg, int sz, int szCaption, DateTime date)
        {
            width = w;
            height = h;
            szData = sz;
            szPack = sz;
            type = t;
            time = date;
            if (beg >= 0)
            {
                this.data = new byte[sz];
                Array.Copy(data, beg, this.data, 0, sz);
                caption = VideoHead.GetCaption(data, beg + sz, szCaption);
            }
            else
                this.data = data;
        }

        public Bitmap GetBitmap(Frame[] prev, int bitsColor, out Frame unpackFrame)
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

        public int Type
        {
            get { return type; }
        }

        public int Width
        {
            get { return width; }
        }

        public int Height
        {
            get { return height; }
        }

        public byte[] Data
        {
            get { return data; }
        }

        public int SizePack
        {
            get { return szPack; }
        }

        public string Caption
        {
            set { caption = value; }
            get { return caption; }
        }

        public DateTime Time
        {
            get { return time; }
        }

        public Frame UnpackFrame(Frame[] prev, int bitsColor)
        {
            byte[] unpackData = null;
            if (type == 10)
            {
                unpackData = new byte[data.Length];
                Array.Copy(data, unpackData, data.Length);
            }
            else
            {
                unpackData = new byte[height * width];
                switch (type)
                {
                    case 0: //текущмий кадр начальный
                        Unpack(unpackData, bitsColor, false);
                        break;
                    case 1:
                        byte mask = (byte)((1 << bitsColor) - 1);
                        //prev[0] - обязательно начальный
                        if (prev[0].Type == 10) //уже распакованный
                            Array.Copy(prev[0].Data, unpackData, unpackData.Length);
                        else
                            prev[0].Unpack(unpackData, bitsColor, false); 
                        //налаживаем предыдущие кадры
                        for (int n = 1; n < prev.Length; n++)
                            prev[n].Unpack(unpackData, bitsColor, true);
                        //налаживаем текущий кадр
                        Unpack(unpackData, bitsColor, true);
                        break;
                }
            }
            return new Frame(width, height, 10, unpackData, -1, unpackData.Length, 0, time);
        }

        public void SetFullFrame(Frame[] prev, int bitsColor)
        {
            Frame f = UnpackFrame(prev, bitsColor);
            data = f.data;
            szData = f.data.Length;
            type = 10;
        }

    }
}
