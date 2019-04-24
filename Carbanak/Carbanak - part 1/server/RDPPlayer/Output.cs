using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace RDPPlayer
{
    static public class Output
    {
        static public void MsgErr(string text)
        {
            MessageBox.Show(text, "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        static public DialogResult MsgWrn(string text)
        {
            return MessageBox.Show(text, "Предупреждение", MessageBoxButtons.YesNo, MessageBoxIcon.Warning);
        }

        static public DialogResult MsgWrnCancel(string text)
        {
            return MessageBox.Show(text, "Предупреждение", MessageBoxButtons.YesNoCancel, MessageBoxIcon.Warning);
        }

        static public void MsgInfo(string text)
        {
            MessageBox.Show(text, "Информация", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        static public DialogResult MsgQue(string text, MessageBoxButtons bts)
        {
            return MessageBox.Show(text, "Вопрос", bts, MessageBoxIcon.Question);
        }

        static public DialogResult MsgQue(string text)
        {
            return MsgQue(text, MessageBoxButtons.YesNo);
        }
    }
}
