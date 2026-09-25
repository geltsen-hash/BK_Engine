#pragma once
#include <windows.h>
#include <tchar.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <complex> 
#include <string> 
#include "variable.h"
//#include "function.h"
#include <typeinfo.h>
#include <string>
#include <bitset>
#include <vector>
#include "winuser.h"
#include <mmsystem.h>
#include "resistor_grid.h"
#include <msclr/marshal_cppstd.h>
#include <omp.h>
#include <chrono> 
//#include <thread>
//#include <future>
extern vector<double> Ro_BH_steps;
extern vector<double> Ro_PZ_steps;
extern vector<double> Ro_stratum_steps;
extern vector<double> R_BH_steps;
extern vector<double> R_PZ_steps;
extern vector<BK> for_inversion;
extern vector<vector<ELECTRODE>> electrodeList;
extern BK BK_pallete_L[Ds_steps][Rzp_steps][Dzp_steps][Rop_steps];

//extern vector<vector<vector<vector<BK>>>> BK_pallete_;
extern Vector_BK BK_pallete;


namespace BK_PALETTE_CREATOR {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	using namespace System::Timers;
	using namespace System::IO;
	using namespace System::IO::Ports;

	using namespace System::Runtime::InteropServices;
	using namespace System::Windows::Forms::DataVisualization::Charting;
	using namespace System::Threading;
	using namespace std;
	using namespace System::Collections::Generic; // Для работы с List
	using namespace System::Windows;

	/// <summary>
	/// Сводка для Main_Form
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
			//
			//TODO: добавьте код конструктора
			//
			SetConsoleCP(1251);
			SetConsoleOutputCP(1251);
			int visual_mode = CALK_MODE;
			//chart_iversion->BringToFront();
			//chart_iversion->Visible = false;

			textBox_D_sonde->Text = "90";
			textBox_dr_start_mm->Text = "1";
			textBox_q_r->Text = "1.1";
			textBox_r_max_m->Text = "3.5";
			textBox_tail->Text = "2.5";
			textBox_dz_base_mm->Text = "10";
			textBox_dz_fine_mm->Text = "1";
			textBox_q_z_tail->Text = "1.15";
			textBox_threshold_mm->Text = "20";

			textBox_LONG_Z->Text = "1450"; textBox_LONG_L->Text = "100";				
			textBox_MID_Z->Text = "1950"; textBox_MID_L->Text = "100";
			textBox_SHORT_Z->Text = "2450"; textBox_SHORT_L->Text = "100";
			textBox_B1_GND_Z->Text =  "0"; textBox_B1_GND_L->Text = "1000";
			textBox_B2_GND_Z->Text = "3005"; textBox_B2_GND_L->Text = "995";
			textBox_B0_Z->Text = "4005"; textBox_B0_L->Text = "95";


			textBox_B2_add_GND_Z->Text = "4105"; textBox_B2_add_GND_L->Text = "1500";
			textBox_U_M_Z->Text = "2722"; textBox_U_M_L->Text = "10";		
			textBox_U_N_Z->Text = "2822"; textBox_U_N_L->Text = "10";
			
			textBox_Ro_BH->Text = "1.0";
			textBox_D_BH_mm->Text = "147"; 
			textBox_R_PZ_R_BH->Text = "1";
			textBox_Ro_PZ->Text = "1.0";
			textBox_Ro_stratum->Text = "1.0";

			////////////////////////////////////////////////////////////////
			comboBox_choice->SelectedIndex = 1;//log
			textBox_Ro_BH_step->Text = "1 1 1";
			textBox_D_BH_step_mm->Text = "140 1.09 250";
			textBox_R_PZ_R_BH_step->Text = "1.1 1.1 10";
			textBox_Ro_PZ_step->Text = "0.01 2.0 100";
			textBox_Ro_stratum_step->Text = "1 2.5 100000";
			
			checkBox_Ro_BH_1->Checked = true;
			comboBox_work_type->SelectedIndex = 0;

			tableLayoutPanel_chart->Visible = false;
			comboBox_shart_curves_X->SelectedItem = "Ro_stratum / Ro_BH";
			comboBox_shart_curves_Y->SelectedItem = "Ia";
			comboBox_D_BH->SelectedIndex = 0;
			comboBox_D_PZ->SelectedIndex = 0;
			comboBox_Ro_PZ->SelectedIndex = 0;


			//dataGridView_electrodes->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::ColumnHeader;
			dataGridView_electrodes->DefaultCellStyle->SelectionBackColor;
			dataGridView_electrodes->SelectionMode = DataGridViewSelectionMode::FullRowSelect;
			dataGridView_electrodes->EditMode = DataGridViewEditMode::EditOnEnter;
			ResizeGrid(dataGridView_electrodes);
			
			dataGridView_grid_param->RowHeadersVisible = false;
			dataGridView_grid_param->Rows->Add();
			DataGridViewRow^ row = dataGridView_grid_param->Rows[0];
			row->Cells["dr_start_mm"]->Value = "1";
			row->Cells["q_r"]->Value = "1.1";
			row->Cells["r_max_m"]->Value = "3.5";
			row->Cells["dz_base_mm"]->Value = "5";
			row->Cells["q_z_tail"]->Value = "1.15";
			row->Cells["dz_fine_mm"]->Value = "1";
			row->Cells["threshold_mm"]->Value = "20";
			row->Cells["D_sonde"]->Value = "90";
			row->Cells["tail"]->Value = "2.5";
			dataGridView_grid_param->AllowUserToAddRows = false;

			dataGridView_envir->RowHeadersVisible = false;
			dataGridView_envir->Rows->Add();
			DataGridViewRow^ row1 = dataGridView_envir->Rows[0];
			row1->Cells["Ro_BH"]->Value = "1.0";
			row1->Cells["D_BH_mm"]->Value = "147";
			row1->Cells["D_PZ_D_BH"]->Value = "1";
			row1->Cells["Ro_PZ"]->Value = "1.0";
			row1->Cells["Ro_stratum"]->Value = "1.0";
			dataGridView_envir->AllowUserToAddRows = false;

			
		}

	protected:
		/// <summary>
	/// Освободить все используемые ресурсы.
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel1;
	protected:
	private: System::Windows::Forms::Label^  label1;

	private: System::Windows::Forms::TextBox^  textBox_R_PZ_R_BH_step;

	private: System::Windows::Forms::TextBox^  textBox_D_BH_step_mm;

	private: System::Windows::Forms::TextBox^  textBox_Ro_stratum_step;


	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::TextBox^  textBox_Ro_PZ_step;
	private: System::Windows::Forms::TextBox^  textBox_B2_add_GND_Z;

	private: System::Windows::Forms::TextBox^  textBox_B2_GND_Z;

	private: System::Windows::Forms::TextBox^  textBox_B1_GND_Z;

	private: System::Windows::Forms::Label^  label8;
	private: System::Windows::Forms::Label^  label11;
	private: System::Windows::Forms::Label^  label12;
	private: System::Windows::Forms::Label^  label13;
	private: System::Windows::Forms::Label^  label14;
	private: System::Windows::Forms::TextBox^  textBox_B1_GND_L;
	private: System::Windows::Forms::TextBox^  textBox_B2_GND_L;
	private: System::Windows::Forms::TextBox^  textBox_B2_add_GND_L;

	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel3;
	private: System::Windows::Forms::Label^  label9;
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel4;
	private: System::Windows::Forms::Label^  label17;
	private: System::Windows::Forms::Label^  label18;
	private: System::Windows::Forms::Label^  label19;
	private: System::Windows::Forms::TextBox^  textBox_SHORT_Z;
	private: System::Windows::Forms::TextBox^  textBox_MID_Z;
	private: System::Windows::Forms::TextBox^  textBox_LONG_L;
	private: System::Windows::Forms::Label^  label20;
	private: System::Windows::Forms::TextBox^  textBox_MID_L;
	private: System::Windows::Forms::TextBox^  textBox_LONG_Z;
	private: System::Windows::Forms::TextBox^  textBox_SHORT_L;
	private: System::Windows::Forms::Label^  label21;
	private: System::Windows::Forms::Label^  label22;
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel5;
	private: System::Windows::Forms::Label^  label23;
	private: System::Windows::Forms::Label^  label24;
	private: System::Windows::Forms::Label^  label25;
	private: System::Windows::Forms::TextBox^  textBox_U_M_Z;
	private: System::Windows::Forms::TextBox^  textBox_U_N_Z;
	private: System::Windows::Forms::TextBox^  textBox_B0_L;
	private: System::Windows::Forms::Label^  label26;
	private: System::Windows::Forms::TextBox^  textBox_U_N_L;
	private: System::Windows::Forms::TextBox^  textBox_B0_Z;
	private: System::Windows::Forms::TextBox^  textBox_U_M_L;
	private: System::Windows::Forms::Label^  label27;
	private: System::Windows::Forms::Label^  label28;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::TextBox^  textBox_D_sonde;
private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel_tool_param;

	private: System::Windows::Forms::Label^  label16;
	private: System::Windows::Forms::Label^  label15;
	private: System::Windows::Forms::Label^  label10;
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel6;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::Label^  label29;
	private: System::Windows::Forms::Label^  label30;
	private: System::Windows::Forms::Label^  label_;
	private: System::Windows::Forms::Label^  label32;
private: System::Windows::Forms::Label^  label_q_z_tail;

	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel7;
private: System::Windows::Forms::TextBox^  textBox_q_z_tail;



	private: System::Windows::Forms::TextBox^  textBox_tail;
private: System::Windows::Forms::TextBox^  textBox_dz_base_mm;
private: System::Windows::Forms::TextBox^  textBox_dr_start_mm;

	private: System::Windows::Forms::TextBox^  textBox_q_r;
	private: System::Windows::Forms::Label^  label31;
	private: System::Windows::Forms::Label^  label33;
private: System::Windows::Forms::Button^  button_calk_one;

private: System::Windows::Forms::TextBox^  textBox_r_max_m;

private: System::Windows::Forms::Label^  label34;
private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel8;
private: System::Windows::Forms::TextBox^  textBox_R_PZ_R_BH;

private: System::Windows::Forms::TextBox^  textBox_D_BH_mm;

private: System::Windows::Forms::TextBox^  textBox_Ro_stratum;


private: System::Windows::Forms::Label^  label35;
private: System::Windows::Forms::Label^  label36;
private: System::Windows::Forms::Label^  label37;
private: System::Windows::Forms::TextBox^  textBox_Ro_PZ;

private: System::Windows::Forms::Label^  label39;
private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel_grid_param;

private: System::Windows::Forms::Button^  button_set;

private: System::Windows::Forms::Button^  button_calk_pallete;
private: System::Windows::Forms::ComboBox^  comboBox_choice;
private: System::Windows::Forms::CheckBox^  checkBox_Ro_BH_1;
private: System::Windows::Forms::TextBox^  textBox_threshold_mm;

private: System::Windows::Forms::TextBox^  textBox_dz_fine_mm;

private: System::Windows::Forms::Label^  label40;
private: System::Windows::Forms::Label^  label41;
private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel_envir_param;

private: System::Windows::Forms::Label^  label42;
private: System::Windows::Forms::TextBox^  textBox_Ro_BH;
private: System::Windows::Forms::Label^  label38;
private: System::Windows::Forms::ComboBox^  comboBox_work_type;
private: System::Windows::Forms::Button^  button_calc_four;
private: System::Windows::Forms::PictureBox^  pictureBox_U;
private: System::Windows::Forms::Panel^  panel_pallete_param;


private: System::Windows::Forms::MenuStrip^  menuStrip1;
private: System::Windows::Forms::ToolStripMenuItem^  mENUToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  Open_Pallete_file_ToolStripMenuItem;

private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel_chart;

private: System::Windows::Forms::DataVisualization::Charting::Chart^  chart_;

private: System::Windows::Forms::Panel^  panel2;

private: System::Windows::Forms::Panel^  panel3;
private: System::Windows::Forms::ComboBox^  comboBox_shart_curves_X;

private: System::Windows::Forms::ComboBox^  comboBox_shart_curves_Y;


private: System::Windows::Forms::Button^  button_do_chart;
private: System::Windows::Forms::Button^  button_calc_show;
private: System::Windows::Forms::Label^  label45;
private: System::Windows::Forms::ComboBox^  comboBox_Ro_PZ;
private: System::Windows::Forms::Label^  label47;
private: System::Windows::Forms::ComboBox^  comboBox_D_PZ;


private: System::Windows::Forms::Label^  label46;
private: System::Windows::Forms::Label^  label44;
private: System::Windows::Forms::Label^  label43;
private: System::Windows::Forms::ComboBox^  comboBox_D_BH;
private: System::Windows::Forms::OpenFileDialog^  openFileDialog1;
private: System::Windows::Forms::SaveFileDialog^  saveFileDialog1;
private: System::Windows::Forms::Label^  label_Y;
private: System::Windows::Forms::Label^  label_X;

private: System::Windows::Forms::Button^  button_inversion;
private: System::Windows::Forms::CheckBox^  checkBox_leg;
private: System::Windows::Forms::DataGridView^  dataGridView_electrodes;





private: System::Windows::Forms::Panel^  panel1;
private: System::Windows::Forms::Button^  button1_add_work_mode;
private: System::Windows::Forms::Button^  button_set_electrodes;







private: System::Windows::Forms::DataGridViewTextBoxColumn^  name;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  z_start;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  z_end;
private: System::Windows::Forms::ComboBox^  comboBox_work_mode;
private: System::Windows::Forms::ToolStripMenuItem^  loadPresetToolStripMenuItem;










private: System::Windows::Forms::Label^  label48;
private: System::Windows::Forms::Label^  label4;
private: System::Windows::Forms::TextBox^  textBox_Ro_BH_step;









private: System::Windows::Forms::DataGridView^  dataGridView_grid_param;
private: System::Windows::Forms::DataGridView^  dataGridView_envir;






private: System::Windows::Forms::Label^  label49;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  Ro_BH;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  D_BH_mm;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  D_PZ_D_BH;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  Ro_PZ;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  Ro_stratum;
private: System::Windows::Forms::Button^  button_calk_one_;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  D_sonde;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  dr_start_mm;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  q_r;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  r_max_m;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  dz_base_mm;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  dz_fine_mm;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  tail;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  q_z_tail;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  threshold_mm;
private: System::Windows::Forms::ToolStripMenuItem^  savePresetToolStripMenuItem;
private: System::Windows::Forms::Button^  button_calk_n;





private: System::ComponentModel::IContainer^  components;



	private:
		/// <summary>
		/// Обязательная переменная конструктора.
		/// </summary>


