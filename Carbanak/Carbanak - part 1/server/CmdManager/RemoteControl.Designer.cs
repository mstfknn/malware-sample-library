namespace CmdManager
{
    partial class RemoteControl
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.remoteDesktop1 = new VncSharp.RemoteDesktop();
            this.SuspendLayout();
            // 
            // remoteDesktop1
            // 
            this.remoteDesktop1.AutoScroll = true;
            this.remoteDesktop1.AutoScrollMinSize = new System.Drawing.Size(608, 427);
            this.remoteDesktop1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.remoteDesktop1.Location = new System.Drawing.Point(0, 0);
            this.remoteDesktop1.Name = "remoteDesktop1";
            this.remoteDesktop1.Size = new System.Drawing.Size(1338, 781);
            this.remoteDesktop1.TabIndex = 0;
            // 
            // RemoteControl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.remoteDesktop1);
            this.Name = "RemoteControl";
            this.Size = new System.Drawing.Size(1338, 781);
            this.Load += new System.EventHandler(this.RemoteControl_Load);
            this.ResumeLayout(false);

        }

        #endregion

        private VncSharp.RemoteDesktop remoteDesktop1;
    }
}
