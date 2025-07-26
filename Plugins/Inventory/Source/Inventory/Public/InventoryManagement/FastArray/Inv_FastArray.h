#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "Inv_FastArray.generated.h"

struct FGameplayTag;
class UInv_ItemComponent;
class UInv_InventoryComponent;
class UInv_InventoryItem;
class UActorComponent;
/**
 *	单个物品
 */
USTRUCT(BlueprintType)
struct FInv_InventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FInv_InventoryEntry(){}

private:
	
	friend struct FInv_InventoryFastArray;
	friend class UInv_InventoryComponent;
	
	UPROPERTY()
	TObjectPtr<UInv_InventoryItem> Item = nullptr;
};

/**
 *	库存的列表
 */
USTRUCT(BlueprintType)
struct FInv_InventoryFastArray : public FFastArraySerializer
{
	GENERATED_BODY()
	FInv_InventoryFastArray() : OwnerComponent(nullptr) {}
	FInv_InventoryFastArray(UActorComponent* IOwnerComponent) : OwnerComponent(IOwnerComponent) {}

	TArray<UInv_InventoryItem*> GetAllItems() const;
	//快速数组序列化构造
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);//移除时调用
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);//添加时调用
	//快速数组序列化构造结束

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FInv_InventoryEntry,FInv_InventoryFastArray>(Entries, DeltaParms, *this);
	}


	UInv_InventoryItem* AddEntry(UInv_ItemComponent* ItemComponent);
	UInv_InventoryItem* AddEntry(UInv_InventoryItem* Item);
	void RemoveEntry(UInv_InventoryItem* Item);
	UInv_InventoryItem* FindFirstItemByType(const FGameplayTag& ItemType);
	
private:

	friend UInv_InventoryComponent;
	
	// 复制的物品列表
	UPROPERTY()
	TArray<FInv_InventoryEntry> Entries;
	
	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;	
};

template<>
struct TStructOpsTypeTraits<FInv_InventoryFastArray> : public TStructOpsTypeTraitsBase2<FInv_InventoryFastArray>
{
	enum { WithNetDeltaSerializer = true };
};