#pragma region Windows Form Designer generated code
		
		void InitializeComponent(void)
		{
			System::Windows::Forms::DataVisualization::Charting::ChartArea^  chartArea1 = (gcnew System::Windows::Forms::DataVisualization::Charting::ChartArea());
			System::Windows::Forms::DataVisualization::Charting::Legend^  legend1 = (gcnew System::Windows::Forms::DataVisualization::Charting::Legend());
			System::Windows::Forms::DataVisualization::Charting::Series^  series1 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::Series^  series2 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::Series^  series3 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::Series^  series4 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::Series^  series5 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::Series^  series6 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::Series^  series7 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::Series^  series8 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::Series^  series9 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::Series^  series10 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::Series^  series11 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::Series^  series12 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::Series^  series13 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::Series^  series14 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::Series^  series15 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::Series^  series16 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::Series^  series17 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			this->tableLayoutPanel1 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->textBox_D_BH_step_mm = (gcnew System::Windows::Forms::TextBox());
			this->textBox_R_PZ_R_BH_step = (gcnew System::Windows::Forms::TextBox());
			this->textBox_Ro_PZ_step = (gcnew System::Windows::Forms::TextBox());
			this->textBox_Ro_stratum_step = (gcnew System::Windows::Forms::TextBox());
			this->textBox_B2_add_GND_Z = (gcnew System::Windows::Forms::TextBox());
			this->textBox_B2_GND_Z = (gcnew System::Windows::Forms::TextBox());
			this->textBox_B1_GND_Z = (gcnew System::Windows::Forms::TextBox());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->label12 = (gcnew System::Windows::Forms::Label());
			this->label13 = (gcnew System::Windows::Forms::Label());
			this->label14 = (gcnew System::Windows::Forms::Label());
			this->textBox_B1_GND_L = (gcnew System::Windows::Forms::TextBox());
			this->textBox_B2_GND_L = (gcnew System::Windows::Forms::TextBox());
			this->textBox_B2_add_GND_L = (gcnew System::Windows::Forms::TextBox());
			this->tableLayoutPanel3 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->tableLayoutPanel4 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->label17 = (gcnew System::Windows::Forms::Label());
			this->label18 = (gcnew System::Windows::Forms::Label());
			this->textBox_SHORT_Z = (gcnew System::Windows::Forms::TextBox());
			this->textBox_MID_Z = (gcnew System::Windows::Forms::TextBox());
			this->label20 = (gcnew System::Windows::Forms::Label());
			this->textBox_MID_L = (gcnew System::Windows::Forms::TextBox());
			this->textBox_LONG_Z = (gcnew System::Windows::Forms::TextBox());
			this->label21 = (gcnew System::Windows::Forms::Label());
			this->label22 = (gcnew System::Windows::Forms::Label());
			this->label19 = (gcnew System::Windows::Forms::Label());
			this->textBox_LONG_L = (gcnew System::Windows::Forms::TextBox());
			this->textBox_SHORT_L = (gcnew System::Windows::Forms::TextBox());
			this->tableLayoutPanel5 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->label23 = (gcnew System::Windows::Forms::Label());
			this->label24 = (gcnew System::Windows::Forms::Label());
			this->label25 = (gcnew System::Windows::Forms::Label());
			this->textBox_U_M_Z = (gcnew System::Windows::Forms::TextBox());
			this->textBox_U_N_Z = (gcnew System::Windows::Forms::TextBox());
			this->textBox_B0_L = (gcnew System::Windows::Forms::TextBox());
			this->label26 = (gcnew System::Windows::Forms::Label());
			this->textBox_U_N_L = (gcnew System::Windows::Forms::TextBox());
			this->textBox_B0_Z = (gcnew System::Windows::Forms::TextBox());
			this->textBox_U_M_L = (gcnew System::Windows::Forms::TextBox());
			this->label27 = (gcnew System::Windows::Forms::Label());
			this->label28 = (gcnew System::Windows::Forms::Label());
			this->comboBox_work_type = (gcnew System::Windows::Forms::ComboBox());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->textBox_D_sonde = (gcnew System::Windows::Forms::TextBox());
			this->tableLayoutPanel_tool_param = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->button_calc_four = (gcnew System::Windows::Forms::Button());
			this->button_calk_one = (gcnew System::Windows::Forms::Button());
			this->label16 = (gcnew System::Windows::Forms::Label());
			this->label15 = (gcnew System::Windows::Forms::Label());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->tableLayoutPanel6 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->label29 = (gcnew System::Windows::Forms::Label());
			this->label30 = (gcnew System::Windows::Forms::Label());
			this->label_ = (gcnew System::Windows::Forms::Label());
			this->label32 = (gcnew System::Windows::Forms::Label());
			this->label_q_z_tail = (gcnew System::Windows::Forms::Label());
			this->tableLayoutPanel7 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->label41 = (gcnew System::Windows::Forms::Label());
			this->textBox_dz_fine_mm = (gcnew System::Windows::Forms::TextBox());
			this->label40 = (gcnew System::Windows::Forms::Label());
			this->textBox_r_max_m = (gcnew System::Windows::Forms::TextBox());
			this->label34 = (gcnew System::Windows::Forms::Label());
			this->textBox_dr_start_mm = (gcnew System::Windows::Forms::TextBox());
			this->textBox_q_r = (gcnew System::Windows::Forms::TextBox());
			this->textBox_q_z_tail = (gcnew System::Windows::Forms::TextBox());
			this->textBox_dz_base_mm = (gcnew System::Windows::Forms::TextBox());
			this->textBox_tail = (gcnew System::Windows::Forms::TextBox());
			this->textBox_threshold_mm = (gcnew System::Windows::Forms::TextBox());
			this->label31 = (gcnew System::Windows::Forms::Label());
			this->label33 = (gcnew System::Windows::Forms::Label());
			this->tableLayoutPanel8 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->textBox_Ro_BH = (gcnew System::Windows::Forms::TextBox());
			this->label38 = (gcnew System::Windows::Forms::Label());
			this->label35 = (gcnew System::Windows::Forms::Label());
			this->textBox_Ro_stratum = (gcnew System::Windows::Forms::TextBox());
			this->textBox_Ro_PZ = (gcnew System::Windows::Forms::TextBox());
			this->label39 = (gcnew System::Windows::Forms::Label());
			this->label37 = (gcnew System::Windows::Forms::Label());
			this->textBox_R_PZ_R_BH = (gcnew System::Windows::Forms::TextBox());
			this->textBox_D_BH_mm = (gcnew System::Windows::Forms::TextBox());
			this->label36 = (gcnew System::Windows::Forms::Label());
			this->tableLayoutPanel_grid_param = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->button_set = (gcnew System::Windows::Forms::Button());
			this->button_calk_pallete = (gcnew System::Windows::Forms::Button());
			this->comboBox_choice = (gcnew System::Windows::Forms::ComboBox());
			this->checkBox_Ro_BH_1 = (gcnew System::Windows::Forms::CheckBox());
			this->tableLayoutPanel_envir_param = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->label42 = (gcnew System::Windows::Forms::Label());
			this->pictureBox_U = (gcnew System::Windows::Forms::PictureBox());
			this->panel_pallete_param = (gcnew System::Windows::Forms::Panel());
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->mENUToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->Open_Pallete_file_ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->loadPresetToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->savePresetToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->tableLayoutPanel_chart = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->panel2 = (gcnew System::Windows::Forms::Panel());
			this->comboBox_Ro_PZ = (gcnew System::Windows::Forms::ComboBox());
			this->label47 = (gcnew System::Windows::Forms::Label());
			this->comboBox_D_PZ = (gcnew System::Windows::Forms::ComboBox());
			this->label46 = (gcnew System::Windows::Forms::Label());
			this->label44 = (gcnew System::Windows::Forms::Label());
			this->label43 = (gcnew System::Windows::Forms::Label());
			this->comboBox_D_BH = (gcnew System::Windows::Forms::ComboBox());
			this->button_do_chart = (gcnew System::Windows::Forms::Button());
			this->comboBox_shart_curves_Y = (gcnew System::Windows::Forms::ComboBox());
			this->panel3 = (gcnew System::Windows::Forms::Panel());
			this->label_Y = (gcnew System::Windows::Forms::Label());
			this->label_X = (gcnew System::Windows::Forms::Label());
			this->label45 = (gcnew System::Windows::Forms::Label());
			this->comboBox_shart_curves_X = (gcnew System::Windows::Forms::ComboBox());
			this->chart_ = (gcnew System::Windows::Forms::DataVisualization::Charting::Chart());
			this->button_calc_show = (gcnew System::Windows::Forms::Button());
			this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
			this->saveFileDialog1 = (gcnew System::Windows::Forms::SaveFileDialog());
			this->button_inversion = (gcnew System::Windows::Forms::Button());
			this->checkBox_leg = (gcnew System::Windows::Forms::CheckBox());
			this->dataGridView_electrodes = (gcnew System::Windows::Forms::DataGridView());
			this->name = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->z_start = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->z_end = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->comboBox_work_mode = (gcnew System::Windows::Forms::ComboBox());
			this->button_set_electrodes = (gcnew System::Windows::Forms::Button());
			this->button1_add_work_mode = (gcnew System::Windows::Forms::Button());
			this->label48 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->textBox_Ro_BH_step = (gcnew System::Windows::Forms::TextBox());
			this->dataGridView_grid_param = (gcnew System::Windows::Forms::DataGridView());
			this->D_sonde = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->dr_start_mm = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->q_r = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->r_max_m = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->dz_base_mm = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->dz_fine_mm = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->tail = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->q_z_tail = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->threshold_mm = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->dataGridView_envir = (gcnew System::Windows::Forms::DataGridView());
			this->Ro_BH = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->D_BH_mm = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->D_PZ_D_BH = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Ro_PZ = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Ro_stratum = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->label49 = (gcnew System::Windows::Forms::Label());
			this->button_calk_one_ = (gcnew System::Windows::Forms::Button());
			this->button_calk_n = (gcnew System::Windows::Forms::Button());
			this->tableLayoutPanel1->SuspendLayout();
			this->tableLayoutPanel3->SuspendLayout();
			this->tableLayoutPanel4->SuspendLayout();
			this->tableLayoutPanel5->SuspendLayout();
			this->tableLayoutPanel_tool_param->SuspendLayout();
			this->tableLayoutPanel6->SuspendLayout();
			this->tableLayoutPanel7->SuspendLayout();
			this->tableLayoutPanel8->SuspendLayout();
			this->tableLayoutPanel_grid_param->SuspendLayout();
			this->tableLayoutPanel_envir_param->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox_U))->BeginInit();
			this->panel_pallete_param->SuspendLayout();
			this->menuStrip1->SuspendLayout();
			this->tableLayoutPanel_chart->SuspendLayout();
			this->panel2->SuspendLayout();
			this->panel3->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->chart_))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView_electrodes))->BeginInit();
			this->panel1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView_grid_param))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView_envir))->BeginInit();
			this->SuspendLayout();
			// 
			// tableLayoutPanel1
			// 
			this->tableLayoutPanel1->CellBorderStyle = System::Windows::Forms::TableLayoutPanelCellBorderStyle::Single;
			this->tableLayoutPanel1->ColumnCount = 3;
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
				140)));
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
				820)));
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
				145)));
			this->tableLayoutPanel1->Controls->Add(this->label1, 0, 0);
			this->tableLayoutPanel1->Controls->Add(this->label2, 0, 1);
			this->tableLayoutPanel1->Controls->Add(this->label5, 0, 2);
			this->tableLayoutPanel1->Controls->Add(this->label3, 0, 3);
			this->tableLayoutPanel1->Controls->Add(this->textBox_D_BH_step_mm, 1, 0);
			this->tableLayoutPanel1->Controls->Add(this->textBox_R_PZ_R_BH_step, 1, 1);
			this->tableLayoutPanel1->Controls->Add(this->textBox_Ro_PZ_step, 1, 2);
			this->tableLayoutPanel1->Controls->Add(this->textBox_Ro_stratum_step, 1, 3);
			this->tableLayoutPanel1->Location = System::Drawing::Point(16, 36);
			this->tableLayoutPanel1->Name = L"tableLayoutPanel1";
			this->tableLayoutPanel1->RowCount = 5;
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 30)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 30)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 30)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 30)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 20)));
			this->tableLayoutPanel1->Size = System::Drawing::Size(658, 128);
			this->tableLayoutPanel1->TabIndex = 0;
			// 
			// label1
			// 
			this->label1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label1->Location = System::Drawing::Point(4, 4);
			this->label1->Margin = System::Windows::Forms::Padding(3);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(134, 20);
			this->label1->TabIndex = 0;
			this->label1->Text = L"D_BH_mm";
			this->label1->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// label2
			// 
			this->label2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label2->Location = System::Drawing::Point(4, 35);
			this->label2->Margin = System::Windows::Forms::Padding(3);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(134, 20);
			this->label2->TabIndex = 2;
			this->label2->Text = L"D_PZ / D_BH";
			this->label2->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// label5
			// 
			this->label5->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label5->AutoSize = true;
			this->label5->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label5->Location = System::Drawing::Point(4, 66);
			this->label5->Margin = System::Windows::Forms::Padding(3);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(134, 20);
			this->label5->TabIndex = 5;
			this->label5->Text = L"Ro_PZ";
			this->label5->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// label3
			// 
			this->label3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label3->AutoSize = true;
			this->label3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label3->Location = System::Drawing::Point(4, 97);
			this->label3->Margin = System::Windows::Forms::Padding(3);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(134, 20);
			this->label3->TabIndex = 3;
			this->label3->Text = L"Ro_STRATUM";
			this->label3->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// textBox_D_BH_step_mm
			// 
			this->textBox_D_BH_step_mm->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left));
			this->textBox_D_BH_step_mm->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(204)));
			this->textBox_D_BH_step_mm->Location = System::Drawing::Point(145, 4);
			this->textBox_D_BH_step_mm->Multiline = true;
			this->textBox_D_BH_step_mm->Name = L"textBox_D_BH_step_mm";
			this->textBox_D_BH_step_mm->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->textBox_D_BH_step_mm->Size = System::Drawing::Size(512, 24);
			this->textBox_D_BH_step_mm->TabIndex = 2;
			this->textBox_D_BH_step_mm->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit3);
			// 
			// textBox_R_PZ_R_BH_step
			// 
			this->textBox_R_PZ_R_BH_step->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left));
			this->textBox_R_PZ_R_BH_step->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(204)));
			this->textBox_R_PZ_R_BH_step->Location = System::Drawing::Point(145, 35);
			this->textBox_R_PZ_R_BH_step->Multiline = true;
			this->textBox_R_PZ_R_BH_step->Name = L"textBox_R_PZ_R_BH_step";
			this->textBox_R_PZ_R_BH_step->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->textBox_R_PZ_R_BH_step->Size = System::Drawing::Size(512, 24);
			this->textBox_R_PZ_R_BH_step->TabIndex = 2;
			this->textBox_R_PZ_R_BH_step->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit3);
			// 
			// textBox_Ro_PZ_step
			// 
			this->textBox_Ro_PZ_step->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left));
			this->textBox_Ro_PZ_step->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->textBox_Ro_PZ_step->Location = System::Drawing::Point(145, 66);
			this->textBox_Ro_PZ_step->Multiline = true;
			this->textBox_Ro_PZ_step->Name = L"textBox_Ro_PZ_step";
			this->textBox_Ro_PZ_step->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->textBox_Ro_PZ_step->Size = System::Drawing::Size(512, 24);
			this->textBox_Ro_PZ_step->TabIndex = 6;
			this->textBox_Ro_PZ_step->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit3);
			// 
			// textBox_Ro_stratum_step
			// 
			this->textBox_Ro_stratum_step->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left));
			this->textBox_Ro_stratum_step->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(204)));
			this->textBox_Ro_stratum_step->Location = System::Drawing::Point(145, 97);
			this->textBox_Ro_stratum_step->Multiline = true;
			this->textBox_Ro_stratum_step->Name = L"textBox_Ro_stratum_step";
			this->textBox_Ro_stratum_step->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->textBox_Ro_stratum_step->Size = System::Drawing::Size(512, 24);
			this->textBox_Ro_stratum_step->TabIndex = 2;
			this->textBox_Ro_stratum_step->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit3);
			// 
			// textBox_B2_add_GND_Z
			// 
			this->textBox_B2_add_GND_Z->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_B2_add_GND_Z->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(204)));
			this->textBox_B2_add_GND_Z->Location = System::Drawing::Point(155, 66);
			this->textBox_B2_add_GND_Z->Name = L"textBox_B2_add_GND_Z";
			this->textBox_B2_add_GND_Z->Size = System::Drawing::Size(74, 27);
			this->textBox_B2_add_GND_Z->TabIndex = 2;
			this->textBox_B2_add_GND_Z->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit1);
			// 
			// textBox_B2_GND_Z
			// 
			this->textBox_B2_GND_Z->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_B2_GND_Z->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->textBox_B2_GND_Z->Location = System::Drawing::Point(155, 35);
			this->textBox_B2_GND_Z->Name = L"textBox_B2_GND_Z";
			this->textBox_B2_GND_Z->Size = System::Drawing::Size(74, 27);
			this->textBox_B2_GND_Z->TabIndex = 2;
			this->textBox_B2_GND_Z->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit1);
			// 
			// textBox_B1_GND_Z
			// 
			this->textBox_B1_GND_Z->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_B1_GND_Z->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->textBox_B1_GND_Z->Location = System::Drawing::Point(155, 4);
			this->textBox_B1_GND_Z->Name = L"textBox_B1_GND_Z";
			this->textBox_B1_GND_Z->Size = System::Drawing::Size(74, 27);
			this->textBox_B1_GND_Z->TabIndex = 6;
			this->textBox_B1_GND_Z->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit2);
			// 
			// label8
			// 
			this->label8->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label8->AutoSize = true;
			this->label8->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label8->Location = System::Drawing::Point(4, 35);
			this->label8->Margin = System::Windows::Forms::Padding(3);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(144, 20);
			this->label8->TabIndex = 7;
			this->label8->Text = L"B2  Z mm";
			this->label8->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// label11
			// 
			this->label11->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label11->AutoSize = true;
			this->label11->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label11->Location = System::Drawing::Point(4, 66);
			this->label11->Margin = System::Windows::Forms::Padding(3);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(144, 20);
			this->label11->TabIndex = 8;
			this->label11->Text = L"B2~  Z mm";
			this->label11->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// label12
			// 
			this->label12->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label12->AutoSize = true;
			this->label12->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label12->Location = System::Drawing::Point(236, 4);
			this->label12->Margin = System::Windows::Forms::Padding(3);
			this->label12->Name = L"label12";
			this->label12->Size = System::Drawing::Size(74, 20);
			this->label12->TabIndex = 9;
			this->label12->Text = L"L  mm";
			this->label12->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// label13
			// 
			this->label13->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label13->AutoSize = true;
			this->label13->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label13->Location = System::Drawing::Point(236, 35);
			this->label13->Margin = System::Windows::Forms::Padding(3);
			this->label13->Name = L"label13";
			this->label13->Size = System::Drawing::Size(74, 20);
			this->label13->TabIndex = 10;
			this->label13->Text = L"L  mm";
			this->label13->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// label14
			// 
			this->label14->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label14->AutoSize = true;
			this->label14->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label14->Location = System::Drawing::Point(236, 66);
			this->label14->Margin = System::Windows::Forms::Padding(3);
			this->label14->Name = L"label14";
			this->label14->Size = System::Drawing::Size(74, 20);
			this->label14->TabIndex = 11;
			this->label14->Text = L"L  mm";
			this->label14->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// textBox_B1_GND_L
			// 
			this->textBox_B1_GND_L->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_B1_GND_L->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->textBox_B1_GND_L->Location = System::Drawing::Point(317, 4);
			this->textBox_B1_GND_L->Name = L"textBox_B1_GND_L";
			this->textBox_B1_GND_L->Size = System::Drawing::Size(206, 27);
			this->textBox_B1_GND_L->TabIndex = 12;
			this->textBox_B1_GND_L->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit1);
			// 
			// textBox_B2_GND_L
			// 
			this->textBox_B2_GND_L->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_B2_GND_L->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->textBox_B2_GND_L->Location = System::Drawing::Point(317, 35);
			this->textBox_B2_GND_L->Name = L"textBox_B2_GND_L";
			this->textBox_B2_GND_L->Size = System::Drawing::Size(206, 27);
			this->textBox_B2_GND_L->TabIndex = 13;
			this->textBox_B2_GND_L->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit1);
			// 
			// textBox_B2_add_GND_L
			// 
			this->textBox_B2_add_GND_L->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_B2_add_GND_L->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(204)));
			this->textBox_B2_add_GND_L->Location = System::Drawing::Point(317, 66);
			this->textBox_B2_add_GND_L->Name = L"textBox_B2_add_GND_L";
			this->textBox_B2_add_GND_L->Size = System::Drawing::Size(206, 27);
			this->textBox_B2_add_GND_L->TabIndex = 14;
			this->textBox_B2_add_GND_L->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit1);
			// 
			// tableLayoutPanel3
			// 
			this->tableLayoutPanel3->CellBorderStyle = System::Windows::Forms::TableLayoutPanelCellBorderStyle::Single;
			this->tableLayoutPanel3->ColumnCount = 4;
			this->tableLayoutPanel3->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
				150)));
			this->tableLayoutPanel3->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
				80)));
			this->tableLayoutPanel3->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
				80)));
			this->tableLayoutPanel3->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
				212)));
			this->tableLayoutPanel3->Controls->Add(this->label9, 0, 0);
			this->tableLayoutPanel3->Controls->Add(this->label8, 0, 1);
			this->tableLayoutPanel3->Controls->Add(this->label11, 0, 2);
			this->tableLayoutPanel3->Controls->Add(this->textBox_B1_GND_Z, 1, 0);
			this->tableLayoutPanel3->Controls->Add(this->textBox_B2_GND_Z, 1, 1);
			this->tableLayoutPanel3->Controls->Add(this->textBox_B2_add_GND_L, 3, 2);
			this->tableLayoutPanel3->Controls->Add(this->label14, 2, 2);
			this->tableLayoutPanel3->Controls->Add(this->textBox_B2_GND_L, 3, 1);
			this->tableLayoutPanel3->Controls->Add(this->textBox_B2_add_GND_Z, 1, 2);
			this->tableLayoutPanel3->Controls->Add(this->textBox_B1_GND_L, 3, 0);
			this->tableLayoutPanel3->Controls->Add(this->label12, 2, 0);
			this->tableLayoutPanel3->Controls->Add(this->label13, 2, 1);
			this->tableLayoutPanel3->Location = System::Drawing::Point(4, 107);
			this->tableLayoutPanel3->Name = L"tableLayoutPanel3";
			this->tableLayoutPanel3->RowCount = 3;
			this->tableLayoutPanel3->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 30)));
			this->tableLayoutPanel3->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 30)));
			this->tableLayoutPanel3->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 30)));
			this->tableLayoutPanel3->Size = System::Drawing::Size(397, 96);
			this->tableLayoutPanel3->TabIndex = 2;
			// 
			// label9
			// 
			this->label9->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label9->AutoSize = true;
			this->label9->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label9->Location = System::Drawing::Point(4, 4);
			this->label9->Margin = System::Windows::Forms::Padding(3);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(144, 20);
			this->label9->TabIndex = 0;
			this->label9->Text = L"B1  Z mm";
			this->label9->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// tableLayoutPanel4
			// 
			this->tableLayoutPanel4->CellBorderStyle = System::Windows::Forms::TableLayoutPanelCellBorderStyle::Single;
			this->tableLayoutPanel4->ColumnCount = 4;
			this->tableLayoutPanel4->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
				150)));
			this->tableLayoutPanel4->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
				80)));
			this->tableLayoutPanel4->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
				80)));
			this->tableLayoutPanel4->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
				212)));
			this->tableLayoutPanel4->Controls->Add(this->label17, 0, 0);
			this->tableLayoutPanel4->Controls->Add(this->label18, 0, 1);
			this->tableLayoutPanel4->Controls->Add(this->textBox_SHORT_Z, 1, 0);
			this->tableLayoutPanel4->Controls->Add(this->textBox_MID_Z, 1, 1);
			this->tableLayoutPanel4->Controls->Add(this->label20, 2, 2);
			this->tableLayoutPanel4->Controls->Add(this->textBox_MID_L, 3, 1);
			this->tableLayoutPanel4->Controls->Add(this->textBox_LONG_Z, 1, 2);
			this->tableLayoutPanel4->Controls->Add(this->label21, 2, 0);
			this->tableLayoutPanel4->Controls->Add(this->label22, 2, 1);
			this->tableLayoutPanel4->Controls->Add(this->label19, 0, 2);
			this->tableLayoutPanel4->Controls->Add(this->textBox_LONG_L, 3, 2);
			this->tableLayoutPanel4->Controls->Add(this->textBox_SHORT_L, 3, 0);
			this->tableLayoutPanel4->Location = System::Drawing::Point(4, 241);
			this->tableLayoutPanel4->Name = L"tableLayoutPanel4";
			this->tableLayoutPanel4->RowCount = 3;
			this->tableLayoutPanel4->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 30)));
			this->tableLayoutPanel4->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 30)));
			this->tableLayoutPanel4->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 30)));
			this->tableLayoutPanel4->Size = System::Drawing::Size(397, 96);
			this->tableLayoutPanel4->TabIndex = 3;
			// 
			// label17
			// 
			this->label17->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label17->AutoSize = true;
			this->label17->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label17->Location = System::Drawing::Point(4, 4);
			this->label17->Margin = System::Windows::Forms::Padding(3);
			this->label17->Name = L"label17";
			this->label17->Size = System::Drawing::Size(144, 20);
			this->label17->TabIndex = 0;
			this->label17->Text = L"SHORT Z mm";
			this->label17->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// label18
			// 
			this->label18->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label18->AutoSize = true;
			this->label18->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label18->Location = System::Drawing::Point(4, 35);
			this->label18->Margin = System::Windows::Forms::Padding(3);
			this->label18->Name = L"label18";
			this->label18->Size = System::Drawing::Size(144, 20);
			this->label18->TabIndex = 7;
			this->label18->Text = L"MID  Z mm";
			this->label18->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// textBox_SHORT_Z
			// 
			this->textBox_SHORT_Z->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_SHORT_Z->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->textBox_SHORT_Z->Location = System::Drawing::Point(155, 4);
			this->textBox_SHORT_Z->Name = L"textBox_SHORT_Z";
			this->textBox_SHORT_Z->Size = System::Drawing::Size(74, 27);
			this->textBox_SHORT_Z->TabIndex = 6;
			this->textBox_SHORT_Z->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit1);
			// 
			// textBox_MID_Z
			// 
			this->textBox_MID_Z->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_MID_Z->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->textBox_MID_Z->Location = System::Drawing::Point(155, 35);
			this->textBox_MID_Z->Name = L"textBox_MID_Z";
			this->textBox_MID_Z->Size = System::Drawing::Size(74, 27);
			this->textBox_MID_Z->TabIndex = 2;
			this->textBox_MID_Z->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit1);
			// 
			// label20
			// 
			this->label20->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label20->AutoSize = true;
			this->label20->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label20->Location = System::Drawing::Point(236, 66);
			this->label20->Margin = System::Windows::Forms::Padding(3);
			this->label20->Name = L"label20";
			this->label20->Size = System::Drawing::Size(74, 20);
			this->label20->TabIndex = 11;
			this->label20->Text = L"L  mm";
			this->label20->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// textBox_MID_L
			// 
			this->textBox_MID_L->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_MID_L->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->textBox_MID_L->Location = System::Drawing::Point(317, 35);
			this->textBox_MID_L->Name = L"textBox_MID_L";
			this->textBox_MID_L->Size = System::Drawing::Size(206, 27);
			this->textBox_MID_L->TabIndex = 13;
			this->textBox_MID_L->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit1);
			// 
			// textBox_LONG_Z
			// 
			this->textBox_LONG_Z->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_LONG_Z->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->textBox_LONG_Z->Location = System::Drawing::Point(155, 66);
			this->textBox_LONG_Z->Name = L"textBox_LONG_Z";
			this->textBox_LONG_Z->Size = System::Drawing::Size(74, 27);
			this->textBox_LONG_Z->TabIndex = 2;
			this->textBox_LONG_Z->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit1);
			// 
			// label21
			// 
			this->label21->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label21->AutoSize = true;
			this->label21->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label21->Location = System::Drawing::Point(236, 4);
			this->label21->Margin = System::Windows::Forms::Padding(3);
			this->label21->Name = L"label21";
			this->label21->Size = System::Drawing::Size(74, 20);
			this->label21->TabIndex = 9;
			this->label21->Text = L"L  mm";
			this->label21->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// label22
			// 
			this->label22->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label22->AutoSize = true;
			this->label22->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label22->Location = System::Drawing::Point(236, 35);
			this->label22->Margin = System::Windows::Forms::Padding(3);
			this->label22->Name = L"label22";
			this->label22->Size = System::Drawing::Size(74, 20);
			this->label22->TabIndex = 10;
			this->label22->Text = L"L  mm";
			this->label22->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// label19
			// 
			this->label19->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label19->AutoSize = true;
			this->label19->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label19->Location = System::Drawing::Point(4, 66);
			this->label19->Margin = System::Windows::Forms::Padding(3);
			this->label19->Name = L"label19";
			this->label19->Size = System::Drawing::Size(144, 20);
			this->label19->TabIndex = 8;
			this->label19->Text = L"LONG  Z mm";
			this->label19->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// textBox_LONG_L
			// 
			this->textBox_LONG_L->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_LONG_L->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->textBox_LONG_L->Location = System::Drawing::Point(317, 66);
			this->textBox_LONG_L->Name = L"textBox_LONG_L";
			this->textBox_LONG_L->Size = System::Drawing::Size(206, 27);
			this->textBox_LONG_L->TabIndex = 14;
			this->textBox_LONG_L->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit1);
			// 
			// textBox_SHORT_L
			// 
			this->textBox_SHORT_L->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_SHORT_L->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->textBox_SHORT_L->Location = System::Drawing::Point(317, 4);
			this->textBox_SHORT_L->Name = L"textBox_SHORT_L";
			this->textBox_SHORT_L->Size = System::Drawing::Size(206, 27);
			this->textBox_SHORT_L->TabIndex = 12;
			this->textBox_SHORT_L->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit1);
			// 
			// tableLayoutPanel5
			// 
			this->tableLayoutPanel5->CellBorderStyle = System::Windows::Forms::TableLayoutPanelCellBorderStyle::Single;
			this->tableLayoutPanel5->ColumnCount = 4;
			this->tableLayoutPanel5->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
				150)));
			this->tableLayoutPanel5->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
				80)));
			this->tableLayoutPanel5->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
				80)));
			this->tableLayoutPanel5->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
				212)));
			this->tableLayoutPanel5->Controls->Add(this->label23, 0, 0);
			this->tableLayoutPanel5->Controls->Add(this->label24, 0, 1);
			this->tableLayoutPanel5->Controls->Add(this->label25, 0, 2);
			this->tableLayoutPanel5->Controls->Add(this->textBox_U_M_Z, 1, 0);
			this->tableLayoutPanel5->Controls->Add(this->textBox_U_N_Z, 1, 1);
			this->tableLayoutPanel5->Controls->Add(this->textBox_B0_L, 3, 2);
			this->tableLayoutPanel5->Controls->Add(this->label26, 2, 2);
			this->tableLayoutPanel5->Controls->Add(this->textBox_U_N_L, 3, 1);
			this->tableLayoutPanel5->Controls->Add(this->textBox_B0_Z, 1, 2);
			this->tableLayoutPanel5->Controls->Add(this->textBox_U_M_L, 3, 0);
			this->tableLayoutPanel5->Controls->Add(this->label27, 2, 0);
			this->tableLayoutPanel5->Controls->Add(this->label28, 2, 1);
			this->tableLayoutPanel5->Controls->Add(this->comboBox_work_type, 0, 3);
			this->tableLayoutPanel5->Location = System::Drawing::Point(4, 375);
			this->tableLayoutPanel5->Name = L"tableLayoutPanel5";
			this->tableLayoutPanel5->RowCount = 4;
			this->tableLayoutPanel5->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 30)));
			this->tableLayoutPanel5->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 30)));
			this->tableLayoutPanel5->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 30)));
			this->tableLayoutPanel5->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 20)));
			this->tableLayoutPanel5->Size = System::Drawing::Size(397, 125);
			this->tableLayoutPanel5->TabIndex = 4;
			// 
			// label23
			// 
			this->label23->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label23->AutoSize = true;
			this->label23->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label23->Location = System::Drawing::Point(4, 4);
			this->label23->Margin = System::Windows::Forms::Padding(3);
			this->label23->Name = L"label23";
			this->label23->Size = System::Drawing::Size(144, 20);
			this->label23->TabIndex = 0;
			this->label23->Text = L"U M Z mm";
			this->label23->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// label24
			// 
			this->label24->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label24->AutoSize = true;
			this->label24->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label24->Location = System::Drawing::Point(4, 35);
			this->label24->Margin = System::Windows::Forms::Padding(3);
			this->label24->Name = L"label24";
			this->label24->Size = System::Drawing::Size(144, 20);
			this->label24->TabIndex = 7;
			this->label24->Text = L"U N  Z mm";
			this->label24->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// label25
			// 
			this->label25->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label25->AutoSize = true;
			this->label25->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label25->Location = System::Drawing::Point(4, 66);
			this->label25->Margin = System::Windows::Forms::Padding(3);
			this->label25->Name = L"label25";
			this->label25->Size = System::Drawing::Size(144, 20);
			this->label25->TabIndex = 8;
			this->label25->Text = L"B0  Z mm";
			this->label25->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// textBox_U_M_Z
			// 
			this->textBox_U_M_Z->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_U_M_Z->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->textBox_U_M_Z->Location = System::Drawing::Point(155, 4);
			this->textBox_U_M_Z->Name = L"textBox_U_M_Z";
			this->textBox_U_M_Z->Size = System::Drawing::Size(74, 27);
			this->textBox_U_M_Z->TabIndex = 6;
			this->textBox_U_M_Z->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit1);
			// 
			// textBox_U_N_Z
			// 
			this->textBox_U_N_Z->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_U_N_Z->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->textBox_U_N_Z->Location = System::Drawing::Point(155, 35);
			this->textBox_U_N_Z->Name = L"textBox_U_N_Z";
			this->textBox_U_N_Z->Size = System::Drawing::Size(74, 27);
			this->textBox_U_N_Z->TabIndex = 2;
			this->textBox_U_N_Z->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit1);
			// 
			// textBox_B0_L
			// 
			this->textBox_B0_L->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_B0_L->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->textBox_B0_L->Location = System::Drawing::Point(317, 66);
			this->textBox_B0_L->Name = L"textBox_B0_L";
			this->textBox_B0_L->Size = System::Drawing::Size(206, 27);
			this->textBox_B0_L->TabIndex = 14;
			this->textBox_B0_L->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit1);
			// 
			// label26
			// 
			this->label26->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label26->AutoSize = true;
			this->label26->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label26->Location = System::Drawing::Point(236, 66);
			this->label26->Margin = System::Windows::Forms::Padding(3);
			this->label26->Name = L"label26";
			this->label26->Size = System::Drawing::Size(74, 20);
			this->label26->TabIndex = 11;
			this->label26->Text = L"L  mm";
			this->label26->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// textBox_U_N_L
			// 
			this->textBox_U_N_L->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_U_N_L->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->textBox_U_N_L->Location = System::Drawing::Point(317, 35);
			this->textBox_U_N_L->Name = L"textBox_U_N_L";
			this->textBox_U_N_L->Size = System::Drawing::Size(206, 27);
			this->textBox_U_N_L->TabIndex = 13;
			this->textBox_U_N_L->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit1);
			// 
			// textBox_B0_Z
			// 
			this->textBox_B0_Z->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_B0_Z->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->textBox_B0_Z->Location = System::Drawing::Point(155, 66);
			this->textBox_B0_Z->Name = L"textBox_B0_Z";
			this->textBox_B0_Z->Size = System::Drawing::Size(74, 27);
			this->textBox_B0_Z->TabIndex = 2;
			this->textBox_B0_Z->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit1);
			// 
			// textBox_U_M_L
			// 
			this->textBox_U_M_L->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_U_M_L->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->textBox_U_M_L->Location = System::Drawing::Point(317, 4);
			this->textBox_U_M_L->Name = L"textBox_U_M_L";
			this->textBox_U_M_L->Size = System::Drawing::Size(206, 27);
			this->textBox_U_M_L->TabIndex = 12;
			this->textBox_U_M_L->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit1);
			// 
			// label27
			// 
			this->label27->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label27->AutoSize = true;
			this->label27->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label27->Location = System::Drawing::Point(236, 4);
			this->label27->Margin = System::Windows::Forms::Padding(3);
			this->label27->Name = L"label27";
			this->label27->Size = System::Drawing::Size(74, 20);
			this->label27->TabIndex = 9;
			this->label27->Text = L"L  mm";
			this->label27->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// label28
			// 
			this->label28->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label28->AutoSize = true;
			this->label28->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label28->Location = System::Drawing::Point(236, 35);
			this->label28->Margin = System::Windows::Forms::Padding(3);
			this->label28->Name = L"label28";
			this->label28->Size = System::Drawing::Size(74, 20);
			this->label28->TabIndex = 10;
			this->label28->Text = L"L  mm";
			this->label28->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// comboBox_work_type
			// 
			this->comboBox_work_type->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->comboBox_work_type->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->comboBox_work_type->FormattingEnabled = true;
			this->comboBox_work_type->Items->AddRange(gcnew cli::array< System::Object^  >(4) { L"SML", L"S", L"M", L"L" });
			this->comboBox_work_type->Location = System::Drawing::Point(4, 97);
			this->comboBox_work_type->Name = L"comboBox_work_type";
			this->comboBox_work_type->Size = System::Drawing::Size(144, 28);
			this->comboBox_work_type->TabIndex = 15;
			// 
			// label6
			// 
			this->label6->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label6->AutoSize = true;
			this->label6->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label6->Location = System::Drawing::Point(3, 3);
			this->label6->Margin = System::Windows::Forms::Padding(3);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(130, 20);
			this->label6->TabIndex = 4;
			this->label6->Text = L"D_sonde_mm";
			this->label6->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// textBox_D_sonde
			// 
			this->textBox_D_sonde->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_D_sonde->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->textBox_D_sonde->Location = System::Drawing::Point(139, 3);
			this->textBox_D_sonde->Name = L"textBox_D_sonde";
			this->textBox_D_sonde->Size = System::Drawing::Size(131, 27);
			this->textBox_D_sonde->TabIndex = 1;
			this->textBox_D_sonde->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit1);
			// 
			// tableLayoutPanel_tool_param
			// 
			this->tableLayoutPanel_tool_param->CellBorderStyle = System::Windows::Forms::TableLayoutPanelCellBorderStyle::Single;
			this->tableLayoutPanel_tool_param->ColumnCount = 1;
			this->tableLayoutPanel_tool_param->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				100)));
			this->tableLayoutPanel_tool_param->Controls->Add(this->button_calc_four, 0, 9);
			this->tableLayoutPanel_tool_param->Controls->Add(this->button_calk_one, 0, 8);
			this->tableLayoutPanel_tool_param->Controls->Add(this->label16, 0, 6);
			this->tableLayoutPanel_tool_param->Controls->Add(this->tableLayoutPanel5, 0, 7);
			this->tableLayoutPanel_tool_param->Controls->Add(this->label15, 0, 4);
			this->tableLayoutPanel_tool_param->Controls->Add(this->label10, 0, 2);
			this->tableLayoutPanel_tool_param->Controls->Add(this->tableLayoutPanel4, 0, 5);
			this->tableLayoutPanel_tool_param->Controls->Add(this->tableLayoutPanel6, 0, 1);
			this->tableLayoutPanel_tool_param->Controls->Add(this->label7, 0, 0);
			this->tableLayoutPanel_tool_param->Controls->Add(this->tableLayoutPanel3, 0, 3);
			this->tableLayoutPanel_tool_param->Location = System::Drawing::Point(1195, 40);
			this->tableLayoutPanel_tool_param->Name = L"tableLayoutPanel_tool_param";
			this->tableLayoutPanel_tool_param->RowCount = 10;
			this->tableLayoutPanel_tool_param->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute,
				30)));
			this->tableLayoutPanel_tool_param->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute,
				40)));
			this->tableLayoutPanel_tool_param->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute,
				30)));
			this->tableLayoutPanel_tool_param->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->tableLayoutPanel_tool_param->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute,
				30)));
			this->tableLayoutPanel_tool_param->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->tableLayoutPanel_tool_param->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute,
				30)));
			this->tableLayoutPanel_tool_param->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->tableLayoutPanel_tool_param->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute,
				40)));
			this->tableLayoutPanel_tool_param->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute,
				20)));
			this->tableLayoutPanel_tool_param->Size = System::Drawing::Size(405, 587);
			this->tableLayoutPanel_tool_param->TabIndex = 5;
			// 
			// button_calc_four
			// 
			this->button_calc_four->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->button_calc_four->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->button_calc_four->Location = System::Drawing::Point(4, 548);
			this->button_calc_four->Name = L"button_calc_four";
			this->button_calc_four->Size = System::Drawing::Size(397, 34);
			this->button_calc_four->TabIndex = 18;
			this->button_calc_four->Text = L"CALC FOUR";
			this->button_calc_four->UseVisualStyleBackColor = true;
			this->button_calc_four->Click += gcnew System::EventHandler(this, &MyForm::button_calc_four_Click);
			// 
			// button_calk_one
			// 
			this->button_calk_one->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->button_calk_one->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->button_calk_one->Location = System::Drawing::Point(4, 507);
			this->button_calk_one->Name = L"button_calk_one";
			this->button_calk_one->Size = System::Drawing::Size(397, 34);
			this->button_calk_one->TabIndex = 17;
			this->button_calk_one->Text = L"CALC ONE";
			this->button_calk_one->UseVisualStyleBackColor = true;
			this->button_calk_one->Click += gcnew System::EventHandler(this, &MyForm::button_calc_one_Clic);
			// 
			// label16
			// 
			this->label16->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label16->AutoSize = true;
			this->label16->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label16->Location = System::Drawing::Point(4, 344);
			this->label16->Margin = System::Windows::Forms::Padding(3);
			this->label16->Name = L"label16";
			this->label16->Size = System::Drawing::Size(397, 20);
			this->label16->TabIndex = 9;
			this->label16->Text = L"MEASSURE_ELECTRODES";
			this->label16->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// label15
			// 
			this->label15->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label15->AutoSize = true;
			this->label15->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label15->Location = System::Drawing::Point(4, 210);
			this->label15->Margin = System::Windows::Forms::Padding(3);
			this->label15->Name = L"label15";
			this->label15->Size = System::Drawing::Size(397, 20);
			this->label15->TabIndex = 8;
			this->label15->Text = L"CURRENT_ELECTRODES";
			this->label15->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// label10
			// 
			this->label10->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label10->AutoSize = true;
			this->label10->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label10->Location = System::Drawing::Point(4, 76);
			this->label10->Margin = System::Windows::Forms::Padding(3);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(397, 20);
			this->label10->TabIndex = 7;
			this->label10->Text = L"GND_ELECTRODES";
			this->label10->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// tableLayoutPanel6
			// 
			this->tableLayoutPanel6->ColumnCount = 2;
			this->tableLayoutPanel6->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				50)));
			this->tableLayoutPanel6->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				50)));
			this->tableLayoutPanel6->Controls->Add(this->label6, 0, 0);
			this->tableLayoutPanel6->Controls->Add(this->textBox_D_sonde, 1, 0);
			this->tableLayoutPanel6->Location = System::Drawing::Point(4, 35);
			this->tableLayoutPanel6->Name = L"tableLayoutPanel6";
			this->tableLayoutPanel6->RowCount = 1;
			this->tableLayoutPanel6->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 50)));
			this->tableLayoutPanel6->Size = System::Drawing::Size(273, 34);
			this->tableLayoutPanel6->TabIndex = 6;
			// 
			// label7
			// 
			this->label7->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label7->AutoSize = true;
			this->label7->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label7->Location = System::Drawing::Point(4, 4);
			this->label7->Margin = System::Windows::Forms::Padding(3);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(397, 20);
			this->label7->TabIndex = 1;
			this->label7->Text = L"TOOL PARAMETRS ";
			this->label7->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// label29
			// 
			this->label29->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label29->AutoSize = true;
			this->label29->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label29->Location = System::Drawing::Point(4, 4);
			this->label29->Margin = System::Windows::Forms::Padding(3);
			this->label29->Name = L"label29";
			this->label29->Size = System::Drawing::Size(134, 20);
			this->label29->TabIndex = 9;
			this->label29->Text = L"dR start mm";
			this->label29->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// label30
			// 
			this->label30->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label30->AutoSize = true;
			this->label30->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label30->Location = System::Drawing::Point(4, 34);
			this->label30->Margin = System::Windows::Forms::Padding(3);
			this->label30->Name = L"label30";
			this->label30->Size = System::Drawing::Size(134, 20);
			this->label30->TabIndex = 10;
			this->label30->Text = L"Q_r";
			this->label30->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// label_
			// 
			this->label_->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label_->AutoSize = true;
			this->label_->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label_->Location = System::Drawing::Point(4, 97);
			this->label_->Margin = System::Windows::Forms::Padding(3);
			this->label_->Name = L"label_";
			this->label_->Size = System::Drawing::Size(134, 20);
			this->label_->TabIndex = 11;
			this->label_->Text = L"dZ  tool mm";
			this->label_->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// label32
			// 
			this->label32->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label32->AutoSize = true;
			this->label32->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label32->Location = System::Drawing::Point(4, 159);
			this->label32->Margin = System::Windows::Forms::Padding(3);
			this->label32->Name = L"label32";
			this->label32->Size = System::Drawing::Size(134, 20);
			this->label32->TabIndex = 12;
			this->label32->Text = L"tail m";
			this->label32->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// label_q_z_tail
			// 
			this->label_q_z_tail->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label_q_z_tail->AutoSize = true;
			this->label_q_z_tail->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label_q_z_tail->Location = System::Drawing::Point(4, 190);
			this->label_q_z_tail->Margin = System::Windows::Forms::Padding(3);
			this->label_q_z_tail->Name = L"label_q_z_tail";
			this->label_q_z_tail->Size = System::Drawing::Size(134, 20);
			this->label_q_z_tail->TabIndex = 13;
			this->label_q_z_tail->Text = L"Q_z_tail m";
			this->label_q_z_tail->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// tableLayoutPanel7
			// 
			this->tableLayoutPanel7->CellBorderStyle = System::Windows::Forms::TableLayoutPanelCellBorderStyle::Single;
			this->tableLayoutPanel7->ColumnCount = 2;
			this->tableLayoutPanel7->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
				140)));
			this->tableLayoutPanel7->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
				245)));
			this->tableLayoutPanel7->Controls->Add(this->label41, 0, 7);
			this->tableLayoutPanel7->Controls->Add(this->textBox_dz_fine_mm, 1, 4);
			this->tableLayoutPanel7->Controls->Add(this->label40, 0, 4);
			this->tableLayoutPanel7->Controls->Add(this->textBox_r_max_m, 1, 2);
			this->tableLayoutPanel7->Controls->Add(this->label34, 0, 2);
			this->tableLayoutPanel7->Controls->Add(this->textBox_dr_start_mm, 1, 0);
			this->tableLayoutPanel7->Controls->Add(this->label30, 0, 1);
			this->tableLayoutPanel7->Controls->Add(this->textBox_q_r, 1, 1);
			this->tableLayoutPanel7->Controls->Add(this->label29, 0, 0);
			this->tableLayoutPanel7->Controls->Add(this->textBox_q_z_tail, 1, 6);
			this->tableLayoutPanel7->Controls->Add(this->label_, 0, 3);
			this->tableLayoutPanel7->Controls->Add(this->textBox_dz_base_mm, 1, 3);
			this->tableLayoutPanel7->Controls->Add(this->label_q_z_tail, 0, 6);
			this->tableLayoutPanel7->Controls->Add(this->label32, 0, 5);
			this->tableLayoutPanel7->Controls->Add(this->textBox_tail, 1, 5);
			this->tableLayoutPanel7->Controls->Add(this->textBox_threshold_mm, 1, 7);
			this->tableLayoutPanel7->Location = System::Drawing::Point(4, 35);
			this->tableLayoutPanel7->Name = L"tableLayoutPanel7";
			this->tableLayoutPanel7->RowCount = 9;
			this->tableLayoutPanel7->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 29)));
			this->tableLayoutPanel7->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 31)));
			this->tableLayoutPanel7->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 30)));
			this->tableLayoutPanel7->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 30)));
			this->tableLayoutPanel7->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 30)));
			this->tableLayoutPanel7->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 30)));
			this->tableLayoutPanel7->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 30)));
			this->tableLayoutPanel7->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 30)));
			this->tableLayoutPanel7->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 30)));
			this->tableLayoutPanel7->Size = System::Drawing::Size(258, 246);
			this->tableLayoutPanel7->TabIndex = 14;
			// 
			// label41
			// 
			this->label41->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label41->AutoSize = true;
			this->label41->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label41->Location = System::Drawing::Point(4, 221);
			this->label41->Margin = System::Windows::Forms::Padding(3);
			this->label41->Name = L"label41";
			this->label41->Size = System::Drawing::Size(134, 20);
			this->label41->TabIndex = 24;
			this->label41->Text = L"Threshold mm";
			this->label41->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// textBox_dz_fine_mm
			// 
			this->textBox_dz_fine_mm->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_dz_fine_mm->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->textBox_dz_fine_mm->Location = System::Drawing::Point(145, 128);
			this->textBox_dz_fine_mm->Name = L"textBox_dz_fine_mm";
			this->textBox_dz_fine_mm->Size = System::Drawing::Size(239, 27);
			this->textBox_dz_fine_mm->TabIndex = 23;
			this->textBox_dz_fine_mm->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit1);
			// 
			// label40
			// 
			this->label40->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label40->AutoSize = true;
			this->label40->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label40->Location = System::Drawing::Point(4, 128);
			this->label40->Margin = System::Windows::Forms::Padding(3);
			this->label40->Name = L"label40";
			this->label40->Size = System::Drawing::Size(134, 20);
			this->label40->TabIndex = 21;
			this->label40->Text = L"dZ  fine mm";
			this->label40->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// textBox_r_max_m
			// 
			this->textBox_r_max_m->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_r_max_m->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->textBox_r_max_m->Location = System::Drawing::Point(145, 66);
			this->textBox_r_max_m->Name = L"textBox_r_max_m";
			this->textBox_r_max_m->Size = System::Drawing::Size(239, 27);
			this->textBox_r_max_m->TabIndex = 18;
			this->textBox_r_max_m->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit2);
			// 
			// label34
			// 
			this->label34->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label34->AutoSize = true;
			this->label34->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label34->Location = System::Drawing::Point(4, 66);
			this->label34->Margin = System::Windows::Forms::Padding(3);
			this->label34->Name = L"label34";
			this->label34->Size = System::Drawing::Size(134, 20);
			this->label34->TabIndex = 17;
			this->label34->Text = L"r_max m";
			this->label34->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// textBox_dr_start_mm
			// 
			this->textBox_dr_start_mm->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_dr_start_mm->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(204)));
			this->textBox_dr_start_mm->Location = System::Drawing::Point(145, 4);
			this->textBox_dr_start_mm->Name = L"textBox_dr_start_mm";
			this->textBox_dr_start_mm->Size = System::Drawing::Size(239, 27);
			this->textBox_dr_start_mm->TabIndex = 1;
			this->textBox_dr_start_mm->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit1);
			// 
			// textBox_q_r
			// 
			this->textBox_q_r->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_q_r->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->textBox_q_r->Location = System::Drawing::Point(145, 34);
			this->textBox_q_r->Name = L"textBox_q_r";
			this->textBox_q_r->Size = System::Drawing::Size(239, 27);
			this->textBox_q_r->TabIndex = 6;
			this->textBox_q_r->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit2);
			// 
			// textBox_q_z_tail
			// 
			this->textBox_q_z_tail->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_q_z_tail->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->textBox_q_z_tail->Location = System::Drawing::Point(145, 190);
			this->textBox_q_z_tail->Name = L"textBox_q_z_tail";
			this->textBox_q_z_tail->Size = System::Drawing::Size(239, 27);
			this->textBox_q_z_tail->TabIndex = 2;
			this->textBox_q_z_tail->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit2);
			// 
			// textBox_dz_base_mm
			// 
			this->textBox_dz_base_mm->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_dz_base_mm->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->textBox_dz_base_mm->Location = System::Drawing::Point(145, 97);
			this->textBox_dz_base_mm->Name = L"textBox_dz_base_mm";
			this->textBox_dz_base_mm->Size = System::Drawing::Size(239, 27);
			this->textBox_dz_base_mm->TabIndex = 2;
			this->textBox_dz_base_mm->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit1);
			// 
			// textBox_tail
			// 
			this->textBox_tail->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_tail->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->textBox_tail->Location = System::Drawing::Point(145, 159);
			this->textBox_tail->Name = L"textBox_tail";
			this->textBox_tail->Size = System::Drawing::Size(239, 27);
			this->textBox_tail->TabIndex = 2;
			this->textBox_tail->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit2);
			// 
			// textBox_threshold_mm
			// 
			this->textBox_threshold_mm->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_threshold_mm->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(204)));
			this->textBox_threshold_mm->Location = System::Drawing::Point(145, 221);
			this->textBox_threshold_mm->Name = L"textBox_threshold_mm";
			this->textBox_threshold_mm->Size = System::Drawing::Size(239, 27);
			this->textBox_threshold_mm->TabIndex = 19;
			this->textBox_threshold_mm->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit1);
			// 
			// label31
			// 
			this->label31->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label31->AutoSize = true;
			this->label31->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label31->Location = System::Drawing::Point(12, 8);
			this->label31->Margin = System::Windows::Forms::Padding(3);
			this->label31->Name = L"label31";
			this->label31->Size = System::Drawing::Size(155, 20);
			this->label31->TabIndex = 15;
			this->label31->Text = L"PALLETE STEPS";
			this->label31->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// label33
			// 
			this->label33->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label33->AutoSize = true;
			this->label33->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label33->Location = System::Drawing::Point(4, 4);
			this->label33->Margin = System::Windows::Forms::Padding(3);
			this->label33->Name = L"label33";
			this->label33->Size = System::Drawing::Size(258, 20);
			this->label33->TabIndex = 16;
			this->label33->Text = L"GRID_PARAMETRS";
			this->label33->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// tableLayoutPanel8
			// 
			this->tableLayoutPanel8->CellBorderStyle = System::Windows::Forms::TableLayoutPanelCellBorderStyle::Single;
			this->tableLayoutPanel8->ColumnCount = 2;
			this->tableLayoutPanel8->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
				140)));
			this->tableLayoutPanel8->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
				242)));
			this->tableLayoutPanel8->Controls->Add(this->textBox_Ro_BH, 1, 0);
			this->tableLayoutPanel8->Controls->Add(this->label38, 0, 0);
			this->tableLayoutPanel8->Controls->Add(this->label35, 0, 4);
			this->tableLayoutPanel8->Controls->Add(this->textBox_Ro_stratum, 1, 4);
			this->tableLayoutPanel8->Controls->Add(this->textBox_Ro_PZ, 1, 3);
			this->tableLayoutPanel8->Controls->Add(this->label39, 0, 3);
			this->tableLayoutPanel8->Controls->Add(this->label37, 0, 2);
			this->tableLayoutPanel8->Controls->Add(this->textBox_R_PZ_R_BH, 1, 2);
			this->tableLayoutPanel8->Controls->Add(this->textBox_D_BH_mm, 1, 1);
			this->tableLayoutPanel8->Controls->Add(this->label36, 0, 1);
			this->tableLayoutPanel8->Location = System::Drawing::Point(4, 35);
			this->tableLayoutPanel8->Name = L"tableLayoutPanel8";
			this->tableLayoutPanel8->RowCount = 5;
			this->tableLayoutPanel8->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 30)));
			this->tableLayoutPanel8->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 30)));
			this->tableLayoutPanel8->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 30)));
			this->tableLayoutPanel8->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 30)));
			this->tableLayoutPanel8->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 20)));
			this->tableLayoutPanel8->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 20)));
			this->tableLayoutPanel8->Size = System::Drawing::Size(300, 160);
			this->tableLayoutPanel8->TabIndex = 17;
			// 
			// textBox_Ro_BH
			// 
			this->textBox_Ro_BH->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_Ro_BH->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->textBox_Ro_BH->Location = System::Drawing::Point(145, 4);
			this->textBox_Ro_BH->Name = L"textBox_Ro_BH";
			this->textBox_Ro_BH->Size = System::Drawing::Size(236, 27);
			this->textBox_Ro_BH->TabIndex = 24;
			this->textBox_Ro_BH->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit2);
			// 
			// label38
			// 
			this->label38->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label38->AutoSize = true;
			this->label38->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label38->Location = System::Drawing::Point(4, 4);
			this->label38->Margin = System::Windows::Forms::Padding(3);
			this->label38->Name = L"label38";
			this->label38->Size = System::Drawing::Size(134, 20);
			this->label38->TabIndex = 24;
			this->label38->Text = L"R0_BH";
			this->label38->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// label35
			// 
			this->label35->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label35->AutoSize = true;
			this->label35->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label35->Location = System::Drawing::Point(4, 128);
			this->label35->Margin = System::Windows::Forms::Padding(3);
			this->label35->Name = L"label35";
			this->label35->Size = System::Drawing::Size(134, 20);
			this->label35->TabIndex = 3;
			this->label35->Text = L"Ro_STRATUM";
			this->label35->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// textBox_Ro_stratum
			// 
			this->textBox_Ro_stratum->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_Ro_stratum->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->textBox_Ro_stratum->Location = System::Drawing::Point(145, 128);
			this->textBox_Ro_stratum->Name = L"textBox_Ro_stratum";
			this->textBox_Ro_stratum->Size = System::Drawing::Size(236, 27);
			this->textBox_Ro_stratum->TabIndex = 2;
			this->textBox_Ro_stratum->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit3);
			// 
			// textBox_Ro_PZ
			// 
			this->textBox_Ro_PZ->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_Ro_PZ->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->textBox_Ro_PZ->Location = System::Drawing::Point(145, 97);
			this->textBox_Ro_PZ->Name = L"textBox_Ro_PZ";
			this->textBox_Ro_PZ->Size = System::Drawing::Size(236, 27);
			this->textBox_Ro_PZ->TabIndex = 6;
			this->textBox_Ro_PZ->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit2);
			// 
			// label39
			// 
			this->label39->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label39->AutoSize = true;
			this->label39->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label39->Location = System::Drawing::Point(4, 97);
			this->label39->Margin = System::Windows::Forms::Padding(3);
			this->label39->Name = L"label39";
			this->label39->Size = System::Drawing::Size(134, 20);
			this->label39->TabIndex = 5;
			this->label39->Text = L"Ro_PZ";
			this->label39->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// label37
			// 
			this->label37->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label37->AutoSize = true;
			this->label37->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label37->Location = System::Drawing::Point(4, 66);
			this->label37->Margin = System::Windows::Forms::Padding(3);
			this->label37->Name = L"label37";
			this->label37->Size = System::Drawing::Size(134, 20);
			this->label37->TabIndex = 2;
			this->label37->Text = L"D_PZ / D_BH";
			this->label37->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// textBox_R_PZ_R_BH
			// 
			this->textBox_R_PZ_R_BH->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_R_PZ_R_BH->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->textBox_R_PZ_R_BH->Location = System::Drawing::Point(145, 66);
			this->textBox_R_PZ_R_BH->Name = L"textBox_R_PZ_R_BH";
			this->textBox_R_PZ_R_BH->Size = System::Drawing::Size(236, 27);
			this->textBox_R_PZ_R_BH->TabIndex = 2;
			this->textBox_R_PZ_R_BH->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit2);
			// 
			// textBox_D_BH_mm
			// 
			this->textBox_D_BH_mm->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_D_BH_mm->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->textBox_D_BH_mm->Location = System::Drawing::Point(145, 35);
			this->textBox_D_BH_mm->Name = L"textBox_D_BH_mm";
			this->textBox_D_BH_mm->Size = System::Drawing::Size(236, 27);
			this->textBox_D_BH_mm->TabIndex = 2;
			this->textBox_D_BH_mm->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit2);
			// 
			// label36
			// 
			this->label36->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label36->AutoSize = true;
			this->label36->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label36->Location = System::Drawing::Point(4, 35);
			this->label36->Margin = System::Windows::Forms::Padding(3);
			this->label36->Name = L"label36";
			this->label36->Size = System::Drawing::Size(134, 20);
			this->label36->TabIndex = 0;
			this->label36->Text = L"D_BH_mm";
			this->label36->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// tableLayoutPanel_grid_param
			// 
			this->tableLayoutPanel_grid_param->CellBorderStyle = System::Windows::Forms::TableLayoutPanelCellBorderStyle::Single;
			this->tableLayoutPanel_grid_param->ColumnCount = 1;
			this->tableLayoutPanel_grid_param->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				100)));
			this->tableLayoutPanel_grid_param->Controls->Add(this->label33, 0, 0);
			this->tableLayoutPanel_grid_param->Controls->Add(this->tableLayoutPanel7, 0, 1);
			this->tableLayoutPanel_grid_param->Location = System::Drawing::Point(1203, 633);
			this->tableLayoutPanel_grid_param->Name = L"tableLayoutPanel_grid_param";
			this->tableLayoutPanel_grid_param->RowCount = 2;
			this->tableLayoutPanel_grid_param->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute,
				30)));
			this->tableLayoutPanel_grid_param->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->tableLayoutPanel_grid_param->Size = System::Drawing::Size(266, 279);
			this->tableLayoutPanel_grid_param->TabIndex = 18;
			// 
			// button_set
			// 
			this->button_set->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->button_set->Location = System::Drawing::Point(412, 3);
			this->button_set->Name = L"button_set";
			this->button_set->Size = System::Drawing::Size(75, 27);
			this->button_set->TabIndex = 19;
			this->button_set->Text = L"SET";
			this->button_set->UseVisualStyleBackColor = true;
			this->button_set->Click += gcnew System::EventHandler(this, &MyForm::button_set_environment_Click);
			// 
			// button_calk_pallete
			// 
			this->button_calk_pallete->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(204)));
			this->button_calk_pallete->Location = System::Drawing::Point(518, 3);
			this->button_calk_pallete->Name = L"button_calk_pallete";
			this->button_calk_pallete->Size = System::Drawing::Size(75, 27);
			this->button_calk_pallete->TabIndex = 20;
			this->button_calk_pallete->Text = L"CALC";
			this->button_calk_pallete->UseVisualStyleBackColor = true;
			this->button_calk_pallete->Click += gcnew System::EventHandler(this, &MyForm::button_calk_pallete_Click);
			// 
			// comboBox_choice
			// 
			this->comboBox_choice->FormattingEnabled = true;
			this->comboBox_choice->Items->AddRange(gcnew cli::array< System::Object^  >(3) { L"GIVEN", L"LOGARITMIC start Q end", L"LINE start step end" });
			this->comboBox_choice->Location = System::Drawing::Point(176, 6);
			this->comboBox_choice->Name = L"comboBox_choice";
			this->comboBox_choice->Size = System::Drawing::Size(214, 24);
			this->comboBox_choice->TabIndex = 21;
			// 
			// checkBox_Ro_BH_1
			// 
			this->checkBox_Ro_BH_1->AutoSize = true;
			this->checkBox_Ro_BH_1->Location = System::Drawing::Point(1623, 1161);
			this->checkBox_Ro_BH_1->Name = L"checkBox_Ro_BH_1";
			this->checkBox_Ro_BH_1->Size = System::Drawing::Size(99, 21);
			this->checkBox_Ro_BH_1->TabIndex = 22;
			this->checkBox_Ro_BH_1->Text = L"Ro_BH = 1";
			this->checkBox_Ro_BH_1->UseVisualStyleBackColor = true;
			this->checkBox_Ro_BH_1->Visible = false;
			// 
			// tableLayoutPanel_envir_param
			// 
			this->tableLayoutPanel_envir_param->CellBorderStyle = System::Windows::Forms::TableLayoutPanelCellBorderStyle::Single;
			this->tableLayoutPanel_envir_param->ColumnCount = 1;
			this->tableLayoutPanel_envir_param->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle()));
			this->tableLayoutPanel_envir_param->Controls->Add(this->tableLayoutPanel8, 0, 1);
			this->tableLayoutPanel_envir_param->Controls->Add(this->label42, 0, 0);
			this->tableLayoutPanel_envir_param->Location = System::Drawing::Point(1210, 974);
			this->tableLayoutPanel_envir_param->Name = L"tableLayoutPanel_envir_param";
			this->tableLayoutPanel_envir_param->RowCount = 2;
			this->tableLayoutPanel_envir_param->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute,
				30)));
			this->tableLayoutPanel_envir_param->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->tableLayoutPanel_envir_param->Size = System::Drawing::Size(308, 199);
			this->tableLayoutPanel_envir_param->TabIndex = 23;
			// 
			// label42
			// 
			this->label42->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label42->AutoSize = true;
			this->label42->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label42->Location = System::Drawing::Point(4, 4);
			this->label42->Margin = System::Windows::Forms::Padding(3);
			this->label42->Name = L"label42";
			this->label42->Size = System::Drawing::Size(300, 20);
			this->label42->TabIndex = 17;
			this->label42->Text = L"ENVIRONMENT";
			this->label42->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// pictureBox_U
			// 
			this->pictureBox_U->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->pictureBox_U->Location = System::Drawing::Point(1606, 78);
			this->pictureBox_U->Name = L"pictureBox_U";
			this->pictureBox_U->Size = System::Drawing::Size(482, 787);
			this->pictureBox_U->TabIndex = 24;
			this->pictureBox_U->TabStop = false;
			this->pictureBox_U->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &MyForm::pictureBox_U_Paint);
			this->pictureBox_U->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::pictureBox_U_MouseDown);
			this->pictureBox_U->MouseEnter += gcnew System::EventHandler(this, &MyForm::pictureBox_U_MouseEnter);
			this->pictureBox_U->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::pictureBox_U_MouseMove);
			// 
			// panel_pallete_param
			// 
			this->panel_pallete_param->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->panel_pallete_param->Controls->Add(this->tableLayoutPanel1);
			this->panel_pallete_param->Controls->Add(this->label31);
			this->panel_pallete_param->Controls->Add(this->comboBox_choice);
			this->panel_pallete_param->Controls->Add(this->button_set);
			this->panel_pallete_param->Controls->Add(this->button_calk_pallete);
			this->panel_pallete_param->Location = System::Drawing::Point(13, 556);
			this->panel_pallete_param->Name = L"panel_pallete_param";
			this->panel_pallete_param->Size = System::Drawing::Size(679, 165);
			this->panel_pallete_param->TabIndex = 25;
			// 
			// menuStrip1
			// 
			this->menuStrip1->ImageScalingSize = System::Drawing::Size(20, 20);
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->mENUToolStripMenuItem });
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(2135, 28);
			this->menuStrip1->TabIndex = 26;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// mENUToolStripMenuItem
			// 
			this->mENUToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {
				this->Open_Pallete_file_ToolStripMenuItem,
					this->loadPresetToolStripMenuItem, this->savePresetToolStripMenuItem
			});
			this->mENUToolStripMenuItem->Name = L"mENUToolStripMenuItem";
			this->mENUToolStripMenuItem->Size = System::Drawing::Size(63, 24);
			this->mENUToolStripMenuItem->Text = L"MENU";
			// 
			// Open_Pallete_file_ToolStripMenuItem
			// 
			this->Open_Pallete_file_ToolStripMenuItem->Name = L"Open_Pallete_file_ToolStripMenuItem";
			this->Open_Pallete_file_ToolStripMenuItem->Size = System::Drawing::Size(193, 26);
			this->Open_Pallete_file_ToolStripMenuItem->Text = L"open pallete file";
			this->Open_Pallete_file_ToolStripMenuItem->Click += gcnew System::EventHandler(this, &MyForm::Open_Pallete_file_ToolStripMenuItem_Click);
			// 
			// loadPresetToolStripMenuItem
			// 
			this->loadPresetToolStripMenuItem->Name = L"loadPresetToolStripMenuItem";
			this->loadPresetToolStripMenuItem->Size = System::Drawing::Size(193, 26);
			this->loadPresetToolStripMenuItem->Text = L"load preset";
			this->loadPresetToolStripMenuItem->Click += gcnew System::EventHandler(this, &MyForm::button_load_preset_Click);
			// 
			// savePresetToolStripMenuItem
			// 
			this->savePresetToolStripMenuItem->Name = L"savePresetToolStripMenuItem";
			this->savePresetToolStripMenuItem->Size = System::Drawing::Size(193, 26);
			this->savePresetToolStripMenuItem->Text = L"save preset";
			this->savePresetToolStripMenuItem->Click += gcnew System::EventHandler(this, &MyForm::savePresetToolStripMenuItem_Click);
			// 
			// tableLayoutPanel_chart
			// 
			this->tableLayoutPanel_chart->CellBorderStyle = System::Windows::Forms::TableLayoutPanelCellBorderStyle::Single;
			this->tableLayoutPanel_chart->ColumnCount = 1;
			this->tableLayoutPanel_chart->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				100)));
			this->tableLayoutPanel_chart->Controls->Add(this->panel2, 0, 0);
			this->tableLayoutPanel_chart->Controls->Add(this->panel3, 0, 2);
			this->tableLayoutPanel_chart->Controls->Add(this->chart_, 0, 1);
			this->tableLayoutPanel_chart->Location = System::Drawing::Point(1895, 53);
			this->tableLayoutPanel_chart->Name = L"tableLayoutPanel_chart";
			this->tableLayoutPanel_chart->RowCount = 3;
			this->tableLayoutPanel_chart->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute,
				40)));
			this->tableLayoutPanel_chart->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->tableLayoutPanel_chart->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute,
				40)));
			this->tableLayoutPanel_chart->Size = System::Drawing::Size(991, 896);
			this->tableLayoutPanel_chart->TabIndex = 27;
			// 
			// panel2
			// 
			this->panel2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->panel2->Controls->Add(this->comboBox_Ro_PZ);
			this->panel2->Controls->Add(this->label47);
			this->panel2->Controls->Add(this->comboBox_D_PZ);
			this->panel2->Controls->Add(this->label46);
			this->panel2->Controls->Add(this->label44);
			this->panel2->Controls->Add(this->label43);
			this->panel2->Controls->Add(this->comboBox_D_BH);
			this->panel2->Controls->Add(this->button_do_chart);
			this->panel2->Controls->Add(this->comboBox_shart_curves_Y);
			this->panel2->Location = System::Drawing::Point(4, 4);
			this->panel2->Name = L"panel2";
			this->panel2->Size = System::Drawing::Size(983, 34);
			this->panel2->TabIndex = 13;
			// 
			// comboBox_Ro_PZ
			// 
			this->comboBox_Ro_PZ->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->comboBox_Ro_PZ->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->comboBox_Ro_PZ->FormattingEnabled = true;
			this->comboBox_Ro_PZ->Items->AddRange(gcnew cli::array< System::Object^  >(3) { L"Ia", L"Ib", L"dU" });
			this->comboBox_Ro_PZ->Location = System::Drawing::Point(636, 3);
			this->comboBox_Ro_PZ->Name = L"comboBox_Ro_PZ";
			this->comboBox_Ro_PZ->Size = System::Drawing::Size(176, 28);
			this->comboBox_Ro_PZ->TabIndex = 33;
			this->comboBox_Ro_PZ->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::comboBox_Ro_PZ_SelectedIndexChanged);
			// 
			// label47
			// 
			this->label47->AutoSize = true;
			this->label47->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label47->Location = System::Drawing::Point(557, 5);
			this->label47->Name = L"label47";
			this->label47->Size = System::Drawing::Size(64, 20);
			this->label47->TabIndex = 32;
			this->label47->Text = L"Ro_PZ";
			// 
			// comboBox_D_PZ
			// 
			this->comboBox_D_PZ->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->comboBox_D_PZ->FormattingEnabled = true;
			this->comboBox_D_PZ->Items->AddRange(gcnew cli::array< System::Object^  >(3) { L"Ia", L"Ib", L"dU" });
			this->comboBox_D_PZ->Location = System::Drawing::Point(446, 2);
			this->comboBox_D_PZ->Name = L"comboBox_D_PZ";
			this->comboBox_D_PZ->Size = System::Drawing::Size(100, 28);
			this->comboBox_D_PZ->TabIndex = 31;
			this->comboBox_D_PZ->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::comboBox_D_PZ_SelectedIndexChanged);
			// 
			// label46
			// 
			this->label46->AutoSize = true;
			this->label46->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label46->Location = System::Drawing::Point(371, 5);
			this->label46->Name = L"label46";
			this->label46->Size = System::Drawing::Size(55, 20);
			this->label46->TabIndex = 30;
			this->label46->Text = L"D_PZ";
			// 
			// label44
			// 
			this->label44->AutoSize = true;
			this->label44->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label44->Location = System::Drawing::Point(3, 4);
			this->label44->Name = L"label44";
			this->label44->Size = System::Drawing::Size(20, 20);
			this->label44->TabIndex = 29;
			this->label44->Text = L"Y";
			// 
			// label43
			// 
			this->label43->AutoSize = true;
			this->label43->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label43->Location = System::Drawing::Point(189, 5);
			this->label43->Name = L"label43";
			this->label43->Size = System::Drawing::Size(60, 20);
			this->label43->TabIndex = 19;
			this->label43->Text = L"D_BH";
			// 
			// comboBox_D_BH
			// 
			this->comboBox_D_BH->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->comboBox_D_BH->FormattingEnabled = true;
			this->comboBox_D_BH->Items->AddRange(gcnew cli::array< System::Object^  >(3) { L"Ia", L"Ib", L"dU" });
			this->comboBox_D_BH->Location = System::Drawing::Point(254, 0);
			this->comboBox_D_BH->Name = L"comboBox_D_BH";
			this->comboBox_D_BH->Size = System::Drawing::Size(100, 28);
			this->comboBox_D_BH->TabIndex = 18;
			this->comboBox_D_BH->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::comboBox_D_BH_SelectedIndexChanged);
			// 
			// button_do_chart
			// 
			this->button_do_chart->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->button_do_chart->Location = System::Drawing::Point(905, 3);
			this->button_do_chart->Name = L"button_do_chart";
			this->button_do_chart->Size = System::Drawing::Size(75, 30);
			this->button_do_chart->TabIndex = 17;
			this->button_do_chart->Text = L">>";
			this->button_do_chart->UseVisualStyleBackColor = true;
			this->button_do_chart->Click += gcnew System::EventHandler(this, &MyForm::button_do_chart_Click);
			// 
			// comboBox_shart_curves_Y
			// 
			this->comboBox_shart_curves_Y->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->comboBox_shart_curves_Y->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(204)));
			this->comboBox_shart_curves_Y->FormattingEnabled = true;
			this->comboBox_shart_curves_Y->Items->AddRange(gcnew cli::array< System::Object^  >(15) {
				L"Ia", L"Ib", L"dU", L"IaS", L"IaM",
					L"IaL", L"IaSML", L"IbS", L"IbM", L"IbL", L"IbSML", L"dUS", L"dUM", L"dUL", L"dUSML"
			});
			this->comboBox_shart_curves_Y->Location = System::Drawing::Point(29, 0);
			this->comboBox_shart_curves_Y->Name = L"comboBox_shart_curves_Y";
			this->comboBox_shart_curves_Y->Size = System::Drawing::Size(152, 28);
			this->comboBox_shart_curves_Y->TabIndex = 16;
			this->comboBox_shart_curves_Y->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::comboBox_shart_curves_Y_SelectedIndexChanged);
			// 
			// panel3
			// 
			this->panel3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->panel3->Controls->Add(this->label_Y);
			this->panel3->Controls->Add(this->label_X);
			this->panel3->Controls->Add(this->label45);
			this->panel3->Controls->Add(this->comboBox_shart_curves_X);
			this->panel3->Location = System::Drawing::Point(4, 850);
			this->panel3->Name = L"panel3";
			this->panel3->Size = System::Drawing::Size(983, 39);
			this->panel3->TabIndex = 14;
			// 
			// label_Y
			// 
			this->label_Y->AutoSize = true;
			this->label_Y->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label_Y->Location = System::Drawing::Point(312, 9);
			this->label_Y->Name = L"label_Y";
			this->label_Y->Size = System::Drawing::Size(20, 20);
			this->label_Y->TabIndex = 32;
			this->label_Y->Text = L"Y";
			// 
			// label_X
			// 
			this->label_X->AutoSize = true;
			this->label_X->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label_X->Location = System::Drawing::Point(18, 9);
			this->label_X->Name = L"label_X";
			this->label_X->Size = System::Drawing::Size(21, 20);
			this->label_X->TabIndex = 31;
			this->label_X->Text = L"X";
			// 
			// label45
			// 
			this->label45->AutoSize = true;
			this->label45->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label45->Location = System::Drawing::Point(948, 6);
			this->label45->Name = L"label45";
			this->label45->Size = System::Drawing::Size(21, 20);
			this->label45->TabIndex = 30;
			this->label45->Text = L"X";
			// 
			// comboBox_shart_curves_X
			// 
			this->comboBox_shart_curves_X->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->comboBox_shart_curves_X->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(204)));
			this->comboBox_shart_curves_X->FormattingEnabled = true;
			this->comboBox_shart_curves_X->Items->AddRange(gcnew cli::array< System::Object^  >(2) { L"Ro_stratum / Ro_BH", L"IA_M / IA_SML" });
			this->comboBox_shart_curves_X->Location = System::Drawing::Point(659, 3);
			this->comboBox_shart_curves_X->Name = L"comboBox_shart_curves_X";
			this->comboBox_shart_curves_X->Size = System::Drawing::Size(284, 28);
			this->comboBox_shart_curves_X->TabIndex = 16;
			// 
			// chart_
			// 
			this->chart_->BackImageWrapMode = System::Windows::Forms::DataVisualization::Charting::ChartImageWrapMode::Unscaled;
			this->chart_->BorderlineColor = System::Drawing::Color::Black;
			this->chart_->BorderlineDashStyle = System::Windows::Forms::DataVisualization::Charting::ChartDashStyle::Solid;
			chartArea1->CursorX->IsUserEnabled = true;
			chartArea1->CursorX->IsUserSelectionEnabled = true;
			chartArea1->CursorY->Interval = 0.0001;
			chartArea1->CursorY->IsUserEnabled = true;
			chartArea1->CursorY->IsUserSelectionEnabled = true;
			chartArea1->Name = L"ChartArea1";
			this->chart_->ChartAreas->Add(chartArea1);
			legend1->Name = L"Legend1";
			legend1->Position->Auto = false;
			legend1->Position->Height = 5;
			legend1->Position->Width = 55;
			legend1->Position->X = 10;
			legend1->Position->Y = 1;
			this->chart_->Legends->Add(legend1);
			this->chart_->Location = System::Drawing::Point(3, 44);
			this->chart_->Margin = System::Windows::Forms::Padding(2);
			this->chart_->Name = L"chart_";
			series1->BorderWidth = 2;
			series1->ChartArea = L"ChartArea1";
			series1->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Line;
			series1->Color = System::Drawing::Color::Green;
			series1->Legend = L"Legend1";
			series1->MarkerBorderWidth = 3;
			series1->Name = L"SML";
			series2->BorderWidth = 2;
			series2->ChartArea = L"ChartArea1";
			series2->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Line;
			series2->Color = System::Drawing::Color::DarkGoldenrod;
			series2->Legend = L"Legend1";
			series2->MarkerBorderWidth = 3;
			series2->MarkerSize = 1;
			series2->Name = L"S";
			series3->ChartArea = L"ChartArea1";
			series3->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Line;
			series3->Color = System::Drawing::Color::Red;
			series3->Legend = L"Legend1";
			series3->MarkerBorderWidth = 3;
			series3->MarkerSize = 1;
			series3->Name = L"M";
			series4->ChartArea = L"ChartArea1";
			series4->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Line;
			series4->Color = System::Drawing::Color::RoyalBlue;
			series4->Legend = L"Legend1";
			series4->MarkerBorderWidth = 3;
			series4->Name = L"L";
			series5->ChartArea = L"ChartArea1";
			series5->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Line;
			series5->Legend = L"Legend1";
			series5->MarkerBorderWidth = 3;
			series5->MarkerSize = 1;
			series5->Name = L"1";
			series6->ChartArea = L"ChartArea1";
			series6->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Line;
			series6->Legend = L"Legend1";
			series6->MarkerBorderWidth = 3;
			series6->Name = L"2";
			series7->ChartArea = L"ChartArea1";
			series7->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Line;
			series7->Legend = L"Legend1";
			series7->MarkerBorderWidth = 3;
			series7->Name = L"3";
			series8->ChartArea = L"ChartArea1";
			series8->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Line;
			series8->Legend = L"Legend1";
			series8->MarkerBorderWidth = 3;
			series8->Name = L"4";
			series9->ChartArea = L"ChartArea1";
			series9->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Line;
			series9->Legend = L"Legend1";
			series9->MarkerBorderWidth = 3;
			series9->Name = L"5";
			series10->ChartArea = L"ChartArea1";
			series10->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Line;
			series10->Legend = L"Legend1";
			series10->MarkerBorderWidth = 3;
			series10->Name = L"6";
			series11->ChartArea = L"ChartArea1";
			series11->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Line;
			series11->Legend = L"Legend1";
			series11->MarkerBorderWidth = 3;
			series11->Name = L"7";
			series12->ChartArea = L"ChartArea1";
			series12->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Line;
			series12->Legend = L"Legend1";
			series12->MarkerBorderWidth = 3;
			series12->Name = L"8";
			series13->ChartArea = L"ChartArea1";
			series13->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Line;
			series13->Legend = L"Legend1";
			series13->MarkerBorderWidth = 3;
			series13->Name = L"9";
			series14->ChartArea = L"ChartArea1";
			series14->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Line;
			series14->Legend = L"Legend1";
			series14->MarkerBorderWidth = 3;
			series14->Name = L"10";
			series15->ChartArea = L"ChartArea1";
			series15->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Line;
			series15->Legend = L"Legend1";
			series15->MarkerBorderWidth = 3;
			series15->Name = L"11";
			series16->ChartArea = L"ChartArea1";
			series16->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Line;
			series16->Legend = L"Legend1";
			series16->MarkerBorderWidth = 3;
			series16->Name = L"12";
			series17->ChartArea = L"ChartArea1";
			series17->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Line;
			series17->Legend = L"Legend1";
			series17->MarkerBorderWidth = 3;
			series17->Name = L"13";
			this->chart_->Series->Add(series1);
			this->chart_->Series->Add(series2);
			this->chart_->Series->Add(series3);
			this->chart_->Series->Add(series4);
			this->chart_->Series->Add(series5);
			this->chart_->Series->Add(series6);
			this->chart_->Series->Add(series7);
			this->chart_->Series->Add(series8);
			this->chart_->Series->Add(series9);
			this->chart_->Series->Add(series10);
			this->chart_->Series->Add(series11);
			this->chart_->Series->Add(series12);
			this->chart_->Series->Add(series13);
			this->chart_->Series->Add(series14);
			this->chart_->Series->Add(series15);
			this->chart_->Series->Add(series16);
			this->chart_->Series->Add(series17);
			this->chart_->Size = System::Drawing::Size(985, 800);
			this->chart_->TabIndex = 12;
			this->chart_->Text = L"CCCC";
			this->chart_->TextAntiAliasingQuality = System::Windows::Forms::DataVisualization::Charting::TextAntiAliasingQuality::Normal;
			this->chart_->CursorPositionChanged += gcnew System::EventHandler<System::Windows::Forms::DataVisualization::Charting::CursorEventArgs^ >(this, &MyForm::cursor_position);
			// 
			// button_calc_show
			// 
			this->button_calc_show->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->button_calc_show->Location = System::Drawing::Point(423, 1);
			this->button_calc_show->Name = L"button_calc_show";
			this->button_calc_show->Size = System::Drawing::Size(341, 33);
			this->button_calc_show->TabIndex = 28;
			this->button_calc_show->Text = L"SWITSH TO SHOW_MODE";
			this->button_calc_show->UseVisualStyleBackColor = true;
			this->button_calc_show->Click += gcnew System::EventHandler(this, &MyForm::button_calc_show_Click);
			// 
			// openFileDialog1
			// 
			this->openFileDialog1->FileName = L"openFileDialog1";
			// 
			// button_inversion
			// 
			this->button_inversion->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->button_inversion->Location = System::Drawing::Point(822, 1);
			this->button_inversion->Name = L"button_inversion";
			this->button_inversion->Size = System::Drawing::Size(129, 33);
			this->button_inversion->TabIndex = 30;
			this->button_inversion->Text = L"INVERSION";
			this->button_inversion->UseVisualStyleBackColor = true;
			this->button_inversion->Click += gcnew System::EventHandler(this, &MyForm::button_inversion_Click);
			// 
			// checkBox_leg
			// 
			this->checkBox_leg->AutoSize = true;
			this->checkBox_leg->Location = System::Drawing::Point(1154, 1);
			this->checkBox_leg->Name = L"checkBox_leg";
			this->checkBox_leg->Size = System::Drawing::Size(77, 21);
			this->checkBox_leg->TabIndex = 31;
			this->checkBox_leg->Text = L"Legotin";
			this->checkBox_leg->UseVisualStyleBackColor = true;
			// 
			// dataGridView_electrodes
			// 
			this->dataGridView_electrodes->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dataGridView_electrodes->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(3) {
				this->name,
					this->z_start, this->z_end
			});
			this->dataGridView_electrodes->Location = System::Drawing::Point(5, 35);
			this->dataGridView_electrodes->Name = L"dataGridView_electrodes";
			this->dataGridView_electrodes->RowTemplate->Height = 24;
			this->dataGridView_electrodes->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->dataGridView_electrodes->Size = System::Drawing::Size(344, 165);
			this->dataGridView_electrodes->TabIndex = 32;
			this->dataGridView_electrodes->ColumnHeaderMouseDoubleClick += gcnew System::Windows::Forms::DataGridViewCellMouseEventHandler(this, &MyForm::dataGridView_electrodes_ColumnHeaderMouseDoubleClick);
			this->dataGridView_electrodes->EditingControlShowing += gcnew System::Windows::Forms::DataGridViewEditingControlShowingEventHandler(this, &MyForm::dataGridView_electrodes_EditingControlShowing);
			this->dataGridView_electrodes->RowsAdded += gcnew System::Windows::Forms::DataGridViewRowsAddedEventHandler(this, &MyForm::dataGridView_electrodes_RowsAdded);
			this->dataGridView_electrodes->UserDeletingRow += gcnew System::Windows::Forms::DataGridViewRowCancelEventHandler(this, &MyForm::dataGridView_electrodes_UserDeletingRow);
			// 
			// name
			// 
			this->name->HeaderText = L"NAME";
			this->name->Name = L"name";
			// 
			// z_start
			// 
			this->z_start->HeaderText = L"z_beg_mm";
			this->z_start->Name = L"z_start";
			// 
			// z_end
			// 
			this->z_end->HeaderText = L"z_end_mm";
			this->z_end->Name = L"z_end";
			// 
			// panel1
			// 
			this->panel1->Controls->Add(this->comboBox_work_mode);
			this->panel1->Controls->Add(this->button_set_electrodes);
			this->panel1->Controls->Add(this->button1_add_work_mode);
			this->panel1->Controls->Add(this->dataGridView_electrodes);
			this->panel1->Location = System::Drawing::Point(8, 190);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(1181, 361);
			this->panel1->TabIndex = 33;
			// 
			// comboBox_work_mode
			// 
			this->comboBox_work_mode->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->comboBox_work_mode->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->comboBox_work_mode->FormattingEnabled = true;
			this->comboBox_work_mode->Items->AddRange(gcnew cli::array< System::Object^  >(4) { L"SML", L"S", L"M", L"L" });
			this->comboBox_work_mode->Location = System::Drawing::Point(323, 1);
			this->comboBox_work_mode->Name = L"comboBox_work_mode";
			this->comboBox_work_mode->Size = System::Drawing::Size(132, 28);
			this->comboBox_work_mode->TabIndex = 38;
			// 
			// button_set_electrodes
			// 
			this->button_set_electrodes->Location = System::Drawing::Point(161, 3);
			this->button_set_electrodes->Name = L"button_set_electrodes";
			this->button_set_electrodes->Size = System::Drawing::Size(156, 23);
			this->button_set_electrodes->TabIndex = 35;
			this->button_set_electrodes->Text = L"SET_ELECTRODES";
			this->button_set_electrodes->UseVisualStyleBackColor = true;
			this->button_set_electrodes->Click += gcnew System::EventHandler(this, &MyForm::button_set_electrodes_Click);
			// 
			// button1_add_work_mode
			// 
			this->button1_add_work_mode->Location = System::Drawing::Point(3, 3);
			this->button1_add_work_mode->Name = L"button1_add_work_mode";
			this->button1_add_work_mode->Size = System::Drawing::Size(152, 23);
			this->button1_add_work_mode->TabIndex = 34;
			this->button1_add_work_mode->Text = L"ADD_WORK_MODE";
			this->button1_add_work_mode->UseVisualStyleBackColor = true;
			this->button1_add_work_mode->Click += gcnew System::EventHandler(this, &MyForm::button_add_work_mode_Click);
			// 
			// label48
			// 
			this->label48->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label48->AutoSize = true;
			this->label48->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 7.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label48->Location = System::Drawing::Point(123, 5);
			this->label48->Margin = System::Windows::Forms::Padding(3);
			this->label48->Name = L"label48";
			this->label48->Size = System::Drawing::Size(135, 17);
			this->label48->TabIndex = 40;
			this->label48->Text = L"GRID_PARAMETRS";
			this->label48->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// label4
			// 
			this->label4->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label4->AutoSize = true;
			this->label4->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label4->Location = System::Drawing::Point(1728, 1162);
			this->label4->Margin = System::Windows::Forms::Padding(3);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(69, 20);
			this->label4->TabIndex = 4;
			this->label4->Text = L"Ro_BH";
			this->label4->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			this->label4->Visible = false;
			// 
			// textBox_Ro_BH_step
			// 
			this->textBox_Ro_BH_step->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left));
			this->textBox_Ro_BH_step->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->textBox_Ro_BH_step->Location = System::Drawing::Point(1812, 1159);
			this->textBox_Ro_BH_step->Multiline = true;
			this->textBox_Ro_BH_step->Name = L"textBox_Ro_BH_step";
			this->textBox_Ro_BH_step->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->textBox_Ro_BH_step->Size = System::Drawing::Size(110, 24);
			this->textBox_Ro_BH_step->TabIndex = 1;
			this->textBox_Ro_BH_step->Visible = false;
			this->textBox_Ro_BH_step->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::digit3);
			// 
			// dataGridView_grid_param
			// 
			this->dataGridView_grid_param->AllowUserToDeleteRows = false;
			this->dataGridView_grid_param->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dataGridView_grid_param->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(9) {
				this->D_sonde,
					this->dr_start_mm, this->q_r, this->r_max_m, this->dz_base_mm, this->dz_fine_mm, this->tail, this->q_z_tail, this->threshold_mm
			});
			this->dataGridView_grid_param->Location = System::Drawing::Point(3, 40);
			this->dataGridView_grid_param->Name = L"dataGridView_grid_param";
			this->dataGridView_grid_param->RowTemplate->Height = 24;
			this->dataGridView_grid_param->ScrollBars = System::Windows::Forms::ScrollBars::None;
			this->dataGridView_grid_param->Size = System::Drawing::Size(947, 52);
			this->dataGridView_grid_param->TabIndex = 39;
			this->dataGridView_grid_param->EditingControlShowing += gcnew System::Windows::Forms::DataGridViewEditingControlShowingEventHandler(this, &MyForm::dataGridView_grid_param_EditingControlShowing);
			// 
			// D_sonde
			// 
			this->D_sonde->HeaderText = L"D_sonde";
			this->D_sonde->Name = L"D_sonde";
			// 
			// dr_start_mm
			// 
			this->dr_start_mm->HeaderText = L"dr_start_mm";
			this->dr_start_mm->Name = L"dr_start_mm";
			// 
			// q_r
			// 
			this->q_r->HeaderText = L"q_r";
			this->q_r->Name = L"q_r";
			// 
			// r_max_m
			// 
			this->r_max_m->HeaderText = L"r_max_m";
			this->r_max_m->Name = L"r_max_m";
			// 
			// dz_base_mm
			// 
			this->dz_base_mm->HeaderText = L"dz_base_mm";
			this->dz_base_mm->Name = L"dz_base_mm";
			// 
			// dz_fine_mm
			// 
			this->dz_fine_mm->HeaderText = L"dz_fine_mm";
			this->dz_fine_mm->Name = L"dz_fine_mm";
			// 
			// tail
			// 
			this->tail->HeaderText = L"tail";
			this->tail->Name = L"tail";
			// 
			// q_z_tail
			// 
			this->q_z_tail->HeaderText = L"q_z_tail";
			this->q_z_tail->Name = L"q_z_tail";
			// 
			// threshold_mm
			// 
			this->threshold_mm->HeaderText = L"threshold_mm";
			this->threshold_mm->Name = L"threshold_mm";
			// 
			// dataGridView_envir
			// 
			this->dataGridView_envir->AllowUserToDeleteRows = false;
			this->dataGridView_envir->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dataGridView_envir->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(5) {
				this->Ro_BH,
					this->D_BH_mm, this->D_PZ_D_BH, this->Ro_PZ, this->Ro_stratum
			});
			this->dataGridView_envir->Location = System::Drawing::Point(13, 121);
			this->dataGridView_envir->Name = L"dataGridView_envir";
			this->dataGridView_envir->RowTemplate->Height = 24;
			this->dataGridView_envir->ScrollBars = System::Windows::Forms::ScrollBars::None;
			this->dataGridView_envir->Size = System::Drawing::Size(555, 52);
			this->dataGridView_envir->TabIndex = 41;
			// 
			// Ro_BH
			// 
			this->Ro_BH->HeaderText = L"Ro_BH";
			this->Ro_BH->Name = L"Ro_BH";
			// 
			// D_BH_mm
			// 
			this->D_BH_mm->HeaderText = L"D_BH_mm";
			this->D_BH_mm->Name = L"D_BH_mm";
			// 
			// D_PZ_D_BH
			// 
			this->D_PZ_D_BH->HeaderText = L"D_PZ/D_BH";
			this->D_PZ_D_BH->Name = L"D_PZ_D_BH";
			// 
			// Ro_PZ
			// 
			this->Ro_PZ->HeaderText = L"Ro_PZ";
			this->Ro_PZ->Name = L"Ro_PZ";
			// 
			// Ro_stratum
			// 
			this->Ro_stratum->HeaderText = L"Ro_stratum";
			this->Ro_stratum->Name = L"Ro_stratum";
			// 
			// label49
			// 
			this->label49->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label49->AutoSize = true;
			this->label49->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 7.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label49->Location = System::Drawing::Point(123, 98);
			this->label49->Margin = System::Windows::Forms::Padding(3);
			this->label49->Name = L"label49";
			this->label49->Size = System::Drawing::Size(109, 17);
			this->label49->TabIndex = 42;
			this->label49->Text = L"ENVIRONMENT";
			this->label49->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// button_calk_one_
			// 
			this->button_calk_one_->Location = System::Drawing::Point(314, 94);
			this->button_calk_one_->Name = L"button_calk_one_";
			this->button_calk_one_->Size = System::Drawing::Size(101, 23);
			this->button_calk_one_->TabIndex = 43;
			this->button_calk_one_->Text = L"CALC_ONE";
			this->button_calk_one_->UseVisualStyleBackColor = true;
			this->button_calk_one_->Click += gcnew System::EventHandler(this, &MyForm::button_calk_one__Click);
			// 
			// button_calk_n
			// 
			this->button_calk_n->Location = System::Drawing::Point(426, 94);
			this->button_calk_n->Name = L"button_calk_n";
			this->button_calk_n->Size = System::Drawing::Size(101, 23);
			this->button_calk_n->TabIndex = 44;
			this->button_calk_n->Text = L"CALC_N";
			this->button_calk_n->UseVisualStyleBackColor = true;
			this->button_calk_n->Click += gcnew System::EventHandler(this, &MyForm::button_calc_N_Click);
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(2135, 1191);
			this->Controls->Add(this->button_calk_n);
			this->Controls->Add(this->button_calk_one_);
			this->Controls->Add(this->label49);
			this->Controls->Add(this->dataGridView_envir);
			this->Controls->Add(this->label48);
			this->Controls->Add(this->tableLayoutPanel_tool_param);
			this->Controls->Add(this->dataGridView_grid_param);
			this->Controls->Add(this->panel1);
			this->Controls->Add(this->pictureBox_U);
			this->Controls->Add(this->tableLayoutPanel_envir_param);
			this->Controls->Add(this->checkBox_leg);
			this->Controls->Add(this->button_inversion);
			this->Controls->Add(this->tableLayoutPanel_grid_param);
			this->Controls->Add(this->button_calc_show);
			this->Controls->Add(this->panel_pallete_param);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->textBox_Ro_BH_step);
			this->Controls->Add(this->checkBox_Ro_BH_1);
			this->Controls->Add(this->menuStrip1);
			this->Controls->Add(this->tableLayoutPanel_chart);
			this->MainMenuStrip = this->menuStrip1;
			this->Name = L"MyForm";
			this->Text = L"MyForm";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &MyForm::MyForm_FormClosing);
			this->Load += gcnew System::EventHandler(this, &MyForm::MyForm_Load);
			this->tableLayoutPanel1->ResumeLayout(false);
			this->tableLayoutPanel1->PerformLayout();
			this->tableLayoutPanel3->ResumeLayout(false);
			this->tableLayoutPanel3->PerformLayout();
			this->tableLayoutPanel4->ResumeLayout(false);
			this->tableLayoutPanel4->PerformLayout();
			this->tableLayoutPanel5->ResumeLayout(false);
			this->tableLayoutPanel5->PerformLayout();
			this->tableLayoutPanel_tool_param->ResumeLayout(false);
			this->tableLayoutPanel_tool_param->PerformLayout();
			this->tableLayoutPanel6->ResumeLayout(false);
			this->tableLayoutPanel6->PerformLayout();
			this->tableLayoutPanel7->ResumeLayout(false);
			this->tableLayoutPanel7->PerformLayout();
			this->tableLayoutPanel8->ResumeLayout(false);
			this->tableLayoutPanel8->PerformLayout();
			this->tableLayoutPanel_grid_param->ResumeLayout(false);
			this->tableLayoutPanel_grid_param->PerformLayout();
			this->tableLayoutPanel_envir_param->ResumeLayout(false);
			this->tableLayoutPanel_envir_param->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox_U))->EndInit();
			this->panel_pallete_param->ResumeLayout(false);
			this->panel_pallete_param->PerformLayout();
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			this->tableLayoutPanel_chart->ResumeLayout(false);
			this->panel2->ResumeLayout(false);
			this->panel2->PerformLayout();
			this->panel3->ResumeLayout(false);
			this->panel3->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->chart_))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView_electrodes))->EndInit();
			this->panel1->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView_grid_param))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView_envir))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		
		int visual_mode = CALK_MODE;
		bool debug_mode = false;
		uint32_t pallete_size = 0;


