#pragma once

#include "CoreMinimal.h"
#include "EditorValidatorBase.h"
#include "EditorValidator_EmptyTick.generated.h"

UCLASS()
class COMMONVALIDATORS_API UEditorValidator_EmptyTick : public UEditorValidatorBase
{
	GENERATED_BODY()
	
	virtual bool CanValidateAsset_Implementation(UObject* InObject) const override;
	virtual EDataValidationResult ValidateLoadedAsset_Implementation(UObject* InAsset, TArray<FText>& ValidationErrors) override;

	bool IsEmptyTick(class UK2Node_Event* EventNode);
};
