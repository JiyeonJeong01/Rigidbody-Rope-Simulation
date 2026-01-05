#pragma once
#include "pch.h"

enum class EActionPhase : uint8_t
{
    Started,
    Performed,
    Canceled
};

enum class EBindingType : uint8_t
{
    AnyOf,   // OR: keys 중 하나라도 눌리면 true
    AllOf     // AND: chord: keys 모두 눌려야 true
};

struct Binding
{

    vector<int> keys;
    EBindingType type = EBindingType::AnyOf;
};

struct InputContext
{
    const wstring* mapName = nullptr;
    const wstring* actionName = nullptr;

    bool startedThisFrame = false; // GetKeyDown
    bool canceledThisFrame = false; // GetKeyUp
    bool isPressed = false; // GetKey

    EActionPhase phase = EActionPhase::Performed;

    bool consumed = false;

    void Consume() { consumed = true; }
};

struct Action
{
    wstring name;
    vector<Binding> bindings;

    // 옵션: 버튼 액션에서 performed를 어떻게 줄지
    // - true: 눌려있는 동안 매 프레임 performed (Held 방식)
    // - false: started(Down) 때만 performed 1회 (Single-shot 방식)
    bool performedWhileHeld = true;

    bool enabled = true;

    std::function<void(InputContext&)> callback;
};

struct ActionMap
{
    wstring name;
    bool enabled = true;
    vector<Action> actions;
};

// https://chatgpt.com/c/695a7ca3-96b0-8322-ad21-c576b7a75852