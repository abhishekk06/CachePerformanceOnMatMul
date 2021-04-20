import random
import argparse
import os

parser = argparse.ArgumentParser(description='Input Matrix generator')
parser.add_argument('--seed', type=int, default=0, help='Seed Value')
parser.add_argument('--n', type=int, default=3, help='Matrix order(nxn)')
parser.add_argument('--dump', type=str, default='input_matrix.in', help='File name')


def createRandomMatrix(n):
    maxVal = 1000  # I don't want to get Java / C++ into trouble
    matrix = []
    for i in range(n):
        matrix.append([random.randint(0, maxVal) for el in range(n)])
    return matrix


def saveMatrix(matrixA, matrixB, filename):
    if os.path.exists(filename): 
        os.remove(filename)
    else:   
        print("New file created: ",filename)
    f = open(filename, "w")
    for i, matrix in enumerate([matrixA, matrixB]):
        if i != 0:
            f.write("\n")
        for line in matrix:
            f.write("\t".join(map(str, line)) + "\n")

def main():
    global args
    args = parser.parse_args()
    print(40*"="+"\nArgs:{}\n".format(args)+40*"=")
    random.seed(args.seed)
    n = args.n
    outpath = args.dump
    matrixA = createRandomMatrix(n)
    matrixB = createRandomMatrix(n)
    saveMatrix(matrixA, matrixB, args.dump)

if __name__ == '__main__':
    main()
