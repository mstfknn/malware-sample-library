namespace Server
{
	partial class FormConfig
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
			this.btOk = new System.Windows.Forms.Button();
			this.btCancel = new System.Windows.Forms.Button();
			this.label1 = new System.Windows.Forms.Label();
			this.tbPort = new System.Windows.Forms.TextBox();
			this.label2 = new System.Windows.Forms.Label();
			this.tbPortClient = new System.Windows.Forms.TextBox();
			this.gridIPClients = new System.Windows.Forms.DataGridView();
			this.Column1 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.label3 = new System.Windows.Forms.Label();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.tbCheckPassword = new System.Windows.Forms.TextBox();
			this.tbNewPassword = new System.Windows.Forms.TextBox();
			this.tbPassword = new System.Windows.Forms.TextBox();
			this.label6 = new System.Windows.Forms.Label();
			this.label5 = new System.Windows.Forms.Label();
			this.label4 = new System.Windows.Forms.Label();
			this.btChangePsw = new System.Windows.Forms.Button();
			((System.ComponentModel.ISupportInitialize)(this.gridIPClients)).BeginInit();
			this.groupBox1.SuspendLayout();
			this.SuspendLayout();
			// 
			// btOk
			// 
			this.btOk.Location = new System.Drawing.Point(391, 6);
			this.btOk.Name = "btOk";
			this.btOk.Size = new System.Drawing.Size(75, 23);
			this.btOk.TabIndex = 0;
			this.btOk.Text = "Ок";
			this.btOk.UseVisualStyleBackColor = true;
			this.btOk.Click += new System.EventHandler(this.btOk_Click);
			// 
			// btCancel
			// 
			this.btCancel.Location = new System.Drawing.Point(391, 35);
			this.btCancel.Name = "btCancel";
			this.btCancel.Size = new System.Drawing.Size(75, 23);
			this.btCancel.TabIndex = 1;
			this.btCancel.Text = "Отмена";
			this.btCancel.UseVisualStyleBackColor = true;
			this.btCancel.Click += new System.EventHandler(this.btCancel_Click);
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(12, 9);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(88, 13);
			this.label1.TabIndex = 2;
			this.label1.Text = "Порт для ботов:";
			// 
			// tbPort
			// 
			this.tbPort.Location = new System.Drawing.Point(119, 6);
			this.tbPort.Name = "tbPort";
			this.tbPort.Size = new System.Drawing.Size(59, 20);
			this.tbPort.TabIndex = 3;
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(13, 28);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(35, 13);
			this.label2.TabIndex = 4;
			this.label2.Text = "Порт:";
			// 
			// tbPortClient
			// 
			this.tbPortClient.Location = new System.Drawing.Point(119, 25);
			this.tbPortClient.Name = "tbPortClient";
			this.tbPortClient.Size = new System.Drawing.Size(60, 20);
			this.tbPortClient.TabIndex = 5;
			// 
			// gridIPClients
			// 
			this.gridIPClients.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.gridIPClients.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.gridIPClients.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.Column1});
			this.gridIPClients.Location = new System.Drawing.Point(16, 76);
			this.gridIPClients.Name = "gridIPClients";
			this.gridIPClients.Size = new System.Drawing.Size(240, 150);
			this.gridIPClients.TabIndex = 6;
			// 
			// Column1
			// 
			this.Column1.DataPropertyName = "ip";
			this.Column1.HeaderText = "IP";
			this.Column1.Name = "Column1";
			this.Column1.Width = 150;
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(13, 60);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(127, 13);
			this.label3.TabIndex = 7;
			this.label3.Text = "Разрешенные клиенты:";
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.btChangePsw);
			this.groupBox1.Controls.Add(this.tbCheckPassword);
			this.groupBox1.Controls.Add(this.tbNewPassword);
			this.groupBox1.Controls.Add(this.tbPassword);
			this.groupBox1.Controls.Add(this.label6);
			this.groupBox1.Controls.Add(this.label5);
			this.groupBox1.Controls.Add(this.label4);
			this.groupBox1.Controls.Add(this.label3);
			this.groupBox1.Controls.Add(this.gridIPClients);
			this.groupBox1.Controls.Add(this.tbPortClient);
			this.groupBox1.Controls.Add(this.label2);
			this.groupBox1.Location = new System.Drawing.Point(12, 64);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(454, 240);
			this.groupBox1.TabIndex = 8;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Клиенты (CmdManager):";
			// 
			// tbCheckPassword
			// 
			this.tbCheckPassword.Location = new System.Drawing.Point(348, 126);
			this.tbCheckPassword.Name = "tbCheckPassword";
			this.tbCheckPassword.PasswordChar = '*';
			this.tbCheckPassword.Size = new System.Drawing.Size(100, 20);
			this.tbCheckPassword.TabIndex = 13;
			// 
			// tbNewPassword
			// 
			this.tbNewPassword.Location = new System.Drawing.Point(348, 100);
			this.tbNewPassword.Name = "tbNewPassword";
			this.tbNewPassword.PasswordChar = '*';
			this.tbNewPassword.Size = new System.Drawing.Size(100, 20);
			this.tbNewPassword.TabIndex = 12;
			// 
			// tbPassword
			// 
			this.tbPassword.Location = new System.Drawing.Point(348, 73);
			this.tbPassword.Name = "tbPassword";
			this.tbPassword.PasswordChar = '*';
			this.tbPassword.Size = new System.Drawing.Size(100, 20);
			this.tbPassword.TabIndex = 11;
			// 
			// label6
			// 
			this.label6.Location = new System.Drawing.Point(262, 121);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(89, 30);
			this.label6.TabIndex = 10;
			this.label6.Text = "Подтверждение пароля:";
			// 
			// label5
			// 
			this.label5.AutoSize = true;
			this.label5.Location = new System.Drawing.Point(262, 103);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(83, 13);
			this.label5.TabIndex = 9;
			this.label5.Text = "Новый пароль:";
			// 
			// label4
			// 
			this.label4.AutoSize = true;
			this.label4.Location = new System.Drawing.Point(262, 76);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(48, 13);
			this.label4.TabIndex = 8;
			this.label4.Text = "Пароль:";
			// 
			// btChangePsw
			// 
			this.btChangePsw.Location = new System.Drawing.Point(305, 154);
			this.btChangePsw.Name = "btChangePsw";
			this.btChangePsw.Size = new System.Drawing.Size(109, 23);
			this.btChangePsw.TabIndex = 14;
			this.btChangePsw.Text = "Сменить пароль";
			this.btChangePsw.UseVisualStyleBackColor = true;
			this.btChangePsw.Click += new System.EventHandler(this.btChangePsw_Click);
			// 
			// FormConfig
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(474, 315);
			this.Controls.Add(this.groupBox1);
			this.Controls.Add(this.tbPort);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.btCancel);
			this.Controls.Add(this.btOk);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "FormConfig";
			this.ShowIcon = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "Настройки сервера";
			this.TopMost = true;
			((System.ComponentModel.ISupportInitialize)(this.gridIPClients)).EndInit();
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Button btOk;
		private System.Windows.Forms.Button btCancel;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.TextBox tbPort;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.TextBox tbPortClient;
		private System.Windows.Forms.DataGridView gridIPClients;
		private System.Windows.Forms.DataGridViewTextBoxColumn Column1;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.TextBox tbCheckPassword;
		private System.Windows.Forms.TextBox tbNewPassword;
		private System.Windows.Forms.TextBox tbPassword;
		private System.Windows.Forms.Label label6;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.Button btChangePsw;
	}
}