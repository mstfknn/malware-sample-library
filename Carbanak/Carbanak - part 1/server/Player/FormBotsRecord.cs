using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;

namespace Player
{
    public partial class FormBotsRecord : Form
    {
        class VideoFile
        {
            public string uid;
            public string fileName;
            public VideoHead head;
        }

        class CaptionInfo
        {
            public string text; //тест заголовка
            public int bots; //количество ботов в которых есть
            public int files; //количество файлов в которых есть
            public int count; //сколько раз встречается всего
        }

        string pathVideo;
        bool pathFtp = false;
        RivFTP ftp;
        string[] uids;
        List<VideoFile> videos;
        List<VideoFile> addVideos;
        object lockObj;
        string selUID;
        Thread ftpThread;
        Thread runFtpThread;
        bool stopFtpThread = false;
        FormPlayer player;
        List<CaptionInfo> captions;

        public FormBotsRecord(FormPlayer player)
        {
            InitializeComponent();
            lockObj = new object();
            addVideos = new List<VideoFile>();
            this.player = player;
            captions = new List<CaptionInfo>();
        }

        private void FormBotsRecord_Load(object sender, EventArgs e)
        {
            pathVideo = Application.StartupPath + "\\video";
            cbPaths.Items.Add(pathVideo);
            foreach (string path in FormPlayer.cfg.pathVideo)
                cbPaths.Items.Add(path);
            cbPaths.SelectedIndex = 0;
            timer.Start();
        }

        void UpdateUIDs()
        {
            lbUids.Items.Clear();
            if (ftpThread != null) ftpThread.Abort();
            if (pathVideo.Length > 0)
            {
                string[] dirs = null;
                if (pathVideo.Substring(0, 3).ToLower() == "ftp")
                {
                    ftp = new RivFTP(pathVideo);
                    dirs = ftp.ListDirectory();
                    pathFtp = true;
                    string ftpSuffix = string.Empty;
                    int p = pathVideo.IndexOf('@');
                    if (p > 0)
                        p++;
                    else
                        p = 6;
                    ftpSuffix = pathVideo.Substring(p);
                    ftpSuffix = ftpSuffix.Replace('/', '_');
                    ftpSuffix = ftpSuffix.Replace('\\', '_');
                    ftpSuffix = ftpSuffix.Replace(':', '_');
                    pathVideo = Application.StartupPath + "\\video_ftp\\" + ftpSuffix;
                }
                else
                {
                    pathFtp = false;
                    dirs = System.IO.Directory.GetDirectories(pathVideo);
                }
                uids = new string[dirs.Length];

                lbUids.Items.Add("Все");
                for (int i = 0; i < dirs.Length; i++)
                {
                    int p = dirs[i].LastIndexOf('\\');
                    uids[i] = dirs[i].Substring(p + 1);
                    lbUids.Items.Add(uids[i]);
                }
            }
            tbCountBots.Text = (lbUids.Items.Count - 1).ToString();
        }

        void UpdateFiles()
        {
            if (lbUids.SelectedIndex < 0) return;
            string path = pathVideo + '\\';
            if (selUID != null)
                path += selUID;
            videos = new List<VideoFile>();
            if (System.IO.Directory.Exists(path))
            {
                StopFtpLoad();
                string[] files = System.IO.Directory.GetFiles(path, "*.frm", System.IO.SearchOption.AllDirectories);
                progressFtp.Visible = true;
                progressFtp.Maximum = files.Length;
                for (int i = 0; i < files.Length; i++)
                {
                    progressFtp.Value = i;
                    if( (i % 100) == 0 ) Application.DoEvents();
                    VideoFile vf = LoadVideoFile(files[i]);
                    if (vf != null)
                        videos.Add(vf);
                }
                progressFtp.Visible = false;
            }
            UpdateLV(string.Empty);
            if (pathFtp && selUID != null)
            {
                try
                {
                    if (runFtpThread != null)
                    {
                        Cursor = Cursors.WaitCursor;
                        while(runFtpThread.ThreadState != ThreadState.Stopped)
                            System.Threading.Thread.Sleep(500);
                        Cursor = Cursors.Default;
                    }
                    runFtpThread = new Thread(StartFtpLoad);
                    progressFtp.Value = 0;
                    progressFtp.Visible = true;
                    runFtpThread.Start();
                }
                catch (Exception e)
                {
                    //Console.WriteLine(e.ToString());
                }
            }
        }

