// 关于：Stbi loading errors，error LNK2005: stbi_load already defined in Define.cpp
// #Ref: https://stackoverflow.com/questions/70597478/stbi-loading-errors
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>