// IPickupable.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IPickupable.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	None = 0,
	Weapon = 1,
	Consumable = 2,
	Quest = 3,
	Collectible = 4,
	Equipment = 5
};

UINTERFACE(MinimalAPI, BlueprintType)
class UPickupable : public UInterface
{
	GENERATED_BODY()
};

class LOSTPLACE_API IPickupable
{
	GENERATED_BODY()

public:
	// 获取拾取优先级
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup")
	int32 GetPickupPriority() const;
	virtual int32 GetPickupPriority_Implementation() const { return 1; }
    
	// 获取物品价值
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup")
	int32 GetItemValue() const;
	virtual int32 GetItemValue_Implementation() const { return 1; }
    
	// 获取物品类型
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup")
	EItemType GetItemType() const;
	virtual EItemType GetItemType_Implementation() const { return EItemType::None; }
    
	// 是否可以被拾取
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup")
	bool CanBePickedUp() const;
	virtual bool CanBePickedUp_Implementation() const { return true; }
    
	// 执行拾取
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup")
	void OnPickedUp(AActor* Picker);
	virtual void OnPickedUp_Implementation(AActor* Picker) {}
    
	// 高亮显示
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup")
	void SetHighlighted(bool bHighlight);
	virtual void SetHighlighted_Implementation(bool bHighlight) {}
};
