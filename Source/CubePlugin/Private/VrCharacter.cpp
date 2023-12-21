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
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the camera
	//Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	//Camera->SetupAttachment(GetMesh());

	//Create our components
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

	//CameraComp->bUsePawnControlRotation = true;
	//(Cast<UCharacterMovementComponent>(GetMovementComponent()))->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;



	//AutoPossessPlayer = EAutoReceiveInput::Player0;

	/*


	if (APlayerController* PlayerCharacterController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), "FOUND A PLAYER CONTROLLER");
		float CameraBlendTime = 1.0f;
		PlayerCharacterController->SetViewTargetWithBlend(PlayerCharacterController->GetPawn(), CameraBlendTime, EViewTargetBlendFunction::VTBlend_Linear);
	}*/

}

// Called when the game starts or when spawned
void AVrCharacter::BeginPlay()
{
	Super::BeginPlay();

	check(GEngine != nullptr);

	// Display a debug message for five seconds. 
	// The -1 "Key" value argument prevents the message from being updated or refreshed.
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("We are using FPSCharacter."));

	// We also need to read the file containing the odometry data, which needs to be provided in six DoF format
	// x, y, z, a, b, c
	// Needs to be the same coordination scheme as the original data
	std::ifstream source;                    // build a read-Stream

	//source.open("C:\\Users\\admin\\Desktop\\Input Data\\odom.txt");  // open data
	source.open("C:\\Users\\admin\\Downloads\\updateodometry.csv");


	UE_LOG(LogTemp, Warning, TEXT(" %f "), 12345678.f);

	int col;
	double time, x, y, z, a, b, c;

	std::stringstream ss;
	ss << source.rdbuf();


	while (csv_istream(ss)
		>> col >> time >> x >> y >> z >> a >> b >> c) {
		//...do something with the record...

		TArray<double> TempOdometry;
		TempOdometry.Add(x * 100);
		TempOdometry.Add(- y * 100);
		TempOdometry.Add(z * 100);
		TempOdometry.Add(a);
		TempOdometry.Add(b);
		TempOdometry.Add(-c);
		//TempOdometry.Add(d);
		TempOdometry.Add(time / 1000);

		Odometry.Add(TempOdometry);

		UE_LOG(LogTemp, Warning, TEXT(" %lf "), time);
	}

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

	ScanIndex = 0;

	


	
}

// Called every frame
void AVrCharacter::Tick(float DeltaTime)
{

	bool VROn = true;


	Super::Tick(DeltaTime);

	ClockTime += DeltaTime;

	//UE_LOG(LogTemp, Warning, TEXT("%f"), ClockTime);

	UE_LOG(LogTemp, Warning, TEXT("There are %i odometry readings"), Odometry.Num());

	if (ClockTime > 0.1 && ScanIndex < 299)
	{
		SetActorLocation(FVector(Odometry[ScanIndex][0], Odometry[ScanIndex][1], Odometry[ScanIndex][2] + 180.0));

		UE_LOG(LogTemp, Warning, TEXT("Setting the actor y location at: %lf"), Odometry[ScanIndex][1]);

		

		ClockTime = 0;

		/*

		UE_LOG(LogTemp, Warning, TEXT("We are accessing reading number %i"), ScanIndex);

		UE_LOG(LogTemp, Warning, TEXT("Q1 %lf"), Odometry[ScanIndex][3]);
		UE_LOG(LogTemp, Warning, TEXT("Q2 %lf"), Odometry[ScanIndex][4]);
		UE_LOG(LogTemp, Warning, TEXT("Q3 %lf"), Odometry[ScanIndex][5]);
		UE_LOG(LogTemp, Warning, TEXT("Q4 %lf"), Odometry[ScanIndex][6]);

		*/


		UE_LOG(LogTemp, Warning, TEXT("The index of this scan: %i"), ScanIndex);


		FRotator rot = FRotator(Odometry[ScanIndex][4], Odometry[ScanIndex][5], Odometry[ScanIndex][3]);

		GetWorld()->GetGameState<ADynamicGameState>()->LoadNext(FVector(Odometry[ScanIndex][0], Odometry[ScanIndex][1], Odometry[ScanIndex][2]), ScanIndex, rot);

		/*
		if (rot.Pitch < 0)
		{
			rot.Pitch += 360;
		}

		if (rot.Roll < 0)
		{
			rot.Roll += 360;
		}

		if (rot.Yaw < 0)
		{
			rot.Yaw += 360;
		}
		*/

		UE_LOG(LogTemp, Warning, TEXT("%lf %lf %lf"), rot.Yaw, rot.Pitch, rot.Roll);

		
		//SetActorLocationAndRotation(FVector(Odometry[ScanIndex][0], Odometry[ScanIndex][1], Odometry[ScanIndex][2]), rot);
		
		// ENABLE THE NEXT TWO LINES FOR SETTING AUTO-ROTATION
		if (!VROn)
		{
			SetActorRotation(rot);
			UGameplayStatics::GetPlayerController(this->GetWorld(), 0)->SetControlRotation(rot);
		}

		ScanIndex += 1;

	}
	
	if (ScanIndex == 299)
	{
		ScanIndex = 0;
	}

	// Headset
	

	if (VROn)
	{
		FQuat hmdRotation;
		FVector hmdLocationOffset;
		GEngine->XRSystem->GetCurrentPose(IXRTrackingSystem::HMDDeviceId, hmdRotation, hmdLocationOffset);

		SetActorRotation(hmdRotation);
		UGameplayStatics::GetPlayerController(this->GetWorld(), 0)->SetControlRotation(hmdRotation.Rotator());
	}



}

// Called to bind functionality to input
void AVrCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

