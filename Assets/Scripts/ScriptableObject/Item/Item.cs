using System.Collections;
using System.Collections.Generic;
using UnityEngine;


[CreateAssetMenu( fileName = "Item", menuName = "Scriptable Object/Item" )]
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

    protected virtual void SendSyncProtocol( Player _player )
    {
        Protocol.Both.SyncUseItem protocol;
        protocol.Serial = _player.serial;
        protocol.Guid = ResourceManager.Instance.GetAssetGuid( this );
        Network.Instance.Send( protocol );
    }

    private void OnEnable()
    {
        itemType = EItemType.None;
    }
}
