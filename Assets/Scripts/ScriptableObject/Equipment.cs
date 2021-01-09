using System.Collections;
using System.Collections.Generic;
using UnityEngine;

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
}
