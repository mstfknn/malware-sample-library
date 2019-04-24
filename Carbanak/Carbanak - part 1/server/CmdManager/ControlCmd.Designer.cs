namespace CmdManager
{
	partial class ControlCmd
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
			this.btStop = new System.Windows.Forms.Button();
			this.btStart = new System.Windows.Forms.Button();
			this.cmdPrompt = new CmdManager.CommandPrompt();
			this.tableLayoutPanel1.SuspendLayout();
			this.panel1.SuspendLayout();
			this.SuspendLayout();
			// 
			// tableLayoutPanel1
			// 
			this.tableLayoutPanel1.ColumnCount = 1;
			this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
			this.tableLayoutPanel1.Controls.Add(this.cmdPrompt, 0, 0);
			this.tableLayoutPanel1.Controls.Add(this.panel1, 0, 1);
			this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 0);
			this.tableLayoutPanel1.Name = "tableLayoutPanel1";
			this.tableLayoutPanel1.RowCount = 2;
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
			this.tableLayoutPanel1.Size = new System.Drawing.Size(763, 513);
			this.tableLayoutPanel1.TabIndex = 0;
			// 
			// panel1
			// 
			this.panel1.AutoSize = true;
			this.panel1.Controls.Add(this.btStop);
			this.panel1.Controls.Add(this.btStart);
			this.panel1.Location = new System.Drawing.Point(3, 477);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(162, 33);
			this.panel1.TabIndex = 3;
			// 
			// btStop
			// 
			this.btStop.Location = new System.Drawing.Point(84, 7);
			this.btStop.Name = "btStop";
			this.btStop.Size = new System.Drawing.Size(75, 23);
			this.btStop.TabIndex = 1;
			this.btStop.Text = "Остановить";
			this.btStop.UseVisualStyleBackColor = true;
			this.btStop.Click += new System.EventHandler(this.btStop_Click);
			// 
			// btStart
			// 
			this.btStart.Location = new System.Drawing.Point(3, 7);
			this.btStart.Name = "btStart";
			this.btStart.Size = new System.Drawing.Size(75, 23);
			this.btStart.TabIndex = 0;
			this.btStart.Text = "Запустить";
			this.btStart.UseVisualStyleBackColor = true;
			this.btStart.Click += new System.EventHandler(this.btStart_Click);
			// 
			// cmdPrompt
			// 
			this.cmdPrompt.BackColor = System.Drawing.Color.White;
			this.cmdPrompt.Delimiters = new char[] {
        ' '};
			this.cmdPrompt.Dock = System.Windows.Forms.DockStyle.Fill;
			this.cmdPrompt.ForeColor = System.Drawing.Color.Black;
			this.cmdPrompt.Location = new System.Drawing.Point(0, 0);
			this.cmdPrompt.Margin = new System.Windows.Forms.Padding(0);
			this.cmdPrompt.MessageColor = System.Drawing.Color.Black;
			this.cmdPrompt.MinimumSize = new System.Drawing.Size(0, 17);
			this.cmdPrompt.Name = "cmdPrompt";
			this.cmdPrompt.PromptColor = System.Drawing.Color.Black;
			this.cmdPrompt.PromptString = "";
			this.cmdPrompt.Size = new System.Drawing.Size(763, 485);
			this.cmdPrompt.TabIndex = 2;
			this.cmdPrompt.Command += new CmdManager.CommandPrompt.CommandEventHandler(this.cmdPrompt_Command);
			// 
			// ControlCmd
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this.tableLayoutPanel1);
			this.Name = "ControlCmd";
			this.Size = new System.Drawing.Size(763, 513);
			this.tableLayoutPanel1.ResumeLayout(false);
			this.tableLayoutPanel1.PerformLayout();
			this.panel1.ResumeLayout(false);
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
		private CommandPrompt cmdPrompt;
		private System.Windows.Forms.Panel panel1;
		private System.Windows.Forms.Button btStart;
		private System.Windows.Forms.Button btStop;
	}
}
