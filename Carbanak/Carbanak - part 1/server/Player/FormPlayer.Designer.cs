namespace Player
{
    partial class FormPlayer
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
			this.components = new System.ComponentModel.Container();
			this.tlpMain = new System.Windows.Forms.TableLayoutPanel();
			this.scroolVideo = new System.Windows.Forms.HScrollBar();
			this.panelInfo = new System.Windows.Forms.Panel();
			this.chkZoom = new System.Windows.Forms.CheckBox();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.dateTimeVideoBot = new System.Windows.Forms.DateTimePicker();
			this.label11 = new System.Windows.Forms.Label();
			this.dateTimeVideo = new System.Windows.Forms.DateTimePicker();
			this.label9 = new System.Windows.Forms.Label();
			this.tbLenVideo = new System.Windows.Forms.TextBox();
			this.label5 = new System.Windows.Forms.Label();
			this.tbSize = new System.Windows.Forms.TextBox();
			this.label4 = new System.Windows.Forms.Label();
			this.tbWH = new System.Windows.Forms.TextBox();
			this.label3 = new System.Windows.Forms.Label();
			this.tbCountFrames = new System.Windows.Forms.TextBox();
			this.label2 = new System.Windows.Forms.Label();
			this.udFrame = new System.Windows.Forms.NumericUpDown();
			this.label1 = new System.Windows.Forms.Label();
			this.panel2 = new System.Windows.Forms.Panel();
			this.pbImage = new System.Windows.Forms.PictureBox();
			this.panel4 = new System.Windows.Forms.Panel();
			this.toolStrip1 = new System.Windows.Forms.ToolStrip();
			this.btStop = new System.Windows.Forms.ToolStripButton();
			this.btPlay = new System.Windows.Forms.ToolStripButton();
			this.btForward = new System.Windows.Forms.ToolStripButton();
			this.cbSpeed = new System.Windows.Forms.ToolStripComboBox();
			this.btFullScreen = new System.Windows.Forms.ToolStripButton();
			this.btFilter = new System.Windows.Forms.ToolStripButton();
			this.pbScale = new System.Windows.Forms.PictureBox();
			this.panelButtons = new System.Windows.Forms.Panel();
			this.btOpenAnchor = new System.Windows.Forms.Button();
			this.btBotsRecord = new System.Windows.Forms.Button();
			this.btOpen = new System.Windows.Forms.Button();
			this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
			this.label6 = new System.Windows.Forms.Label();
			this.cbCaptions = new System.Windows.Forms.ComboBox();
			this.label7 = new System.Windows.Forms.Label();
			this.cbCaptionsFrames = new System.Windows.Forms.ComboBox();
			this.label8 = new System.Windows.Forms.Label();
			this.tbNameBot = new System.Windows.Forms.TextBox();
			this.tbFileVideo = new System.Windows.Forms.TextBox();
			this.label10 = new System.Windows.Forms.Label();
			this.panelWaiting = new System.Windows.Forms.Panel();
			this.tbFramesFulled = new System.Windows.Forms.TextBox();
			this.labelFramesFulled = new System.Windows.Forms.Label();
			this.progressLoadVideo = new System.Windows.Forms.ProgressBar();
			this.dlgOpenFile = new System.Windows.Forms.OpenFileDialog();
			this.timer = new System.Windows.Forms.Timer(this.components);
			this.timer2 = new System.Windows.Forms.Timer(this.components);
			this.tlpMain.SuspendLayout();
			this.panelInfo.SuspendLayout();
			this.groupBox1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.udFrame)).BeginInit();
			this.panel2.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.pbImage)).BeginInit();
			this.panel4.SuspendLayout();
			this.toolStrip1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.pbScale)).BeginInit();
			this.panelButtons.SuspendLayout();
			this.tableLayoutPanel1.SuspendLayout();
			this.panelWaiting.SuspendLayout();
			this.SuspendLayout();
			// 
			// tlpMain
			// 
			this.tlpMain.ColumnCount = 2;
			this.tlpMain.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
			this.tlpMain.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
			this.tlpMain.Controls.Add(this.scroolVideo, 0, 2);
			this.tlpMain.Controls.Add(this.panelInfo, 0, 5);
			this.tlpMain.Controls.Add(this.panel2, 0, 1);
			this.tlpMain.Controls.Add(this.panel4, 0, 4);
			this.tlpMain.Controls.Add(this.pbScale, 0, 3);
			this.tlpMain.Controls.Add(this.panelButtons, 1, 0);
			this.tlpMain.Controls.Add(this.tableLayoutPanel1, 0, 0);
			this.tlpMain.Controls.Add(this.panelWaiting, 1, 5);
			this.tlpMain.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tlpMain.Location = new System.Drawing.Point(0, 0);
			this.tlpMain.Name = "tlpMain";
			this.tlpMain.RowCount = 6;
			this.tlpMain.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30F));
			this.tlpMain.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
			this.tlpMain.RowStyles.Add(new System.Windows.Forms.RowStyle());
			this.tlpMain.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 40F));
			this.tlpMain.RowStyles.Add(new System.Windows.Forms.RowStyle());
			this.tlpMain.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 80F));
			this.tlpMain.Size = new System.Drawing.Size(881, 534);
			this.tlpMain.TabIndex = 0;
			this.tlpMain.Resize += new System.EventHandler(this.tlpMain_Resize);
			// 
			// scroolVideo
			// 
			this.scroolVideo.Dock = System.Windows.Forms.DockStyle.Top;
			this.scroolVideo.LargeChange = 1;
			this.scroolVideo.Location = new System.Drawing.Point(0, 360);
			this.scroolVideo.Maximum = 10;
			this.scroolVideo.Name = "scroolVideo";
			this.scroolVideo.Size = new System.Drawing.Size(762, 17);
			this.scroolVideo.TabIndex = 2;
			this.scroolVideo.ValueChanged += new System.EventHandler(this.scroolVideo_ValueChanged);
			this.scroolVideo.Validated += new System.EventHandler(this.scroolVideo_Validated);
			// 
			// panelInfo
			// 
			this.panelInfo.Controls.Add(this.chkZoom);
			this.panelInfo.Controls.Add(this.groupBox1);
			this.panelInfo.Controls.Add(this.tbLenVideo);
			this.panelInfo.Controls.Add(this.label5);
			this.panelInfo.Controls.Add(this.tbSize);
			this.panelInfo.Controls.Add(this.label4);
			this.panelInfo.Controls.Add(this.tbWH);
			this.panelInfo.Controls.Add(this.label3);
			this.panelInfo.Controls.Add(this.tbCountFrames);
			this.panelInfo.Controls.Add(this.label2);
			this.panelInfo.Controls.Add(this.udFrame);
			this.panelInfo.Controls.Add(this.label1);
			this.panelInfo.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panelInfo.Location = new System.Drawing.Point(3, 457);
			this.panelInfo.Name = "panelInfo";
			this.panelInfo.Size = new System.Drawing.Size(756, 74);
			this.panelInfo.TabIndex = 3;
			// 
			// chkZoom
			// 
			this.chkZoom.AutoSize = true;
			this.chkZoom.Location = new System.Drawing.Point(637, 16);
			this.chkZoom.Margin = new System.Windows.Forms.Padding(2);
			this.chkZoom.Name = "chkZoom";
			this.chkZoom.Size = new System.Drawing.Size(113, 17);
			this.chkZoom.TabIndex = 16;
			this.chkZoom.Text = "Масштабировать";
			this.chkZoom.UseVisualStyleBackColor = true;
			this.chkZoom.CheckedChanged += new System.EventHandler(this.chkZoom_CheckedChanged);
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.dateTimeVideoBot);
			this.groupBox1.Controls.Add(this.label11);
			this.groupBox1.Controls.Add(this.dateTimeVideo);
			this.groupBox1.Controls.Add(this.label9);
			this.groupBox1.Location = new System.Drawing.Point(475, 3);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(157, 62);
			this.groupBox1.TabIndex = 14;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Время:";
			// 
			// dateTimeVideoBot
			// 
			this.dateTimeVideoBot.Format = System.Windows.Forms.DateTimePickerFormat.Time;
			this.dateTimeVideoBot.Location = new System.Drawing.Point(71, 38);
			this.dateTimeVideoBot.Name = "dateTimeVideoBot";
			this.dateTimeVideoBot.ShowUpDown = true;
			this.dateTimeVideoBot.Size = new System.Drawing.Size(70, 20);
			this.dateTimeVideoBot.TabIndex = 15;
			this.dateTimeVideoBot.ValueChanged += new System.EventHandler(this.dateTimeVideoBot_ValueChanged);
			// 
			// label11
			// 
			this.label11.AutoSize = true;
			this.label11.Location = new System.Drawing.Point(5, 41);
			this.label11.Name = "label11";
			this.label11.Size = new System.Drawing.Size(66, 13);
			this.label11.TabIndex = 14;
			this.label11.Text = "на клиенте:";
			// 
			// dateTimeVideo
			// 
			this.dateTimeVideo.Format = System.Windows.Forms.DateTimePickerFormat.Time;
			this.dateTimeVideo.Location = new System.Drawing.Point(71, 13);
			this.dateTimeVideo.Name = "dateTimeVideo";
			this.dateTimeVideo.ShowUpDown = true;
			this.dateTimeVideo.Size = new System.Drawing.Size(70, 20);
			this.dateTimeVideo.TabIndex = 13;
			this.dateTimeVideo.ValueChanged += new System.EventHandler(this.dateTimeVideo_ValueChanged);
			// 
			// label9
			// 
			this.label9.AutoSize = true;
			this.label9.Location = new System.Drawing.Point(5, 16);
			this.label9.Name = "label9";
			this.label9.Size = new System.Drawing.Size(67, 13);
			this.label9.TabIndex = 12;
			this.label9.Text = "на сервере:";
			// 
			// tbLenVideo
			// 
			this.tbLenVideo.Location = new System.Drawing.Point(388, 7);
			this.tbLenVideo.Name = "tbLenVideo";
			this.tbLenVideo.ReadOnly = true;
			this.tbLenVideo.Size = new System.Drawing.Size(57, 20);
			this.tbLenVideo.TabIndex = 9;
			// 
			// label5
			// 
			this.label5.AutoSize = true;
			this.label5.Location = new System.Drawing.Point(309, 10);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(73, 13);
			this.label5.TabIndex = 8;
			this.label5.Text = "длина видео:";
			// 
			// tbSize
			// 
			this.tbSize.Location = new System.Drawing.Point(312, 34);
			this.tbSize.Name = "tbSize";
			this.tbSize.ReadOnly = true;
			this.tbSize.Size = new System.Drawing.Size(84, 20);
			this.tbSize.TabIndex = 7;
			// 
			// label4
			// 
			this.label4.AutoSize = true;
			this.label4.Location = new System.Drawing.Point(198, 37);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(108, 13);
			this.label4.TabIndex = 6;
			this.label4.Text = "Длина кадра (байт):";
			// 
			// tbWH
			// 
			this.tbWH.Location = new System.Drawing.Point(92, 34);
			this.tbWH.Name = "tbWH";
			this.tbWH.ReadOnly = true;
			this.tbWH.Size = new System.Drawing.Size(100, 20);
			this.tbWH.TabIndex = 5;
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(9, 37);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(82, 13);
			this.label3.TabIndex = 4;
			this.label3.Text = "Размер кадра:";
			// 
			// tbCountFrames
			// 
			this.tbCountFrames.Location = new System.Drawing.Point(238, 7);
			this.tbCountFrames.Name = "tbCountFrames";
			this.tbCountFrames.ReadOnly = true;
			this.tbCountFrames.Size = new System.Drawing.Size(57, 20);
			this.tbCountFrames.TabIndex = 3;
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(154, 10);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(78, 13);
			this.label2.TabIndex = 2;
			this.label2.Text = "всего кадров:";
			// 
			// udFrame
			// 
			this.udFrame.Location = new System.Drawing.Point(92, 8);
			this.udFrame.Name = "udFrame";
			this.udFrame.Size = new System.Drawing.Size(55, 20);
			this.udFrame.TabIndex = 1;
			this.udFrame.ValueChanged += new System.EventHandler(this.udFrame_ValueChanged);
			this.udFrame.Validated += new System.EventHandler(this.udFrame_Validated);
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(9, 10);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(77, 13);
			this.label1.TabIndex = 0;
			this.label1.Text = "Номер кадра:";
			// 
			// panel2
			// 
			this.panel2.AutoScroll = true;
			this.panel2.Controls.Add(this.pbImage);
			this.panel2.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panel2.Location = new System.Drawing.Point(3, 33);
			this.panel2.Name = "panel2";
			this.panel2.Size = new System.Drawing.Size(756, 324);
			this.panel2.TabIndex = 4;
			// 
			// pbImage
			// 
			this.pbImage.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.pbImage.Location = new System.Drawing.Point(0, 0);
			this.pbImage.Name = "pbImage";
			this.pbImage.Size = new System.Drawing.Size(762, 448);
			this.pbImage.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
			this.pbImage.TabIndex = 1;
			this.pbImage.TabStop = false;
			this.pbImage.Paint += new System.Windows.Forms.PaintEventHandler(this.pbImage_Paint);
			this.pbImage.MouseDown += new System.Windows.Forms.MouseEventHandler(this.pbImage_MouseDown);
			this.pbImage.MouseMove += new System.Windows.Forms.MouseEventHandler(this.pbImage_MouseMove);
			this.pbImage.MouseUp += new System.Windows.Forms.MouseEventHandler(this.pbImage_MouseUp);
			// 
			// panel4
			// 
			this.panel4.Anchor = System.Windows.Forms.AnchorStyles.None;
			this.panel4.AutoSize = true;
			this.panel4.Controls.Add(this.toolStrip1);
			this.panel4.Location = new System.Drawing.Point(288, 420);
			this.panel4.Name = "panel4";
			this.panel4.Size = new System.Drawing.Size(185, 31);
			this.panel4.TabIndex = 6;
			// 
			// toolStrip1
			// 
			this.toolStrip1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.toolStrip1.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
			this.toolStrip1.ImageScalingSize = new System.Drawing.Size(24, 24);
			this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.btStop,
            this.btPlay,
            this.btForward,
            this.cbSpeed,
            this.btFullScreen,
            this.btFilter});
			this.toolStrip1.Location = new System.Drawing.Point(0, 0);
			this.toolStrip1.Name = "toolStrip1";
			this.toolStrip1.RenderMode = System.Windows.Forms.ToolStripRenderMode.System;
			this.toolStrip1.Size = new System.Drawing.Size(185, 31);
			this.toolStrip1.TabIndex = 1;
			this.toolStrip1.Text = "toolStrip1";
			// 
			// btStop
			// 
			this.btStop.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.btStop.Image = global::Player.Properties.Resources.stop;
			this.btStop.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btStop.Name = "btStop";
			this.btStop.Size = new System.Drawing.Size(28, 28);
			this.btStop.Text = "Стоп";
			this.btStop.Click += new System.EventHandler(this.btStop_Click);
			// 
			// btPlay
			// 
			this.btPlay.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.btPlay.Image = global::Player.Properties.Resources.play;
			this.btPlay.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btPlay.Name = "btPlay";
			this.btPlay.Size = new System.Drawing.Size(28, 28);
			this.btPlay.Text = "toolStripButton2";
			this.btPlay.ToolTipText = "Воспроизведение";
			this.btPlay.Click += new System.EventHandler(this.btPlay_Click);
			// 
			// btForward
			// 
			this.btForward.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.btForward.Image = global::Player.Properties.Resources.forward;
			this.btForward.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btForward.Name = "btForward";
			this.btForward.Size = new System.Drawing.Size(28, 28);
			this.btForward.Text = "toolStripButton3";
			this.btForward.ToolTipText = "Вперед быстро";
			this.btForward.Click += new System.EventHandler(this.btForward_Click);
			// 
			// cbSpeed
			// 
			this.cbSpeed.AutoSize = false;
			this.cbSpeed.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cbSpeed.Items.AddRange(new object[] {
            "1",
            "2",
            "4",
            "8",
            "16",
            "32"});
			this.cbSpeed.Name = "cbSpeed";
			this.cbSpeed.Size = new System.Drawing.Size(40, 23);
			// 
			// btFullScreen
			// 
			this.btFullScreen.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.btFullScreen.Image = global::Player.Properties.Resources.fullscreen;
			this.btFullScreen.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btFullScreen.Name = "btFullScreen";
			this.btFullScreen.Size = new System.Drawing.Size(28, 28);
			this.btFullScreen.Text = "toolStripfullScreenBtn";
			this.btFullScreen.ToolTipText = "На весь экран";
			this.btFullScreen.Click += new System.EventHandler(this.btFullScreen_Click);
			// 
			// btFilter
			// 
			this.btFilter.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.btFilter.Image = global::Player.Properties.Resources.filter;
			this.btFilter.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btFilter.Name = "btFilter";
			this.btFilter.Size = new System.Drawing.Size(28, 28);
			this.btFilter.Text = "Фильтр";
			this.btFilter.Click += new System.EventHandler(this.btFilter_Click);
			// 
			// pbScale
			// 
			this.pbScale.Dock = System.Windows.Forms.DockStyle.Fill;
			this.pbScale.Location = new System.Drawing.Point(0, 377);
			this.pbScale.Margin = new System.Windows.Forms.Padding(0);
			this.pbScale.Name = "pbScale";
			this.pbScale.Size = new System.Drawing.Size(762, 40);
			this.pbScale.TabIndex = 7;
			this.pbScale.TabStop = false;
			this.pbScale.Paint += new System.Windows.Forms.PaintEventHandler(this.pbScale_Paint);
			this.pbScale.MouseDown += new System.Windows.Forms.MouseEventHandler(this.pbScale_MouseDown);
			// 
			// panelButtons
			// 
			this.panelButtons.AutoSize = true;
			this.panelButtons.Controls.Add(this.btOpenAnchor);
			this.panelButtons.Controls.Add(this.btBotsRecord);
			this.panelButtons.Controls.Add(this.btOpen);
			this.panelButtons.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panelButtons.Location = new System.Drawing.Point(765, 3);
			this.panelButtons.Name = "panelButtons";
			this.tlpMain.SetRowSpan(this.panelButtons, 2);
			this.panelButtons.Size = new System.Drawing.Size(113, 354);
			this.panelButtons.TabIndex = 5;
			// 
			// btOpenAnchor
			// 
			this.btOpenAnchor.Location = new System.Drawing.Point(3, 89);
			this.btOpenAnchor.Name = "btOpenAnchor";
			this.btOpenAnchor.Size = new System.Drawing.Size(99, 37);
			this.btOpenAnchor.TabIndex = 5;
			this.btOpenAnchor.Text = "Открыть по якорю";
			this.btOpenAnchor.UseVisualStyleBackColor = true;
			this.btOpenAnchor.Click += new System.EventHandler(this.btOpenAnchor_Click);
			// 
			// btBotsRecord
			// 
			this.btBotsRecord.Location = new System.Drawing.Point(3, 46);
			this.btBotsRecord.Name = "btBotsRecord";
			this.btBotsRecord.Size = new System.Drawing.Size(99, 37);
			this.btBotsRecord.TabIndex = 4;
			this.btBotsRecord.Text = "Записанное видео";
			this.btBotsRecord.UseVisualStyleBackColor = true;
			this.btBotsRecord.Click += new System.EventHandler(this.btBotsRecord_Click);
			// 
			// btOpen
			// 
			this.btOpen.Location = new System.Drawing.Point(3, 3);
			this.btOpen.Name = "btOpen";
			this.btOpen.Size = new System.Drawing.Size(99, 37);
			this.btOpen.TabIndex = 2;
			this.btOpen.Text = "Открыть видео";
			this.btOpen.UseVisualStyleBackColor = true;
			this.btOpen.Click += new System.EventHandler(this.btOpen_Click);
			// 
			// tableLayoutPanel1
			// 
			this.tableLayoutPanel1.AutoSize = true;
			this.tableLayoutPanel1.ColumnCount = 8;
			this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
			this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 33.33333F));
			this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
			this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 33.29161F));
			this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
			this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 33.37505F));
			this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
			this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 189F));
			this.tableLayoutPanel1.Controls.Add(this.label6, 4, 0);
			this.tableLayoutPanel1.Controls.Add(this.cbCaptions, 5, 0);
			this.tableLayoutPanel1.Controls.Add(this.label7, 6, 0);
			this.tableLayoutPanel1.Controls.Add(this.cbCaptionsFrames, 7, 0);
			this.tableLayoutPanel1.Controls.Add(this.label8, 2, 0);
			this.tableLayoutPanel1.Controls.Add(this.tbNameBot, 3, 0);
			this.tableLayoutPanel1.Controls.Add(this.tbFileVideo, 1, 0);
			this.tableLayoutPanel1.Controls.Add(this.label10, 0, 0);
			this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 0);
			this.tableLayoutPanel1.Margin = new System.Windows.Forms.Padding(0);
			this.tableLayoutPanel1.Name = "tableLayoutPanel1";
			this.tableLayoutPanel1.RowCount = 1;
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
			this.tableLayoutPanel1.Size = new System.Drawing.Size(762, 30);
			this.tableLayoutPanel1.TabIndex = 8;
			// 
			// label6
			// 
			this.label6.Anchor = System.Windows.Forms.AnchorStyles.Left;
			this.label6.AutoSize = true;
			this.label6.Location = new System.Drawing.Point(331, 8);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(64, 13);
			this.label6.TabIndex = 0;
			this.label6.Text = "Заголовок:";
			// 
			// cbCaptions
			// 
			this.cbCaptions.Dock = System.Windows.Forms.DockStyle.Fill;
			this.cbCaptions.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cbCaptions.FormattingEnabled = true;
			this.cbCaptions.Location = new System.Drawing.Point(401, 3);
			this.cbCaptions.Name = "cbCaptions";
			this.cbCaptions.Size = new System.Drawing.Size(119, 21);
			this.cbCaptions.TabIndex = 1;
			this.cbCaptions.SelectedIndexChanged += new System.EventHandler(this.cbCaptions_SelectedIndexChanged);
			// 
			// label7
			// 
			this.label7.Anchor = System.Windows.Forms.AnchorStyles.Left;
			this.label7.AutoSize = true;
			this.label7.Location = new System.Drawing.Point(526, 8);
			this.label7.Name = "label7";
			this.label7.Size = new System.Drawing.Size(43, 13);
			this.label7.TabIndex = 2;
			this.label7.Text = "Кадры:";
			// 
			// cbCaptionsFrames
			// 
			this.cbCaptionsFrames.Dock = System.Windows.Forms.DockStyle.Fill;
			this.cbCaptionsFrames.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cbCaptionsFrames.FormattingEnabled = true;
			this.cbCaptionsFrames.Location = new System.Drawing.Point(575, 3);
			this.cbCaptionsFrames.Name = "cbCaptionsFrames";
			this.cbCaptionsFrames.Size = new System.Drawing.Size(184, 21);
			this.cbCaptionsFrames.TabIndex = 3;
			this.cbCaptionsFrames.SelectedIndexChanged += new System.EventHandler(this.cbCaptionsFrames_SelectedIndexChanged);
			// 
			// label8
			// 
			this.label8.Anchor = System.Windows.Forms.AnchorStyles.Left;
			this.label8.AutoSize = true;
			this.label8.Location = new System.Drawing.Point(173, 8);
			this.label8.Name = "label8";
			this.label8.Size = new System.Drawing.Size(28, 13);
			this.label8.TabIndex = 4;
			this.label8.Text = "Бот:";
			// 
			// tbNameBot
			// 
			this.tbNameBot.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tbNameBot.Location = new System.Drawing.Point(207, 3);
			this.tbNameBot.Name = "tbNameBot";
			this.tbNameBot.ReadOnly = true;
			this.tbNameBot.Size = new System.Drawing.Size(118, 20);
			this.tbNameBot.TabIndex = 5;
			// 
			// tbFileVideo
			// 
			this.tbFileVideo.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.tbFileVideo.Location = new System.Drawing.Point(48, 3);
			this.tbFileVideo.Name = "tbFileVideo";
			this.tbFileVideo.ReadOnly = true;
			this.tbFileVideo.Size = new System.Drawing.Size(119, 20);
			this.tbFileVideo.TabIndex = 6;
			// 
			// label10
			// 
			this.label10.Anchor = System.Windows.Forms.AnchorStyles.Left;
			this.label10.AutoSize = true;
			this.label10.Location = new System.Drawing.Point(3, 8);
			this.label10.Name = "label10";
			this.label10.Size = new System.Drawing.Size(39, 13);
			this.label10.TabIndex = 7;
			this.label10.Text = "Файл:";
			// 
			// panelWaiting
			// 
			this.panelWaiting.AutoSize = true;
			this.panelWaiting.Controls.Add(this.tbFramesFulled);
			this.panelWaiting.Controls.Add(this.labelFramesFulled);
			this.panelWaiting.Controls.Add(this.progressLoadVideo);
			this.panelWaiting.Location = new System.Drawing.Point(765, 457);
			this.panelWaiting.Name = "panelWaiting";
			this.panelWaiting.Size = new System.Drawing.Size(113, 65);
			this.panelWaiting.TabIndex = 9;
			// 
			// tbFramesFulled
			// 
			this.tbFramesFulled.Location = new System.Drawing.Point(3, 42);
			this.tbFramesFulled.Name = "tbFramesFulled";
			this.tbFramesFulled.ReadOnly = true;
			this.tbFramesFulled.Size = new System.Drawing.Size(41, 20);
			this.tbFramesFulled.TabIndex = 14;
			// 
			// labelFramesFulled
			// 
			this.labelFramesFulled.AutoSize = true;
			this.labelFramesFulled.Location = new System.Drawing.Point(0, 27);
			this.labelFramesFulled.Name = "labelFramesFulled";
			this.labelFramesFulled.Size = new System.Drawing.Size(110, 13);
			this.labelFramesFulled.TabIndex = 13;
			this.labelFramesFulled.Text = "Обработано кадров:";
			// 
			// progressLoadVideo
			// 
			this.progressLoadVideo.Location = new System.Drawing.Point(0, 1);
			this.progressLoadVideo.Maximum = 3;
			this.progressLoadVideo.Name = "progressLoadVideo";
			this.progressLoadVideo.Size = new System.Drawing.Size(107, 23);
			this.progressLoadVideo.Step = 1;
			this.progressLoadVideo.TabIndex = 12;
			this.progressLoadVideo.Visible = false;
			// 
			// dlgOpenFile
			// 
			this.dlgOpenFile.Filter = "Video files|*.frm";
			// 
			// timer
			// 
			this.timer.Tick += new System.EventHandler(this.timer_Tick);
			// 
			// timer2
			// 
			this.timer2.Interval = 500;
			this.timer2.Tick += new System.EventHandler(this.timer2_Tick);
			// 
			// FormPlayer
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(881, 534);
			this.Controls.Add(this.tlpMain);
			this.MaximumSize = new System.Drawing.Size(3002, 3000);
			this.Name = "FormPlayer";
			this.ShowIcon = false;
			this.Text = "Проигрыватель";
			this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.FormPlayer_FormClosing);
			this.tlpMain.ResumeLayout(false);
			this.tlpMain.PerformLayout();
			this.panelInfo.ResumeLayout(false);
			this.panelInfo.PerformLayout();
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.udFrame)).EndInit();
			this.panel2.ResumeLayout(false);
			this.panel2.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.pbImage)).EndInit();
			this.panel4.ResumeLayout(false);
			this.panel4.PerformLayout();
			this.toolStrip1.ResumeLayout(false);
			this.toolStrip1.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.pbScale)).EndInit();
			this.panelButtons.ResumeLayout(false);
			this.tableLayoutPanel1.ResumeLayout(false);
			this.tableLayoutPanel1.PerformLayout();
			this.panelWaiting.ResumeLayout(false);
			this.panelWaiting.PerformLayout();
			this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel tlpMain;
        private System.Windows.Forms.HScrollBar scroolVideo;
        private System.Windows.Forms.Panel panelInfo;
        private System.Windows.Forms.NumericUpDown udFrame;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox tbCountFrames;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox tbSize;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox tbWH;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.OpenFileDialog dlgOpenFile;
        private System.Windows.Forms.TextBox tbLenVideo;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.PictureBox pbImage;
        private System.Windows.Forms.Panel panelButtons;
        private System.Windows.Forms.Button btOpen;
        private System.Windows.Forms.Button btBotsRecord;
        private System.Windows.Forms.Panel panel4;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripButton btStop;
        private System.Windows.Forms.ToolStripButton btPlay;
        private System.Windows.Forms.ToolStripButton btForward;
        private System.Windows.Forms.ToolStripComboBox cbSpeed;
        private System.Windows.Forms.Timer timer;
        private System.Windows.Forms.ToolStripButton btFullScreen;
        private System.Windows.Forms.ToolStripButton btFilter;
        private System.Windows.Forms.PictureBox pbScale;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.ComboBox cbCaptions;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.ComboBox cbCaptionsFrames;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.TextBox tbNameBot;
        private System.Windows.Forms.Timer timer2;
        private System.Windows.Forms.DateTimePicker dateTimeVideo;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Button btOpenAnchor;
        private System.Windows.Forms.TextBox tbFileVideo;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Panel panelWaiting;
        private System.Windows.Forms.TextBox tbFramesFulled;
        private System.Windows.Forms.Label labelFramesFulled;
        private System.Windows.Forms.ProgressBar progressLoadVideo;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.DateTimePicker dateTimeVideoBot;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.CheckBox chkZoom;
    }
}

