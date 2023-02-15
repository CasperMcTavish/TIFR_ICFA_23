import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit

def read_input_data(filename):
    data = []
    with open(filename, "r") as f:
        for line in f:
            line = line.strip().split()
            data.append(int(line[0]))
    return np.array(data)

def exponential_decay(x, a, b, c):
    return a * np.exp(-b * x) + c

def fit_exponential_decay(data):
    x = np.arange(len(data))
    popt, pcov = curve_fit(exponential_decay, x, data, p0=[1, 0.1, 0])
    return popt

def calculate_muon_lifetime(popt):
    muon_lifetime = -1 / popt[1]
    return muon_lifetime

if __name__ == "__main__":
    input_data = read_input_data("muon_data.txt")
    popt = fit_exponential_decay(input_data)
    muon_lifetime = calculate_muon_lifetime(popt)
    print("Muon lifetime:", muon_lifetime)
    
    x = np.arange(len(input_data))
    y = exponential_decay(x, *popt)
    plt.plot(x, input_data, "o", label="Input Data")
    plt.plot(x, y, label="Exponential Decay Fit")
    plt.xlabel("Count Value")
    plt.ylabel("Frequency")
    plt.legend()
    plt.show()

