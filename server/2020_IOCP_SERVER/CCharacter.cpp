#include "CCharacter.h"

CCharacter::CCharacter(int id, std::string name, short x, short y)
{
    info = new CInfo(id, name, x, y);
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

std::unordered_set<int>& CCharacter::GetViewlist()
{
    return viewList;
}

std::mutex& CCharacter::GetViewlock()
{
    return viewLock;
}

CInfo* CCharacter::GetInfo()
{
    return info;
}
