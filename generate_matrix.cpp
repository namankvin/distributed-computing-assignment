#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
using namespace std;
namespace fs = std::filesystem;

int getValue(int i, int j) {
    return (i * 31 + j * 17) % 1000;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage: ./generate_matrix <matrix_size> <block_size>\n";
        return 1;
    }
    int n = stoi(argv[1]), blockSize = stoi(argv[2]);
    if (n % blockSize != 0) {
        cout << "Matrix size must be divisible by block size.\n";
        return 1;
    }
    fs::create_directories("format_a");
    fs::create_directories("format_b");
    cout << "Generating matrix of size " << n << " x " << n << "\n";
    // FORMAT A: (i, j, value)
    cout << "Generating Format A...\n";
    ofstream foutA("format_a/matrix_entries.txt");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int value = getValue(i, j);
            foutA << i << "," << j << "," << value << '\n';
        }
    }
    foutA.close();
    cout << "Format A completed.\n";
    // FORMAT B: BLOCK BASED
    cout << "Generating Format B...\n";
    int blocksPerDimension = n / blockSize;
    for (int br = 0; br < blocksPerDimension; br++) {
        for (int bc = 0; bc < blocksPerDimension; bc++) {
            string filename = "format_b/block_" + to_string(br) + "_" + to_string(bc) + ".txt";
            ofstream fout(filename);
            int rowStart = br * blockSize;
            int colStart = bc * blockSize;
            for (int i = rowStart; i < rowStart + blockSize; i++) {
                for (int j = colStart; j < colStart + blockSize; j++) {
                    int value = getValue(i, j);
                    fout << value;
                    if (j != colStart + blockSize - 1)
                        fout << " ";
                }
                fout << '\n';
            }
            fout.close();
        }
    }

    cout << "Format B completed.\n";

    return 0;
}