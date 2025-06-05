
#include "EditorValidator_EmptyTick.h"

#include "Misc/DataValidation.h"
#include "Engine/Blueprint.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_Event.h"
#include "Engine/MemberReference.h"

bool UEditorValidator_EmptyTick::CanValidateAsset_Implementation(const FAssetData& InAssetData, UObject* InObject, FDataValidationContext& InContext) const
{
#if ENGINE_MAJOR_VERSION > 5 || (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 6)
	// Empty ticks are automatically disabled in UE 5.6 onwards, no need to do anything for those versions
	return false;
#else
	return InObject && InObject->IsA<UBlueprint>();
#endif
}

EDataValidationResult UEditorValidator_EmptyTick::ValidateLoadedAsset_Implementation(const FAssetData& InAssetData, UObject* InAsset, FDataValidationContext& Context)
{
	static const FName EventTickName(TEXT("ReceiveTick"));

	UBlueprint* Blueprint = Cast<UBlueprint>(InAsset);
	if (!Blueprint) return EDataValidationResult::NotValidated;

	for (UEdGraph* Graph : Blueprint->UbergraphPages)
	{
		for (UEdGraphNode* Node : Graph->Nodes)
		{
			UK2Node_Event* EventNode = Cast<UK2Node_Event>(Node);
			if (EventNode && EventNode->EventReference.GetMemberName() == EventTickName)
			{
				if (IsEmptyTick(EventNode))
				{
					Context.AddError(FText::FromString(TEXT("Empty Tick nodes still produce overhead, please use or remove it.")));
					return EDataValidationResult::Invalid;
				}
			}
		}
	}

	return EDataValidationResult::Valid;
}

bool UEditorValidator_EmptyTick::IsEmptyTick(UK2Node_Event* EventNode)
{
	if (EventNode->IsAutomaticallyPlacedGhostNode()) return false; // Ghost nodes aren't real nodes

	UEdGraphPin* ExecThenPin = EventNode->FindPin(UEdGraphSchema_K2::PN_Then);
	return ExecThenPin && ExecThenPin->LinkedTo.IsEmpty();
}
