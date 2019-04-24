namespace CmdManager
{
    partial class FullScreenView
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
            this.tabsOpenBots = new System.Windows.Forms.TabControl();
            this.tbBots = new System.Windows.Forms.TabPage();
            this.tabsOpenBots.SuspendLayout();
            this.SuspendLayout();
            // 
            // tabsOpenBots
            // 
            this.tabsOpenBots.Alignment = System.Windows.Forms.TabAlignment.Left;
            this.tabsOpenBots.Controls.Add(this.tbBots);
            this.tabsOpenBots.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabsOpenBots.DrawMode = System.Windows.Forms.TabDrawMode.OwnerDrawFixed;
            this.tabsOpenBots.ItemSize = new System.Drawing.Size(25, 150);
            this.tabsOpenBots.Location = new System.Drawing.Point(0, 0);
            this.tabsOpenBots.Margin = new System.Windows.Forms.Padding(4);
            this.tabsOpenBots.Multiline = true;
            this.tabsOpenBots.Name = "tabsOpenBots";
            this.tabsOpenBots.SelectedIndex = 0;
            this.tabsOpenBots.Size = new System.Drawing.Size(1320, 726);
            this.tabsOpenBots.SizeMode = System.Windows.Forms.TabSizeMode.Fixed;
            this.tabsOpenBots.TabIndex = 1;
            this.tabsOpenBots.DrawItem += new System.Windows.Forms.DrawItemEventHandler(this.tabsOpenBots_DrawItem);
            // 
            // tbBots
            // 
            this.tbBots.Location = new System.Drawing.Point(154, 4);
            this.tbBots.Margin = new System.Windows.Forms.Padding(4);
            this.tbBots.Name = "tbBots";
            this.tbBots.Padding = new System.Windows.Forms.Padding(4);
            this.tbBots.Size = new System.Drawing.Size(1162, 718);
            this.tbBots.TabIndex = 1;
            this.tbBots.UseVisualStyleBackColor = true;
            // 
            // FullScreenView
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1320, 726);
            this.Controls.Add(this.tabsOpenBots);
            this.MinimizeBox = false;
            this.Name = "FullScreenView";
            this.Text = "FullScreenView";
            this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.FullScreenView_FormClosed);
            this.tabsOpenBots.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TabControl tabsOpenBots;
        private System.Windows.Forms.TabPage tbBots;
    }
}