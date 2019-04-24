using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Xml;

namespace Player
{
    public class Config
    {
        public List<string> pathVideo;
        
        public Config()
        {
            pathVideo = new List<string>();
        }

        string GetNameFile()
        {
            return Application.StartupPath + @"\config.xml";
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
                        if (xml.Name == "PathVideo")
                        {
                            xml.Read();
                            pathVideo.Add(xml.Value);
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

            foreach (string path in pathVideo)
            {
                xml.WriteStartElement("PathVideo");
                xml.WriteString(path);
                xml.WriteEndElement();
            }

            xml.WriteEndElement();
            xml.WriteEndDocument();
            xml.Close();
        }

    }
}
