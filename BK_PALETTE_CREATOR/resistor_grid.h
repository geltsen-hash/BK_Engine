#pragma once
#include "pch.h"
#include <vector>



using namespace std;

class ResistorGrid {
private:
	int W, H, N;
	vector<double> r_coords;
	vector<double> z_coords;
	vector<double> v;
	vector<double> R_hor;
	vector<double> R_ver;
	vector<int> is_fixed;
	vector<double> fixed_v;
	vector<MeasElectrode> saved_m_els;
	int getZIndex(double target_z) const;

	int getRIndex(double target_r) const;

	double getRhoForRadius(double r, const vector<LayerDakhnov>& zones) const;

public:
	ResistorGrid(const vector<double>& r_nodes, const vector<double>& z_nodes);

	void fillModel(const vector<LayerDakhnov>& zones);

	void setSonde(const vector<Electrode>& a_els, const vector<Electrode>& b_els, const vector<MeasElectrode>& m_els);

	void solveDirect(bool debug);

	double getElectrodeCurrent(const Electrode& el) const;
	
	double getMeasElectrodeCurrent(const MeasElectrode& el) const;
	
	double getMeasElectrodeVoltage(const MeasElectrode& el) const;

	double getRightBoundaryCurrent() const;
	
	void exportMapsWithLegendToBMP();
};

