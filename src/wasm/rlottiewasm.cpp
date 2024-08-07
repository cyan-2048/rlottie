#include <emscripten/bind.h>
#include <malloc.h>
#include <stdlib.h>
#include <unistd.h>

#include <string>

#include "rlottie.h"

using namespace emscripten;

const char *resource = "";

typedef unsigned char uint8_t;

class __attribute__((visibility("default"))) RlottieWasm {
public:
    static std::unique_ptr<RlottieWasm> create()
    {
        return std::unique_ptr<RlottieWasm>(new RlottieWasm(resource));
    }
    int frames() const { return mFrameCount; }

    bool load(std::string jsonData)
    {
        mPlayer = rlottie::Animation::loadFromData(std::move(jsonData), "", "",
                                                   false);
        mFrameCount = mPlayer ? mPlayer->totalFrame() : 0;
        return mPlayer ? true : false;
    }

    void setFillColor(std::string keypath, float r, float g, float b)
    {
        if (!mPlayer) return;

        mPlayer->setValue<rlottie::Property::FillColor>(
            keypath, rlottie::Color(r, g, b));
    }

    void setStrokeColor(std::string keypath, float r, float g, float b)
    {
        if (!mPlayer) return;

        mPlayer->setValue<rlottie::Property::StrokeColor>(
            keypath, rlottie::Color(r, g, b));
    }

    void setFillOpacity(std::string keypath, float opacity)
    {
        if (!mPlayer || opacity > 100 || opacity < 0) return;

        mPlayer->setValue<rlottie::Property::FillOpacity>(keypath, opacity);
    }

    void setStrokeOpacity(std::string keypath, float opacity)
    {
        if (!mPlayer || opacity > 100 || opacity < 0) return;

        mPlayer->setValue<rlottie::Property::StrokeOpacity>(keypath, opacity);
    }

    void setStrokeWidth(std::string keypath, float width)
    {
        if (!mPlayer || width < 0) return;

        mPlayer->setValue<rlottie::Property::StrokeWidth>(keypath, width);
    }

    void setAnchor(std::string keypath, float x, float y)
    {
        if (!mPlayer) return;

        mPlayer->setValue<rlottie::Property::TrAnchor>(keypath,
                                                       rlottie::Point(x, y));
    }

    void setPosition(std::string keypath, float x, float y)
    {
        if (!mPlayer) return;

        mPlayer->setValue<rlottie::Property::TrPosition>(keypath,
                                                         rlottie::Point(x, y));
    }

    void setScale(std::string keypath, float width, float height)
    {
        if (!mPlayer) return;

        mPlayer->setValue<rlottie::Property::TrScale>(
            keypath, rlottie::Size(width, height));
    }

    void setRotation(std::string keypath, float degree)
    {
        if (!mPlayer || degree > 360 || degree < 0) return;

        mPlayer->setValue<rlottie::Property::TrRotation>(keypath, degree);
    }

    void setOpacity(std::string keypath, float opacity)
    {
        if (!mPlayer || opacity > 100 || opacity < 0) return;

        mPlayer->setValue<rlottie::Property::TrOpacity>(keypath, opacity);
    }

    // canvas pixel pix[0] pix[1] pix[2] pix[3] {B G R A}
    // lottie pixel pix[0] pix[1] pix[2] pix[3] {R G B A}
    val render(int frame, int width, int height)
    {
        if (!mPlayer) return val(typed_memory_view<uint8_t>(0, nullptr));

        resize(width, height);
        mPlayer->renderSync(
            frame, rlottie::Surface((uint32_t *)mBuffer.get(), mWidth, mHeight,
                                    mWidth * 4));
        convertToCanvasFormat();

        return val(typed_memory_view(mWidth * mHeight * 4, mBuffer.get()));
    }
    ~RlottieWasm() {}

private:
    void resize(int width, int height)
    {
        if (width == mWidth && height == mHeight) return;

        mWidth = width;
        mHeight = height;

        mBuffer = std::make_unique<uint8_t[]>(mWidth * mHeight * 4);
    }

    explicit RlottieWasm(const char *data)
    {
        mPlayer = rlottie::Animation::loadFromData(data, "", "", false);
        mFrameCount = mPlayer ? mPlayer->totalFrame() : 0;
    }

    void convertToCanvasFormat()
    {
        int      totalBytes = mWidth * mHeight * 4;
        uint8_t *buffer = mBuffer.get();
        for (int i = 0; i < totalBytes; i += 4) {
            unsigned char a = buffer[i + 3];
            // compute only if alpha is non zero
            if (a) {
                unsigned char r = buffer[i + 2];
                unsigned char g = buffer[i + 1];
                unsigned char b = buffer[i];

                if (a != 255) {  // un premultiply
                    r = (r * 255) / a;
                    g = (g * 255) / a;
                    b = (b * 255) / a;

                    buffer[i] = r;
                    buffer[i + 1] = g;
                    buffer[i + 2] = b;

                } else {
                    // only swizzle r and b
                    buffer[i] = r;
                    buffer[i + 2] = b;
                }
            }
        }
    }

private:
    int                                 mWidth{0};
    int                                 mHeight{0};
    int                                 mFrameCount{0};
    std::unique_ptr<uint8_t[]>          mBuffer;
    std::unique_ptr<rlottie::Animation> mPlayer;
};

unsigned int getUsedMemory()
{
    struct mallinfo mi = mallinfo();

    unsigned int dynamicTop = (unsigned int)sbrk(0);
    return dynamicTop + mi.fordblks;
}

// Binding code
EMSCRIPTEN_BINDINGS(rlottie_bindings)
{
    class_<RlottieWasm>("RlottieWasm")
        .constructor(&RlottieWasm::create)
        .function("load", &RlottieWasm::load, allow_raw_pointers())
        .function("frames", &RlottieWasm::frames)
        .function("render", &RlottieWasm::render)
        .function("setFillColor", &RlottieWasm::setFillColor)
        .function("setStrokeColor", &RlottieWasm::setStrokeColor)
        .function("setFillOpacity", &RlottieWasm::setFillOpacity)
        .function("setStrokeOpacity", &RlottieWasm::setStrokeOpacity)
        .function("setStrokeWidth", &RlottieWasm::setStrokeWidth)
        .function("setAnchor", &RlottieWasm::setAnchor)
        .function("setPosition", &RlottieWasm::setPosition)
        .function("setScale", &RlottieWasm::setScale)
        .function("setRotation", &RlottieWasm::setRotation)
        .function("setOpacity", &RlottieWasm::setOpacity);

    function("getUsedMemory", &getUsedMemory);
}
