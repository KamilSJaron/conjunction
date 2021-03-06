/*
Deme is set of admixing individuals, picking of parents is relative to ther fitness.
Copyright (C) 2013-2016  Kamil S. Jaron

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <map>
#include <cmath>
#include <iomanip>
#include <fstream>

#include "../include/RandomGenerators.h"
#include "../include/Chiasmata.h"
#include "../include/Chromosome.h"
#include "../include/SelectionModel.h"
#include "../include/Deme.h"

/* DECLARATION */

// // // // // // // // // // // // // //
// constructor/destructors functions / //
// // // // // // // // // // // // // //

Deme::Deme(const Context& context, int ind, std::vector<int> neigb, char init, int size, double sel, double beta, int in_ch, int in_loc, int in_sel_loci, double in_lambda, int in_x, int in_y)
	: context{context}
{
	x = in_x;
	y = in_y;
	index = ind;
	neigbours = neigb;
	deme_size = size;
	deme = new Individual[deme_size];
	if(init == 'A' or init == 'B'){
		for(int i=0;i<deme_size;i++){
			deme[i] = Individual(&context, init, in_ch, in_loc, in_lambda, in_sel_loci, std::tuple<int, int, int>(in_x, in_y, i));
		}
	} else {
		int i = 0;
		while(i< (deme_size / 2)){
			deme[i] = Individual(&context, 'A', in_ch, in_loc, in_lambda, in_sel_loci, std::tuple<int, int, int>(in_x, in_y, i));
			i++;
		}
		while(i< deme_size){
			deme[i] = Individual(&context, 'B', in_ch, in_loc, in_lambda, in_sel_loci, std::tuple<int, int, int>(in_x, in_y, i));
			i++;
		}
	}
	selection_model.setSelectionPressure(sel);
	selection_model.setBeta(beta);
}


Deme::~Deme(){
	delete[] deme;
}

// // // // // // // // // // // // // // //
// // // // comunication functions / // // /
// // // // // // // // // // // // // // //

std::vector<int> Deme::getNeigbours(){
	return neigbours;
}

Individual Deme::getIndividual(int ind){
	return deme[ind];
}

int Deme::getDemeIndex(){
	return index;
}

void Deme::setDemeSize(int size){
	deme_size = size;
}

int Deme::getDemeSize(){
	return deme_size;
}

int Deme::getX(){
	return x;
}

int Deme::getY(){
	return y;
}

// // // // // // // // // // // // // //
// // // // computing functions / // // /
// // // // // // // // // // // // // //

void Deme::Breed(){
	double lambda = deme[0].getLambda();
	int sel_loci = deme[0].getNumberOfSelectedLoci();
	std::vector<double> fitnessVector;
	std::vector<Chromosome> gamete1, gamete2;
	std::vector<Chiasmata> chiasmata1, chiasmata2;
	getFitnessVector(fitnessVector);

	// for(unsigned int i = 0; i < fitnessVector.size(); i++){
	// 	std::cout << " " << fitnessVector[i] << " ";
	// }

	double RandMax = fitnessVector[deme_size-1];
	// std::cout << " RAND MAX: " << RandMax << std::endl;
	double roll;
	std::map<double, int> parentPick;
	std::map<double, int>::iterator it;
	std::vector<int> mothers(deme_size);
	std::vector<int> fathers(deme_size);

	for(int i=0;i < deme_size*2;i++){
		roll = (context.random.uniform() * RandMax);
		it = parentPick.find(roll);
		while(it != parentPick.end()){ //rolling twice same number will overwrite the first one, but this event is so rare, that solution is to just roll again without any bias
			roll = (context.random.uniform() * RandMax);
			it = parentPick.find(roll);
		}
		parentPick[roll] = i;
	}

	int i = 0;
	std::map<double, int>::iterator pos=parentPick.begin();

	while(pos!=parentPick.end()){

		if(fitnessVector[i] >= pos->first){
			if(pos->second >= deme_size){
				mothers[pos->second - deme_size] = i;
			} else {
				fathers[pos->second] = i;
			}
			++pos;
		} else {
			i++;
		}
	}

	Individual *metademe = new Individual[deme_size];
	for(int i=0;i<deme_size;i++){
		deme[mothers[i]].makeGamete(gamete1, chiasmata1);
		deme[fathers[i]].makeGamete(gamete2, chiasmata2);
		std::tuple<int, int, int> ind_birthplace(x,y,i);
		metademe[i] = Individual(&context, gamete1, chiasmata1, gamete2, chiasmata2, lambda, sel_loci, ind_birthplace);
		metademe[i].setParents(deme[mothers[i]].getBirthplace(),
							   deme[fathers[i]].getBirthplace());
	}

	for(int i=0;i<deme_size;i++){
		deme[i] = metademe[i];
	}
	delete[] metademe;
}

