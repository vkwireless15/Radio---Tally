using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;
using static System.Windows.Forms.VisualStyles.VisualStyleElement.ProgressBar;

namespace Telly_Control
{
    public partial class Telly_control : Form
    {
        SerialPort SP;
        char[] RX_BF;
        //byte[] RX_BFB = new byte[128];
        bool FirstStart = false;
   
        byte DevWorkMode = 0;
        byte BatteryVal = 0;
        byte Eter = 0;
        string WifiVersion_ = "";
        byte Connection = 0;
        string APName = "";
        string Key = "";
        string Transmitter = "";
        string Receiver = "";
        string Freq = "";

        byte RX_moduleConf = 0;
        byte TX_moduleConf = 0;
        byte WiFi_moduleConf = 0;
        byte TallyConf = 0;

        byte Address_ = 0;
        string ID = "";

        public Telly_control()
        {
            InitializeComponent();
        }

        private void Telly_control_Load(object sender, EventArgs e)
        {
            Controlls_panel.Visible = false;
            string [] ComPortsList = SerialPort.GetPortNames();
            if(ComPortsList.Length != 0)
            {
                ComPortsList_.Text = ComPortsList[0];
                ComPortsList_.Items.Clear();
                ComPortsList_.Items.AddRange(ComPortsList);
            }
            else 
            {
                ComPortsList_.Text = "No Ports";
            }
        }

        private void Connect__Click(object sender, EventArgs e)
        {
            SP = new SerialPort();
            try
            {
                SP.PortName = ComPortsList_.Text;
                SP.BaudRate = 115200;
                SP.DataReceived += GetComMessage;
                SP.Open();
                SP.Write("FW");
                //ConnectionPanel.Visible = false;
            }
            catch
            {
                MessageBox.Show("Ошибка подключения");
            }
        }

