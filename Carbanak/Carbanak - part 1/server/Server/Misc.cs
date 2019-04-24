using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Web;
using System.Net;
using System.IO;
using System.Security.Cryptography;

namespace Server
{
	public class Misc
	{
		static public string GetHTTP(string url)
		{
			if (url.Substring(0, 4) != "http")
				url = "http://" + url;
			HttpWebRequest request = WebRequest.Create(url) as HttpWebRequest;
			request.Method = "GET";
			HttpWebResponse response = (HttpWebResponse)request.GetResponse();
			Encoding enc = Encoding.GetEncoding(response.CharacterSet);
			StreamReader sr = new StreamReader(response.GetResponseStream(), enc);
			return sr.ReadToEnd();
		}

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
	}
}
