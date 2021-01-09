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

    private static Dictionary<string/*Guid*/ , Equipment> loadedEquipments = new Dictionary<string/*Guid*/, Equipment>();

    public static Equipment GetEquipment( string _guid )
    {
        if ( !loadedEquipments.TryGetValue( _guid, out Equipment equip ) )
        {
            Debug.LogError( "Equipment not found. Guid = " + _guid );
            return null;
        }

        return equip;
    }

    public static Equipment GetEquipment( Reference _reference )
    {
        return GetEquipment( _reference.AssetGUID );
    }

    #region Addressable
    [System.Serializable]
    public class Reference : AssetReferenceT<Equipment>
    {
        public Reference( string _guid ) : base( _guid ) { }
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

                    string guid = ResourceManager.Instance.GetAssetGuid( loc.PrimaryKey );
                    loadedEquipments.Add( guid, equipHandle.Result );
                };
            }
        };
    }
    #endregion
}
