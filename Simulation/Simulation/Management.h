#pragma once

class Object;

class Management
{
    DECLARE_SINGLETON(Management)
private:
    Management();
    ~Management();

public :
    void        Update_Management(const float& fTimeDelta);
    void        LateUpdate_Management(const float& fTimeDelta);
    void        FixedUpdate_Management(const float& fTimeDelta);
    void        Render_Management();
    void        Release_Management();

public :
    void        Add_Object(Object* pObj) { m_ObjectList.push_back(pObj); }

private:
    list<Object*>		m_ObjectList;

};

