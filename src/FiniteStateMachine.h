#pragma once
#include "TemplateUtils.h"
#include <array>
#include <stdint.h>

template <uint_fast8_t StateCount, typename TransitionFunction, typename Action>
class FiniteStateMachine {
   public:
    FiniteStateMachine() {
        static_assert(
            utils::verifyFunctionReturnType<bool>(TransitionFunction()),
            "Transition function must return boolean");
        table.fill(nullptr);
        onStateActions.fill(nullptr);
        onChangeActions.fill(nullptr);
    }
    // updates current state based on transition function, true => new state
    // different than the old one
    template <typename... Args>
    bool update(Args&&... tfArgs) {
        for (uint_fast8_t i = 0; i < StateCount; ++i) {
            auto idx = currentState * StateCount + i;
            if (table[idx] && table[idx](std::forward<Args>(tfArgs)...)) {
                auto oldState = currentState;
                currentState = i;
                bool changed = oldState != currentState;
                changed ? stateChanged = oldState : stateChanged = -1;
                return changed;
            }
        }
        return false;
    }

    // Defines a condition under which the machine changes its state
    template <uint_fast8_t From, uint_fast8_t To>
    void rule(TransitionFunction tf) {
        static_assert(From < StateCount,
                      "'From' type must be one of the specified states");
        static_assert(To < StateCount,
                      "'To' type must be one of the specified states");
        table[From * StateCount + To] = tf;
    }
    template <uint_fast8_t State>
    void onState(Action action) {
        static_assert(State < StateCount,
                      "Cannot set an action for a state that was not specified "
                      "during initialization");
        onStateActions[State] = action;
    }
    template <uint_fast8_t State>
    void onChange(Action action) {
        static_assert(State < StateCount,
                      "Cannot set an action for a state that was not specified "
                      "during initialization");
        onChangeActions[State] = action;
    }
    template <typename... Args>
    void performAction(Args&&... args) {
        if (stateChanged >= 0 && onChangeActions[stateChanged] != nullptr) {
            onChangeActions[stateChanged](std::forward<Args>(args)...);
            stateChanged = -1;
        }
        if (onStateActions[currentState] != nullptr) {
            onStateActions[currentState](std::forward<Args>(args)...);
        }
    }
    auto getCurrentState() const { return currentState; }

    // state accessible by all other states
    template <uint_fast8_t To>
    void setUniversalStateRule(TransitionFunction tf) {
        static_assert(To < StateCount,
                      "'To' type must be one of the specified states");
        for (auto i = 0; i < StateCount; ++i) {
            if (i != To) {
                table[i * StateCount + To] = tf;
            }
        }
    }

    template <uint_fast8_t StateA, uint_fast8_t StateB>
    void clearBidirectionalRules() {
        static_assert(StateA < StateCount && StateB < StateCount,
                      "Must be one of the specified states");
        table[StateA * StateCount + StateB] = nullptr;
        table[StateB * StateCount + StateA] = nullptr;
    }

    void makeStateInaccessible(uint_fast8_t StateA) {
        for (auto i = 0; i < StateCount; ++i) {
            table[i * StateCount + StateA] = nullptr;
        }
    }

   private:
    std::array<TransitionFunction, StateCount * StateCount> table;
    std::array<Action, StateCount>
        onStateActions;  // if current state=A, onStateActions[A] is called for
                         // each performAction call
    std::array<Action, StateCount>
        onChangeActions;  // if current state=A, change A->B calls
                          // onStateChange[A] once
    int stateChanged = -1;
    uint_fast8_t currentState = 0;
};