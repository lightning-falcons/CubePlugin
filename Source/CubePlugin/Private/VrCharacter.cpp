// Fill out your copyright notice in the Description page of Project Settings.


#include "VrCharacter.h"

struct csv_istream {
	std::istream& is_;
	csv_istream(std::istream& is) : is_(is) {}
	void scan_ws() const {
		while (is_.good()) {
			int c = is_.peek();
			if (c != ' ' && c != '\t') break;
			is_.get();
		}
	}
	void scan(std::string* s = 0) const {
		std::string ws;
		int c = is_.get();
		if (is_.good()) {
			do {
				if (c == ',' || c == '\n') break;
				if (s) {
					ws += c;
					if (c != ' ' && c != '\t') {
						*s += ws;
						ws.clear();
					}
				}
				c = is_.get();
			} while (is_.good());
			if (is_.eof()) is_.clear();
		}
	}
	template <typename T, bool> struct set_value {
		void operator () (std::string in, T& v) const {
			std::istringstream(in) >> v;
		}
	};
	template <typename T> struct set_value<T, true> {
		template <bool SIGNED> void convert(std::string in, T& v) const {
			if (SIGNED) v = ::strtoll(in.c_str(), 0, 0);
			else v = ::strtoull(in.c_str(), 0, 0);
		}
		void operator () (std::string in, T& v) const {
			convert<is_signed_int<T>::val>(in, v);
		}
	};
	template <typename T> const csv_istream& operator >> (T& v) const {
		std::string tmp;
		scan(&tmp);
		set_value<T, is_int<T>::val>()(tmp, v);
		return *this;
	}
	const csv_istream& operator >> (std::string& v) const {
		v.clear();
		scan_ws();
		if (is_.peek() != '"') scan(&v);
		else {
			std::string tmp;
			is_.get();
			std::getline(is_, tmp, '"');
			while (is_.peek() == '"') {
				v += tmp;
				v += is_.get();
				std::getline(is_, tmp, '"');
			}
			v += tmp;
			scan();
		}
		return *this;
	}
	template <typename T>
	const csv_istream& operator >> (T& (*manip)(T&)) const {
		is_ >> manip;
		return *this;
	}
	operator bool() const { return !is_.fail(); }
};

template <typename T> struct is_signed_int { enum { val = false }; };
template <> struct is_signed_int<short> { enum { val = true }; };
template <> struct is_signed_int<int> { enum { val = true }; };
template <> struct is_signed_int<long> { enum { val = true }; };
template <> struct is_signed_int<long long> { enum { val = true }; };

template <typename T> struct is_unsigned_int { enum { val = false }; };
template <> struct is_unsigned_int<unsigned short> { enum { val = true }; };
template <> struct is_unsigned_int<unsigned int> { enum { val = true }; };
template <> struct is_unsigned_int<unsigned long> { enum { val = true }; };
template <> struct is_unsigned_int<unsigned long long> { enum { val = true }; };

template <typename T> struct is_int {
	enum { val = (is_signed_int<T>::val || is_unsigned_int<T>::val) };
};


// Sets default values
AVrCharacter::AVrCharacter()
{
 	// Set this character to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = true;

	//Create our camera and associated components
	// Need to create the spring arm, otherwise camera does not rotate properly
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	StaticMeshComp = CreateDefaultSubobject <UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));

	StaticMeshComp->SetupAttachment(RootComponent);
	SpringArmComp->SetupAttachment(StaticMeshComp);
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);

	//Setting Default Variables and Behavior of the SpringArmComponent
	SpringArmComp->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));
	SpringArmComp->TargetArmLength = 0.f;
	SpringArmComp->bEnableCameraLag = true;
	SpringArmComp->CameraLagSpeed = 3.0f;

	// Should give control over the rotation more to the character themselves
	// However this doesn't work completely
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// VideoWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("widget component"));

}

