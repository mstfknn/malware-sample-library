using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Xml;

namespace CmdManager
{
	public class Config
	{
		public class IpPort
		{
			public string ip { get; set; }
			public int port { get; set; }

			public IpPort()
			{
			}

			public IpPort(IpPort i)
			{
				ip = i.ip;
				port = i.port;
			}
		}

		public List<IpPort> servers;

		public Config()
		{
			servers = new List<IpPort>();
		}

		string GetNameFile()
		{
			return Application.StartupPath + @"\CmdManager.xml";
		}

		public bool Load()
		{
			string nameFile = GetNameFile();
			servers.Clear();
			if (System.IO.File.Exists(nameFile))
			{
				XmlTextReader xml = new XmlTextReader(nameFile);
				while (xml.Read())
				{
					if (xml.NodeType == XmlNodeType.Element)
					{
						if (xml.Name == "Server")
						{
							xml.Read();
							string[] ss = xml.Value.Split(':');
							if (ss.Length == 2)
							{
								IpPort ip = new IpPort();
								ip.ip = ss[0];
								int port;
								if (int.TryParse(ss[1], out port))
								{
									if (port > 0 && port < 65536)
									{
										ip.port = port;
										servers.Add(ip);
									}
								}
							}
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

			foreach (IpPort i in servers)
			{
				xml.WriteStartElement("Server");
				xml.WriteString(i.ip + ':' + i.port.ToString());
				xml.WriteEndElement();
			}

			xml.WriteEndElement();
			xml.WriteEndDocument();
			xml.Close();
		}
	}


}
