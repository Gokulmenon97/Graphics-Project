#include "HeightMap.h"
#include <iostream>

HeightMap::HeightMap(const std::string& name) {
	int iWidth, iHeight, iChans;
	unsigned char* data = SOIL_load_image(name.c_str(), &iWidth, &iHeight, &iChans, 1);

	if (!data) {
		std::cout << " Heightmap can't load file !\n";
		return;
	}

	numVertices = iWidth * iHeight;
	numIndices = (iWidth - 1) * (iHeight - 1) * 6;
	vertices = new Vector3[numVertices];
	textureCoords = new Vector2[numVertices];
	indices = new GLuint[numIndices];

	Vector3 vertexScale = Vector3(1.0f, 0.025f, 1.0f);
	Vector2 textureScale = Vector2(1/16.0f, 1/16.0f);

	for (int z = 0; z < iHeight; ++z)
	{
		for (int x = 0; x < iWidth; ++x)
		{
			int offset = (z * iWidth) + x;
			vertices[offset] = Vector3(x, data[offset], z) * vertexScale;
			textureCoords[offset] = Vector2(x, z) * textureScale;
		}
	}
	SOIL_free_image_data(data);

	int i = 0;
	for (int z = 0; z < iHeight - 1; ++z) {
		for (int x = 0; x < iWidth - 1; ++x) {
			int a = (z * (iWidth)) + x;
			int b = (z * (iWidth)) + (x + 1);
			int c = ((z + 1) * (iWidth)) + (x + 1);
			int d = ((z + 1) * (iWidth)) + x;
			indices[i++] = a;
			indices[i++] = c;
			indices[i++] = b;
			indices[i++] = c;
			indices[i++] = a;
			indices[i++] = d;
		}
	}

	GenerateNormals();
	GenerateTangents();
	BufferData();

	heightmapSize.x = vertexScale.x * (iWidth - 1);
	heightmapSize.y = vertexScale.y * 255.0f;// each height is a byte !
	heightmapSize.z = vertexScale.z * (iHeight - 1);

	for (int i = 0; i < numVertices; i++)
	{
		if (vertices[i].y >= heightmapSize.y)
		{
			indexOfGroundPos.emplace_back(i);
		}
		else
		{
			indexOfUnderGroundPos.emplace_back(i);
		}
	}

	std::cout << "\nNumber of Indices for Ground Area :: " << indexOfGroundPos.size() << "\n";
	std::cout << "\nNumber of Indices for Under_Ground Area :: " << indexOfUnderGroundPos.size() << "\n";
}

vector<int> HeightMap::GetIndexOfGround()
{
	return indexOfGroundPos;
}

vector<int> HeightMap::GetIndexOfUnderGround()
{
	return indexOfUnderGroundPos;
}

vector<Vector3> HeightMap::GetRandPositionOnGround()
{
	srand((unsigned int)time(0));

	vector<int> randPos;
	vector<int> tempPos;

	for (int i = 0; i < 30000; i++)
	{
		bool isAdded = false;

		while (isAdded == false)
		{
			if (randPos.empty() && tempPos.empty())
			{
				int randtemp = rand() % indexOfGroundPos.size();
				randPos.emplace_back(indexOfGroundPos[randtemp]);
				tempPos.emplace_back(randPos[i]);
				isAdded = true;
			}
			else if (!randPos.empty() && !tempPos.empty())
			{
				int samePos = 0;
				int randtemp = rand() % indexOfGroundPos.size();
				randPos.emplace_back(indexOfGroundPos[randtemp]);

				for (int j = 0; j < tempPos.size(); j++)
				{
					if (randPos[i] == tempPos[j])
					{
						samePos++;
					}
					else
					{
						
					}
				}
				if (samePos == 0)
				{
					tempPos.emplace_back(randPos[i]);
					isAdded = true;
				}
				else if(samePos > 0)
				{
					randPos.pop_back();
				}
			}
		}
	}
	
	vector<Vector3> GeneratedRandPositions;

	for (int i = 0; i < randPos.size(); i++)
	{
		GeneratedRandPositions.emplace_back(vertices[randPos[i]]);
	}

	return GeneratedRandPositions;

}