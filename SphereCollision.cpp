#include "SphereCollision.h"
#include "Raycast.h"
#include "Util.h"
#include <stdexcept>

bool SphereCollision::Collide(Collision& col) const
{
    if (Util::instanceof<SphereCollision>(col)) {
        SphereCollision _col = dynamic_cast<SphereCollision&>(col);
        return Collide(_col);
    }
    if (Util::instanceof<Raycast>(col)) {
        Raycast _col = dynamic_cast<Raycast&>(col);
        return Collide(_col);
    }
    throw std::logic_error("����`�̑���Ƃ̓����蔻����s�����Ƃ��܂���");
    return false;
}

bool SphereCollision::Collide(const SphereCollision& col) const
{
    if (this->r == 0 || col.r == 0) return false;
    Vector3 distance = col.pos - this->pos;
    return distance.LengthSq() <= powf(col.r, 2) + powf(this->r, 2);
}

bool SphereCollision::Collide(const Raycast& col) const
{
    return col.Collide((SphereCollision&)*this);
}
