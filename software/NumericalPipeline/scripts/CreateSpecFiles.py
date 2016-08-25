#!/usr/bin/python

import json, sys

def CreateSpecFiles ( network_pattern_spec_file, network_spec_file, pattern_spec_file ):
  with open(network_pattern_spec_file) as infile:
    data = json.load(infile)
  with open(network_spec_file, 'w') as outfile:
    outfile.write(data["Network"])
  with open(pattern_spec_file, 'w') as outfile:
    json.dump(data["Pattern"], outfile)

if __name__ == "__main__":
  CreateSpecFiles(sys.argv[1], sys.argv[2], sys.argv[3])
