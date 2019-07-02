#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <fstream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

using namespace std;

// This function writes the 2D array data to file
template<typename t_data>
void write_csv(vector<vector<t_data>> a, int nrows, int ncols, std::ostream &outs)
{
    for (int row = 0; row < nrows; row++) {
        for (int col = 0; col < ncols; col++) {
            outs << a[row][col];
            if (col < ncols - 1)
                outs << ",";
        }
        outs << std::endl;
    }
}

#endif // UTIL_H
