#pragma once
#include <cstdint>
#include <string>
#include "Types.h"

namespace EngineConstants {
namespace Path {
inline char const delimiter = '/';
inline std::string const texturesFolder = "textures";
inline std::string const fontsFolder = "fonts";
inline std::string const shadersFolder = "shaders";
inline std::string const atlasFolder = "atlas";
}  // namespace Path
namespace Window {
inline int constexpr width = 1200;
inline int constexpr height = 800;
}  // namespace Window
namespace Color {
inline Vec4 const white = {1.0f, 1.0f, 1.0f, 1.0f};
inline Vec4 const black = {0.0f, 0.0f, 0.0f, 1.0f};
inline Vec4 const red = {1.0f, 0.0f, 0.0f, 1.0f};
inline Vec4 const green = {0.0f, 1.0f, 0.0f, 1.0f};
inline Vec4 const blue = {0.0f, 0.0f, 1.0f, 1.0f};
}  // namespace Color
namespace Font {
inline int constexpr fontBegin = 0;
inline int constexpr fontEnd = 128;
inline int constexpr maxFontSize = 120;
inline int constexpr smallSize = 12;
inline int constexpr mediumSize = 24;
inline int constexpr bigSize = 36;
inline std::string const defaultFont = "Rokkitt-Regular.ttf";
}  // namespace Font
}  // namespace EngineConstants