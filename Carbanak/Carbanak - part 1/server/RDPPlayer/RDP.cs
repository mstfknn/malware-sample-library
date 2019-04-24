using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Server;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace RDPPlayer
{
    public class Server
    {
        string ipServer;
        int portServer;
        bool connected = false;
        Sender sc;
        object lockServer;
        int idBot; //ид плеера на сервере (ид бота)
        bool stopServer; //true - остановить фоновый поток сервера, необходимо при закрытии сервера
        string uid;

        static List<Server> servers;

        public Server( string ipServer, int portServer)
        {
            this.ipServer = ipServer;
            this.portServer = portServer;
            lockServer = new object();
            idBot = 0;
            uid = "RDPPlayer" + Sender.GenUID();
        }

        bool Connect()
        {
            lock (lockServer) //защищаемся от повторного коннекта
            {
                try
                {
                    if (!connected) //если еще не соединены
                    {
                        Socket _sc = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                        _sc.Connect(ipServer, portServer);
                        sc = new Sender(_sc);
                        connected = true;
                        byte[] pidBot = BitConverter.GetBytes(idBot);
                        byte[] uid2 = Encoding.ASCII.GetBytes(uid);
                        byte[] send = new byte[pidBot.Length + uid2.Length + 1];
                        Array.Copy(pidBot, send, pidBot.Length);
                        Array.Copy(uid2, 0, send, pidBot.Length, uid2.Length);
                        //регистрируемся
                        byte[] data = SendRecv(Sender.ID_REG, send, send.Length);
                        if (data != null)
                        {
                            if (data.Length >= 4)
                            {
                                idBot = BitConverter.ToInt32(data, 0);
                                return true;
                            }
                        }
                    }
                }
                catch
                {
                    return false;
                }
            }
            return false;
        }

        //отсылка пакета
        bool Send(int id, byte[] data, int c_data)
        {
            if (connected)
            {
                lock (lockServer)
                {
                    return sc.WritePacket(id, data, c_data);
                }
            }
            return false;
        }

        //отсылка пакета и получение данных (ответа)
        byte[] SendRecv(int id, byte[] data, int c_data)
        {
            if (connected)
            {
                lock (lockServer)
                {
                    if (sc.WritePacket(id, data, c_data))
                    {
                        byte[] recvData;
                        int id2 = sc.ReadPacket(out recvData);
                        if (id2 > 0)
                            return recvData;
                    }
                }
            }
            return null;
        }

        static public Server GetServer(string ipServer, int portServer)
        {
            if (servers != null)
                foreach (Server s in servers)
                    if (s.ipServer == ipServer && s.portServer == portServer)
                        return s;
            Server newServer = new Server(ipServer, portServer);
            //запускаем сервер в отдельном потоке, там он соединится
            Thread th = new Thread(newServer.LoopServer);
            th.Start();
            if( servers == null ) servers = new List<Server>();
            servers.Add(newServer);
            return newServer;
        }

        //закрывает все сервера, чтобы выйти из программы
        static public void CloseAllServers()
        {
            if (servers != null)
                foreach (Server srv in servers)
                    srv.Close();
        }

        //дает команду на проброс порта, возвращает ид порта по которому будем запрашивать номер порта через который идет проброс
        public int OpenPortForward(int port, string ip)
        {
            byte[] data = new byte[2 + ip.Length + 1];
            byte[] bport = BitConverter.GetBytes(port);
            Array.Copy(bport, data, 2);
            Sender.PutCString(ip, data, 2);
            byte[] data2 = SendRecv(Sender.ID_START_PORTFORWARD, data, data.Length); //для порта достаточно 2-х байт
            if (data2 != null && data2.Length >= 2)
                return BitConverter.ToInt32(data2, 0);
            return 0;
        }

        //запрашивает номер порта через который идет проброс, возвращает 0, если такого порта нет
        public int GetPortForward(int idPort)
        {
            byte[] data = SendRecv(Sender.ID_GET_PORTFORWARD, BitConverter.GetBytes(idPort), 4);
            if (data != null && data.Length >= 2)
                return BitConverter.ToInt16(data, 0);
            return 0;
        }

        //поддержка связи с сервером, в отдельном потоке
        void LoopServer()
        {
            int ping = 5; //через сколько секунд посылаем пинг
            byte[] pingData = new byte[] {0}; //данные для пинга
            while (!stopServer)
            {
                if (connected)
                {
                    if (--ping == 0)
                    {
                        if (!Send(Sender.ID_PING, pingData, pingData.Length))
                        {
                            connected = false;
                            try
                            {
                                sc.Close();
                            }
                            catch
                            {
                            }
                        }
                        ping = 5;
                    }
                }
                else
                    Connect();
                Thread.Sleep(1000);
            }
        }

        public void Close()
        {
            stopServer = true;
        }

        public bool IsConnected
        {
            get { return connected; }
        }

        public bool IsStopped
        {
            get { return stopServer; }
        }

        public string IP
        {
            get { return ipServer; }
        }

        public int Port
        {
            get { return portServer; }
        }
    }

    public class Connector
    {
        Server server;
        string ipClient;
        int portTunnel; //порт через который идет проброс
        int portForward; //номер пробрасываемого порта
        int idPort; //ид пробрасываемого порта

        protected Connector( string ipServer, int portServer, string ipClient, int portForward)
        {
            server = Server.GetServer(ipServer, portServer);
            this.ipClient = ipClient;
            this.portForward = portForward;
        }

        //дает команду серверу на проброс порта port, возвращает true, если операция прошла успешно
        protected bool OpenPortForward()
        {
            //если к серверу еще не подсоединились, то ждем 60 сек.
            int wait = 60;
            while (!server.IsConnected && wait > 0)
            {
                wait--;
                Thread.Sleep(1000);
            }
            if (!server.IsConnected) return false;//к серверу так и не подсоединились, возвращаемся
            idPort = server.OpenPortForward(portForward, ipClient);
            if (idPort == 0) return false;
            return true;
        }

        protected bool GetPortForward()
        {
            portTunnel = server.GetPortForward(idPort);
            if (portTunnel == 0) return false;
            return true;
        }

        public int PortTunnel
        {
            get { return portTunnel; }
        }

        public string IPServer
        {
            get { return server.IP; }
        }

        public int PortServer
        {
            get { return server.Port; }
        }

        public string IPClient
        {
            get { return ipClient; }
        }
    }

    public class RDP : Connector 
    {
        bool stopLoop = false;
        bool portOpen = false; //true - если на сервере открыт порт и к нему можно подключаться

        public RDP(string ipServer, int portServer, string ipClient, int PortForward) : base(ipServer, portServer, ipClient, PortForward)
        {

        }

        void Dispose()
        {
            if (!stopLoop) Stop();
        }

        public void LoopConnect()
        {
            while (!stopLoop)
            {
                if (OpenPortForward()) break;
                Thread.Sleep(1000);
            }
            while (!stopLoop)
            {
                if (GetPortForward()) break;
                Thread.Sleep(1000);
            }
            if (PortTunnel > 0)
            {
                portOpen = true;
                //каждые 5 с. проверя ем открыт ли порт, так как в RDP контроле не всегда можно понять разорвана ли связь
                while (!stopLoop)
                {
                    if (!GetPortForward())
                    {
                        portOpen = false;
                        break;
                    }
                    Thread.Sleep(5000);
                }
            }
        }

        public void Run()
        {
            portOpen = false;
            Thread th = new Thread(this.LoopConnect);
            th.Start();
        }

        public void Stop()
        {
            stopLoop = true;
        }

        public bool IsPortOpen
        {
            get { return portOpen; }
        }
    }

    public class VNC : Connector
    {
        bool stopLoop = false;
        bool portOpen = false; //true - если на сервере открыт порт и к нему можно подключаться

        public VNC(string ipServer, int portServer, string ipClient, int PortForward)
            : base(ipServer, portServer, ipClient, PortForward)
        {

        }

        void Dispose()
        {
            if (!stopLoop) Stop();
        }

        public void LoopConnect()
        {
            while (!stopLoop)
            {
                if (OpenPortForward()) break;
                Thread.Sleep(1000);
            }
            while (!stopLoop)
            {
                if (GetPortForward()) break;
                Thread.Sleep(1000);
            }
            if (PortTunnel > 0)
            {
                portOpen = true;
            }
        }

        public void Run()
        {
            portOpen = false;
            Thread th = new Thread(this.LoopConnect);
            th.Start();
        }

        public void Stop()
        {
            stopLoop = true;
        }

        public bool IsPortOpen
        {
            get { return portOpen; }
        }
    }

}
