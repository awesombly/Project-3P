using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[CreateAssetMenu( fileName = "QuickslotData", menuName = "Scriptable Object/QuickslotData" )]
public class QuickslotData : ScriptableObject
{
    public List<AssetReferenceEquipment> initialItems = new List<AssetReferenceEquipment>();

    internal Dictionary<int/*index*/, Item> slotItems = new Dictionary<int/*index*/, Item>();
    internal int totalCount;

    private void Awake()
    {
        totalCount = initialItems.Count;

        for ( int i = 0; i < initialItems.Count; ++i )
        {
            AssetReferenceEquipment equipRef = initialItems[ i ];
            if ( ReferenceEquals( equipRef, null ) )
            {
                slotItems.Add( i, null );
                continue;
            }

            Equipment equip = ResourceManager.Instance.GetAsset<Equipment>( equipRef );
            slotItems.Add( i, equip );
        }
    }
}
