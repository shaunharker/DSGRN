import sys
import pdb

def main(resultsfile):
  parameternodes = []
  f=open(resultsfile,'r')
  for line in f:
    parameternodes.append(line.replace("\n",""))
  exit(parameternodes)

if __name__ == '__main__':
    main(sys.argv[1])