private: System::Void Cells_KeyPress(System::Object^ sender, System::Windows::Forms::KeyPressEventArgs^ e) {
	TextBox^ tb = dynamic_cast<TextBox^>(sender);
	if (tb == nullptr) return;

	// Разрешаем цифры и Backspace
	if (Char::IsDigit(e->KeyChar) || e->KeyChar == (char)Keys::Back) return;

	// Разрешаем только один разделитель (точку или запятую)
	if (e->KeyChar == '.' || e->KeyChar == ',') {
		// Если разделитель уже есть, блокируем второй
		if (tb->Text->Contains(".") || tb->Text->Contains(",")) {
			e->Handled = true;
		}
		return;
	}

	// Все остальное блокируем
	e->Handled = true;
}

//
double GetDouble(System::Windows::Forms::TextBox^ tb, double defaultVal) {
		try {
			System::String^ text = tb->Text->Replace(".", ",");
			if (System::String::IsNullOrWhiteSpace(text)) return defaultVal;
			return System::Convert::ToDouble(text);
		}
		catch (...) {
			return defaultVal; 
		}
	}
//double 
private: System::Void digit3(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) {
	if (!Char::IsDigit(e->KeyChar) && !Char::IsControl(e->KeyChar)) e->Handled = true;
	if ((e->KeyChar) == '-') e->Handled = false; if ((e->KeyChar) == ' ') e->Handled = false;
	if ((e->KeyChar) == ',') e->Handled = false; if ((e->KeyChar) == '.') e->Handled = false;
}
//double
private: System::Void digit2(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) {
	if (!Char::IsDigit(e->KeyChar) && !Char::IsControl(e->KeyChar)) e->Handled = true;
	if ((e->KeyChar) == '-') e->Handled = false; if ((e->KeyChar) == ',') e->Handled = false; 
	if ((e->KeyChar) == '.') e->Handled = false;
}
//
private: System::Void digit1(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) {
	if (!Char::IsDigit(e->KeyChar) && !Char::IsControl(e->KeyChar)) e->Handled = true;
	 
}