// Called when the game starts or when spawned
void AVrCharacter::BeginPlay()
{
	Super::BeginPlay();

	check(GEngine != nullptr);

	// Display a debug message for five seconds. 
	// The -1 "Key" value argument prevents the message from being updated or refreshed.
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("We are using FPSCharacter."));

	// Determine the appropriate IMC mapping
	LoadOne = GetWorld()->GetGameState<ADynamicGameState>()->LoadOne;

	// We also need to read the file containing the odometry data, which needs to be provided in six DoF format
	// x, y, z, a, b, c
	// Needs to be the same coordination scheme as the original data
	std::ifstream source;                    // build a read-Stream

	//source.open("C:\\Users\\admin\\Desktop\\Input Data\\odom.txt");  // open data
	// source.open("C:\\Users\\its\\Documents\\Unreal Projects\\
	// \\Data\\Odometry.csv");
	LoadOne = GetWorld()->GetGameState<ADynamicGameState>()->LoadOne;

	if (!LoadOne)
	{
		source.open(GetWorld()->GetGameState<ADynamicGameState>()->FullOdometryPath);

		UE_LOG(LogTemp, Warning, TEXT(" %f "), 12345678.f);

		int col;
		double time, x, y, z, a, b, c;

		std::stringstream ss;
		ss << source.rdbuf();

		// This assumes a particular csv file format
		while (csv_istream(ss)
			>> col >> time >> x >> y >> z >> a >> b >> c) {
			//...do something with the record...

			TArray<double> TempOdometry;
			TempOdometry.Add(x * 100);
			TempOdometry.Add(y * 100);
			TempOdometry.Add(z * 100);
			TempOdometry.Add(a);
			TempOdometry.Add(b);
			TempOdometry.Add(c);
			//TempOdometry.Add(d);
			TempOdometry.Add(time / 1000000000);

			// UE_LOG(LogTemp, Warning, TEXT("[IMPORTING] YAW %lf PITCH %lf ROLL %lf"), a, b, c);

			Odometry.Add(TempOdometry);
			TimeStampOdometry.Add(time / 1000000000);

			UE_LOG(LogTemp, Warning, TEXT(" %lf "), time);
		}

		// This is for the import of the quarternion, but is done incorrectly
		/*

		for (std::string line; std::getline(source, line, ','); )   //read stream line by line
		{
			//std::istringstream in(line);      //make a stream for the line itself
			std::stringstream ss(line);

			float time, x, y, z, a, b, c, d;
			ss >> time >> x >> y >> z >> a >> b >> c >> d;

			float readings[] = { time, x, y, z, a, b, c, d };

			TArray<float> TempOdometry;
			TempOdometry.Add(x);
			TempOdometry.Add(y);
			TempOdometry.Add(z);
			TempOdometry.Add(a);
			TempOdometry.Add(b);
			TempOdometry.Add(c);
			TempOdometry.Add(d);
			TempOdometry.Add(time / 1000);

			Odometry.Add(TempOdometry);

			UE_LOG(LogTemp, Warning, TEXT(" %f "), time);
		}*/

		UE_LOG(LogTemp, Warning, TEXT(" %f "), 12345678.f);

		ScanIndex = -1;

		// This is how much we downsample the odometry as used for the LIDAR scans
		// i.e so that there is 1:1 correspondence between LIDAR scan and odometry reading
		// We are setting the Odometry variable for the game state, not the character
		int DownSamplePer = GetWorld()->GetGameState<ADynamicGameState>()->DownSamplePer;
		GetWorld()->GetGameState<ADynamicGameState>()->Odometry = ADynamicGameState::ExtractEvery(Odometry, DownSamplePer, GetWorld()->GetGameState<ADynamicGameState>()->NumberFrames);

		//Odometry = ADynamicGameState::ExtractEvery(Odometry, 3, 299);
	}


	if (!UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("FAILED TO GET CONTROLLER"));
	}

	// TestVideo = CreateWidget<UVideoWidget>((APlayerController*) UGameplayStatics::GetPlayerController(GetWorld(), 0), VideoWidgetClass);

	/*
	TestVideo = CreateWidget<UVideoWidget>(GetWorld(), VideoWidgetClass);

	if (TestVideo)
	{
		// TestVideo->AddToPlayerScreen();
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("NO WIDGET CREATED"));

	}

	*/

	////////////////////////////////////////////////////
	// This was an attempt to extract all blueprint subclasses of a class
	// It doesn't work right now

	/*
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	// Retrieve all blueprint classes 
	TArray<FAssetData> BlueprintList;

	FARFilter Filter;
	Filter.ClassNames.Add(UBlueprint::StaticClass()->GetFName());
	Filter.ClassNames.Add(UBlueprintGeneratedClass::StaticClass()->GetFName());
	
	Module.Get().GetAssets(Filter, BlueprintList);

	for (int32 AssetIdx = 0; AssetIdx < BlueprintList.Num(); ++AssetIdx)
	{
		if (BlueprintList[AssetIdx].GetAsset() != NULL)
		{
			if (UObject* ChildObject = Cast<UObject>(BlueprintList[AssetIdx].GetAsset()))
			{
				UBlueprint* BlueprintObj = Cast<UBlueprint>(StaticLoadObject(UBlueprint::StaticClass(), NULL, *ChildObject->GetPathName()));

				if (BlueprintObj != NULL && BlueprintObj->GeneratedClass != NULL && BlueprintObj->GeneratedClass->GetDefaultObject() != NULL)
				{
					if (Cast<UVideoWidget>(BlueprintObj->GeneratedClass->GetDefaultObject()) != NULL)
					{
						ItemReferences.Add(*BlueprintObj->GeneratedClass);
					}
				}
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("There are %i subclasses of the UVideoWidget."), ItemReferences.Num());
	*/

	/////////////////////////////

	// Get all the UVideoWidget subclass objects
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), ItemReferences2, UVideoWidget::StaticClass(), false);

	UE_LOG(LogTemp, Warning, TEXT("There are %i subclasses of the UVideoWidget. [NewMethod -- FIRST]"), ItemReferences2.Num());



	/*
	TSubclassOf<AActor> ClassToFind; // Needs to be populated somehow (e.g. by exposing to blueprints as uproperty and setting it there

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ClassToFind, FoundActors);
	*/

	// Enhanced Input
	// Make sure that we have a valid PlayerController.
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{

		UE_LOG(LogTemp, Warning, TEXT("PC1"));

		// Get the Enhanced Input Local Player Subsystem from the Local Player related to our Player Controller.
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{

			UE_LOG(LogTemp, Warning, TEXT("PC2"));

			// PawnClientRestart can run more than once in an Actor's lifetime, so start by clearing out any leftover mappings.
			// Subsystem->ClearAllMappings();

			// Add each mapping context, along with their priority values. Higher values outprioritize lower values.
			// Subsystem->AddMappingContext(VRMappingContext, 5);
		}
	}



	
}

