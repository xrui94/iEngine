#pragma once

#include <string>


// class Api
// {
// public:
//     static Api& GetInstance();

//     static std::string CreateScene();

//     static std::string AddCamera();

//     static std::string AddMesh();

// private:
//     // 禁止外部构造
//     Api();

//     // 禁止外部析构
//     ~Api();

//     // 禁止外部拷贝构造
//     Api(const Api& api) = delete;

//     // 禁止外部赋值操作
//     const Api& operator=(const Api& api) = delete;
// };

std::string GetOrCreateEngine();

std::string CreateScene();

std::string AddCamera();

std::string AddMesh();