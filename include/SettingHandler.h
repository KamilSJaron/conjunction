/*
 * Dispatcher.h
 *
 *  Created on: Jun 17, 2016
 *      Author: kjaron
 */

#ifndef SRC_DISPATCHER_H_
#define SRC_DISPATCHER_H_

class SettingHandler{
	public:
		SettingHandler(std::string filename);

		SimulationSetting getSimualtionSetting(int index);
		int getNumberOfSimulations() const;

		// printing functions

		void printParameters() const;
		void printParameterCounts() const;
		//	void plotAsciiWorld();

	private:
		int parseSetting(std::ifstream& myfile);
		int parseWorldDefinition(std::string& line);
		void parameterSave(std::string& parameter, double value);
		void parameterSave(std::string& parameter, std::vector<double>& value);
		void updateNumberOfSimulations();
		template<typename T>
		int printVectorValue(int index, std::vector<T> val_vec) const;
		char setPatameterOfSetting(SimulationSetting& mySetting, std::string parameter, int index);

		// simulation parameters

		std::vector<int> loci, chrom, deme, gen;
		std::vector<double> sel, beta, lambda;

		// capture output parameters
		std::string file_name_patten, type_of_save;
		std::vector<int> seed, delay, saves;

		// world setting

		int up_down_demes, left_right_demes, dimension, edges_per_deme;
		std::string type_of_updown_edges, type_of_leftright_edges;

		// internals
		int number_of_simulations;
		std::vector<std::string> parameters_in_order;
		std::vector<int> parameters_numbers;
};

#endif /* SRC_DISPATCHER_H_ */