// Called every frame
void AVrCharacter::Tick(float DeltaTime)
{
	// Get the desired loading pattern (one or playback)
	LoadOne = GetWorld()->GetGameState<ADynamicGameState>()->LoadOne;

	// Find all the video widgets (created in blueprints)
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), ItemReferences2, UVideoWidget::StaticClass(), false);


	UE_LOG(LogTemp, Warning, TEXT("There are %i subclasses of the UVideoWidget. [NewMethod]"), ItemReferences2.Num());

	// This can be toggled to determine whether the camera should 
	// be always in the direction of travel or should be controlled
	// by the VR HMD
	bool VROn = true;

	// This can be toggled to determine whether HMD direction should set
	// relative or absolute orientation
	bool RelativeOrientation = true;

	Super::Tick(DeltaTime * GetWorld()->GetGameState<ADynamicGameState>()->PlaybackSpeed);

	// Local tracking of passed time
	// ClockTime += DeltaTime;

	// Get the clock time from the game state (synchronisation step)
	ClockTime = GetWorld()->GetGameState<ADynamicGameState>()->ClockTime;

	//UE_LOG(LogTemp, Warning, TEXT("%f"), ClockTime);

	UE_LOG(LogTemp, Warning, TEXT("There are %i odometry readings"), Odometry.Num());
	//UE_LOG(LogTemp, Warning, TEXT("Time to exceed before actor is moved: %lf"), TimeStampOdometry[ScanIndex]);
	//UE_LOG(LogTemp, Warning, TEXT("Should we move to next frame? %lf %lf %i"), ClockTime, TimeStampOdometry[ScanIndex], ClockTime > TimeStampOdometry[ScanIndex]);

	// Default values, change if applicable (i.e we are actually loading)
	CurrentOdometryLocation = FVector(0, 0, 0);
	FRotator rot = FRotator(0, 0, 0);


	// This is a temporary feature where we move a frame every 0.1 s.
	if (!LoadOne)
	{
		if ((ClockTime > TimeStampOdometry[ScanIndex + 1] && ScanIndex < 29900) || ImmediateReload)
		{

			UE_LOG(LogTemp, Warning, TEXT("MOVING TO NEXT FRAME..."));

			// Iterate to the next frame only if not immediate reload
			if (!ImmediateReload)
			{
				ScanIndex += 1;
			}
			else
			{
				ImmediateReload = false;
			}

			// Set the current odometry in the game state
			// GetWorld()->GetGameState<ADynamicGameState>()->CurrentOdometry = Odometry[ScanIndex];




			// The clocktime is re-set
			// ClockTime = 0;

			/*

			UE_LOG(LogTemp, Warning, TEXT("We are accessing reading number %i"), ScanIndex);

			UE_LOG(LogTemp, Warning, TEXT("Q1 %lf"), Odometry[ScanIndex][3]);
			UE_LOG(LogTemp, Warning, TEXT("Q2 %lf"), Odometry[ScanIndex][4]);
			UE_LOG(LogTemp, Warning, TEXT("Q3 %lf"), Odometry[ScanIndex][5]);
			UE_LOG(LogTemp, Warning, TEXT("Q4 %lf"), Odometry[ScanIndex][6]);

			*/


			UE_LOG(LogTemp, Warning, TEXT("The index of this scan: %i"), ScanIndex);

			// Get the rotator, which describes the orientation of the character should face
			//FRotator rot = FRotator(Odometry[ScanIndex][4], -Odometry[ScanIndex][5], Odometry[ScanIndex][3]);
			rot = FRotator(-Odometry[ScanIndex + 1][4], -Odometry[ScanIndex + 1][3], Odometry[ScanIndex + 1][5]);




			UE_LOG(LogTemp, Warning, TEXT("Setting the actor y location at: %lf"), Odometry[ScanIndex][1]);

			// The local scan is loaded with the same position and orientation as the character
			//GetWorld()->GetGameState<ADynamicGameState>()->LoadNext(FVector(Odometry[ScanIndex][0], Odometry[ScanIndex][1], Odometry[ScanIndex][2]), ScanIndex, rot);



			UE_LOG(LogTemp, Warning, TEXT("%lf %lf %lf"), rot.Yaw, rot.Pitch, rot.Roll);


			//SetActorLocationAndRotation(FVector(Odometry[ScanIndex][0], Odometry[ScanIndex][1], Odometry[ScanIndex][2]), rot);

			// If the VR is off, then we need to set the orientation of the character
			if (!VROn)
			{
				// We set it both directly and using the player controller
				// Need to do this to ensure all roll, pitch and yaw values are indeed set
				SetActorRotation(rot.GetInverse());
				UGameplayStatics::GetPlayerController(this->GetWorld(), 0)->SetControlRotation(rot.GetInverse());

			}

			// ScanIndex += 1;

		}
		else
		{
			// Perform interpolation of position in between time points
			double TimeEarly = TimeStampOdometry[ScanIndex];
			double TimeLate = TimeStampOdometry[ScanIndex + 1];
			double ProportionThrough;

			// Check whether we are before the start
			if (ClockTime > TimeEarly)
			{
				ProportionThrough = (ClockTime - TimeEarly) / (TimeLate - TimeEarly);
			}
			else
			{
				ProportionThrough = 0;
			}

			FVector LocationEarly = FVector(Odometry[ScanIndex][0], -Odometry[ScanIndex][1], Odometry[ScanIndex][2] + 190.0 + ZAdjustment);
			FVector LocationLate = FVector(Odometry[ScanIndex + 1][0], -Odometry[ScanIndex + 1][1], Odometry[ScanIndex + 1][2] + 190.0 + ZAdjustment);


			// The Character should be set at the location PLUS 190 cm so they appear above the ground
			// if (Motion == MOVING)
			{
				// Only update the character location if they are supposed to be moving
				SetActorLocation(LocationEarly + ProportionThrough * (LocationLate - LocationEarly));

				// This is for the orthogonal movements, deprecated for controllers
				AddActorLocalOffset(FVector(0, OrthogonalX, OrthogonalY));

				// This is for the summed vector offset in the world
				AddActorWorldOffset(VectorOffset);

			}
		}

		// This is for debugging purposes only
		CurrentOdometryLocation = FVector(Odometry[ScanIndex + 1][0], -Odometry[ScanIndex + 1][1], Odometry[ScanIndex + 1][2]);

		// This is the rotation as specified by the odometry file
		rot = FRotator(-Odometry[ScanIndex + 1][4], -Odometry[ScanIndex + 1][3], Odometry[ScanIndex + 1][5]);
	}
	else
	{

		// Single point cloud mode

		// Character located 190 cm above ground
		SetActorLocation(FVector(0, 0, 190.0 + ZAdjustment));
		
		// Orthogonal offset deprecated
		// AddActorLocalOffset(FVector(0, OrthogonalX, OrthogonalY));
		
		// This is for the summed vector offset in the world
		AddActorWorldOffset(VectorOffset);
	}
	

	// If the VR mode is on, the orientation should always be in the direction of the HMD
	if (VROn && ScanIndex != -1)
	{

		// Determine the correct rotation of the camera, does NOT include HMD rotation
		// Accessed by VRPawn
		PitchAdjustmentRotator = FRotator(FQuat(rot) * FQuat(FRotator(0.0, AddedRotationAngle, 0.0)) * FQuat(FRotator(PitchAdjustment, 0.0, 0.0)));

	}

}

