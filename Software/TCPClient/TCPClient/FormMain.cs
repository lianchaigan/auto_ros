using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Diagnostics.Tracing;
using System.Data;
using System.Drawing;
using System.Linq;
using System.IO;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;
using Common;
using System.Windows.Forms.DataVisualization.Charting;

namespace TCPClient
{
    public partial class FormMain : Form
    {
        #region Constant
        private const int TCP_NOP = 0;
        private const int TCP_CONNECT = 1;
        private const int TCP_SCAN = 2;
        #endregion
 
        #region Thread Declarations
        private Thread ControlThread;
        private Thread DataThread;
        private AutoResetEvent evtCtrl = new AutoResetEvent(false);
        private AutoResetEvent evtWait = new AutoResetEvent(false);
        private AutoResetEvent evtDataReady = new AutoResetEvent(false);
        private int g_State = TCP_NOP;
        private int g_nTcpReqId = 2000000000;
        private int bytesRead = 0;
        private Series s1;
        #endregion

        Socket client = null;
        IPEndPoint remoteEP = null;
        private byte[] RxTcpBuf = new byte[1024*20];

        public FormMain()
        {
            InitializeComponent();
        }
        #region Callbacks
        private void ConnectCallback(IAsyncResult ar)
        {
            try
            {
                // Retrieve the socket from the state object.  
                Socket client = (Socket)ar.AsyncState;

                // Complete the connection.  
                client.EndConnect(ar);

                // Signal that the connection has been made.  
                evtWait.Set();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        private void ReceiveCallback(IAsyncResult ar)
        {
            try
            {
                Socket client = (Socket)ar.AsyncState;

                // Read data from the remote device.  
                bytesRead = client.EndReceive(ar);
                RxTcpBuf[bytesRead] = 0;

                // String str = Encoding.UTF8.GetString(RxTcpBuf, 0, bytesRead);
                // Debug.WriteLine(str);

                // Invoke(new DELEGATE_SCAN_UPDATE(ScanUpdate), str);

                evtDataReady.Set();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        private void SendCallback(IAsyncResult ar)
        {
            try
            {
                // Retrieve the socket from the state object.  
                Socket client = (Socket)ar.AsyncState;

                // Complete sending the data to the remote device.  
                int bytesSent = client.EndSend(ar);
                Console.WriteLine("Sent {0} bytes to server.", bytesSent);

                // Signal that all bytes have been sent.  
                evtWait.Set();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }
        #endregion

        #region Delegate
        public delegate void DELEGATE_SCAN_UPDATE( string str );
        public delegate void DELEGATE_DATA_UPDATE( float Angle, float Distance, bool bValid );

        private void DataUpdate( float Angle, float Distance, bool bValid )
        {
            float a = 180.0f * Angle / 3.142f;
           // string line = a.ToString() + " " + Distance.ToString() + " " + bValid.ToString() + "\r\n";
           // tboxOutput.Text += line;

            if( Distance > 0.5f )
            {
                Distance = 0.5f;
            }
            s1.Points.AddXY(a, Distance);
        }


        private void ScanUpdate(string str)
        {
            String s1 = "\"laser_points\":\"";
            String s2 = null;
            int i = 0;
            bool r = str.Contains(s1);
            if (true == r)
            {
                i = str.IndexOf(s1) + s1.Length;
                s2 = str.Substring(i);
            }

            string tok = "\"";
            i = s2.IndexOf(tok);
            string sf = s2.Remove(i);
            Debug.WriteLine(sf);
            byte[] data = Convert.FromBase64String(sf);
            int acc = 0;
            int dacc = 0;
            byte[] dData = new byte[1024*18];
            while(acc != data.Length)
            {
                if(acc<9)
                {
                    dData[dacc++] = data[acc++];
                    continue;
                }

                switch(data[acc])
                {
                    case 0x81:
                    case 0x7F:
                        byte sentinel = data[acc];
                        byte rcount = data[acc+1];
                        byte rval = data[acc+2];

                        if ( 0 == rcount )
                        {
                            if(0x7f == sentinel)
                            {
                                if (0x81 == rval)
                                {
                                    acc += 3;
                                }
                            }
                            else
                            {
                                if (0x81 == sentinel)
                                {
                                    if (0x7f == rval)
                                    {
                                        acc += 3;
                                    }
                                }
                            }
            
                            dData[dacc++] = data[acc++];
                        }
                        else
                        {
                            
                            if (rcount < 0x0f )
                            {
                                while( 0 != rcount)
                                {
                                    dData[dacc++] = rval;
                                    rcount--;
                                }

                                acc += 3;
                            }
                            else
                            {
                                dData[dacc++] = data[acc++];
                            }
                        }

                        break;

                    default:
                        dData[dacc++] = data[acc++];
                        break;
                }
            }
            //Read the contents of the file into a stream
            FileStream fs = File.Create("Data.txt");

            using (BinaryWriter writer = new BinaryWriter(fs))
            {   
                writer.Write(data,0,data.Length);
            }

            //Read the contents of the file into a stream
            fs = File.Create("DData.txt");

            using (BinaryWriter writer = new BinaryWriter(fs))
            {
                writer.Write(dData, 0, dacc);
            }

            Protocol pkt = new Protocol();
            int pos = 0x09;
            byte[] cabin = new byte[12];
            byte[] size = new byte[4];
            Array.ConstrainedCopy(dData, 5, size, 0, 4);
            int total = MyConvert.ByteToInt32(size, 0);
            acc = 0;

            while (pos < total)
            {
                //Buffer.BlockCopy(dData, 0, cabin, 0, 12);
                Array.ConstrainedCopy(dData, pos, cabin, 0, 12);
                pkt.SetData(cabin);
                float rad = pkt.Radian;
                float dist = pkt.Distance;
                bool valid = pkt.Valid;

                pos += 12;

                tboxOutput.Text += rad.ToString() + " " + dist.ToString() + " " + valid.ToString() + "\r\n";
                
            }


        }
        #endregion

        private void ControlTask()
        {
            while (true)
            {
                evtCtrl.WaitOne();

                switch( g_State )
                {
                    case TCP_CONNECT:
                        // Connect to the remote endpoint.  
                        client.BeginConnect(remoteEP,
                        new AsyncCallback(ConnectCallback), client);
                        evtWait.WaitOne();
                        break;

                    case TCP_SCAN:
                        g_nTcpReqId++;
                        String str = "{\"args\":null,\"command\":\"getlaserscan\",\"request_id\":" + g_nTcpReqId.ToString() + "}\r\n\r\n";
                        Debug.WriteLine(str);
                        // Begin receiving the data from the remote device.  
                        client.BeginReceive(RxTcpBuf, 0, RxTcpBuf.Length, 0,
                            new AsyncCallback(ReceiveCallback), client);

                        // Begin sending the data to the remote device.  
                        client.BeginSend(Encoding.ASCII.GetBytes(str), 0, str.Length, 0,
                            new AsyncCallback(SendCallback), client);
                        evtWait.WaitOne();
                        break;
                    default:
                        break;
                }
            }
        }


        private void DataTask()
        {
            while (true)
            {
                evtDataReady.WaitOne();

                String str = Encoding.UTF8.GetString(RxTcpBuf, 0, bytesRead);
                Debug.WriteLine(str);

                String s1 = "\"laser_points\":\"";
                String s2 = null;
                int i = 0;
                bool r = str.Contains(s1);
                if (true == r)
                {
                    i = str.IndexOf(s1) + s1.Length;
                    s2 = str.Substring(i);
                }

                string tok = "\"";
                i = s2.IndexOf(tok);
                string sf = s2.Remove(i);
                Debug.WriteLine(sf);
                byte[] data = Convert.FromBase64String(sf);
                int acc = 0;
                int dacc = 0;
                byte[] dData = new byte[1024 * 18];
                while (acc != data.Length)
                {
                    if (acc < 9)
                    {
                        dData[dacc++] = data[acc++];
                        continue;
                    }

                    switch (data[acc])
                    {
                        case 0x81:
                        case 0x7F:
                            byte sentinel = data[acc];
                            byte rcount = data[acc + 1];
                            byte rval = data[acc + 2];

                            if (0 == rcount)
                            {
                                if (0x7f == sentinel)
                                {
                                    if (0x81 == rval)
                                    {
                                        acc += 3;
                                    }
                                }
                                else
                                {
                                    if (0x81 == sentinel)
                                    {
                                        if (0x7f == rval)
                                        {
                                            acc += 3;
                                        }
                                    }
                                }

                                dData[dacc++] = data[acc++];
                            }
                            else
                            {

                                if (rcount < 0x0f)
                                {
                                    if ( 0!= rval)
                                    {
                                        dData[dacc++] = data[acc++];
                                        Debug.Write("Something wrong\r\n");
                                        continue;
                                    }
                                    while (0 != rcount)
                                    {
                                        dData[dacc++] = rval;
                                        rcount--;
                                    }

                                    acc += 3;
                                }
                                else
                                {
                                    dData[dacc++] = data[acc++];
                                }
                            }

                            break;

                        default:
                            dData[dacc++] = data[acc++];
                            break;
                    }
                }
#if false
                //Read the contents of the file into a stream
                FileStream fs = File.Create("Data.txt");

                using (BinaryWriter writer = new BinaryWriter(fs))
                {
                    writer.Write(data, 0, data.Length);
                }

                //Read the contents of the file into a stream
                fs = File.Create("DData.txt");

                using (BinaryWriter writer = new BinaryWriter(fs))
                {
                    writer.Write(dData, 0, dacc);
                }
#endif
                Protocol pkt = new Protocol();
                int pos = 0x09;
                byte[] cabin = new byte[12];
                byte[] size = new byte[4];
                Array.ConstrainedCopy(dData, 5, size, 0, 4);
                int total = MyConvert.ByteToInt32(size, 0);
                acc = 0;
#if false
                fs = File.Create("cabin.txt");

                byte[] terminate = new byte[2];
                terminate[0] = 0x0a;
                terminate[1] = 0x0d;
#endif
                while (pos < total)
                {
                    //Buffer.BlockCopy(dData, 0, cabin, 0, 12);
                    Array.ConstrainedCopy(dData, pos, cabin, 0, 12);
                    pkt.SetData(cabin);
                    float rad = pkt.Radian;
                    float dist = pkt.Distance;
                    bool valid = pkt.Valid;

#if false
                    fs.Write(cabin, 0, 12);
                   fs.Write(terminate,0,2);
                    
#endif 
                    pos += 12;

                    Invoke(new DELEGATE_DATA_UPDATE(DataUpdate), rad, dist, valid );
                }

#if false
                fs.Close();
#endif
            }
        }


        private void btnConnect_Click(object sender, EventArgs e)
        {

            if( "Connect" == btnConnect.Text )
            {
                btnConnect.Text = "Disconnect";

                IPAddress ip = IPAddress.Parse(tboxIP.Text);
                int port = Convert.ToInt32(tboxPort.Text);

                remoteEP = new IPEndPoint(ip, port);

                // Create a TCP/IP socket.  
                client = new Socket(ip.AddressFamily, SocketType.Stream, ProtocolType.Tcp);

                DataThread = new Thread(new ThreadStart(DataTask));
                DataThread.IsBackground = false;
                DataThread.Start();

                ControlThread = new Thread(new ThreadStart(ControlTask));
                ControlThread.IsBackground = false;
                ControlThread.Start();
                g_State = TCP_CONNECT;
                evtCtrl.Set();
            }
            else
            {
                if( null != ControlThread )
                {
                    ControlThread.Abort();
                }
                
                btnConnect.Text = "Connect";
            }

        }

        private void btnScan_Click(object sender, EventArgs e)
        {
            s1.Points.Clear();
            g_State = TCP_SCAN;
            evtCtrl.Set();
        }

        private void FormMain_Load(object sender, EventArgs e)
        {
            chart1.Series.Clear();

            ChartArea ca = chart1.ChartAreas[0];
            chart1.ChartAreas[0].AxisX.LineColor = Color.LightGray;
            chart1.ChartAreas[0].AxisX.MajorGrid.LineColor = Color.LightGray;
            chart1.ChartAreas[0].AxisY.LineColor = Color.LightGray;
            chart1.ChartAreas[0].AxisY.MajorGrid.LineColor = Color.LightGray;
            Axis ax = ca.AxisX;
            Axis ay = ca.AxisY;

            ax.Minimum = -180;
            ax.Maximum = 180;
            ax.Interval = 15;   // 15° interval
            ax.Crossing = 0;    // start the segments at the top!

            ay.Minimum = 0;
            ay.Maximum = 0.5;
            ay.Interval = 0.1;


            Series s0 = chart1.Series.Add("points");
            s0.MarkerStyle = MarkerStyle.Circle;
            s0.SetCustomProperty("PolarDrawingStyle", "Marker");
            s0.MarkerSize = 2;
            s0.MarkerColor = Color.LightGray;
            s0.Color = Color.LightGray;
            s0.ChartType = SeriesChartType.Polar;

            //for (double vx = ax.Minimum; vx < ax.Maximum; vx += ax.Interval)
            //{
            //    for (double vy = ay.Minimum; vy <= ay.Maximum; vy += ay.Interval)
            //    {
            //        //s0.Points.AddXY(vx, vy);
            //    }
            //}


            //s0.Points[333].MarkerColor = Color.Red;
            //s0.Points[333].MarkerSize = 12;
            s1 = chart1.Series.Add("real");
            s1.MarkerStyle = MarkerStyle.Circle;
            s1.SetCustomProperty("PolarDrawingStyle", "Marker");
            s1.MarkerSize = 2;
            s1.MarkerColor = Color.Blue;
            s1.ChartType = SeriesChartType.Polar;

            //s1.Points.AddXY(90, 5);
        }
    }
}
