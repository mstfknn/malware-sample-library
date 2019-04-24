using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Security.Cryptography;

namespace CmdManager
{
	public class Misc
	{
		static public string GetHashMD5(string s)
		{
			byte[] data = GetHashMD5Bin(s);
			StringBuilder sb = new StringBuilder(48);
			for (int i = 0; i < data.Length; i++)
				sb.Append(data[i].ToString("x2"));
			return sb.ToString();
		}

		static public byte[] GetHashMD5Bin(string s)
		{
			using (MD5 md5 = MD5.Create())
			{
				return md5.ComputeHash(Encoding.UTF8.GetBytes(s));
			}
		}

		static public byte[] GetBytesHashMD5(string md5)
		{
			if (md5.Length != 32) return null;
			byte[] ret = new byte[16];
			for (int i = 0; i < 16; i++)
			{
				if (!byte.TryParse(md5.Substring(2 * i, 2), System.Globalization.NumberStyles.AllowHexSpecifier, null, out ret[i]))
					return null;
			}
			return ret;
		}

		static public byte[] MD5File(string fileName)
		{
			try
			{
				using (MD5 md5 = MD5.Create())
				{
					System.IO.FileStream fs = new System.IO.FileStream(fileName, System.IO.FileMode.Open, System.IO.FileAccess.Read);
					byte[] ret = md5.ComputeHash(fs);
					fs.Close();
					return ret;
				}
			}
			catch
			{
				return null;
			}
		}
	}
}
