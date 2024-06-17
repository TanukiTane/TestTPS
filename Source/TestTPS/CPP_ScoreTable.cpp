// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_ScoreTable.h"
#include "WidgetBlueprint.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Kismet2/BlueprintEditorUtils.h"




	void UCPP_ScoreTable::NativeConstruct()
	{
		Super::NativeConstruct();
	}

	void UCPP_ScoreTable::AddPlayerToTable(const FString& PlayerName)
	{
		if (!PlayersInfo.Contains(PlayerName))
		{
			const UWidgetBlueprintGeneratedClass* WidgetBlueprintGeneratedClass = Cast<UWidgetBlueprintGeneratedClass>(GetClass());

			const UPackage* Package = WidgetBlueprintGeneratedClass->GetPackage();
			UWidgetBlueprint* MainAsset = Cast<UWidgetBlueprint>(Package->FindAssetInPackage());

			UTextBlock* PlayerTextBlock = MainAsset->WidgetTree->ConstructWidget<UTextBlock>();
			UTextBlock* ScoreTextBlock = MainAsset->WidgetTree->ConstructWidget<UTextBlock>();

			PlayerTextBlock->SetText(FText::FromString(PlayerName));
			ScoreTextBlock->SetText(FText::FromString("0"));

			PlayerNamesBox->AddChildToVerticalBox(PlayerTextBlock);
			PlayerScoresBox->AddChildToVerticalBox(ScoreTextBlock);

			PlayersInfo.Emplace(PlayerName, ScoreTextBlock);

			MainAsset->Modify();
			FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(MainAsset);
	}

	void UCPP_ScoreTable::UpdateScore(const FString& PlayerName, const uint8 InScore)
{
	if (PlayersInfo.Find(PlayerName))
	{
		UTextBlock* ScoreTextBlock = *PlayersInfo.Find(PlayerName);
		ScoreTextBlock->SetText(FText::FromString(FString::FromInt(InScore)));
	}
}