void Deme::integrateMigrantVector(std::vector<Individual>& migBuffer){
	unsigned int i = 0;
	while(i < migBuffer.size()){
		deme[i] = migBuffer[i];
		i++;
	}
	return;
}

double Deme::getMeanBproportion() const{
	double props = 0;
	for(int i = 0;i < deme_size;i++){
		props += deme[i].getBprop();
	}
//	std::cout << props / deme_size << props << " / " << deme_size << std::endl;
	return (props / deme_size);
}

double Deme::getProportionOfHomozygotes(char population) const{
	double proportion = 0;
	if(population == 'A'){
		for(int i = 0;i < deme_size;i++){
			proportion += (deme[i].isPureA());
		}
		proportion = proportion / deme_size;
		return proportion;
	}
	if(population == 'B'){
		for(int i = 0;i < deme_size;i++){
			proportion += (deme[i].isPureB());
		}
		proportion = proportion / deme_size;
		return proportion;
	}
	return 0;
}


double Deme::getProportionOfHeterozygotes() const{
	double proportion = 0;
	for(int i = 0;i < deme_size;i++){
		proportion += (deme[i].isPureA());
		proportion += (deme[i].isPureB());
	}
	proportion = 1 - (proportion / deme_size);
	return proportion;
}


void Deme::getFitnessVector(std::vector<double> &fitnessVector){
	double sum = 0, read_fitness = 0;
	fitnessVector.reserve(deme_size);
	if(deme[0].getNumberOfLoci(0) == deme[0].getNumberOfSelectedLoci()){
		for(int i = 0;i < deme_size;i++){
			// getBprop > getHetProp ??
			read_fitness = selection_model.getFitness(deme[i].getBprop());
	//		std::cout << " B prop: " << deme[i].getBprop() << " - fitness: " << read_fitness << std::endl;
			sum += read_fitness;
			fitnessVector.push_back(sum);
		}
	} else {
		for(int i = 0;i < deme_size;i++){
			// getBprop > getHetProp ??
			read_fitness = selection_model.getFitness(deme[i].getSelectedHybridIndex());
	//		std::cout << " B prop: " << deme[i].getBprop() << " - fitness: " << read_fitness << std::endl;
			sum += read_fitness;
			fitnessVector.push_back(sum);
		}
	}

	return;
}

// Computes average fitness in the deme
double Deme::getMeanFitness(){
	double sum = 0, read_fitness = 0, read_heterozygocity = 0;
	for(int i = 0;i < deme_size;i++){
		read_heterozygocity = deme[i].getBprop();
		read_fitness = selection_model.getFitness(read_heterozygocity);
		sum += read_fitness;
	}
	return sum / deme_size;
}


double Deme::getVARhi() const{
	double varz = 0, z = getMeanBproportion();
	for(int i = 0; i < deme_size; i++){
		varz += pow(z - deme[i].getBprop(),2);
	}
	varz = varz / deme_size;
	return varz;
}

double Deme::getVARp(){
	// number of chromosomes / loci is called form first individual of deme, and its first chromosome
	int number_chromosomes = deme[0].getNumberOfChromosomes(), number_loci = deme[0].getNumberOfLoci(0);
	double p = 0, varp = 0, pmean = getMeanBproportion();
	std::vector<double> ps;
	for(int ch = 0; ch < number_chromosomes; ch++){
		getps(ps,ch);
		for(unsigned int locus = 0;locus < ps.size();locus++){
			p = ps[locus];
			varp += (pmean - p) * (pmean - p);
		}
		ps.clear();
	}
	varp = varp / (number_loci*number_chromosomes);
	return varp;
}

