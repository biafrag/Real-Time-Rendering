#pragma once
#include<iostream>
#include<vector>
#include <fstream>
#include<string>
#include <QVector3D>
#include <sstream>
void readFile(std::string fileName, std::vector<QVector3D>& points, std::vector<QVector3D>& normals, std::vector<QVector2D>& texCoords, std::vector<int>& indexPointsTriangles,std::vector<int>& indexPointsQuads, std::vector<int>& indexNormalsTriangles, std::vector<int>& indexTexTriangles, std::vector<int>& indexNormalsQuads,std::vector<int>& indexTexQuads)
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
          //std::cout << point.x() << " " << point.y() << " "<< point.z() << " " << std::endl;
          std::cout << point.length() << std::endl;
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
      else if(type == "f" )
      {
          //std::cout<<type<<std::endl;
          std::string s;
          std::string aux;
          std::vector<int> v;
          v.resize(12);
          std::string auxtype ;
          myfile >> auxtype;
          int cont = 0;
          while(auxtype != "f")
          {
              if(myfile.eof())
              {
                  break;
              }
              std::istringstream f(auxtype);
              while (getline(f, s, '/'))
              {
                      v[cont] = std::atoi(s.c_str());
                      cont++;
              }
              //std::cout << v[0] << " " << v[1] << " "<< v[2] << " " << std::endl;
              myfile >> auxtype;
          }
          type = auxtype;
          if(cont == 12)
          {
              for(int i = 0; i < 4 ; i++)
              {
                  indexPointsQuads.push_back(v[3*i] - 1);
                  indexTexQuads.push_back(v[3*i + 1] - 1);
                  indexNormalsQuads.push_back(v[3*i + 2] - 1);
              }
          }
          else
          {
              for(int i = 0; i < 3 ; i++)
              {
                  indexPointsTriangles.push_back(v[3*i] - 1);
                  indexTexTriangles.push_back(v[3*i + 1] - 1);
                  indexNormalsTriangles.push_back(v[3*i + 2] - 1);
              }
          }
          std::cout <<std::endl;
      }
      //cont++;
    }
    myfile.close();
}

