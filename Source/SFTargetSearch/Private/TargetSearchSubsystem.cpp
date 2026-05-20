// Copyright Strayfarer & Contributors. Released under the MIT license.


#include "TargetSearchSubsystem.h"

#include "Conditional.h"
#include "TargetQueryDataAsset.h"
#include "TargetSearchLog.h"
#include "TargetSearchSettings.h"

SF::UTargetSearchSubsystem* SF::UTargetSearchSubsystem::Get(const UObject& WorldContext)
{
	checkf(WorldContext.GetWorld(), TEXT("%hs expects world context to have valid world!"), __FUNCTION__)
	return WorldContext.GetWorld()->GetSubsystem<UTargetSearchSubsystem>();
}

bool SF::UTargetSearchSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer) 
		&& Outer->HasAnyFlags(RF_WasLoaded) // do not create subsystem for the dummy world that gets initialized on engine startup
		&& GetDefault<UTargetSearchSettings>()->ShouldSystemStartInWorld(*Outer->GetWorld());
}

SF::FTargetQueryResult SF::UTargetSearchSubsystem::RunQuery(UObject* Instigator, UTargetQueryDataAsset* Query)
{
	if (!Query)
	{
		UE_LOG(LogTargetSearch, Error, TEXT("%hs called with invalid query!"), __FUNCTION__)
		return FTargetQueryResult();
	}
	if (!Query->GetSearchCondition())
	{
		UE_LOG(LogTargetSearch, Error, TEXT("%hs called with query with invalid search condition!"), __FUNCTION__)
		return FTargetQueryResult();
	}
	
	LastInstigator = Instigator;
	LastQuery = Query;
	
	// prepare relevant candidates, early return if there are none
	
	TArray<UObject*> RelevantCandidates;
	if (!TryRetrieveRelevantTargets(Instigator, Query, OUT RelevantCandidates))
	{
		UE_LOG(LogTargetSearch, VeryVerbose, TEXT("%hs: no registered targets for category %s for query %s, returning no best candidate."), 
			__FUNCTION__, *Query->GetTargetCategory().ToString(), *Query->GetName())
		return FTargetQueryResult::MakeWithNoCandidate(Instigator, Query);
	}

	// test each relevant candidate against the search condition and find the best
	
	FTargetQueryResult Result;
	Result.Instigator = Instigator;
	Result.Query = Query;
	
	FTargetCandidateQueryResult* BestEntry = nullptr;
	float BestEntryAssessment = TNumericLimits<float>::Min();
	
	for (UObject* ObjectToTest : RelevantCandidates)
	{
		if (!ObjectToTest)
		{
			UE_LOG(LogTargetSearch, Error, TEXT("%hs: There's a registered candidate which is not valid anymore! "
				"Make sure you deregister targets correctly."), __FUNCTION__)
			continue;
		}
		
		FTargetCandidateQueryResult& ResultEntry = Result.CandidateResults.AddDefaulted_GetRef();
		ResultEntry.Assessment = Query->GetSearchCondition()->Evaluate({ ObjectToTest, Instigator, &ResultEntry.DebugTrace });
		ResultEntry.Candidate = ObjectToTest;

		if (ResultEntry.Assessment.GetBinaryAnswer())
		{
			if (!BestEntry || ResultEntry.Assessment.GetFuzzyAnswer() > BestEntryAssessment)
			{
				BestEntry = &ResultEntry;
				BestEntryAssessment = ResultEntry.Assessment.GetFuzzyAnswer();
			}
		}
	}

	if (BestEntry)
	{
		BestEntry->bIsBest = true;
		Result.BestCandidateResult = *BestEntry;
		Result.bHasBestCandidate = true;
	}

	CacheQueryResult(Result);
	return Result;
}

void SF::UTargetSearchSubsystem::RegisterTarget(UObject* PotentialTarget, const FGameplayTag& TargetCategory)
{
	if (!PotentialTarget)
	{
		UE_LOG(LogTargetSearch, Error, TEXT("%hs called with invalid PotentialTarget parameter!"), __FUNCTION__);
		return;
	}
	if (TargetCategory == TAG_TargetCategory || !TargetCategory.MatchesTag(TAG_TargetCategory))
	{
		UE_LOG(LogTargetSearch, Error, TEXT("%hs called with TargetCategory parameter not being a sub-tag of 'TargetCategory'!"), __FUNCTION__);
		return;
	}
	
	FTargetArray* TargetsForCategory = RegisteredTargetCandidates.Find(TargetCategory);
	if (!TargetsForCategory)
	{
		TargetsForCategory = &RegisteredTargetCandidates.Add(TargetCategory, {});
	}
	TargetsForCategory->Targets.AddUnique(PotentialTarget);
}

