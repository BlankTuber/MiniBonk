#pragma once

#include "CoreMinimal.h"
#include "Systems/AbilityTypes.h"

namespace AbilityMath
{
    inline float ApplyModifier(float Current, EModifierType ModifierType, float Value, float MaxValue = MAX_FLT)
    {
        float Result = Current;

        if (ModifierType == EModifierType::Flat)
        {
            Result += Value;
        }
        else if (ModifierType == EModifierType::Percentage)
        {
            Result *= (1.f + Value);
        }

        return FMath::RoundToFloat(FMath::Min(Result, MaxValue));
    }
}
