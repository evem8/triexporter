/****************************************************************************************
 
   Copyright (C) 2012 Autodesk, Inc.
   All rights reserved.
 
   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.
 
****************************************************************************************/

//! \file fbxhashmap.h
#ifndef _FBXSDK_CORE_BASE_HASHMAP_H_
#define _FBXSDK_CORE_BASE_HASHMAP_H_

#include <fbxsdk/fbxsdk_def.h>

#include <fbxsdk/core/base/fbxarray.h>
#include <fbxsdk/core/base/fbxmap.h>

#include <fbxsdk/fbxsdk_nsbegin.h>

template<class T> class FbxNoOpDestruct { public: static inline void DoIt(T&) {} };
template<class T> class FbxPtrDestruct  { public: static inline void DoIt(T& v) { FbxDelete(v); v = NULL; } };

// true if equal, false otherwise
template<class T> class FbxDefaultComparator{ public: static inline bool CompareIt( const T& t1, const T& t2 ) { return t1 == t2; } };

template< typename KEY, typename VALUE, typename HASH, class Destruct = FbxNoOpDestruct<VALUE>, class Comparator = FbxDefaultComparator<KEY> >
class FbxHashMap
{
public:
	typedef KEY KeyType;
	typedef VALUE ValueType;
	typedef HASH HashFunctorType;

private:

	class ListItem
	{
	public:
		ListItem* mNext;
		ValueType mValue;
		KeyType mKey;

		ListItem()
			:
		mNext(NULL)
		{
		}

        ~ListItem()
        {
            Destruct::DoIt(mValue);        
        }
	};

public:

	class Iterator
	{
	public:

		typedef ListItem ListItemType;
		typedef FbxPair< KeyType, ValueType > KeyValuePair;

		Iterator( const Iterator& pOther )
			:
			mMap( pOther.mMap ),
			mBucketIndex( pOther.mBucketIndex ),
			mCurrentItem( pOther.mCurrentItem )
		{

		}

		~Iterator(){};

		KeyValuePair operator*() const
		{
			KeyValuePair lItem;

			if( mCurrentItem )
			{
				lItem.mFirst = mCurrentItem->mKey;
				lItem.mSecond = mCurrentItem->mValue;
				return lItem;
			}

			FBX_ASSERT_NOW("Accessing out of bounds iterator");

			return lItem;
		}

		void Next()
		{
			if( !mCurrentItem )
				return;

			if( mCurrentItem->mNext )
			{
				mCurrentItem = mCurrentItem->mNext;
				return;
			}
			else
			{
				mBucketIndex++;
				for( ; mBucketIndex < mMap->mBuckets.GetCount(); ++mBucketIndex )
				{
					if( mMap->mBuckets[ mBucketIndex ] )
					{
						mCurrentItem = mMap->mBuckets[ mBucketIndex ];
						return;
					}
				}
				
				if( mBucketIndex >= mMap->mBuckets.GetCount() )
				{
					*this = mMap->End();
					return;
				}
			}
		}

		bool operator==( const Iterator& pOther ) const
		{
			return	mCurrentItem == pOther.mCurrentItem && 
					mBucketIndex == pOther.mBucketIndex &&
					mMap == pOther.mMap;
		}

		bool operator!=( const Iterator& pOther ) const
		{
			return !(*this == pOther);
		}

		
		Iterator& operator=( const Iterator& pOther )
		{
			this->mBucketIndex = pOther.mBucketIndex;
			this->mMap = pOther.mMap;
			this->mCurrentItem = pOther.mCurrentItem;
			return *this;
		}

    private:
		const FbxHashMap* mMap;		

		int mBucketIndex;
		ListItemType* mCurrentItem;
		
		Iterator(const FbxHashMap* pMap, int pBucketIndex, ListItemType* pCurrentItem)
			:
			mMap( pMap ),
			mBucketIndex(pBucketIndex),
			mCurrentItem(pCurrentItem)
		{

		}

		friend class FbxHashMap;
	};
	

	FbxHashMap( int pBucketSize )
	{
		mBuckets.Resize( pBucketSize );
	}

    FbxHashMap()
    {
        mBuckets.Resize(30);
    }

	~FbxHashMap()
	{
		Clear();
		mBuckets.Clear();
	}

	void Clear()
	{
		for( int i = 0; i < mBuckets.GetCount(); ++i)
		{
			if( mBuckets[i] )
			{
				ListItem* lNext = mBuckets[i]->mNext;
				while( lNext )
				{
					ListItem* lNextNext = lNext->mNext;
					FbxDelete(lNext);
					lNext = lNextNext;
				}

				FbxDelete(mBuckets[i]);
				mBuckets[i] = NULL;
			}
		}
	}

	const Iterator Find( const KeyType& pKey ) const
	{
		unsigned int lIndex = mHashFunctor(pKey);
		lIndex = lIndex % mBuckets.GetCount();
		ListItem* lItem = mBuckets[lIndex];
		while( lItem )
		{
            if( Comparator::CompareIt( lItem->mKey, pKey ) )
			{
				Iterator lIt( this, lIndex, lItem );
				return lIt;
			}
			lItem = lItem->mNext;
		}
		
		return End();
	}
	
	VALUE Remove( const KEY& pKey )
    {
		unsigned int lIndex = mHashFunctor(pKey);
		lIndex = lIndex % mBuckets.GetCount();
		ListItem* lItem = mBuckets.GetAt(lIndex);
        ListItem* lLastItem = NULL;
		
        while( lItem )
		{
			if( lItem->mKey == pKey )
			{
                if( lLastItem )
                    lLastItem->mNext = lItem->mNext;

                if( mBuckets.GetAt(lIndex) == lItem ) 
                    mBuckets.SetAt(lIndex, lItem->mNext );

                VALUE lValue = lItem->mValue;
                FbxDelete(lItem);
                
                return lValue;
			}

            lLastItem = lItem;
			lItem = lItem->mNext;
		}
		
        return VALUE();
    }

	ValueType& operator[]( const KeyType& pKey )
	{
		Iterator lIt = Find( pKey );
		if( lIt != End() )
		{
			return lIt.mCurrentItem->mValue;
		}

		unsigned int lIndex = mHashFunctor(pKey);
		lIndex = lIndex % mBuckets.GetCount();
		ListItem* lItem = FbxNew< ListItem >();
		lItem->mNext = NULL;
		lItem->mKey = pKey;

		if( !mBuckets.GetAt(lIndex) )
		{
			mBuckets.SetAt(lIndex, lItem);
		}
		else
		{
			lItem->mNext = mBuckets.GetAt(lIndex);
			mBuckets.SetAt(lIndex, lItem);
		}

		return lItem->mValue;
	}

	Iterator Start() const
	{
		for( int i = 0; i < mBuckets.GetCount(); ++i )
		{
			if( mBuckets[i] )
			{
				Iterator lIt( this, i, mBuckets[i] );
				return lIt;
			}
		}

		return End();
	}

	Iterator End() const
	{
		Iterator lIt( this, 0, NULL );
		return lIt;
	}

private:

	// not implemented yet!
	FbxHashMap( const FbxHashMap& pOther ) {};

	FbxArray<ListItem*> mBuckets;
	HashFunctorType mHashFunctor;

	friend class Iterator;
};

#include <fbxsdk/fbxsdk_nsend.h>

#endif /* _FBXSDK_CORE_BASE_HASHMAP_H_ */
