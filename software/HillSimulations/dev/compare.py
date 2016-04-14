import pdb
import sys

def main():

    f1='/home/mike/timeseries_outputs/5D_2016_01_28/5D_2016_01_28_FC_test.txt'
    f2='/home/mike/conley3_outputs/5D_2016_01_28_stableFCs_allresults.txt'
    f3='/home/mike/5D_compare_n10_add.txt'
    output = open(f1, 'r')
    original =open(f2,'r')
    new = open(f3,'w')

    original_dict={}
    for line in original:
      splitted = line.split()
      extrema = splitted[9:-2]
      new_extrema = []
      for i in range(len(extrema)-1):
        if 'm' in extrema[i+1]:
          new_extrema.append(extrema[i]+extrema[i+1])
      if splitted[1][:-1] in original_dict: #don't use splitted[9:] because last one repeats 1st, messes us wrap around eval below
        b=[x.replace('[','').replace(',','') for x in new_extrema]
        original_dict[splitted[1].replace(',','')].append(b)
      else:
        b=[x.replace('[','').replace(',','') for x in new_extrema]
        original_dict[splitted[1].replace(',','')] = [b]

    for line in output:
      newline0 = line.split()
      new_line = []
      matched_pattern = ''
      for i in range(len(newline0)-1):
        if 'm' in newline0[i+1]:
          new_line.append((newline0[i]+newline0[i+1]))
      compare = 'match'
      output_pattern = [x.replace(',','') for x in new_line]
      pattern_list = original_dict[newline0[0].replace(',','')]
      for pattern in pattern_list:
        first = output_pattern[0]  #evaluate as both lists wrapping around. eval backwards (0, -1, ...)
        j=-1    
        for i in range(len(pattern)): 
          if pattern[i] == first:    
            k = i-1 #do not use i+1 or else while loop would end sooner than needed
            break
        while pattern[k] != first:
          if output_pattern[j] != pattern[k]:
            compare = 'no match'
            break
          k=k-1
          j=j-1
        if compare == 'match':
          matched_pattern = pattern
          break
        else:
          matched_pattern = pattern
      new.write(line.split()[0][:-1]+": "+compare+"\n")
      if compare == 'no match':
        new.write('last experimental pattern compared to: '+str(matched_pattern)+"\n")
      else:
        new.write('experimental: '+str(matched_pattern)+"\n")
      new.write('output: '+' '.join(new_line)+"\n")
      new.write("\n")
    output.close()
    original.close()
    new.close()

if __name__ == '__main__':
    main()

