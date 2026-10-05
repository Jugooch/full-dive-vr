// Lab props built from engine basic shapes (no art assets needed): grabbable cube/sphere/sword,
// table, target dummy and a planar-reflection mirror. Props are WorldDynamic so the avatar's touch
// zones react to them; floors/walls (WorldStatic) never trigger haptics.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PDLabProps.generated.h"

class USceneCaptureComponent2D;
class UTextureRenderTarget2D;
class UMaterialInstanceDynamic;
class UStaticMeshComponent;

UENUM(BlueprintType)
enum class EPDPropKind : uint8
{
	Cube, Sphere, Sword, Table, TargetDummy
};

UCLASS()
class PARTIALDIVEVR_API APDProp : public AActor
{
	GENERATED_BODY()

public:
	APDProp();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Prop") EPDPropKind Kind = EPDPropKind::Cube;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Prop") FLinearColor Color = FLinearColor(0.8f, 0.3f, 0.1f);

	virtual void OnConstruction(const FTransform& Transform) override;

private:
	UStaticMeshComponent* AddShape(const TCHAR* Shape, const FVector& Location, const FVector& Scale, const FLinearColor& Tint, USceneComponent* Parent);

	UPROPERTY(VisibleAnywhere, Category = "Prop") TObjectPtr<UStaticMeshComponent> Root;
};

/**
 * Vertical 120 x 200 cm mirror facing the actor's +X.
 * Implemented as a scene capture from the player camera reflected across the glass (works with forward
 * shading/Substrate, where planar reflections don't). The image is correct for the centre eye; in VR
 * both eyes see the same reflection (no stereo depth inside the mirror).
 */
UCLASS()
class PARTIALDIVEVR_API APDMirror : public AActor
{
	GENERATED_BODY()

public:
	APDMirror();
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	/** Render-target resolution (long edge). */
	UPROPERTY(EditAnywhere, Category = "Mirror") int32 Resolution = 1280;

	UPROPERTY(VisibleAnywhere, Category = "Mirror") TObjectPtr<UStaticMeshComponent> Surface;
	UPROPERTY(VisibleAnywhere, Category = "Mirror") TObjectPtr<UStaticMeshComponent> Frame;
	UPROPERTY(VisibleAnywhere, Category = "Mirror") TObjectPtr<USceneCaptureComponent2D> Capture;

private:
	UPROPERTY() TObjectPtr<UTextureRenderTarget2D> Target;
	UPROPERTY() TObjectPtr<UMaterialInstanceDynamic> SurfaceMID;
};
