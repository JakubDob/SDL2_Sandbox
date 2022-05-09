#include <gtest/gtest.h>
#include "src/Matrix.h"
#include "src/Utils.h"

using namespace math;
TEST(MatrixTests, transpose){
    Mat3 m(1.f,2.f,3.f,4.f,5.f,6.f,7.f,8.f,9.f);
    auto const& result = transpose(m);
    Mat3 expected(1.f,4.f,7.f,2.f,5.f,8.f,3.f,6.f,9.f);
    EXPECT_EQ(expected, result);
}

TEST(MatrixTests, addTwoVectors){
    Vec3 v1(10.f,20.f,0.f);
    Vec3 v2(1.f,2.f,0.f);
    Vec3 expected(11.f,22.f,0.f);
    auto result = v1+v2;
    EXPECT_EQ(expected, result);
}

TEST(MatrixTests, multiplyByIdentity){
    Mat3 id;
    Vec3 v(100.f,200.f,300.f);
    Vec3 expected(100.f,200.f,300.f);
    auto result = id*v;
    EXPECT_EQ(expected, result);
}

TEST(MatrixTests, dotProduct){
    Vec3 m1(6.f,7.f,8.f);
    Vec3 m2(10.f,20.f,30.f);
    auto result = dot(m1,m2);
    float expected = 440;
    EXPECT_EQ(expected, result);
}

TEST(MatrixTests, initWithVectorAndScalar){
    Vec3 v1(10.f,20.f,30.f);
    Vec4 resultExplicit(v1,41.f);
    Vec4 resultImplicit(v1,{41});
    Vec4 expected(10.f,20.f,30.f,41.f);
    EXPECT_EQ(resultExplicit, expected);
    EXPECT_EQ(resultImplicit, expected);
}

TEST(MatrixTests, vectorMagnitude){
    Vec2 v(-3,-4);
    float result = v.magnitude();
    float expected = 5;
    EXPECT_EQ(expected, result);
}

TEST(MatrixTests, vectorNormalize){
    Vec2 v(-3,-4);
    auto result = v.normalize();
    Vec2 expected(-3.f/5,-4.f/5);
    EXPECT_EQ(expected, result);    
}

TEST(MatrixTests, scaleVectorWithMatrix){
    Mat4 scale;
    scale(0,0) = 2.0f;
    scale(1,1) = 3.0f;
    scale(2,2) = 4.0f;
    scale(3,3) = 5.0f;

    Vec4 a(2.f, 3.f, 0.f, 5.0f);
    Vec4 expected(4.f, 9.f, 0.f, 25.0f);
    auto result = scale * a;
    EXPECT_EQ(expected, result);
}

TEST(MatrixTests, scaleVectorWith2DMatrixFunc){
    //scalingMatrix transforms x,y only
    auto scale = getScalingMatrix(2.f);
    Vec4 a(2.f, 3.f, 0.f, 5.0f);
    Vec4 expected(4.f, 6.f, 0.f, 5.0f);
    auto result = scale * a;
    EXPECT_EQ(expected, result);
}

TEST(MatrixTests, rotateVectorWith2DMatrixFunc){
    //rotate 90 deg counterclockwise: (1,0)->(0,1)
    auto rotate = getRotationMatrix(90.f);
    Vec4 a(1.0f, 0.0f, 1.0f, 1.0f);
    Vec4 expected(0.0f, 1.0f, 1.0f, 1.0f);
    auto result = rotate * a;
    //handle rounding errors from sin and cos
    EXPECT_NEAR(expected[0], result[0], 0.000001f);
    EXPECT_NEAR(expected[1], result[1], 0.000001f);
    EXPECT_NEAR(expected[2], result[2], 0.000001f);
    EXPECT_NEAR(expected[3], result[3], 0.000001f);
}

TEST(MatrixTests, translateWith2DMatrixFunc){
    Vec2 translation (10, 20);
    Vec4 target({100, 100, 0, 1});
    auto translate = getTranslationMatrix(translation);
    auto result = translate * target;
    Vec4 expected({110, 120, 0, 1});
    EXPECT_EQ(expected, result);
}

TEST(MathUtilsTests, getNextPowerOfTwo){
    unsigned x = 34543563;
    unsigned result = utils::nextPowerOfTwo(x);
    unsigned expected = 67108864;
    EXPECT_EQ(expected, result);
}