        void GetComMessage(object sender, SerialDataReceivedEventArgs e)
        {
            var port = (SerialPort)sender;
            int buferSize = port.BytesToRead;
            int COUNT = 0;

            string Mess = "";

            RX_BF = new char[buferSize];

            for (int i = 0; i < buferSize; ++i)
            {
                RX_BF[COUNT] = (char)port.ReadByte();
                Mess += RX_BF[COUNT];
                COUNT++;
            }

            if (COUNT > 0)
            {
                if (FirstStart == false)
                {
                    if (RX_BF[0] == 'T' && RX_BF[1] == 'P')
                    {
                        ConnectionPanel.Invoke(new Action(() => { ConnectionPanel.Visible = false; }));
                        Controlls_panel.Invoke(new Action(() => { Controlls_panel.Visible = true; }));
                        Message_label.Invoke(new Action(() => { Message_label.Text = Mess; Message_label.ForeColor = Color.Green; }));
                        FirstStart = true;
                        SP.Write("GP"); //Команда загрузки параметров из прибора
                    }
                    else
                    {
                        Message_label.Invoke(new Action(() => { Message_label.Text = "Неизвестное устройство"; Message_label.ForeColor = Color.Red; }));
                        ConnectionPanel.Invoke(new Action(() => { ConnectionPanel.Visible = false; }));
                    }
                }
                else
                {
                    if (RX_BF[0] == 'D' && RX_BF[1] == 'V' && RX_BF[2] == 'M')
                    {
                        DevWorkMode = (byte)RX_BF[3];
                        if (DevWorkMode == 0)
                        {
                            WorkMode.Invoke(new Action(() => { WorkMode.Text = "Режим: Ведущий"; }));
                        }
                        else
                        {
                            WorkMode.Invoke(new Action(() => { WorkMode.Text = "Режим: Ведомый"; }));
                        }
                    }

                    if (RX_BF[0] == 'D' && RX_BF[1] == 'V' && RX_BF[2] == 'B')
                    {
                        BatteryVal = (byte)RX_BF[3];
                        Battery.Invoke(new Action(() => { Battery.Text = "Заряд АКБ:" + BatteryVal.ToString() + "%"; }));
                    }

                    if (RX_BF[0] == 'D' && RX_BF[1] == 'V' && RX_BF[2] == 'E')
                    {
                        Eter = (byte)RX_BF[3];
                        if (Eter == 0)
                        {
                            RadioMode.Invoke(new Action(() => { RadioMode.Text = "Эфир: RX"; }));
                        }
                        else
                        {
                            RadioMode.Invoke(new Action(() => { RadioMode.Text = "Эфир: TX"; }));
                        }
                    }

                    if (RX_BF[0] == 'W' && RX_BF[1] == 'F' && RX_BF[2] == 'V')
                    {
                        for (int i = 3; i < RX_BF.Length; i++)
                        { WifiVersion_ += RX_BF[i]; }
                        WifiVersion.Invoke(new Action(() => { WifiVersion.Text = "Версия:" + WifiVersion_; }));
                    }

                    if (RX_BF[0] == 'W' && RX_BF[1] == 'F' && RX_BF[2] == 'C')
                    {
                        Connection = (byte)RX_BF[3];
                        if (Connection == 0)
                        {
                            APstate.Invoke(new Action(() => { APstate.Text = "Состояние подключения: Не подключено"; }));
                        }
                        else
                        {
                            APstate.Invoke(new Action(() => { APstate.Text = "Состояние подключения: Подключено"; }));
                        }
                    }

                    if (RX_BF[0] == 'W' && RX_BF[1] == 'F' && RX_BF[2] == 'A')
                    {
                        for (int i = 3; i < RX_BF.Length; i++)
                        { APName += RX_BF[i]; }
                        AP.Invoke(new Action(() => { AP.Text = "Точка доступа:" + APName; }));
                    }

                    if (RX_BF[0] == 'W' && RX_BF[1] == 'F' && RX_BF[2] == 'K')
                    {
                        for (int i = 3; i < RX_BF.Length; i++)
                        { Key += RX_BF[i]; }
                    }

                    if (RX_BF[0] == 'F' && RX_BF[1] == 'M' && RX_BF[2] == 'T')
                    {
                        for (int i = 3; i < RX_BF.Length; i++)
                        { Transmitter += RX_BF[i]; }
                        RadioTx.Invoke(new Action(() => { RadioTx.Text = "Передатчик:" + Transmitter; }));
                    }

                    if (RX_BF[0] == 'F' && RX_BF[1] == 'M' && RX_BF[2] == 'R')
                    {
                        for (int i = 3; i < RX_BF.Length; i++)
                        { Receiver += RX_BF[i]; }
                        RadioRx.Invoke(new Action(() => { RadioRx.Text = "Приёмник:" + Receiver; }));
                    }

                    if (RX_BF[0] == 'F' && RX_BF[1] == 'M' && RX_BF[2] == 'F')
                    {
                        for (int i = 3; i < RX_BF.Length; i++)
                        { Freq += RX_BF[i]; }
                        Frequency.Invoke(new Action(() => { Frequency.Text = "Частота:" + Freq; }));
                    }

                    if (RX_BF[0] == 'C' && RX_BF[1] == 'N' && RX_BF[2] == 'F')
                    {
                        RX_moduleConf = (byte)RX_BF[3];
                        TX_moduleConf = (byte)RX_BF[4];
                        WiFi_moduleConf = (byte)RX_BF[5];
                        TallyConf = (byte)RX_BF[6];
                        Address_ = (byte)RX_BF[7];
                        ID += RX_BF[8];
                        ID += RX_BF[9];

                        DevID.Invoke(new Action(() => { DevID.Text = "ID:" + ID; }));
                        Addr.Invoke(new Action(() => { Addr.Text = "Адрес:" + Address_.ToString(); }));

                        if (TallyConf == 1)
                        {
                            Tally.Invoke(new Action(() => { Tally.Text = "Tally: Активирован"; }));
                        }
                        else
                        { Tally.Invoke(new Action(() => { Tally.Text = "Tally: Не активирован"; })); }

                        if (RX_moduleConf == 1)
                        {
                            RadioRx.Invoke(new Action(() => { RadioRx.Text = "Приёмник:" + Receiver + ", Активирован"; }));
                        }
                        else
                        {
                            RadioRx.Invoke(new Action(() => { RadioRx.Text = "Приёмник:" + Receiver + ", Не активирован"; }));
                        }

                        if (TX_moduleConf == 1)
                        {
                            RadioTx.Invoke(new Action(() => { RadioTx.Text = "Передатчик:" + Transmitter + ", Активирован"; }));
                        }
                        else
                        {
                            RadioTx.Invoke(new Action(() => { RadioTx.Text = "Передатчик:" + Transmitter + ", Не активирован"; }));
                        }

                        if(WiFi_moduleConf == 1)
                        {
                            WifiVersion.Invoke(new Action(() => { WifiVersion.Text = "Версия:" + WifiVersion_ + ", Активирован"; }));
                        }
                        else
                        {
                            WifiVersion.Invoke(new Action(() => { WifiVersion.Text = "Версия:" + WifiVersion_ + ", Не активирован"; }));
                        }


                    }
                }
            }

        }
    }
}
