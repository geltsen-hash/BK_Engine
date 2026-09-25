#include "pch.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <complex>
#include <cmath>
#include <iomanip>
#include <cstdlib>
using Complex = std::complex<double>;
// Заглушка для внешних переменных и функций, которые использовались в Fortran глобально.
// Структура создана для того, чтобы скомпилировать переведенный C++ код.
struct ExternalContext {
	int NonConstantConstraintNumber;
	double RadiusOfElectrode;
	double CurrentOfSource;
	double VoltageOfSource;
	int IndexOfElectrodeA0;
	int IndexOfElectrodeM1;
	int IndexOfElectrodeN;
	double K_DLL;

	Complex VoltageOfSourceAterCalculation;
	Complex CurrentOfSourceAterCalculation;
	Complex ImpedanceOfSystemDLL;
	Complex ApparentResistivityOfDeepLaterolog;

	std::vector<Complex> NonConstantConstraintValue;
	std::vector<std::vector<Complex>> FocusingConditionDeep;

	void SolveMatrixB(std::vector<std::vector<Complex>>& B, std::vector<Complex>& Vector, std::vector<Complex>& X, int MatrixSize) {
		// Заглушка внешней функции решения системы линейных уравнений B * X = Vector
	}

	const double Pi = 3.14159265358979323846;
};
class AC_Laterolog_Tool : public ExternalContext {
public:
	void NonConstantConstraint_AC_Laterolog_Deep_RouHPhi() {

		// Исправленные опечатки OCR (5 -> =, 1 -> +, 12Pi -> 1.0/(2.0*Pi), и др.)
		int NumberOfInsulator = NonConstantConstraintNumber;
		int NumberOfElectrode = NumberOfInsulator - 1;
		int MatrixSize = NumberOfInsulator + 2 * NumberOfElectrode;
		if (NonConstantConstraintValue.size() != static_cast<size_t>(NonConstantConstraintNumber)) {
			NonConstantConstraintValue.assign(NonConstantConstraintNumber, 0.0);
		}
		// Динамическое распределение массивов (C++ vector автоматически заполняет 0 при инициализации)
		std::vector<std::vector<Complex>> SourceMatrix(NonConstantConstraintNumber, std::vector<Complex>(NonConstantConstraintNumber, 0.0));
		std::vector<std::vector<Complex>> A(NonConstantConstraintNumber, std::vector<Complex>(NonConstantConstraintNumber, 0.0));
		std::vector<std::vector<Complex>> B(MatrixSize, std::vector<Complex>(MatrixSize, 0.0));
		std::vector<std::vector<Complex>> Bcopy(MatrixSize, std::vector<Complex>(MatrixSize, 0.0));
		std::vector<Complex> RouHphi(NumberOfInsulator, 0.0);
		std::vector<Complex> I(NumberOfElectrode, 0.0);
		std::vector<Complex> V(NumberOfElectrode, 0.0);
		std::vector<Complex> X(MatrixSize, 0.0);
		std::vector<Complex> Vector(MatrixSize, 0.0);
		std::vector<Complex> VectorCopy(MatrixSize, 0.0);
		// Чтение исходной матрицы из файла 'SourceMatirx.dat'
		std::ifstream inFile("SourceMatirx.dat");
		if (inFile.is_open()) {
			for (int m = 0; m < NonConstantConstraintNumber; ++m) {
				for (int n = 0; n < NonConstantConstraintNumber; ++n) {
					double realPart = 0.0, imagPart = 0.0;
					// Ожидается чтение пары чисел для формирования комплексного числа
					inFile >> realPart >> imagPart;
					SourceMatrix[m][n] = Complex(realPart, imagPart);
					A[m][n] = SourceMatrix[m][n];
				}
			}
			inFile.close();
		}
		// Формирование матрицы B: Строки от 0 до NumberOfInsulator - 1 (C++ 0-based)
		for (int m = 0; m < NumberOfInsulator; ++m) {
			for (int n = 0; n < NumberOfInsulator; ++n) {
				B[m][n] = A[m][n];
			}
		}
		for (int m = 0; m < NumberOfInsulator; ++m) {
			for (int n = NumberOfInsulator; n < NumberOfInsulator + NumberOfElectrode; ++n) {
				B[m][n] = 0.0;
			}
		}
		for (int m = 0; m < NumberOfInsulator; ++m) {
			int l = 0;
			for (int n = NumberOfInsulator + NumberOfElectrode; n < NumberOfInsulator + 2 * NumberOfElectrode; ++n) {
				l++;
				int M_fort = m + 1; // 1-based аналог M для логики из Фортрана
				if (l == M_fort)       B[m][n] = -1.0 / (2.0 * Pi * RadiusOfElectrode);
				if (l == (M_fort - 1)) B[m][n] = 1.0 / (2.0 * Pi * RadiusOfElectrode);
				// В оригинальном коде эти строки перезаписывают предыдущие условия без else, 
				// логика перенесена 1-к-1.
				if (l == M_fort)       B[m][n] = -1.0 / (2.0 * Pi);
				if (l == (M_fort - 1)) B[m][n] = 1.0 / (2.0 * Pi);
			}
		}
		// Формирование матрицы B: Строки от NumberOfInsulator до NumberOfInsulator + NumberOfElectrode - 1
		int l_counter = 0;
		for (int m = NumberOfInsulator; m < NumberOfInsulator + NumberOfElectrode; ++m) {
			l_counter++;
			for (int n = 0; n < NumberOfInsulator + 2 * NumberOfElectrode; ++n) {
				int N_fort = n + 1;
				if (N_fort == l_counter) B[m][n] = 1.0;
				if (N_fort == (l_counter + 1)) B[m][n] = -1.0;

				if (N_fort == (l_counter + NumberOfInsulator)) B[m][n] = 1.0 / (2.0 * Pi * RadiusOfElectrode);
				if (N_fort == (l_counter + NumberOfInsulator)) B[m][n] = 1.0 / (2.0 * Pi);
			}
		}
		// Формирование матрицы B: Строки фокусировок
		for (int m = NumberOfInsulator + NumberOfElectrode; m < NumberOfInsulator + 2 * NumberOfElectrode; ++m) {
			for (int n = NumberOfInsulator; n < NumberOfInsulator + 2 * NumberOfElectrode; ++n) {
				int M_fort = m + 1;
				int N_fort = n + 1;
				// Предполагается, что FocusingConditionDeep - это 0-based std::vector / структура данных
				B[m][n] = FocusingConditionDeep[M_fort - NumberOfInsulator - NumberOfElectrode - 1][N_fort - NumberOfInsulator - 1];
			}
		}
		// Последняя строка матрицы для источника тока или напряжения
		int m_last = NumberOfInsulator + 2 * NumberOfElectrode - 1; // 0-based индекс последней строки

		if (CurrentOfSource != 0.0 && VoltageOfSource == 0.0) {
			// IndexOfElectrodeA0 предполагается 1-based как в Fortran. Смещаем на -1 для C++.
			B[m_last][NumberOfInsulator + IndexOfElectrodeA0 - 1] = 1.0;
		}
		else if (CurrentOfSource == 0.0 && VoltageOfSource != 0.0) {
			B[m_last][NumberOfInsulator + NumberOfElectrode + IndexOfElectrodeA0 - 1] = 1.0;
		}
		else {
			std::cerr << "\nWrong Message: Please check input current or voltage source value.\n\n";
			std::exit(EXIT_FAILURE); // Заменяет Фортрановский STOP
		}
		// Формирование вектора правой части
		if (CurrentOfSource != 0.0 && VoltageOfSource == 0.0) {
			Vector[m_last] = CurrentOfSource;
		}
		else if (CurrentOfSource == 0.0 && VoltageOfSource != 0.0) {
			Vector[m_last] = VoltageOfSource;
		}
		// Решение системы BX = Vector
		Bcopy = B;
		VectorCopy = Vector;

		SolveMatrixB(B, Vector, X, MatrixSize);

		B = Bcopy;
		Vector = VectorCopy;
		// Извлечение результатов: RouHphi, I и V
		for (int m = 0; m < NumberOfInsulator; ++m) {
			RouHphi[m] = X[m];
			NonConstantConstraintValue[m] = RouHphi[m];
		}

		int l_idx = 0;
		for (int m = NumberOfInsulator; m < NumberOfInsulator + NumberOfElectrode; ++m) {
			I[l_idx++] = X[m];
		}

		l_idx = 0;
		for (int m = NumberOfInsulator + NumberOfElectrode; m < NumberOfInsulator + 2 * NumberOfElectrode; ++m) {
			V[l_idx++] = X[m];
		}
		// Вычисление импеданса системы
		Complex Vs = V[IndexOfElectrodeA0 - 1];
		Complex Is = I[IndexOfElectrodeA0 - 1];
		VoltageOfSourceAterCalculation = Vs;
		CurrentOfSourceAterCalculation = Is;
		ImpedanceOfSystemDLL = Vs / Is;
		// Кажущееся сопротивление (Apparent Resistivity)
		ApparentResistivityOfDeepLaterolog = K_DLL * (V[IndexOfElectrodeM1 - 1] - V[IndexOfElectrodeN - 1]) / Is;
		// Тестирование: Вывод матрицы B и Вектора в файлы
		std::ofstream outMatrix("MatrixB.dat");
		if (outMatrix.is_open()) {
			outMatrix << std::fixed << std::setprecision(20);
			for (int m = 0; m < MatrixSize; ++m) {
				for (int n = 0; n < MatrixSize; ++n) {
					outMatrix << B[m][n].real() << " " << B[m][n].imag() << "\n";
				}
			}
			outMatrix.close();
		}
		std::ofstream outVector("Vector.DAT");
		if (outVector.is_open()) {
			outVector << std::fixed << std::setprecision(20);
			for (int m = 0; m < MatrixSize; ++m) {
				outVector << Vector[m].real() << " " << Vector[m].imag() << "\n";
			}
			outVector.close();
		}
		// Тестирование: Проверка на равенство B*X и Vector
		for (int m = 0; m < MatrixSize; ++m) {
			VectorCopy[m] = 0.0;
			for (int n = 0; n < MatrixSize; ++n) {
				VectorCopy[m] += B[m][n] * X[n];
			}
		}
		for (int m = 0; m < MatrixSize; ++m) {
			VectorCopy[m] = VectorCopy[m] - Vector[m];
		}
		std::ofstream outError("VectorError.DAT");
		if (outError.is_open()) {
			for (int m = 0; m < MatrixSize; ++m) {
				outError << VectorCopy[m].real() << " " << VectorCopy[m].imag() << "\n";
			}
			outError.close();
		}
		std::ofstream outX("ValueOfX.DAT");
		if (outX.is_open()) {
			for (int m = 0; m < MatrixSize; ++m) {
				outX << "X(" << m + 1 << ")=" << X[m].real() << " " << X[m].imag() << "\n";
			}
			outX.close();
		}

	}
};