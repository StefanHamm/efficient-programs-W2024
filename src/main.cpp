// include the necessary header files
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>


//main function which uses 4 path as arguments and calls the join funciton

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        std::cerr << "Usage: " << argv[0] << " <path1> <path2> <path3> <path4>" << std::endl;
        return 1;
    }

    std::string path1 = argv[1];
    std::string path2 = argv[2];
    std::string path3 = argv[3];
    std::string path4 = argv[4];

    std::string result = join(path1, path2, path3, path4);

    std::cout << result << std::endl;

    return 0;
}