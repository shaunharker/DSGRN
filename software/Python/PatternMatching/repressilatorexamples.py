from subprocess import call
import os
import patternmatch as pm

def onewayforcing():
    call(["dsgrn network "+os.path.expanduser("networks/6D_OneWayForcing.txt")+" analyze morseset 4 1213 >dsgrn_output.json"],shell=True)
    f=open('patterns.txt','w')
    f.write('X1 max, X2 max, Y1 min, Y2 min, Z1 max, Z2 max, X1 min, X2 min, Y1 max, Y2 max, Z1 min, Z2 min\nX1 max, Y1 min, Z1 max, X1 min, Y1 max, Z1 min\nX2 max, Y2 min, Z2 max, X2 min, Y2 max, Z2 min')
    f.close()

def twowayforcing():
    call(["dsgrn network "+os.path.expanduser("networks/6D_TwoWayForcing.txt")+" analyze morseset 4 28015 >dsgrn_output.json"],shell=True)
    f=open('patterns.txt','w')
    f.write('X1 max, X2 max, Y1 min, Y2 min, Z1 max, Z2 max, X1 min, X2 min, Y1 max, Y2 max, Z1 min, Z2 min\nX1 max, Y1 min, Z1 max, X1 min, Y1 max, Z1 min\nX2 max, Y2 min, Z2 max, X2 min, Y2 max, Z2 min')
    f.close()

def search(forcing=1):
    if forcing==1:
        onewayforcing()
    elif forcing==2:
        twowayforcing()
    else:
        print "Unrecognized input argument to search(). Aborting."
        return None
    pm.callPatternMatch()

if __name__=='__main__':
    paramsForTwoWayForcing=[4695, 4707, 4903, 4905, 4909, 4911, 4918, 4921, 4923, 4924, 5017, 5019, 5029, 5031, 5343, 5355, 5667, 6199, 6201, 6205, 6207, 6211, 6214, 6217, 6219, 6220, 6313, 6315, 6325, 6327, 6849, 6855, 6867, 6963, 6975, 16359, 16371, 16575, 16587, 16683, 16695, 17007, 17019, 17331, 17863, 17865, 17866, 17871, 17872, 17875, 17878, 17883, 17884, 17979, 17980, 17991, 17992, 18511, 18513, 18514, 18519, 18520, 18523, 18531, 18532, 18627, 18628, 18639, 18640, 28015, 28017, 28021, 28023, 28231, 28233, 28237, 28239, 28246, 28249, 28251, 28252, 28671, 28989, 28995, 29311, 29313, 29317, 29319, 29527, 29529, 29533, 29535, 29539, 29542, 29545, 29547, 29548, 29961, 29967, 30177, 30183, 30285, 30291, 39681, 39687, 39897, 39903, 39915, 40335, 40653, 40659, 40977, 40978, 40983, 40984, 41191, 41193, 41194, 41199, 41200, 41203, 41206, 41211, 41212, 41625, 41626, 41631, 41632, 41839, 41841, 41842, 41847, 41848, 41851, 41859, 41860, 41949, 41955]
    search(2)