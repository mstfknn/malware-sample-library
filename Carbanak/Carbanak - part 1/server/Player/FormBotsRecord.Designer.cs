namespace Player
{
    partial class FormBotsRecord
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
            this.lbUids = new System.Windows.Forms.ListBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.btOpen = new System.Windows.Forms.Button();
            this.btCancel = new System.Windows.Forms.Button();
            this.lvFiles = new System.Windows.Forms.ListView();
            this.columnHeader4 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader7 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader2 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader5 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader6 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader3 = new System.Windows.Forms.ColumnHeader();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.tbFilterUID = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.tbFilterNameVideo = new System.Windows.Forms.TextBox();
            this.cbCaptions = new System.Windows.Forms.ComboBox();
            this.label6 = new System.Windows.Forms.Label();
            this.cbPaths = new System.Windows.Forms.ComboBox();
            this.label7 = new System.Windows.Forms.Label();
            this.btAddPath = new System.Windows.Forms.Button();
            this.btDelPath = new System.Windows.Forms.Button();
            this.dlgFolder = new System.Windows.Forms.FolderBrowserDialog();
            this.timer = new System.Windows.Forms.Timer(this.components);
            this.progressFtp = new System.Windows.Forms.ProgressBar();
            this.btOpenAnchor = new System.Windows.Forms.Button();
            this.btExportCaptions = new System.Windows.Forms.Button();
            this.dlgSaveFile = new System.Windows.Forms.SaveFileDialog();
            this.btDelVideo = new System.Windows.Forms.Button();
            this.label8 = new System.Windows.Forms.Label();
            this.tbCountBots = new System.Windows.Forms.TextBox();
            this.tbCountVideo = new System.Windows.Forms.TextBox();
            this.label9 = new System.Windows.Forms.Label();
            this.btBugVideo = new System.Windows.Forms.Button();
            this.label10 = new System.Windows.Forms.Label();
            this.tbFilterCaption = new System.Windows.Forms.TextBox();
            this.label11 = new System.Windows.Forms.Label();
            this.tbUnikBots = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // lbUids
            // 
            this.lbUids.FormattingEnabled = true;
            this.lbUids.Location = new System.Drawing.Point(2, 51);
            this.lbUids.Name = "lbUids";
            this.lbUids.Size = new System.Drawing.Size(252, 498);
            this.lbUids.TabIndex = 0;
            this.lbUids.SelectedIndexChanged += new System.EventHandler(this.lbUids_SelectedIndexChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(-1, 35);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(34, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "UIDs:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(257, 35);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(47, 13);
            this.label2.TabIndex = 3;
            this.label2.Text = "Файлы:";
            // 
            // btOpen
            // 
            this.btOpen.Location = new System.Drawing.Point(1014, 554);
            this.btOpen.Name = "btOpen";
            this.btOpen.Size = new System.Drawing.Size(75, 23);
            this.btOpen.TabIndex = 4;
            this.btOpen.Text = "Открыть";
            this.btOpen.UseVisualStyleBackColor = true;
            this.btOpen.Click += new System.EventHandler(this.btOpen_Click);
            // 
            // btCancel
            // 
            this.btCancel.Location = new System.Drawing.Point(1014, 578);
            this.btCancel.Name = "btCancel";
            this.btCancel.Size = new System.Drawing.Size(75, 23);
            this.btCancel.TabIndex = 5;
            this.btCancel.Text = "Отмена";
            this.btCancel.UseVisualStyleBackColor = true;
            this.btCancel.Click += new System.EventHandler(this.btCancel_Click);
            // 
            // lvFiles
            // 
            this.lvFiles.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader4,
            this.columnHeader7,
            this.columnHeader1,
            this.columnHeader2,
            this.columnHeader5,
            this.columnHeader6,
            this.columnHeader3});
            this.lvFiles.FullRowSelect = true;
            this.lvFiles.HideSelection = false;
            this.lvFiles.Location = new System.Drawing.Point(260, 51);
            this.lvFiles.MultiSelect = false;
            this.lvFiles.Name = "lvFiles";
            this.lvFiles.Size = new System.Drawing.Size(829, 498);
            this.lvFiles.TabIndex = 6;
            this.lvFiles.UseCompatibleStateImageBehavior = false;
            this.lvFiles.View = System.Windows.Forms.View.Details;
            this.lvFiles.DrawItem += new System.Windows.Forms.DrawListViewItemEventHandler(this.lvFiles_DrawItem);
            this.lvFiles.DrawSubItem += new System.Windows.Forms.DrawListViewSubItemEventHandler(this.lvFiles_DrawSubItem);
            // 
            // columnHeader4
            // 
            this.columnHeader4.Text = "UID";
            this.columnHeader4.Width = 150;
            // 
            // columnHeader7
            // 
            this.columnHeader7.Text = "IP бота";
            this.columnHeader7.Width = 100;
            // 
            // columnHeader1
            // 
            this.columnHeader1.Text = "Имя файла";
            this.columnHeader1.Width = 140;
            // 
            // columnHeader2
            // 
            this.columnHeader2.Text = "Название видео";
            this.columnHeader2.Width = 140;
            // 
            // columnHeader5
            // 
            this.columnHeader5.Text = "Начало записи";
            this.columnHeader5.Width = 120;
            // 
            // columnHeader6
            // 
            this.columnHeader6.Text = "Конец записи";
            this.columnHeader6.Width = 120;
            // 
            // columnHeader3
            // 
            this.columnHeader3.Text = "Процесс с которого писали";
            this.columnHeader3.Width = 250;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(256, 557);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(50, 13);
            this.label3.TabIndex = 7;
            this.label3.Text = "Фильтр:";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(312, 557);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(29, 13);
            this.label4.TabIndex = 8;
            this.label4.Text = "UID:";
            // 
            // tbFilterUID
            // 
            this.tbFilterUID.Location = new System.Drawing.Point(347, 554);
            this.tbFilterUID.Name = "tbFilterUID";
            this.tbFilterUID.Size = new System.Drawing.Size(79, 20);
            this.tbFilterUID.TabIndex = 9;
            this.tbFilterUID.TextChanged += new System.EventHandler(this.tbFilterUID_TextChanged);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(428, 559);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(93, 13);
            this.label5.TabIndex = 10;
            this.label5.Text = "Название видео:";
            // 
            // tbFilterNameVideo
            // 
            this.tbFilterNameVideo.Location = new System.Drawing.Point(523, 554);
            this.tbFilterNameVideo.Name = "tbFilterNameVideo";
            this.tbFilterNameVideo.Size = new System.Drawing.Size(77, 20);
            this.tbFilterNameVideo.TabIndex = 11;
            this.tbFilterNameVideo.TextChanged += new System.EventHandler(this.tbFilterNameVideo_TextChanged);
            // 
            // cbCaptions
            // 
            this.cbCaptions.FormattingEnabled = true;
            this.cbCaptions.Location = new System.Drawing.Point(347, 580);
            this.cbCaptions.Name = "cbCaptions";
            this.cbCaptions.Size = new System.Drawing.Size(461, 21);
            this.cbCaptions.Sorted = true;
            this.cbCaptions.TabIndex = 12;
            this.cbCaptions.SelectedIndexChanged += new System.EventHandler(this.cbCaptions_SelectedIndexChanged);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(256, 583);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(64, 13);
            this.label6.TabIndex = 13;
            this.label6.Text = "Заголовки:";
            // 
            // cbPaths
            // 
            this.cbPaths.FormattingEnabled = true;
            this.cbPaths.Location = new System.Drawing.Point(81, 6);
            this.cbPaths.Name = "cbPaths";
            this.cbPaths.Size = new System.Drawing.Size(367, 21);
            this.cbPaths.TabIndex = 14;
            this.cbPaths.Validating += new System.ComponentModel.CancelEventHandler(this.cbPaths_Validating);
            this.cbPaths.SelectedIndexChanged += new System.EventHandler(this.cbPaths_SelectedIndexChanged);
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(-1, 9);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(76, 13);
            this.label7.TabIndex = 15;
            this.label7.Text = "Путь к видео:";
            // 
            // btAddPath
            // 
            this.btAddPath.Location = new System.Drawing.Point(454, 4);
            this.btAddPath.Name = "btAddPath";
            this.btAddPath.Size = new System.Drawing.Size(131, 23);
            this.btAddPath.TabIndex = 16;
            this.btAddPath.Text = "Добавить новый путь";
            this.btAddPath.UseVisualStyleBackColor = true;
            this.btAddPath.Click += new System.EventHandler(this.btAddPath_Click);
            // 
            // btDelPath
            // 
            this.btDelPath.Location = new System.Drawing.Point(591, 4);
            this.btDelPath.Name = "btDelPath";
            this.btDelPath.Size = new System.Drawing.Size(129, 23);
            this.btDelPath.TabIndex = 17;
            this.btDelPath.Text = "Удалить текущий путь";
            this.btDelPath.UseVisualStyleBackColor = true;
            this.btDelPath.Click += new System.EventHandler(this.btDelPath_Click);
            // 
            // timer
            // 
            this.timer.Interval = 500;
            this.timer.Tick += new System.EventHandler(this.timer_Tick);
            // 
            // progressFtp
            // 
            this.progressFtp.Location = new System.Drawing.Point(814, 603);
            this.progressFtp.Maximum = 3;
            this.progressFtp.Name = "progressFtp";
            this.progressFtp.Size = new System.Drawing.Size(275, 23);
            this.progressFtp.TabIndex = 18;
            this.progressFtp.Visible = false;
            // 
            // btOpenAnchor
            // 
            this.btOpenAnchor.Location = new System.Drawing.Point(814, 554);
            this.btOpenAnchor.Name = "btOpenAnchor";
            this.btOpenAnchor.Size = new System.Drawing.Size(113, 23);
            this.btOpenAnchor.TabIndex = 19;
            this.btOpenAnchor.Text = "Открыть по якорю";
            this.btOpenAnchor.UseVisualStyleBackColor = true;
            this.btOpenAnchor.Click += new System.EventHandler(this.btOpenAnchor_Click);
            // 
            // btExportCaptions
            // 
            this.btExportCaptions.Location = new System.Drawing.Point(814, 578);
            this.btExportCaptions.Name = "btExportCaptions";
            this.btExportCaptions.Size = new System.Drawing.Size(122, 23);
            this.btExportCaptions.TabIndex = 20;
            this.btExportCaptions.Text = "Экспорт заголовков";
            this.btExportCaptions.UseVisualStyleBackColor = true;
            this.btExportCaptions.Click += new System.EventHandler(this.btExportCaptions_Click);
            // 
            // dlgSaveFile
            // 
            this.dlgSaveFile.DefaultExt = "txt";
            // 
            // btDelVideo
            // 
            this.btDelVideo.Location = new System.Drawing.Point(726, 4);
            this.btDelVideo.Name = "btDelVideo";
            this.btDelVideo.Size = new System.Drawing.Size(101, 23);
            this.btDelVideo.TabIndex = 21;
            this.btDelVideo.Text = "Удаление видео";
            this.btDelVideo.UseVisualStyleBackColor = true;
            this.btDelVideo.Click += new System.EventHandler(this.btDelVideo_Click);
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(2, 556);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(40, 13);
            this.label8.TabIndex = 22;
            this.label8.Text = "Ботов:";
            // 
            // tbCountBots
            // 
            this.tbCountBots.Location = new System.Drawing.Point(47, 551);
            this.tbCountBots.Name = "tbCountBots";
            this.tbCountBots.ReadOnly = true;
            this.tbCountBots.Size = new System.Drawing.Size(64, 20);
            this.tbCountBots.TabIndex = 23;
            // 
            // tbCountVideo
            // 
            this.tbCountVideo.Location = new System.Drawing.Point(479, 604);
            this.tbCountVideo.Name = "tbCountVideo";
            this.tbCountVideo.ReadOnly = true;
            this.tbCountVideo.Size = new System.Drawing.Size(64, 20);
            this.tbCountVideo.TabIndex = 25;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(434, 609);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(41, 13);
            this.label9.TabIndex = 24;
            this.label9.Text = "Видео:";
            // 
            // btBugVideo
            // 
            this.btBugVideo.Location = new System.Drawing.Point(933, 555);
            this.btBugVideo.Name = "btBugVideo";
            this.btBugVideo.Size = new System.Drawing.Size(75, 23);
            this.btBugVideo.TabIndex = 26;
            this.btBugVideo.Text = "Баг видео";
            this.btBugVideo.UseVisualStyleBackColor = true;
            this.btBugVideo.Click += new System.EventHandler(this.btBugVideo_Click);
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(606, 558);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(64, 13);
            this.label10.TabIndex = 27;
            this.label10.Text = "Заголовок:";
            // 
            // tbFilterCaption
            // 
            this.tbFilterCaption.Location = new System.Drawing.Point(676, 554);
            this.tbFilterCaption.Name = "tbFilterCaption";
            this.tbFilterCaption.Size = new System.Drawing.Size(77, 20);
            this.tbFilterCaption.TabIndex = 28;
            this.tbFilterCaption.TextChanged += new System.EventHandler(this.tbFilterCaption_TextChanged);
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(257, 607);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(105, 13);
            this.label11.TabIndex = 29;
            this.label11.Text = "Уникальных ботов:";
            // 
            // tbUnikBots
            // 
            this.tbUnikBots.Location = new System.Drawing.Point(368, 604);
            this.tbUnikBots.Name = "tbUnikBots";
            this.tbUnikBots.ReadOnly = true;
            this.tbUnikBots.Size = new System.Drawing.Size(58, 20);
            this.tbUnikBots.TabIndex = 30;
            // 
            // FormBotsRecord
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1101, 629);
            this.Controls.Add(this.tbUnikBots);
            this.Controls.Add(this.label11);
            this.Controls.Add(this.tbFilterCaption);
            this.Controls.Add(this.label10);
            this.Controls.Add(this.btBugVideo);
            this.Controls.Add(this.tbCountVideo);
            this.Controls.Add(this.label9);
            this.Controls.Add(this.tbCountBots);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.btDelVideo);
            this.Controls.Add(this.btExportCaptions);
            this.Controls.Add(this.btOpenAnchor);
            this.Controls.Add(this.progressFtp);
            this.Controls.Add(this.btDelPath);
            this.Controls.Add(this.btAddPath);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.cbPaths);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.cbCaptions);
            this.Controls.Add(this.tbFilterNameVideo);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.tbFilterUID);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.lvFiles);
            this.Controls.Add(this.btCancel);
            this.Controls.Add(this.btOpen);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.lbUids);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "FormBotsRecord";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Записанное видео";
            this.Load += new System.EventHandler(this.FormBotsRecord_Load);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.FormBotsRecord_FormClosing);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ListBox lbUids;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button btOpen;
        private System.Windows.Forms.Button btCancel;
        private System.Windows.Forms.ListView lvFiles;
        private System.Windows.Forms.ColumnHeader columnHeader1;
        private System.Windows.Forms.ColumnHeader columnHeader2;
        private System.Windows.Forms.ColumnHeader columnHeader3;
        private System.Windows.Forms.ColumnHeader columnHeader4;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox tbFilterUID;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox tbFilterNameVideo;
        private System.Windows.Forms.ColumnHeader columnHeader7;
        private System.Windows.Forms.ColumnHeader columnHeader5;
        private System.Windows.Forms.ColumnHeader columnHeader6;
        private System.Windows.Forms.ComboBox cbCaptions;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.ComboBox cbPaths;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Button btAddPath;
        private System.Windows.Forms.Button btDelPath;
        private System.Windows.Forms.FolderBrowserDialog dlgFolder;
        private System.Windows.Forms.Timer timer;
        private System.Windows.Forms.ProgressBar progressFtp;
        private System.Windows.Forms.Button btOpenAnchor;
        private System.Windows.Forms.Button btExportCaptions;
        private System.Windows.Forms.SaveFileDialog dlgSaveFile;
        private System.Windows.Forms.Button btDelVideo;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.TextBox tbCountBots;
        private System.Windows.Forms.TextBox tbCountVideo;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Button btBugVideo;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.TextBox tbFilterCaption;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.TextBox tbUnikBots;
    }
}