// Called to bind functionality to input
void AVrCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UE_LOG(LogTemp, Warning, TEXT("SPIC CALLED"));


	// Make sure that we are using a UEnhancedInputComponent; if not, the project is not configured correctly.
	if (UEnhancedInputComponent* PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{

		UE_LOG(LogTemp, Warning, TEXT("EPI Setup"));

		// There are ways to bind a UInputAction* to a handler function and multiple types of ETriggerEvent that may be of interest.

		// This calls the handler function on the tick when MyInputAction starts, such as when pressing an action button.
		if (BirdAction)
		{

			UE_LOG(LogTemp, Warning, TEXT("BA Bound"));

			PlayerEnhancedInputComponent->BindAction(BirdAction, ETriggerEvent::Started, this, &AVrCharacter::Bird);
			// PlayerEnhancedInputComponent->BindAction(BirdAction, ETriggerEvent::Canceled, this, &AVrCharacter::Ground);

		}

		if (MovementAction)
		{

			UE_LOG(LogTemp, Warning, TEXT("MA Bound"));

			PlayerEnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Started, this, &AVrCharacter::Movement);

		}

		if (BackTimeAction)
		{

			UE_LOG(LogTemp, Warning, TEXT("BTA Bound"));

			PlayerEnhancedInputComponent->BindAction(BackTimeAction, ETriggerEvent::Started, this, &AVrCharacter::BackTime);

		}

		if (BackTimeAction)
		{

			UE_LOG(LogTemp, Warning, TEXT("BTA Bound"));

			PlayerEnhancedInputComponent->BindAction(ForwardTimeAction, ETriggerEvent::Started, this, &AVrCharacter::ForwardTime);

		}

		if (ToggleVideoVisibilityAction)
		{

			UE_LOG(LogTemp, Warning, TEXT("BTA Bound"));

			PlayerEnhancedInputComponent->BindAction(ToggleVideoVisibilityAction, ETriggerEvent::Started, this, &AVrCharacter::ToggleVideoVisibility);

		}

		if (OrthogonalRightAction)
		{

			UE_LOG(LogTemp, Warning, TEXT("BTA Bound"));

			PlayerEnhancedInputComponent->BindAction(OrthogonalRightAction, ETriggerEvent::Started, this, &AVrCharacter::OrthogonalRight);

		}

		if (OrthogonalLeftAction)
		{

			UE_LOG(LogTemp, Warning, TEXT("BTA Bound"));

			PlayerEnhancedInputComponent->BindAction(OrthogonalLeftAction, ETriggerEvent::Started, this, &AVrCharacter::OrthogonalLeft);

		}

		if (OrthogonalUpAction)
		{

			UE_LOG(LogTemp, Warning, TEXT("BTA Bound"));

			PlayerEnhancedInputComponent->BindAction(OrthogonalUpAction, ETriggerEvent::Started, this, &AVrCharacter::OrthogonalUp);

		}

		if (OrthogonalDownAction)
		{

			UE_LOG(LogTemp, Warning, TEXT("BTA Bound"));

			PlayerEnhancedInputComponent->BindAction(OrthogonalDownAction, ETriggerEvent::Started, this, &AVrCharacter::OrthogonalDown);

		}

		if (CounterClockwiseRotationAction)
		{

			UE_LOG(LogTemp, Warning, TEXT("BTA Bound"));

			PlayerEnhancedInputComponent->BindAction(CounterClockwiseRotationAction, ETriggerEvent::Started, this, &AVrCharacter::CounterClockwiseRotation);

		}

		if (ClockwiseRotationAction)
		{

			UE_LOG(LogTemp, Warning, TEXT("BTA Bound"));

			PlayerEnhancedInputComponent->BindAction(ClockwiseRotationAction, ETriggerEvent::Started, this, &AVrCharacter::ClockwiseRotation);

		}

		/*
		// This calls the handler function (a UFUNCTION) by name on every tick while the input conditions are met, such as when holding a movement key down.
		if (MyOtherInputAction)
		{
			PlayerEnhancedInputComponent->BindAction(MyOtherInputAction, ETriggerEvent::Triggered, this, TEXT("MyOtherInputHandlerFunction"));
		}
		*/
	}

}


