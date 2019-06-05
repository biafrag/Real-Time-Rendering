#pragma once
#include<iostream>
#include<vector>
#include <fstream>
#include<string>
#include <QVector3D>
#include <sstream>
void readFile(std::string fileName, std::vector<QVector3D>& points, std::vector<QVector3D>& normals,std::vector<QVector3D>& tangents, std::vector<QVector2D>& texCoords, std::vector<int>& indexPointsTriangles)
{

    int cont = 0;
    std::string line;
    std::string type = " ";
    std::ifstream myfile (fileName);
    while (1)
    {
      if(myfile.eof())
      {
          break;
      }

      else if(type != "v" && type != "vn" && type != "vt" && type != "t" && type != "f")
      {
          myfile >> type;
      }
      else if(type == "v")
      {
          QVector3D point;
           myfile >> point[0] >> point[1] >> point[2];
          //std::cout << point.x() << " " << point.y() << " "<< point.z() << " " << std::endl;
          points.push_back(point);
          myfile >> type;
      }
      else if(type == "vt")
      {
          QVector2D tex;
          myfile >> tex[0] >> tex[1];
          //std::cout << tex.x() << " " << tex.y() << std::endl;
          texCoords.push_back(tex);
          myfile >> type;
      }
      else if(type == "vn")
      {
          QVector3D normal;
          myfile >> normal[0] >> normal[1] >> normal[2];
         // std::cout << normal.x() << " " << normal.y() << " "<< normal.z() << " " << std::endl;
          normals.push_back(normal);
          myfile >> type;
      }
      else if(type == "t")
      {
          QVector3D tangent;
          myfile >> tangent[0] >> tangent[1] >> tangent[2];
         // std::cout << normal.x() << " " << normal.y() << " "<< normal.z() << " " << std::endl;
          tangents.push_back(tangent);
          myfile >> type;
      }
      else if(type == "f" )
      {
          int ind;
          myfile >> ind;
          indexPointsTriangles.push_back(ind);
          myfile >> type;
      }
    }
    myfile.close();
}


