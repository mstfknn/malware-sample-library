namespace CmdManager
{
	partial class ControlBotPages
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
			this.tabPages = new System.Windows.Forms.TabControl();
			this.SuspendLayout();
			// 
			// tabPages
			// 
			this.tabPages.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tabPages.DrawMode = System.Windows.Forms.TabDrawMode.OwnerDrawFixed;
			this.tabPages.ItemSize = new System.Drawing.Size(128, 18);
			this.tabPages.Location = new System.Drawing.Point(0, 0);
			this.tabPages.Margin = new System.Windows.Forms.Padding(0);
			this.tabPages.Name = "tabPages";
			this.tabPages.SelectedIndex = 0;
			this.tabPages.Size = new System.Drawing.Size(684, 508);
			this.tabPages.SizeMode = System.Windows.Forms.TabSizeMode.Fixed;
			this.tabPages.TabIndex = 0;
			this.tabPages.DrawItem += new System.Windows.Forms.DrawItemEventHandler(this.tabPages_DrawItem);
			this.tabPages.MouseDown += new System.Windows.Forms.MouseEventHandler(this.tabPages_MouseDown);
			// 
			// ControlBotPages
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this.tabPages);
			this.Margin = new System.Windows.Forms.Padding(0);
			this.Name = "ControlBotPages";
			this.Size = new System.Drawing.Size(684, 508);
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.TabControl tabPages;
	}
}