void SF::UTargetSearchSubsystem::DeregisterTarget(UObject* PotentialTarget, const FGameplayTag& TargetCategory)
{
	if (!PotentialTarget)
	{
		UE_LOG(LogTargetSearch, Error, TEXT("%hs called with invalid PotentialTarget parameter!"), __FUNCTION__);
		return;
	}
	if (TargetCategory == TAG_TargetCategory || !TargetCategory.MatchesTag(TAG_TargetCategory))
	{
		UE_LOG(LogTargetSearch, Error, TEXT("%hs called with TargetCategory parameter not being a sub-tag of 'TargetCategory'!"), __FUNCTION__);
		return;
	}
	
	if (FTargetArray* TargetsForQueryCategory = RegisteredTargetCandidates.Find(TargetCategory))
	{
		TargetsForQueryCategory->Targets.Remove(PotentialTarget);
	}
}

bool SF::UTargetSearchSubsystem::IsTargetRegistered(UObject* Object, const FGameplayTag& TargetCategory)
{
	if (!Object)
	{
		UE_LOG(LogTargetSearch, Error, TEXT("%hs called with invalid PotentialTarget parameter!"), __FUNCTION__);
		return false;
	}
	if (TargetCategory == TAG_TargetCategory || !TargetCategory.MatchesTag(TAG_TargetCategory))
	{
		UE_LOG(LogTargetSearch, Error, TEXT("%hs called with TargetCategory parameter not being a sub-tag of 'TargetCategory'!"), __FUNCTION__);
		return false;
	}
	
	if (const FTargetArray* TargetsForQueryCategory = RegisteredTargetCandidates.Find(TargetCategory))
	{
		return TargetsForQueryCategory->Targets.Contains(Object);
	}
	return false;
}

const SF::FTargetQueryResult* SF::UTargetSearchSubsystem::FindQueryResultCache(UObject* Instigator, UTargetQueryDataAsset* Query) const
{
	if (!IsValid(Instigator))
	{
		UE_LOG(LogTargetSearch, Error, TEXT("%hs expects valid instigator!"), __FUNCTION__)
		return nullptr;
	}

	const FQueryResultsCache* QueryResultCache = QueryResultByInstigatorCache.Find(Instigator);
	if (!QueryResultCache)
	{
		UE_LOG(LogTargetSearch, VeryVerbose, TEXT("%hs couldn't find query result cache for instigator %s!"), __FUNCTION__, *Instigator->GetName())
		return nullptr;
	}

	const FTargetQueryResult* QueryResult = QueryResultCache->Map.Find(Query);
	return QueryResult;
}

void SF::UTargetSearchSubsystem::CacheQueryResult(const FTargetQueryResult& Result)
{
	FQueryResultsCache* QueryResultCache = QueryResultByInstigatorCache.Find(Result.Instigator);
	if (!QueryResultCache)
	{
		QueryResultCache = &QueryResultByInstigatorCache.Add(Result.Instigator, {});
	}
	QueryResultCache->Map.Add(Result.Query, Result);
}

bool SF::UTargetSearchSubsystem::TryRetrieveRelevantTargets(UObject* Instigator, const UTargetQueryDataAsset* Query,
	TArray<UObject*>& OutCandidates)
{
	TSet<UObject*> CandidateSet{};
	for (const TTuple<FGameplayTag, FTargetArray>& CategoryCandidatePair : RegisteredTargetCandidates)
	{
		if (CategoryCandidatePair.Key.MatchesTag(Query->GetTargetCategory()))
		{
			CandidateSet.Append(CategoryCandidatePair.Value.Targets);
		}
	}
	
	if (!Query->CanAcquireInstigatorAsTarget())
	{
		CandidateSet.Remove(Instigator);
	}
	
	OutCandidates = CandidateSet.Array();
	return !OutCandidates.IsEmpty();
}
