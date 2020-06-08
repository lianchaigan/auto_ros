namespace TCPClient
{
    partial class FormMain
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea1 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            this.btnConnect = new System.Windows.Forms.Button();
            this.tboxIP = new System.Windows.Forms.TextBox();
            this.tboxPort = new System.Windows.Forms.TextBox();
            this.GBConnect = new System.Windows.Forms.GroupBox();
            this.tboxOutput = new System.Windows.Forms.TextBox();
            this.btnScan = new System.Windows.Forms.Button();
            this.chart1 = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.GBConnect.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.chart1)).BeginInit();
            this.SuspendLayout();
            // 
            // btnConnect
            // 
            this.btnConnect.Location = new System.Drawing.Point(158, 18);
            this.btnConnect.Name = "btnConnect";
            this.btnConnect.Size = new System.Drawing.Size(75, 23);
            this.btnConnect.TabIndex = 0;
            this.btnConnect.Text = "Connect";
            this.btnConnect.UseVisualStyleBackColor = true;
            this.btnConnect.Click += new System.EventHandler(this.btnConnect_Click);
            // 
            // tboxIP
            // 
            this.tboxIP.Location = new System.Drawing.Point(19, 21);
            this.tboxIP.Name = "tboxIP";
            this.tboxIP.Size = new System.Drawing.Size(100, 20);
            this.tboxIP.TabIndex = 1;
            // 
            // tboxPort
            // 
            this.tboxPort.Location = new System.Drawing.Point(19, 47);
            this.tboxPort.Name = "tboxPort";
            this.tboxPort.Size = new System.Drawing.Size(100, 20);
            this.tboxPort.TabIndex = 2;
            // 
            // GBConnect
            // 
            this.GBConnect.Controls.Add(this.tboxPort);
            this.GBConnect.Controls.Add(this.btnConnect);
            this.GBConnect.Controls.Add(this.tboxIP);
            this.GBConnect.Location = new System.Drawing.Point(4, 12);
            this.GBConnect.Name = "GBConnect";
            this.GBConnect.Size = new System.Drawing.Size(272, 78);
            this.GBConnect.TabIndex = 3;
            this.GBConnect.TabStop = false;
            this.GBConnect.Text = "Connection";
            // 
            // tboxOutput
            // 
            this.tboxOutput.Location = new System.Drawing.Point(4, 198);
            this.tboxOutput.Multiline = true;
            this.tboxOutput.Name = "tboxOutput";
            this.tboxOutput.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.tboxOutput.Size = new System.Drawing.Size(272, 269);
            this.tboxOutput.TabIndex = 4;
            // 
            // btnScan
            // 
            this.btnScan.Location = new System.Drawing.Point(162, 96);
            this.btnScan.Name = "btnScan";
            this.btnScan.Size = new System.Drawing.Size(75, 23);
            this.btnScan.TabIndex = 3;
            this.btnScan.Text = "Scan";
            this.btnScan.UseVisualStyleBackColor = true;
            this.btnScan.Click += new System.EventHandler(this.btnScan_Click);
            // 
            // chart1
            // 
            chartArea1.Name = "ChartArea1";
            this.chart1.ChartAreas.Add(chartArea1);
            this.chart1.Location = new System.Drawing.Point(282, 12);
            this.chart1.Name = "chart1";
            this.chart1.Size = new System.Drawing.Size(496, 455);
            this.chart1.TabIndex = 5;
            this.chart1.Text = "chart1";
            // 
            // FormMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(790, 485);
            this.Controls.Add(this.chart1);
            this.Controls.Add(this.btnScan);
            this.Controls.Add(this.tboxOutput);
            this.Controls.Add(this.GBConnect);
            this.Name = "FormMain";
            this.Text = "TCP Client";
            this.Load += new System.EventHandler(this.FormMain_Load);
            this.GBConnect.ResumeLayout(false);
            this.GBConnect.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.chart1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnConnect;
        private System.Windows.Forms.TextBox tboxIP;
        private System.Windows.Forms.TextBox tboxPort;
        private System.Windows.Forms.GroupBox GBConnect;
        private System.Windows.Forms.TextBox tboxOutput;
        private System.Windows.Forms.Button btnScan;
        private System.Windows.Forms.DataVisualization.Charting.Chart chart1;
    }
}

