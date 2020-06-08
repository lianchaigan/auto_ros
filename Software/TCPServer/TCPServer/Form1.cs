using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;
using SimpleTCP;

namespace TCPServer
{
    public partial class FormMain : Form
    {
        SimpleTcpServer server;

        private void Server_DataReceived(object sender, SimpleTCP.Message e)
        {
        }

        public FormMain()
        {
            InitializeComponent();

            server = new SimpleTcpServer();
            server.Delimiter = 0x13;//enter
            server.StringEncoder = Encoding.UTF8;
            server.DataReceived += Server_DataReceived;
        }

        private void FormMain_Load(object sender, EventArgs e)
        {

        }

        private void btnStart_Click(object sender, EventArgs e)
        {
            IPAddress ip = IPAddress.Parse(tboxIP.Text);

            if ("Start" == btnStart.Text)
            {
                btnStart.Text = "Stop";

                try
                {
                    server.Start(ip, Convert.ToInt32(tboxPort.Text));
                }
                catch (Exception)
                {
                    MessageBox.Show("Server cannot start. Check your IP address");
                    return;
                }
            }
            else
            {
                server.Stop();
                btnStart.Text = "Start";
            }
        }
    }
}
