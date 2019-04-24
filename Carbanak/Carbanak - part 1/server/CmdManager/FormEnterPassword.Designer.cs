namespace CmdManager
{
	partial class FormEnterPassword
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
			this.labelEnterPassword = new System.Windows.Forms.Label();
			this.tbPassword = new System.Windows.Forms.TextBox();
			this.btOk = new System.Windows.Forms.Button();
			this.btCancel = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// labelEnterPassword
			// 
			this.labelEnterPassword.AutoSize = true;
			this.labelEnterPassword.Location = new System.Drawing.Point(12, 9);
			this.labelEnterPassword.Name = "labelEnterPassword";
			this.labelEnterPassword.Size = new System.Drawing.Size(112, 13);
			this.labelEnterPassword.TabIndex = 0;
			this.labelEnterPassword.Text = "Введите пароль для:";
			// 
			// tbPassword
			// 
			this.tbPassword.Location = new System.Drawing.Point(15, 25);
			this.tbPassword.Name = "tbPassword";
			this.tbPassword.PasswordChar = '*';
			this.tbPassword.Size = new System.Drawing.Size(240, 20);
			this.tbPassword.TabIndex = 1;
			// 
			// btOk
			// 
			this.btOk.Location = new System.Drawing.Point(58, 64);
			this.btOk.Name = "btOk";
			this.btOk.Size = new System.Drawing.Size(75, 23);
			this.btOk.TabIndex = 2;
			this.btOk.Text = "Ок";
			this.btOk.UseVisualStyleBackColor = true;
			this.btOk.Click += new System.EventHandler(this.btOk_Click);
			// 
			// btCancel
			// 
			this.btCancel.Location = new System.Drawing.Point(139, 64);
			this.btCancel.Name = "btCancel";
			this.btCancel.Size = new System.Drawing.Size(75, 23);
			this.btCancel.TabIndex = 3;
			this.btCancel.Text = "Отмена";
			this.btCancel.UseVisualStyleBackColor = true;
			this.btCancel.Click += new System.EventHandler(this.btCancel_Click);
			// 
			// FormEnterPassword
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(273, 99);
			this.Controls.Add(this.btCancel);
			this.Controls.Add(this.btOk);
			this.Controls.Add(this.tbPassword);
			this.Controls.Add(this.labelEnterPassword);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "FormEnterPassword";
			this.ShowIcon = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "Ввод пароля";
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Label labelEnterPassword;
		private System.Windows.Forms.TextBox tbPassword;
		private System.Windows.Forms.Button btOk;
		private System.Windows.Forms.Button btCancel;
	}
}