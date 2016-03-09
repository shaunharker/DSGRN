import sys
import pdb

def main():
	resultsfile = '/home/mike/conley3_outputs/5D_2016_01_28_stableFCs_allresults.txt'
	parameters=[]
	savein = '/home/mike/5D_2016_01_28_pnMG2.txt'
	savein2 = '/home/mike/5D_2016_01_28_parameters2.txt'
	file_ = open(savein, 'w')
	file2 = open(savein2, 'w')
	f=open(resultsfile,'r')
 	for line in f:
		splitted = line.split()
		if splitted[1][:-1] not in parameters:
			parameters.append(splitted[1][:-1])
            file_.write(str(splitted[4][:-1]) + ' ' + str(splitted[1][:-1]) + "\n")
			file2.write(str(splitted[1][:-1]) + "\n")
	file_.close()
	file2.close()


if __name__ == '__main__':
    main()