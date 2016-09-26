using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using FieldTalk.Modbus.Master;

namespace GuiExample
{
    public partial class Form1 : Form
    {
        private MbusMasterFunctions myProtocol;

        public Form1()
        {
            InitializeComponent();
        }


        private void cmdOpenSerial_Click(object sender, EventArgs e)
        {
            //
            // First we must instantiate class if we haven't done so already
            //
            if ((myProtocol == null))
            {
                try
                {
                    if ((cmbSerialProtocol.SelectedIndex == 0))
                        myProtocol = new MbusRtuMasterProtocol(); // RTU
                    else
                        myProtocol = new MbusAsciiMasterProtocol(); // ASCII
                }
                catch (OutOfMemoryException ex)
                {
                    lblResult.Text = ("Could not instantiate serial protocol class! Error was " + ex.Message);
                    return;
                }
            }
            else // already instantiated, close protocol, reinstantiate
            {
                if (myProtocol.isOpen())
                    myProtocol.closeProtocol();
                myProtocol = null;
                try
                {
                    if ((cmbSerialProtocol.SelectedIndex == 0))
                        myProtocol = new MbusRtuMasterProtocol(); // RTU
                    else
                        myProtocol = new MbusAsciiMasterProtocol(); // ASCII
                }
                catch (OutOfMemoryException ex)
                {
                    lblResult.Text = ("Could not instantiate serial protocol class! Error was " + ex.Message);
                    return;
                }
            }
            //
            // Here we configure the protocol
            //
            int retryCnt;
            int pollDelay;
            int timeOut;
            int baudRate;
            int parity;
            int dataBits;
            int stopBits;
            int res;
            try
            {
                retryCnt = int.Parse(cmbRetry.Text);
            }
            catch (Exception)
            {
                retryCnt = 0;
            }
            try
            {
                pollDelay = int.Parse(txtPollDelay.Text);
            }
            catch (Exception)
            {
                pollDelay = 0;
            }
            try
            {
                timeOut = int.Parse(txtTimeout.Text);
            }
            catch (Exception)
            {
                timeOut = 1000;
            }
            try
            {
                baudRate = int.Parse(cmbBaudRate.Text);
            }
            catch (Exception)
            {
                baudRate = 19200;
            }
            switch (cmbParity.SelectedIndex)
            {
                default:
                case 0:
                    parity = MbusSerialMasterProtocol.SER_PARITY_NONE;
                break;
                case 1:
                    parity = MbusSerialMasterProtocol.SER_PARITY_EVEN;
                break;
                case 2:
                    parity = MbusSerialMasterProtocol.SER_PARITY_ODD;
                break;
            }
            switch (cmbDataBits.SelectedIndex)
            {
                default:
                case 0:
                    dataBits = MbusSerialMasterProtocol.SER_DATABITS_8;
                break;
                case 1:
                    dataBits = MbusSerialMasterProtocol.SER_DATABITS_7;
                break;
            }
            switch (cmbStopBits.SelectedIndex)
            {
                default:
                case 0:
                    stopBits = MbusSerialMasterProtocol.SER_STOPBITS_1;
                break;
                case 1:
                    stopBits = MbusSerialMasterProtocol.SER_STOPBITS_2;
                break;
            }
            myProtocol.timeout = timeOut;
            myProtocol.retryCnt = retryCnt;
            myProtocol.pollDelay = pollDelay;
            // Note: The following cast is required as the myProtocol object is declared
            // as the superclass of MbusSerialMasterProtocol. That way myProtocol can
            // represent different protocol types.
            res = ((MbusSerialMasterProtocol)(myProtocol)).openProtocol(cmbComPort.Text, baudRate, dataBits, stopBits, parity);
            if ((res == BusProtocolErrors.FTALK_SUCCESS))
            {
                lblResult.Text = ("Serial port opened successfully with parameters: "
                            + (cmbComPort.Text + (", "
                            + (baudRate + (" baud, "
                            + (dataBits + (" data bits, "
                            + (stopBits + (" stop bits, parity " + parity)))))))));
            }
            else
            {
                lblResult.Text = ("Could not open protocol, error was: " + BusProtocolErrors.getBusProtocolErrorText(res));
            }
        }