void AVrCharacter::SelectView(ViewType View)
{
	CurrentView = View;

	if (View == BIRDVIEW)
	{
		ZAdjustment = GetWorld()->GetGameState<ADynamicGameState>()->HeightBirds;
		PitchAdjustment = 270.0;
	}
	else if (View == ROADVIEW)
	{
		ZAdjustment = 0.0;
		PitchAdjustment = 0.0;
	}
}

void AVrCharacter::PauseMovement()
{
	Motion = STOPPED;
}

void AVrCharacter::StartMovement()
{
	Motion = MOVING;
}

void AVrCharacter::Bird(const FInputActionValue& Value)
{

	UE_LOG(LogTemp, Warning, TEXT("BIRD HAS BEEN CALLED"));

	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue)
	{


		UE_LOG(LogTemp, Warning, TEXT("IA Triggered"));

		if (CurrentView == ROADVIEW)
		{
			SelectView(BIRDVIEW);

			// For the bird view, turn off the video
			((UVideoWidget*)ItemReferences2[0])->CurrentVisibility = ESlateVisibility::Hidden;

		}
		else if (CurrentView == BIRDVIEW)
		{
			SelectView(ROADVIEW);
		}

		// Refresh the position whenever the viewpoint changes
		ImmediateReload = true;

	}

}

