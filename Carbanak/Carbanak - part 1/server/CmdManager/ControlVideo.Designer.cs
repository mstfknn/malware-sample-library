namespace CmdManager
{
	partial class ControlVideo
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
			this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
			this.panel1 = new System.Windows.Forms.Panel();
			this.pbVideo = new System.Windows.Forms.PictureBox();
			this.btStart = new System.Windows.Forms.Button();
			this.btRefresh = new System.Windows.Forms.Button();
			this.btStop = new System.Windows.Forms.Button();
			this.tableLayoutPanel1.SuspendLayout();
			this.panel1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.pbVideo)).BeginInit();
			this.SuspendLayout();
			// 
			// tableLayoutPanel1
			// 
			this.tableLayoutPanel1.ColumnCount = 1;
			this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
			this.tableLayoutPanel1.Controls.Add(this.pbVideo, 0, 0);
			this.tableLayoutPanel1.Controls.Add(this.panel1, 0, 1);
			this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 0);
			this.tableLayoutPanel1.Name = "tableLayoutPanel1";
			this.tableLayoutPanel1.RowCount = 2;
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
			this.tableLayoutPanel1.Size = new System.Drawing.Size(919, 564);
			this.tableLayoutPanel1.TabIndex = 0;
			// 
			// panel1
			// 
			this.panel1.AutoSize = true;
			this.panel1.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.panel1.Controls.Add(this.btStart);
			this.panel1.Controls.Add(this.btRefresh);
			this.panel1.Controls.Add(this.btStop);
			this.panel1.Location = new System.Drawing.Point(3, 520);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(123, 41);
			this.panel1.TabIndex = 1;
			// 
			// pbVideo
			// 
			this.pbVideo.Dock = System.Windows.Forms.DockStyle.Fill;
			this.pbVideo.Location = new System.Drawing.Point(3, 3);
			this.pbVideo.Name = "pbVideo";
			this.pbVideo.Size = new System.Drawing.Size(913, 511);
			this.pbVideo.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
			this.pbVideo.TabIndex = 0;
			this.pbVideo.TabStop = false;
			// 
			// btStart
			// 
			this.btStart.AutoSize = true;
			this.btStart.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.btStart.Image = global::CmdManager.Properties.Resources.start_32;
			this.btStart.Location = new System.Drawing.Point(41, 0);
			this.btStart.Margin = new System.Windows.Forms.Padding(0);
			this.btStart.Name = "btStart";
			this.btStart.Size = new System.Drawing.Size(38, 38);
			this.btStart.TabIndex = 2;
			this.btStart.UseVisualStyleBackColor = true;
			this.btStart.Click += new System.EventHandler(this.btStart_Click);
			// 
			// btRefresh
			// 
			this.btRefresh.AutoSize = true;
			this.btRefresh.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.btRefresh.Image = global::CmdManager.Properties.Resources.refresh_32;
			this.btRefresh.Location = new System.Drawing.Point(82, 0);
			this.btRefresh.Name = "btRefresh";
			this.btRefresh.Size = new System.Drawing.Size(38, 38);
			this.btRefresh.TabIndex = 1;
			this.btRefresh.UseVisualStyleBackColor = true;
			this.btRefresh.Click += new System.EventHandler(this.btRefresh_Click);
			// 
			// btStop
			// 
			this.btStop.AutoSize = true;
			this.btStop.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.btStop.Image = global::CmdManager.Properties.Resources.stop_32;
			this.btStop.Location = new System.Drawing.Point(0, 0);
			this.btStop.Margin = new System.Windows.Forms.Padding(0);
			this.btStop.Name = "btStop";
			this.btStop.Size = new System.Drawing.Size(38, 38);
			this.btStop.TabIndex = 0;
			this.btStop.UseVisualStyleBackColor = true;
			this.btStop.Click += new System.EventHandler(this.btStop_Click);
			// 
			// ControlVideo
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this.tableLayoutPanel1);
			this.Name = "ControlVideo";
			this.Size = new System.Drawing.Size(919, 564);
			this.tableLayoutPanel1.ResumeLayout(false);
			this.tableLayoutPanel1.PerformLayout();
			this.panel1.ResumeLayout(false);
			this.panel1.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.pbVideo)).EndInit();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
		private System.Windows.Forms.PictureBox pbVideo;
		private System.Windows.Forms.Panel panel1;
		private System.Windows.Forms.Button btRefresh;
		private System.Windows.Forms.Button btStop;
		private System.Windows.Forms.Button btStart;
	}
}
