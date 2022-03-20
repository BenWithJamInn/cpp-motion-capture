#pragma once

#define SAMPLE_PERIOD 3000 // in ms

void capture();
void saveSampleToCSV(float sampleData[][SAMPLE_PERIOD], int shapeWidth, int shapeLength, std::string columnNames[], std::string name);
