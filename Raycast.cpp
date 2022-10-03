#include "Raycast.h"
#include "SphereCollision.h"
#include "Util.h"

bool Raycast::Collide(Collision& col) const
{
    if (Util::instanceof<SphereCollision>(col)) {
        SphereCollision _col = dynamic_cast<SphereCollision&>(col);
        return Collide(_col);
    }
    throw std::logic_error("����`�̑���Ƃ̓����蔻����s�����Ƃ��܂���");
    return false;
}

bool Raycast::Collide(SphereCollision& col) const
{
    if (col.r == 0) return false;
    if (this->distance == 0) return false;

    Vector3 direction = this->direction.GetNormalize();
    Vector3 toSphere = col.pos - this->pos;

    //������A���C���狅�֌������x�N�g���ɐ������~�낷
    float posInVec = toSphere.Dot(direction);

    //�������~�낵���ʒu�����C����O�Ȃ瓖�����ĂȂ�
    if (posInVec < 0) return false;

    //���C���L�����ŁA�������~�낵���ʒu�������艜�Ȃ瓖�����ĂȂ�
    if (this->distance > 0 && this->distance < posInVec) return false;

    Vector3 hitPos = this->pos + direction * posInVec;
    float hitDistance = (col.pos - hitPos).LengthSq();

    return hitDistance <= powf(col.r + this->thickness, 2);
}