void Deme::getps(std::vector<double>& ps, int ch){
	double p = 0;
	int TotalCHnum = deme_size * 2; //total number of chromosomes in deme (which one is computed using parameter ch)
	int number_loci = deme[0].getNumberOfLoci(0);
	std::vector<bool> states;	// 0 'A', 1 'B'
	std::vector<std::map<int, char>::iterator> chroms; //vector with chromosome junctions
	std::vector<int> ch_sizes; //vector with number of chromosome junctions

	chroms.reserve(TotalCHnum);
	ch_sizes.reserve(TotalCHnum);
	states.reserve(TotalCHnum);
	ps.reserve(number_loci);

	for(int i = 0;i < deme_size;i++){
		if(deme[i].getChromosomeBegining(0,ch)->second == 'A'){
			states.push_back(0);
		} else {
			states.push_back(1);
		}
		if(deme[i].getChromosomeBegining(1,ch)->second == 'A'){
			states.push_back(0);
		} else {
			states.push_back(1);
		}
		chroms.push_back(deme[i].getChromosomeBegining(0,ch));
		chroms.push_back(deme[i].getChromosomeBegining(1,ch));
		ch_sizes.push_back(deme[i].getNumberOfJunctions(0,ch));
		ch_sizes.push_back(deme[i].getNumberOfJunctions(1,ch));
	}
	for(int i = 0;i < number_loci;i++){
// 		I have to update states every iteration
		for(int j = 0;j < TotalCHnum;j++){
			if(chroms[j]->first == i){
				if(chroms[j]->second == 'A'){
					states[j] = 0;
				} else {
					states[j] = 1;
				}
				if(ch_sizes[j] != 0){
					chroms[j]++;
					ch_sizes[j] -= 1;
				}
			}
		}
		p = sum(states) / (double)(deme_size * 2);
//		std::cout << "\n " << index << "-ch: " << i << " \n";
//		std::cout << p << " = " << sum(states) << " / " << (double)(deme_size * 2) << std::endl;
		ps.push_back(p);
	}
}



double Deme::getLD(){
	double LD = 0, varz = getVARhi(), z = getMeanBproportion(), varp = getVARp();
	double n = deme[0].getNumberOfLoci(0) * deme[0].getNumberOfChromosomes(); //LOCI

	LD = (varz - ((1 / (2 * n)) * ((z * (1 - z)) - varp))) / (0.5 * (1 - (1 / n))) ;

	return LD;
}

double Deme::getLD(double z,double varz,double varp){
	double LD = 0, n = deme[0].getNumberOfLoci(0) * deme[0].getNumberOfChromosomes();

	LD = (varz - ((1 / (2 * n)) * ((z * (1 - z)) - varp))) / (0.5 * (1 - (1 / n))) ;

	return LD;
}



// // // // // // // // // // // // // //
// // // // plotting functions / // // //
// // // // // // // // // // // // // //

void Deme::showDeme(){
	int neigbsize = neigbours.size();
	std::cerr << std::setw(5) << std::right << index << ":  ";
	for(int i = 0; i < neigbsize; i++){
		std::cerr << std::setw(5) << std::left << neigbours[i] << " ";
	}
	std::cerr << std::endl;
}

void Deme::streamSummary(std::ostream& stream){
	int number_chromosomes = deme[0].getNumberOfChromosomes(), number_loci = deme[0].getNumberOfLoci(0);
	double z = getMeanBproportion();
	double varz = getVARhi();
	double varp = getVARp();
	int neigbsize = neigbours.size();
	stream << std::setw(5) << std::right << index << ":  ";
	for(int i = 0; i < neigbsize; i++){
		stream << std::setw(5) << std::left << neigbours[i] << " ";
	}
	stream << std::setw(6) << std::left << x;
	if(neigbours.size() > 2){
		stream << std::setw(6) << std::left << y;
	}
	stream << std::setw(12) << std::left << roundForPrint(getMeanFitness())
	<< std::setw(12) << std::left << roundForPrint(getProportionOfHeterozygotes())
	<< std::setw(12) << std::left << roundForPrint(z)
	<< std::setw(12) << std::left << roundForPrint(varz);
	if(number_loci * number_chromosomes > 1){
		stream	<< std::setw(12) << std::left << roundForPrint(varp)
		<< std::setw(12) << std::left << roundForPrint(getLD(z,varz,varp));
	}
	if((number_loci * number_chromosomes) <= 16){
		std::vector<double> ps;
		for(int ch = 0;ch < number_chromosomes; ch++){
			ps.clear();
			getps(ps,ch);
			for(unsigned int l = 0; l < ps.size();l++){
				stream << std::setw(12) << std::left << roundForPrint(ps[l]);
			}
		}
	}
	stream << std::endl;
}