private: System::Void DoubleKeyPress(System::Object^ sender, System::Windows::Forms::KeyPressEventArgs^ e) {
	// Если символ - не цифра, не Backspace и не запятая
	if (!Char::IsDigit(e->KeyChar) && e->KeyChar != 8 && e->KeyChar != ',' && e->KeyChar != '.') {
		e->Handled = true; // Блокируем
	}
}

void ResizeGrid(DataGridView^ dgv) {
	if (dgv->Columns->Count == 0) return;
	// 1. Считаем суммарную ширину всех колонок
	int totalColsWidth = 0;
	for (int i = 0; i < dgv->Columns->Count; i++) {
	    if (dgv->Columns[i]->Visible) {
		     totalColsWidth += dgv->Columns[i]->Width;
	    }
	}
	// 2. УчитываемRowHeaders (серая колонка слева), если она есть
	int rowHeaderW = dgv->RowHeadersVisible ? dgv->RowHeadersWidth : 0;
	// 3. Учитываем вертикальный скролл (только если строк > 10)
	int scrollWidth = 0;
	if (dgv->Rows->Count > 10) {
		scrollWidth = SystemInformation::VerticalScrollBarWidth;
			dgv->ScrollBars = ScrollBars::Vertical; // Только вертикальный
	}
	else {
		dgv->ScrollBars = ScrollBars::None;
	}
	// 4. Устанавливаем НОВУЮ ШИРИНУ всей таблицы
	// Таблица будет расширяться вправо при добавлении новых колонок
	dgv->Width = totalColsWidth + rowHeaderW + scrollWidth + 4;

	// 5. Подгоняем высоту под 10 строк
	int maxRows = 10;
	int headerH = dgv->ColumnHeadersHeight;
    int rowH = dgv->RowTemplate->Height;

	if (dgv->Rows->Count > maxRows) {
		dgv->Height = headerH + (rowH * maxRows) + 2;
	}
	else {
		dgv->Height = headerH + (rowH * dgv->Rows->Count) + 2;
		}
}

