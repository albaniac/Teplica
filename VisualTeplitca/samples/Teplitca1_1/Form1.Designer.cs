namespace GuiExample
{
    partial class Form1
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
            this.components = new System.ComponentModel.Container();
            this.TabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage3 = new System.Windows.Forms.TabPage();
            this.groupBox7 = new System.Windows.Forms.GroupBox();
            this.groupBox6 = new System.Windows.Forms.GroupBox();
            this.tabPage4 = new System.Windows.Forms.TabPage();
            this.tabPage5 = new System.Windows.Forms.TabPage();
            this.TabPage1 = new System.Windows.Forms.TabPage();
            this.lblResult = new System.Windows.Forms.Label();
            this.Label13 = new System.Windows.Forms.Label();
            this.GroupBox3 = new System.Windows.Forms.GroupBox();
            this.cmdOpenTCP = new System.Windows.Forms.Button();
            this.Label12 = new System.Windows.Forms.Label();
            this.cmbTcpProtocol = new System.Windows.Forms.ComboBox();
            this.Label11 = new System.Windows.Forms.Label();
            this.txtHostName = new System.Windows.Forms.TextBox();
            this.Label10 = new System.Windows.Forms.Label();
            this.txtTCPPort = new System.Windows.Forms.TextBox();
            this.GroupBox2 = new System.Windows.Forms.GroupBox();
            this.cmbDataBits = new System.Windows.Forms.ComboBox();
            this.cmbStopBits = new System.Windows.Forms.ComboBox();
            this.Label8 = new System.Windows.Forms.Label();
            this.Label7 = new System.Windows.Forms.Label();
            this.Label6 = new System.Windows.Forms.Label();
            this.cmbParity = new System.Windows.Forms.ComboBox();
            this.Label3 = new System.Windows.Forms.Label();
            this.Label2 = new System.Windows.Forms.Label();
            this.cmbBaudRate = new System.Windows.Forms.ComboBox();
            this.cmbComPort = new System.Windows.Forms.ComboBox();
            this.Label1 = new System.Windows.Forms.Label();
            this.cmdOpenSerial = new System.Windows.Forms.Button();
            this.cmbSerialProtocol = new System.Windows.Forms.ComboBox();
            this.GroupBox1 = new System.Windows.Forms.GroupBox();
            this.cmbRetry = new System.Windows.Forms.ComboBox();
            this.Label4 = new System.Windows.Forms.Label();
            this.Label5 = new System.Windows.Forms.Label();
            this.txtTimeout = new System.Windows.Forms.TextBox();
            this.txtPollDelay = new System.Windows.Forms.TextBox();
            this.Label9 = new System.Windows.Forms.Label();
            this.TabPage2 = new System.Windows.Forms.TabPage();
            this.Label19 = new System.Windows.Forms.Label();
            this.txtNumCoils = new System.Windows.Forms.TextBox();
            this.Label20 = new System.Windows.Forms.Label();
            this.txtStartCoil = new System.Windows.Forms.TextBox();
            this.Label17 = new System.Windows.Forms.Label();
            this.txtNumRdRegs = new System.Windows.Forms.TextBox();
            this.Label18 = new System.Windows.Forms.Label();
            this.txtStartRdReg = new System.Windows.Forms.TextBox();
            this.cmbCommand = new System.Windows.Forms.ComboBox();
            this.cmdExecute = new System.Windows.Forms.Button();
            this.Label16 = new System.Windows.Forms.Label();
            this.txtNumWrRegs = new System.Windows.Forms.TextBox();
            this.GroupBox5 = new System.Windows.Forms.GroupBox();
            this.txtWriteVal8 = new System.Windows.Forms.TextBox();
            this.txtWriteVal7 = new System.Windows.Forms.TextBox();
            this.txtWriteVal6 = new System.Windows.Forms.TextBox();
            this.txtWriteVal5 = new System.Windows.Forms.TextBox();
            this.txtWriteVal4 = new System.Windows.Forms.TextBox();
            this.txtWriteVal3 = new System.Windows.Forms.TextBox();
            this.txtWriteVal2 = new System.Windows.Forms.TextBox();
            this.txtWriteVal1 = new System.Windows.Forms.TextBox();
            this.GroupBox4 = new System.Windows.Forms.GroupBox();
            this.lblReadValues = new System.Windows.Forms.Label();
            this.lblResult2 = new System.Windows.Forms.Label();
            this.Label15 = new System.Windows.Forms.Label();
            this.txtStartWrReg = new System.Windows.Forms.TextBox();
            this.Label14 = new System.Windows.Forms.Label();
            this.txtSlave = new System.Windows.Forms.TextBox();
            this.ToolTip1 = new System.Windows.Forms.ToolTip(this.components);
            this.dateTimePicker1 = new System.Windows.Forms.DateTimePicker();
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.label21 = new System.Windows.Forms.Label();
            this.label22 = new System.Windows.Forms.Label();
            this.TabControl1.SuspendLayout();
            this.tabPage3.SuspendLayout();
            this.groupBox6.SuspendLayout();
            this.TabPage1.SuspendLayout();
            this.GroupBox3.SuspendLayout();
            this.GroupBox2.SuspendLayout();
            this.GroupBox1.SuspendLayout();
            this.TabPage2.SuspendLayout();
            this.GroupBox5.SuspendLayout();
            this.GroupBox4.SuspendLayout();
            this.SuspendLayout();
            // 
            // TabControl1
            // 
            this.TabControl1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.TabControl1.Controls.Add(this.tabPage3);
            this.TabControl1.Controls.Add(this.tabPage4);
            this.TabControl1.Controls.Add(this.tabPage5);
            this.TabControl1.Controls.Add(this.TabPage1);
            this.TabControl1.Controls.Add(this.TabPage2);
            this.TabControl1.Location = new System.Drawing.Point(0, 0);
            this.TabControl1.Name = "TabControl1";
            this.TabControl1.SelectedIndex = 0;
            this.TabControl1.Size = new System.Drawing.Size(1295, 869);
            this.TabControl1.TabIndex = 0;
            // 
            // tabPage3
            // 
            this.tabPage3.BackColor = System.Drawing.SystemColors.Control;
            this.tabPage3.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.tabPage3.Controls.Add(this.statusStrip1);
            this.tabPage3.Controls.Add(this.dateTimePicker1);
            this.tabPage3.Controls.Add(this.groupBox7);
            this.tabPage3.Controls.Add(this.groupBox6);
            this.tabPage3.Location = new System.Drawing.Point(4, 22);
            this.tabPage3.Name = "tabPage3";
            this.tabPage3.Size = new System.Drawing.Size(1287, 843);
            this.tabPage3.TabIndex = 2;
            this.tabPage3.Text = "Информация о теплице";
            // 
            // groupBox7
            // 
            this.groupBox7.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
            this.groupBox7.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Italic))), System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.groupBox7.Location = new System.Drawing.Point(660, 22);
            this.groupBox7.Name = "groupBox7";
            this.groupBox7.Size = new System.Drawing.Size(600, 700);
            this.groupBox7.TabIndex = 1;
            this.groupBox7.TabStop = false;
            this.groupBox7.Text = "Секция №2";
            // 
            // groupBox6
            // 
            this.groupBox6.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
            this.groupBox6.Controls.Add(this.label22);
            this.groupBox6.Controls.Add(this.label21);
            this.groupBox6.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Italic))), System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.groupBox6.Location = new System.Drawing.Point(28, 22);
            this.groupBox6.Name = "groupBox6";
            this.groupBox6.Size = new System.Drawing.Size(600, 700);
            this.groupBox6.TabIndex = 0;
            this.groupBox6.TabStop = false;
            this.groupBox6.Text = "Секция №1";
            this.groupBox6.Enter += new System.EventHandler(this.groupBox6_Enter);
            // 
            // tabPage4
            // 
            this.tabPage4.BackColor = System.Drawing.SystemColors.Control;
            this.tabPage4.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.tabPage4.Location = new System.Drawing.Point(4, 22);
            this.tabPage4.Name = "tabPage4";
            this.tabPage4.Size = new System.Drawing.Size(1287, 843);
            this.tabPage4.TabIndex = 3;
            this.tabPage4.Text = "Управление теплицей";
            // 
            // tabPage5
            // 
            this.tabPage5.BackColor = System.Drawing.SystemColors.Control;
            this.tabPage5.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.tabPage5.Location = new System.Drawing.Point(4, 22);
            this.tabPage5.Name = "tabPage5";
            this.tabPage5.Size = new System.Drawing.Size(1287, 843);
            this.tabPage5.TabIndex = 4;
            this.tabPage5.Text = "Установки";
            // 
            // TabPage1
            // 
            this.TabPage1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.TabPage1.Controls.Add(this.lblResult);
            this.TabPage1.Controls.Add(this.Label13);
            this.TabPage1.Controls.Add(this.GroupBox3);
            this.TabPage1.Controls.Add(this.GroupBox2);
            this.TabPage1.Controls.Add(this.GroupBox1);
            this.TabPage1.Location = new System.Drawing.Point(4, 22);
            this.TabPage1.Name = "TabPage1";
            this.TabPage1.Size = new System.Drawing.Size(1287, 843);
            this.TabPage1.TabIndex = 0;
            this.TabPage1.Text = "Configuration";
            // 
            // lblResult
            // 
            this.lblResult.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.lblResult.Location = new System.Drawing.Point(6, 276);
            this.lblResult.Name = "lblResult";
            this.lblResult.Size = new System.Drawing.Size(1009, 550);
            this.lblResult.TabIndex = 15;
            this.lblResult.Text = "Result:";
            // 
            // Label13
            // 
            this.Label13.Location = new System.Drawing.Point(6, 242);
            this.Label13.Name = "Label13";
            this.Label13.Size = new System.Drawing.Size(585, 30);
            this.Label13.TabIndex = 14;
            this.Label13.Text = "All options must be selected BEFORE opening the port. Once port is opened, no cha" +
    "nges can be made unless port is re-opened first.";
            // 
            // GroupBox3
            // 
            this.GroupBox3.Controls.Add(this.cmdOpenTCP);
            this.GroupBox3.Controls.Add(this.Label12);
            this.GroupBox3.Controls.Add(this.cmbTcpProtocol);
            this.GroupBox3.Controls.Add(this.Label11);
            this.GroupBox3.Controls.Add(this.txtHostName);
            this.GroupBox3.Controls.Add(this.Label10);
            this.GroupBox3.Controls.Add(this.txtTCPPort);
            this.GroupBox3.Location = new System.Drawing.Point(194, 6);
            this.GroupBox3.Name = "GroupBox3";
            this.GroupBox3.Size = new System.Drawing.Size(208, 227);
            this.GroupBox3.TabIndex = 13;
            this.GroupBox3.TabStop = false;
            this.GroupBox3.Text = "MODBUS/TCP";
            // 
            // cmdOpenTCP
            // 
            this.cmdOpenTCP.Location = new System.Drawing.Point(61, 194);
            this.cmdOpenTCP.Name = "cmdOpenTCP";
            this.cmdOpenTCP.Size = new System.Drawing.Size(95, 22);
            this.cmdOpenTCP.TabIndex = 15;
            this.cmdOpenTCP.Text = "Open TCP";
            this.cmdOpenTCP.Click += new System.EventHandler(this.cmdOpenTCP_Click);
            // 
            // Label12
            // 
            this.Label12.Location = new System.Drawing.Point(10, 83);
            this.Label12.Name = "Label12";
            this.Label12.Size = new System.Drawing.Size(56, 18);
            this.Label12.TabIndex = 14;
            this.Label12.Text = "Protocol:";
            // 
            // cmbTcpProtocol
            // 
            this.cmbTcpProtocol.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbTcpProtocol.Items.AddRange(new object[] {
            "Modbus/TCP",
            "Encapsulated RTU"});
            this.cmbTcpProtocol.Location = new System.Drawing.Point(76, 82);
            this.cmbTcpProtocol.Name = "cmbTcpProtocol";
            this.cmbTcpProtocol.Size = new System.Drawing.Size(115, 21);
            this.cmbTcpProtocol.TabIndex = 13;
            // 
            // Label11
            // 
            this.Label11.Location = new System.Drawing.Point(10, 54);
            this.Label11.Name = "Label11";
            this.Label11.Size = new System.Drawing.Size(64, 20);
            this.Label11.TabIndex = 12;
            this.Label11.Text = "IP Address:";
            // 
            // txtHostName
            // 
            this.txtHostName.Location = new System.Drawing.Point(76, 53);
            this.txtHostName.Name = "txtHostName";
            this.txtHostName.Size = new System.Drawing.Size(113, 20);
            this.txtHostName.TabIndex = 11;
            this.txtHostName.Text = "127.0.0.1";
            // 
            // Label10
            // 
            this.Label10.Location = new System.Drawing.Point(10, 24);
            this.Label10.Name = "Label10";
            this.Label10.Size = new System.Drawing.Size(60, 20);
            this.Label10.TabIndex = 10;
            this.Label10.Text = "TCP Port:";
            // 
            // txtTCPPort
            // 
            this.txtTCPPort.Location = new System.Drawing.Point(76, 23);
            this.txtTCPPort.Name = "txtTCPPort";
            this.txtTCPPort.Size = new System.Drawing.Size(66, 20);
            this.txtTCPPort.TabIndex = 9;
            this.txtTCPPort.Text = "502";
            // 
            // GroupBox2
            // 
            this.GroupBox2.Controls.Add(this.cmbDataBits);
            this.GroupBox2.Controls.Add(this.cmbStopBits);
            this.GroupBox2.Controls.Add(this.Label8);
            this.GroupBox2.Controls.Add(this.Label7);
            this.GroupBox2.Controls.Add(this.Label6);
            this.GroupBox2.Controls.Add(this.cmbParity);
            this.GroupBox2.Controls.Add(this.Label3);
            this.GroupBox2.Controls.Add(this.Label2);
            this.GroupBox2.Controls.Add(this.cmbBaudRate);
            this.GroupBox2.Controls.Add(this.cmbComPort);
            this.GroupBox2.Controls.Add(this.Label1);
            this.GroupBox2.Controls.Add(this.cmdOpenSerial);
            this.GroupBox2.Controls.Add(this.cmbSerialProtocol);
            this.GroupBox2.Location = new System.Drawing.Point(12, 6);
            this.GroupBox2.Name = "GroupBox2";
            this.GroupBox2.Size = new System.Drawing.Size(166, 228);
            this.GroupBox2.TabIndex = 5;
            this.GroupBox2.TabStop = false;
            this.GroupBox2.Text = "Serial Modbus";
            // 
            // cmbDataBits
            // 
            this.cmbDataBits.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbDataBits.Items.AddRange(new object[] {
            "8",
            "7"});
            this.cmbDataBits.Location = new System.Drawing.Point(82, 134);
            this.cmbDataBits.Name = "cmbDataBits";
            this.cmbDataBits.Size = new System.Drawing.Size(71, 21);
            this.cmbDataBits.TabIndex = 13;
            // 
            // cmbStopBits
            // 
            this.cmbStopBits.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbStopBits.Items.AddRange(new object[] {
            "1",
            "2"});
            this.cmbStopBits.Location = new System.Drawing.Point(82, 161);
            this.cmbStopBits.Name = "cmbStopBits";
            this.cmbStopBits.Size = new System.Drawing.Size(71, 21);
            this.cmbStopBits.TabIndex = 12;
            // 
            // Label8
            // 
            this.Label8.Location = new System.Drawing.Point(10, 135);
            this.Label8.Name = "Label8";
            this.Label8.Size = new System.Drawing.Size(63, 18);
            this.Label8.TabIndex = 11;
            this.Label8.Text = "Databits:";
            // 
            // Label7
            // 
            this.Label7.Location = new System.Drawing.Point(10, 162);
            this.Label7.Name = "Label7";
            this.Label7.Size = new System.Drawing.Size(48, 18);
            this.Label7.TabIndex = 10;
            this.Label7.Text = "Stopbits:";
            // 
            // Label6
            // 
            this.Label6.Location = new System.Drawing.Point(10, 108);
            this.Label6.Name = "Label6";
            this.Label6.Size = new System.Drawing.Size(48, 18);
            this.Label6.TabIndex = 9;
            this.Label6.Text = "Parity:";
            // 
            // cmbParity
            // 
            this.cmbParity.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbParity.Items.AddRange(new object[] {
            "None",
            "Even",
            "Odd"});
            this.cmbParity.Location = new System.Drawing.Point(82, 107);
            this.cmbParity.Name = "cmbParity";
            this.cmbParity.Size = new System.Drawing.Size(71, 21);
            this.cmbParity.TabIndex = 8;
            // 
            // Label3
            // 
            this.Label3.Location = new System.Drawing.Point(10, 81);
            this.Label3.Name = "Label3";
            this.Label3.Size = new System.Drawing.Size(56, 18);
            this.Label3.TabIndex = 7;
            this.Label3.Text = "Protocol:";
            // 
            // Label2
            // 
            this.Label2.Location = new System.Drawing.Point(10, 54);
            this.Label2.Name = "Label2";
            this.Label2.Size = new System.Drawing.Size(63, 18);
            this.Label2.TabIndex = 5;
            this.Label2.Text = "Baud Rate:";
            // 
            // cmbBaudRate
            // 
            this.cmbBaudRate.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbBaudRate.Items.AddRange(new object[] {
            "9600",
            "14400",
            "19200",
            "38400",
            "56000",
            "57600",
            "115200",
            "125000",
            "128000",
            "256000"});
            this.cmbBaudRate.Location = new System.Drawing.Point(82, 53);
            this.cmbBaudRate.Name = "cmbBaudRate";
            this.cmbBaudRate.Size = new System.Drawing.Size(71, 21);
            this.cmbBaudRate.TabIndex = 4;
            // 
            // cmbComPort
            // 
            this.cmbComPort.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbComPort.Items.AddRange(new object[] {
            "COM1",
            "COM2",
            "COM3",
            "COM4",
            "COM5",
            "COM6",
            "COM7",
            "COM8",
            "COM9"});
            this.cmbComPort.Location = new System.Drawing.Point(82, 26);
            this.cmbComPort.Name = "cmbComPort";
            this.cmbComPort.Size = new System.Drawing.Size(71, 21);
            this.cmbComPort.TabIndex = 3;
            // 
            // Label1
            // 
            this.Label1.Location = new System.Drawing.Point(10, 27);
            this.Label1.Name = "Label1";
            this.Label1.Size = new System.Drawing.Size(56, 18);
            this.Label1.TabIndex = 1;
            this.Label1.Text = "COM Port:";
            // 
            // cmdOpenSerial
            // 
            this.cmdOpenSerial.Location = new System.Drawing.Point(34, 192);
            this.cmdOpenSerial.Name = "cmdOpenSerial";
            this.cmdOpenSerial.Size = new System.Drawing.Size(95, 22);
            this.cmdOpenSerial.TabIndex = 2;
            this.cmdOpenSerial.Text = "Open Serial";
            this.cmdOpenSerial.Click += new System.EventHandler(this.cmdOpenSerial_Click);
            // 
            // cmbSerialProtocol
            // 
            this.cmbSerialProtocol.DisplayMember = "1";
            this.cmbSerialProtocol.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbSerialProtocol.Items.AddRange(new object[] {
            "RTU",
            "ASCII"});
            this.cmbSerialProtocol.Location = new System.Drawing.Point(82, 80);
            this.cmbSerialProtocol.Name = "cmbSerialProtocol";
            this.cmbSerialProtocol.Size = new System.Drawing.Size(71, 21);
            this.cmbSerialProtocol.TabIndex = 6;
            // 
            // GroupBox1
            // 
            this.GroupBox1.Controls.Add(this.cmbRetry);
            this.GroupBox1.Controls.Add(this.Label4);
            this.GroupBox1.Controls.Add(this.Label5);
            this.GroupBox1.Controls.Add(this.txtTimeout);
            this.GroupBox1.Controls.Add(this.txtPollDelay);
            this.GroupBox1.Controls.Add(this.Label9);
            this.GroupBox1.Location = new System.Drawing.Point(418, 6);
            this.GroupBox1.Name = "GroupBox1";
            this.GroupBox1.Size = new System.Drawing.Size(184, 228);
            this.GroupBox1.TabIndex = 4;
            this.GroupBox1.TabStop = false;
            this.GroupBox1.Text = "Common Options";
            // 
            // cmbRetry
            // 
            this.cmbRetry.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbRetry.Items.AddRange(new object[] {
            "0",
            "1",
            "2",
            "3"});
            this.cmbRetry.Location = new System.Drawing.Point(76, 58);
            this.cmbRetry.Name = "cmbRetry";
            this.cmbRetry.Size = new System.Drawing.Size(66, 21);
            this.cmbRetry.TabIndex = 9;
            // 
            // Label4
            // 
            this.Label4.Location = new System.Drawing.Point(10, 27);
            this.Label4.Name = "Label4";
            this.Label4.Size = new System.Drawing.Size(60, 20);
            this.Label4.TabIndex = 8;
            this.Label4.Text = "Time-out:";
            // 
            // Label5
            // 
            this.Label5.Location = new System.Drawing.Point(10, 61);
            this.Label5.Name = "Label5";
            this.Label5.Size = new System.Drawing.Size(51, 20);
            this.Label5.TabIndex = 10;
            this.Label5.Text = "Retries:";
            // 
            // txtTimeout
            // 
            this.txtTimeout.Location = new System.Drawing.Point(76, 26);
            this.txtTimeout.Name = "txtTimeout";
            this.txtTimeout.Size = new System.Drawing.Size(66, 20);
            this.txtTimeout.TabIndex = 0;
            this.txtTimeout.Text = "1000";
            // 
            // txtPollDelay
            // 
            this.txtPollDelay.Location = new System.Drawing.Point(76, 92);
            this.txtPollDelay.Name = "txtPollDelay";
            this.txtPollDelay.Size = new System.Drawing.Size(66, 20);
            this.txtPollDelay.TabIndex = 11;
            this.txtPollDelay.Text = "0";
            // 
            // Label9
            // 
            this.Label9.Location = new System.Drawing.Point(10, 93);
            this.Label9.Name = "Label9";
            this.Label9.Size = new System.Drawing.Size(71, 20);
            this.Label9.TabIndex = 12;
            this.Label9.Text = "Poll Delay:";
            // 
            // TabPage2
            // 
            this.TabPage2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.TabPage2.Controls.Add(this.Label19);
            this.TabPage2.Controls.Add(this.txtNumCoils);
            this.TabPage2.Controls.Add(this.Label20);
            this.TabPage2.Controls.Add(this.txtStartCoil);
            this.TabPage2.Controls.Add(this.Label17);
            this.TabPage2.Controls.Add(this.txtNumRdRegs);
            this.TabPage2.Controls.Add(this.Label18);
            this.TabPage2.Controls.Add(this.txtStartRdReg);
            this.TabPage2.Controls.Add(this.cmbCommand);
            this.TabPage2.Controls.Add(this.cmdExecute);
            this.TabPage2.Controls.Add(this.Label16);
            this.TabPage2.Controls.Add(this.txtNumWrRegs);
            this.TabPage2.Controls.Add(this.GroupBox5);
            this.TabPage2.Controls.Add(this.GroupBox4);
            this.TabPage2.Controls.Add(this.lblResult2);
            this.TabPage2.Controls.Add(this.Label15);
            this.TabPage2.Controls.Add(this.txtStartWrReg);
            this.TabPage2.Controls.Add(this.Label14);
            this.TabPage2.Controls.Add(this.txtSlave);
            this.TabPage2.Location = new System.Drawing.Point(4, 22);
            this.TabPage2.Name = "TabPage2";
            this.TabPage2.Size = new System.Drawing.Size(1287, 843);
            this.TabPage2.TabIndex = 1;
            this.TabPage2.Text = "Read/Write";
            // 
            // Label19
            // 
            this.Label19.Location = new System.Drawing.Point(462, 38);
            this.Label19.Name = "Label19";
            this.Label19.Size = new System.Drawing.Size(62, 17);
            this.Label19.TabIndex = 30;
            this.Label19.Text = "# of Coils:";
            // 
            // txtNumCoils
            // 
            this.txtNumCoils.Location = new System.Drawing.Point(465, 58);
            this.txtNumCoils.Name = "txtNumCoils";
            this.txtNumCoils.Size = new System.Drawing.Size(41, 20);
            this.txtNumCoils.TabIndex = 29;
            this.txtNumCoils.Text = "1";
            // 
            // Label20
            // 
            this.Label20.Location = new System.Drawing.Point(375, 38);
            this.Label20.Name = "Label20";
            this.Label20.Size = new System.Drawing.Size(81, 17);
            this.Label20.TabIndex = 28;
            this.Label20.Text = "Start Coil (r/w):";
            // 
            // txtStartCoil
            // 
            this.txtStartCoil.Location = new System.Drawing.Point(375, 58);
            this.txtStartCoil.Name = "txtStartCoil";
            this.txtStartCoil.Size = new System.Drawing.Size(41, 20);
            this.txtStartCoil.TabIndex = 27;
            this.txtStartCoil.Text = "1";
            // 
            // Label17
            // 
            this.Label17.Location = new System.Drawing.Point(102, 38);
            this.Label17.Name = "Label17";
            this.Label17.Size = new System.Drawing.Size(88, 17);
            this.Label17.TabIndex = 26;
            this.Label17.Text = "# of Regs (read):";
            // 
            // txtNumRdRegs
            // 
            this.txtNumRdRegs.Location = new System.Drawing.Point(106, 58);
            this.txtNumRdRegs.Name = "txtNumRdRegs";
            this.txtNumRdRegs.Size = new System.Drawing.Size(41, 20);
            this.txtNumRdRegs.TabIndex = 25;
            this.txtNumRdRegs.Text = "1";
            // 
            // Label18
            // 
            this.Label18.Location = new System.Drawing.Point(12, 38);
            this.Label18.Name = "Label18";
            this.Label18.Size = new System.Drawing.Size(91, 17);
            this.Label18.TabIndex = 24;
            this.Label18.Text = "Start Reg (read):";
            // 
            // txtStartRdReg
            // 
            this.txtStartRdReg.Location = new System.Drawing.Point(15, 58);
            this.txtStartRdReg.Name = "txtStartRdReg";
            this.txtStartRdReg.Size = new System.Drawing.Size(41, 20);
            this.txtStartRdReg.TabIndex = 23;
            this.txtStartRdReg.Text = "1";
            // 
            // cmbCommand
            // 
            this.cmbCommand.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbCommand.Items.AddRange(new object[] {
            "Read Holding Registers",
            "Read Input Registers",
            "Read Coils",
            "Write Coils",
            "Write Holding Registers",
            "Write Single Register",
            "Read/Write Registers"});
            this.cmbCommand.Location = new System.Drawing.Point(106, 10);
            this.cmbCommand.Name = "cmbCommand";
            this.cmbCommand.Size = new System.Drawing.Size(212, 21);
            this.cmbCommand.TabIndex = 22;
            // 
            // cmdExecute
            // 
            this.cmdExecute.Location = new System.Drawing.Point(12, 10);
            this.cmdExecute.Name = "cmdExecute";
            this.cmdExecute.Size = new System.Drawing.Size(86, 20);
            this.cmdExecute.TabIndex = 21;
            this.cmdExecute.Text = "Execute";
            this.cmdExecute.Click += new System.EventHandler(this.cmdExecute_Click);
            // 
            // Label16
            // 
            this.Label16.Location = new System.Drawing.Point(282, 38);
            this.Label16.Name = "Label16";
            this.Label16.Size = new System.Drawing.Size(90, 17);
            this.Label16.TabIndex = 20;
            this.Label16.Text = "# of Regs (write):";
            // 
            // txtNumWrRegs
            // 
            this.txtNumWrRegs.Location = new System.Drawing.Point(285, 58);
            this.txtNumWrRegs.Name = "txtNumWrRegs";
            this.txtNumWrRegs.Size = new System.Drawing.Size(41, 20);
            this.txtNumWrRegs.TabIndex = 19;
            this.txtNumWrRegs.Text = "1";
            // 
            // GroupBox5
            // 
            this.GroupBox5.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.GroupBox5.Controls.Add(this.txtWriteVal8);
            this.GroupBox5.Controls.Add(this.txtWriteVal7);
            this.GroupBox5.Controls.Add(this.txtWriteVal6);
            this.GroupBox5.Controls.Add(this.txtWriteVal5);
            this.GroupBox5.Controls.Add(this.txtWriteVal4);
            this.GroupBox5.Controls.Add(this.txtWriteVal3);
            this.GroupBox5.Controls.Add(this.txtWriteVal2);
            this.GroupBox5.Controls.Add(this.txtWriteVal1);
            this.GroupBox5.Location = new System.Drawing.Point(12, 757);
            this.GroupBox5.Name = "GroupBox5";
            this.GroupBox5.Size = new System.Drawing.Size(1003, 49);
            this.GroupBox5.TabIndex = 18;
            this.GroupBox5.TabStop = false;
            this.GroupBox5.Text = "Write values:";
            // 
            // txtWriteVal8
            // 
            this.txtWriteVal8.Location = new System.Drawing.Point(509, 17);
            this.txtWriteVal8.Name = "txtWriteVal8";
            this.txtWriteVal8.Size = new System.Drawing.Size(65, 20);
            this.txtWriteVal8.TabIndex = 9;
            this.txtWriteVal8.Text = "1";
            // 
            // txtWriteVal7
            // 
            this.txtWriteVal7.Location = new System.Drawing.Point(438, 17);
            this.txtWriteVal7.Name = "txtWriteVal7";
            this.txtWriteVal7.Size = new System.Drawing.Size(65, 20);
            this.txtWriteVal7.TabIndex = 8;
            this.txtWriteVal7.Text = "1";
            // 
            // txtWriteVal6
            // 
            this.txtWriteVal6.Location = new System.Drawing.Point(367, 17);
            this.txtWriteVal6.Name = "txtWriteVal6";
            this.txtWriteVal6.Size = new System.Drawing.Size(65, 20);
            this.txtWriteVal6.TabIndex = 7;
            this.txtWriteVal6.Text = "1";
            // 
            // txtWriteVal5
            // 
            this.txtWriteVal5.Location = new System.Drawing.Point(296, 17);
            this.txtWriteVal5.Name = "txtWriteVal5";
            this.txtWriteVal5.Size = new System.Drawing.Size(65, 20);
            this.txtWriteVal5.TabIndex = 6;
            this.txtWriteVal5.Text = "1";
            // 
            // txtWriteVal4
            // 
            this.txtWriteVal4.Location = new System.Drawing.Point(225, 17);
            this.txtWriteVal4.Name = "txtWriteVal4";
            this.txtWriteVal4.Size = new System.Drawing.Size(65, 20);
            this.txtWriteVal4.TabIndex = 5;
            this.txtWriteVal4.Text = "1";
            // 
            // txtWriteVal3
            // 
            this.txtWriteVal3.Location = new System.Drawing.Point(154, 17);
            this.txtWriteVal3.Name = "txtWriteVal3";
            this.txtWriteVal3.Size = new System.Drawing.Size(65, 20);
            this.txtWriteVal3.TabIndex = 4;
            this.txtWriteVal3.Text = "1";
            // 
            // txtWriteVal2
            // 
            this.txtWriteVal2.Location = new System.Drawing.Point(83, 17);
            this.txtWriteVal2.Name = "txtWriteVal2";
            this.txtWriteVal2.Size = new System.Drawing.Size(65, 20);
            this.txtWriteVal2.TabIndex = 3;
            this.txtWriteVal2.Text = "1";
            // 
            // txtWriteVal1
            // 
            this.txtWriteVal1.Location = new System.Drawing.Point(12, 17);
            this.txtWriteVal1.Name = "txtWriteVal1";
            this.txtWriteVal1.Size = new System.Drawing.Size(65, 20);
            this.txtWriteVal1.TabIndex = 2;
            this.txtWriteVal1.Text = "1";
            // 
            // GroupBox4
            // 
            this.GroupBox4.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.GroupBox4.Controls.Add(this.lblReadValues);
            this.GroupBox4.Location = new System.Drawing.Point(12, 88);
            this.GroupBox4.Name = "GroupBox4";
            this.GroupBox4.Size = new System.Drawing.Size(1003, 657);
            this.GroupBox4.TabIndex = 17;
            this.GroupBox4.TabStop = false;
            this.GroupBox4.Text = "Read values:";
            // 
            // lblReadValues
            // 
            this.lblReadValues.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.lblReadValues.Location = new System.Drawing.Point(7, 15);
            this.lblReadValues.Name = "lblReadValues";
            this.lblReadValues.Size = new System.Drawing.Size(990, 633);
            this.lblReadValues.TabIndex = 0;
            // 
            // lblResult2
            // 
            this.lblResult2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.lblResult2.Location = new System.Drawing.Point(12, 819);
            this.lblResult2.Name = "lblResult2";
            this.lblResult2.Size = new System.Drawing.Size(999, 20);
            this.lblResult2.TabIndex = 16;
            this.lblResult2.Text = "Result:";
            // 
            // Label15
            // 
            this.Label15.Location = new System.Drawing.Point(192, 38);
            this.Label15.Name = "Label15";
            this.Label15.Size = new System.Drawing.Size(99, 17);
            this.Label15.TabIndex = 3;
            this.Label15.Text = "Start Reg (write):";
            // 
            // txtStartWrReg
            // 
            this.txtStartWrReg.Location = new System.Drawing.Point(195, 58);
            this.txtStartWrReg.Name = "txtStartWrReg";
            this.txtStartWrReg.Size = new System.Drawing.Size(41, 20);
            this.txtStartWrReg.TabIndex = 2;
            this.txtStartWrReg.Text = "1";
            // 
            // Label14
            // 
            this.Label14.Location = new System.Drawing.Point(326, 14);
            this.Label14.Name = "Label14";
            this.Label14.Size = new System.Drawing.Size(38, 19);
            this.Label14.TabIndex = 1;
            this.Label14.Text = "Slave";
            // 
            // txtSlave
            // 
            this.txtSlave.Location = new System.Drawing.Point(367, 10);
            this.txtSlave.Name = "txtSlave";
            this.txtSlave.Size = new System.Drawing.Size(41, 20);
            this.txtSlave.TabIndex = 0;
            this.txtSlave.Text = "1";
            // 
            // dateTimePicker1
            // 
            this.dateTimePicker1.Location = new System.Drawing.Point(1060, 782);
            this.dateTimePicker1.Name = "dateTimePicker1";
            this.dateTimePicker1.Size = new System.Drawing.Size(200, 20);
            this.dateTimePicker1.TabIndex = 2;
            this.dateTimePicker1.ValueChanged += new System.EventHandler(this.dateTimePicker1_ValueChanged);
            // 
            // statusStrip1
            // 
            this.statusStrip1.Location = new System.Drawing.Point(0, 817);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Size = new System.Drawing.Size(1283, 22);
            this.statusStrip1.TabIndex = 3;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // label21
            // 
            this.label21.AutoSize = true;
            this.label21.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label21.Location = new System.Drawing.Point(26, 42);
            this.label21.Name = "label21";
            this.label21.Size = new System.Drawing.Size(212, 20);
            this.label21.TabIndex = 0;
            this.label21.Text = "Температура в помещении";
            this.label21.Click += new System.EventHandler(this.label21_Click);
            // 
            // label22
            // 
            this.label22.AutoSize = true;
            this.label22.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label22.Location = new System.Drawing.Point(26, 71);
            this.label22.Name = "label22";
            this.label22.Size = new System.Drawing.Size(202, 20);
            this.label22.TabIndex = 1;
            this.label22.Text = "Влажность  в помещении\r\n";
            // 
            // Form1
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(1292, 866);
            this.Controls.Add(this.TabControl1);
            this.Name = "Form1";
            this.Text = "Программа управления теплицей";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.TabControl1.ResumeLayout(false);
            this.tabPage3.ResumeLayout(false);
            this.tabPage3.PerformLayout();
            this.groupBox6.ResumeLayout(false);
            this.groupBox6.PerformLayout();
            this.TabPage1.ResumeLayout(false);
            this.GroupBox3.ResumeLayout(false);
            this.GroupBox3.PerformLayout();
            this.GroupBox2.ResumeLayout(false);
            this.GroupBox1.ResumeLayout(false);
            this.GroupBox1.PerformLayout();
            this.TabPage2.ResumeLayout(false);
            this.TabPage2.PerformLayout();
            this.GroupBox5.ResumeLayout(false);
            this.GroupBox5.PerformLayout();
            this.GroupBox4.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion
        
        private System.Windows.Forms.TabControl TabControl1;
        private System.Windows.Forms.TabPage TabPage1;
        private System.Windows.Forms.TabPage TabPage2;
        private System.Windows.Forms.TextBox txtTimeout;
        private System.Windows.Forms.Label Label1;
        private System.Windows.Forms.GroupBox GroupBox1;
        private System.Windows.Forms.GroupBox GroupBox2;
        private System.Windows.Forms.Label Label2;
        private System.Windows.Forms.Label Label3;
        private System.Windows.Forms.Label Label4;
        private System.Windows.Forms.ComboBox cmbRetry;
        private System.Windows.Forms.Label Label5;
        private System.Windows.Forms.ComboBox cmbParity;
        private System.Windows.Forms.Label Label6;
        private System.Windows.Forms.Label Label7;
        private System.Windows.Forms.Label Label8;
        private System.Windows.Forms.ComboBox cmbStopBits;
        private System.Windows.Forms.ComboBox cmbDataBits;
        private System.Windows.Forms.ComboBox cmbComPort;
        private System.Windows.Forms.ComboBox cmbBaudRate;
        private System.Windows.Forms.Label Label9;
        private System.Windows.Forms.TextBox txtPollDelay;
        private System.Windows.Forms.GroupBox GroupBox3;
        private System.Windows.Forms.Label Label10;
        private System.Windows.Forms.TextBox txtTCPPort;
        private System.Windows.Forms.Label Label11;
        private System.Windows.Forms.TextBox txtHostName;
        private System.Windows.Forms.Button cmdOpenSerial;
        private System.Windows.Forms.Label Label12;
        private System.Windows.Forms.ComboBox cmbTcpProtocol;
        private System.Windows.Forms.Button cmdOpenTCP;
        private System.Windows.Forms.Label Label13;
        private System.Windows.Forms.Label lblResult;
        private System.Windows.Forms.ToolTip ToolTip1;
        private System.Windows.Forms.ComboBox cmbSerialProtocol;
        private System.Windows.Forms.Label lblResult2;
        private System.Windows.Forms.GroupBox GroupBox4;
        private System.Windows.Forms.GroupBox GroupBox5;
        private System.Windows.Forms.TextBox txtSlave;
        private System.Windows.Forms.Label Label14;
        private System.Windows.Forms.Label Label15;
        private System.Windows.Forms.TextBox txtWriteVal1;
        private System.Windows.Forms.TextBox txtWriteVal2;
        private System.Windows.Forms.TextBox txtWriteVal3;
        private System.Windows.Forms.TextBox txtWriteVal4;
        private System.Windows.Forms.TextBox txtWriteVal5;
        private System.Windows.Forms.TextBox txtWriteVal6;
        private System.Windows.Forms.TextBox txtWriteVal7;
        private System.Windows.Forms.TextBox txtWriteVal8;
        private System.Windows.Forms.Label Label16;
        private System.Windows.Forms.Button cmdExecute;
        private System.Windows.Forms.ComboBox cmbCommand;
        private System.Windows.Forms.Label lblReadValues;
        private System.Windows.Forms.TextBox txtStartWrReg;
        private System.Windows.Forms.TextBox txtNumWrRegs;
        private System.Windows.Forms.Label Label17;
        private System.Windows.Forms.TextBox txtNumRdRegs;
        private System.Windows.Forms.Label Label18;
        private System.Windows.Forms.TextBox txtStartRdReg;
        private System.Windows.Forms.Label Label19;
        private System.Windows.Forms.Label Label20;
        private System.Windows.Forms.TextBox txtNumCoils;
        private System.Windows.Forms.TextBox txtStartCoil;
        private System.Windows.Forms.TabPage tabPage3;
        private System.Windows.Forms.TabPage tabPage4;
        private System.Windows.Forms.TabPage tabPage5;
        private System.Windows.Forms.GroupBox groupBox6;
        private System.Windows.Forms.GroupBox groupBox7;
        private System.Windows.Forms.DateTimePicker dateTimePicker1;
        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.Label label21;
        private System.Windows.Forms.Label label22;
    }
}

