#pragma once
#include<iostream>
#include<vector>
#include <fstream>
#include<string>
#include <QVector3D>

void readFile(std::string fileName, std::vector<QVector3D>& points, std::vector<QVector3D>& normals, std::vector<QVector2D>& texCoords, std::vector<int>& indexPoints, std::vector<int>& indexNormals, std::vector<int>& indexTex )
{

    std::string line;
    std::string type = " ";
    std::ifstream myfile ("../stones/stones.obj");
    while (!myfile.eof())
    {
      while(type != "v" && type != "vn" && type != "vt" && type != "f" && !myfile.eof() )
      {
          myfile >> type;
          std::cout<<type<<std::endl;
      }
      while(type == "v")
      {
          std::cout<<type<<std::endl;
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
      while(type == "vt")
      {
          std::cout<<type<<std::endl;
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
      while(type == "vn")
      {
          std::cout<<type<<std::endl;
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
      if(type == "f" )
      {
          //std::cout<<type<<std::endl;
          int index;
          for(int count = 0;count < 9;count++)
          {
              myfile >> index;
              printf("%d\n ",index);
              //indexPoints.push_back(index);
              //myfile >> index;
              //indexNormals.push_back(index);
              //myfile >> index;
             // indexTex.push_back(index);
//              std::cout << indexPoints[indexPoints.size() - 1] << "/" << indexNormals[indexNormals.size() - 1]<< "/"<< indexTex[indexTex.size() - 1] <<std::endl;
          }
          if(!myfile.eof())
          {
              myfile >> type;
          }
          else
          {
              myfile.close();
          }
          //std::cout<<type<<std::endl;
      }
    }

}
