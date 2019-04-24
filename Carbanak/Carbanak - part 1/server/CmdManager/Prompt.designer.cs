namespace CmdManager
{
    partial class CommandPrompt
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
            this.components = new System.ComponentModel.Container();
            this.panelBottom = new System.Windows.Forms.Panel();
            this.txtInput = new System.Windows.Forms.TextBox();
            this.lblPrompt = new System.Windows.Forms.Label();
            this.rtbMessages = new System.Windows.Forms.RichTextBox();
            this.toolTipCommand = new System.Windows.Forms.ToolTip(this.components);
            this.panelBottom.SuspendLayout();
            this.SuspendLayout();
            // 
            // panelBottom
            // 
            this.panelBottom.BackColor = System.Drawing.SystemColors.Window;
            this.panelBottom.Controls.Add(this.txtInput);
            this.panelBottom.Controls.Add(this.lblPrompt);
            this.panelBottom.Dock = System.Windows.Forms.DockStyle.Top;
            this.panelBottom.Location = new System.Drawing.Point(0, 78);
            this.panelBottom.Margin = new System.Windows.Forms.Padding(0);
            this.panelBottom.Name = "panelBottom";
            this.panelBottom.Size = new System.Drawing.Size(200, 17);
            this.panelBottom.TabIndex = 0;
            // 
            // txtInput
            // 
            this.txtInput.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.txtInput.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtInput.Location = new System.Drawing.Point(13, 0);
            this.txtInput.Name = "txtInput";
            this.txtInput.Size = new System.Drawing.Size(187, 13);
            this.txtInput.TabIndex = 1;
            this.txtInput.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.txtInput_KeyPress);
            this.txtInput.TextChanged += new System.EventHandler(this.txtInput_TextChanged);
            this.txtInput.KeyDown += new System.Windows.Forms.KeyEventHandler(this.txtInput_KeyDown);
            // 
            // lblPrompt
            // 
            this.lblPrompt.AutoSize = true;
            this.lblPrompt.BackColor = System.Drawing.SystemColors.Window;
            this.lblPrompt.Dock = System.Windows.Forms.DockStyle.Left;
            this.lblPrompt.Location = new System.Drawing.Point(0, 0);
            this.lblPrompt.Margin = new System.Windows.Forms.Padding(0);
            this.lblPrompt.Name = "lblPrompt";
            this.lblPrompt.Size = new System.Drawing.Size(13, 13);
            this.lblPrompt.TabIndex = 0;
            this.lblPrompt.Text = ">";
            // 
            // rtbMessages
            // 
            this.rtbMessages.BackColor = System.Drawing.SystemColors.Window;
            this.rtbMessages.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.rtbMessages.Dock = System.Windows.Forms.DockStyle.Top;
            this.rtbMessages.Location = new System.Drawing.Point(0, 0);
            this.rtbMessages.Name = "rtbMessages";
            this.rtbMessages.ReadOnly = true;
            this.rtbMessages.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.None;
            this.rtbMessages.Size = new System.Drawing.Size(200, 78);
            this.rtbMessages.TabIndex = 2;
            this.rtbMessages.TabStop = false;
            this.rtbMessages.Text = "";
            this.rtbMessages.TextChanged += new System.EventHandler(this.rtbMessages_TextChanged);
            this.rtbMessages.Click += new System.EventHandler(this.rtbMessages_Click);
            // 
            // toolTipCommand
            // 
            this.toolTipCommand.AutoPopDelay = 5000;
            this.toolTipCommand.InitialDelay = 0;
            this.toolTipCommand.ReshowDelay = 100;
            // 
            // CommandPrompt
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.panelBottom);
            this.Controls.Add(this.rtbMessages);
            this.MinimumSize = new System.Drawing.Size(0, 17);
            this.Name = "CommandPrompt";
            this.Size = new System.Drawing.Size(200, 95);
            this.Load += new System.EventHandler(this.Prompt_Load);
            this.FontChanged += new System.EventHandler(this.Prompt_FontChanged);
            this.Resize += new System.EventHandler(this.CommandPrompt_Resize);
            this.ForeColorChanged += new System.EventHandler(this.Prompt_ForeColorChanged);
            this.BackColorChanged += new System.EventHandler(this.Prompt_BackColorChanged);
            this.panelBottom.ResumeLayout(false);
            this.panelBottom.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel panelBottom;
        private System.Windows.Forms.Label lblPrompt;
        private System.Windows.Forms.TextBox txtInput;
        private System.Windows.Forms.RichTextBox rtbMessages;
        private System.Windows.Forms.ToolTip toolTipCommand;




    }
}
