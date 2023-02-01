#include "ColPrimitive3D.h"
#include <math.h>

using namespace ColPrimitive3D;

bool ColPrimitive3D::IsHit(Sphere sphere, Plane plane)
{
    //原点から球への距離
    float dist = sphere.pos.Dot(plane.normal);

    //平面の原点からの距離を引いた長さが半径より長ければ当たってない
    if (fabsf(dist - plane.distance) > sphere.r) return false;

    //じゃあ当たってる
    return true;
}

bool ColPrimitive3D::IsHit(Sphere sphere, AABB box) {
    float minX = box.pos.x - box.size.x;
    float maxX = box.pos.x + box.size.x;
    float minY = box.pos.y - box.size.y;
    float maxY = box.pos.y + box.size.y;
    float minZ = box.pos.z - box.size.z;
    float maxZ = box.pos.z + box.size.z;

    float len = 0; //最短距離

    //X軸について
    if (sphere.pos.x < minX)
        len += (sphere.pos.x - minX) * (sphere.pos.x - minX);
    if (sphere.pos.x > maxX)
        len += (sphere.pos.x - maxX) * (sphere.pos.x - maxX);

    //Y軸について
    if (sphere.pos.y < minY)
        len += (sphere.pos.y - minY) * (sphere.pos.y - minY);
    if (sphere.pos.y > maxY)
        len += (sphere.pos.y - maxY) * (sphere.pos.y - maxY);

    //Z軸について
    if (sphere.pos.z < minZ)
        len += (sphere.pos.z - minZ) * (sphere.pos.z - minZ);
    if (sphere.pos.z > maxZ)
        len += (sphere.pos.z - maxZ) * (sphere.pos.z - maxZ);

    if (len == 0) { //0なら箱の中に球の中心があるってこと！
        return true;
    }

    //球の中心と箱の最短距離が半径より小さいならそりゃあ当たってる
    if (len < sphere.r * sphere.r) { 
        return true;
    }

    return false;
}

Vector3 GetClosestPoint(Vector3 point, Triangle triangle) {
    //p0の外側か？？？
    Vector3 p0_p1 = triangle.p1 - triangle.p0;
    Vector3 p0_p2 = triangle.p2 - triangle.p0;
    Vector3 p0_pt = point - triangle.p0;

    float d_01_0p = p0_p1.Dot(p0_pt);
    float d_02_0p = p0_p2.Dot(p0_pt);

    if (d_01_0p < 0 && d_02_0p < 0) {
        return triangle.p0;
    }

    //p1の外側か？？？
    Vector3 p1_p0 = triangle.p0 - triangle.p1;
    Vector3 p1_p2 = triangle.p2 - triangle.p1;
    Vector3 p1_pt = point - triangle.p1;

    float d_10_1p = p1_p0.Dot(p1_pt);
    float d_12_1p = p1_p2.Dot(p1_pt);

    if (d_10_1p < 0 && d_12_1p < 0) {
        return triangle.p1;
    }

    //p2の外側か？？？
    Vector3 p2_p0 = triangle.p0 - triangle.p2;
    Vector3 p2_p1 = triangle.p1 - triangle.p2;
    Vector3 p2_pt = point - triangle.p2;

    float d_20_2p = p2_p0.Dot(p2_pt);
    float d_21_2p = p2_p1.Dot(p2_pt);

    if (d_20_2p < 0 && d_21_2p < 0) {
        return triangle.p2;
    }

    float d_01_1p = p0_p1.Dot(p1_pt);
    float d_02_1p = p0_p2.Dot(p1_pt);
    float cc = d_01_0p * d_02_1p - d_01_1p * d_02_0p;

    //p0-p1間辺の外側か？？？
    if (d_01_0p >= 0 && d_01_1p < 0 && cc <= 0) {
        return triangle.p0 + d_01_0p / (d_01_0p - d_01_1p) * p0_p1;
    }

    float d_01_2p = p0_p1.Dot(p2_pt);
    float d_02_2p = p0_p2.Dot(p2_pt);
    float cb = d_01_2p * d_02_0p - d_01_0p * d_02_2p;

    //p0-p2間辺の外側か？？？
    if (d_02_0p >= 0 && d_02_2p < 0 && cb <= 0) {
        return triangle.p0 + d_02_0p / (d_02_0p - d_02_2p) * p0_p2;
    }

    float d_12_2p = p1_p2.Dot(p2_pt);
    float d_10_2p = p1_p0.Dot(p2_pt);
    float ca = d_12_1p * d_10_2p - d_12_2p * d_10_1p;

    //p1-p2間辺の外側か？？？
    if (d_12_1p >= 0 && d_12_2p < 0 && ca <= 0) {
        return triangle.p1 + d_12_1p / (d_12_1p - d_12_2p) * p1_p2;
    }

    float denom = 1.0f / (ca + cb + cc);
    float v = cb * denom;
    float w = cc * denom;
    return triangle.p0 + p0_p1 * v + p0_p2 * w;
}

bool ColPrimitive3D::IsHit(Sphere sphere, Triangle triangle, Vector3* outClosestPos)
{
    Vector3 closest = GetClosestPoint(sphere.pos, triangle);
    if (outClosestPos != nullptr) {
        *outClosestPos = closest;
    }

    Vector3 v = closest - sphere.pos;

    if (v.LengthSq() > sphere.r * sphere.r) return false;

    return true;
}
