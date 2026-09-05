#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <vector>

using namespace std;
namespace fs = std::filesystem;

long long getValue(long long i, long long j) {
    return (i * 31 + j * 17) % 1000;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: ./verify_transpose <output_dir>\n";
        return 1;
    }

    string outputDir = argv[1];
    if (!fs::exists(outputDir) || !fs::is_directory(outputDir)) {
        cerr << "Directory not found: " << outputDir << "\n";
        return 1;
    }

    long long totalBlocks = 0;
    long long totalElements = 0;
    long long mismatches = 0;

    for (const auto& entry : fs::directory_iterator(outputDir)) {
        string filename = entry.path().filename().string();

        if (filename.empty() || filename[0] == '.' || filename[0] == '_') {
            continue;
        }

        string cleanName = filename;
        size_t txtIdx = cleanName.find(".txt");
        if (txtIdx != string::npos) {
            cleanName = cleanName.substr(0, txtIdx);
        }
        size_t dashIdx = cleanName.find("-r-");
        if (dashIdx != string::npos) {
            cleanName = cleanName.substr(0, dashIdx);
        }

        if (cleanName.rfind("block_", 0) != 0) {
            continue;
        }

        string coords = cleanName.substr(6);
        size_t underscore = coords.find('_');
        if (underscore == string::npos) continue;

        int bc = stoi(coords.substr(0, underscore));
        int br = stoi(coords.substr(underscore + 1));

        ifstream fin(entry.path());
        if (!fin.is_open()) continue;

        string line;
        int localRow = 0;
        int blockSize = -1;

        while (getline(fin, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            int val;
            vector<int> rowValues;
            while (ss >> val) {
                rowValues.push_back(val);
            }

            if (blockSize == -1) {
                blockSize = rowValues.size();
            }

            int localCol = 0;
            for (int actualVal : rowValues) {
                long long globalI = (long long)bc * blockSize + localRow;
                long long globalJ = (long long)br * blockSize + localCol;
                long long expectedVal = getValue(globalJ, globalI);

                if (actualVal != expectedVal) {
                    mismatches++;
                }
                totalElements++;
                localCol++;
            }
            localRow++;
        }
        fin.close();
        totalBlocks++;
    }

    cout << "Blocks verified   : " << totalBlocks << "\n";
    cout << "Elements verified : " << totalElements << "\n";
    cout << "Mismatches        : " << mismatches << "\n";

    if (mismatches == 0 && totalElements > 0) {
        cout << "Result            : SUCCESS\n";
        return 0;
    } else {
        cout << "Result            : FAILED\n";
        return 1;
    }
}