        VideoFile LoadVideoFile(string path)
        {
            try
            {
                VideoHead vh = new VideoHead(path);
                if (!vh.IsBad)
                {
                    VideoFile vf = new VideoFile();
                    vf.fileName = path;
                    string[] ss = path.Split(System.IO.Path.DirectorySeparatorChar);
                    vf.uid = ss[ss.Length - 2]; //предпоследняя строка UID бота (папка)
                    vf.head = vh;
                    return vf;
                }
            }
            catch (Exception e)
            {
            }
            return null;
        }

        void AddInList(VideoFile vf)
        {
            ListViewItem lvi = lvFiles.Items.Add(vf.uid);
            lvi.SubItems.Add(vf.head.IP);
            lvi.SubItems.Add(System.IO.Path.GetFileName(vf.fileName));
            lvi.SubItems.Add(vf.head.Name);
            lvi.SubItems.Add(vf.head.DateBeg.ToString("dd.MM.yyyy HH:mm:ss"));
            lvi.SubItems.Add(vf.head.DateEnd.ToString("dd.MM.yyyy HH:mm:ss"));
            lvi.SubItems.Add(vf.head.NameProcess);
            lvi.Tag = vf;
            foreach (string s in vf.head.Captions)
            {
                bool ok = true;
                if (cbCaptions.Items.Contains(s))
                {
                    ok = false;
                    //captions[i].count++;
                    break;
                }
                if (ok)
                {
                    cbCaptions.Items.Add(s);
                    CaptionInfo caption = new CaptionInfo();
                    caption.text = s;
                    caption.bots = 1;
                    caption.files = 1;
                    caption.count = 1;
                    captions.Add(caption);
                }
            }
        }

        void UpdateLV(string filterName)
        {
            lvFiles.Items.Clear();
            string fUID = tbFilterUID.Text.Trim();
            string fName = tbFilterNameVideo.Text.Trim();
            string fCaption = tbFilterCaption.Text.Trim();
            List<string> captions = new List<string>(); ;
            cbCaptions.Items.Clear();
            captions.Clear();
            Dictionary<string, bool> uids = new Dictionary<string, bool>();
            progressFtp.Visible = true;
            progressFtp.Maximum = videos.Count;
            int n = 0;
            foreach (VideoFile vf in videos)
            {
                progressFtp.Value = n;
                if ((n % 100) == 0) Application.DoEvents();
                bool yesCaption = false;
                if (fCaption.Length > 0)
                {
                    foreach (string s in vf.head.Captions)
                        if (s.Contains(fCaption))
                        {
                            yesCaption = true;
                            break;
                        }
                }
                else
                    yesCaption = true;
                if (vf.uid.Contains(fUID) && vf.head.Name.Contains(fName) && yesCaption)
                {
                    AddInList(vf);
                    if( !uids.ContainsKey(vf.uid) )
                        uids.Add(vf.uid, true);
                }
                n++;
            }
            progressFtp.Visible = false;
            tbCountVideo.Text = lvFiles.Items.Count.ToString();
            tbUnikBots.Text = uids.Count.ToString();
        }

        void StartFtpLoad()
        {
            StopFtpLoad();
            ftpThread = new Thread(FtpLoad);
            ftpThread.Start();
        }

        void StopFtpLoad()
        {
            if (ftpThread != null)
            {
                stopFtpThread = true;
                while (ftpThread.ThreadState != ThreadState.Stopped)
                    System.Threading.Thread.Sleep(1000);
                stopFtpThread = false;
            }
        }

        void FtpLoad()
        {
            string[] dirs;
            if (selUID.Length == 0)
            {
                dirs = ftp.ListDirectory();
            }
            else
            {
                dirs = new string[] { selUID };
            }
            for (int i = 0; i < dirs.Length && !stopFtpThread; i++)
            {
                try
                {
                    string[] files = ftp.ListDirectory(dirs[i] + "/*.frm");
                    for (int j = 0; j < files.Length; j++)
                    {
                        string pathTo = pathVideo + '\\' + dirs[i];
                        if (!System.IO.Directory.Exists(pathTo))
                            System.IO.Directory.CreateDirectory(pathTo);
                        pathTo += '\\' + files[j];
                        bool ok = true;
                        foreach (VideoFile vf in videos)
                            if (vf.fileName == pathTo)
                            {
                                ok = false;
                                break;
                            }
                        if (ok)
                        {
                            string pathFrom = dirs[i] + '/' + files[j];
                            ftp.LoadFile(pathFrom, pathTo);
                            VideoFile vf = LoadVideoFile(pathTo);
                            if (vf != null)
                                lock (lockObj)
                                {
                                    addVideos.Add(vf);
                                }
                        }
                    }
                }
                catch (Exception e)
                {
                    //Console.WriteLine(e.ToString());
                }
            }
        }

