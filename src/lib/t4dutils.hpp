#include <iostream>
#include <string>
#include <vector>

float splitterT4D(std::string myString, int piece, char splitchar) {
    int splitcount = 0;
    //std::cout << myString[0] << "\n";
    int pos = 0;
    int splits[5];

    for (int i = 0; i < myString.length(); i++) {
        if (myString[i]==splitchar) {
            //std::cout << i << "\n";
            splits[splitcount] = i+1;
            splitcount++;
        }
    }
    return strtof( myString.substr(splits[piece], splits[piece+1]-splits[piece]-1).c_str(), 0);
}

std::string splitterT4Dd(std::string myString, int piece, char splitchar)
{
    int splitcount = 0;
    // std::cout << myString[0] << "\n";
    int pos = 0;
    int splits[5];

    for (int i = 0; i < myString.length(); i++)
    {
        if (myString[i] == splitchar)
        {
            // std::cout << i << "\n";
            splits[splitcount] = i + 1;
            splitcount++;
        }
    }
    return myString.substr(splits[piece], splits[piece + 1] - splits[piece] - 1).c_str();
}

std::string splitterT4Ds(std::string myString, int piece, char splitchar)
{
    int splitcount = 0;
    // std::cout << myString[0] << "\n";
    int pos = 0;
    int splits[5];

    for (int i = 0; i < myString.length(); i++)
    {
        if (myString[i] == splitchar)
        {
            // std::cout << i << "\n";
            splits[splitcount] = i + 1;
            splitcount++;
        }
    }
    return myString.substr(splits[piece], splits[piece + 1] - splits[piece] - 1);
}

class splitterFaces4D
{
    private:
        
    public:
        std::vector<int> vecs;
        int vertCounts[5] = {1, 1, 1, 1, 1};
        void parse(std::string myString, int face, int vert){
            int facePos = 0;
            for (int i = 0; i < 4; i++) {
                std::string yo = "/"+splitterT4Ds(myString, i, ' ');
                std::clog << yo << ' ';
                std::clog << splitterT4D(yo, 0, '/') << "\n";
                vecs.push_back(splitterT4D(yo, 0, '/'));
            }
    }

};