using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;

namespace RDPPlayer
{
    public class Misc
    {
        //проверяет на правильность ip адрес
        static public bool IsValidIP(string ip)
        {
            IPAddress address;
            return IPAddress.TryParse(ip, out address);
        }

    }
}
