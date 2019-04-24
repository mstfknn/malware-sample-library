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
    public partial class FormPlayer : Form
    {
        Video video;
        Rectangle oldSizeForm; //размер окна до FullScreen
        Size szFullScreen;
        int numCurrFrame = -1;
        FormBotsRecord formVideo;
        public static Config cfg;
        int selBegX, selBegY, selEndX, selEndY;
        int isSel = 0;

        Filter currFilter = null;
        bool filterOn = false;
        Thread threadLoad;

        bool notSetFrame = false; //для игнорирования установки кадра, во время его установки

        public FormPlayer()
        {
            szFullScreen = new Size(0, 0);
            InitializeComponent();
            timer2.Start();
        }

        void UpdateInfo()
        {
            tbCountFrames.Text = video.CountFrames.ToString();
            tbLenVideo.Text = video.LenVideo.ToString() + " c.";
            scroolVideo.Maximum = video.CountFrames > 0 ? video.CountFrames - 1 : 0;
            udFrame.Maximum = video.CountFrames > 0 ? video.CountFrames - 1 : 0;
        }

        private void btOpen_Click(object sender, EventArgs e)
        {
            if (dlgOpenFile.ShowDialog() == DialogResult.OK)
            {
                OpenFile(dlgOpenFile.FileName, "Неизвестно");
            }
        }

        void SetFrame(int n)
        {
            if (notSetFrame) return;
           // if (numCurrFrame == n) return;
            if (video == null) return;
            notSetFrame = true;
            if (n > video.PosLoadVideo)
            {
                MessageBox.Show("Видео еще не загрузилось до указанного кадра, подождите окончательной загрузки", "Предупреждение");
                return;
            }
            if (!video.IsBelongFilter(n))
                n = video.NextFrameFilter(n - 1);
            if (n >= 0)
            {
                numCurrFrame = n;
                Frame frame = video.GetFrame(n);
                if (frame != null)
                {
                    tbWH.Text = frame.Width.ToString() + "x" + frame.Height.ToString();
                    tbSize.Text = frame.SizePack.ToString();
                    pbImage.Image = video.GetFrameBitmap(n);
                    //pbImage.Size = new Size(frame.Width, frame.Height);
                    if (chkZoom.Checked)
                    {
                        pbImage.Width = panel2.Width;
                        pbImage.Height = panel2.Height;
                    }
                    udFrame.Value = n;
                    scroolVideo.Value = n;
                    if( cbCaptions.Text != frame.Caption)
                        cbCaptions.Text = frame.Caption;
                    SetTimeVideo(n);
                    pbScale.Invalidate();
                }
            }
            else
            {
                udFrame.Value = numCurrFrame;
                scroolVideo.Value = numCurrFrame;
            }
            notSetFrame = false;
        }

        public void OpenFile(string nameFile, string uid)
        {
            if (!System.IO.File.Exists(nameFile))
                MessageBox.Show("Файл '" + nameFile + "' отсутствует", "Ошибка");
            else
            {
                if (threadLoad != null) threadLoad.Abort();
                video = Video.LoadVideo(nameFile);
                if (video == null)
                    MessageBox.Show("Указанный файл имеет не верный формат", "Ошибка");
                video.FillCBCaptions(cbCaptions);
                SetFrame(0);
                UpdateInfo();
                tbNameBot.Text = uid + " (" + video.NameVideo + ')';
                pbScale.Invalidate();
                threadLoad = new Thread(video.SetFullFrames);
                threadLoad.Start();
                progressLoadVideo.Value = 0;
                progressLoadVideo.Maximum = video.CountFrames > 0 ? video.CountFrames - 1 : 0;
                progressLoadVideo.Visible = true;
                labelFramesFulled.Visible = true;
                tbFramesFulled.Visible = true;
                tbFramesFulled.Text = string.Empty;
                tbFileVideo.Text = nameFile;
            }
        }

        public void SetTimeVideo(DateTime time)
        {
            if (video != null)
            {
                int frame = video.GetFrameForServerTime(time); ;
                if (frame >= video.CountFrames) frame = video.CountFrames - 1;
                SetFrame(frame);
            }
        }

        public void SetTimeVideoBot(DateTime time)
        {
            if (video != null)
            {
                int frame = video.GetFrameForBotTime(time);
                SetFrame(frame);
            }
        }

        void SetTimeVideo(int frame)
        {
            dateTimeVideo.Value = video.GetServerTimeForFrame(frame);
            dateTimeVideoBot.Value = video.GetFrame(frame).Time;
        }

        private void udFrame_ValueChanged(object sender, EventArgs e)
        {
            SetFrame((int)udFrame.Value);
        }

        private void scroolVideo_ValueChanged(object sender, EventArgs e)
        {
            SetFrame(scroolVideo.Value);
        }

        private void btBotsRecord_Click(object sender, EventArgs e)
        {
            if (formVideo == null)
            {
                formVideo = new FormBotsRecord(this);
                formVideo.Show();
            }
            else
                formVideo.Focus();
        }

        private void btPlay_Click(object sender, EventArgs e)
        {
            if (timer.Enabled) timer.Stop();
            if ((int)udFrame.Value == video.CountFrames - 1) SetFrame(0);
            timer.Interval = 1000 / video.Freq;
            timer.Start();
        }

        private void btStop_Click(object sender, EventArgs e)
        {
            timer.Stop();
        }

        private void btForward_Click(object sender, EventArgs e)
        {
            if( timer.Enabled ) timer.Stop();
            if ((int)udFrame.Value == video.CountFrames - 1) SetFrame(0);
            int speed = 1;
            if (cbSpeed.SelectedIndex >= 0)
            {
                speed = int.Parse((string)cbSpeed.Items[cbSpeed.SelectedIndex]);
            }
            int interval = 1000 / (video.Freq * speed);
            if (interval < 10) interval = 10;
            timer.Interval = interval;
            timer.Start();
        }

        private void timer_Tick(object sender, EventArgs e)
        {
            int curr = (int)udFrame.Value;
            curr++;
            if (curr < video.CountFrames)
            {
                SetFrame(curr);
            }
            else
                timer.Stop();
        }

        private void btFullScreen_Click(object sender, EventArgs e)
        {
            if (btFullScreen.Checked)
            {
                szFullScreen = new Size(0, 0);
                this.Left = oldSizeForm.Left;
                this.Top = oldSizeForm.Top;
                this.Width = oldSizeForm.Width;
                this.Height = oldSizeForm.Height;
            }
            else
            {
                Rectangle rectScreen = Screen.PrimaryScreen.Bounds;
                oldSizeForm = new Rectangle(this.Left, this.Top, this.Width, this.Height);
                Point begImage = this.PointToScreen(new Point(0, 0));
                int width = rectScreen.Width + panelButtons.Width + (begImage.X - this.Left) + 2 * tlpMain.Margin.Left;
                int height = rectScreen.Height + panelInfo.Height + (begImage.Y - this.Top) + 2 * tlpMain.Margin.Left;
                this.szFullScreen = new Size(width, height);
                this.Left = 0 - (begImage.X - this.Left);
                this.Top = 0 - (begImage.Y - this.Top);
                this.Width = width;
                this.Height = height;
            }
            btFullScreen.Checked = !btFullScreen.Checked;
        }


        private const long WM_GETMINMAXINFO = 0x24;

        public struct POINTAPI
        {
            public int x;
            public int y;
        }

        public struct MINMAXINFO
        {
            public POINTAPI ptReserved;
            public POINTAPI ptMaxSize;
            public POINTAPI ptMaxPosition;
            public POINTAPI ptMinTrackSize;
            public POINTAPI ptMaxTrackSize;
        }
       
        protected override void WndProc(ref Message m)
        {
            base.WndProc(ref m);
            if (m.Msg == WM_GETMINMAXINFO)
            {
                MINMAXINFO mmi = (MINMAXINFO)m.GetLParam(typeof(MINMAXINFO));
                mmi.ptMinTrackSize.x = this.szFullScreen.Width;
                mmi.ptMinTrackSize.y = this.szFullScreen.Height;
                //mmi.ptMaxSize.x = int.MaxValue;// this.MaximumSize.Width;
                //mmi.ptMaxSize.y = int.MaxValue; // this.MaximumSize.Height;
                //mmi.ptMaxTrackSize.x = int.MaxValue; // this.MaximumSize.Width;
                //mmi.ptMaxTrackSize.y = int.MaxValue; // this.MaximumSize.Height;
                System.Runtime.InteropServices.Marshal.StructureToPtr(mmi, m.LParam, true);
                //m.Result = (IntPtr)0;
            }
        }

        private void pbImage_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                selBegX = e.X;
                selBegY = e.Y;
                isSel = 1;
            }
        }

        private void pbImage_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                if (isSel == 1)
                {
                    selEndX = e.X;
                    selEndY = e.Y;
                    if (selBegX == selEndX || selBegY == selEndY)
                        isSel = 0;
                    else
                        isSel = 2;
                    pbImage.Invalidate();
                }
            }
        }

        private void pbImage_MouseMove(object sender, MouseEventArgs e)
        {
            if (isSel == 1)
            {
                selEndX = e.X;
                selEndY = e.Y;
                pbImage.Invalidate();
            }
        }

        private void pbImage_Paint(object sender, PaintEventArgs e)
        {
            if (isSel != 0)
            {
                Pen pen = new Pen(isSel == 3 ? Color.Green : Color.Red);
                pen.DashStyle = System.Drawing.Drawing2D.DashStyle.Dash;
                Rectangle r = NormalSelect();
                e.Graphics.DrawRectangle(pen, r);
            }
        }

        Rectangle NormalSelect()
        {
            int x1, y1, x2, y2;
            if (selBegX < selEndX)
            {
                x1 = selBegX;
                x2 = selEndX;
            }
            else
            {
                x2 = selBegX;
                x1 = selEndX;
            }
            if (selBegY < selEndY)
            {
                y1 = selBegY;
                y2 = selEndY;
            }
            else
            {
                y2 = selBegY;
                y1 = selEndY;
            }
            return new Rectangle(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
        }

        private void btFilter_Click(object sender, EventArgs e)
        {
            if (!btFilter.Checked)
            {
                if (isSel == 2)
                {
                    Rectangle r = NormalSelect();
                    byte[] m = video.GetPartFrame((int)udFrame.Value, r);
                    currFilter = new Filter(m, r.Width, r.Height, 2);
                    video.SetFilter(currFilter);
                    isSel = 3;
                    pbImage.Invalidate();
                }
                if (currFilter != null)
                {
                    filterOn = true;
                    btFilter.Checked = true;
                    video.FilterOn = true;
                }
            }
            else
            {
                filterOn = false;
                btFilter.Checked = false;
                video.FilterOn =  false;
            }
        }

        private void udFrame_Validated(object sender, EventArgs e)
        {
            //SetFrame((int)udFrame.Value);
        }

        private void scroolVideo_Validated(object sender, EventArgs e)
        {
            //SetFrame(scroolVideo.Value);
        }

        private void cbCaptions_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (cbCaptions.SelectedIndex >= 0)
            {
                string s = (string)cbCaptions.Items[cbCaptions.SelectedIndex];
                video.FillCBFrames(s, cbCaptionsFrames);
                int n = 0;
                video.GetNearPosFrames(s, (int)udFrame.Value, ref n);
                cbCaptionsFrames.SelectedIndex = n;
            }
            pbScale.Invalidate();
        }

        private void cbCaptionsFrames_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (cbCaptionsFrames.SelectedIndex >= 0 && cbCaptions.SelectedIndex >= 0)
            {
                string s = (string)cbCaptions.Items[cbCaptions.SelectedIndex];
                int[] fm = video.GetPosFrames(s, cbCaptionsFrames.SelectedIndex);
                SetFrame(fm[0]);
            }
        }

        private void pbScale_Paint(object sender, PaintEventArgs e)
        {
            if (video == null) return;
            TimeSpan ts = video.DateEnd.Subtract(video.DateBeg);
            int dx = 0;
            int dt = 0;
            int seconds = (int)ts.TotalSeconds;
            if (seconds == 0)
            {
                seconds = video.CountFrames / video.Freq;
                if (seconds == 0) seconds = video.CountFrames;
            }
            if (seconds == 0) return;
            while (dx < 30)
            {
                dt++;
                dx = pbScale.Width * dt / seconds;
            }
            int top = 10; 
            int cf = pbScale.Width / dx; //количество отсчетов
            double tf = (double)seconds / cf; 
            double dx2 = (double)pbScale.Width / cf;
            Pen pen = new Pen(Color.Black);
            StringFormat format = new StringFormat();
            if (cbCaptions.SelectedIndex >= 0)
            {
                string s = (string)cbCaptions.Items[cbCaptions.SelectedIndex];
                SolidBrush sbr = new SolidBrush(Color.LightGreen);
                for (int i = 0; i < cbCaptionsFrames.Items.Count; i++)
                {
                    int[] fm = video.GetPosFrames(s, i);
                    if (fm != null)
                    {
                        int x1 = fm[0] * pbScale.Width / video.CountFrames;
                        int x2 = fm[1] * pbScale.Width / video.CountFrames;
                        e.Graphics.FillRectangle(sbr, new Rectangle(x1, top - 3, x2 - x1, 6));
                    }
                }
            }
            e.Graphics.DrawLine(pen, 0, top, pbScale.Width, top);
            format.Alignment = StringAlignment.Center;
            for (int i = 1; i < cf; i++)
            {
                int x = (int)(i * dx2);
                e.Graphics.DrawLine(pen, x, top, x, top + 5);
                int v = (int)(i * tf);
                e.Graphics.DrawString(v.ToString(), Font, Brushes.Black, new RectangleF(x - dx / 2, top + 7, dx, 20), format);
            }
            int posCursor = video.CountFrames > 0 ? numCurrFrame * pbScale.Width / video.CountFrames : 0;
            Pen penCursor = new Pen(Color.Blue);
            e.Graphics.DrawLines(penCursor, new Point[]{ new Point(posCursor, top), new Point(posCursor - 5, 0),new Point(posCursor + 5, 0), new Point(posCursor, top)});
        }

        private void pbScale_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                if (video != null)
                {
                    int n = video.CountFrames * e.X / pbScale.Width;
                    SetFrame(n);
                }
            }
        }

        private void FormPlayer_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (threadLoad != null) threadLoad.Abort();
        }

        private void timer2_Tick(object sender, EventArgs e)
        {
            if (progressLoadVideo.Visible)
            {
                if (threadLoad.ThreadState == ThreadState.Stopped)
                {
                    progressLoadVideo.Visible = false;
                    labelFramesFulled.Visible = false;
                    tbFramesFulled.Visible = false;
                }
                else
                {
                    progressLoadVideo.Value = video.PosLoadVideo;
                    tbFramesFulled.Text = video.PosLoadVideo.ToString();
                }
            }
        }

        private void dateTimeVideo_ValueChanged(object sender, EventArgs e)
        {
            SetTimeVideo(dateTimeVideo.Value);
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
                    string pathVideo = Application.StartupPath + "\\video";
                    string file = Video.FindAnchor(pathVideo, anchor, out posTime, out uid);
                    if (file.Length > 0)
                    {
                        OpenFile(file, uid);
                        SetTimeVideo(posTime);
                    }
                    else
                        MessageBox.Show("По указанному якорю ничего не найдено");
                }
            }
        }

        private void dateTimeVideoBot_ValueChanged(object sender, EventArgs e)
        {
            SetTimeVideoBot(dateTimeVideoBot.Value);
        }

        private void chkZoom_CheckedChanged(object sender, EventArgs e)
        {
            if (chkZoom.Checked)
            {
                panel2.AutoScroll = false;
                panel2.AutoSize = true;
                pbImage.SizeMode = PictureBoxSizeMode.Zoom;
            }
            else
            {
                panel2.AutoScroll = true;
                panel2.AutoSize = false;
                pbImage.SizeMode = PictureBoxSizeMode.AutoSize;
            }
        }
        private void tlpMain_Resize(object sender, EventArgs e)
        {
            SetFrame(numCurrFrame);
        }
    }
}