void do_chart() {
	int count = chart_->Series->Count;
	for (int i = 0; i < count; i++)
		chart_->Series[i]->Points->Clear();

	chart_->ChartAreas["ChartArea1"]->AxisX->IsLogarithmic = true;
    chart_->ChartAreas["ChartArea1"]->AxisY->IsLogarithmic = true;
	//chart_->Series[0]->Color = System::Drawing::Color::Red;
	//chart_->Series[1]->Color = System::Drawing::Color::RoyalBlue;
	//chart_->ChartAreas["ChartArea1"]->AxisX->Title = "IA0";
	//chart_->ChartAreas["ChartArea1"]->AxisY->Title = "IA2/IA0";
	chart_->ChartAreas["ChartArea1"]->CursorX->Interval = 0.00000000000001;
	chart_->ChartAreas["ChartArea1"]->CursorY->Interval = 0.00000000000001;
	chart_->ChartAreas["ChartArea1"]->AxisX->LabelStyle->Format = "E1";
	chart_->ChartAreas["ChartArea1"]->AxisY->LabelStyle->Format = "E1";

	comboBox_D_BH->Visible = true;

	//R_BH, R_PZ, Ro_PZ, RO_stratum
	
	if (comboBox_D_BH->SelectedIndex < 0)comboBox_D_BH->SelectedIndex = 0;
	if (comboBox_D_PZ->SelectedIndex < 0)comboBox_D_PZ->SelectedIndex = 0;
	if (comboBox_Ro_PZ->SelectedIndex < 0)comboBox_Ro_PZ->SelectedIndex = 0;
	int D_BH_idx = comboBox_D_BH->SelectedIndex;
	int D_PZ_idx = comboBox_D_PZ->SelectedIndex;
	int Ro_PZ_idx = comboBox_Ro_PZ->SelectedIndex;

	//R_BH, R_PZ, Ro_PZ, RO_stratum
	if (comboBox_shart_curves_X->SelectedItem == "Ro_stratum / Ro_BH" && (
		comboBox_shart_curves_Y->SelectedItem == "Ia"||
		comboBox_shart_curves_Y->SelectedItem == "Ib" ||
		comboBox_shart_curves_Y->SelectedItem == "dU"
		)) {
		chart_->Series[0]->ChartType = SeriesChartType::Line;
		chart_->Series[0]->LegendText = "SML"; chart_->Series[1]->LegendText = "S"; chart_->Series[2]->LegendText = "M"; chart_->Series[3]->LegendText = "L";
		for (int i = 4; i < count; i++)
		    chart_->Series[i]->IsVisibleInLegend = false;
		if (checkBox_leg->Checked == true) {
			chart_->Series[4]->BorderDashStyle = ChartDashStyle::Dash; chart_->Series[4]->Color = chart_->Series[0]->Color;
			chart_->Series[5]->BorderDashStyle = ChartDashStyle::Dash; chart_->Series[5]->Color = chart_->Series[1]->Color;
			chart_->Series[6]->BorderDashStyle = ChartDashStyle::Dash; chart_->Series[6]->Color = chart_->Series[2]->Color;
			chart_->Series[7]->BorderDashStyle = ChartDashStyle::Dash; chart_->Series[7]->Color = chart_->Series[3]->Color;			
		}
		Console::WriteLine( " Y " + comboBox_shart_curves_Y->SelectedItem  + " Ia :D_BH_idx " + D_BH_idx + " D_PZ_idx " + D_PZ_idx + " Ro_PZ_idx " + Ro_PZ_idx);
		for (int n_Ro_str = 0; n_Ro_str < BK_pallete.n_Ro_stratum; n_Ro_str++) {
			for (int i = 0; i < 4; i++) {
				if(comboBox_shart_curves_Y->SelectedItem == "Ia"){
					chart_->Series[i]->Points->AddXY(BK_pallete.data[D_BH_idx][D_PZ_idx][Ro_PZ_idx][n_Ro_str].Ro_stratum_Ro_BH, BK_pallete.data[D_BH_idx][D_PZ_idx][Ro_PZ_idx][n_Ro_str].Ia[i]);
					if(checkBox_leg->Checked == true)
					    chart_->Series[i + 4]->Points->AddXY(BK_pallete_L[D_BH_idx][Ro_PZ_idx][D_PZ_idx][n_Ro_str].Ro_stratum_Ro_BH, BK_pallete_L[D_BH_idx][Ro_PZ_idx][D_PZ_idx][n_Ro_str].Ia[i]);
				}
				if (comboBox_shart_curves_Y->SelectedItem == "Ib"){
					chart_->Series[i]->Points->AddXY(BK_pallete.data[D_BH_idx][D_PZ_idx][Ro_PZ_idx][n_Ro_str].Ro_stratum_Ro_BH, BK_pallete.data[D_BH_idx][D_PZ_idx][Ro_PZ_idx][n_Ro_str].Ib[i]);
					if (checkBox_leg->Checked == true)
						chart_->Series[i + 4]->Points->AddXY(BK_pallete_L[D_BH_idx][Ro_PZ_idx][D_PZ_idx][n_Ro_str].Ro_stratum_Ro_BH, BK_pallete_L[D_BH_idx][Ro_PZ_idx][D_PZ_idx][n_Ro_str].Ib[i]);

				}
				if (comboBox_shart_curves_Y->SelectedItem == "dU"){
					chart_->Series[i]->Points->AddXY(BK_pallete.data[D_BH_idx][D_PZ_idx][Ro_PZ_idx][n_Ro_str].Ro_stratum_Ro_BH, fabs(BK_pallete.data[D_BH_idx][D_PZ_idx][Ro_PZ_idx][n_Ro_str].dU[i]));
					if (checkBox_leg->Checked == true)
						chart_->Series[i + 4]->Points->AddXY(BK_pallete_L[D_BH_idx][Ro_PZ_idx][D_PZ_idx][n_Ro_str].Ro_stratum_Ro_BH, BK_pallete_L[D_BH_idx][Ro_PZ_idx][D_PZ_idx][n_Ro_str].dU[i]);

				}
			}
		}
	}
	
	else if (comboBox_shart_curves_X->SelectedItem == "Ro_stratum / Ro_BH" && (
		comboBox_shart_curves_Y->SelectedItem == "IaS" ||
		comboBox_shart_curves_Y->SelectedItem == "IaM" ||
		comboBox_shart_curves_Y->SelectedItem == "IaL" ||
		comboBox_shart_curves_Y->SelectedItem == "IaSML"||
		
		comboBox_shart_curves_Y->SelectedItem == "IbS" ||
		comboBox_shart_curves_Y->SelectedItem == "IbM" ||
		comboBox_shart_curves_Y->SelectedItem == "IbL" ||
		comboBox_shart_curves_Y->SelectedItem == "IbSML" ||

		comboBox_shart_curves_Y->SelectedItem == "dUS" ||
		comboBox_shart_curves_Y->SelectedItem == "dUM" ||
		comboBox_shart_curves_Y->SelectedItem == "dUL" ||
		comboBox_shart_curves_Y->SelectedItem == "dUSML" 
		)) {
		comboBox_D_BH->Visible = false;
		chart_->Series[0]->ChartType = SeriesChartType::Line;
		Console::WriteLine(" Y " + comboBox_shart_curves_Y->SelectedItem + " for all R_BH"  + " D_PZ_idx " + D_PZ_idx + " Ro_PZ_idx " + Ro_PZ_idx);
		for (int D_BH_idx = 0; D_BH_idx < BK_pallete.R_BH_nodes.size(); D_BH_idx++) {
			chart_->Series[D_BH_idx]->IsVisibleInLegend = true;
			chart_->Series[D_BH_idx]->LegendText = (BK_pallete.R_BH_nodes[D_BH_idx] * 2000.0).ToString("F0");
			for (int n_Ro_str = 0; n_Ro_str < BK_pallete.n_Ro_stratum; n_Ro_str++) {
				if(comboBox_shart_curves_Y->SelectedItem == "IaS")
				    chart_->Series[D_BH_idx]->Points->AddXY(BK_pallete.data[D_BH_idx][D_PZ_idx][Ro_PZ_idx][n_Ro_str].Ro_stratum_Ro_BH, fabs(BK_pallete.data[D_BH_idx][D_PZ_idx][Ro_PZ_idx][n_Ro_str].Ia[S]));
				if (comboBox_shart_curves_Y->SelectedItem == "IaM")
					chart_->Series[D_BH_idx]->Points->AddXY(BK_pallete.data[D_BH_idx][D_PZ_idx][Ro_PZ_idx][n_Ro_str].Ro_stratum_Ro_BH, fabs(BK_pallete.data[D_BH_idx][D_PZ_idx][Ro_PZ_idx][n_Ro_str].Ia[M]));
				if (comboBox_shart_curves_Y->SelectedItem == "IaL")
					chart_->Series[D_BH_idx]->Points->AddXY(BK_pallete.data[D_BH_idx][D_PZ_idx][Ro_PZ_idx][n_Ro_str].Ro_stratum_Ro_BH, fabs(BK_pallete.data[D_BH_idx][D_PZ_idx][Ro_PZ_idx][n_Ro_str].Ia[L]));
				if (comboBox_shart_curves_Y->SelectedItem == "IaSML")
					chart_->Series[D_BH_idx]->Points->AddXY(BK_pallete.data[D_BH_idx][D_PZ_idx][Ro_PZ_idx][n_Ro_str].Ro_stratum_Ro_BH, fabs(BK_pallete.data[D_BH_idx][D_PZ_idx][Ro_PZ_idx][n_Ro_str].Ia[SML]));
			/////////////////////////////////////////////////////////////////////
				if (comboBox_shart_curves_Y->SelectedItem == "IbS")
					chart_->Series[D_BH_idx]->Points->AddXY(BK_pallete.data[D_BH_idx][D_PZ_idx][Ro_PZ_idx][n_Ro_str].Ro_stratum_Ro_BH, fabs(BK_pallete.data[D_BH_idx][D_PZ_idx][Ro_PZ_idx][n_Ro_str].Ib[S]));
				if (comboBox_shart_curves_Y->SelectedItem == "IbM")
					chart_->Series[D_BH_idx]->Points->AddXY(BK_pallete.data[D_BH_idx][D_PZ_idx][Ro_PZ_idx][n_Ro_str].Ro_stratum_Ro_BH, fabs(BK_pallete.data[D_BH_idx][D_PZ_idx][Ro_PZ_idx][n_Ro_str].Ib[M]));
				if (comboBox_shart_curves_Y->SelectedItem == "IbL")
					chart_->Series[D_BH_idx]->Points->AddXY(BK_pallete.data[D_BH_idx][D_PZ_idx][Ro_PZ_idx][n_Ro_str].Ro_stratum_Ro_BH, fabs(BK_pallete.data[D_BH_idx][D_PZ_idx][Ro_PZ_idx][n_Ro_str].Ib[L]));
				if (comboBox_shart_curves_Y->SelectedItem == "IbSML")
					chart_->Series[D_BH_idx]->Points->AddXY(BK_pallete.data[D_BH_idx][D_PZ_idx][Ro_PZ_idx][n_Ro_str].Ro_stratum_Ro_BH, fabs(BK_pallete.data[D_BH_idx][D_PZ_idx][Ro_PZ_idx][n_Ro_str].Ib[SML]));		
			/////////////////////////////////////////////////////////////////////////
				if (comboBox_shart_curves_Y->SelectedItem == "dUS")
					chart_->Series[D_BH_idx]->Points->AddXY(BK_pallete.data[D_BH_idx][D_PZ_idx][Ro_PZ_idx][n_Ro_str].Ro_stratum_Ro_BH, fabs(BK_pallete.data[D_BH_idx][D_PZ_idx][Ro_PZ_idx][n_Ro_str].dU[S]));
				if (comboBox_shart_curves_Y->SelectedItem == "dUM")
					chart_->Series[D_BH_idx]->Points->AddXY(BK_pallete.data[D_BH_idx][D_PZ_idx][Ro_PZ_idx][n_Ro_str].Ro_stratum_Ro_BH, fabs(BK_pallete.data[D_BH_idx][D_PZ_idx][Ro_PZ_idx][n_Ro_str].dU[M]));
				if (comboBox_shart_curves_Y->SelectedItem == "dUL")
					chart_->Series[D_BH_idx]->Points->AddXY(BK_pallete.data[D_BH_idx][D_PZ_idx][Ro_PZ_idx][n_Ro_str].Ro_stratum_Ro_BH, fabs(BK_pallete.data[D_BH_idx][D_PZ_idx][Ro_PZ_idx][n_Ro_str].dU[L]));
				if (comboBox_shart_curves_Y->SelectedItem == "dUSML")
					chart_->Series[D_BH_idx]->Points->AddXY(BK_pallete.data[D_BH_idx][D_PZ_idx][Ro_PZ_idx][n_Ro_str].Ro_stratum_Ro_BH, fabs(BK_pallete.data[D_BH_idx][D_PZ_idx][Ro_PZ_idx][n_Ro_str].dU[SML]));

			}
		}
	}

	//
	else if (comboBox_shart_curves_X->SelectedItem == "IA_M / IA_SML" &&
		comboBox_shart_curves_Y->SelectedItem == "IaM") {
		Console::WriteLine("X: IA_M / IA_SML     Y:IA_M" );
		for (int i = 1; i < count; i++)
			chart_->Series[i]->IsVisibleInLegend = false;
		chart_->Series[0]->LegendText = "IA ratio";
		chart_->Series[0]->ChartType = SeriesChartType::Point;
		for (int n_Ro_str = 0; n_Ro_str < BK_pallete.n_Ro_stratum; n_Ro_str++) {
			double SML_ = BK_pallete.data[D_BH_idx][D_PZ_idx][Ro_PZ_idx][n_Ro_str].Ia[SML];
			double M_ = BK_pallete.data[D_BH_idx][D_PZ_idx][Ro_PZ_idx][n_Ro_str].Ia[M];
			chart_->Series[0]->Points->AddXY( M_ / SML_  , M_);

		}



	}

	else{
		chart_->ChartAreas["ChartArea1"]->AxisX->IsLogarithmic = false;
		chart_->ChartAreas["ChartArea1"]->AxisY->IsLogarithmic = false;
	}
}

bool first = true;
private: System::Void button_do_chart_Click(System::Object^  sender, System::EventArgs^  e) {
	first = false;
	do_chart();
}

private: System::Void button_calc_one_Clic(System::Object^  sender, System::EventArgs^  e) {
	cout << fixed << endl;
	
	SondeParams p = fill_params_from_GUI();
	vector<Electrode> A_group;
	vector<Electrode> B_group;
	vector<MeasElectrode> M_group;
	vector<double> r_coords;
	vector<double> z_coords;
	vector<LayerDakhnov> model;

	
	std::vector<double> layer_boundaries;
	LayerDakhnov B_hole, Pen_zone, stratum;

	//////////////////////////////
	B_hole.r_boundary = GetDouble(textBox_D_BH_mm, 147) / 2000.0; //
	layer_boundaries.push_back(B_hole.r_boundary);
	B_hole.rho = GetDouble(textBox_Ro_BH, 1);
	model.push_back(B_hole);
	
	//
	Pen_zone.r_boundary = B_hole.r_boundary * GetDouble(textBox_R_PZ_R_BH, 1);
	layer_boundaries.push_back(Pen_zone.r_boundary);
	Pen_zone.rho = GetDouble(textBox_Ro_PZ, 1.0);
	model.push_back(Pen_zone);

	stratum.r_boundary = p.r_max;
	stratum.rho = GetDouble(textBox_Ro_stratum, 1.0);
	layer_boundaries.push_back(stratum.r_boundary);
	model.push_back(stratum);

	//cout << "  p.r_max " << p.r_max << " r_tool " << p.r_tool << " p.dr_start " << p.dr_start << " p.q_r " << p.q_r << endl;
	//cout << "  p.z_tool_start " << p.z_tool_start << " p.z_tool_end " << p.z_tool_end << " p.dz_tool " << p.dz_tool << " p.q_z " << p.q_z << " p.tail " << p.tail << endl;
	//r_coords = buildRadialGrid(r_tool, r_max, dr_start, q_r);//old
	r_coords = buildRadialGridAdaptive(p.r_tool, layer_boundaries, p.dr_start, p.q_r); //
	
	int work_type = comboBox_work_type->SelectedIndex;
	
	set_work_electrodes(work_type, p, A_group, B_group, M_group);//
	z_coords = buildSmartDepthGrid_new(A_group, B_group, M_group, p.dz_tool, p.dz_fine, p.threshold, p.q_z_tail, p.tail);
	//z_coords = buildDepthGrid(p.z_tool_start, p.z_tool_end, p.dz_tool, p.q_z, p.tail);
	
	// Вывод информации о получившейся матрице
	cout << "=== КОНФИГУРАЦИЯ СЕТКИ ===\n";
	cout << "Узлов по радиусу (W): " << r_coords.size() << "\n";
	cout << "Узлов по глубине (H): " << z_coords.size() << "\n";
	cout << "Размер матрицы (N)  : " << r_coords.size() * z_coords.size() << " узлов\n\n";

	//инициализация сетки
	ResistorGrid grid(r_coords, z_coords);
	grid.fillModel(model);

	grid.setSonde(A_group, B_group, M_group);
	grid.solveDirect(debug_mode);

	cout << "\n=== ДЕТАЛЬНЫЙ БАЛАНС ТОКОВ ===\n";
	double sum_A = 0.0;
	for (const auto& a : A_group) {
		double I = grid.getElectrodeCurrent(a);
		sum_A += I;
		cout << a.name << " (current el): " << fixed << setprecision(6) << I << " A\n";
	}
	cout << "---------------------------------\n";
	double sum_B = 0.0;
	for (const auto& b : B_group) {
		double I = abs(grid.getElectrodeCurrent(b));
		sum_B += I;
		cout << b.name << " (ground el): " << fixed << setprecision(6) << I << " A\n";
	}
	double sum_Right = grid.getRightBoundaryCurrent();
	double total_out = sum_B + sum_Right;
	double balance_error = abs(sum_A - total_out);
	cout << "---------------------------------\n";
	cout << "Разница потенциалов dU UM - UN "  << grid.getMeasElectrodeVoltage(M_group[0]) - grid.getMeasElectrodeVoltage(M_group[1]) << " V\n";
	cout << "---------------------------------\n";
	cout << "Сумма втекающих (I_A):     " << sum_A << " A\n";
	cout << "Сумма на электроды (I_B):  " << sum_B << " A\n";
	cout << "Свободный уход (I_Right):  " << sum_Right << " A\n";
	cout << "НЕВЯЗКА (Ошибка Кирхгофа): " << scientific << fixed  << balance_error << " A\n\n";
	
	grid.exportMapsWithLegendToBMP();


	Bitmap^ tempImage = gcnew Bitmap("potentials_adaptive.bmp");
    mainImage = gcnew Bitmap(tempImage);
    delete tempImage;
	
	offset.X = (pictureBox_U->Width - mainImage->Width * zoom) / 2;
	offset.Y = (pictureBox_U->Height - mainImage->Height * zoom) / 2;
	pictureBox_U->Invalidate(); 
	pictureBox_U->Focus();     

	offset = PointF(0, 0);
	pictureBox_U->MouseWheel += gcnew MouseEventHandler(this, &MyForm::OnMouseWheel);
	

}

