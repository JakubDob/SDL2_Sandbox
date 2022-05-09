#pragma once
#include "Matrix.h"
#include "Lang.h"
#include "Types.h"
#include <random>
#include <iostream>
#include <SDL2/SDL.h>
#include <chrono>
#include <cstring>

#if HAS_CONCEPTS
#include <concepts>
#endif

namespace utils {
template <typename T>
class RandomMatrix {
   public:
    template <size_t ROWS>
    static auto getNormalizedVector() {
        math::Matrix<T, ROWS, 1> ret;
        for (int i = 0; i < ROWS; ++i) {
            ret[i] = negOneDist(gen);
        }
        return ret.normalize();
    }

    template <size_t ROWS>
    static auto getVector(T min, T max) {
        std::uniform_real_distribution<T> dist(min, max);
        math::Matrix<T, ROWS, 1> ret;
        for (int i = 0; i < ROWS; ++i) {
            ret[i] = dist(gen);
        }
        return ret;
    }
    template <size_t ROWS, size_t COLS>
    static auto getMatrix(T min, T max) {
        std::uniform_real_distribution<T> dist(min, max);
        math::Matrix<T, ROWS, COLS> ret;
        for (int i = 0; i < ROWS * COLS; ++i) {
            ret[i] = dist(gen);
        }
        return ret;
    }
    static auto getScalar(T min, T max) {
        std::uniform_real_distribution<T> dist(min, max);
        return dist(gen);
    }
    static RandomMatrix& instance() {
        static RandomMatrix m;
        return m;
    }

   private:
    inline static std::random_device rd;
    inline static std::mt19937 gen = std::mt19937(rd());
    inline static std::uniform_real_distribution<T> negOneDist =
        std::uniform_real_distribution<T>(-1.0, 1.0);
    RandomMatrix() {}
};

class TimeStamp {
   public:
    using clock_t = std::chrono::high_resolution_clock;
    using time_point_t = clock_t::time_point;
    TimeStamp();
    long long getElapsedMs(TimeStamp const& o) const;
    long long getElapsedMs() const;
    time_point_t getSavedTime() const;
    void reset();
    bool resetIfOlderThanMs(long long ms);
    bool isOlderThanMs(long long ms) const;

   private:
    time_point_t saved;
};

inline Vec2 clockwiseNormal(Vec4 const& edge) {
    return Vec2(edge[1], -edge[0]);
};
inline Vec2 counterclockwiseNormal(Vec4 const& edge) {
    return Vec2(-edge[1], edge[0]);
};
inline Vec2 clockwiseNormalNormalized(Vec4 const& edge) {
    return Vec2(edge[1], -edge[0]).normalize();
};
inline Vec2 counterclockwiseNormalNormalized(Vec4 const& edge) {
    return Vec2(-edge[1], edge[0]).normalize();
};
inline Vec2 clockwiseNormal(Vec2 const& edge) {
    return Vec2(edge[1], -edge[0]);
};
inline Vec2 counterclockwiseNormal(Vec2 const& edge) {
    return Vec2(-edge[1], edge[0]);
};
inline Vec2 clockwiseNormalNormalized(Vec2 const& edge) {
    return Vec2(edge[1], -edge[0]).normalize();
};
inline Vec2 counterclockwiseNormalNormalized(Vec2 const& edge) {
    return Vec2(-edge[1], edge[0]).normalize();
};

inline bool pointInsideRect(int x, int y, Vec4 const& r) {
    return x > r[0] && x < r[0] + r[2] && y > r[1] && y < r[1] + r[3];
}

inline unsigned nextPowerOfTwo(unsigned value) {
    // check if number loses most significant 1 after subtraction by 1 (eg. 8 =
    // 1000b, 7 = 0111b: TRUE; 9 = 1001b, 8 = 1000b: FALSE)
    bool isPowerOfTwo = value > 0 && ((value & (value - 1)) == 0);
    if (isPowerOfTwo) {
        return value << 1;
    }
    unsigned result = 1;
    while (value) {
        value >>= 1;
        result <<= 1;
    }
    return result;
}

// defines screen coordinate system and maps to a square <-1,1> by <-1,1>
inline Mat4 ortographicProjection(scalar_t left, scalar_t right,
                                  scalar_t bottom, scalar_t top) {
    // these values should match viewport's aspect ratio
    scalar_t lrMidpoint = (left + right) / 2;
    scalar_t tdMidpoint = (top + bottom) / 2;
    scalar_t lrPercentage = 1 / (right - left);
    scalar_t tdPercentage = 1 / (top - bottom);
    auto translateToCenter =
        math::getTranslationMatrix(Vec2(-lrMidpoint, -tdMidpoint));
    // non-uniform scaling, z not used in 2d
    auto scaleTo2by2Range =
        Mat4({lrPercentage * 2, 0, 0, 0, 0, tdPercentage * 2, 0, 0, 0, 0, 1, 0,
              0, 0, 0, 1});
    return scaleTo2by2Range * translateToCenter;
}

inline int colorToInt(Vec4 const& c) {
    int result = 0;
    result = ((static_cast<int>(c[0] * 255) & 0xff) << 24) |
             ((static_cast<int>(c[1] * 255) & 0xff) << 16) |
             ((static_cast<int>(c[2] * 255) & 0xff) << 8) |
             static_cast<int>(c[3] * 255);
    return result;
}

inline SDL_Color colorVecToSdlColor(Vec4 const& c) {
    SDL_Color ret;
    ret.r = c[0] * 255;
    ret.g = c[1] * 255;
    ret.b = c[2] * 255;
    ret.a = c[3] * 255;
    return ret;
}

inline void rotateSurfaceUpsideDown(SDL_Surface* surface) {
    SDL_LockSurface(surface);
    auto offset = surface->pitch;
    auto* firstRow = static_cast<unsigned char*>(surface->pixels);
    auto* lastRow = firstRow + (surface->h - 1) * offset;
    std::vector<unsigned char> tmp(offset);
    for (int i = 0; i < surface->h / 2; ++i) {
        auto* r1 = firstRow + i * offset;
        auto* r2 = lastRow - i * offset;
        std::memcpy(tmp.data(), r1, offset);
        std::memcpy(r1, r2, offset);
        std::memcpy(r2, tmp.data(), offset);
    }
    SDL_UnlockSurface(surface);
}
}  // namespace utils

inline std::ostream& operator<<(std::ostream& os, SDL_Rect const& r) {
    os << "SDL_Rect (x: " << r.x << ", y: " << r.y << ", w: " << r.w
       << ", h: " << r.h << ")\n";
    return os;
}