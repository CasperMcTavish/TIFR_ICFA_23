import matplotlib.pyplot as plt


def read_file(file_name):
    datas = []
    
    with open(str(file_name)) as r:
        for line in r:
            data = line.split(" ")
            strip = (x.strip() for x in data)
            strip = [x for x in strip if x]
            datas.append(strip)
    
    
    return datas

def plot_files(data, name, label):
    fig, (plot1, plot2, plot3, plot4) = plt.subplots(4,1)
    x = []
    chan_0 = []
    chan_1 = []
    chan_2 = []
    chan_3 = []
    for i in range(len(data)):
        x.append(float(data[i][0]))
        chan_0.append(float(data[i][1]))
        chan_1.append(float(data[i][2]))
        chan_2.append(float(data[i][3]))
        chan_3.append(float(data[i][4]))
   
    plt.xlabel(str(label[0]))
    plt.ylabel(str(label[1]))
    plot1.scatter(x, chan_0, label = "chan_1")
    plot1.legend(loc = "upper left")
    plot2.scatter(x, chan_1, label = "chan_2")
    plot2.legend(loc = "upper left")
    plot3.scatter(x, chan_2, label = "chan_3")
    plot3.legend(loc = "upper left") 
    plot4.scatter(x, chan_3, label = "chan_4")
    plot4.legend(loc = "upper left")
    #plt.gca().invert_yaxis()
   
    plt.show()

def main():
    
   data = read_file('position_data.txt')
   #print(data)
   plot_files(data, "position against count", ["position","count"])

main()
