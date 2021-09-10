#include "CDBManager.h"

CDBManager::CDBManager()
{
    if(instance == nullptr)
        instance = this;
    else
    {
        delete this;
    }
}

CDBManager::~CDBManager()
{
}

CDBManager* CDBManager::GetInstance()
{
    if (instance == nullptr)
    {
        return nullptr;
    }
    else
    {
        return instance;
    }
}
