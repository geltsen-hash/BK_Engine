#include "pch.h"
#include "Interface_Form.h"
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
#pragma comment(lib,"Winmm.lib")

#include <omp.h>
#pragma warning(disable:4244)

using namespace std;
using namespace System;
using namespace System::Windows::Forms;
using namespace System::Runtime::InteropServices;

vector<double> Ro_BH_steps;
vector<double> Ro_PZ_steps;
vector<double> Ro_stratum_steps;
vector<double> R_BH_steps;
vector<double> R_PZ_steps;
vector<BK> for_inversion;
vector<vector<ELECTRODE>> electrodeList;
//vector<vector<vector<vector<BK>>>> BK_pallete_;
Vector_BK BK_pallete;
ifstream fin;
BK BK_pallete_L[Ds_steps][Rzp_steps][Dzp_steps][Rop_steps];
string BK_pallete_file_name;
string BK_pallete_file_name_leg[Ds_steps][Rzp_steps][Dzp_steps];

string dir_pass = "D:\\BOKOVOI\\Palet_120M\\";

[STAThread]
int main() {

	//по диаметру скважины
	for (uint8_t Ds_step = 0; Ds_step < Ds_steps; Ds_step++) {
		//по контрасту УЭС пласта к УЭС зоны проникновения
		for (uint8_t Rzp_step = 0; Rzp_step < Rzp_steps; Rzp_step++) {
			//по диаметру зоны проникновения
			for (uint8_t Dzp_step = 0; Dzp_step < Dzp_steps; Dzp_step++) {
				//формируем строку с именем файла
				string file_name = dir_pass + Ds[Ds_step] + "-Rz_" + Rzp[Rzp_step] + "-Dz_" + Dzp[Dzp_step] + "-M.dat";
				//пишем массив имен файлов на всякий случай
				BK_pallete_file_name_leg[Ds_step][Rzp_step][Dzp_step] = file_name;
				//открываем файл
				fin.open(file_name.c_str(), ios::in);
				//внутри файла по контрасту УЭС пласта к УЭС скважинного флюида
				for (uint8_t Rp_step = 0; Rp_step < Rop_steps; Rp_step++) {
					float Rop_Ros; BK buff;
					//читаем в буфер 
					fin >> Rop_Ros;
					fin >> buff.Ib[S]; fin >> buff.Ib[M]; fin >> buff.Ib[L];
					fin >> buff.dU[S]; fin >> buff.dU[M]; fin >> buff.dU[L];
					fin >> buff.Ia[S]; fin >> buff.Ia[M]; fin >> buff.Ia[L];
					fin >> buff.Ib[SML]; fin >> buff.dU[SML]; fin >> buff.Ia[SML];
					string rzp_str = Rzp[Rzp_step];
					std::replace(rzp_str.begin(), rzp_str.end(), '_', '.');
					string dzp_str = Dzp[Dzp_step];
					std::replace(dzp_str.begin(), dzp_str.end(), '_', '.');

					buff.R_BH = std::stod(Ds[Ds_step]);
					buff.Ro_PZ_Ro_BH = std::stod(rzp_str);
					buff.R_PZ_R_BH = std::stod(dzp_str);
					buff.Ro_stratum_Ro_BH = Rop_Ros_ratio_steps[Rp_step];
					//загоняем в палетку
					BK_pallete_L[Ds_step][Rzp_step][Dzp_step][Rp_step] = buff;

				}
#ifdef debug_mode				
				if (!fin) {
					cout << (int)Ds_step << " " << (int)Rzp_step << " " << (int)Dzp_step << " not open! " << BK_pallete_file_name_leg[Ds_step][Rzp_step][Dzp_step] << endl;
				}
				else {
					cout << (int)Ds_step << " " << (int)Rzp_step << " " << (int)Dzp_step << " ok! " << BK_pallete_file_name_leg[Ds_step][Rzp_step][Dzp_step] << endl;
				}
#endif					
				fin.close();


			}
		}
	}


	//читаем палетки из файла
	//BK_pallete_file_name = dir_pass + "BK_120_pallete.leg";
	//fin.open(BK_pallete_file_name.c_str(), ios_base::binary);
	//fin.read((char*)BK_pallete_L, sizeof(BK_pallete_L));
	//fin.close();

	BK_PALETTE_CREATOR::MyForm^ SONDE_OKR = gcnew BK_PALETTE_CREATOR::MyForm;
	Application::Run(SONDE_OKR);
	//SONDE_OKR->Show();
	getchar(); getchar();
	return 0;
    
}
