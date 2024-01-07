namespace Telly_Control
{
    partial class Telly_control
    {
        /// <summary>
        /// Обязательная переменная конструктора.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Освободить все используемые ресурсы.
        /// </summary>
        /// <param name="disposing">истинно, если управляемый ресурс должен быть удален; иначе ложно.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Код, автоматически созданный конструктором форм Windows

        /// <summary>
        /// Требуемый метод для поддержки конструктора — не изменяйте 
        /// содержимое этого метода с помощью редактора кода.
        /// </summary>
        private void InitializeComponent()
        {
            this.ConnectionPanel = new System.Windows.Forms.Panel();
            this.ComPortsList_ = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.Connect_ = new System.Windows.Forms.Button();
            this.Controlls_panel = new System.Windows.Forms.Panel();
            this.Slave_Control = new System.Windows.Forms.Panel();
            this.SlaveTable = new System.Windows.Forms.DataGridView();
            this.SyncParam = new System.Windows.Forms.Button();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.Tally = new System.Windows.Forms.Label();
            this.Addr = new System.Windows.Forms.Label();
            this.DevID = new System.Windows.Forms.Label();
            this.RadioMode = new System.Windows.Forms.Label();
            this.Battery = new System.Windows.Forms.Label();
            this.WorkMode = new System.Windows.Forms.Label();
            this.DeviceParam = new System.Windows.Forms.Button();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.Frequency = new System.Windows.Forms.Label();
            this.RadioRx = new System.Windows.Forms.Label();
            this.RadioTx = new System.Windows.Forms.Label();
            this.SetNewFMParam = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.SetNewWifiParam = new System.Windows.Forms.Button();
            this.AP = new System.Windows.Forms.Label();
            this.APstate = new System.Windows.Forms.Label();
            this.WifiVersion = new System.Windows.Forms.Label();
            this.Message_label = new System.Windows.Forms.Label();
            this.FrequencyRx = new System.Windows.Forms.Label();
            this.ConnectionPanel.SuspendLayout();
            this.Controlls_panel.SuspendLayout();
            this.Slave_Control.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.SlaveTable)).BeginInit();
            this.groupBox3.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // ConnectionPanel
            // 
            this.ConnectionPanel.Anchor = System.Windows.Forms.AnchorStyles.None;
            this.ConnectionPanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.ConnectionPanel.Controls.Add(this.ComPortsList_);
            this.ConnectionPanel.Controls.Add(this.label1);
            this.ConnectionPanel.Controls.Add(this.Connect_);
            this.ConnectionPanel.Location = new System.Drawing.Point(394, 187);
            this.ConnectionPanel.Name = "ConnectionPanel";
            this.ConnectionPanel.Size = new System.Drawing.Size(261, 142);
            this.ConnectionPanel.TabIndex = 0;
            // 
            // ComPortsList_
            // 
            this.ComPortsList_.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.ComPortsList_.FormattingEnabled = true;
            this.ComPortsList_.ImeMode = System.Windows.Forms.ImeMode.NoControl;
            this.ComPortsList_.Location = new System.Drawing.Point(61, 61);
            this.ComPortsList_.Name = "ComPortsList_";
            this.ComPortsList_.Size = new System.Drawing.Size(197, 21);
            this.ComPortsList_.TabIndex = 2;
            this.ComPortsList_.Text = "No Ports";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(3, 64);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(52, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Com port:";
            // 
            // Connect_
            // 
            this.Connect_.Location = new System.Drawing.Point(183, 116);
            this.Connect_.Name = "Connect_";
            this.Connect_.Size = new System.Drawing.Size(75, 23);
            this.Connect_.TabIndex = 0;
            this.Connect_.Text = "Connect";
            this.Connect_.UseVisualStyleBackColor = true;
            this.Connect_.Click += new System.EventHandler(this.Connect__Click);
            // 
            // Controlls_panel
            // 
            this.Controlls_panel.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.Controlls_panel.Controls.Add(this.Slave_Control);
            this.Controlls_panel.Controls.Add(this.groupBox3);
            this.Controlls_panel.Controls.Add(this.groupBox2);
            this.Controlls_panel.Controls.Add(this.groupBox1);
            this.Controlls_panel.Location = new System.Drawing.Point(2, 12);
            this.Controlls_panel.Name = "Controlls_panel";
            this.Controlls_panel.Size = new System.Drawing.Size(1037, 476);
            this.Controlls_panel.TabIndex = 1;
            // 
            // Slave_Control
            // 
            this.Slave_Control.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.Slave_Control.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.Slave_Control.Controls.Add(this.SlaveTable);
            this.Slave_Control.Controls.Add(this.SyncParam);
            this.Slave_Control.Location = new System.Drawing.Point(477, 3);
            this.Slave_Control.Name = "Slave_Control";
            this.Slave_Control.Size = new System.Drawing.Size(557, 459);
            this.Slave_Control.TabIndex = 8;
            // 
            // SlaveTable
            // 
            this.SlaveTable.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.SlaveTable.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.SlaveTable.Location = new System.Drawing.Point(3, 3);
            this.SlaveTable.Name = "SlaveTable";
            this.SlaveTable.Size = new System.Drawing.Size(546, 420);
            this.SlaveTable.TabIndex = 8;
            // 
            // SyncParam
            // 
            this.SyncParam.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.SyncParam.Location = new System.Drawing.Point(452, 429);
            this.SyncParam.Name = "SyncParam";
            this.SyncParam.Size = new System.Drawing.Size(97, 23);
            this.SyncParam.TabIndex = 7;
            this.SyncParam.Text = "Синхронизация устройств";
            this.SyncParam.UseVisualStyleBackColor = true;
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.Tally);
            this.groupBox3.Controls.Add(this.Addr);
            this.groupBox3.Controls.Add(this.DevID);
            this.groupBox3.Controls.Add(this.RadioMode);
            this.groupBox3.Controls.Add(this.Battery);
            this.groupBox3.Controls.Add(this.WorkMode);
            this.groupBox3.Controls.Add(this.DeviceParam);
            this.groupBox3.Location = new System.Drawing.Point(10, 3);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(461, 149);
            this.groupBox3.TabIndex = 7;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Параметры устройства";
            // 
            // Tally
            // 
            this.Tally.AutoSize = true;
            this.Tally.Location = new System.Drawing.Point(6, 130);
            this.Tally.Name = "Tally";
            this.Tally.Size = new System.Drawing.Size(32, 13);
            this.Tally.TabIndex = 9;
            this.Tally.Text = "Tally:";
            // 
            // Addr
            // 
            this.Addr.AutoSize = true;
            this.Addr.Location = new System.Drawing.Point(6, 41);
            this.Addr.Name = "Addr";
            this.Addr.Size = new System.Drawing.Size(41, 13);
            this.Addr.TabIndex = 8;
            this.Addr.Text = "Адрес:";
            // 
            // DevID
            // 
            this.DevID.AutoSize = true;
            this.DevID.Location = new System.Drawing.Point(6, 20);
            this.DevID.Name = "DevID";
            this.DevID.Size = new System.Drawing.Size(21, 13);
            this.DevID.TabIndex = 7;
            this.DevID.Text = "ID:";
            // 
            // RadioMode
            // 
            this.RadioMode.AutoSize = true;
            this.RadioMode.Location = new System.Drawing.Point(6, 109);
            this.RadioMode.Name = "RadioMode";
            this.RadioMode.Size = new System.Drawing.Size(37, 13);
            this.RadioMode.TabIndex = 6;
            this.RadioMode.Text = "Эфир:";
            // 
            // Battery
            // 
            this.Battery.AutoSize = true;
            this.Battery.Location = new System.Drawing.Point(6, 86);
            this.Battery.Name = "Battery";
            this.Battery.Size = new System.Drawing.Size(65, 13);
            this.Battery.TabIndex = 5;
            this.Battery.Text = "Заряд АКБ:";
            // 
            // WorkMode
            // 
            this.WorkMode.AutoSize = true;
            this.WorkMode.Location = new System.Drawing.Point(6, 63);
            this.WorkMode.Name = "WorkMode";
            this.WorkMode.Size = new System.Drawing.Size(45, 13);
            this.WorkMode.TabIndex = 4;
            this.WorkMode.Text = "Режим:";
            // 
            // DeviceParam
            // 
            this.DeviceParam.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.DeviceParam.Location = new System.Drawing.Point(380, 120);
            this.DeviceParam.Name = "DeviceParam";
            this.DeviceParam.Size = new System.Drawing.Size(75, 23);
            this.DeviceParam.TabIndex = 4;
            this.DeviceParam.Text = "Изменить";
            this.DeviceParam.UseVisualStyleBackColor = true;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.FrequencyRx);
            this.groupBox2.Controls.Add(this.Frequency);
            this.groupBox2.Controls.Add(this.RadioRx);
            this.groupBox2.Controls.Add(this.RadioTx);
            this.groupBox2.Controls.Add(this.SetNewFMParam);
            this.groupBox2.Location = new System.Drawing.Point(10, 313);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(461, 149);
            this.groupBox2.TabIndex = 1;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Параметры FM приёмопередатчика";
            // 
            // Frequency
            // 
            this.Frequency.AutoSize = true;
            this.Frequency.Location = new System.Drawing.Point(6, 72);
            this.Frequency.Name = "Frequency";
            this.Frequency.Size = new System.Drawing.Size(52, 13);
            this.Frequency.TabIndex = 6;
            this.Frequency.Text = "Частота:";
            // 
            // RadioRx
            // 
            this.RadioRx.AutoSize = true;
            this.RadioRx.Location = new System.Drawing.Point(6, 49);
            this.RadioRx.Name = "RadioRx";
            this.RadioRx.Size = new System.Drawing.Size(62, 13);
            this.RadioRx.TabIndex = 5;
            this.RadioRx.Text = "Приёмник:";
            // 
            // RadioTx
            // 
            this.RadioTx.AutoSize = true;
            this.RadioTx.Location = new System.Drawing.Point(6, 26);
            this.RadioTx.Name = "RadioTx";
            this.RadioTx.Size = new System.Drawing.Size(70, 13);
            this.RadioTx.TabIndex = 4;
            this.RadioTx.Text = "Передатчик:";
            // 
            // SetNewFMParam
            // 
            this.SetNewFMParam.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.SetNewFMParam.Location = new System.Drawing.Point(380, 120);
            this.SetNewFMParam.Name = "SetNewFMParam";
            this.SetNewFMParam.Size = new System.Drawing.Size(75, 23);
            this.SetNewFMParam.TabIndex = 4;
            this.SetNewFMParam.Text = "Изменить";
            this.SetNewFMParam.UseVisualStyleBackColor = true;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.SetNewWifiParam);
            this.groupBox1.Controls.Add(this.AP);
            this.groupBox1.Controls.Add(this.APstate);
            this.groupBox1.Controls.Add(this.WifiVersion);
            this.groupBox1.Location = new System.Drawing.Point(10, 158);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(461, 149);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Параметры WiFI модуля";
            // 
            // SetNewWifiParam
            // 
            this.SetNewWifiParam.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.SetNewWifiParam.Location = new System.Drawing.Point(380, 120);
            this.SetNewWifiParam.Name = "SetNewWifiParam";
            this.SetNewWifiParam.Size = new System.Drawing.Size(75, 23);
            this.SetNewWifiParam.TabIndex = 3;
            this.SetNewWifiParam.Text = "Изменить";
            this.SetNewWifiParam.UseVisualStyleBackColor = true;
            // 
            // AP
            // 
            this.AP.AutoSize = true;
            this.AP.Location = new System.Drawing.Point(6, 80);
            this.AP.Name = "AP";
            this.AP.Size = new System.Drawing.Size(83, 13);
            this.AP.TabIndex = 2;
            this.AP.Text = "Точка доступа:";
            // 
            // APstate
            // 
            this.APstate.AutoSize = true;
            this.APstate.Location = new System.Drawing.Point(6, 55);
            this.APstate.Name = "APstate";
            this.APstate.Size = new System.Drawing.Size(134, 13);
            this.APstate.TabIndex = 1;
            this.APstate.Text = "Состояние подключения:";
            // 
            // WifiVersion
            // 
            this.WifiVersion.AutoSize = true;
            this.WifiVersion.Location = new System.Drawing.Point(6, 30);
            this.WifiVersion.Name = "WifiVersion";
            this.WifiVersion.Size = new System.Drawing.Size(47, 13);
            this.WifiVersion.TabIndex = 0;
            this.WifiVersion.Text = "Версия:";
            // 
            // Message_label
            // 
            this.Message_label.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.Message_label.AutoSize = true;
            this.Message_label.Location = new System.Drawing.Point(3, 491);
            this.Message_label.Name = "Message_label";
            this.Message_label.Size = new System.Drawing.Size(144, 13);
            this.Message_label.TabIndex = 2;
            this.Message_label.Text = "Устройсво не подключено!";
            // 
            // FrequencyRx
            // 
            this.FrequencyRx.AutoSize = true;
            this.FrequencyRx.Location = new System.Drawing.Point(6, 94);
            this.FrequencyRx.Name = "FrequencyRx";
            this.FrequencyRx.Size = new System.Drawing.Size(52, 13);
            this.FrequencyRx.TabIndex = 7;
            this.FrequencyRx.Text = "Частота:";
            // 
            // Telly_control
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1041, 512);
            this.Controls.Add(this.Message_label);
            this.Controls.Add(this.Controlls_panel);
            this.Controls.Add(this.ConnectionPanel);
            this.Name = "Telly_control";
            this.Text = "Telly control";
            this.Load += new System.EventHandler(this.Telly_control_Load);
            this.ConnectionPanel.ResumeLayout(false);
            this.ConnectionPanel.PerformLayout();
            this.Controlls_panel.ResumeLayout(false);
            this.Slave_Control.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.SlaveTable)).EndInit();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Panel ConnectionPanel;
        private System.Windows.Forms.ComboBox ComPortsList_;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button Connect_;
        private System.Windows.Forms.Panel Controlls_panel;
        private System.Windows.Forms.Label Message_label;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label WifiVersion;
        private System.Windows.Forms.Label APstate;
        private System.Windows.Forms.Button SetNewWifiParam;
        private System.Windows.Forms.Label AP;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Label RadioRx;
        private System.Windows.Forms.Label RadioTx;
        private System.Windows.Forms.Button SetNewFMParam;
        private System.Windows.Forms.Label Frequency;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Label RadioMode;
        private System.Windows.Forms.Label Battery;
        private System.Windows.Forms.Label WorkMode;
        private System.Windows.Forms.Button DeviceParam;
        private System.Windows.Forms.Panel Slave_Control;
        private System.Windows.Forms.DataGridView SlaveTable;
        private System.Windows.Forms.Button SyncParam;
        private System.Windows.Forms.Label DevID;
        private System.Windows.Forms.Label Addr;
        private System.Windows.Forms.Label Tally;
        private System.Windows.Forms.Label FrequencyRx;
    }
}

