import numpy as np
from scipy.optimize import curve_fit
from matplotlib import pyplot
import matplotlib.pyplot as plt
import sys
import warnings
from math import sqrt

#pass the data filename as a command line argument
n = len(sys.argv)
if(n < 2):  # check if the filename is passed ot not
    print("Too few arguments, usage: fit_muon_life <data filename>")
    exit()

print("Filename passed:", sys.argv[1])

# define the true objective function - the fit function
def objective(x, a, b, c):
    return a * np.exp(-b * x) + c

# suppress the warning "RuntimeWarning: Overflow encountered in exp"
warnings.filterwarnings('ignore')

# load the data
File_data = np.loadtxt(sys.argv[1], usecols=0, dtype=int)
print("Total Entries", len(File_data))

# Remove all the zeroes from the data.
# Double muons coming within 100 ns cause a zero to be
# registered since te system least count is 100 ns
indices = np.where(File_data == 0)
data = np.delete(File_data, indices)

print("Entries after zero suppression", len(data))

# make the histogram with auto binning: Auto binning works the best
counts, binEdges, patches = plt.hist(data, bins='auto')

bin_count = len(binEdges)
print("No of Bins:", bin_count)

# choose the input and output variables
x, y = binEdges[0:bin_count-1], counts
# curve fitting
popt, pcov = curve_fit(objective, x, y)
# summarize the parameter values
a, b, c = popt
print("Fit Parameters:", popt)

# Get the standard error of the fit parameters
perr = np.sqrt(np.diag(pcov))
print("Fit Parameter Errors:", perr)

# calculate the muon lifetime
t = 100e-9/b * 1e6 #Get value in microsencods
print("Muon Lifetime:", t, "μs")

# calculate the error in muon lifetime
d_t = 100e-9 * perr[1] / b**2 * 1e6 #Get value in microsencods
print("Error in Muon Lifetime:", d_t, "μs")

# plot the fit curve
# calculate the output for the range
y_line = objective(x, a, b, c )
# create a line plot for the mapping function
plt.plot(x, y_line, '--', color='red')
#plt.yscale("log")
plt.title(str("Muon Lifetime τ = " + str(round(t, 4)) + " ± " + str(round(d_t, 4)) + " μs"))
plt.xlabel("Time in μs")
plt.ylabel("Counts")

# create plot info area
xpos = int(x[-1])
ypos = int(y_line.max())
entries_str = f"Total Entries: {len(data)}"
a_str = f"a = {round(a, 3)}"
b_str = f"b = {round(b, 3)}"
c_str = f"c = {round(c, 3)}"
t_str = f"t = {round(t, 3)}"
d_t_str = f"d_t = {round(d_t, 3)}"
ystep = ypos/20
plt.text(xpos, ypos-ystep*0, entries_str, ha='right')
plt.text(xpos, ypos-ystep*1, "Fit function: a * np.exp(-b * x) + c", ha='right' )
plt.text(xpos, ypos-ystep*2, a_str, ha='right')
plt.text(xpos, ypos-ystep*3, b_str, ha='right')
plt.text(xpos, ypos-ystep*4, c_str, ha='right')
plt.text(xpos, ypos-ystep*5, t_str, ha='right')
plt.text(xpos, ypos-ystep*6, d_t_str, ha='right')
# finally draw the plot
plt.show()

