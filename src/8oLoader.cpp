#include <fstream>
#include <iostream>
#include <string>

int main() {
  std::ifstream inputFile("../programs/IBM.8o");
  if (!inputFile) {
    std::cerr << "Unable to open file" << std::endl;
    return 1;
  }

  std::string line;
  while (std::getline(inputFile, line)) {
    if (line[0] != '#')
      std::cout << line << std::endl;
  }

  inputFile.close();
  return 0;
}