        private void cmdOpenTCP_Click(object sender, EventArgs e)
        {
            //
            // First we must instantiate class if we haven't done so already
            //
            if ((myProtocol == null))
            {
                try
                {
                    if ((cmbTcpProtocol.SelectedIndex == 0))
                        myProtocol = new MbusTcpMasterProtocol();
                    else
                        myProtocol = new MbusRtuOverTcpMasterProtocol();
                }
                catch (OutOfMemoryException ex)
                {
                    lblResult.Text = ("Could not instantiate ethernet protocol class! Error was " + ex.Message);
                    return;
                }
            }
            else // already instantiated, close protocol and reinstantiate
            {
                if (myProtocol.isOpen())
                    myProtocol.closeProtocol();
                myProtocol = null;
                try
                {
                    if ((cmbTcpProtocol.SelectedIndex == 0))
                        myProtocol = new MbusTcpMasterProtocol();
                    else
                        myProtocol = new MbusRtuOverTcpMasterProtocol();
                }
                catch (OutOfMemoryException ex)
                {
                    lblResult.Text = ("Could not instantiate ethernet protocol class! Error was " + ex.Message);
                    return;
                }
            }
            //
            // Here we configure the protocol
            //
            int retryCnt;
            int pollDelay;
            int timeOut;
            int tcpPort;
            int res;
            try
            {
                retryCnt = int.Parse(cmbRetry.Text);
            }
            catch (Exception)
            {
                retryCnt = 0;
            }
            try
            {
                pollDelay = int.Parse(txtPollDelay.Text);
            }
            catch (Exception)
            {
                pollDelay = 0;
            }
            try
            {
                timeOut = int.Parse(txtTimeout.Text);
            }
            catch (Exception)
            {
                timeOut = 1000;
            }
            try
            {
                tcpPort = int.Parse(txtTCPPort.Text);
            }
            catch (Exception)
            {
                tcpPort = 502;
            }
            myProtocol.timeout = timeOut;
            myProtocol.retryCnt = retryCnt;
            myProtocol.pollDelay = pollDelay;
            // Note: The following cast is required as the myProtocol object is declared
            // as the superclass of MbusTcpMasterProtocol. That way myProtocol can
            // represent different protocol types.
            ((MbusTcpMasterProtocol)myProtocol).port = (short) tcpPort;
            res = ((MbusTcpMasterProtocol) myProtocol).openProtocol(txtHostName.Text);
            if ((res == BusProtocolErrors.FTALK_SUCCESS))
            {
                lblResult.Text = ("Modbus/TCP port opened successfully with parameters: " + (txtHostName.Text + (", TCP port " + tcpPort)));
            }
            else
            {
                lblResult.Text = ("Could not open protocol, error was: " + BusProtocolErrors.getBusProtocolErrorText(res));
            }
        }


        private void Form1_Load(object sender, EventArgs e)
        {
            ToolTip1.SetToolTip(txtPollDelay, "Delay in milliseconds between two consecutive Modbus operations, 0 to disable");
            ToolTip1.SetToolTip(cmbRetry, "How many times do we retry operation if it fails first time?");
            ToolTip1.SetToolTip(cmbSerialProtocol, "Serial protocol choice: ASCII or RTU");
            ToolTip1.SetToolTip(cmbTcpProtocol, "Ethernet protocol choice: MODBUS/TCP or Encapsulated RTU over TCP");
            cmbComPort.SelectedIndex = 0;
            cmbParity.SelectedIndex = 0;
            cmbStopBits.SelectedIndex = 0;
            cmbDataBits.SelectedIndex = 0;
            cmbBaudRate.SelectedIndex = 0;
            cmbSerialProtocol.SelectedIndex = 0;
            cmbTcpProtocol.SelectedIndex = 0;
            cmbRetry.SelectedIndex = 0;
            cmbCommand.SelectedIndex = 0;
        }


