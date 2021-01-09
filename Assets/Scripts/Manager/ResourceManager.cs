using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AddressableAssets;
using UnityEngine.AddressableAssets.ResourceLocators;
using UnityEngine.ResourceManagement.AsyncOperations;
using UnityEngine.ResourceManagement.ResourceLocations;

public class ResourceManager : Singleton<ResourceManager>
{
    private Dictionary<string/*key*/, string/*guid*/> guids = new Dictionary<string/*key*/, string/*guid*/>();

    public string GetAssetGuid( string _addressableKey )
    {
        if ( !guids.TryGetValue( _addressableKey, out string guid ) )
        {
            Debug.LogError( "Guid not found. key = " + _addressableKey );
        }

        return guid;
    }

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

            Equipment.LoadAddressables();
        };
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
}
