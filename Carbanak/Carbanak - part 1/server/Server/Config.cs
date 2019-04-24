using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Xml;

namespace Server
{
    public class Config
    {
		public class Client
		{
			public string ip { get; set; }
			public Client(Client c)
			{
				ip = c.ip;
			}
			public Client(string ip)
			{
				this.ip = ip;
			}
		}

        public int port, portClient;
		public string infoDomen;
		public string infoPsw;
		public List<Client> clients; //какие айпи могут подключаться к серверу в качестве клиента (CmdManager)
		public string password; //хеш пароля в виде строки
		byte[] passwordBin; //хеш пароля в байтовом массиве

        public Config()
        {
            port = 700;
			portClient = 800;
			infoDomen = string.Empty;
			clients = new List<Client>();
			password = string.Empty;
			passwordBin = new byte[16];
        }

        string GetNameFile()
        {
            return Application.StartupPath + @"\config_server.xml";
        }

        public bool Load()
        {
            string nameFile = GetNameFile();
			clients.Clear();
            if (System.IO.File.Exists(nameFile))
            {
                XmlTextReader xml = new XmlTextReader(nameFile);
                while (xml.Read())
                {
                    if (xml.NodeType == XmlNodeType.Element)
                    {
                        if (xml.Name == "Port")
                        {
                            xml.Read();
                            int.TryParse( xml.Value, out port);
                        }
						if (xml.Name == "PortClient")
						{
							xml.Read();
							int.TryParse(xml.Value, out portClient);
						}
						if (xml.Name == "InfoDomen")
						{
							xml.Read();
							infoDomen = xml.Value;
						}
						if (xml.Name == "InfoPsw")
						{
							xml.Read();
							infoPsw = xml.Value;
						}
						if (xml.Name == "IPClient")
						{
							xml.Read();
							Client c = new Client(xml.Value);
							clients.Add(c);
						}
						if (xml.Name == "Password")
						{
							xml.Read();
							password = xml.Value.Trim();
							if (password.Length == 0)
								passwordBin = new byte[16];
							else
								passwordBin = Misc.GetBytesHashMD5(password);
						}
					}
                }
                xml.Close(); 
                return true;
            }
            return false;
        }

        public void Save()
        {
            string nameFile = GetNameFile();
            XmlTextWriter xml = new XmlTextWriter(nameFile, System.Text.Encoding.GetEncoding(1251));
            xml.Formatting = Formatting.Indented;
            xml.WriteStartDocument();
            xml.WriteStartElement("cfg");

            xml.WriteStartElement("Port");
            xml.WriteString(port.ToString());
            xml.WriteEndElement();

			xml.WriteStartElement("PortClient");
			xml.WriteString(portClient.ToString());
			xml.WriteEndElement();

			xml.WriteStartElement("InfoDomen");
			xml.WriteString(infoDomen);
			xml.WriteEndElement();

			xml.WriteStartElement("InfoPsw");
			xml.WriteString(infoPsw);
			xml.WriteEndElement();

			for (int i = 0; i < clients.Count; i++)
			{
				xml.WriteStartElement("IPClient");
				xml.WriteString(clients[i].ip);
				xml.WriteEndElement();
			}

			xml.WriteStartElement("Password");
			xml.WriteString(password);
			xml.WriteEndElement();

			xml.WriteEndElement();
            xml.WriteEndDocument();
            xml.Close();
        }

		public bool CheckMD5Password(byte[] hash)
		{
			if( passwordBin == null || hash == null ) return false;
			return passwordBin.SequenceEqual(hash);
		}

		public bool CheckPassword(string psw)
		{
			byte[] pswMD5;
			if (psw.Length == 0)
			{
				pswMD5 = new byte[16];
			}
			else
			{
				pswMD5 = Misc.GetHashMD5Bin(psw);
			}
			return CheckMD5Password(pswMD5);
		}

		public void ChangePassword(string psw)
		{
			if (psw.Length == 0)
			{
				password = string.Empty;
				passwordBin = new byte[16];
			}
			else
			{
				password = Misc.GetHashMD5(psw);
				passwordBin = Misc.GetHashMD5Bin(psw);
			}
		}
    }

}
