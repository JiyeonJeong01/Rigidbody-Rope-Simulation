#include "pch.h"
#include "Management.h"
#include "Object.h"

IMPLEMENT_SINGLETON(Management)

Management::Management()
{
}

Management::~Management()
{
}

void Management::Update_Management(const float& fTimeDelta)
{
    for (auto* pObj : m_ObjectList)
    {
        if (pObj->Get_Active() == false) continue;
        pObj->Update_GameObject(fTimeDelta);
    }
}

void Management::LateUpdate_Management(const float& fTimeDelta)
{
    for (auto* pObj : m_ObjectList)
    {
        if (pObj->Get_Active() == false) continue;
        pObj->LateUpdate_GameObject(fTimeDelta);
    }
}

void Management::FixedUpdate_Management(const float& fTimeDelta)
{
    for (auto* pObj : m_ObjectList)
    {
        if (pObj->Get_Active() == false) continue;
        pObj->FixedUpdate_GameObject(fTimeDelta);
    }
}

void Management::Render_Management()
{
        for (auto* pObj : m_ObjectList)
    {
        if (pObj->Get_Active() == false) continue;
        pObj->Render_GameObject();
    }
}

void Management::Release_Management()
{
    for (auto* pObj : m_ObjectList)
        Safe_Release(pObj);
}
