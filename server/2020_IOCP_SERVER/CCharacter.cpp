#include "CCharacter.h"

CCharacter::CCharacter(std::string name, short x, short y) :name{ name }, x{ x }, y{ y }
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

std::string& CCharacter::GetName()
{
    return name;
}

short& CCharacter::GetX()
{
    return x;
}

short& CCharacter::GetY()
{
    return y;
}
