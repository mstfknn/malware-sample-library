namespace Player
{
    partial class FormBots
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
            this.lvBots = new System.Windows.Forms.ListView();
            this.clnUid = new System.Windows.Forms.ColumnHeader();
            this.clnIp = new System.Windows.Forms.ColumnHeader();
            this.btOk = new System.Windows.Forms.Button();
            this.btUpdate = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // lvBots
            // 
            this.lvBots.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.clnUid,
            this.clnIp});
            this.lvBots.Dock = System.Windows.Forms.DockStyle.Top;
            this.lvBots.FullRowSelect = true;
            this.lvBots.Location = new System.Drawing.Point(0, 0);
            this.lvBots.Name = "lvBots";
            this.lvBots.Size = new System.Drawing.Size(590, 384);
            this.lvBots.TabIndex = 0;
            this.lvBots.UseCompatibleStateImageBehavior = false;
            this.lvBots.View = System.Windows.Forms.View.Details;
            // 
            // clnUid
            // 
            this.clnUid.Text = "UID";
            this.clnUid.Width = 200;
            // 
            // clnIp
            // 
            this.clnIp.Text = "IP";
            this.clnIp.Width = 150;
            // 
            // btOk
            // 
            this.btOk.Location = new System.Drawing.Point(513, 390);
            this.btOk.Name = "btOk";
            this.btOk.Size = new System.Drawing.Size(75, 23);
            this.btOk.TabIndex = 1;
            this.btOk.Text = "Ок";
            this.btOk.UseVisualStyleBackColor = true;
            this.btOk.Click += new System.EventHandler(this.btOk_Click);
            // 
            // btUpdate
            // 
            this.btUpdate.Location = new System.Drawing.Point(432, 390);
            this.btUpdate.Name = "btUpdate";
            this.btUpdate.Size = new System.Drawing.Size(75, 23);
            this.btUpdate.TabIndex = 2;
            this.btUpdate.Text = "Обновить";
            this.btUpdate.UseVisualStyleBackColor = true;
            this.btUpdate.Click += new System.EventHandler(this.btUpdate_Click);
            // 
            // FormBots
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(590, 416);
            this.Controls.Add(this.btUpdate);
            this.Controls.Add(this.btOk);
            this.Controls.Add(this.lvBots);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "FormBots";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Подключенные боты";
            this.Load += new System.EventHandler(this.FormBots_Load);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ListView lvBots;
        private System.Windows.Forms.ColumnHeader clnUid;
        private System.Windows.Forms.ColumnHeader clnIp;
        private System.Windows.Forms.Button btOk;
        private System.Windows.Forms.Button btUpdate;
    }
}