import sys
import pdb

def main(resultsfile):
	parameternodes=[]
	f=open(resultsfile,'r')
		for line in f:
			splitted = line.split()
			if splitted[1][:-1] not in parameternodes:
				parameternodes.append(splitted[1][:-1].replace("\n",""))
  exit(parameternodes)

if __name__ == '__main__':
    main(sys.argv[1])