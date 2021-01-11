using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AddressableAssets;
using UnityEngine.AddressableAssets.ResourceLocators;
using UnityEngine.ResourceManagement.AsyncOperations;
using UnityEngine.ResourceManagement.ResourceLocations;

public class ResourceManager : Singleton<ResourceManager>
{
    internal static bool isInited = false;

    #region Loading Variables
    public struct LoadingInfo
    {
        public float LoadedRatio;
        public float TotalPercent;
        private List<AsyncOperationHandle<Object>> AsyncHandles;

        public void AddHandle( AsyncOperationHandle<Object> _handle )
        {
            AsyncHandles.Add( _handle );
            TotalPercent = AsyncHandles.Count;
        }

        public void Clear()
        {
            if ( ReferenceEquals( AsyncHandles, null ) )
            {
                AsyncHandles = new List<AsyncOperationHandle<Object>>();
            }

            AsyncHandles.Clear();
            TotalPercent = 0.0f;
            LoadedRatio = 1.0f;
        }

        public float GetCompletedPercent()
        {
            float percent = 0.0f;
            foreach ( AsyncOperationHandle<Object> handle in AsyncHandles )
            {
                percent += handle.PercentComplete;
            }

            return percent;
        }
    }
    private LoadingInfo loadingInfo;

    private float LoadedRatio
    {
        get { return loadingInfo.LoadedRatio; }
        set 
        {
            if ( Mathf.Approximately( loadingInfo.LoadedRatio, value ) )
            {
                return;
            }

            loadingInfo.LoadedRatio = value;
            OnChangeLoadedRatio?.Invoke( loadingInfo.LoadedRatio );

            if ( loadingInfo.LoadedRatio >= 1.0f )
            {
                loadingInfo.Clear();
                isInited = true;
            }
        }
    }
    public delegate void DelChangeLoadedRatio( float _loadedRatio );
    public event DelChangeLoadedRatio OnChangeLoadedRatio;
    #endregion
    
    private Dictionary<string/*key*/, string/*guid*/> guids = new Dictionary<string/*key*/, string/*guid*/>();
    private Dictionary<string/*guid*/, Object/*asset*/> loadedAssets = new Dictionary<string/*guid*/, Object/*asset*/>();
    private Dictionary<Object/*asset*/, string/*guid*/> assetGuids = new Dictionary<Object/*asset*/, string/*guid*/>();

    private void Update()
    {
        UpdateLoadingInfo();
    }

    public void Init()
    {
        loadingInfo.Clear();

        Addressables.InitializeAsync().Completed += ( _handle ) =>
        {
            if ( _handle.Status != AsyncOperationStatus.Succeeded )
            {
                Debug.LogError( "Failed InitializeAsync(). Status = " + _handle.Status );
                return;
            }

            InitGuidsData();

            LoadAssetsAsync<Item>( "Item" );
        };
    }

    public Type GetAsset<Type>( string _guid ) where Type : Object
    {
        if ( !loadedAssets.TryGetValue( _guid, out Object asset ) )
        {
            if ( isInited )
            {
                Debug.LogError( "Asset not found. Type = " + typeof( Type ).Name + ", Guid = " + _guid );
            }
            return null;
        }

        return asset as Type;
    }

    public Type GetAsset<Type>( AssetReference _reference ) where Type : Object
    {
        return GetAsset<Type>( _reference.AssetGUID );
    }

    public string GetAssetGuid( Object _asset )
    {
        string guid = string.Empty;
        if ( ReferenceEquals( _asset, null ) )
        {
            Debug.LogError( "Asset is null." );
            return guid;
        }

        if ( !assetGuids.TryGetValue( _asset, out guid ) )
        {
            Debug.LogError( "Guid not found. asset = " + _asset.name );
        }

        return guid;
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

    private void LoadAssetsAsync<Type>( string _label ) where Type : Object
    {
        Addressables.LoadResourceLocationsAsync( _label ).Completed += ( _locHandle ) =>
        {
            if ( _locHandle.Status != AsyncOperationStatus.Succeeded )
            {
                Debug.LogError( "Failed LoadResource. label = " + _label );
                return;
            }

            IList<IResourceLocation> locations = _locHandle.Result;
            foreach ( IResourceLocation loc in locations )
            {
                AsyncOperationHandle<Object> assetHandle = Addressables.LoadAssetAsync<Object>( loc );
                loadingInfo.AddHandle( assetHandle );

                assetHandle.Completed += ( _assetHandle ) =>
                {
                    if ( _assetHandle.Status != AsyncOperationStatus.Succeeded )
                    {
                        Debug.LogError( "Failed LoadAsset. location = " + loc.ToString() );
                        return;
                    }

                    string guid = GetAssetGuid( loc.PrimaryKey );
                    loadedAssets.Add( guid, _assetHandle.Result );
                    assetGuids.Add( _assetHandle.Result, guid );
                };
            }
        };
    }

    private void UpdateLoadingInfo()
    {
        if ( loadingInfo.TotalPercent <= 0.0f )
        {
            return;
        }

        LoadedRatio = ( loadingInfo.GetCompletedPercent() / loadingInfo.TotalPercent );
    }
}

[System.Serializable]
public class AssetReferenceItem : AssetReferenceT<Item>
{
    public AssetReferenceItem( string _guid ) : base( _guid ) { }
}

[System.Serializable]
public class AssetReferenceEquipment : AssetReferenceT<Equipment>
{
    public AssetReferenceEquipment( string _guid ) : base( _guid ) { }
}
