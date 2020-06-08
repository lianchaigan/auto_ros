using System;
using System.IO;
using System.IO.Ports;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace SerialTerminal
{
    public partial class FormMain : Form
    {
        #region Constant
        private readonly int[] baudrate = { 9600, 19200, 38400, 115200, 230400, 460800, 921600, 3860000 };
        #endregion

        #region SerialPorts
        private SerialPort Serial = new SerialPort();
        #endregion

        #region THreading
        private Thread ControlThread;
        private AutoResetEvent evtSerialOnRx = new AutoResetEvent(false);
        #endregion

        #region LocalHelpers
        private void UpdateCOMPortList()
        {
            string[] Ports = System.IO.Ports.SerialPort.GetPortNames();
            cboxComport.Items.Clear();
            cboxBaudrate.Items.Clear();

            foreach (var item in Ports)
            {
                cboxComport.Items.Add(item);
            }

            foreach (var baud in baudrate)
            {
                cboxBaudrate.Items.Add(baud.ToString());
            }
        }

        private byte CheckSum(byte[] Data, int offset, int n)
        {
            int i = 0;
            byte CheckSum = 0x00;

            for (i = 0; i < n; i++)
            {
                CheckSum ^= Data[i + offset];
            }
            return CheckSum;
        }
        #endregion

        #region Handlers
        void SerialOnReceivedHandler(object sender, SerialDataReceivedEventArgs e)
        {
            evtSerialOnRx.Set();
        }
        #endregion

        #region Threading Tasks
        private void ControlTask()
        {
            while (true)
            {
                evtSerialOnRx.WaitOne();

                byte[] tmp = new byte[512];

                Serial.Read(tmp, 0, Serial.BytesToRead);
            }
        }

        #endregion

        public FormMain()
        {
            InitializeComponent();
        }

        private void FormMain_Load(object sender, EventArgs e)
        {
            UpdateCOMPortList();
        }

        private void btnRefresh_Click(object sender, EventArgs e)
        {
            UpdateCOMPortList();
        }

        private void btnConnect_Click(object sender, EventArgs e)
        {
            if ("Disconnect" == btnConnect.Text.ToString())
            {
                if (true == Serial.IsOpen)
                {
                    Serial.Close();
                }

                btnConnect.Text = "Connect";
                cboxComport.Enabled = true;
                cboxBaudrate.Enabled = true;
                btnRefresh.Enabled = true;

                sslabelOutput.Text = "Disconnected.";
                return;
            }

            // Get user comport from cbox
            try
            {
                Serial.PortName = cboxComport.Text;
            }
            catch
            {
                MessageBox.Show("Error! No COM Port selected");
                return;
            }

            // Get user baudrate from cbox
            try
            {
                Serial.BaudRate = int.Parse(cboxBaudrate.Text.ToString());
            }
            catch
            {
                MessageBox.Show("Error! No Baudrate selected");
                return;
            }

            // Serial Port Configuration
            Serial.Parity = Parity.None;
            Serial.DataBits = 8;
            Serial.ReceivedBytesThreshold = 1;
            Serial.StopBits = StopBits.One;
            Serial.Handshake = Handshake.None;
            Serial.WriteTimeout = 3000;

            // Check if com port is opened by other application
            if (false == Serial.IsOpen)
            {
                // Com port available
                Serial.Open();
                Serial.DataReceived += new SerialDataReceivedEventHandler(SerialOnReceivedHandler);

                btnConnect.Text = "Disconnect";
                cboxComport.Enabled = false;
                cboxBaudrate.Enabled = false;
                btnRefresh.Enabled = false;

                // double comform it is opened
                if (true == Serial.IsOpen)
                {
                    ControlThread = new Thread(new ThreadStart(ControlTask));
                    ControlThread.IsBackground = false;
                    ControlThread.Start();

                    sslabelOutput.Text = "Connected.";
                }
                else
                {
                    MessageBox.Show(" Error! Unable to open Serial port ");
                }
            }
            else
            {
                MessageBox.Show(" Error! Selected Serial port in use");
            }
        }

        private void btnSend_Click(object sender, EventArgs e)
        {
            byte[] data = new byte[6];

            for(int i=0;i<255; i++)
            {
                data[0] = (byte)i;
                data[1] = 1;
                data[2] = 0x40;
                data[3] = CheckSum(data, 0, 3);

                if (true == Serial.IsOpen)
                {
                    Serial.Write(data, 0, 4);
                }

                Thread.Sleep(1000);
            }

        }
    }
}
