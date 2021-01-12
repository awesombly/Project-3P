using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Item : ScriptableObject
{
    public string id;
    public string desc;
    public Sprite icon;
    internal enum EItemType
    {
        None,
        Equipment,
        Emotion,
    }
    internal EItemType itemType;

    public virtual void OnUseItem( Player _player )
    {
    }

    protected virtual void SendSyncProtocol( Player _player)
    {
        Protocol.Both.SyncUseItem protocol;
        protocol.Serial = _player.serial;
        protocol.Guid = ResourceManager.Instance.GetAssetGuid( this );
        Network.Instance.Send( protocol );
    }
}

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
