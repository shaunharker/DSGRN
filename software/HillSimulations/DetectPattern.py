# The MIT License (MIT)

# Copyright (c) 2016 Michael Lan

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.


# NOTE: Work in progress.

    def checkForOscillations(self,savein,parameterstring,timeseries):
        """
        Checks if all genes are in desired oscillation:
        For each gene's function, take its end slice, assuming that at this end slice the function would have already settled into the 
        desired behavior. Going from left to right in the end slice, record each max/min based on whether the area around that point 
        changes its direction of increase or decrease (the basic definition of an extrema point; checking increasing or decreasing is done 
        just by seeing if adjacent points are bigger/smaller). 

        After this is done, it checks if the global max and min are within a degree of error to one another. If they are, that function 
        has probably gone to a fixed point, so conclude the function is not oscillating. Since we assume that we only want one unique 
        max and min in this end slice, if there are too many non-unique extrema (outliers), then the behavior of that function
        is not considered a 'desired oscillation'; same with comparing each max with the global max. Later, another add-on to the method
        may be: If periods (distance b/w max/min) do not match, that function may also be ruled out as a 'desired oscillation'.
        
        Finally, record the order the extrema appear at and puts the ordering in a list 'output', which is saved in a file acc. to path 'savein'

        So far this new method has been tested on the parameter node '923669' and found all its genes are oscillating. The extrema
        ordering that this method outputs also matches one of the experimental extrema ordering patterns

        Structure of list 'timeseries':
        timeseries[time][value of each gene at time]

        e.g. timeseries[5] gives [0.5,0.4,0.3], which is a list of the values of each gene at time=5. Each gene is indexed by an 
        integer. For instance, p53 is indexed as '1', so timeseries[5][1] would correspond to p53's value
        This method relies on several assumptions, namely: We only desire oscillations with consistent amplitudes and periods. In other 
        words, all the max in the end behavior of the function are equal, and same with the min.
        """
        output=[]
        output.append(parameterstring)
        time_of_extrema={} #contains the extrema of every single gene, stored in the format time:k max or min. Repeated extrema are included.
        numOfGenes=0 #records how many genes are oscillating
        for k in range(len(self.varnames)):
          #the first check to make sure function didn't go to fixed point. If fails, avoid performing lengthy loop on end slice of gene's function  
          #uses 'or' because the function's period may be at 10 timesteps; if so, it won't be at 15 timesteps. Prevents eval oscillations as fixed points
          if abs(timeseries[-1000:][time][k] - timeseries[-990:][time][k]) > 10**-3 or abs(timeseries[-1000:][time][k] - timeseries[-985:][time][k]) > 10**-3: 
              extrema_order_k={} #records all extrema for gene k in time:value format
              list_of_max=[] #to be used to find # of outliers
              list_of_min=[]
              slope = ''
              for i in range(999):
                if slope == 'decreasing' and (timeseries[-1000+i][k] - timeseries[-1000+i+1][k])<0:
                  extrema_order_k[-1000+i] = timeseries[-1000+i][k]
                  list_of_min.append(timeseries[-1000+i][k])
                  slope = 'increasing'
                elif slope == 'increasing' and (timeseries[-1000+i][k] - timeseries[-1000+i+1][k])>0:
                  extrema_order_k[-1000+i] = timeseries[-1000+i][k]
                  list_of_max.append(timeseries[-1000+i][k])
                  slope = 'decreasing'
                elif (timeseries[-1000+i][k] - timeseries[-1000+i+1][k])>0 and slope == '':
                  slope = 'decreasing'
                elif (timeseries[-1000+i][k] - timeseries[-1000+i+1][k])<0 and slope == '':
                  slope = 'increasing'
              
              #loop through and compare each max/min to the max/min. If too many outlier extrema, reject the oscillation as 'undesired'
              list_of_normals_max=[] #max values which are not outliers relative to other max values
              for i in range(len(list_of_max)):
                    num_of_differnces = 0 #how many times is max[i] different than other max, max[j]?
                    for j in range(len(list_of_max)):
                      if abs(list_of_max[i] - list_of_max[j]) > 10**-3: #if True, one of the two is outlier
                        num_of_differnces += 1
                    if num_of_differences < 3: #not 'if num_of_differences==0' b/c it will be diff. than an outlier; allow at most 2 outliers
                      list_of_normals_max.append(list_of_max[i])
              list_of_normals_min=[]
              for i in range(len(list_of_min)):
                    num_of_differnces = 0 #how many times is max[i] different than other max, max[j]?
                    for j in range(len(list_of_min)):
                      if abs(list_of_min[i] - list_of_min[j]) > 10**-3: #if True, one of the two is outlier
                        num_of_differnces += 1
                    if num_of_differences < 3:
                      list_of_normals_min.append(list_of_min[i])

              if len(list_of_max)-len(list_of_normals_max) < 2 and len(list_of_min)-len(list_of_normals_min) <2:
                values=extrema_order_k.values()
                global_max = max(values)
                global_min = min(values)
                if abs(global_max - global_min) > 10**-3: #second check: if True, function even more likely didn't go to a fixed point
                  numOfGenes+=1
                  for time in extrema_order_k:
                    if extrema_order_k[time] in list_of_normals_max:
                      time_of_extrema[time] = str(k)+' max' #if extrema close to global max, add it to outputs
                    if extrema_order_k[time] in list_of_normals_min:
                      time_of_extrema[time] = str(k)+' min'
        if numOfGenes == self.dim():      #Checks if all genes oscillated
          #rearrange all extrema of every gene according to where they are in time
          ordered_extrema = collections.OrderedDict(sorted(time_of_extrema.items())) 
          for time in ordered_extrema:
            if ordered_extrema[time] not in output: #checks for the cyclic pattern. Repeated extrema not included.
              output.append(ordered_extrema[time])
          for j in range(len(output)): #replace index number, used in timeseries, with parameter's name
            extrema = output[j]
            extrema_list = extrema.split()
            for i in range(len(extrema_list)):
              for k,v in enumerate(self.varnames):
                if extrema_list[i] == str(k):       
                  extrema_list[i] = v
            output[j] = (' '.join(extrema_list))
          file = open(savein, 'a')
          file.write(str(output).replace('[','').replace(']','').replace("'","")+"\n")
          file.close()
          