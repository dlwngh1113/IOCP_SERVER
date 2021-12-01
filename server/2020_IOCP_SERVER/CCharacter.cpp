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
    info->c_lock.lock();
    info->hp -= otherAtk;
    info->c_lock.unlock();
    return true;
}

std::unordered_set<int>& CCharacter::GetViewlist() 
{
    return viewList;
}

CInfo* CCharacter::GetInfo()
{
    return info;
}
