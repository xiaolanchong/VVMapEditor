//! \file NeuralNet.h
//! \brief класс CNeuralNet
//!
//! Создан давно, А. Малистов, А. Хамухин
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEURALNET_H__FCEFF579_C82E_49C6_9C5D_297BEABAD775__INCLUDED_)
#define AFX_NEURALNET_H__FCEFF579_C82E_49C6_9C5D_297BEABAD775__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <windows.h>
#include <math.h>
#include <stdlib.h>
//! класс нейронной сети с обучением методом градиентного спуска
class CNeuralNet  
{
public:
	class Layer;
	Layer* layers;//!< массив слоёв нейросети

	//! Посчитать ошибку нейросети на тестовом наборе (результат записывается в TestErr)
	virtual void CalcTestErr();
	//! Один шаг градиентного спуска. percent - доля примеров, которые выбираются случайным образом для подсчёта целевой функции, если равен 100, то все примеры. Градиент считается методом обратного распространения ошибки
	virtual void Iteration(double percent=100);
	//! Передать обучающую информацию в виде двумерного массива N x (NEnt+NOut), в свойствах одного примера сначала передаётся NEnt свойств, затем ответ в виде NOut 1 и -1.
	virtual void SetXLearn(long N, double *X);
	//! Передать тестовую информацию в виде двумерного массива N x (NEnt+NOut), в свойствах одного примера сначала передаётся NEnt свойств, затем ответ в виде NOut 1 и -1.
	virtual void SetXTest(long N, double *X);
	class Neuron;
	
	//! Получить компоненту index ответа нейросети после вызова CNeuralNet::GetResult
	virtual double Y(long index);
	//! Расчитать все выходы нейронов при входе X
	virtual void GetResult(double *X);
	/*! \brief Задать конфигурацию нейросети
		\param NumOfLayers - количество слоёв нейросети
		\param list - массив размерности (NumOfLayres+1), list[0] - количество входов, list[1],...,list[NumOfLayers] - 
		количество нейронов на соответствующих слоях. Соответственно, list[NumOfLayers] задаёт одновременно количество
		выходов (ответов) нейросети.
	*/
	virtual void Init(long NumOfLayers, long *list);
	//! Удалить из памяти данные, выделенные нейросетью
	virtual void Delete();
	//! Задать начальные значения переменных
	virtual void Init();
	//! Полулчить число нейронов на слое номер index (отсчёт слоёв начинается с 0)
	virtual long GetNumOfNeuronsInLayer(long index);
	

	double step;//!< множитель градиента в алгоритме градиентного спуска
	long NL; //!< количество слоёв в сети
	long NEnt;//!< количеств входов сети
	long NOut;//!< количество выходов сети
	long RecLen;//!< =NEnt+NOut
	long TotalNumOfWeights;//!< Общее количество весов сети
	long NLearn;//!< количество примеров в обучении
	long NTest;//!<  количество примеров в тесте
	long CountOfLearning;//!< количество примеров, по которым подсчитано текущее значение целевой функции при минимизации (может быть меньше NLearn)
	double* XLearn;//!< массив с обучающей информацией
	double* XTest;//!< массив с тестовой информацией
	double Er;//!< Текущая ошибка обучающего набора
	double TestEr;//!< Текущая ошибка тестового набора
	
	//! конструктор
	CNeuralNet();
	//! деструктор
	virtual ~CNeuralNet();
	
	//! Нейрон нейросети
	class Neuron {
	public:
		double *W;//!< веса нейрона
		double *dE;//!< производные \f$ E'_{W_i}\f$ по весам данного нейрона (E - ошибка на обучающем наборе
		long NW;//!< число весов нейрона
		double V;//!< \f$ W_0+\sum\limits_{i=1}^{NW} W_iX_i\f$
		double Y;//!< пороговая функция, расчитанная в точке V (выход нейрона)

		double dY;//!< производная пороговой функции \f$ f'_V \f$
		double Z;//!< сумма для обратного распространения ошибки, необходима для рассчёта производной по формуле диффиренцирования сложной функции (сумма копится от выходных нейронов к нижним)
		double *XX;//!< \f$X_i \cdot dY \f$

		double *pushArr;//!< массив для сохранения состояния нейрона (для весов)
		
