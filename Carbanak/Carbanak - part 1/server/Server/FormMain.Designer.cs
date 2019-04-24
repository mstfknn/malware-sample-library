namespace Server
{
    partial class FormMain
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
			this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
			this.lvBots = new System.Windows.Forms.ListView();
			this.clnUid = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.clnInfo = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.clnComment = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.clnPorts = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.clnIp = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.clnDateBeg = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.clnElapsed = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.clnRecv = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.panel1 = new System.Windows.Forms.Panel();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.tbVNCPort = new System.Windows.Forms.TextBox();
			this.label8 = new System.Windows.Forms.Label();
			this.btRunVNC = new System.Windows.Forms.Button();
			this.btRunRDP = new System.Windows.Forms.Button();
			this.btStartSocks5 = new System.Windows.Forms.Button();
			this.btOffBot = new System.Windows.Forms.Button();
			this.tbPortForward = new System.Windows.Forms.TextBox();
			this.btClosePortForward = new System.Windows.Forms.Button();
			this.btPortForward = new System.Windows.Forms.Button();
			this.panel2 = new System.Windows.Forms.Panel();
			this.btResetLogs = new System.Windows.Forms.Button();
			this.tbSendKb = new System.Windows.Forms.TextBox();
			this.btConfig = new System.Windows.Forms.Button();
			this.label7 = new System.Windows.Forms.Label();
			this.tbRecvKb = new System.Windows.Forms.TextBox();
			this.label6 = new System.Windows.Forms.Label();
			this.tableLayoutPanel2 = new System.Windows.Forms.TableLayoutPanel();
			this.tbFilterIP = new System.Windows.Forms.TextBox();
			this.label3 = new System.Windows.Forms.Label();
			this.label4 = new System.Windows.Forms.Label();
			this.tbFilterUID = new System.Windows.Forms.TextBox();
			this.label1 = new System.Windows.Forms.Label();
			this.label5 = new System.Windows.Forms.Label();
			this.tbCountBots = new System.Windows.Forms.TextBox();
			this.tbElapsedServer = new System.Windows.Forms.TextBox();
			this.label2 = new System.Windows.Forms.Label();
			this.tableLayoutPanel3 = new System.Windows.Forms.TableLayoutPanel();
			this.tbComment = new System.Windows.Forms.TextBox();
			this.btCmdBot = new System.Windows.Forms.Button();
			this.tbCmdBot = new System.Windows.Forms.TextBox();
			this.btComment = new System.Windows.Forms.Button();
			this.timer = new System.Windows.Forms.Timer(this.components);
			this.tableLayoutPanel1.SuspendLayout();
			this.panel1.SuspendLayout();
			this.groupBox1.SuspendLayout();
			this.panel2.SuspendLayout();
			this.tableLayoutPanel2.SuspendLayout();
			this.tableLayoutPanel3.SuspendLayout();
			this.SuspendLayout();
			// 
			// tableLayoutPanel1
			// 
			this.tableLayoutPanel1.ColumnCount = 2;
			this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
			this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
			this.tableLayoutPanel1.Controls.Add(this.lvBots, 0, 1);
			this.tableLayoutPanel1.Controls.Add(this.panel1, 0, 3);
			this.tableLayoutPanel1.Controls.Add(this.panel2, 1, 3);
			this.tableLayoutPanel1.Controls.Add(this.tableLayoutPanel2, 0, 0);
			this.tableLayoutPanel1.Controls.Add(this.tableLayoutPanel3, 0, 2);
			this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 0);
			this.tableLayoutPanel1.Name = "tableLayoutPanel1";
			this.tableLayoutPanel1.RowCount = 4;
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
			this.tableLayoutPanel1.Size = new System.Drawing.Size(1035, 538);
			this.tableLayoutPanel1.TabIndex = 0;
			// 
			// lvBots
			// 
			this.lvBots.Activation = System.Windows.Forms.ItemActivation.OneClick;
			this.lvBots.BackColor = System.Drawing.Color.White;
			this.lvBots.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.lvBots.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.clnUid,
            this.clnInfo,
            this.clnComment,
            this.clnPorts,
            this.clnIp,
            this.clnDateBeg,
            this.clnElapsed,
            this.clnRecv});
			this.tableLayoutPanel1.SetColumnSpan(this.lvBots, 2);
			this.lvBots.Dock = System.Windows.Forms.DockStyle.Fill;
			this.lvBots.FullRowSelect = true;
			this.lvBots.GridLines = true;
			this.lvBots.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
			this.lvBots.HideSelection = false;
			this.lvBots.Location = new System.Drawing.Point(0, 26);
			this.lvBots.Margin = new System.Windows.Forms.Padding(0);
			this.lvBots.Name = "lvBots";
			this.lvBots.OwnerDraw = true;
			this.lvBots.Size = new System.Drawing.Size(1035, 353);
			this.lvBots.TabIndex = 2;
			this.lvBots.UseCompatibleStateImageBehavior = false;
			this.lvBots.View = System.Windows.Forms.View.Details;
			this.lvBots.DrawColumnHeader += new System.Windows.Forms.DrawListViewColumnHeaderEventHandler(this.lvBots_DrawColumnHeader);
			this.lvBots.DrawItem += new System.Windows.Forms.DrawListViewItemEventHandler(this.lvBots_DrawItem);
			this.lvBots.SelectedIndexChanged += new System.EventHandler(this.lvBots_SelectedIndexChanged);
			// 
			// clnUid
			// 
			this.clnUid.Text = "ID";
			this.clnUid.Width = 200;
			// 
			// clnInfo
			// 
			this.clnInfo.Text = "Инфо";
			this.clnInfo.Width = 300;
			// 
			// clnComment
			// 
			this.clnComment.Text = "Комментарий";
			this.clnComment.Width = 300;
			// 
			// clnPorts
			// 
			this.clnPorts.Text = "Проброшенные порты";
			this.clnPorts.Width = 200;
			// 
			// clnIp
			// 
			this.clnIp.Text = "IP";
			this.clnIp.Width = 100;
			// 
			// clnDateBeg
			// 
			this.clnDateBeg.Text = "Время соединения";
			this.clnDateBeg.Width = 120;
			// 
			// clnElapsed
			// 
			this.clnElapsed.Text = "Прошло времени";
			this.clnElapsed.Width = 120;
			// 
			// clnRecv
			// 
			this.clnRecv.Text = "Принято Кбайт";
			this.clnRecv.Width = 120;
			// 
			// panel1
			// 
			this.panel1.AutoSize = true;
			this.panel1.Controls.Add(this.groupBox1);
			this.panel1.Location = new System.Drawing.Point(3, 411);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(768, 124);
			this.panel1.TabIndex = 3;
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.tbVNCPort);
			this.groupBox1.Controls.Add(this.label8);
			this.groupBox1.Controls.Add(this.btRunVNC);
			this.groupBox1.Controls.Add(this.btRunRDP);
			this.groupBox1.Controls.Add(this.btStartSocks5);
			this.groupBox1.Controls.Add(this.btOffBot);
			this.groupBox1.Controls.Add(this.tbPortForward);
			this.groupBox1.Controls.Add(this.btClosePortForward);
			this.groupBox1.Controls.Add(this.btPortForward);
			this.groupBox1.Location = new System.Drawing.Point(0, 0);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(765, 121);
			this.groupBox1.TabIndex = 20;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Быстрые команды";
			// 
			// tbVNCPort
			// 
			this.tbVNCPort.Location = new System.Drawing.Point(166, 95);
			this.tbVNCPort.Name = "tbVNCPort";
			this.tbVNCPort.Size = new System.Drawing.Size(72, 20);
			this.tbVNCPort.TabIndex = 22;
			this.tbVNCPort.Text = "5900";
			// 
			// label8
			// 
			this.label8.AutoSize = true;
			this.label8.Location = new System.Drawing.Point(125, 98);
			this.label8.Name = "label8";
			this.label8.Size = new System.Drawing.Size(35, 13);
			this.label8.TabIndex = 21;
			this.label8.Text = "Порт:";
			// 
			// btRunVNC
			// 
			this.btRunVNC.AutoSize = true;
			this.btRunVNC.Image = global::Server.Properties.Resources.vnc_48;
			this.btRunVNC.Location = new System.Drawing.Point(128, 20);
			this.btRunVNC.Name = "btRunVNC";
			this.btRunVNC.Size = new System.Drawing.Size(112, 66);
			this.btRunVNC.TabIndex = 20;
			this.btRunVNC.Text = "Запуск\r\nVNC";
			this.btRunVNC.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.btRunVNC.UseVisualStyleBackColor = true;
			this.btRunVNC.Click += new System.EventHandler(this.btRunVNC_Click);
			// 
			// btRunRDP
			// 
			this.btRunRDP.AutoSize = true;
			this.btRunRDP.Image = global::Server.Properties.Resources.rdp48;
			this.btRunRDP.Location = new System.Drawing.Point(12, 20);
			this.btRunRDP.Name = "btRunRDP";
			this.btRunRDP.Size = new System.Drawing.Size(112, 66);
			this.btRunRDP.TabIndex = 6;
			this.btRunRDP.Text = "Запуск\r\nRDP";
			this.btRunRDP.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.btRunRDP.UseVisualStyleBackColor = true;
			this.btRunRDP.Click += new System.EventHandler(this.btRunRDP_Click);
			// 
			// btStartSocks5
			// 
			this.btStartSocks5.AutoSize = true;
			this.btStartSocks5.Image = global::Server.Properties.Resources.socks48;
			this.btStartSocks5.Location = new System.Drawing.Point(244, 20);
			this.btStartSocks5.Name = "btStartSocks5";
			this.btStartSocks5.Size = new System.Drawing.Size(112, 66);
			this.btStartSocks5.TabIndex = 19;
			this.btStartSocks5.Text = "Запуск\r\nSocks5";
			this.btStartSocks5.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.btStartSocks5.UseVisualStyleBackColor = true;
			this.btStartSocks5.Click += new System.EventHandler(this.btStartSocks5_Click);
			// 
			// btOffBot
			// 
			this.btOffBot.AutoSize = true;
			this.btOffBot.Image = global::Server.Properties.Resources.exit48;
			this.btOffBot.Location = new System.Drawing.Point(617, 20);
			this.btOffBot.Name = "btOffBot";
			this.btOffBot.Size = new System.Drawing.Size(139, 66);
			this.btOffBot.TabIndex = 8;
			this.btOffBot.Text = "Отключить\r\nбот";
			this.btOffBot.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.btOffBot.UseVisualStyleBackColor = true;
			this.btOffBot.Click += new System.EventHandler(this.btOffBot_Click);
			// 
			// tbPortForward
			// 
			this.tbPortForward.Location = new System.Drawing.Point(361, 95);
			this.tbPortForward.Name = "tbPortForward";
			this.tbPortForward.Size = new System.Drawing.Size(112, 20);
			this.tbPortForward.TabIndex = 12;
			// 
			// btClosePortForward
			// 
			this.btClosePortForward.AutoSize = true;
			this.btClosePortForward.Image = global::Server.Properties.Resources.connect_no_48;
			this.btClosePortForward.Location = new System.Drawing.Point(478, 20);
			this.btClosePortForward.Name = "btClosePortForward";
			this.btClosePortForward.Size = new System.Drawing.Size(122, 66);
			this.btClosePortForward.TabIndex = 7;
			this.btClosePortForward.Text = "Закрыть\r\nвсе\r\nпорты";
			this.btClosePortForward.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.btClosePortForward.UseVisualStyleBackColor = true;
			this.btClosePortForward.Click += new System.EventHandler(this.btClosePortForward_Click);
			// 
			// btPortForward
			// 
			this.btPortForward.AutoSize = true;
			this.btPortForward.Image = global::Server.Properties.Resources.connect_creating_48;
			this.btPortForward.Location = new System.Drawing.Point(361, 20);
			this.btPortForward.Name = "btPortForward";
			this.btPortForward.Size = new System.Drawing.Size(123, 66);
			this.btPortForward.TabIndex = 11;
			this.btPortForward.Text = "Проброс\r\nпорта";
			this.btPortForward.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.btPortForward.UseVisualStyleBackColor = true;
			this.btPortForward.Click += new System.EventHandler(this.btPortForward_Click);
			// 
			// panel2
			// 
			this.panel2.AutoSize = true;
			this.panel2.Controls.Add(this.btResetLogs);
			this.panel2.Controls.Add(this.tbSendKb);
			this.panel2.Controls.Add(this.btConfig);
			this.panel2.Controls.Add(this.label7);
			this.panel2.Controls.Add(this.tbRecvKb);
			this.panel2.Controls.Add(this.label6);
			this.panel2.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panel2.Location = new System.Drawing.Point(787, 411);
			this.panel2.Name = "panel2";
			this.panel2.Size = new System.Drawing.Size(245, 124);
			this.panel2.TabIndex = 4;
			// 
			// btResetLogs
			// 
			this.btResetLogs.Location = new System.Drawing.Point(3, 67);
			this.btResetLogs.Name = "btResetLogs";
			this.btResetLogs.Size = new System.Drawing.Size(96, 23);
			this.btResetLogs.TabIndex = 14;
			this.btResetLogs.Text = "Сбросить логи";
			this.btResetLogs.UseVisualStyleBackColor = true;
			this.btResetLogs.Click += new System.EventHandler(this.btResetLogs_Click);
			// 
			// tbSendKb
			// 
			this.tbSendKb.Location = new System.Drawing.Point(155, 43);
			this.tbSendKb.Name = "tbSendKb";
			this.tbSendKb.ReadOnly = true;
			this.tbSendKb.Size = new System.Drawing.Size(80, 20);
			this.tbSendKb.TabIndex = 7;
			this.tbSendKb.Visible = false;
			// 
			// btConfig
			// 
			this.btConfig.AutoSize = true;
			this.btConfig.Image = global::Server.Properties.Resources.cog_icon_2_48x48_3789;
			this.btConfig.Location = new System.Drawing.Point(105, 67);
			this.btConfig.Name = "btConfig";
			this.btConfig.Size = new System.Drawing.Size(137, 54);
			this.btConfig.TabIndex = 13;
			this.btConfig.Text = "Настройки\r\nсервера";
			this.btConfig.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.btConfig.UseVisualStyleBackColor = true;
			this.btConfig.Click += new System.EventHandler(this.btConfig_Click);
			// 
			// label7
			// 
			this.label7.AutoSize = true;
			this.label7.Location = new System.Drawing.Point(21, 46);
			this.label7.Name = "label7";
			this.label7.Size = new System.Drawing.Size(121, 13);
			this.label7.TabIndex = 6;
			this.label7.Text = "Отправлено килобайт:";
			this.label7.Visible = false;
			// 
			// tbRecvKb
			// 
			this.tbRecvKb.Location = new System.Drawing.Point(155, 17);
			this.tbRecvKb.Name = "tbRecvKb";
			this.tbRecvKb.ReadOnly = true;
			this.tbRecvKb.Size = new System.Drawing.Size(80, 20);
			this.tbRecvKb.TabIndex = 5;
			this.tbRecvKb.Visible = false;
			// 
			// label6
			// 
			this.label6.AutoSize = true;
			this.label6.Location = new System.Drawing.Point(21, 20);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(103, 13);
			this.label6.TabIndex = 4;
			this.label6.Text = "Принято килобайт:";
			this.label6.Visible = false;
			// 
			// tableLayoutPanel2
			// 
			this.tableLayoutPanel2.AutoSize = true;
			this.tableLayoutPanel2.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.tableLayoutPanel2.BackColor = System.Drawing.SystemColors.Control;
			this.tableLayoutPanel2.ColumnCount = 10;
			this.tableLayoutPanel1.SetColumnSpan(this.tableLayoutPanel2, 2);
			this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
			this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
			this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
			this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
			this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
			this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
			this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
			this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
			this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
			this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
			this.tableLayoutPanel2.Controls.Add(this.tbFilterIP, 4, 0);
			this.tableLayoutPanel2.Controls.Add(this.label3, 0, 0);
			this.tableLayoutPanel2.Controls.Add(this.label4, 3, 0);
			this.tableLayoutPanel2.Controls.Add(this.tbFilterUID, 2, 0);
			this.tableLayoutPanel2.Controls.Add(this.label1, 6, 0);
			this.tableLayoutPanel2.Controls.Add(this.label5, 1, 0);
			this.tableLayoutPanel2.Controls.Add(this.tbCountBots, 7, 0);
			this.tableLayoutPanel2.Controls.Add(this.tbElapsedServer, 9, 0);
			this.tableLayoutPanel2.Controls.Add(this.label2, 8, 0);
			this.tableLayoutPanel2.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tableLayoutPanel2.Location = new System.Drawing.Point(0, 0);
			this.tableLayoutPanel2.Margin = new System.Windows.Forms.Padding(0);
			this.tableLayoutPanel2.Name = "tableLayoutPanel2";
			this.tableLayoutPanel2.RowCount = 1;
			this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
			this.tableLayoutPanel2.Size = new System.Drawing.Size(1035, 26);
			this.tableLayoutPanel2.TabIndex = 5;
			// 
			// tbFilterIP
			// 
			this.tbFilterIP.Anchor = System.Windows.Forms.AnchorStyles.Left;
			this.tbFilterIP.Location = new System.Drawing.Point(279, 3);
			this.tbFilterIP.Name = "tbFilterIP";
			this.tbFilterIP.Size = new System.Drawing.Size(100, 20);
			this.tbFilterIP.TabIndex = 3;
			// 
			// label3
			// 
			this.label3.Anchor = System.Windows.Forms.AnchorStyles.Left;
			this.label3.AutoSize = true;
			this.label3.ForeColor = System.Drawing.Color.Blue;
			this.label3.Location = new System.Drawing.Point(3, 6);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(61, 13);
			this.label3.TabIndex = 0;
			this.label3.Text = "Фильтры: ";
			// 
			// label4
			// 
			this.label4.Anchor = System.Windows.Forms.AnchorStyles.Left;
			this.label4.AutoSize = true;
			this.label4.Location = new System.Drawing.Point(253, 6);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(20, 13);
			this.label4.TabIndex = 2;
			this.label4.Text = "IP:";
			// 
			// tbFilterUID
			// 
			this.tbFilterUID.Anchor = System.Windows.Forms.AnchorStyles.Left;
			this.tbFilterUID.Location = new System.Drawing.Point(97, 3);
			this.tbFilterUID.Name = "tbFilterUID";
			this.tbFilterUID.Size = new System.Drawing.Size(150, 20);
			this.tbFilterUID.TabIndex = 1;
			// 
			// label1
			// 
			this.label1.Anchor = System.Windows.Forms.AnchorStyles.Left;
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(635, 6);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(91, 13);
			this.label1.TabIndex = 0;
			this.label1.Text = "Активных ботов:";
			// 
			// label5
			// 
			this.label5.Anchor = System.Windows.Forms.AnchorStyles.Left;
			this.label5.AutoSize = true;
			this.label5.Location = new System.Drawing.Point(70, 6);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(21, 13);
			this.label5.TabIndex = 4;
			this.label5.Text = "ID:";
			// 
			// tbCountBots
			// 
			this.tbCountBots.Anchor = System.Windows.Forms.AnchorStyles.Left;
			this.tbCountBots.Location = new System.Drawing.Point(732, 3);
			this.tbCountBots.Name = "tbCountBots";
			this.tbCountBots.ReadOnly = true;
			this.tbCountBots.Size = new System.Drawing.Size(80, 20);
			this.tbCountBots.TabIndex = 1;
			// 
			// tbElapsedServer
			// 
			this.tbElapsedServer.Anchor = System.Windows.Forms.AnchorStyles.Left;
			this.tbElapsedServer.Location = new System.Drawing.Point(952, 3);
			this.tbElapsedServer.Name = "tbElapsedServer";
			this.tbElapsedServer.ReadOnly = true;
			this.tbElapsedServer.Size = new System.Drawing.Size(80, 20);
			this.tbElapsedServer.TabIndex = 3;
			this.tbElapsedServer.Visible = false;
			// 
			// label2
			// 
			this.label2.Anchor = System.Windows.Forms.AnchorStyles.Left;
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(818, 6);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(128, 13);
			this.label2.TabIndex = 2;
			this.label2.Text = "Время работы сервера:";
			this.label2.Visible = false;
			// 
			// tableLayoutPanel3
			// 
			this.tableLayoutPanel3.AutoSize = true;
			this.tableLayoutPanel3.ColumnCount = 4;
			this.tableLayoutPanel1.SetColumnSpan(this.tableLayoutPanel3, 2);
			this.tableLayoutPanel3.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
			this.tableLayoutPanel3.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
			this.tableLayoutPanel3.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
			this.tableLayoutPanel3.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
			this.tableLayoutPanel3.Controls.Add(this.tbComment, 3, 0);
			this.tableLayoutPanel3.Controls.Add(this.btCmdBot, 0, 0);
			this.tableLayoutPanel3.Controls.Add(this.tbCmdBot, 1, 0);
			this.tableLayoutPanel3.Controls.Add(this.btComment, 2, 0);
			this.tableLayoutPanel3.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tableLayoutPanel3.Location = new System.Drawing.Point(0, 379);
			this.tableLayoutPanel3.Margin = new System.Windows.Forms.Padding(0);
			this.tableLayoutPanel3.Name = "tableLayoutPanel3";
			this.tableLayoutPanel3.RowCount = 1;
			this.tableLayoutPanel3.RowStyles.Add(new System.Windows.Forms.RowStyle());
			this.tableLayoutPanel3.Size = new System.Drawing.Size(1035, 29);
			this.tableLayoutPanel3.TabIndex = 6;
			// 
			// tbComment
			// 
			this.tbComment.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tbComment.Location = new System.Drawing.Point(620, 4);
			this.tbComment.Margin = new System.Windows.Forms.Padding(3, 4, 3, 3);
			this.tbComment.Name = "tbComment";
			this.tbComment.Size = new System.Drawing.Size(412, 20);
			this.tbComment.TabIndex = 21;
			// 
			// btCmdBot
			// 
			this.btCmdBot.Anchor = System.Windows.Forms.AnchorStyles.Left;
			this.btCmdBot.Location = new System.Drawing.Point(3, 3);
			this.btCmdBot.Name = "btCmdBot";
			this.btCmdBot.Size = new System.Drawing.Size(94, 23);
			this.btCmdBot.TabIndex = 16;
			this.btCmdBot.Text = "Команда боту";
			this.btCmdBot.UseVisualStyleBackColor = true;
			this.btCmdBot.Click += new System.EventHandler(this.btCmdBot_Click);
			// 
			// tbCmdBot
			// 
			this.tbCmdBot.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tbCmdBot.Location = new System.Drawing.Point(103, 4);
			this.tbCmdBot.Margin = new System.Windows.Forms.Padding(3, 4, 3, 3);
			this.tbCmdBot.Name = "tbCmdBot";
			this.tbCmdBot.Size = new System.Drawing.Size(411, 20);
			this.tbCmdBot.TabIndex = 17;
			// 
			// btComment
			// 
			this.btComment.Location = new System.Drawing.Point(520, 3);
			this.btComment.Name = "btComment";
			this.btComment.Size = new System.Drawing.Size(94, 23);
			this.btComment.TabIndex = 20;
			this.btComment.Text = "Комментарий";
			this.btComment.UseVisualStyleBackColor = true;
			this.btComment.Click += new System.EventHandler(this.btComment_Click);
			// 
			// timer
			// 
			this.timer.Interval = 5000;
			this.timer.Tick += new System.EventHandler(this.timer_Tick);
			// 
			// FormMain
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(1035, 538);
			this.Controls.Add(this.tableLayoutPanel1);
			this.Name = "FormMain";
			this.ShowIcon = false;
			this.Text = "Бэкконект сервер";
			this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.FormMain_FormClosed);
			this.tableLayoutPanel1.ResumeLayout(false);
			this.tableLayoutPanel1.PerformLayout();
			this.panel1.ResumeLayout(false);
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.panel2.ResumeLayout(false);
			this.panel2.PerformLayout();
			this.tableLayoutPanel2.ResumeLayout(false);
			this.tableLayoutPanel2.PerformLayout();
			this.tableLayoutPanel3.ResumeLayout(false);
			this.tableLayoutPanel3.PerformLayout();
			this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.ListView lvBots;
        private System.Windows.Forms.ColumnHeader clnUid;
        private System.Windows.Forms.ColumnHeader clnIp;
        private System.Windows.Forms.ColumnHeader clnDateBeg;
        private System.Windows.Forms.ColumnHeader clnElapsed;
        private System.Windows.Forms.ColumnHeader clnRecv;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.TextBox tbCountBots;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Timer timer;
        private System.Windows.Forms.TextBox tbElapsedServer;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.ColumnHeader clnPorts;
		private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.TextBox tbFilterUID;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox tbFilterIP;
		private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox tbSendKb;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.TextBox tbRecvKb;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Button btRunRDP;
        private System.Windows.Forms.Button btClosePortForward;
        private System.Windows.Forms.Button btOffBot;
		private System.Windows.Forms.TextBox tbPortForward;
		private System.Windows.Forms.Button btPortForward;
		private System.Windows.Forms.Button btConfig;
		private System.Windows.Forms.TextBox tbCmdBot;
		private System.Windows.Forms.Button btCmdBot;
		private System.Windows.Forms.Button btStartSocks5;
		private System.Windows.Forms.ColumnHeader clnInfo;
		private System.Windows.Forms.ColumnHeader clnComment;
		private System.Windows.Forms.TextBox tbComment;
		private System.Windows.Forms.Button btComment;
		private System.Windows.Forms.TableLayoutPanel tableLayoutPanel2;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.TableLayoutPanel tableLayoutPanel3;
		private System.Windows.Forms.Button btRunVNC;
		private System.Windows.Forms.TextBox tbVNCPort;
		private System.Windows.Forms.Label label8;
		private System.Windows.Forms.Button btResetLogs;
    }
}

