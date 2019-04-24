namespace CmdManager
{
	partial class ControlFileTransfer
	{
		/// <summary> 
		/// Требуется переменная конструктора.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary> 
		/// Освободить все используемые ресурсы.
		/// </summary>
		/// <param name="disposing">истинно, если управляемый ресурс должен быть удален; иначе ложно.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Код, автоматически созданный конструктором компонентов

		/// <summary> 
		/// Обязательный метод для поддержки конструктора - не изменяйте 
		/// содержимое данного метода при помощи редактора кода.
		/// </summary>
		private void InitializeComponent()
		{
			this.dlgOpenFile = new System.Windows.Forms.OpenFileDialog();
			this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
			this.lvFilesBot = new System.Windows.Forms.ListView();
			this.clnFileName = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.clnDate = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.clnSize = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.tbBotFolder = new System.Windows.Forms.TextBox();
			this.lvFilesMy = new System.Windows.Forms.ListView();
			this.columnHeader2 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.columnHeader3 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.columnHeader4 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.tbMyFolder = new System.Windows.Forms.TextBox();
			this.toolStrip1 = new System.Windows.Forms.ToolStrip();
			this.btFolderBotUp = new System.Windows.Forms.ToolStripButton();
			this.btBotFolderUpdate = new System.Windows.Forms.ToolStripButton();
			this.btCreateBotFolder = new System.Windows.Forms.ToolStripButton();
			this.btDelBotFile = new System.Windows.Forms.ToolStripButton();
			this.toolStrip2 = new System.Windows.Forms.ToolStrip();
			this.btFolderMyUp = new System.Windows.Forms.ToolStripButton();
			this.btMyFolderUpdate = new System.Windows.Forms.ToolStripButton();
			this.btCreateMyFolder = new System.Windows.Forms.ToolStripButton();
			this.btDelMyFile = new System.Windows.Forms.ToolStripButton();
			this.toolStrip3 = new System.Windows.Forms.ToolStrip();
			this.btCopyMyBot = new System.Windows.Forms.ToolStripButton();
			this.btCopyBotMy = new System.Windows.Forms.ToolStripButton();
			this.btStopCopy = new System.Windows.Forms.ToolStripButton();
			this.tbMyFileDetails = new System.Windows.Forms.TextBox();
			this.tbBotFileDetails = new System.Windows.Forms.TextBox();
			this.labelInfoCopy = new System.Windows.Forms.Label();
			this.progressCopy = new System.Windows.Forms.ProgressBar();
			this.tbHistory = new System.Windows.Forms.TextBox();
			this.tableLayoutPanel1.SuspendLayout();
			this.toolStrip1.SuspendLayout();
			this.toolStrip2.SuspendLayout();
			this.toolStrip3.SuspendLayout();
			this.SuspendLayout();
			// 
			// tableLayoutPanel1
			// 
			this.tableLayoutPanel1.ColumnCount = 3;
			this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
			this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 48F));
			this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
			this.tableLayoutPanel1.Controls.Add(this.lvFilesBot, 0, 2);
			this.tableLayoutPanel1.Controls.Add(this.tbBotFolder, 0, 1);
			this.tableLayoutPanel1.Controls.Add(this.lvFilesMy, 2, 2);
			this.tableLayoutPanel1.Controls.Add(this.tbMyFolder, 2, 1);
			this.tableLayoutPanel1.Controls.Add(this.toolStrip1, 0, 0);
			this.tableLayoutPanel1.Controls.Add(this.toolStrip2, 2, 0);
			this.tableLayoutPanel1.Controls.Add(this.toolStrip3, 1, 2);
			this.tableLayoutPanel1.Controls.Add(this.tbMyFileDetails, 2, 3);
			this.tableLayoutPanel1.Controls.Add(this.tbBotFileDetails, 0, 3);
			this.tableLayoutPanel1.Controls.Add(this.labelInfoCopy, 0, 4);
			this.tableLayoutPanel1.Controls.Add(this.progressCopy, 0, 5);
			this.tableLayoutPanel1.Controls.Add(this.tbHistory, 0, 6);
			this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 0);
			this.tableLayoutPanel1.Name = "tableLayoutPanel1";
			this.tableLayoutPanel1.RowCount = 7;
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
			this.tableLayoutPanel1.Size = new System.Drawing.Size(802, 470);
			this.tableLayoutPanel1.TabIndex = 6;
			// 
			// lvFilesBot
			// 
			this.lvFilesBot.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.clnFileName,
            this.clnDate,
            this.clnSize});
			this.lvFilesBot.Dock = System.Windows.Forms.DockStyle.Fill;
			this.lvFilesBot.FullRowSelect = true;
			this.lvFilesBot.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
			this.lvFilesBot.HideSelection = false;
			this.lvFilesBot.Location = new System.Drawing.Point(3, 68);
			this.lvFilesBot.Name = "lvFilesBot";
			this.lvFilesBot.Size = new System.Drawing.Size(371, 277);
			this.lvFilesBot.TabIndex = 0;
			this.lvFilesBot.UseCompatibleStateImageBehavior = false;
			this.lvFilesBot.View = System.Windows.Forms.View.Details;
			this.lvFilesBot.SelectedIndexChanged += new System.EventHandler(this.lvFilesBot_SelectedIndexChanged);
			this.lvFilesBot.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.lvFilesBot_MouseDoubleClick);
			// 
			// clnFileName
			// 
			this.clnFileName.Text = "Имя файла";
			this.clnFileName.Width = 200;
			// 
			// clnDate
			// 
			this.clnDate.Text = "Дата";
			this.clnDate.Width = 100;
			// 
			// clnSize
			// 
			this.clnSize.Text = "Размер";
			this.clnSize.Width = 120;
			// 
			// tbBotFolder
			// 
			this.tbBotFolder.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tbBotFolder.Location = new System.Drawing.Point(3, 42);
			this.tbBotFolder.Name = "tbBotFolder";
			this.tbBotFolder.Size = new System.Drawing.Size(371, 20);
			this.tbBotFolder.TabIndex = 1;
			// 
			// lvFilesMy
			// 
			this.lvFilesMy.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader2,
            this.columnHeader3,
            this.columnHeader4});
			this.lvFilesMy.Dock = System.Windows.Forms.DockStyle.Fill;
			this.lvFilesMy.FullRowSelect = true;
			this.lvFilesMy.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
			this.lvFilesMy.HideSelection = false;
			this.lvFilesMy.Location = new System.Drawing.Point(428, 68);
			this.lvFilesMy.Name = "lvFilesMy";
			this.lvFilesMy.Size = new System.Drawing.Size(371, 277);
			this.lvFilesMy.TabIndex = 2;
			this.lvFilesMy.UseCompatibleStateImageBehavior = false;
			this.lvFilesMy.View = System.Windows.Forms.View.Details;
			this.lvFilesMy.SelectedIndexChanged += new System.EventHandler(this.lvFilesMy_SelectedIndexChanged);
			this.lvFilesMy.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.lvFilesMy_MouseDoubleClick);
			// 
			// columnHeader2
			// 
			this.columnHeader2.Text = "Имя файла";
			this.columnHeader2.Width = 200;
			// 
			// columnHeader3
			// 
			this.columnHeader3.Text = "Дата";
			this.columnHeader3.Width = 100;
			// 
			// columnHeader4
			// 
			this.columnHeader4.Text = "Размер";
			this.columnHeader4.Width = 120;
			// 
			// tbMyFolder
			// 
			this.tbMyFolder.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tbMyFolder.Location = new System.Drawing.Point(428, 42);
			this.tbMyFolder.Name = "tbMyFolder";
			this.tbMyFolder.Size = new System.Drawing.Size(371, 20);
			this.tbMyFolder.TabIndex = 3;
			// 
			// toolStrip1
			// 
			this.toolStrip1.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
			this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.btFolderBotUp,
            this.btBotFolderUpdate,
            this.btCreateBotFolder,
            this.btDelBotFile});
			this.toolStrip1.Location = new System.Drawing.Point(0, 0);
			this.toolStrip1.Name = "toolStrip1";
			this.toolStrip1.Size = new System.Drawing.Size(377, 39);
			this.toolStrip1.TabIndex = 4;
			this.toolStrip1.Text = "toolStrip1";
			// 
			// btFolderBotUp
			// 
			this.btFolderBotUp.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.btFolderBotUp.Image = global::CmdManager.Properties.Resources.folderUp;
			this.btFolderBotUp.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
			this.btFolderBotUp.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btFolderBotUp.Name = "btFolderBotUp";
			this.btFolderBotUp.Size = new System.Drawing.Size(28, 36);
			this.btFolderBotUp.Text = "Выйти из папки";
			this.btFolderBotUp.Click += new System.EventHandler(this.btFolderBotUp_Click);
			// 
			// btBotFolderUpdate
			// 
			this.btBotFolderUpdate.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.btBotFolderUpdate.Image = global::CmdManager.Properties.Resources.adept_update_4772;
			this.btBotFolderUpdate.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
			this.btBotFolderUpdate.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btBotFolderUpdate.Name = "btBotFolderUpdate";
			this.btBotFolderUpdate.Size = new System.Drawing.Size(36, 36);
			this.btBotFolderUpdate.Text = "Обновить";
			this.btBotFolderUpdate.Click += new System.EventHandler(this.btBotFolderUpdate_Click);
			// 
			// btCreateBotFolder
			// 
			this.btCreateBotFolder.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.btCreateBotFolder.Image = global::CmdManager.Properties.Resources.folder_add_4851;
			this.btCreateBotFolder.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
			this.btCreateBotFolder.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btCreateBotFolder.Name = "btCreateBotFolder";
			this.btCreateBotFolder.Size = new System.Drawing.Size(36, 36);
			this.btCreateBotFolder.Text = "Создать папку";
			this.btCreateBotFolder.Click += new System.EventHandler(this.btCreateBotFolder_Click);
			// 
			// btDelBotFile
			// 
			this.btDelBotFile.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.btDelBotFile.Image = global::CmdManager.Properties.Resources.document_delete3;
			this.btDelBotFile.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
			this.btDelBotFile.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btDelBotFile.Name = "btDelBotFile";
			this.btDelBotFile.Size = new System.Drawing.Size(36, 36);
			this.btDelBotFile.Text = "Удалить файл или папку";
			this.btDelBotFile.Click += new System.EventHandler(this.btDelBotFile_Click);
			// 
			// toolStrip2
			// 
			this.toolStrip2.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
			this.toolStrip2.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.btFolderMyUp,
            this.btMyFolderUpdate,
            this.btCreateMyFolder,
            this.btDelMyFile});
			this.toolStrip2.Location = new System.Drawing.Point(425, 0);
			this.toolStrip2.Name = "toolStrip2";
			this.toolStrip2.Size = new System.Drawing.Size(377, 39);
			this.toolStrip2.TabIndex = 5;
			this.toolStrip2.Text = "toolStrip2";
			// 
			// btFolderMyUp
			// 
			this.btFolderMyUp.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.btFolderMyUp.Image = global::CmdManager.Properties.Resources.folderUp;
			this.btFolderMyUp.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
			this.btFolderMyUp.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btFolderMyUp.Name = "btFolderMyUp";
			this.btFolderMyUp.Size = new System.Drawing.Size(28, 36);
			this.btFolderMyUp.Text = "Выйти из папки";
			this.btFolderMyUp.Click += new System.EventHandler(this.btFolderMyUp_Click);
			// 
			// btMyFolderUpdate
			// 
			this.btMyFolderUpdate.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.btMyFolderUpdate.Image = global::CmdManager.Properties.Resources.adept_update_4772;
			this.btMyFolderUpdate.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
			this.btMyFolderUpdate.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btMyFolderUpdate.Name = "btMyFolderUpdate";
			this.btMyFolderUpdate.Size = new System.Drawing.Size(36, 36);
			this.btMyFolderUpdate.Text = "Обновить";
			this.btMyFolderUpdate.Click += new System.EventHandler(this.btMyFolderUpdate_Click);
			// 
			// btCreateMyFolder
			// 
			this.btCreateMyFolder.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.btCreateMyFolder.Image = global::CmdManager.Properties.Resources.folder_add_4851;
			this.btCreateMyFolder.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
			this.btCreateMyFolder.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btCreateMyFolder.Name = "btCreateMyFolder";
			this.btCreateMyFolder.Size = new System.Drawing.Size(36, 36);
			this.btCreateMyFolder.Text = "Создать папку";
			this.btCreateMyFolder.Click += new System.EventHandler(this.btCreateMyFolder_Click);
			// 
			// btDelMyFile
			// 
			this.btDelMyFile.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.btDelMyFile.Image = global::CmdManager.Properties.Resources.document_delete3;
			this.btDelMyFile.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
			this.btDelMyFile.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btDelMyFile.Name = "btDelMyFile";
			this.btDelMyFile.Size = new System.Drawing.Size(36, 36);
			this.btDelMyFile.Text = "Удалить файл или папку";
			this.btDelMyFile.Click += new System.EventHandler(this.btDelMyFile_Click);
			// 
			// toolStrip3
			// 
			this.toolStrip3.Anchor = System.Windows.Forms.AnchorStyles.None;
			this.toolStrip3.Dock = System.Windows.Forms.DockStyle.None;
			this.toolStrip3.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
			this.toolStrip3.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.btCopyMyBot,
            this.btCopyBotMy,
            this.btStopCopy});
			this.toolStrip3.LayoutStyle = System.Windows.Forms.ToolStripLayoutStyle.VerticalStackWithOverflow;
			this.toolStrip3.Location = new System.Drawing.Point(379, 155);
			this.toolStrip3.Name = "toolStrip3";
			this.toolStrip3.Padding = new System.Windows.Forms.Padding(4, 0, 4, 0);
			this.toolStrip3.Size = new System.Drawing.Size(44, 103);
			this.toolStrip3.TabIndex = 6;
			this.toolStrip3.Text = "toolStrip3";
			// 
			// btCopyMyBot
			// 
			this.btCopyMyBot.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.btCopyMyBot.Image = global::CmdManager.Properties.Resources.left_arrow_3191;
			this.btCopyMyBot.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
			this.btCopyMyBot.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btCopyMyBot.Name = "btCopyMyBot";
			this.btCopyMyBot.Padding = new System.Windows.Forms.Padding(0, 0, 4, 0);
			this.btCopyMyBot.Size = new System.Drawing.Size(35, 28);
			this.btCopyMyBot.Text = "Копирование файла на бот";
			this.btCopyMyBot.Click += new System.EventHandler(this.btCopyMyBot_Click);
			// 
			// btCopyBotMy
			// 
			this.btCopyBotMy.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.btCopyBotMy.Image = global::CmdManager.Properties.Resources.right_arrow_3040;
			this.btCopyBotMy.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
			this.btCopyBotMy.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btCopyBotMy.Name = "btCopyBotMy";
			this.btCopyBotMy.Padding = new System.Windows.Forms.Padding(0, 0, 4, 0);
			this.btCopyBotMy.Size = new System.Drawing.Size(35, 28);
			this.btCopyBotMy.Text = "Копирование файла с бота";
			this.btCopyBotMy.Click += new System.EventHandler(this.btCopyBotMy_Click);
			// 
			// btStopCopy
			// 
			this.btStopCopy.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
			this.btStopCopy.Image = global::CmdManager.Properties.Resources.stop_6497;
			this.btStopCopy.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
			this.btStopCopy.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.btStopCopy.Name = "btStopCopy";
			this.btStopCopy.Size = new System.Drawing.Size(35, 36);
			this.btStopCopy.Text = "Остановить операции копирования";
			this.btStopCopy.Click += new System.EventHandler(this.btStopCopy_Click);
			// 
			// tbMyFileDetails
			// 
			this.tbMyFileDetails.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tbMyFileDetails.Location = new System.Drawing.Point(428, 351);
			this.tbMyFileDetails.Name = "tbMyFileDetails";
			this.tbMyFileDetails.ReadOnly = true;
			this.tbMyFileDetails.Size = new System.Drawing.Size(371, 20);
			this.tbMyFileDetails.TabIndex = 7;
			// 
			// tbBotFileDetails
			// 
			this.tbBotFileDetails.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tbBotFileDetails.Location = new System.Drawing.Point(3, 351);
			this.tbBotFileDetails.Name = "tbBotFileDetails";
			this.tbBotFileDetails.ReadOnly = true;
			this.tbBotFileDetails.Size = new System.Drawing.Size(371, 20);
			this.tbBotFileDetails.TabIndex = 8;
			// 
			// labelInfoCopy
			// 
			this.labelInfoCopy.AutoSize = true;
			this.tableLayoutPanel1.SetColumnSpan(this.labelInfoCopy, 3);
			this.labelInfoCopy.Dock = System.Windows.Forms.DockStyle.Fill;
			this.labelInfoCopy.Location = new System.Drawing.Point(3, 374);
			this.labelInfoCopy.Name = "labelInfoCopy";
			this.labelInfoCopy.Size = new System.Drawing.Size(796, 13);
			this.labelInfoCopy.TabIndex = 9;
			// 
			// progressCopy
			// 
			this.tableLayoutPanel1.SetColumnSpan(this.progressCopy, 3);
			this.progressCopy.Dock = System.Windows.Forms.DockStyle.Fill;
			this.progressCopy.Location = new System.Drawing.Point(3, 390);
			this.progressCopy.Maximum = 1000;
			this.progressCopy.Name = "progressCopy";
			this.progressCopy.Size = new System.Drawing.Size(796, 23);
			this.progressCopy.TabIndex = 10;
			// 
			// tbHistory
			// 
			this.tableLayoutPanel1.SetColumnSpan(this.tbHistory, 3);
			this.tbHistory.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tbHistory.Location = new System.Drawing.Point(3, 419);
			this.tbHistory.MinimumSize = new System.Drawing.Size(4, 48);
			this.tbHistory.Multiline = true;
			this.tbHistory.Name = "tbHistory";
			this.tbHistory.ReadOnly = true;
			this.tbHistory.Size = new System.Drawing.Size(796, 48);
			this.tbHistory.TabIndex = 11;
			// 
			// ControlFileTransfer
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this.tableLayoutPanel1);
			this.Name = "ControlFileTransfer";
			this.Size = new System.Drawing.Size(802, 470);
			this.Load += new System.EventHandler(this.ControlFileTransfer_Load);
			this.tableLayoutPanel1.ResumeLayout(false);
			this.tableLayoutPanel1.PerformLayout();
			this.toolStrip1.ResumeLayout(false);
			this.toolStrip1.PerformLayout();
			this.toolStrip2.ResumeLayout(false);
			this.toolStrip2.PerformLayout();
			this.toolStrip3.ResumeLayout(false);
			this.toolStrip3.PerformLayout();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.OpenFileDialog dlgOpenFile;
		private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
		private System.Windows.Forms.ListView lvFilesBot;
		private System.Windows.Forms.ColumnHeader clnFileName;
		private System.Windows.Forms.ColumnHeader clnDate;
		private System.Windows.Forms.ColumnHeader clnSize;
		private System.Windows.Forms.TextBox tbBotFolder;
		private System.Windows.Forms.ListView lvFilesMy;
		private System.Windows.Forms.ColumnHeader columnHeader2;
		private System.Windows.Forms.ColumnHeader columnHeader3;
		private System.Windows.Forms.ColumnHeader columnHeader4;
		private System.Windows.Forms.TextBox tbMyFolder;
		private System.Windows.Forms.ToolStrip toolStrip1;
		private System.Windows.Forms.ToolStripButton btFolderBotUp;
		private System.Windows.Forms.ToolStrip toolStrip2;
		private System.Windows.Forms.ToolStripButton btFolderMyUp;
		private System.Windows.Forms.ToolStrip toolStrip3;
		private System.Windows.Forms.ToolStripButton btCopyMyBot;
		private System.Windows.Forms.ToolStripButton btCopyBotMy;
		private System.Windows.Forms.TextBox tbMyFileDetails;
		private System.Windows.Forms.TextBox tbBotFileDetails;
		private System.Windows.Forms.Label labelInfoCopy;
		private System.Windows.Forms.ProgressBar progressCopy;
		private System.Windows.Forms.TextBox tbHistory;
		private System.Windows.Forms.ToolStripButton btBotFolderUpdate;
		private System.Windows.Forms.ToolStripButton btMyFolderUpdate;
		private System.Windows.Forms.ToolStripButton btStopCopy;
		private System.Windows.Forms.ToolStripButton btCreateBotFolder;
		private System.Windows.Forms.ToolStripButton btDelBotFile;
		private System.Windows.Forms.ToolStripButton btCreateMyFolder;
		private System.Windows.Forms.ToolStripButton btDelMyFile;
	}
}
