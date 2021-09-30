#include "CCharacter.h"

CCharacter::CCharacter(int id, std::string name, short x, short y) :id{ id }, name { name }, x{ x }, y{ y }
{
}

void CCharacter::Move(short dx, short dy)
{
    x += dx;
    y += dy;
}

void CCharacter::Teleport(short x, short y)
{
    this->x = x;
    this->y = y;
}

std::unordered_set<int>& CCharacter::GetViewlist()
{
    return viewList;
}

std::mutex& CCharacter::GetViewlock()
{
    return viewLock;
}

std::string& CCharacter::GetName()
{
    return name;
}

short CCharacter::GetX() const
{
    return x;
}

short CCharacter::GetY() const
{
    return y;
}

int CCharacter::GetID() const
{
    return id;
}

void CCharacter::SetStatus(bool atkable, short hp, short atk)
{
    this->isAttkable = atkable;
    this->atk = atk;
    this->hp = hp;
}
