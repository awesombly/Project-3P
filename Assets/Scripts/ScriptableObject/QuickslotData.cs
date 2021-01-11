using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[CreateAssetMenu( fileName = "QuickslotData", menuName = "Scriptable Object/QuickslotData" )]
public class QuickslotData : ScriptableObject
{
    public List<AssetReferenceItem> initialItems = new List<AssetReferenceItem>();

    internal Dictionary<int/*index*/, Item> slotItems = new Dictionary<int/*index*/, Item>();
    internal int totalCount;

    private void Awake()
    {
        if ( !ResourceManager.isInited )
        {
            return;
        }

        totalCount = initialItems.Count;

        for ( int i = 0; i < initialItems.Count; ++i )
        {
            AssetReferenceItem equipRef = initialItems[ i ];
            if ( ReferenceEquals( equipRef, null ) )
            {
                slotItems.Add( i, null );
                continue;
            }

            Item item = ResourceManager.Instance.GetAsset<Item>( equipRef );
            slotItems.Add( i, item );
        }
    }
}
