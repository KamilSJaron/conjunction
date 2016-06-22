
int testIndividual(){
	Individual Lili;
	Individual Hugo('C');
    Individual Igor('C', 3, 50, 2.3);

	double sumCh = 0.0;
	for(int i = 0; i < 10000; i++){
		sumCh += Igor.getChiasma();
	}
	sumCh = sumCh / 10000;

	if(sumCh > 2.5 or sumCh < 2.1){
		cerr << "WARNING: unexpected behavior of generator of Chiasmas, " << endl;
		cerr << "Change seed for random number and rerun tests, if this message occurs again"
		<< " something is wrong, please do not ignore it and report the issue." << endl;
		return 2;
	}

	// to test all computing functions on semi-known system, one gamete for the tested individual Stuart
	// will be a product of recombination and the second will be pure 'A' gamete. Therefore heterozygocity
	// will be equivalent to proportion of B in the sirt set
	vector<Chromosome> gamete1;;
	Igor.makeGamete(gamete1);

	vector<Chromosome> gamete2;
	for(unsigned int i = 0; i < gamete1.size(); i++){
		gamete2.push_back(Chromosome('A',gamete1[i].getResolution()));
	}
	Individual Stuart(gamete1, gamete2, 1.6);

	int count = 0;
	for(int i=0;i<Stuart.getNumberOfChromosomes();i++){
		count += gamete1[i].countB();
		count += gamete2[i].countB();
	}

	if(count != Stuart.getBcount()){
		cerr << "Discrepancy between B count of all Chromosomes and Individual \n";
		return 1;
	}

	double total_number_of_loci = (Stuart.getNumberOfChromosomes() * Stuart.getNumberOfLoci(0));
	if((count / (total_number_of_loci * 2)) != Stuart.getBprop()){
		cerr << "Discrepancy between B prop of all Chromosomes and Individual \n";
		return 1;
	}

	if((double)count / total_number_of_loci != Stuart.getHetProp()){
		cerr << "Manually comuted heterozygocity: "
				<< count << " / " << total_number_of_loci << " = " << (double)count / total_number_of_loci
				<< " !=  heterozygocity from function: " << Stuart.getHetProp() << endl;
		return 1;
	}

	if( 0 != Stuart.Acheck()){
		cerr << "Error: Acheck have not passed \n";
		return 1;
	}

	if( 0 != Stuart.Bcheck()){
		cerr << "Warning: Bcheck have not passed. Very very unlikely scenario, rerun test. \n";
		return 1;
	}

	// number of junctions independently derived from every chromosome compared to number of juctions derived from the method of individual
	int number_junction = 0;
	for(int i=0;i<Stuart.getNumberOfChromosomes();i++){
		number_junction += gamete1[i].getNumberOfJunctions();
		number_junction += gamete2[i].getNumberOfJunctions();
	}

	if(Stuart.getNumberOfJunctions(0, 0) != gamete1[0].getNumberOfJunctions()){
		cerr << "Number of junctions of st chromosome and st haplotype does not match number of junction in first gamete." << endl;
		return 1;
	}

	if(Stuart.getNumberOfJunctions() != number_junction){
		cerr << "Total number of junctions does not match number of junction in first gamete." << endl;
		return 1;
	}

	return 0;
}


