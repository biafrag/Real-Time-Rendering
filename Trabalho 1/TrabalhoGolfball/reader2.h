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
          float number;
          myfile >> number;
          point.setX(number);
          myfile >> number;
          point.setY(number);
          myfile >> number;
          point.setZ(number);
          //std::cout << point.x() << " " << point.y() << " "<< point.z() << " " << std::endl;
          points.push_back(point);
          myfile >> type;
      }
      else if(type == "vt")
      {
          QVector2D tex;
          float number;
          myfile >> number;
          tex.setX(number);
          myfile >> number;
          tex.setY(number);
          //std::cout << tex.x() << " " << tex.y() << std::endl;
          texCoords.push_back(tex);
          myfile >> type;
      }
      else if(type == "vn")
      {
          QVector3D normal;
          float number;
          myfile >> number;
          normal.setX(number);
          myfile >> number;
          normal.setY(number);
          myfile >> number;
          normal.setZ(number);
         // std::cout << normal.x() << " " << normal.y() << " "<< normal.z() << " " << std::endl;
          normals.push_back(normal);
          myfile >> type;
      }
      else if(type == "t")
      {
          QVector3D tangent;
          float number;
          myfile >> number;
          tangent.setX(number);
          myfile >> number;
          tangent.setY(number);
          myfile >> number;
          tangent.setZ(number);
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