void Deme::streamBlocks(std::ostream& stream){
	std::vector<std::string> block_sizes;
	for(int ind_index = 0; ind_index < deme_size; ind_index++){
		deme[ind_index].getGenotype(block_sizes);
		stream << x << "," << y << "," << ind_index << '\t';
		streamLine(stream,block_sizes);
	}
}

// small wrapper to stream tuples in streamChiasmata functions
std::string cat_tuple(std::tuple<int, int, int> in_tup){
	return std::to_string(std::get<0>(in_tup)) + ',' +
	       std::to_string(std::get<1>(in_tup)) + ',' +
	       std::to_string(std::get<2>(in_tup));
}

void Deme::streamChiasmata(std::ostream& stream){
	std::tuple<int, int, int> birthplace, mum, dad;
	std::vector<std::string> recombination_events;
	for(int ind_index = 0; ind_index < deme_size; ind_index++){
		deme[ind_index].getChiasmata(recombination_events);
		birthplace = deme[ind_index].getBirthplace();
		mum = deme[ind_index].getMum();
		dad = deme[ind_index].getDad();
		stream << cat_tuple(birthplace) << '\t';
		stream << cat_tuple(mum) << '\t';
		stream << cat_tuple(dad) << '\t';
		streamLine(stream,recombination_events);
	}
}

void Deme::streamHIs(std::ostream& stream) const{
	std::vector<double> HIs;
 	HIs.clear();
 	HIs.reserve(deme_size);
	for(int i = 0;i < deme_size;i++){
		HIs.push_back(deme[i].getBprop());
	}
	stream << x << "," << y << '\t';
	streamLine(stream, HIs);
	return;
}

void Deme::streamJunctions(std::ostream& stream) const{
	std::vector<double> juncs;
 	juncs.clear();
 	juncs.reserve(deme_size);
	for(int i = 0;i < deme_size;i++){
		juncs.push_back(deme[i].getNumberOfJunctions());
	}
	stream << x << "," << y << '\t';
	streamLine(stream, juncs);
	return;
}

void Deme::streamHeterozygosity(std::ostream& stream) const{
	std::vector<double> heterozs;
 	heterozs.clear();
 	heterozs.reserve(deme_size);
	for(int i = 0;i < deme_size;i++){
		heterozs.push_back(deme[i].getHetProp());
	}
	stream << x << "," << y << '\t';
	streamLine(stream, heterozs);
	return;
}

void Deme::readAllGenotypes(){
	for(int i=0;i<deme_size;i++){
		std::cerr << "Individual: " << i << " B proportion: " << deme[i].getBprop() << std::endl;
		deme[i].readGenotype();
	}
}


void Deme::readGenotypeFrequencies(){
	int number_chromosomes = deme[0].getNumberOfChromosomes(), number_loci = deme[0].getNumberOfLoci(0);
	std::vector<double> freqs;
	freqs.reserve(number_chromosomes*number_loci*2 + 1);
	for(int i=0;i < number_chromosomes*number_loci*2 + 1;i++){
		freqs.push_back(0);
	}
	for(int i=0;i<deme_size;i++){
		freqs[deme[i].getBcount()]++;
	}
	for(int i=0;i < number_chromosomes*number_loci*2 + 1;i++){
		std::cout << ((round((freqs[i] / deme_size) * 10000)) / 10000) << ' ';
	}
}

 // // // // //
//  PRIVATE //
// // // // //

int Deme::sum(std::vector<bool>& ve){
	int sum = 0;
	for(unsigned int i = 0; i < ve.size();i++){
		sum += ve[i];
	}
	return sum;
}

// CURENTLY DUPLICIT IN WORLD (as save_line)
template<typename T>
int Deme::streamLine(std::ostream& stream, std::vector<T>& vec) const{
	for(unsigned int ind = 0; ind < vec.size(); ind++){
		stream << vec[ind] << '\t';
	}
	stream << std::endl;
	return 0;
}

double Deme::roundForPrint(double number) const{
	return ((round(number * 1000000)) / 1000000);
}
