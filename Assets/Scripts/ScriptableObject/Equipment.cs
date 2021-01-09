using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AddressableAssets;
using UnityEngine.ResourceManagement.AsyncOperations;
using UnityEngine.ResourceManagement.ResourceLocations;

public class Item : ScriptableObject
{
    public string id;
    public string desc;
    public int price;
    internal enum EItemType
    {
        None,
        Equipment,
    }
    internal EItemType itemType;
}

public enum EEquipType
{
    Weapon,
}

[CreateAssetMenu( fileName = "Equipment", menuName = "Scriptable Object/Equipment" )]
public class Equipment : Item
{
    public EEquipType equipType;
    public Sprite icon;

    [System.Serializable]
    public struct ModelInfo
    {
        public EBoneType AttachBone;
        public GameObject Prefab;
    }
    public List<ModelInfo> modelInfos;

    private void OnEnable()
    {
        itemType = EItemType.Equipment;
    }

    private static Dictionary<string/*key*/, Equipment> loadedEquipments = new Dictionary<string/*key*/, Equipment>();

    public static Equipment GetEquipment( string key )
    {
        if ( !loadedEquipments.ContainsKey( key ) )
        {
            Debug.LogError( "equipment not found. key = " + key );
            return null;
        }

        return loadedEquipments[ key ];
    }

    #region Addressable
    public class Reference : AssetReferenceT<Equipment>
    {
        public Reference( string guid ) : base( guid ) { }
    }

    public static void LoadAddressables()
    {
        Addressables.LoadResourceLocationsAsync( "Equipment" ).Completed += ( locHandle ) =>
        {
            if ( locHandle.Status != AsyncOperationStatus.Succeeded )
            {
                Debug.LogError( "Failed LoadResource. label = Equipment" );
                return;
            }

            IList<IResourceLocation> locations = locHandle.Result;
            foreach ( IResourceLocation loc in locations )
            {
                Addressables.LoadAssetAsync<Equipment>( loc ).Completed += ( equipHandle ) =>
                {
                    if ( equipHandle.Status != AsyncOperationStatus.Succeeded )
                    {
                        Debug.LogError( "Failed LoadAsset. location = " + loc.ToString() );
                        return;
                    }

                    loadedEquipments.Add( loc.PrimaryKey, equipHandle.Result );
                };
            }
        };
    }
    #endregion
}