void AVrCharacter::Movement(const FInputActionValue& Value)
{

	UE_LOG(LogTemp, Warning, TEXT("BIRD HAS BEEN CALLED"));

	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue)
	{


		UE_LOG(LogTemp, Warning, TEXT("IA Triggered"));

		if (Motion == MOVING)
		{
			PauseMovement();

		}
		else if (Motion == STOPPED)
		{
			StartMovement();
		}

	}

}


/*
void AVrCharacter::Ground(const FInputActionValue& Value)
{

	UE_LOG(LogTemp, Warning, TEXT("GROUND HAS BEEN CALLED"));

	const bool CurrentValue = Value.Get<bool>();
	if (!CurrentValue)
	{
		UE_LOG(LogTemp, Warning, TEXT("IA Triggered - GROUND"));
		SelectView(BIRDVIEW);
	}
	else
	{
		SelectView(ROADVIEW);
	}

}
*/

void AVrCharacter::BackTime(const FInputActionValue& Value)
{

	UE_LOG(LogTemp, Warning, TEXT("BACKTIME HAS BEEN CALLED"));

	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue)
	{
		UE_LOG(LogTemp, Warning, TEXT("IA Triggered - BACKTIME"));
		double NewTime = ClockTime - 1;
		SetTime(NewTime);
		GetWorld()->GetGameState<ADynamicGameState>()->SetTime(NewTime);
		((UVideoWidget*) ItemReferences2[0])->SetTime(NewTime);
	}

}

