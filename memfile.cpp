#include "memfile.h"

AppConfigs::AppConfigs(std::string filename){
    std::ifstream file(filename);
    std::string parameter, value;
    while(!(file.eof())){
        file >> parameter;
        file >> value;
        if (parameter == PARAMETER_WIDTH){
            this->camera_width = atoi(value.c_str());
        }
        else if (parameter == PARAMETER_HEIGHT){
            this->camera_height = atoi(value.c_str());
        }
        else if (parameter == PARAMETER_CAMID){
            this->camera_id = atoi(value.c_str());
        }
        else if (parameter == PARAMETER_FRAMERATE){
            this->framerate = atoi(value.c_str());
        }
        else if (parameter == PARAMETER_FRAMERATE_THRES){
            this->framerate_thres = atoi(value.c_str());
        }
        else if (parameter == PARAMETER_ARUCO_IDS){
            while(value != "end"){
                aruco_list.append(atoi(value.c_str()));
                file >> value;
            }
        }
        else if (parameter == PARAMETER_AUTOEXPOSURE){
            this->autoexposure = atoi(value.c_str());
        }
        else if (parameter == PARAMETER_AUTOFOCUS){
            this->autofocus = atoi(value.c_str());
        }
    }
    file.close();
    std::cout << "Camera Width: " << this->camera_width << std::endl;
    std::cout << "Camera Height: " << this->camera_height << std::endl;
    std::cout << "Autoexposure: " << this->autoexposure << std::endl;
    std::cout << "Autofocus: " << this->autofocus << std::endl;
    std::cout << "Camera ID: " << this->camera_id << std::endl;
    std::cout << "Framerate: " << this->framerate << std::endl;
    std::cout << "Framerate Threshold: " << this->framerate_thres << std::endl;
    std::cout << "Marker IDs: ";
    for (auto iter = aruco_list.begin(); iter != aruco_list.end(); iter++){
        std::cout << *iter << " ";
    }
    std::cout << std::endl;
}

void AppConfigs::Save(std::string filename){
    std::ofstream file(filename);
    file << PARAMETER_WIDTH << " " << this->camera_width << std::endl
         << PARAMETER_HEIGHT << " " << this->camera_height << std::endl
         << PARAMETER_AUTOEXPOSURE << " " << this->autoexposure << std::endl
         << PARAMETER_AUTOFOCUS << " " << this->autofocus << std::endl
         << PARAMETER_CAMID << " " << this->camera_id << std::endl
         << PARAMETER_FRAMERATE << " " << this->framerate << std::endl
         << PARAMETER_FRAMERATE_THRES << " " << this->framerate_thres << std::endl
         << PARAMETER_ARUCO_IDS << " ";
    for (auto iter = this->aruco_list.begin(); iter != this->aruco_list.end(); iter++)
        file << std::to_string(*iter) << " ";
    file << "end";
}


QStringList AppConfigs::GetArucoIDs(void){
    QStringList strlist;
    for (auto iter = aruco_list.begin(); iter != aruco_list.end(); iter++)
        strlist.append(MARKER_ID_HEADER + QString::fromStdString(std::to_string(*iter)));
    return strlist;
}

void AppConfigs::SetArucoIDs(QStringList aruco_list){
    this->aruco_list.clear();
    for (auto iter = aruco_list.begin(); iter != aruco_list.end(); iter++)
        this->aruco_list.append(iter->remove(0, 10).toInt());
}