private: System::Void button_calk_one__Click(System::Object^  sender, System::EventArgs^  e) {
	cout << fixed << endl;
	//SondeParams p = fill_params_from_GUI();
	Params p = fill_params();
	vector<Electrode> A_group;
	vector<Electrode> B_group;
	vector<MeasElectrode> M_group;
	vector<double> r_coords;
	vector<double> z_coords;
	vector<LayerDakhnov> model;
	std::vector<double> layer_boundaries;
	LayerDakhnov B_hole, Pen_zone, stratum;

	DataGridViewRow^ row = dataGridView_grid_param->Rows[0];

	B_hole.r_boundary = GetGridVal(dataGridView_grid_param, row, "D_BH_mm", 147) / 2000.0;
	layer_boundaries.push_back(B_hole.r_boundary);
	B_hole.rho = GetGridVal(dataGridView_grid_param, row, "Ro_BH", 1);
	model.push_back(B_hole);

	Pen_zone.r_boundary = GetGridVal(dataGridView_grid_param, row, "D_PZ_D_BH", 1);
	layer_boundaries.push_back(Pen_zone.r_boundary);
	Pen_zone.rho = GetGridVal(dataGridView_grid_param, row, "Ro_PZ", 1.0);
	model.push_back(Pen_zone);

	stratum.r_boundary = p.r_max;
	stratum.rho = GetGridVal(dataGridView_grid_param, row, "Ro_stratum", 1.0);
	layer_boundaries.push_back(stratum.r_boundary);
	model.push_back(stratum);

	//cout << "  p.r_max " << p.r_max << " r_tool " << p.r_tool << " p.dr_start " << p.dr_start << " p.q_r " << p.q_r << endl;
	//cout << "  p.z_tool_start " << p.z_tool_start << " p.z_tool_end " << p.z_tool_end << " p.dz_tool " << p.dz_tool << " p.q_z " << p.q_z << " p.tail " << p.tail << endl;
	r_coords = buildRadialGridAdaptive(p.r_tool, layer_boundaries, p.dr_start, p.q_r); //

	int work_mode = comboBox_work_mode->SelectedIndex;
	set_work_electrodes_new(work_mode, electrodeList, p, A_group, B_group, M_group);//
	z_coords = buildSmartDepthGrid_new(A_group, B_group, M_group, p.dz_tool, p.dz_fine, p.threshold, p.q_z_tail, p.tail);
	
	// Вывод информации о получившейся матрице
	cout << "=== КОНФИГУРАЦИЯ СЕТКИ ===\n";
	cout << "Узлов по радиусу (W): " << r_coords.size() << "\n";
	cout << "Узлов по глубине (H): " << z_coords.size() << "\n";
	cout << "Размер матрицы (N)  : " << r_coords.size() * z_coords.size() << " узлов\n\n";

	//инициализация сетки
	ResistorGrid grid(r_coords, z_coords);
	grid.fillModel(model);

	grid.setSonde(A_group, B_group, M_group);
	grid.solveDirect(debug_mode);

	cout << "\n=== ДЕТАЛЬНЫЙ БАЛАНС ТОКОВ ===\n";
	double sum_A = 0.0;
	for (const auto& a : A_group) {
		double I = grid.getElectrodeCurrent(a);
		sum_A += I;
		cout << a.name << " (current el): " << fixed << setprecision(6) << I << " A\n";
	}
	cout << "---------------------------------\n";
	double sum_B = 0.0;
	for (const auto& b : B_group) {
		double I = abs(grid.getElectrodeCurrent(b));
		sum_B += I;
		cout << b.name << " (ground el): " << fixed << setprecision(6) << I << " A\n";
	}
	double sum_Right = grid.getRightBoundaryCurrent();
	double total_out = sum_B + sum_Right;
	double balance_error = abs(sum_A - total_out);
	cout << "---------------------------------\n";
	cout << "Разница потенциалов dU UM - UN " << grid.getMeasElectrodeVoltage(M_group[0]) - grid.getMeasElectrodeVoltage(M_group[1]) << " V\n";
	cout << "---------------------------------\n";
	cout << "Сумма втекающих (I_A):     " << sum_A << " A\n";
	cout << "Сумма на электроды (I_B):  " << sum_B << " A\n";
	cout << "Свободный уход (I_Right):  " << sum_Right << " A\n";
	cout << "НЕВЯЗКА (Ошибка Кирхгофа): " << scientific << fixed << balance_error << " A\n\n";

	grid.exportMapsWithLegendToBMP();


	Bitmap^ tempImage = gcnew Bitmap("potentials_adaptive.bmp");
	mainImage = gcnew Bitmap(tempImage);
	delete tempImage;

	offset.X = (pictureBox_U->Width - mainImage->Width * zoom) / 2;
	offset.Y = (pictureBox_U->Height - mainImage->Height * zoom) / 2;
	pictureBox_U->Invalidate();
	pictureBox_U->Focus();

	offset = PointF(0, 0);
	pictureBox_U->MouseWheel += gcnew MouseEventHandler(this, &MyForm::OnMouseWheel);



}

private: System::Void button_calc_four_Click(System::Object^  sender, System::EventArgs^  e) {
	SondeParams p = fill_params_from_GUI();

	vector<LayerDakhnov> model;
	LayerDakhnov B_hole, Pen_zone, stratum;
	//add layers boundarys//
	std::vector<double> layer_boundaries;//
	B_hole.r_boundary = GetDouble(textBox_D_BH_mm, 147) / 2000.0; //пїЅпїЅпїЅпїЅпїЅпїЅ пїЅ пїЅпїЅ
	layer_boundaries.push_back(B_hole.r_boundary);
	B_hole.rho = GetDouble(textBox_Ro_BH, 1.0);
	model.push_back(B_hole);
	
	Pen_zone.r_boundary = B_hole.r_boundary * GetDouble(textBox_R_PZ_R_BH, 1);
	layer_boundaries.push_back(Pen_zone.r_boundary);
	Pen_zone.rho = GetDouble(textBox_Ro_PZ, 1.0);
	model.push_back(Pen_zone);

	stratum.r_boundary = p.r_max;
	stratum.rho = GetDouble(textBox_Ro_stratum, 1.0);
	layer_boundaries.push_back(stratum.r_boundary);
	model.push_back(stratum);

	//строим адаптивную сетку по радиусу и заполняем вектор радиальных координат 
	vector<double> r_coords = buildRadialGridAdaptive(p.r_tool, layer_boundaries, p.dr_start, p.q_r);

	BK bk_buff;
	for (int i = 0; i < 4; i++) {
		vector<Electrode> A_group;
		vector<Electrode> B_group;
		vector<MeasElectrode> M_group;
		//устанавливаем электроды
		set_work_electrodes(i, p, A_group, B_group, M_group);
		//строим адаптивную сетку по оси и заполняем вектор осевых координат 
		vector<double> z_coords = buildSmartDepthGrid_new(A_group, B_group, M_group, p.dz_tool, p.dz_fine, p.threshold, p.q_z_tail, p.tail);
		ResistorGrid grid(r_coords, z_coords);
		//заполняем модель среды, инициализируем прибор и решаем
		grid.fillModel(model);
		grid.setSonde(A_group, B_group, M_group);
		grid.solveDirect(debug_mode);
		if (i == 0) {
			// в режиме SML все электроды А -токоые только UM и UN - измерительные
			bk_buff.Ia[i] = grid.getElectrodeCurrent(A_group[1]);		
		}
			
		else {
			//в режимах S,M,L измерительный электрод только один
			bk_buff.Ia[i] = grid.getElectrodeCurrent(A_group[0]);
			
		}
		//во всех режимах заземляющие электроды одни и те же
		bk_buff.Ib[i] = abs(grid.getElectrodeCurrent(B_group[0]));

		bk_buff.dU[i] = grid.getMeasElectrodeVoltage(M_group[0]) - grid.getMeasElectrodeVoltage(M_group[1]);
	}

	bk_buff.R_BH = B_hole.r_boundary;
	bk_buff.R_PZ_R_BH = Pen_zone.r_boundary;
	bk_buff.Ro_PZ_Ro_BH = Pen_zone.rho;
	bk_buff.Ro_stratum_Ro_BH = stratum.rho;

	cout << " R_BH = " << B_hole.rho  << "; D_BH mm = " << bk_buff.R_BH*2000.0 << "; D_PZ mm = " << bk_buff.R_PZ_R_BH *2000 << "; Ro_PZ  = " << bk_buff.Ro_PZ_Ro_BH << "; Ro_stratum  = " << bk_buff.Ro_stratum_Ro_BH << endl;

	cout << "One point data" << endl;
	cout << "Ia SML, S, M, L, ";
	for (int i = 0; i < 4; i++)
		cout << bk_buff.Ia[i] << " ";
	cout << endl;

	cout << "Ib SML, S, M, L, ";
	for (int i = 0; i < 4; i++)
		cout << bk_buff.Ib[i] << " ";
	cout << endl;

	cout << "dU SML, S, M, L, ";
	for (int i = 0; i < 4; i++)
		cout << bk_buff.dU[i] << " ";
	cout << endl;

}

private: System::Void button_calc_N_Click(System::Object^  sender, System::EventArgs^  e) {
	Params p = fill_params();
	vector<LayerDakhnov> model;
	LayerDakhnov B_hole, Pen_zone, stratum;
	std::vector<double> layer_boundaries;//

	DataGridViewRow^ row = dataGridView_grid_param->Rows[0];
	B_hole.r_boundary = GetGridVal(dataGridView_grid_param, row, "D_BH_mm", 147) / 2000.0;
	layer_boundaries.push_back(B_hole.r_boundary);
	B_hole.rho = GetGridVal(dataGridView_grid_param, row, "Ro_BH", 1);
	model.push_back(B_hole);

	Pen_zone.r_boundary = GetGridVal(dataGridView_grid_param, row, "D_PZ_D_BH", 1);
	layer_boundaries.push_back(Pen_zone.r_boundary);
	Pen_zone.rho = GetGridVal(dataGridView_grid_param, row, "Ro_PZ", 1.0);
	model.push_back(Pen_zone);

	stratum.r_boundary = p.r_max;
	stratum.rho = GetGridVal(dataGridView_grid_param, row, "Ro_stratum", 1.0);
	layer_boundaries.push_back(stratum.r_boundary);
	model.push_back(stratum);

    vector<double> r_coords = buildRadialGridAdaptive(p.r_tool, layer_boundaries, p.dr_start, p.q_r);


	// 1. Получаем количество режимов от пользователя
	uint32_t N = (uint32_t)4; // например, 4 или любое другое число

	// 2. Инициализируем структуру нужным размером
	BK_ bk_buff(N);

	vector<Electrode> A_group;
	vector<Electrode> B_group;
	vector<MeasElectrode> M_group;
	
	
	// 3. Основной цикл вычислений
	for (uint32_t i = 0; i < N; i++) {
		vector<Electrode> A_group;
		vector<Electrode> B_group;
		vector<MeasElectrode> M_group;

		// Передаем i в функцию, если логика выбора электродов зависит от номера итерации
		set_work_electrodes_new(i, electrodeList, p, A_group, B_group, M_group);

		vector<double> z_coords = buildSmartDepthGrid_new(A_group, B_group, M_group, p.dz_tool, p.dz_fine, p.threshold, p.q_z_tail, p.tail);
		ResistorGrid grid(r_coords, z_coords);

		grid.fillModel(model);
		grid.setSonde(A_group, B_group, M_group);
		grid.solveDirect(debug_mode);

		// Логика записи результатов
		if (i == 0) {
			// Пример вашей логики для первого режима
			bk_buff.Ia[i] = grid.getElectrodeCurrent(A_group[1]);
		}
		else {
			bk_buff.Ia[i] = grid.getElectrodeCurrent(A_group[0]);
		}

		bk_buff.Ib[i] = abs(grid.getElectrodeCurrent(B_group[0]));
		bk_buff.dU[i] = grid.getMeasElectrodeVoltage(M_group[0]) - grid.getMeasElectrodeVoltage(M_group[1]);
	}

	// 4. Заполнение параметров среды
	bk_buff.R_BH = B_hole.r_boundary;
	bk_buff.R_PZ_R_BH = Pen_zone.r_boundary;
	bk_buff.Ro_PZ_Ro_BH = Pen_zone.rho;
	bk_buff.Ro_stratum_Ro_BH = stratum.rho;

	// 5. Вывод (теперь цикл до N)
	cout << "Data for " << N << " modes:" << endl;
	for (uint32_t i = 0; i < N; i++) {
		cout << "Mode " << i << ": Ia=" << bk_buff.Ia[i]
			<< ", Ib=" << bk_buff.Ib[i]
			<< ", dU=" << bk_buff.dU[i] << endl;
	}
	

}

SondeParams  fill_params_from_GUI() {
	SondeParams p;
	/////////////////////////////////////////////////
	p.dr_start = GetDouble(textBox_dr_start_mm, 1) / 1000.0;
	p.q_r = GetDouble(textBox_q_r, 1.1);
	p.r_max = GetDouble(textBox_r_max_m, 3.5);
	p.dz_tool = GetDouble(textBox_dz_base_mm, 5) / 1000.0;
	p.q_z_tail = GetDouble(textBox_q_z_tail, 1.15);
	p.dz_fine = GetDouble(textBox_dz_fine_mm, 1)/1000.0;
	p.threshold = GetDouble(textBox_threshold_mm, 20) / 1000.0;
	
	/////////////////////////////////////////////////
	p.r_tool = GetDouble(textBox_D_sonde, 90) / 2000.0;
	p.tail = GetDouble(textBox_tail, 2.5);
	p.z_tool_start = p.tail;
	p.voltage = 1.0;
	p.short_z = GetDouble(textBox_SHORT_Z, 1450);
	p.short_l = GetDouble(textBox_SHORT_L, 100);
	p.mid_z = GetDouble(textBox_MID_Z, 1950);
	p.mid_l = GetDouble(textBox_MID_L, 100);
	p.long_z = GetDouble(textBox_LONG_Z, 2450);
	p.long_l = GetDouble(textBox_LONG_L, 100);
	p.b0_z = GetDouble(textBox_B0_Z, 4005);
	p.b0_l = GetDouble(textBox_B0_L, 95);
	p.b1_z = GetDouble(textBox_B1_GND_Z, 0);
	p.b1_l = GetDouble(textBox_B1_GND_L, 1000);
	p.b2_z = GetDouble(textBox_B2_GND_Z, 3005);
	p.b2_l = GetDouble(textBox_B2_GND_L, 995);
	p.b2_add_z = GetDouble(textBox_B2_add_GND_Z, 4105);
	p.b2_add_l = GetDouble(textBox_B2_add_GND_L, 1500);
	p.z_tool_end = p.tail + (p.b2_add_z + p.b2_add_l)/1000.0;
	p.um_z = GetDouble(textBox_U_M_Z, 2722);
	p.um_l = GetDouble(textBox_U_M_L, 10);
	p.un_z = GetDouble(textBox_U_N_Z, 2822);
	p.un_l = GetDouble(textBox_U_N_L, 10);
	return p;
}

static double GetGridVal(DataGridView^ grid, DataGridViewRow^ row, String^ colName, double defaultVal) {
	if (row != nullptr && grid->Columns->Contains(colName)) {
		Object^ cellValue = row->Cells[colName]->Value;
		if (cellValue == nullptr) return defaultVal;
		try {
			// Точно по вашей логике: берем текст, заменяем точку на запятую
			String^ text = cellValue->ToString()->Replace(".", ",");
			if (String::IsNullOrWhiteSpace(text)) return defaultVal;
			return Convert::ToDouble(text);
		}
		catch (...) {
			return defaultVal;
		}
	}
	return defaultVal;
}

Params fill_params() {
	Params p;
	// Проверка наличия данных в таблице
	if (dataGridView_grid_param->Rows->Count == 0) return p;

	DataGridViewRow^ row = dataGridView_grid_param->Rows[0]; // Берем единственную строку параметров

	p.dr_start = GetGridVal(dataGridView_grid_param, row, "dr_start_mm", 1.0) / 1000.0;
	p.q_r = GetGridVal(dataGridView_grid_param, row, "q_r", 1.1);
	p.r_max = GetGridVal(dataGridView_grid_param, row, "r_max_m", 3.5);
	p.dz_tool = GetGridVal(dataGridView_grid_param, row, "dz_base_mm", 5.0) / 1000.0;
	p.q_z_tail = GetGridVal(dataGridView_grid_param, row, "q_z_tail", 1.15);
	p.dz_fine = GetGridVal(dataGridView_grid_param, row, "dz_fine_mm", 1.0) / 1000.0;
	p.threshold = GetGridVal(dataGridView_grid_param, row, "threshold_mm", 20.0) / 1000.0;
	p.r_tool = GetGridVal(dataGridView_grid_param, row, "D_sonde", 90.0) / 2000.0;
	p.tail = GetGridVal(dataGridView_grid_param, row, "tail", 2.5);

	p.z_tool_start = p.tail;
	p.voltage = 1.0;

	return p;
}

// заполнянм электроды
void set_work_electrodes(int work_mode, const SondeParams& p,
	vector<Electrode>& A_gr,
	vector<Electrode>& B_gr,
	vector<MeasElectrode>& M_gr)
{
	//три вида электродов - токовые U = bat, заземление U = 0, измерительные U измеряем.
	A_gr.clear(); B_gr.clear(); M_gr.clear();

	//координаты начала и конца электродов по оси (Z)
	double s_z_s = p.tail + p.short_z / 1000.0;
	double s_z_e = s_z_s + p.short_l / 1000.0;
	double m_z_s = p.tail + p.mid_z / 1000.0;
	double m_z_e = m_z_s + p.mid_l / 1000.0;
	double l_z_s = p.tail + p.long_z / 1000.0;
	double l_z_e = l_z_s + p.long_l / 1000.0;

	//Сначала добавляем электроды, которые всегда измерительные
	M_gr.push_back({ p.tail + p.um_z / 1000.0, p.tail + (p.um_z + p.um_l) / 1000.0, "U_M" });
	M_gr.push_back({ p.tail + p.un_z / 1000.0, p.tail + (p.un_z + p.un_l) / 1000.0, "U_N" });

	// В зависимости от режима работы заполняем токовые электроды SML или S или M или L
	//а остальные заполняем как измерительные
	if (work_mode == SML) {
		A_gr.push_back({ s_z_s, s_z_e, p.voltage, "shot work" });
		A_gr.push_back({ m_z_s, m_z_e, p.voltage, "Midddle work" });
		A_gr.push_back({ l_z_s, l_z_e, p.voltage, "Long work" });
	}
	else if (work_mode == S) {
		A_gr.push_back({ s_z_s, s_z_e, p.voltage, "shot work" });
		M_gr.push_back({ m_z_s, m_z_e, "Midddle meas" });
		M_gr.push_back({ l_z_s, l_z_e, "Long meas" });
	}
	else if (work_mode == M) {
		M_gr.push_back({ s_z_s, s_z_e, "shot meas" });
		A_gr.push_back({ m_z_s, m_z_e, p.voltage, "Midddle work" });
		M_gr.push_back({ l_z_s, l_z_e, "Long meas" });
	}
	else if (work_mode == L) {
		M_gr.push_back({ s_z_s, s_z_e, "shot meas" });
		M_gr.push_back({ m_z_s, m_z_e, "Midddle meas" });
		A_gr.push_back({ l_z_s, l_z_e, p.voltage, "Long work" });
	}

	//заполняем электроды заземления
	B_gr.push_back({ p.tail + p.b0_z / 1000.0, p.tail + (p.b0_z + p.b0_l) / 1000.0, 0.0, "B0" });
	B_gr.push_back({ p.tail + p.b1_z / 1000.0, p.tail + (p.b1_z + p.b1_l) / 1000.0, 0.0, "B1" });
	B_gr.push_back({ p.tail + p.b2_z / 1000.0, p.tail + (p.b2_z + p.b2_l) / 1000.0, 0.0, "B2" });
	B_gr.push_back({ p.tail + p.b2_add_z / 1000.0, p.tail + (p.b2_add_z + p.b2_add_l) / 1000.0, 0.0, "B2_add" });

	
}


void set_work_electrodes_new(int wm, const vector<vector<ELECTRODE>> &electrodeList, Params p,
	vector<Electrode>& A_gr,
	vector<Electrode>& B_gr,
	vector<MeasElectrode>& M_gr)
{
	//три вида электродов - токовые U = bat, заземление U = 0, измерительные U измеряем.
	A_gr.clear(); B_gr.clear(); M_gr.clear();

	int N_electrodes = electrodeList[0].size(); //количество столбцов
	cout << "N_electrodes "<< N_electrodes <<endl;
	for (int el = 0; el < N_electrodes; el++) {
		if (electrodeList[wm][el].mode == CURR) {
			double s_z_m = p.tail + electrodeList[wm][el].z_start_mm / 1000.0;
			double e_z_m = p.tail + electrodeList[wm][el].z_end_mm / 1000.0;
			A_gr.push_back({ s_z_m , e_z_m, p.voltage, electrodeList[wm][el].name });
		}
		if (electrodeList[wm][el].mode == GRND) {
			double s_z_m = p.tail + electrodeList[wm][el].z_start_mm / 1000.0;
			double e_z_m = p.tail + electrodeList[wm][el].z_end_mm / 1000.0;
			B_gr.push_back({ s_z_m , e_z_m, 0.0, electrodeList[wm][el].name });
		}
		if (electrodeList[wm][el].mode == MEAS) {
			double s_z_m = p.tail + electrodeList[wm][el].z_start_mm / 1000.0;
			double e_z_m = p.tail + electrodeList[wm][el].z_end_mm / 1000.0;
			M_gr.push_back({ s_z_m , e_z_m, electrodeList[wm][el].name });
		}
	}
	
	for (int i = 0; i < A_gr.size(); i++) {
		cout << "A name " << A_gr[i].name << " V " << A_gr[i].voltage << " start " << A_gr[i].z_start_m << " end " << A_gr[i].z_end_m << endl;
	}
	cout << endl;
	for (int i = 0; i <B_gr.size(); i++) {
		cout << "B name " << B_gr[i].name << " V " << B_gr[i].voltage << " start " << B_gr[i].z_start_m << " end " << B_gr[i].z_end_m << endl;
	}
	cout << endl;
	for (int i = 0; i < M_gr.size(); i++) {
		cout << "M name " << M_gr[i].name  << " start " << M_gr[i].z_start_m << " end " << M_gr[i].z_end_m << endl;
	}

}

private: System::Void button_set_environment_Click(System::Object^  sender, System::EventArgs^  e) {
	cout << fixed << setprecision(3);
	//
	Ro_BH_steps.clear();
	Ro_PZ_steps.clear();
	Ro_stratum_steps.clear();
	R_BH_steps.clear();
	R_PZ_steps.clear();

	bool sucsess = true;
	enum CHOICE {
		GIVEN, LOGARITMIC, LINEAR
	};

	Ro_BH_steps.push_back(1.0);

	/*
	//read Ro_BH_steps
	if (checkBox_Ro_BH_1->Checked == true) {
		Ro_BH_steps.push_back(1.0);
	}
	else {
		//read Ro_BH_steps
		try {
			std::stringstream ss(msclr::interop::marshal_as<std::string>(textBox_Ro_BH_step->Text->Replace(",", ".")));
			Ro_BH_steps.clear();
			double temp;
			if (comboBox_choice->SelectedIndex == GIVEN) {
				while (ss >> temp) { Ro_BH_steps.push_back(temp); }
			}
			if (comboBox_choice->SelectedIndex == LOGARITMIC) {
				double start; double Q; double end;
				ss >> start; ss >> Q; ss >> end;
				//cout << start <<" " << Q <<" " << end << endl;
				for (double n = start; n < end; n *= Q) {
					//cout << n << endl;
					Ro_BH_steps.push_back(n);
				}
			}
			if (comboBox_choice->SelectedIndex == LINEAR) {
				double start; double step; double end;
				ss >> start; ss >> step; ss >> end;
				for (double n = start; n <= end; n += step) {
					Ro_BH_steps.push_back(n);
				}
			}

		}
		catch (...) { sucsess = false; }
	}
	*/

	// read R_BH_step
	try {
		std::stringstream ss(msclr::interop::marshal_as<std::string>(textBox_D_BH_step_mm->Text->Replace(",", ".")));
		R_BH_steps.clear();
		double temp;
		if (comboBox_choice->SelectedIndex == GIVEN) {
			while (ss >> temp) { R_BH_steps.push_back(temp/2000.0); }//
		}
		if (comboBox_choice->SelectedIndex == LOGARITMIC) {
			double start; double Q; double end;
			ss >> start; ss >> Q; ss >> end;
			start /= 2000.0; end /= 2000.0;
			//cout << start <<" " << Q <<" " << end << endl;
			double first_radius = GetDouble(textBox_D_sonde, 90) / 2000.0;//
			R_BH_steps.push_back(first_radius);
			for (double n = start; n < end; n *= Q) {
				//cout << n << endl;
				R_BH_steps.push_back(n);
			}
		}
		if (comboBox_choice->SelectedIndex == LINEAR) {
			double start; double step; double end;
			ss >> start; ss >> step; ss >> end;
			double first_radius = GetDouble(textBox_D_sonde, 90) / 2000.0;//
			R_BH_steps.push_back(first_radius);
			start /= 2000.0; end /= 2000.0;
			for (double n = start; n <= end; n += step) {
				R_BH_steps.push_back(n);
			}
		}
	}
	catch (...) { sucsess = false; }
	//read R_PZ/R_BH_step
	try {
		std::stringstream ss(msclr::interop::marshal_as<std::string>(textBox_R_PZ_R_BH_step->Text->Replace(",", ".")));
		R_PZ_steps.clear();
		double temp;
		if (comboBox_choice->SelectedIndex == GIVEN) {
			while (ss >> temp) { R_PZ_steps.push_back(temp); }
		}
		if (comboBox_choice->SelectedIndex == LOGARITMIC) {
			double start; double Q; double end;
			ss >> start; ss >> Q; ss >> end;
			//cout << start <<" " << Q <<" " << end << endl;
			double first_radius = 1.0;
			R_PZ_steps.push_back(first_radius);//
			for (double n = start; n < end; n *= Q) {
				//cout << n << endl;
				R_PZ_steps.push_back(n);
			}
		}
		if (comboBox_choice->SelectedIndex == LINEAR) {
			double start; double step; double end;
			ss >> start; ss >> step; ss >> end;//
			double first_radius = 1.0;
			R_PZ_steps.push_back(first_radius);
			for (double n = start; n <= end; n += step) {
				R_PZ_steps.push_back(n);
			}
		}
	}
	catch (...) { sucsess = false; }
	//read Ro_PZ_step
	try {
		std::stringstream ss(msclr::interop::marshal_as<std::string>(textBox_Ro_PZ_step->Text->Replace(",", ".")));
		Ro_PZ_steps.clear();
		double temp;
		if (comboBox_choice->SelectedIndex == GIVEN) {
			while (ss >> temp) { Ro_PZ_steps.push_back(temp); }
		}
		if (comboBox_choice->SelectedIndex == LOGARITMIC) {
			double start; double Q; double end;
			ss >> start; ss >> Q; ss >> end;
			//cout << start <<" " << Q <<" " << end << endl;
			for (double n = start; n < end; n *= Q) {
				//cout << n << endl;
				Ro_PZ_steps.push_back(n);
			}
		}
		if (comboBox_choice->SelectedIndex == LINEAR) {
			double start; double step; double end;
			ss >> start; ss >> step; ss >> end;
			for (double n = start; n <= end; n += step) {
				Ro_PZ_steps.push_back(n);
			}
		}
	}
	catch (...) { sucsess = false; }
	//read Ro_stratum_step
	try {
		std::stringstream ss(msclr::interop::marshal_as<std::string>(textBox_Ro_stratum_step->Text->Replace(",", ".")));
		Ro_stratum_steps.clear();
		double temp;
		if (comboBox_choice->SelectedIndex == GIVEN) {
			while (ss >> temp) { Ro_stratum_steps.push_back(temp); }
		}
		if (comboBox_choice->SelectedIndex == LOGARITMIC) {
			double start; double Q; double end;
			ss >> start; ss >> Q; ss >> end;
			//cout << start <<" " << Q <<" " << end << endl;
			for (double n = start; n < end; n *= Q) {
				//cout << n << endl;
				Ro_stratum_steps.push_back(n);
			}
		}
		if (comboBox_choice->SelectedIndex == LINEAR) {
			double start; double step; double end;
			ss >> start; ss >> step; ss >> end;
			for (double n = start; n <= end; n += step) {
				Ro_stratum_steps.push_back(n);
			}
		}
	}
	catch (...) { sucsess = false; }
	
	cout << "sucsess " << sucsess << endl;
	//ss.str(""); ss.clear();
	if (sucsess == true) {

		//R_BH, R_PZ, Ro_PZ, RO_stratum
		cout << " D_BH_mm_steps " << R_BH_steps.size() << " : ";
		for (int i = 0; i < R_BH_steps.size(); i++)
			cout << R_BH_steps[i] * 2000.0 << " ";
		cout << endl; cout << endl;

		cout << " D_PZ/D_BH_steps " << R_PZ_steps.size() << " : ";
		for (int i = 0; i < R_PZ_steps.size(); i++)
			cout << R_PZ_steps[i] << " ";
		cout << endl; cout << endl;

		cout << " Ro_PZ/Ro_BH_steps " << Ro_PZ_steps.size() << " : ";
		for (int i = 0; i < Ro_PZ_steps.size(); i++)
			cout << Ro_PZ_steps[i] << " ";
		cout << endl; cout << endl;

		cout << " Ro_stratum/Ro_BH_steps " << Ro_stratum_steps.size() << " : ";
		for (int i = 0; i < Ro_stratum_steps.size(); i++)
			cout << Ro_stratum_steps[i] << " ";
		cout << endl;

		pallete_size = R_BH_steps.size()*R_PZ_steps.size()*Ro_PZ_steps.size()*Ro_stratum_steps.size();
		cout << " pallete size  " << pallete_size << endl;
		cout << " calk time H  " << pallete_size / 36000.0 << endl;
		cout << endl; cout << endl;
	}

}

