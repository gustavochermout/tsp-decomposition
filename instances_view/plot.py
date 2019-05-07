import matplotlib.pyplot as plt

file = '../in/eil51.in'  

with open(file) as f:  
    lines = f.readlines()
    size = int(lines[0])

    for i in range(1, size + 1):
        id, x, y = lines[i].split(" ")
        plt.plot(int(x), int(y), 'bo')

plt.show()