        void DelVideo(string path, DateTime date)
        {
            Cursor.Current = Cursors.WaitCursor;
            StopFtpLoad();
            progressFtp.Visible = true;
            string pathTo = Application.StartupPath + "\\DeletedVideo";
            string[] dirs = System.IO.Directory.GetDirectories(path);
            progressFtp.Maximum = dirs.Length;
            for (int d = 0; d < dirs.Length; d++)
            {
                progressFtp.Value = d;
                string pathTo2 = System.IO.Path.Combine(pathTo, System.IO.Path.GetFileName(dirs[d]));
                if (!System.IO.Directory.Exists(pathTo2)) //такой папки нет, создаем
                {
                    System.IO.Directory.CreateDirectory(pathTo2);
                }
                string[] files = System.IO.Directory.GetFiles(dirs[d]);
                int del = 0; //сколько удалили файлов
                for (int i = 0; i < files.Length; i++)
                {
                    System.IO.FileInfo fi = new System.IO.FileInfo(files[i]);
                    if (fi.LastWriteTime < date)
                    {
                        string fileTo = System.IO.Path.Combine(pathTo2, System.IO.Path.GetFileName(files[i]));
                        if (System.IO.File.Exists(fileTo)) System.IO.File.Delete(fileTo);
                        System.IO.File.Move(files[i], fileTo);
                        del++;
                    }
                }
                if (del == files.Length) //удалили все, удаляем папку
                {
                    System.IO.Directory.Delete(dirs[d]);
                }
            }
            progressFtp.Visible = false;
            UpdateUIDs();
            Cursor.Current = Cursors.Default;
        }

        private void lbUids_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (lbUids.SelectedIndex >= 0)
            {
                if (lbUids.SelectedIndex > 0)
                    selUID = (string)lbUids.Items[lbUids.SelectedIndex];
                else
                    selUID = string.Empty;
            }
            else
                selUID = null;
            UpdateFiles();
        }

        public string fileName
        {
            get
            {
                if (lvFiles.SelectedItems.Count > 0)
                {
                    return ((VideoFile)lvFiles.Items[lvFiles.SelectedItems[0].Index].Tag).fileName;
                }
                else
                    return string.Empty;
            }
        }

        public string UID
        {
            get
            {
                if (lvFiles.SelectedItems.Count > 0)
                {
                    return ((VideoFile)lvFiles.Items[lvFiles.SelectedItems[0].Index].Tag).uid;
                }
                else
                    return string.Empty;
            }
        }

        private void btOpen_Click(object sender, EventArgs e)
        {
            player.Focus();
            player.OpenFile(fileName, UID);
        }

        private void btCancel_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void tbFilterUID_TextChanged(object sender, EventArgs e)
        {
            UpdateLV(string.Empty);
        }

        private void tbFilterNameVideo_TextChanged(object sender, EventArgs e)
        {
            UpdateLV(string.Empty);
        }

        private void lvFiles_DrawItem(object sender, DrawListViewItemEventArgs e)
        {
            if (cbCaptions.SelectedIndex >= 0)
            {
                VideoFile vf = (VideoFile)e.Item.Tag;
                if (vf.head.Captions.Contains((string)cbCaptions.Items[cbCaptions.SelectedIndex]))
                    e.Graphics.FillRectangle(Brushes.LightBlue, e.Bounds);
                else
                    e.DrawBackground();
            }
            else
                e.DrawBackground();
            e.DrawFocusRectangle();
        }

        private void lvFiles_DrawSubItem(object sender, DrawListViewSubItemEventArgs e)
        {
            e.DrawText();
        }

        private void cbCaptions_SelectedIndexChanged(object sender, EventArgs e)
        {
            foreach (ListViewItem item in lvFiles.Items)
            {
                VideoFile vf = (VideoFile)item.Tag;
                if (vf.head.Captions.Contains((string)cbCaptions.Items[cbCaptions.SelectedIndex]))
                    item.BackColor = Color.LightBlue;
                else
                    item.BackColor = lvFiles.BackColor;
            }
        }