void AVrCharacter::ForwardTime(const FInputActionValue& Value)
{

	UE_LOG(LogTemp, Warning, TEXT("BACKTIME HAS BEEN CALLED"));

	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue)
	{
		UE_LOG(LogTemp, Warning, TEXT("IA Triggered - FORWARDTIME"));
		double NewTime = ClockTime + 1;
		SetTime(NewTime);
		GetWorld()->GetGameState<ADynamicGameState>()->SetTime(NewTime);
		((UVideoWidget*)ItemReferences2[0])->SetTime(NewTime);
	}

}

void AVrCharacter::ToggleVideoVisibility(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("VISIBILITY HAS BEEN CALLED"));

	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue)
	{
		UVideoWidget* VideoWidget = (UVideoWidget*)ItemReferences2[0];
		VideoWidget->ToggleVisibility();
	}

}

void AVrCharacter::OrthogonalRight(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue)
	{
		OrthogonalX += 100.0;
	}

	// Refresh the position whenever the location changes
	ImmediateReload = true;
	
}

void AVrCharacter::OrthogonalLeft(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue)
	{
		OrthogonalX -= 100.0;
	}

	// Refresh the position whenever the location changes
	ImmediateReload = true;
}

void AVrCharacter::OrthogonalUp(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue)
	{
		OrthogonalY += 100.0;
	}

	// Refresh the position whenever the location changes
	ImmediateReload = true;
	
}

void AVrCharacter::OrthogonalDown(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue)
	{
		OrthogonalY -= 100.0;
	}

	// Refresh the position whenever the location changes
	ImmediateReload = true;
}

void AVrCharacter::CounterClockwiseRotation(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue)
	{
		AddedRotationAngle -= 10.0;
	}

	// Refresh the position whenever the location changes
	ImmediateReload = true;
}

void AVrCharacter::ClockwiseRotation(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue)
	{
		AddedRotationAngle += 10.0;
	}

	// Refresh the position whenever the location changes
	ImmediateReload = true;
}

void AVrCharacter::ForwardOffset(const FInputActionValue& Value, FVector ForwardVector)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue)
	{
		VectorOffset += ForwardVector * 10.0;
		AddActorWorldOffset(ForwardVector * 10.0);

	}

	// Refresh the position whenever the location changes
	ImmediateReload = true;

}

void AVrCharacter::BackOffset(const FInputActionValue& Value, FVector ForwardVector)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue)
	{
		VectorOffset -= ForwardVector * 10.0;
		AddActorWorldOffset( -ForwardVector * 10.0);
	}

	// Refresh the position whenever the location changes
	ImmediateReload = true;
}

void AVrCharacter::ResetLocation(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue)
	{
		VectorOffset = FVector(0, 0, 0);
	}

	// Refresh the position whenever the location changes
	ImmediateReload = true;
}

void AVrCharacter::SetTime(double Time)
{
	
	// Find out what the current time is
	double CurrentTime = ClockTime;

	// Need to change the actual time
	// This doesn't matter that much because we are pulling
	// the time from the game state anyways
	ClockTime = Time;

	// Check whether we go back or forwards in time
	if (ClockTime < CurrentTime)
	{
		// We need to go back in time

		// Need to change the ScanIndex (which will then set the position)
		while (ClockTime < Odometry[ScanIndex][6] && ScanIndex > 0)
		{
			ScanIndex -= 1;
		}

		// Now reload
		ImmediateReload = true;
		GetWorld()->GetGameState<ADynamicGameState>()->ImmediateReload = true;
		GetWorld()->GetGameState<ADynamicGameState>()->ImmediateReloadSecond = true;
		((UVideoWidget*)ItemReferences2[0])->ImmediateReload = true;

	}
	else if (ClockTime > CurrentTime)
	{
		while (ClockTime > Odometry[ScanIndex][6] && ScanIndex < Odometry.Num() - 3)
		{
			ScanIndex += 1;
		}

		// Now reload
		ImmediateReload = true;
		GetWorld()->GetGameState<ADynamicGameState>()->ImmediateReload = true;
		GetWorld()->GetGameState<ADynamicGameState>()->ImmediateReloadSecond = true;
		((UVideoWidget*)ItemReferences2[0])->ImmediateReload = true;


	}

}