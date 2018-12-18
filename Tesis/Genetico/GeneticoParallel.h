#ifndef GENETICO_H
#define GENETICO_H

#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <tuple>
#include "Individuo.h"
#include <pthread.h>
#include "../Utils/sad.h"


//TODO INicializar el srand(time(NULL)) en el main

using namespace std;

typedef tuple<int,int> Pareja;
typedef function<void(Individuo &)> FunMutar;
typedef function<vector<Individuo>(Individuo,Individuo)> FunCrossover;



class Genetico{
	public:
		Genetico(){};
		Genetico(int sizePoblacion, vector<int> sizeCromo, int nFen, Prob probMutacion, FunFenotipos funFen, 
			FunFitness funFit, FunMutar funMutar, FunCrossover funCross);
		Individuo run(int iteraciones);
		int binary(Val e, int ini, int end);
		void selection();
		Pareja emparejar();
		void crossover(int madre, int padre, vector<Individuo> &res);
		void mutarPobl(vector<Individuo> &nextGeneration);
		void masacre(vector<Individuo> &nextGeneration);
		void print();
		void print(vector<Individuo> &nextGeneration);
		void del();
		FunFenotipos funFen;
		FunFitness funFit;
		FunMutar funMutar;
		FunCrossover funCross;
		vector<Individuo> poblacion;
		Prob probMutacion;
		vector<int> sizeCromos;
		int nFen;
		int sizePoblacion;
};

struct ThreadGenArgs{
	int ini;
	int end;
	Genetico * gen;
};


Genetico::Genetico(int sizePoblacion, vector<int> sizeCromos, int nFen, Prob probMutacion, FunFenotipos funFen,
		FunFitness funFit, FunMutar funMutar, FunCrossover funCross){
	this->sizePoblacion = sizePoblacion;
	this->sizeCromos = sizeCromos;
	this->nFen = nFen;
	this->probMutacion = probMutacion;
	this->funFit = funFit;
	this->funFen = funFen;
	this->funMutar = funMutar;
	this->funCross = funCross;
	for(int i = 0; i < sizePoblacion; i++){
		poblacion.push_back(Individuo(sizeCromos, nFen, funFen, funFit));
	}
}

int Genetico::binary(Prob e, int ini, int end){
	if(ini == end){
		if(poblacion[ini].probSelecAcum >= e) return ini;
		return ini + 1; 
	}
	if(end - ini == 1){
		if(poblacion[end].probSelecAcum <= e) return end + 1;
		if(poblacion[ini].probSelecAcum < e) return end;
		return ini;
	}
	int med = (end + ini) / 2;
	if(poblacion[med].probSelecAcum == e) return med;
	if(poblacion[med].probSelecAcum > e) return binary(e,ini,med - 1);
	return binary(e,med,end);
}

Pareja Genetico::emparejar(){
	Prob randMadre = ((Prob) rand() / RAND_MAX);
	Prob randPadre = ((Prob) rand() / RAND_MAX);
	int madre = binary(randMadre, 0, sizePoblacion - 1);
	int padre = binary(randPadre, 0, sizePoblacion - 1);
	return make_tuple(madre,padre);
}


void Genetico::mutarPobl(vector<Individuo> &nextGeneration){
	float random = 0;
	for(int i = 0 ; i < nextGeneration.size(); i++){
		random = ((Val) rand() / RAND_MAX);
		if(random <= probMutacion){
			funMutar(nextGeneration[i]);	
			delete[] nextGeneration[i].fenotipos;
			nextGeneration[i].evaluar(funFen, funFit);
		} 
	}
}

void Genetico::masacre(vector<Individuo> &nextGeneration){
	sort(nextGeneration.begin(), nextGeneration.end(), funSortIn);
	auto ini = nextGeneration.begin();
	auto end = nextGeneration.begin() + sizePoblacion;
	for(Individuo i : poblacion){
		i.del();
	}
	poblacion = vector<Individuo>(ini,end);
	for(int i = sizePoblacion; i < nextGeneration.size(); i++){
		nextGeneration[i].del();
	}
}

void * parallelCross(void * arg){
	ThreadGenArgs tArgs = *((ThreadGenArgs *) arg);
	int ini = tArgs.ini;
	int end = tArgs.end;
	Genetico * gen = tArgs.gen;
	vector<Individuo> * hijos;
	for(int i = ini; i < end; i++){
		int padre = 0;
		int madre = 0;
		tie(madre,padre) = emparejar();
//		cout<<"MP->"<<madre<<"->"<<padre<<endl;
		if(madre == gen->sizePoblacion) madre = gen->sizePoblacion - 1;
		if(padre == gen->sizePoblacion) padre = gen->sizePoblacion - 1;
		(*hijos) = funCross(gen->poblacion[madre],gen->poblacion[padre]);
	}
	return (void *) hijos;
}


void Genetico::selection(){
	Val fitnessTotal = 0;
	Val probSelecTotal = 0;
	vector<Individuo> nextGeneration;
	for(Individuo i : poblacion){
		fitnessTotal += i.fitness;
	}
	for(int i = 0; i < sizePoblacion; i++){
		poblacion[i].probSelec = poblacion[i].fitness / fitnessTotal;
		probSelecTotal += poblacion[i].probSelec;
		poblacion[i].probSelecAcum = probSelecTotal;
	}
	
	pthread_t threads[NUM_THREADS];
	ThreadGenArgs threadArgs[NUM_THREADS];
	vector<Individuo> * hijos = nullptr;
	int despl = sizePoblacion/NUM_THREADS;
	for(int i = 0; i < NUM_THREADS; i++){
		threadArgs[i].ini = i * despl;
		if(i == NUM_THREADS - 1) threadArgs[i].end = sizePoblacion;
		else threadArgs[i].end = threadArgs[i].ini + despl;
		threadArgs[i].gen = this;
		pthread_create(&threads[i], NULL, parallelCross, (void *) &threadArgs[i]);
	}
	for(int i = 0; i < NUM_THREADS; i++){
		pthread_join(threads[i],(void**) hijos);
		for(Individuo i : (*hijos)){
			nextGeneration.push_back(i);
		}
		delete hijos;
		hijos = nullptr;
	}
	mutarPobl(nextGeneration);
	masacre(nextGeneration);
}


Individuo Genetico::run(int iteraciones){
	for(int i = 0; i < iteraciones; i++){
		selection();
	}

	return poblacion[0];
}


void Genetico::print(){
	for(Individuo i : poblacion){
		i.print();
	}
}

void Genetico::print(vector<Individuo> &nextGeneration){
	for(Individuo i : nextGeneration){
		i.print();
	}	
}

void Genetico::del(){
	for(Individuo i : poblacion){
		i.del();
	}
}


#endif
