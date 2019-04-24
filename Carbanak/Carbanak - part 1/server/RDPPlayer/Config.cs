using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Xml;

namespace RDPPlayer
{
    public class Config
    {
        public class Server
        {
            public string IP;
            public int port;

            public bool Load(XmlTextReader xml)
            {
                while (xml.Read())
                {
                    if (xml.NodeType == XmlNodeType.EndElement) break;
                    if (xml.NodeType == XmlNodeType.Element)
                    {
                        if (xml.Name == "IP")
                        {
                            xml.Read();
                            IP = xml.Value;
                            xml.Read(); //переходим на EndElement
                        }
                        else
                            if (xml.Name == "Port")
                            {
                                xml.Read();
                                int.TryParse(xml.Value, out port);
                                xml.Read();
                            }
                    }
                }
                return true;
            }

            public bool Save(XmlTextWriter xml)
            {
                xml.WriteStartElement("IP");
                xml.WriteString(IP);
                xml.WriteEndElement();

                xml.WriteStartElement("Port");
                xml.WriteString(port.ToString());
                xml.WriteEndElement();
                return true;
            }

            public string IP_Port
            {
                get
                {
                    string ret = IP;
                    if (port > 0) ret += ':' + port.ToString();
                    return ret;
                }
            }

            static public string GetIP(string IPPort)
            {
                string[] ss = IPPort.Split(':');
                return ss[0].Trim();
            }

            static public int GetPort(string IPPort)
            {
                string[] ss = IPPort.Split(':');
                int port = 700;
                if (ss.Length >= 2)
                    int.TryParse(ss[1], out port);
                return port;
            }
        }

        public class Client
        {
            public string IP;
            public string user;
            public string psw;

            public bool Load(XmlTextReader xml)
            {
                while (xml.Read())
                {
                    if (xml.NodeType == XmlNodeType.EndElement) break;
                    if (xml.NodeType == XmlNodeType.Element)
                    {
                        if (xml.Name == "IP")
                        {
                            xml.Read();
                            IP = xml.Value;
                            xml.Read(); //переходим на EndElement
                        }
                        else
                            if (xml.Name == "User")
                            {
                                xml.Read();
                                user = xml.Value;
                                xml.Read();
                            }
                            else
                                if (xml.Name == "Password")
                                {
                                    xml.Read();
                                    psw = xml.Value;
                                    xml.Read();
                                }
                    }
                }
                return true;
            }

            public bool Save(XmlTextWriter xml)
            {
                xml.WriteStartElement("IP");
                xml.WriteString(IP);
                xml.WriteEndElement();

                xml.WriteStartElement("User");
                xml.WriteString(user);
                xml.WriteEndElement();

                xml.WriteStartElement("Password");
                xml.WriteString(psw);
                xml.WriteEndElement();

                return true;
            }
        }

        public List<Server> servers;
        public List<Client> clients;

        string GetNameFile()
        {
            return Application.StartupPath + @"\config_rdpplayer.xml";
        }

        public Config()
        {
            servers = new List<Server>();
            clients = new List<Client>();
        }

        public bool Load()
        {
            string nameFile = GetNameFile();
            if (System.IO.File.Exists(nameFile))
            {
                XmlTextReader xml = new XmlTextReader(nameFile);
                while (xml.Read())
                {
                    if (xml.NodeType == XmlNodeType.Element)
                    {
                        if (xml.Name == "Server")
                        {
                            Server srv = new Server();
                            if (srv.Load(xml))
                            {
                                servers.Add(srv);
                            }
                        }
                        if (xml.Name == "Client")
                        {
                            Client cln = new Client();
                            if (cln.Load(xml))
                            {
                                clients.Add(cln);
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

            foreach (Server srv in servers)
            {
                xml.WriteStartElement("Server");
                srv.Save(xml);
                xml.WriteEndElement();
            }

            foreach (Client cln in clients)
            {
                xml.WriteStartElement("Client");
                cln.Save(xml);
                xml.WriteEndElement();
            }

            xml.WriteEndElement();
            xml.WriteEndDocument();
            xml.Close();
        }

        public void InitCBServers(ComboBox cb)
        {
            cb.Items.Clear();
            cb.Items.Add("127.0.0.1");
            foreach (Server srv in servers)
                cb.Items.Add(srv.IP_Port);
        }

        public void AddServer(string s)
        {
            Server srv = new Server();
            srv.IP = Server.GetIP(s);
            srv.port = Server.GetPort(s);
            servers.Add(srv);
        }

        public void AddClient(string ip, string user, string psw)
        {
            Client cln = GetClient(ip);
            if (cln == null)
            {
                cln = new Client();
                clients.Add(cln);
            }
            cln.IP = ip;
            cln.user = user;
            cln.psw = psw;
        }

        public Client GetClient(string ip)
        {
            return clients.Find(c => c.IP == ip);
        }
    }
}
