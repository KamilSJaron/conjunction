#ifndef IMIGRANT_H
#define IMIGRANT_H

#include <vector>

#include "../include/Chromosome.h"
#include "../include/Context.h"

class Imigrant
{
	public:
		/* DECLARATION */
		Imigrant(const Context &context, int input_ch, int size, double input_lambda);
		/* init Individual as 'A', 'B' or pure "AB" heterozygot*/
		Imigrant(const Context &context, char origin, int input_ch, int size, double input_lambda);
		/*init Individual by gametes on imput */
		Imigrant(const Context &context, std::vector<Chromosome>& gamete, double input_lambda);
		~Imigrant(); /* destructor */

		/* COMPUTIONG METHODS */
		int getChiasma();
		void makeGamete(std::vector<Chromosome>& gamete);

		void getSizesOfBBlocks(std::vector<int>& sizes);

		/* STATS */
		int getNumberOfBBlocks();
		int getNumberOfJunctions() const;

		double getBprop() const;
		bool isPureA() const;
		bool isPureB() const;

		/* GETTERS */
		int getLambda() const;

		/* ASCII PLOTTING METHODS */
		void readGenotype();

	private:
		const Context &context;
		std::vector<Chromosome> genome;
		int number_of_chromosomes;
		double lambda;
};


#endif
