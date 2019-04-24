using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;

namespace Player
{
    public class RivFTP
    {
        string url;

        public RivFTP(string url)
        {
            this.url = url;
            if (url[url.Length - 1] == '\\') url = url.Substring(0, url.Length - 1);
        }

        public string[] ListDirectory()
        {
            return ListDirectory(string.Empty);
        }

        public string[] ListDirectory(string maska)
        {
            string url2 = url;
            if( maska.Length > 0 ) url2 += '/' + maska;
            FtpWebRequest reqFTP = (FtpWebRequest)FtpWebRequest.Create(url2);
            reqFTP.KeepAlive = false;
            reqFTP.Method = WebRequestMethods.Ftp.ListDirectory;
        
            string result = String.Empty;
            FtpWebResponse response = (FtpWebResponse)reqFTP.GetResponse();
            long size = response.ContentLength;
            System.IO.Stream datastream = response.GetResponseStream();
            System.IO.StreamReader sr = new System.IO.StreamReader(datastream);
            result = sr.ReadToEnd();
            sr.Close();
            datastream.Close();
            response.Close();
            //while (result.Length > 2 && result.Substring(result.Length - 2, 2) == "\r\n")
            //    result = result.Substring(0, result.Length - 2);
            string[] ret = result.Split(new string[] { "\r\n" }, StringSplitOptions.RemoveEmptyEntries);
            //оставляем только последнюю папку или файл
            for (int i = 0; i < ret.Length; i++)
            {
                int p = ret[i].LastIndexOf('/');
                if (p >= 0)
                    ret[i] = ret[i].Substring(p + 1);
            }
            return ret;
        }

        public bool LoadFile(string from, string to)
        {
            FtpWebRequest reqFTP = (FtpWebRequest)FtpWebRequest.Create(url + '/' + from);
            reqFTP.KeepAlive = false;
            reqFTP.Method = WebRequestMethods.Ftp.DownloadFile;

            string result = String.Empty;
            FtpWebResponse response = (FtpWebResponse)reqFTP.GetResponse();
            long size = response.ContentLength;
            System.IO.Stream datastream = response.GetResponseStream();
            System.IO.FileStream toFile = new System.IO.FileStream(to, System.IO.FileMode.Create);
            byte[] buffer = new byte[4096];
            while(true)
            {
                int sz = datastream.Read(buffer, 0, buffer.Length);
                if (sz == 0) break;
                toFile.Write(buffer, 0, sz);
            }
            toFile.Close();
            datastream.Close();
            response.Close();
            return true;
        }
    }
}
