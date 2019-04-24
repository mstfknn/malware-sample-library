namespace RDPPlayer
{
    partial class FormConnectRDP
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
            this.label1 = new System.Windows.Forms.Label();
            this.cbServer = new System.Windows.Forms.ComboBox();
            this.label2 = new System.Windows.Forms.Label();
            this.tbClient = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.tbUser = new System.Windows.Forms.TextBox();
            this.tbPsw = new System.Windows.Forms.TextBox();
            this.btConnect = new System.Windows.Forms.Button();
            this.btCancel = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(47, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Сервер:";
            // 
            // cbServer
            // 
            this.cbServer.FormattingEnabled = true;
            this.cbServer.Location = new System.Drawing.Point(65, 6);
            this.cbServer.Name = "cbServer";
            this.cbServer.Size = new System.Drawing.Size(184, 21);
            this.cbServer.TabIndex = 1;
            this.cbServer.Validating += new System.ComponentModel.CancelEventHandler(this.cbServer_Validating);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 37);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(46, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "Клиент:";
            // 
            // tbClient
            // 
            this.tbClient.Location = new System.Drawing.Point(64, 34);
            this.tbClient.Name = "tbClient";
            this.tbClient.Size = new System.Drawing.Size(185, 20);
            this.tbClient.TabIndex = 3;
            this.tbClient.Validated += new System.EventHandler(this.tbClient_Validated);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(12, 72);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(83, 13);
            this.label3.TabIndex = 4;
            this.label3.Text = "Пользователь:";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(12, 98);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(48, 13);
            this.label4.TabIndex = 5;
            this.label4.Text = "Пароль:";
            // 
            // tbUser
            // 
            this.tbUser.Location = new System.Drawing.Point(101, 69);
            this.tbUser.Name = "tbUser";
            this.tbUser.Size = new System.Drawing.Size(148, 20);
            this.tbUser.TabIndex = 6;
            // 
            // tbPsw
            // 
            this.tbPsw.Location = new System.Drawing.Point(101, 95);
            this.tbPsw.Name = "tbPsw";
            this.tbPsw.Size = new System.Drawing.Size(148, 20);
            this.tbPsw.TabIndex = 7;
            // 
            // btConnect
            // 
            this.btConnect.Location = new System.Drawing.Point(285, 4);
            this.btConnect.Name = "btConnect";
            this.btConnect.Size = new System.Drawing.Size(95, 23);
            this.btConnect.TabIndex = 8;
            this.btConnect.Text = "Подключиться";
            this.btConnect.UseVisualStyleBackColor = true;
            this.btConnect.Click += new System.EventHandler(this.btConnect_Click);
            // 
            // btCancel
            // 
            this.btCancel.Location = new System.Drawing.Point(285, 33);
            this.btCancel.Name = "btCancel";
            this.btCancel.Size = new System.Drawing.Size(95, 23);
            this.btCancel.TabIndex = 9;
            this.btCancel.Text = "Отмена";
            this.btCancel.UseVisualStyleBackColor = true;
            this.btCancel.Click += new System.EventHandler(this.btCancel_Click);
            // 
            // FormConnectRDP
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(385, 130);
            this.Controls.Add(this.btCancel);
            this.Controls.Add(this.btConnect);
            this.Controls.Add(this.tbPsw);
            this.Controls.Add(this.tbUser);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.tbClient);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.cbServer);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "FormConnectRDP";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Подключение к удаленному рабочему столу";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ComboBox cbServer;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox tbClient;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox tbUser;
        private System.Windows.Forms.TextBox tbPsw;
        private System.Windows.Forms.Button btConnect;
        private System.Windows.Forms.Button btCancel;
    }
}