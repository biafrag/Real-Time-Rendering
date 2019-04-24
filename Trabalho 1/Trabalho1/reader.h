#pragma once
#include<iostream>
#include<vector>
#include <fstream>
#include<string>
#include <QVector3D>
#include <sstream>
void readFile(std::string fileName, std::vector<QVector3D>& points, std::vector<QVector3D>& normals, std::vector<QVector2D>& texCoords, std::vector<int>& indexPoints, std::vector<int>& indexNormals, std::vector<int>& indexTex )
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

      else if(type != "v" && type != "vn" && type != "vt" && type != "f")
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
          std::cout << point.x() << " " << point.y() << " "<< point.z() << " " << std::endl;
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
          std::cout << tex.x() << " " << tex.y() << std::endl;
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
          std::cout << normal.x() << " " << normal.y() << " "<< normal.z() << " " << std::endl;
          normals.push_back(normal);
          myfile >> type;
      }
      else if(type == "f" )
      {
          std::cout<<type<<std::endl;
          std::string s;
          std::string aux;
          std::vector<int> v;
          v.resize(3);
          for(int count = 0;count < 3;count++)
          {

              myfile >> aux;
              std::istringstream f(aux);
              int cont = 0;
              while (getline(f, s, '/'))
              {
                      v[cont] = std::atoi(s.c_str());
                      cont++;
              }
              indexPoints.push_back(v[0]);
              indexTex.push_back(v[1]);
              indexNormals.push_back(v[2]);
              std::cout << indexPoints[indexPoints.size() - 1] << " " << indexTex[indexTex.size() - 1] << " "<< indexNormals[indexNormals.size() - 1] << " " << std::endl;
          }
          std::cout <<std::endl;
          myfile >> type;
      }
      cont++;
    }

}
