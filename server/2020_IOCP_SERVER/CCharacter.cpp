#include "CCharacter.h"

CCharacter::CCharacter(int id, std::string name, short x, short y)
{
    info = new CInfo(id, name, x, y);
}

CCharacter::CCharacter(CInfo* info) :info{ info }
{

}

CCharacter::~CCharacter()
{
    delete info;
}

void CCharacter::Move(short dx, short dy)
{
    info->x += dx;
    info->y += dy;
}

void CCharacter::Teleport(short x, short y)
{
    info->x = x;
    info->y = y;
}

bool CCharacter::GetDamage(short otherAtk)
{
    if (info->isAttkable)
    {
        info->c_lock.lock();
        info->hp -= otherAtk;
        if (info->hp <= 0)
        {
            info->x = 0;
            info->y = 0;
            info->exp /= 2;
            info->hp = info->level * 70;
        }
        info->c_lock.unlock();
        return true;
    }
    return false;
}

std::unordered_set<int>& CCharacter::GetViewlist() 
{
    return viewList;
}

CInfo* CCharacter::GetInfo()
{
    return info;
}
