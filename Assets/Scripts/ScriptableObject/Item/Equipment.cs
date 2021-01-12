using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public enum EEquipType
{
    Weapon,
}

[CreateAssetMenu( fileName = "Equipment", menuName = "Scriptable Object/Equipment" )]
public class Equipment : Item
{
    public EEquipType equipType;

    [System.Serializable]
    public struct ModelInfo
    {
        public EBoneType AttachBone;
        public GameObject Prefab;
    }
    public List<ModelInfo> modelInfos;

    public override void OnUseItem( Player _player )
    {
        _player.SetEquipment( this );

        if ( _player.isLocal )
        {
            SendSyncProtocol( _player );
        }
    }

    private void OnEnable()
    {
        itemType = EItemType.Equipment;
    }
}
