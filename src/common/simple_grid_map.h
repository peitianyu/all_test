#ifndef __SIMPLE_GRID_MAP_H__
#define __SIMPLE_GRID_MAP_H__

#include "read_file.h"
#include <vector>

struct GridMap
{
    int width;
    int height;
    bool* data;

    GridMap(int width, int height)
    {
        this->width = width;
        this->height = height;
        data = new bool[width * height];
    }

    GridMap(const std::string& file_name)
    {
        ReadFile rf(file_name);

        std::vector<std::vector<bool>> data;
        for(int i = 0; !rf.eof(); ++i) {
            std::string line = rf.get_line();
            std::vector<bool> row;
            for(uint j = 0; j < line.size(); ++j) 
                row.push_back(line[j] == '1');
            data.push_back(row);
        }

        width = data[0].size();
        height = data.size();
        this->data = new bool[width * height];
        for(int i = 0; i < height; ++i) {
            for(int j = 0; j < width; ++j){
                set_val(j, i, data[i][j]);
            }
        }
    }

    ~GridMap()
    {
        delete[] data;
    }

    void set_val(int x, int y, bool val)
    {
        if(!is_in(x, y)) return;

        data[y * width + x] = val;
    }

    bool is_occ(int x, int y) const
    {
        // FIXME: 假假的写一个
        if(!is_in(x, y)) return true;

        return data[y * width + x];
    }

    bool is_in(int x, int y) const
    {
        return x >= 0 && x < width && y >= 0 && y < height;
    }
};

#endif // __SIMPLE_GRID_MAP_H__