        private void cmdExecute_Click(object sender, EventArgs e)
        {
            short[] writeVals = new short[125];
            short[] readVals = new short[125];
            int slave;
            int startWrReg;
            int numWrRegs;
            int startRdReg;
            int numRdRegs;
            int i;
            int res;
            int startCoil;
            int numCoils;
            bool[] coilVals = new bool[2000];
            try
            {
                try
                {
                    slave = int.Parse(txtSlave.Text);
                }
                catch (Exception)
                {
                    slave = 1;
                }
                try
                {
                    startCoil = int.Parse(txtStartCoil.Text);
                }
                catch (Exception)
                {
                    startCoil = 1;
                }
                try
                {
                    numCoils = int.Parse(txtNumCoils.Text);
                }
                catch (Exception)
                {
                    numCoils = 1;
                }
                try
                {
                    startRdReg = int.Parse(txtStartRdReg.Text);
                }
                catch (Exception)
                {
                    startRdReg = 1;
                }
                try
                {
                    startWrReg = int.Parse(txtStartWrReg.Text);
                }
                catch (Exception)
                {
                    startWrReg = 1;
                }
                try
                {
                    numWrRegs = int.Parse(txtNumWrRegs.Text);
                }
                catch (Exception)
                {
                    numWrRegs = 1;
                }
                try
                {
                    numRdRegs = int.Parse(txtNumRdRegs.Text);
                }
                catch (Exception)
                {
                    numRdRegs = 1;
                }
                try
                {
                    writeVals[0] = Int16.Parse(txtWriteVal1.Text);
                    writeVals[1] = Int16.Parse(txtWriteVal2.Text);
                    writeVals[2] = Int16.Parse(txtWriteVal3.Text);
                    writeVals[3] = Int16.Parse(txtWriteVal4.Text);
                    writeVals[4] = Int16.Parse(txtWriteVal5.Text);
                    writeVals[5] = Int16.Parse(txtWriteVal6.Text);
                    writeVals[6] = Int16.Parse(txtWriteVal7.Text);
                    writeVals[7] = Int16.Parse(txtWriteVal8.Text);
                    coilVals[0] = (writeVals[0] != 0);
                    coilVals[1] = (writeVals[1] != 0);
                    coilVals[2] = (writeVals[2] != 0);
                    coilVals[3] = (writeVals[3] != 0);
                    coilVals[4] = (writeVals[4] != 0);
                    coilVals[5] = (writeVals[5] != 0);
                    coilVals[6] = (writeVals[6] != 0);
                    coilVals[7] = (writeVals[7] != 0);
                }
                catch (Exception)
                {
                }
                switch (cmbCommand.SelectedIndex)
                {
                    //
                    // Read Holding Registers
                    //
                    case 0:
                        res = myProtocol.readMultipleRegisters(slave, startRdReg, readVals, numRdRegs);
                        lblResult2.Text = ("Result: " + (BusProtocolErrors.getBusProtocolErrorText(res) + "\r\n"));
                        if ((res == BusProtocolErrors.FTALK_SUCCESS))
                        {
                            lblReadValues.Text = "";
                            for (i = 0; (i <= (numRdRegs - 1)); i++)
                            {
                                lblReadValues.Text = (lblReadValues.Text + (readVals[i] + "  "));
                            }
                        }
                    break;
                    //
                    // Read Input Registers
                    //
                    case 1:
                        res = myProtocol.readInputRegisters(slave, startRdReg, readVals, numRdRegs);
                        lblResult2.Text = ("Result: " + (BusProtocolErrors.getBusProtocolErrorText(res) + "\r\n"));
                        if ((res == BusProtocolErrors.FTALK_SUCCESS))
                        {
                            lblReadValues.Text = "";
                            for (i = 0; (i <= (numRdRegs - 1)); i++)
                            {
                                lblReadValues.Text = (lblReadValues.Text + (readVals[i] + "  "));
                            }
                        }
                    break;
                    //
                    // Read Coils
                    //
                    case 2:
                        res = myProtocol.readCoils(slave, startCoil, coilVals, numCoils);
                        lblResult2.Text = ("Result: " + (BusProtocolErrors.getBusProtocolErrorText(res) + "\r\n"));
                        if ((res == BusProtocolErrors.FTALK_SUCCESS))
                        {
                            lblReadValues.Text = "";
                            for (i = 0; (i <= (numCoils - 1)); i++)
                            {
                                if (coilVals[i])
                                    lblReadValues.Text = (lblReadValues.Text + "1  ");
                                else
                                    lblReadValues.Text = (lblReadValues.Text + "0  ");
                            }
                        }
                    break;
                    //
                    // Write Coils
                    //
                    case 3:
                        res = myProtocol.forceMultipleCoils(slave, startCoil, coilVals, numCoils);
                        lblResult2.Text = ("Result: " + (BusProtocolErrors.getBusProtocolErrorText(res) + "\r\n"));
                    break;
                    //
                    // Write Holding Registers
                    //
                    case 4:
                        res = myProtocol.writeMultipleRegisters(slave, startWrReg, writeVals, numWrRegs);
                        lblResult2.Text = ("Result: " + (BusProtocolErrors.getBusProtocolErrorText(res) + "\r\n"));
                    break;
                    //
                    // Write Single Registers
                    //
                    case 5:
                        res = myProtocol.writeSingleRegister(slave, startWrReg, writeVals[0]);
                        lblResult2.Text = ("Result: " + (BusProtocolErrors.getBusProtocolErrorText(res) + "\r\n"));
                    break;
                    //
                    // Read/Write Registers
                    //
                    case 6:
                        res = myProtocol.readWriteRegisters(slave, startRdReg, readVals, numRdRegs, startWrReg, writeVals, numWrRegs);
                        lblResult2.Text = ("Result: "  + (BusProtocolErrors.getBusProtocolErrorText(res) + "\r\n"));
                    break;
                }
            }
            catch (Exception ex)
            {
                lblResult2.Text = ("Exception occured: " + ex.Message);
            }
        }

        private void groupBox6_Enter (object sender, EventArgs e)
        {

        }

        private void dateTimePicker1_ValueChanged (object sender, EventArgs e)
        {

        }

        private void label21_Click (object sender, EventArgs e)
        {

        }
    }
}