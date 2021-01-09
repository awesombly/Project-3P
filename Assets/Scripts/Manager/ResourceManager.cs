using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AddressableAssets;
using UnityEngine.AddressableAssets.ResourceLocators;
using UnityEngine.ResourceManagement.AsyncOperations;
using UnityEngine.ResourceManagement.ResourceLocations;

public class ResourceManager : Singleton<ResourceManager>
{
    private Dictionary<string/*key*/, string/*guid*/> guids = new Dictionary<string/*key*/, string/*guid*/>();
    private Dictionary<string/*guid*/, Object/*asset*/> loadedAssets = new Dictionary<string/*guid*/, Object/*asset*/>();

    public void Init()
    {
        Addressables.InitializeAsync().Completed += ( locHandle ) =>
        {
            if ( locHandle.Status != AsyncOperationStatus.Succeeded )
            {
                Debug.LogError( "Failed InitializeAsync(). Status = " + locHandle.Status );
                return;
            }

            InitGuidsData();

            /// TODO : Label 스크립터블로 멤버 추가
            LoadAddressables<Equipment>( "Equipment" );
        };
    }

    public Type GetAsset<Type>( string _guid ) where Type : Object
    {
        if ( !loadedAssets.TryGetValue( _guid, out Object asset ) )
        {
            Debug.LogError( "Asset not found. Type = " + typeof( Type ).Name + ", Guid = " + _guid );
            return null;
        }

        return asset as Type;
    }

    public Type GetAsset<Type>( AssetReference _reference ) where Type : Object
    {
        return GetAsset<Type>( _reference.AssetGUID );
    }

    private string GetAssetGuid( string _addressableKey )
    {
        if ( !guids.TryGetValue( _addressableKey, out string guid ) )
        {
            Debug.LogError( "Guid not found. key = " + _addressableKey );
        }

        return guid;
    }

    private void InitGuidsData()
    {
        foreach ( IResourceLocator locator in Addressables.ResourceLocators )
        {
            foreach ( object objKey in locator.Keys )
            {
                string key = objKey as string;
                if ( key == null )
                {
                    continue;
                }

                if ( !System.Guid.TryParse( key, out System.Guid guid ) )
                {
                    continue;
                }

                IList<IResourceLocation> locationsFromKey;
                if ( !locator.Locate( key, typeof( System.Object ), out locationsFromKey ) )
                {
                    continue;
                }

                IResourceLocation location = locationsFromKey[ 0 ];
                guids.Add( location.PrimaryKey, key );
            }
        }
    }

    private void LoadAddressables<Type>( string _label ) where Type : Object
    {
        Addressables.LoadResourceLocationsAsync( _label ).Completed += ( locHandle ) =>
        {
            if ( locHandle.Status != AsyncOperationStatus.Succeeded )
            {
                Debug.LogError( "Failed LoadResource. label = " + _label );
                return;
            }

            IList<IResourceLocation> locations = locHandle.Result;
            foreach ( IResourceLocation loc in locations )
            {
                Addressables.LoadAssetAsync<Type>( loc ).Completed += ( assetHandle ) =>
                {
                    if ( assetHandle.Status != AsyncOperationStatus.Succeeded )
                    {
                        Debug.LogError( "Failed LoadAsset. location = " + loc.ToString() );
                        return;
                    }

                    string guid = GetAssetGuid( loc.PrimaryKey );
                    /// TODO : Type별로 컨테이너 분류
                    loadedAssets.Add( guid, assetHandle.Result );
                };
            }
        };
    }
}

[System.Serializable]
public class AssetReferenceEquipment : AssetReferenceT<Equipment>
{
    public AssetReferenceEquipment( string _guid ) : base( _guid ) { }
}