        private void cbPaths_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (cbPaths.SelectedIndex >= 0)
                pathVideo = (string)cbPaths.Items[cbPaths.SelectedIndex];
            else
                pathVideo = string.Empty;
            UpdateUIDs();
        }

        private void btAddPath_Click(object sender, EventArgs e)
        {
            if (dlgFolder.ShowDialog() == DialogResult.OK)
            {
                FormPlayer.cfg.pathVideo.Add(dlgFolder.SelectedPath);
                cbPaths.Items.Add(dlgFolder.SelectedPath);
                FormPlayer.cfg.Save();
                cbPaths.SelectedIndex = cbPaths.Items.Count - 1;
            }
        }

        private void cbPaths_Validating(object sender, CancelEventArgs e)
        {
            if (cbPaths.SelectedIndex < 0 && cbPaths.Text.Length > 0)
            {
                string path = cbPaths.Text.Trim();
                if (path.Length > 3)
                {
                    bool add = false;
                    if (path.Substring(0, 3).ToLower() == "ftp")
                    {
                        string[] ss = path.Split(';');
                        add = true;
                    }
                    else
                    {
                        if (System.IO.Directory.Exists(path))
                        {
                            add = true;
                        }
                    }
                    if (add)
                    {
                        FormPlayer.cfg.pathVideo.Add(path);
                        cbPaths.Items.Add(path);
                        FormPlayer.cfg.Save();
                        cbPaths.SelectedIndex = cbPaths.Items.Count - 1;
                    }
                }
            }
        }

        private void btDelPath_Click(object sender, EventArgs e)
        {
            if (cbPaths.SelectedIndex > 0)
            {
                FormPlayer.cfg.pathVideo.Remove((string)cbPaths.Items[cbPaths.SelectedIndex]);
                cbPaths.Items.RemoveAt(cbPaths.SelectedIndex);
                FormPlayer.cfg.Save();
                cbPaths.SelectedIndex = -1;
            }
        }

        private void timer_Tick(object sender, EventArgs e)
        {
            if (progressFtp.Visible && runFtpThread != null)
            {
                if (ftpThread.ThreadState == ThreadState.Stopped && runFtpThread.ThreadState == ThreadState.Stopped)
                    progressFtp.Visible = false;
                lock (lockObj)
                {
                    foreach (VideoFile vf in addVideos)
                    {
                        AddInList(vf);
                    }
                    tbCountVideo.Text = lvFiles.Items.Count.ToString();
                    addVideos.Clear();
                }
                if (progressFtp.Value == 3)
                    progressFtp.Value = 0;
                else
                    progressFtp.Value++;
            }
        }

        private void FormBotsRecord_FormClosing(object sender, FormClosingEventArgs e)
        {
            StopFtpLoad();
        }

        private void btOpenAnchor_Click(object sender, EventArgs e)
        {
            FormAnchor form = new FormAnchor();
            if (form.ShowDialog() == DialogResult.OK)
            {
                string anchor = form.Anchor;
                if (anchor.Length > 0)
                {
                    DateTime posTime;
                    string uid;
                    string file = Video.FindAnchor(pathVideo, anchor, out posTime, out uid);
                    if (file.Length > 0)
                    {
                        player.Focus();
                        player.OpenFile(file, uid);
                        player.SetTimeVideo(posTime);
                    }
                    else
                        MessageBox.Show("По указанному якорю ничего не найдено");
                }
            }
        }

        private void btExportCaptions_Click(object sender, EventArgs e)
        {
            if (dlgSaveFile.ShowDialog() == DialogResult.OK)
            {
                System.IO.StreamWriter sw = new System.IO.StreamWriter(dlgSaveFile.FileName);
                foreach (CaptionInfo c in captions)
                {
                    sw.WriteLine(c.count.ToString() + '\t' + c.text);
                }
                sw.Close();
                MessageBox.Show("Экспорт окончен");
            }
        }

        private void btDelVideo_Click(object sender, EventArgs e)
        {
            FormDateDelete form = new FormDateDelete();
            if (form.ShowDialog() == DialogResult.OK)
            {
                DelVideo(pathVideo, form.Date);
            }
        }

        private void btBugVideo_Click(object sender, EventArgs e)
        {
            string fn = fileName;
            if (fn.Length > 0)
            {
                string folder = System.IO.Path.Combine(Application.StartupPath + "\\VideoBug", UID);
                if (!System.IO.Directory.Exists(folder))
                    System.IO.Directory.CreateDirectory(folder);
                string bugFile = System.IO.Path.Combine(folder, System.IO.Path.GetFileName(fn));
                System.IO.File.Copy(fn, bugFile, true);
            }
        }

        private void tbFilterCaption_TextChanged(object sender, EventArgs e)
        {
            UpdateLV(string.Empty);
        }
    }
}
