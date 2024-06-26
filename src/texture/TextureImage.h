#pragma once

// 纹理图像数据信息
struct TextureImage
{
    int32_t width;
    int32_t height;
    int32_t nChannels;
    uint8_t* image;
};