#pragma once
#include <string>
#include "Mesh.h"
#include <time.h>

class HeightMap : public Mesh {
public:
	HeightMap(const std::string &name);
	~HeightMap(void) {};
	Vector3 GetHeightmapSize() const { return heightmapSize; }
	vector<Vector3> GetRandPositionOnGround();
	vector<int> GetIndexOfGround();
	vector<int> GetIndexOfUnderGround();

protected:
	vector<int> indexOfGroundPos;
	vector<int> indexOfUnderGroundPos;
	Vector3 heightmapSize;
};