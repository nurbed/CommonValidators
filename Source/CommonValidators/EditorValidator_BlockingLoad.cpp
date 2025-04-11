
#include "EditorValidator_BlockingLoad.h"

#include "Misc/DataValidation.h"
#include "Engine/Blueprint.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_Event.h"
#include "Engine/MemberReference.h"
#include "K2Node_CallFunction.h"

bool UEditorValidator_BlockingLoad::CanValidateAsset_Implementation(const FAssetData& InAssetData, UObject* InObject, FDataValidationContext& InContext) const
{
	return InObject && InObject->IsA<UBlueprint>();
}

EDataValidationResult UEditorValidator_BlockingLoad::ValidateLoadedAsset_Implementation(const FAssetData& InAssetData, UObject* InAsset, FDataValidationContext& Context)
{
	UBlueprint* Blueprint = Cast<UBlueprint>(InAsset);
	if (!Blueprint) return EDataValidationResult::NotValidated;

	EDataValidationResult DataValidationResult = EDataValidationResult::Valid;

	for (UEdGraph* Graph : Blueprint->UbergraphPages)
	{
		for (UEdGraphNode* Node : Graph->Nodes)
		{
			if (IsBlockingLoad(Node))
			{
				Context.AddError(FText::FromString(TEXT("Blocking (synchronous) loading nodes will produce a hitch, use an asynchronous version instead.")));
				DataValidationResult = EDataValidationResult::Invalid;
			}
		}
	}

	return DataValidationResult;
}

bool UEditorValidator_BlockingLoad::IsBlockingLoad(UEdGraphNode* Node)
{
	UK2Node_CallFunction* CallFunctionNode = Cast<UK2Node_CallFunction>(Node);
	
	if (!CallFunctionNode)
	{
		// Not a function call node
		return false;
	}

	static const FName LoadAssetBlockingFunctionName(TEXT("LoadAsset_Blocking"));
	static const FName LoadClassAssetBlockingFunctionName(TEXT("LoadClassAsset_Blocking"));
	FName FunctionName = CallFunctionNode->GetFunctionName();

	if (FunctionName == LoadAssetBlockingFunctionName)
	{
		return true;
	}

	if (FunctionName == LoadClassAssetBlockingFunctionName)
	{
		return true;
	}

	// Not a blocking (synchronous) loading function
	return false;
}
