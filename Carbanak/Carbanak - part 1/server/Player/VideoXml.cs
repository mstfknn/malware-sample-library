using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;

namespace Player
{
    public class VideoXml
    {
        string comment; //комментарий к видео
        int opened; //сколько раз было открыто видео
        bool update; //true - если было что-то изменено
        string nameFile;

        public VideoXml()
        {
            comment = string.Empty;
            opened = 0;
            update = false;
        }

        public bool Load(string nameFile)
        {
            this.nameFile = nameFile;
            if (System.IO.File.Exists(nameFile))
            {
                XmlTextReader xml = new XmlTextReader(nameFile);
                while (xml.Read())
                {
                    if (xml.NodeType == XmlNodeType.Element)
                    {
                        if (xml.Name == "Comment")
                        {
                            xml.Read();
                            comment = xml.Value;
                        }
                        if (xml.Name == "Opened")
                        {
                            xml.Read();
                            int.TryParse(xml.Value, out opened);
                        }
                    }
                }
                xml.Close();
                update = false;
                return true;
            }
            return false;
        }

        public void Save()
        {
            Save(nameFile);
        }

        public void Save(string nameFile)
        {
            if (nameFile == null || nameFile.Length == 0) return;
            XmlTextWriter xml = new XmlTextWriter(nameFile, System.Text.Encoding.GetEncoding(1251));
            xml.Formatting = Formatting.Indented;
            xml.WriteStartDocument();
            xml.WriteStartElement("Video");

            xml.WriteStartElement("Comment");
            xml.WriteString(comment);
            xml.WriteEndElement();

            xml.WriteStartElement("Opened");
            xml.WriteString(opened.ToString());
            xml.WriteEndElement();

            xml.WriteEndElement();
            xml.WriteEndDocument();
            xml.Close();
            update = false;
        }

        public string Comment
        {
            get { return comment; }
            set
            {
                if (comment != value)
                {
                    comment = value;
                    update = true;
                }
            }
        }

        public int Opened
        {
            get { return opened; }
        }

        public int Open()
        {
            opened++;
            update = true;
            return opened;
        }

        public bool Modify
        {
            get { return update; }
        }

        public string NameFile
        {
            get { return nameFile; }
        }
    }
}