		//! Сохранить веса в нейрон pushArr. Применяется перед новой итерацией
		void Push() {if (NW>0) CopyMemory(pushArr,W,sizeof(double)*NW);	}
		//! Восстановить веса нейрона из массива pushArr. Применяется, если новая итерация не уменьшила ошибку.
		void Pop() {if (NW>0) CopyMemory(W,pushArr,sizeof(double)*NW);	}
		//! конструктор
		Neuron(){Init();
		}
		//! установка начальных значений
		void Init() { W=NULL; NW=0; Y=0; Z=0; XX=NULL; dE=NULL; pushArr=NULL;
		}
		//! удаление массивов из памяти
		void Delete() {
			if (pushArr) delete[] pushArr;
			if (dE) delete[] dE;
			if (W) delete[] W;
			if (XX) delete[] XX;
			Init();
		}
		//! деструктор
		~Neuron(){
			Delete();
		}
		//! инициализировать нейрон с весами для NumOfEnters входов
		void Init(long NumOfEnters) {
			Delete();
			NW=NumOfEnters+1;
			W=new double[NW];
			dE=new double[NW];
			XX=new double[NW];
			pushArr=new double[NW];
			InitW();
		}
		//! записать в веса нейрона равномерно распределённые случайные значения от -1 до 1
		void InitW(){
			long i;
			for (i=0; i<NW; i++)  {
				W[i]=(2.0*rand()/RAND_MAX-1)*2;
			}
		}
		//! расчет пороговой функции
		inline double f(double v) {
			//return (v/(1+fabs(v)));
			if (v>1) return 1;			if (v<-1) return -1;			return v;
		}
		//! расчет производной от пороговой функции
		double df(double v) {
			double Zn=1+fabs(v);
			//return (1/(Zn*Zn));
			if ((v>1) || (v<-1)) return (1/(Zn*Zn));	return 1;
		}
		//! Расчитать выход нейрона при входе X
		inline double GetResult(double *X) {
			V=W[NW-1]; XX[NW-1]=1;
			for (long i=NW-2; i>=0; i--) {
				V+=W[i]*(XX[i]=X[i]);
			}
			return  Y=f(V);
		}
		
		//! обнулить градиенты
		void CleardE(){
			for (long i=0; i<NW; i++) dE[i]=0;
		}
		//! посчитать производную dY и значения XX
		void SetdY() {
			dY=df(V); Z=0;
			for (long i=0; i<NW; i++) XX[i]*=dY;			
		}
		//! "распространить ошибку" sz с верхнего нейрона
		void SendZ(double sz) {
			Z+=sz;
		}
		//! расчитать добавочное значение градиента функции ошибок для текущего примера обучения
		void AdddE() {
			for (long i=0; i<NW; i++) dE[i]+=Z*XX[i];
		}
		//! сделать шаг градиентного спуска с коэффициентом eps (значение CNueralNet::step) и штрафом за большие веса k
		void ChangeW(double eps, double k) {
			for (long i=0; i<NW; i++) W[i]-=eps*(dE[i]+k*W[i]);
		}
	};
	//! слои нейронов
	class Layer {
	public:
		Neuron* neurons;//!< массив нейронов данного слоя
		long NN;//!< количество нейронов
		double *Y;//!< выходы нейронов данного слоя
		//! конструктор
		Layer() {Init();
		}
		//! задание начальных значений
		void Init() {neurons=NULL; Y=NULL; NN=0;
		}
		//! удаление массивов из памяти
		void Delete() {
			if (Y) delete[] Y;
			if (neurons) delete[] neurons;
			Init();
		}
		//! деструктор
		~Layer(){
			Delete();			
		}
		//! проинициализировать слой с NumOfNeurons нейронами, в каждом NumOfEnters входов
		void Init(long NumOfNeurons, long NumOfEnters) {
			NN=NumOfNeurons;
			neurons=new Neuron[NN];
			Y=new double[NN];
			long i;
			for (i=0; i<NN; i++) {
				neurons[i].Init(NumOfEnters);
				Y[i]=0;
			}
		}
		//! произвести расчёт для всех нейронов слоя по входу X
		void GetResult(double *X) {	for (long i=NN-1; i>=0; i--) Y[i]=neurons[i].GetResult(X);	}
		//! сохранить состояние нейронов слоя (только один уровень сохранения, стек состояний не поддерживается)
		void Push() {for (long i=0; i<NN; i++) neurons[i].Push();}
		//! восстановить сохраненное состояние нейронов слоя
		void Pop() {for (long i=0; i<NN; i++) neurons[i].Pop();}
	};
private:
	//! сохранить состояние нейронов сети (только один уровень сохранения, стек состояний не поддерживается)
	void NPush();
	//! восстановить сохраненное состояние нейронов сети
	void NPop();
	//! установка начальных значений в переменных нейронов для подсчёта градиента
	void IterCleardE();
	//! установка ошибки обратного распространения Z для верхнего слоя
	void IterInitLastLayer(double *YOut);
	//! сделать шаг вдоль текущего градиента
	void IterChangeW();
	//! расчитать все производные в нейронах
	void IterSetdY();
	//! посчитать ошибку сети по набору X из N примеров
	double CalcEr(double *X,long N);
	//! удалить тестовых набор
	void ClearXLearn();
	//! удалить обучающий набор
	void ClearXTest();
	
	long cnt;//!< счётчик итераций
public:
	//! загрузить нейросеть из буфера памяти data
	virtual void Load(void* data);
	//! Создаёт буфер *data с сохраненным состоянием сети. Возвращает количество байт в буфере.
	virtual long Save(void** data);
};

#endif // !defined(AFX_NEURALNET_H__FCEFF579_C82E_49C6_9C5D_297BEABAD775__INCLUDED_)