const char *pallete_file_mame;

/*
void Pallete_Calculation() {
		auto start = std::chrono::steady_clock::now();

		uint32_t status = NOT_COMPLETED;
		ofstream fout;
		SondeParams p = fill_params_from_GUI();

		//
		BK_PALLETE_FILE_HEADER BK_header;
		BK_header.status = NOT_COMPLETED;
		BK_header.params = p;

		
		//R_BH, R_PZ, Ro_PZ, RO_stratum
		for (int n_R_BH = 0; n_R_BH < R_BH_steps.size(); n_R_BH++) {//
			BK_header.BK_pallete_nodes.R_BH[n_R_BH] = R_BH_steps[n_R_BH];
		}
		BK_header.BK_pallete_nodes.points[0] = R_BH_steps.size();

		for (int n_R_PZ = 0; n_R_PZ < R_PZ_steps.size(); n_R_PZ++) {// 
			BK_header.BK_pallete_nodes.R_PZ_R_BH[n_R_PZ] = R_PZ_steps[n_R_PZ];
		}
		BK_header.BK_pallete_nodes.points[1] = R_PZ_steps.size();

		for (int n_Ro_PZ = 0; n_Ro_PZ < Ro_PZ_steps.size(); n_Ro_PZ++) {//
			BK_header.BK_pallete_nodes.Ro_PZ_Ro_BH[n_Ro_PZ] = Ro_PZ_steps[n_Ro_PZ];
		}
		BK_header.BK_pallete_nodes.points[2] = Ro_PZ_steps.size();

		for (int n_Ro_stratum = 0; n_Ro_stratum < (int)Ro_stratum_steps.size(); n_Ro_stratum++) {//
			BK_header.BK_pallete_nodes.Ro_stratum_Ro_BH[n_Ro_stratum] = Ro_stratum_steps[n_Ro_stratum];
		}
		BK_header.BK_pallete_nodes.points[3] = Ro_stratum_steps.size();

		fout.open(pallete_file_mame, std::ios::binary);
		fout.write((char*)&BK_header, sizeof(BK_PALLETE_FILE_HEADER));
		fout.close();

		
		omp_set_num_threads(14);
		std::cout << "OpenMP threads : " << omp_get_max_threads() << std::endl;

		uint32_t N_points = 0;
		//Ro_BH =1; R_BH, R_PZ, Ro_PZ, RO_stratum
		for (int n_Ro_BH = 0; n_Ro_BH < Ro_BH_steps.size(); n_Ro_BH++) {//

			for (int n_R_BH = 0; n_R_BH < R_BH_steps.size(); n_R_BH++) {//

				for (int n_R_PZ = 0; n_R_PZ < R_PZ_steps.size(); n_R_PZ++) {//

					for (int n_Ro_PZ = 0; n_Ro_PZ < Ro_PZ_steps.size(); n_Ro_PZ++) {//
						double start_work = omp_get_wtime();
						//omp_set_nested(1);

						BK *bk_Ro_stratum_buff = new BK[Ro_stratum_steps.size()];

#pragma omp parallel for schedule(dynamic,1)
						for (int n_Ro_stratum = 0; n_Ro_stratum < (int)Ro_stratum_steps.size(); n_Ro_stratum++) {//пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ																												 
							//пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ //
							vector<LayerDakhnov> model;//
							vector<double> layer_boundaries;//
							LayerDakhnov B_hole, Pen_zone, stratum;//
							//пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
							B_hole.r_boundary = R_BH_steps[n_R_BH];
							layer_boundaries.push_back(B_hole.r_boundary);
							B_hole.rho = Ro_BH_steps[n_Ro_BH];
							model.push_back(B_hole);
							//пїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ 
							//пїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ R_PZ_steps пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ: R_PZ / R_BH,
							//пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ, пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
							Pen_zone.r_boundary = R_BH_steps[n_R_BH] * R_PZ_steps[n_R_PZ];//
							layer_boundaries.push_back(Pen_zone.r_boundary);
							Pen_zone.rho = Ro_PZ_steps[n_Ro_PZ];
							model.push_back(Pen_zone);
							//пїЅпїЅпїЅпїЅпїЅ
							stratum.r_boundary = p.r_max;
							stratum.rho = Ro_stratum_steps[n_Ro_stratum];
							layer_boundaries.push_back(stratum.r_boundary);
							model.push_back(stratum);
							//пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ 
							// пїЅпїЅпїЅ пїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
							vector<double> r_coords;
							r_coords = buildRadialGridAdaptive(p.r_tool, layer_boundaries, p.dr_start, p.q_r);

							BK bk_buff;
							for (int i = 0; i < 4; i++) {
								vector<Electrode> A_group;
								vector<Electrode> B_group;
								vector<MeasElectrode> M_group;
								//пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ (пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ).
								set_work_electrodes(i, p, A_group, B_group, M_group);
								//
								vector<double> z_coords = buildSmartDepthGrid_new(A_group, B_group, M_group, p.dz_tool, p.dz_fine, p.threshold, p.q_z_tail, p.tail);
								ResistorGrid grid(r_coords, z_coords);
								//пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ
								grid.fillModel(model);

								grid.setSonde(A_group, B_group, M_group);
								grid.solveDirect(debug_mode);

								if (i == 0) {
									// пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ, пїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
									bk_buff.Ia[i] = grid.getElectrodeCurrent(A_group[1]);
									//пїЅ пїЅпїЅпїЅпїЅпїЅпїЅ SML, пїЅпїЅ пїЅ M_group пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ - UM пїЅ UN
								}

								else {
									//пїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ S, M пїЅ L пїЅ A_group пїЅпїЅпїЅпїЅпїЅпїЅ 1 пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ - 0пїЅпїЅпїЅпїЅпїЅпїЅ
									bk_buff.Ia[i] = grid.getElectrodeCurrent(A_group[0]);
								}
								//пїЅпїЅ пїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅ	B_group пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅ пїЅ0 пїЅпїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ
								bk_buff.Ib[i] = abs(grid.getElectrodeCurrent(B_group[0]));
								// пїЅпїЅ пїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ UM пїЅ UN пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ, пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅ пїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ.
								//пїЅпїЅ UM пїЅ UN пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
								//пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ, пїЅ пїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ
								bk_buff.dU[i] = grid.getMeasElectrodeVoltage(M_group[0]) - grid.getMeasElectrodeVoltage(M_group[1]);
							}

							bk_buff.R_BH = R_BH_steps[n_R_BH];
							bk_buff.R_PZ_R_BH = R_PZ_steps[n_R_PZ];
							bk_buff.Ro_PZ_Ro_BH = Ro_PZ_steps[n_Ro_PZ];
							bk_buff.Ro_stratum_Ro_BH = Ro_stratum_steps[n_Ro_stratum];

							bk_Ro_stratum_buff[n_Ro_stratum] = bk_buff;
						}

#pragma omp master
						{
							N_points += Ro_stratum_steps.size();
							//
							fout.open(pallete_file_mame, std::ios::binary | ios::app);
							fout.write((char*)bk_Ro_stratum_buff, sizeof(BK) * Ro_stratum_steps.size());
							fout.close();
							delete[] bk_Ro_stratum_buff;

							float percent = ((float)N_points / (float)pallete_size) * 100.0f;
							double end_work = omp_get_wtime();

							auto end = std::chrono::steady_clock::now();
							std::chrono::duration<double> elapsed_seconds = end - start;

							printf("time: %.2f sec %.2f percent %.2f sec total time \n", end_work - start_work, percent, elapsed_seconds.count());

							printf("D_BH: %.3f D_PZ: %.3f Ro_PZ: %.3f \n", 2.0*R_BH_steps[n_R_BH], (2.0*R_BH_steps[n_R_BH] * R_PZ_steps[n_R_PZ]), Ro_PZ_steps[n_Ro_PZ]);
						}
					}
				}
			}
		}
		status = COMPLETED;
		fstream file(pallete_file_mame, std::ios::in | std::ios::out | std::ios::binary);
		file.write((char*)&BK_header.status, sizeof(status));
		file.close();
		cout << "pallete file write complete sucsessful" << endl;
		auto end = std::chrono::steady_clock::now();
		auto diff = end - start; // 
		auto h = std::chrono::duration_cast<std::chrono::hours>(diff); diff -= h;
		auto m = std::chrono::duration_cast<std::chrono::minutes>(diff); diff -= m;
		auto s = std::chrono::duration_cast<std::chrono::seconds>(diff);
		std::cout << h.count() << " H " << m.count() << " min " << s.count() << " sec" << std::endl;
		this->Invoke(gcnew Action(this, &MyForm::EnableButtons));
		
	
}
*/

void Pallete_Calculation() {
	auto start = std::chrono::steady_clock::now();

	uint32_t status = NOT_COMPLETED;
	ofstream fout;
	SondeParams p = fill_params_from_GUI();

	// Подготовка заголовка файла палетки
	BK_PALLETE_FILE_HEADER BK_header;
	BK_header.status = NOT_COMPLETED;
	BK_header.params = p;

	// R_BH, R_PZ, Ro_PZ, RO_stratum (Запись осей сетки в заголовок)
	for (int n_R_BH = 0; n_R_BH < R_BH_steps.size(); n_R_BH++) {
		BK_header.BK_pallete_nodes.R_BH[n_R_BH] = R_BH_steps[n_R_BH];
	}
	BK_header.BK_pallete_nodes.points[0] = R_BH_steps.size();

	for (int n_R_PZ = 0; n_R_PZ < R_PZ_steps.size(); n_R_PZ++) {
		BK_header.BK_pallete_nodes.R_PZ_R_BH[n_R_PZ] = R_PZ_steps[n_R_PZ];
	}
	BK_header.BK_pallete_nodes.points[1] = R_PZ_steps.size();

	for (int n_Ro_PZ = 0; n_Ro_PZ < Ro_PZ_steps.size(); n_Ro_PZ++) {
		BK_header.BK_pallete_nodes.Ro_PZ_Ro_BH[n_Ro_PZ] = Ro_PZ_steps[n_Ro_PZ];
	}
	BK_header.BK_pallete_nodes.points[2] = Ro_PZ_steps.size();

	for (int n_Ro_stratum = 0; n_Ro_stratum < (int)Ro_stratum_steps.size(); n_Ro_stratum++) {
		BK_header.BK_pallete_nodes.Ro_stratum_Ro_BH[n_Ro_stratum] = Ro_stratum_steps[n_Ro_stratum];
	}
	BK_header.BK_pallete_nodes.points[3] = Ro_stratum_steps.size();

	fout.open(pallete_file_mame, std::ios::binary);
	fout.write((char*)&BK_header, sizeof(BK_PALLETE_FILE_HEADER));
	fout.close();

	omp_set_num_threads(14);
	std::cout << "OpenMP threads : " << omp_get_max_threads() << std::endl;

	uint32_t N_points = 0;
	// Внешние циклы: Ro_BH, R_BH, R_PZ, Ro_PZ
	for (int n_Ro_BH = 0; n_Ro_BH < Ro_BH_steps.size(); n_Ro_BH++) {

		for (int n_R_BH = 0; n_R_BH < R_BH_steps.size(); n_R_BH++) {

			for (int n_R_PZ = 0; n_R_PZ < R_PZ_steps.size(); n_R_PZ++) {

				for (int n_Ro_PZ = 0; n_Ro_PZ < Ro_PZ_steps.size(); n_Ro_PZ++) {
					double start_work = omp_get_wtime();

					BK *bk_Ro_stratum_buff = new BK[Ro_stratum_steps.size()];

#pragma omp parallel for schedule(dynamic,1)
					// Внутренний распараллеленный цикл по удельным сопротивлениям пласта
					for (int n_Ro_stratum = 0; n_Ro_stratum < (int)Ro_stratum_steps.size(); n_Ro_stratum++) {

						// Формирование геоэлектрической модели среды
						vector<LayerDakhnov> model;
						vector<double> layer_boundaries;
						LayerDakhnov B_hole, Pen_zone, stratum;

						// 1. Скважина (Borehole)
						B_hole.r_boundary = R_BH_steps[n_R_BH];
						layer_boundaries.push_back(B_hole.r_boundary);
						B_hole.rho = Ro_BH_steps[n_Ro_BH];
						model.push_back(B_hole);

						// 2. Зона проникновения (Penetration Zone)
						// В массиве R_PZ_steps хранится относительный радиус: R_PZ / R_BH,
						// для вычисления абсолютного физического радиуса умножаем на радиус скважины
						Pen_zone.r_boundary = R_BH_steps[n_R_BH] * R_PZ_steps[n_R_PZ];
						layer_boundaries.push_back(Pen_zone.r_boundary);
						Pen_zone.rho = Ro_PZ_steps[n_Ro_PZ];
						model.push_back(Pen_zone);

						// 3. Неизмененный пласт (Stratum)
						stratum.r_boundary = p.r_max;
						stratum.rho = Ro_stratum_steps[n_Ro_stratum];
						layer_boundaries.push_back(stratum.r_boundary);
						model.push_back(stratum);

						// Построение адаптивной радиальной сетки от прибора до дальней границы
						// таким образом, чтобы узлы сетки совпадали с границами слоев
						vector<double> r_coords;
						r_coords = buildRadialGridAdaptive(p.r_tool, layer_boundaries, p.dr_start, p.q_r);

						//заменяем на новые work_mode и структуру для сохранения данных
						
						BK bk_buff;
						for (int i = 0; i < 4; i++) {
							vector<Electrode> A_group;
							vector<Electrode> B_group;
							vector<MeasElectrode> M_group;

							// Настройка конфигурации электродов для текущего зонда (режима)
							set_work_electrodes(i, p, A_group, B_group, M_group);

							vector<double> z_coords = buildSmartDepthGrid_new(A_group, B_group, M_group, p.dz_tool, p.dz_fine, p.threshold, p.q_z_tail, p.tail);
							ResistorGrid grid(r_coords, z_coords);

							// Заполнение конечно-разностной сетки параметрами среды
							grid.fillModel(model);

							grid.setSonde(A_group, B_group, M_group);
							grid.solveDirect(debug_mode);

							if (i == 0) {
								// Для первого зонда ток измеряется на втором электроде (индекс 1) группы A
								bk_buff.Ia[i] = grid.getElectrodeCurrent(A_group[1]);
								// В конфигурации SML, в M_group находятся два электрода: UM и UN
							}
							else {
								// Для остальных зондов (S, M, L) в A_group один электрод - берем с нулевого индекса
								bk_buff.Ia[i] = grid.getElectrodeCurrent(A_group[0]);
							}

							// Снятие токов фокусировки с B_group (модуль значения тока возврата)
							bk_buff.Ib[i] = abs(grid.getElectrodeCurrent(B_group[0]));

							// Расчет разности потенциалов (dU) между измерительными электродами UM и UN
							// Электроды физически разнесены, вычисляем падение напряжения
							bk_buff.dU[i] = grid.getMeasElectrodeVoltage(M_group[0]) - grid.getMeasElectrodeVoltage(M_group[1]);
						}

						bk_buff.R_BH = R_BH_steps[n_R_BH];
						bk_buff.R_PZ_R_BH = R_PZ_steps[n_R_PZ];
						bk_buff.Ro_PZ_Ro_BH = Ro_PZ_steps[n_Ro_PZ];
						bk_buff.Ro_stratum_Ro_BH = Ro_stratum_steps[n_Ro_stratum];

						bk_Ro_stratum_buff[n_Ro_stratum] = bk_buff;
					} // Конец цикла omp parallel for

#pragma omp master
					{
						// Сохранение рассчитанного блока данных в бинарный файл
						N_points += Ro_stratum_steps.size();

						fout.open(pallete_file_mame, std::ios::binary | ios::app);
						fout.write((char*)bk_Ro_stratum_buff, sizeof(BK) * Ro_stratum_steps.size());
						fout.close();
						delete[] bk_Ro_stratum_buff;

						float percent = ((float)N_points / (float)pallete_size) * 100.0f;
						double end_work = omp_get_wtime();

						auto end = std::chrono::steady_clock::now();
						std::chrono::duration<double> elapsed_seconds = end - start;

						printf("time: %.2f sec %.2f percent %.2f sec total time \n", end_work - start_work, percent, elapsed_seconds.count());
						printf("D_BH: %.3f D_PZ: %.3f Ro_PZ: %.3f \n", 2.0*R_BH_steps[n_R_BH], (2.0*R_BH_steps[n_R_BH] * R_PZ_steps[n_R_PZ]), Ro_PZ_steps[n_Ro_PZ]);
					}
				}
			}
		}
	}

	// Обновление статуса файла как завершенного
	status = COMPLETED;
	fstream file(pallete_file_mame, std::ios::in | std::ios::out | std::ios::binary);
	file.write((char*)&BK_header.status, sizeof(status));
	file.close();

	cout << "pallete file write complete sucsessful" << endl;

	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	auto h = std::chrono::duration_cast<std::chrono::hours>(diff); diff -= h;
	auto m = std::chrono::duration_cast<std::chrono::minutes>(diff); diff -= m;
	auto s = std::chrono::duration_cast<std::chrono::seconds>(diff);

	std::cout << h.count() << " H " << m.count() << " min " << s.count() << " sec" << std::endl;
	this->Invoke(gcnew Action(this, &MyForm::EnableButtons));
}

private: System::Void button_calk_pallete_Click(System::Object^  sender, System::EventArgs^  e) {
	if (saveFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
		pallete_file_mame = (char*)Marshal::StringToHGlobalAnsi(saveFileDialog1->FileName).ToPointer();
	    button_calk_pallete->Enabled = false;
		button_set->Enabled = false;
	    Thread^ navThread = gcnew Thread(gcnew ThreadStart(this, &MyForm::Pallete_Calculation));
	    navThread->Start();
	}
}

void EnableButtons() { button_calk_pallete->Enabled = true; button_set->Enabled = true;}

private: System::Void Open_Pallete_file_ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
	std::ifstream fin;
	Ro_PZ_steps.clear();
	Ro_stratum_steps.clear();
	R_BH_steps.clear();
	R_PZ_steps.clear();

	const char* Data_file_name;
	if (openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
		Data_file_name = (char*)Marshal::StringToHGlobalAnsi(openFileDialog1->FileName).ToPointer();

		BK_PALLETE_FILE_HEADER BK_header;
		fin.open(Data_file_name, std::ios::binary);
		fin.read((char*)&BK_header, sizeof(BK_PALLETE_FILE_HEADER));
		cout << "status " << BK_header.status << endl;
		//fin.seekg(0, ios::beg);

		//R_BH
		for (int i = 0; i < BK_header.BK_pallete_nodes.points[0]; i++) 
			BK_pallete.R_BH_nodes.push_back(BK_header.BK_pallete_nodes.R_BH[i]);
		//R_PZ
		for (int i = 0; i < BK_header.BK_pallete_nodes.points[1]; i++) 
			BK_pallete.R_PZ_nodes.push_back(BK_header.BK_pallete_nodes.R_PZ_R_BH[i]);
		//Ro_PZ
		for (int i = 0; i < BK_header.BK_pallete_nodes.points[2]; i++) 
			BK_pallete.Ro_PZ_nodes.push_back(BK_header.BK_pallete_nodes.Ro_PZ_Ro_BH[i]);
		//RO_stratum
		for (int i = 0; i < BK_header.BK_pallete_nodes.points[3]; i++) 
			BK_pallete.Ro_stratum_nodes.push_back(BK_header.BK_pallete_nodes.Ro_stratum_Ro_BH[i]);
		
		BK_pallete.init(BK_pallete.R_BH_nodes.size(), BK_pallete.R_PZ_nodes.size(), BK_pallete.Ro_PZ_nodes.size(), BK_pallete.Ro_stratum_nodes.size());

		cout << " D_BH_mm_nodes " << BK_pallete.n_R_BH << " :";
		for (int i = 0; i < BK_pallete.n_R_BH; i++) {
			cout << BK_pallete.R_BH_nodes[i] * 2000 << " ";
		}
		cout << endl; cout << endl;

		cout << "  D_PZ/D_BH_nodes " << BK_pallete.n_R_PZ << " :";
		for (int i = 0; i < BK_pallete.n_R_PZ; i++) {
			cout << BK_pallete.R_PZ_nodes[i] << " ";
		}
		cout << endl; cout << endl;

		cout << " Ro_PZ/Ro_BH_nodes " << BK_pallete.n_Ro_PZ << " :";
		for (int i = 0; i < BK_pallete.n_Ro_PZ; i++) {
			cout << BK_pallete.Ro_PZ_nodes[i] << " ";
		}
		cout << endl; cout << endl;

		cout << " Ro_stratum/Ro_BH_nodes " << BK_pallete.n_Ro_stratum << " :";
		for (int i = 0; i < BK_pallete.n_Ro_stratum; i++) {
			cout << BK_pallete.Ro_stratum_nodes[i] << " ";
		}
		cout << endl; cout << endl;

		cout << "total pallete nodes " << BK_pallete.n_R_BH*BK_pallete.n_R_PZ*BK_pallete.n_Ro_PZ*BK_pallete.n_Ro_stratum << endl;

		for (int n_R_BH = 0; n_R_BH < BK_pallete.n_R_BH; n_R_BH++) {//
			for (int n_R_PZ = 0; n_R_PZ < BK_pallete.n_R_PZ; n_R_PZ++) {//
				for (int n_Ro_PZ = 0; n_Ro_PZ < BK_pallete.n_Ro_PZ; n_Ro_PZ++) {//
					for (int n_Ro_stratum = 0; n_Ro_stratum < BK_pallete.n_Ro_stratum; n_Ro_stratum++) {//
						BK buff;
						fin.read((char*)&buff, sizeof(BK));
						BK_pallete.data[n_R_BH][n_R_PZ][n_Ro_PZ][n_Ro_stratum] = buff;
					}
				}
			}
		}
		fin.close();

		///////////////////////////////////////////////////////////////////////////
		visual_mode = SHOW_MODE;
		button_calc_show->Text = "SWITSH TO CALK_MODE";
		tableLayoutPanel_chart->Visible = true;

		tableLayoutPanel_tool_param->Visible = false;
		tableLayoutPanel_envir_param->Visible = false;
		tableLayoutPanel_grid_param->Visible = false;
		panel_pallete_param->Visible = false;
		pictureBox_U->Visible = false;
		///////////////////////////////////////////////////////////////////////////////
		comboBox_D_BH->BeginUpdate();
		comboBox_D_BH->Items->Clear();
		for (double D_BH_mm : BK_pallete.R_BH_nodes) {
			String^ formattedStr = (2000.0*D_BH_mm).ToString("F0");
			comboBox_D_BH->Items->Add(formattedStr);
		}
		comboBox_D_BH->EndUpdate();

		comboBox_D_PZ->BeginUpdate();
		comboBox_D_PZ->Items->Clear();
		for (double R_PZ_D_BH : BK_pallete.R_PZ_nodes) {
			String^ formattedStr = (R_PZ_D_BH).ToString("F3");
			comboBox_D_PZ->Items->Add(formattedStr);
		}
		comboBox_D_PZ->EndUpdate();

		comboBox_Ro_PZ->BeginUpdate();
		comboBox_Ro_PZ->Items->Clear();
		for (double Ro_PZ : BK_pallete.Ro_PZ_nodes) {
			String^ formattedStr = (Ro_PZ).ToString("F3");
			comboBox_Ro_PZ->Items->Add(formattedStr);
		}
		comboBox_Ro_PZ->EndUpdate();

	}

}

