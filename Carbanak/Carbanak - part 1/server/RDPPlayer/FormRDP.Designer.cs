namespace RDPPlayer
{
    partial class FormRDP
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
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.tabRDP = new System.Windows.Forms.TabControl();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.связьToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.itemConnectRDP = new System.Windows.Forms.ToolStripMenuItem();
            this.itemConnectVNC = new System.Windows.Forms.ToolStripMenuItem();
            this.itemCloseConnect = new System.Windows.Forms.ToolStripMenuItem();
            this.tableLayoutPanel1.SuspendLayout();
            this.menuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.ColumnCount = 1;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel1.Controls.Add(this.tabRDP, 0, 1);
            this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 24);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 2;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(838, 479);
            this.tableLayoutPanel1.TabIndex = 0;
            // 
            // tabRDP
            // 
            this.tabRDP.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabRDP.Location = new System.Drawing.Point(3, 3);
            this.tabRDP.Name = "tabRDP";
            this.tabRDP.SelectedIndex = 0;
            this.tabRDP.Size = new System.Drawing.Size(832, 473);
            this.tabRDP.TabIndex = 1;
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.связьToolStripMenuItem,
            this.itemCloseConnect});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(838, 24);
            this.menuStrip1.TabIndex = 1;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // связьToolStripMenuItem
            // 
            this.связьToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.itemConnectRDP,
            this.itemConnectVNC});
            this.связьToolStripMenuItem.Name = "связьToolStripMenuItem";
            this.связьToolStripMenuItem.Size = new System.Drawing.Size(49, 20);
            this.связьToolStripMenuItem.Text = "Связь";
            // 
            // itemConnectRDP
            // 
            this.itemConnectRDP.Name = "itemConnectRDP";
            this.itemConnectRDP.Size = new System.Drawing.Size(194, 22);
            this.itemConnectRDP.Text = "Подключение по RDP";
            this.itemConnectRDP.Click += new System.EventHandler(this.itemConnectRDP_Click);
            // 
            // itemConnectVNC
            // 
            this.itemConnectVNC.Name = "itemConnectVNC";
            this.itemConnectVNC.Size = new System.Drawing.Size(194, 22);
            this.itemConnectVNC.Text = "Подключение по VNC";
            this.itemConnectVNC.Click += new System.EventHandler(this.itemConnectVNC_Click);
            // 
            // itemCloseConnect
            // 
            this.itemCloseConnect.Name = "itemCloseConnect";
            this.itemCloseConnect.Size = new System.Drawing.Size(63, 20);
            this.itemCloseConnect.Text = "Закрыть";
            this.itemCloseConnect.Click += new System.EventHandler(this.itemCloseConnect_Click);
            // 
            // FormRDP
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(838, 503);
            this.Controls.Add(this.tableLayoutPanel1);
            this.Controls.Add(this.menuStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "FormRDP";
            this.ShowIcon = false;
            this.Text = "RDP and VNC Player";
            this.tableLayoutPanel1.ResumeLayout(false);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.TabControl tabRDP;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem связьToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem itemConnectRDP;
        private System.Windows.Forms.ToolStripMenuItem itemConnectVNC;
        private System.Windows.Forms.ToolStripMenuItem itemCloseConnect;
    }
}

