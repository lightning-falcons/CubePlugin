// Copyright Epic Games, Inc. All Rights Reserved.

#include "CreateCube.h"
#include "CreateCubeStyle.h"
#include "CreateCubeCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"
#include "UObject/Object.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "Components/LightComponent.h"
#include "Engine/Engine.h"
#include "Engine/PostProcessVolume.h"
#include "Editor/EditorEngine.h"
#include "UObject/ConstructorHelpers.h"
#include "CubeSimple.h"
#include "LidarPointCloud.h"
#include "LidarPointCloudActor.h"
#include "LidarPointCloudComponent.h"


static const FName CreateCubeTabName("CreateCube");

#define LOCTEXT_NAMESPACE "FCreateCubeModule"

void FCreateCubeModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FCreateCubeStyle::Initialize();
	FCreateCubeStyle::ReloadTextures();

	FCreateCubeCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FCreateCubeCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FCreateCubeModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FCreateCubeModule::RegisterMenus));

}

void FCreateCubeModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FCreateCubeStyle::Shutdown();

	FCreateCubeCommands::Unregister();
}

void FCreateCubeModule::PluginButtonClicked()
{
	// Put your "OnButtonClicked" stuff here
	FText DialogText = FText::Format(
							LOCTEXT("PluginButtonDialogText", "Add code to {0} in {1} to override this button's actions"),
							FText::FromString(TEXT("FCreateCubeModule::PluginButtonClicked()")),
							FText::FromString(TEXT("CreateCube.cpp"))
					   );
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);

	//UStaticMeshComponent* CubeMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube"));

	ULevel* Level = GEditor->GetEditorWorldContext().World()->GetCurrentLevel();
	ACubeSimple* Actor = Cast<ACubeSimple>(GEditor->AddActor(Level, ACubeSimple::StaticClass(), FTransform()));

	Actor->SetActorLocation(FVector(10.f, 10.f, 10.f));

	const FString PathToFile = FString("C:\\Users\\admin\\Downloads\\TransformedCloud_katoomba2.las");
	ULidarPointCloud* GlobalMap = ULidarPointCloud::CreateFromFile(PathToFile);

	ALidarPointCloudActor* GlobalMapActor = Cast<ALidarPointCloudActor>(GEditor->AddActor(Level, ALidarPointCloudActor::StaticClass(), FTransform()));
	GlobalMapActor->SetPointCloud(GlobalMap);


	GlobalMapActor->GetPointCloudComponent()->IntensityInfluence = 0.99;



	// Now what we want to do is dynamically display, with a 0.1 s delay
	//Sleep(2000)




}

void FCreateCubeModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FCreateCubeCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FCreateCubeCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCreateCubeModule, CreateCube)