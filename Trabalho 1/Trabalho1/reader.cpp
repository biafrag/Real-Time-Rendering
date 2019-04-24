#include "reader.h"
void readFile(/*std::string fileName, std::vector<QVector3D>& points, std::vector<QVector3D>& normals, std::vector<QVector3D>& indexes, std::vector<QVector2D>& texCoords*/)
{

    std::string line;
    char type;
    std::ifstream myfile ("../golfball/golfball.obj");
    if (myfile.is_open())
    {
      type = 'z';
      while ( getline (myfile,line) && type != 'v' )
      {
          myfile >> type;
      }
      while(type == 'v')
      {
          QVector3D point;
          std::string f;
          float number;
          myfile >> f;
          number = std::stof(f.c_str());
          point.setX(number);
          myfile >> f;
          number = std::stof(f.c_str());
          point.setY(number);
          myfile >> f;
          number = std::stof(f.c_str());
          point.setZ(number);
          myfile >> type;


      }
      myfile.close();
    }

}