////////////
private: System::Void button_inversion_Click(System::Object^  sender, System::EventArgs^  e) {
	vector <double> Ro_stratum_nodes;
	
	bool sucsess = true;
	try {
		std::stringstream ss(msclr::interop::marshal_as<std::string>(textBox_Ro_stratum->Text->Replace(",", ".")));
		Ro_stratum_nodes.clear();
		double temp;
		double start = 0.0; double Q = 0.0; double end = 0.0;
		ss >> start; ss >> Q; ss >> end;
		//cout << start << " " << Q << " " << end << endl;
		for (double n = start; n < end; n *= Q) {
			//cout << n << endl;
			Ro_stratum_nodes.push_back(n);
		}
	}
	catch (...) { 
		sucsess = false; 
	}

	//пїЅпїЅпїЅпїЅпїЅпїЅ пїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ Ro_stratum
	for (int n_Ro_str = 0; n_Ro_str < Ro_stratum_nodes.size(); n_Ro_str++) {
		
		SondeParams p = fill_params_from_GUI();
		vector<LayerDakhnov> model;
		LayerDakhnov B_hole, Pen_zone, stratum;
		//пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ //
		std::vector<double> layer_boundaries;//пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ пїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ
		//пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
		B_hole.r_boundary = GetDouble(textBox_D_BH_mm, 147) / 2000.0; //пїЅпїЅпїЅпїЅпїЅпїЅ пїЅ пїЅпїЅ
		layer_boundaries.push_back(B_hole.r_boundary);
		B_hole.rho = GetDouble(textBox_Ro_BH, 1.0);
		model.push_back(B_hole);
		//пїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ 
		//пїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ textBox_R_PZ_R_BH пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ: R_PZ / R_BH,
		//пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ, пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅпїЅ  пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ B_hole.r_boundary
		Pen_zone.r_boundary = B_hole.r_boundary * GetDouble(textBox_R_PZ_R_BH, 1);
		layer_boundaries.push_back(Pen_zone.r_boundary);
		Pen_zone.rho = GetDouble(textBox_Ro_PZ, 1.0);
		model.push_back(Pen_zone);
		stratum.r_boundary = p.r_max;

		stratum.rho = Ro_stratum_nodes[n_Ro_str];
		layer_boundaries.push_back(stratum.r_boundary);
		model.push_back(stratum);
	

		//пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ 
	// пїЅпїЅпїЅ пїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
		vector<double> r_coords = buildRadialGridAdaptive(p.r_tool, layer_boundaries, p.dr_start, p.q_r);

		BK bk_buff;
		for (int i = 0; i < 4; i++) {
			vector<Electrode> A_group;
			vector<Electrode> B_group;
			vector<MeasElectrode> M_group;
			//пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ (пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ).
			set_work_electrodes(i, p, A_group, B_group, M_group);
			//пїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
			vector<double> z_coords = buildSmartDepthGrid_new(A_group, B_group, M_group, p.dz_tool, p.dz_fine, p.threshold, p.q_z_tail, p.tail);
			ResistorGrid grid(r_coords, z_coords);
			//пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ
			grid.fillModel(model);
			grid.setSonde(A_group, B_group, M_group);
			grid.solveDirect(debug_mode);
			if (i == 0) {
				// пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ, пїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
				bk_buff.Ia[i] = grid.getElectrodeCurrent(A_group[1]);
				//пїЅ пїЅпїЅпїЅпїЅпїЅпїЅ SML, пїЅпїЅ пїЅ M_group пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ - UM пїЅ UN

			}

			else {
				//пїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ S, M пїЅ L пїЅ A_group пїЅпїЅпїЅпїЅпїЅпїЅ 1 пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ - 0пїЅпїЅпїЅпїЅпїЅпїЅ
				bk_buff.Ia[i] = grid.getElectrodeCurrent(A_group[0]);

			}
			//пїЅпїЅ пїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅ	B_group пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅ пїЅ0 пїЅпїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ
			bk_buff.Ib[i] = abs(grid.getElectrodeCurrent(B_group[0]));
			// пїЅпїЅ пїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ UM пїЅ UN пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ, пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅ пїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ.
			//пїЅпїЅ UM пїЅ UN пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
			//пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ, пїЅ пїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ
			bk_buff.dU[i] = grid.getMeasElectrodeVoltage(M_group[0]) - grid.getMeasElectrodeVoltage(M_group[1]);
		}

		bk_buff.R_BH = B_hole.r_boundary;
		bk_buff.R_PZ_R_BH = Pen_zone.r_boundary;
		bk_buff.Ro_PZ_Ro_BH = Pen_zone.rho;
		bk_buff.Ro_stratum_Ro_BH = stratum.rho;

		for_inversion.push_back(bk_buff);

		cout << " R_BH = " << B_hole.rho << "; D_BH mm = " << bk_buff.R_BH*2000.0 << "; D_PZ mm = " << bk_buff.R_PZ_R_BH * 2000 << "; Ro_PZ  = " << bk_buff.Ro_PZ_Ro_BH << "; Ro_stratum  = " << bk_buff.Ro_stratum_Ro_BH << endl;

		cout << "One point data" << endl;
		cout << "Ia SML, S, M, L, ";
		for (int i = 0; i < 4; i++)
			cout << bk_buff.Ia[i] << " ";
		cout << endl;

		cout << "Ib SML, S, M, L, ";
		for (int i = 0; i < 4; i++)
			cout << bk_buff.Ib[i] << " ";
		cout << endl;

		cout << "dU SML, S, M, L, ";
		for (int i = 0; i < 4; i++)
			cout << bk_buff.dU[i] << " ";
		cout << endl;

	}
	
	
}


//grafics//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Bitmap^ mainImage;      // Само изображение
float zoom = 1.0f;      // Текущий масштаб
PointF offset;          // Смещение (для перемещения)
Point lastMousePos;     // Предыдущая позиция мыши

void OnMouseWheel(Object^ sender, MouseEventArgs^ e) {
	float oldZoom = zoom;
	float zoomFactor = 1.1f;
	if (e->Delta > 0) zoom *= zoomFactor;
	else zoom /= zoomFactor;
	
	if (zoom < 0.1f) zoom = 0.1f;
	if (zoom > 20.0f) zoom = 20.0f;
	float ratio = zoom / oldZoom;
	offset.X = e->X - (e->X - offset.X) * ratio;
	offset.Y = e->Y - (e->Y - offset.Y) * ratio;
	ConstrainOffset();
	pictureBox_U->Invalidate(); 
}

private: System::Void pictureBox_U_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
	if (e->Button == System::Windows::Forms::MouseButtons::Left) {
		lastMousePos = e->Location;
	}
}

private: System::Void pictureBox_U_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
	if (e->Button == System::Windows::Forms::MouseButtons::Left) {
		offset.X += (e->X - lastMousePos.X);
		offset.Y += (e->Y - lastMousePos.Y);
		lastMousePos = e->Location;
		ConstrainOffset(); 
		pictureBox_U->Invalidate();
	}
}
	
private: System::Void pictureBox_U_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) {
	if (mainImage == nullptr) return;
	e->Graphics->InterpolationMode = Drawing2D::InterpolationMode::HighQualityBicubic;
	e->Graphics->PixelOffsetMode = Drawing2D::PixelOffsetMode::HighQuality;
	Graphics^ g = e->Graphics;
	g->InterpolationMode = System::Drawing::Drawing2D::InterpolationMode::HighQualityBicubic;
    g->TranslateTransform(offset.X, offset.Y);
	g->ScaleTransform(zoom, zoom);
    g->DrawImage(mainImage, 0, 0);
}

void ConstrainOffset() {
			 if (mainImage == nullptr) return;

			 float currentWidth = mainImage->Width * zoom;
			 float currentHeight = mainImage->Height * zoom;

			 if (currentWidth > pictureBox_U->Width) {
				 if (offset.X > 0) offset.X = 0;
				 if (offset.X < pictureBox_U->Width - currentWidth) offset.X = pictureBox_U->Width - currentWidth;
			 }
			 else {
				 if (offset.X < 0) offset.X = 0;
				 if (offset.X > pictureBox_U->Width - currentWidth) offset.X = pictureBox_U->Width - currentWidth;
			 }

			 if (currentHeight > pictureBox_U->Height) {
				 if (offset.Y > 0) offset.Y = 0;
				 if (offset.Y < pictureBox_U->Height - currentHeight) offset.Y = pictureBox_U->Height - currentHeight;
			 }
			 else {
				 if (offset.Y < 0) offset.Y = 0;
				 if (offset.Y > pictureBox_U->Height - currentHeight) offset.Y = pictureBox_U->Height - currentHeight;
			 }
		 }

private: System::Void pictureBox_U_MouseEnter(System::Object^  sender, System::EventArgs^  e) {
	pictureBox_U->Focus();
}

private: System::Void cursor_position(System::Object^  sender, System::Windows::Forms::DataVisualization::Charting::CursorEventArgs^  e) {
	label_X->Text = "X: " + (pow(10, chart_->ChartAreas["ChartArea1"]->CursorX->Position)).ToString("G4");
	label_Y->Text = "Y: " + (pow(10, chart_->ChartAreas["ChartArea1"]->CursorY->Position)).ToString("G4");
}

private: System::Void button_calc_show_Click(System::Object^  sender, System::EventArgs^  e) {
	if (visual_mode == CALK_MODE) { 
		visual_mode = SHOW_MODE; 
		button_calc_show->Text = "SWITSH TO INVERSION_MODE" ;
		this->Text = "SHOW_MODE";
		tableLayoutPanel_chart->Visible = true;
		tableLayoutPanel_tool_param->Visible = false;
		tableLayoutPanel_envir_param->Visible = false;
		tableLayoutPanel_grid_param->Visible = false;
		panel_pallete_param->Visible = false;
		pictureBox_U->Visible = false;
		//chart_iversion->Visible = false;
		button_inversion->Visible = false;
	}

	else if (visual_mode == SHOW_MODE) {
		visual_mode = INVERSION_MODE;
		button_calc_show->Text = "SWITSH TO CALK_MODE";
		this->Text = "INVERSION_MODE";
		tableLayoutPanel_chart->Visible = false;
		tableLayoutPanel_tool_param->Visible = false;
		tableLayoutPanel_envir_param->Visible = true;
		this->tableLayoutPanel_envir_param->Location = System::Drawing::Point(10, 40);
		tableLayoutPanel_grid_param->Visible = false;
		panel_pallete_param->Visible = false;
		pictureBox_U->Visible = false;
		//chart_iversion->BringToFront();
		//chart_iversion->Visible = true;
		
		button_inversion->Visible = true;
	}

	else if (visual_mode == INVERSION_MODE) {
		visual_mode = CALK_MODE;
		button_calc_show->Text = "SWITSH TO SHOW_MODE";
		this->Text = "CALK_MODE";
		tableLayoutPanel_chart->Visible = false;
		tableLayoutPanel_tool_param->Visible = true;
		tableLayoutPanel_envir_param->Visible = true;
		this->tableLayoutPanel_envir_param->Location = System::Drawing::Point(326, 326);
		tableLayoutPanel_grid_param->Visible = true;
		panel_pallete_param->Visible = true;
		pictureBox_U->Visible = true;
		//chart_iversion->Visible = false;
		button_inversion->Visible = false;
	}


}

private: System::Void comboBox_D_BH_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
	if (first == false)
		do_chart();
}
private: System::Void comboBox_D_PZ_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
	if(first == false)
	    do_chart();
}
private: System::Void comboBox_Ro_PZ_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
	if (first == false)
		do_chart();
}
private: System::Void comboBox_shart_curves_Y_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
	if (first == false)
		do_chart();
}


	int sell_count = 0;
private: System::Void button_add_work_mode_Click(System::Object^  sender, System::EventArgs^  e) {
	Form^ prompt = gcnew Form();
	prompt->Width = 300; prompt->Height = 160;
	prompt->Text = "new work mode";
	prompt->StartPosition = FormStartPosition::CenterParent;
	prompt->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;

	Label^ lbl = gcnew Label();
	lbl->Text = "set work mode name:";
	lbl->SetBounds(10, 10, 200, 20);

	TextBox^ txtInput = gcnew TextBox();
	txtInput->SetBounds(10, 40, 260, 20);

	Button^ btnOk = gcnew Button();
	btnOk->Text = "ADD";
	btnOk->SetBounds(185, 80, 85, 30);
	btnOk->DialogResult = System::Windows::Forms::DialogResult::OK;

	prompt->Controls->Add(lbl);
	prompt->Controls->Add(txtInput);
	prompt->Controls->Add(btnOk);
	prompt->AcceptButton = btnOk;

	if (prompt->ShowDialog() == System::Windows::Forms::DialogResult::OK && !String::IsNullOrWhiteSpace(txtInput->Text)) {

		DataGridViewComboBoxColumn^ comboCol = gcnew DataGridViewComboBoxColumn();
		comboCol->HeaderText = txtInput->Text;// Заголовок в таблице
		comboCol->Name = "mode" + sell_count;      // Имя для обращения в коде
		comboCol->Items->Add("CURR");
		comboCol->Items->Add("DRND");
		comboCol->Items->Add("MEAS");
		comboCol->DefaultCellStyle->NullValue = "CURR";

		dataGridView_electrodes->Columns->Add(comboCol);
		//Если строки уже были, можно массово установить им начальное значение
		for (int i = 0; i < dataGridView_electrodes->Rows->Count; i++) {
			if (!dataGridView_electrodes->Rows[i]->IsNewRow) {
				dataGridView_electrodes->Rows[i]->Cells["mode" + sell_count]->Value = "CURR";
			}
		}
	}
	ResizeGrid(dataGridView_electrodes);
	sell_count++;
}

int totalModes = 0;
// записываем в вектор параметров электродов и преобразуем к виду трех групп электродов
private: System::Void button_set_electrodes_Click(System::Object^  sender, System::EventArgs^  e) {
	electrodeList.clear();
	int colCount = dataGridView_electrodes->Columns->Count;
	 totalModes = colCount - 3;
	if (totalModes <= 0) return;

	String^ errorRows = "";
	electrodeList.resize(totalModes);

	for (int i = 0; i < dataGridView_electrodes->Rows->Count; i++) {
		DataGridViewRow^ row = dataGridView_electrodes->Rows[i];
		if (row->IsNewRow) continue;
		for (int j = 3; j < colCount; j++) {
			ELECTRODE el;
			msclr::interop::marshal_context context;
			// 1. Получаем заголовок колонки (имя режима) и сохраняем в work_mode
			String^ headerName = dataGridView_electrodes->Columns[j]->HeaderText;
			el.work_mode = context.marshal_as<std::string>(headerName);
			// 2. Читаем остальные поля
			if (row->Cells["z_start"]->Value != nullptr)
				Double::TryParse(row->Cells["z_start"]->Value->ToString(), el.z_start_mm);
			if (row->Cells["z_end"]->Value != nullptr)
				Double::TryParse(row->Cells["z_end"]->Value->ToString(), el.z_end_mm);
			///////////////////////////////////////////////////////
			if (el.z_end_mm <= el.z_start_mm) {
				if (!errorRows->Contains((i + 1).ToString())) errorRows += (i + 1).ToString() + " ";
			}
			//////////////////////////////////////////////////////////
			auto cell = (DataGridViewComboBoxCell^)row->Cells[j];
			if (cell->Value != nullptr) {
				int idx = cell->Items->IndexOf(cell->Value);
				// mode: 0-CURR, 1-GND, 2-MEAS
				el.mode = (idx >= 0) ? idx : 1;
			}
			else { el.mode = 1; }

			if (row->Cells["name"]->Value != nullptr) {
				el.name = context.marshal_as<std::string>(row->Cells["name"]->Value->ToString());
			}

			electrodeList[j - 3].push_back(el);
		}
	}

	if (errorRows != "") {
		MessageBox::Show("Ошибка в строках: " + errorRows, "Ошибка валидации");
		return;
	}
	
	//////////////////////////////////
	comboBox_work_mode->Items->Clear();
	int N_work_modes = (int)electrodeList.size();
	cout << "N_work_modes " << N_work_modes << endl;
	for (int wm = 0; wm < N_work_modes; wm++) {
		// Проверяем, что вектор сценария не пустой, чтобы не обратиться к несуществующему элементу [0]
		if (!electrodeList[wm].empty()) {
			std::string stdName = electrodeList[wm][0].work_mode;
			String^ sysName = gcnew String(stdName.c_str());
			comboBox_work_mode->Items->Add(sysName);
			cout << "work_modes_added: " << stdName << endl;
		}
	}
	if (comboBox_work_mode->Items->Count > 0) {
		comboBox_work_mode->SelectedIndex = 0;
	}
	///////////////////////////////////////////////////
	vector<Electrode> A_group;
	vector<Electrode> B_group;
	vector<MeasElectrode> M_group;
	Params p = fill_params();
	int wm = comboBox_work_mode->SelectedIndex;
	if (wm >= 0 && wm < (int)electrodeList.size())
		set_work_electrodes_new(wm, electrodeList, p, A_group, B_group, M_group);

	ResizeGrid(dataGridView_grid_param);
	
}

private: System::Void savePresetToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
	// Сохранение в файл
	SaveFileDialog^ saveFileDlg = gcnew SaveFileDialog();
	saveFileDlg->Filter = "Preset files (*.txt)|*.txt";
	if (saveFileDlg->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
		msclr::interop::marshal_context context;
		std::ofstream outFile(context.marshal_as<std::string>(saveFileDlg->FileName));
		if (outFile.is_open()) {
			outFile << totalModes << "\n";
			for (int j = 0; j < totalModes; j++) {
				// Пишем название режима один раз для сценария
				outFile << electrodeList[j][0].work_mode << " " << electrodeList[j].size() << "\n";
				for (const auto& el : electrodeList[j]) {
					outFile << el.name << " " << el.z_start_mm << " " << el.z_end_mm << " " << el.mode << "\n";
				}
			}
			outFile.close();
		}
	}
}

private: System::Void Editor_KeyDown(System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e) {
	if (e->KeyCode == Keys::Delete) {
		if (dataGridView_electrodes->SelectedRows->Count > 0) {
			dataGridView_electrodes->EndEdit(); // Сначала завершаем редактирование
			for each (DataGridViewRow^ row in dataGridView_electrodes->SelectedRows) {
				if (!row->IsNewRow) dataGridView_electrodes->Rows->Remove(row);
			}
			ResizeGrid(dataGridView_electrodes);
			e->Handled = true;
		}
	}
}

private: System::Void dataGridView_electrodes_EditingControlShowing(System::Object^ sender, System::Windows::Forms::DataGridViewEditingControlShowingEventArgs^ e) {
	TextBox^ tb = dynamic_cast<TextBox^>(e->Control);
	if (tb != nullptr) {
		tb->KeyPress -= gcnew KeyPressEventHandler(this, &MyForm::DoubleKeyPress);
		String^ colName = dataGridView_electrodes->CurrentCell->OwningColumn->Name;
		if (colName == "z_start" || colName == "z_end") {
			tb->KeyPress += gcnew KeyPressEventHandler(this, &MyForm::DoubleKeyPress);
		}
		tb->KeyDown -= gcnew KeyEventHandler(this, &MyForm::Editor_KeyDown);
		tb->KeyDown += gcnew KeyEventHandler(this, &MyForm::Editor_KeyDown);
	}
	//ComboBox^ combo = dynamic_cast<ComboBox^>(e->Control);
	// (combo != nullptr) {
	//	combo->DroppedDown = true;
	//}
}

private: System::Void dataGridView_electrodes_RowsAdded(System::Object^  sender, System::Windows::Forms::DataGridViewRowsAddedEventArgs^  e) {
	ResizeGrid(dataGridView_electrodes);
}

private: System::Void button_load_preset_Click(System::Object^  sender, System::EventArgs^  e) {
	OpenFileDialog^ openFileDlg = gcnew OpenFileDialog();
	openFileDlg->Filter = "Preset files (*.txt)|*.txt";

	if (openFileDlg->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
		msclr::interop::marshal_context context;
		std::ifstream inFile(context.marshal_as<std::string>(openFileDlg->FileName));
		if (!inFile.is_open()) return;

		dataGridView_electrodes->Rows->Clear();
		while (dataGridView_electrodes->Columns->Count > 3)
			dataGridView_electrodes->Columns->RemoveAt(3);

		int totalScenarios;
		inFile >> totalScenarios;

		for (int j = 0; j < totalScenarios; j++) {
			std::string workModeName; // Это и есть наш HeaderName из файла
			int count;
			inFile >> workModeName >> count;

			DataGridViewComboBoxColumn^ col = gcnew DataGridViewComboBoxColumn();
			col->HeaderText = gcnew String(workModeName.c_str());
			col->Items->Add("CURR");
			col->Items->Add("GND");
			col->Items->Add("MEAS"); 
			dataGridView_electrodes->Columns->Add(col);

			for (int i = 0; i < count; i++) {
				std::string name;
				double z_s, z_e;
				int mIdx;
				inFile >> name >> z_s >> z_e >> mIdx;

				if (j == 0) {
					int r = dataGridView_electrodes->Rows->Add();
					dataGridView_electrodes->Rows[r]->Cells["name"]->Value = gcnew String(name.c_str());
					dataGridView_electrodes->Rows[r]->Cells["z_start"]->Value = z_s;
					dataGridView_electrodes->Rows[r]->Cells["z_end"]->Value = z_e;
				}
				String^ mStr = (mIdx == 0) ? "CURR" : (mIdx == 2) ? "MEAS" : "GND";
				dataGridView_electrodes->Rows[i]->Cells[3 + j]->Value = mStr;
			}
		}
		inFile.close();
	}
	ResizeGrid(dataGridView_electrodes);
	button_set_electrodes->PerformClick();
}

void MyForm::OnDeleteWorkModeClick(System::Object^ sender, System::EventArgs^ ev) {
    Button^ btn = dynamic_cast<Button^>(sender);
    if (btn == nullptr) return;
    // Извлекаем индекс колонки
    int colIdx = safe_cast<int>(btn->Tag);
    // Находим форму (Parent кнопки — это сама форма prompt)
    Form^ prompt = dynamic_cast<Form^>(btn->Parent);
    if (colIdx >= 0 && colIdx < dataGridView_electrodes->Columns->Count) {
        String^ colName = dataGridView_electrodes->Columns[colIdx]->HeaderText;
        if (MessageBox::Show("Удалить режим '" + colName + "'?", "Подтверждение", 
            MessageBoxButtons::YesNo, MessageBoxIcon::Question) == System::Windows::Forms::DialogResult::Yes) {
            dataGridView_electrodes->Columns->RemoveAt(colIdx);    
            if (prompt != nullptr) {
                prompt->DialogResult = System::Windows::Forms::DialogResult::Abort;
                prompt->Close();
            }
        }
    }
	ResizeGrid(dataGridView_electrodes);
}

private: System::Void dataGridView_electrodes_ColumnHeaderMouseDoubleClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellMouseEventArgs^  e) {
	// Работаем только с колонками режимов (начиная с индекса 3)
	if (e->ColumnIndex >= 3) {
		DataGridViewColumn^ targetCol = dataGridView_electrodes->Columns[e->ColumnIndex];

		Form^ prompt = gcnew Form();
		prompt->Width = 350; prompt->Height = 160;
		prompt->Text = "Редактирование режима";
		prompt->StartPosition = FormStartPosition::CenterParent;
		prompt->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;

		Label^ lbl = gcnew Label();
		lbl->Text = "Имя режима:";
		lbl->SetBounds(10, 10, 200, 20);

		TextBox^ txtInput = gcnew TextBox();
		txtInput->SetBounds(10, 35, 310, 20);
		txtInput->Text = targetCol->HeaderText;

		Button^ btnOk = gcnew Button();
		btnOk->Text = "СОХРАНИТЬ";
		btnOk->SetBounds(10, 75, 100, 30);
		btnOk->DialogResult = System::Windows::Forms::DialogResult::OK;

		// КНОПКА УДАЛЕНИЯ
		Button^ btnDel = gcnew Button();
		btnDel->Text = "УДАЛИТЬ";
		btnDel->SetBounds(220, 75, 100, 30);
		btnDel->BackColor = System::Drawing::Color::LightCoral;

		// Прячем индекс текущей колонки в Tag, чтобы вытащить его в другом методе
		btnDel->Tag = e->ColumnIndex;
		// Подписываемся на именованный метод
		btnDel->Click += gcnew EventHandler(this, &MyForm::OnDeleteWorkModeClick);

		prompt->Controls->Add(lbl);
		prompt->Controls->Add(txtInput);
		prompt->Controls->Add(btnOk);
		prompt->Controls->Add(btnDel);
		prompt->AcceptButton = btnOk;

		System::Windows::Forms::DialogResult res = prompt->ShowDialog();

		// Если нажали "СОХРАНИТЬ" (и колонка не была удалена)
		if (res == System::Windows::Forms::DialogResult::OK && !String::IsNullOrWhiteSpace(txtInput->Text)) {
			targetCol->HeaderText = txtInput->Text->Replace(" ", "_");
		}

		delete prompt; // Освобождаем ресурсы формы
	}
}
private: System::Void dataGridView_grid_param_EditingControlShowing(System::Object^  sender, System::Windows::Forms::DataGridViewEditingControlShowingEventArgs^  e) {
	TextBox^ tb = dynamic_cast<TextBox^>(e->Control);
	if (tb != nullptr) {
		// Сначала отписываемся от старых событий (чтобы не копились)
		tb->KeyPress -= gcnew KeyPressEventHandler(this, &MyForm::Cells_KeyPress);
		// Подписываемся на наш фильтр
		tb->KeyPress += gcnew KeyPressEventHandler(this, &MyForm::Cells_KeyPress);
	}
}
private: System::Void MyForm_FormClosing(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e) {
	try {
		// Указываем фиксированное имя файла для конфига
		std::ofstream outFile("config_params.txt");
		if (outFile.is_open()) {
			// Получаем актуальные параметры из таблицы
			Params p = fill_params();
			// Записываем значения через пробел
			outFile << p.dr_start * 1000.0 << " "
				<< p.q_r << " "
				<< p.r_max << " "
				<< p.dz_tool * 1000.0 << " "
				<< p.q_z_tail << " "
				<< p.dz_fine * 1000.0 << " "
				<< p.threshold * 1000.0 << " "
				<< p.r_tool * 2000.0 << " "
				<< p.tail << "\n";
			outFile.close();
		}
	}
	catch (...) {
		// Тихая ошибка при закрытии
	}
}
private: System::Void MyForm_Load(System::Object^ sender, System::EventArgs^ e) {
	// 1. Сначала инициализируем колонки и пустую строку (ваш код AddCol)
	// ... (код создания dataGridView_params) ...

	// 2. Пробуем прочитать сохраненные данные
	std::ifstream inFile("config_params.txt");
	if (inFile.is_open()) {
		double dr_s, q_r, r_m, dz_t, q_z_t, dz_f, thr, d_s, tail;

		if (inFile >> dr_s >> q_r >> r_m >> dz_t >> q_z_t >> dz_f >> thr >> d_s >> tail) {
			DataGridViewRow^ row = dataGridView_grid_param->Rows[0];
			row->Cells["dr_start_mm"]->Value = dr_s;
			row->Cells["q_r"]->Value = q_r;
			row->Cells["r_max_m"]->Value = r_m;
			row->Cells["dz_base_mm"]->Value = dz_t;
			row->Cells["q_z_tail"]->Value = q_z_t;
			row->Cells["dz_fine_mm"]->Value = dz_f;
			row->Cells["threshold_mm"]->Value = thr;
			row->Cells["D_sonde"]->Value = d_s;
			row->Cells["tail"]->Value = tail;
		}
		inFile.close();
	}
	dataGridView_grid_param->AutoResizeColumns(DataGridViewAutoSizeColumnsMode::ColumnHeader);
	ResizeGrid(dataGridView_grid_param);
	dataGridView_envir->AutoResizeColumns(DataGridViewAutoSizeColumnsMode::ColumnHeader);
	ResizeGrid(dataGridView_envir);

}
private: System::Void dataGridView_electrodes_UserDeletingRow(System::Object^ sender, System::Windows::Forms::DataGridViewRowCancelEventArgs^ e) {
	// Если нужно подтверждение:
	if (MessageBox::Show("Удалить выбранную строку?", "Удаление",
		MessageBoxButtons::YesNo) == System::Windows::Forms::DialogResult::No) {
		e->Cancel = true; // Отмена удаления
	}
}


};
}

